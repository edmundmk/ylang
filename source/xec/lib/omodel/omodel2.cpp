//
//  omodel2.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <new>
#include <algorithm>
#include <atomic>
#include <intmath.h>
#include <seglist.h>



/*
    The object model for the virtual machine.  It comprises a thread-safe
    garbage collected heap, and a non-thread-safe implementation of expand
    objects.  Expand objects can have arbitrary keys added and delegate failed
    lookups to a prototype object.
*/


// collector.

class oheap;
struct ocontext;
struct ometatype;
template < typename value_t > class owb;
template < typename value_t > struct omark;


// reftypes.

/* abstract */ class obase;
template < typename element_t > class otuple;
class oclass;
class oexpand;
class ostring;
class otable;
class oarray;


// valtypes.

template < typename key_t, typename value_t > struct okeyval;
template < typename key_t, typename value_t > class okeytable;
class ovalue;
class osymbol;





/*
    The garbage collector is a three-colour concurrent collector based on
    this paper:
            http://www.cs.bell-labs.com/who/lorenz/papers/ismm98.pdf

    Mutators are stopped only briefly in order to mark roots and update the
    mark colours.  The fourth colour (grey) indicates that the object has
    been added to a work list and is already visible to the mark thread.
*/

enum ocolour
{
    O_GREEN     = 0x00,
    O_ORANGE    = 0x01,
    O_PURPLE    = 0x02,
    O_GREY      = 0x03,
    O_COLOUR    = 0x03,
};


typedef seglist< obase* > oworklist;


class oheap
{
public:

    void marked_grey( obase* object );
    

};




/*
    Per-thread context.
*/

struct ocontext
{
    static __thread ocontext* context;

    oheap*      heap;
    ocolour     mark_colour;
    obase*      allocs;

};





/*
    GC metatype, which tells the collector how to mark reference objects.
*/

struct ometatype
{
    void (*mark)( oworklist*, obase*, ocolour );
};



/*
    omark tells us which type to use to write barrier a value type, and
    how to mark it.  Values which are not references do not require a write
    barrier.  Values containing references must have a way to mark them.
*/

template < typename value_t >
struct omark
{
    typedef value_t wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};






/*
    Base of all GC objects.  The atomic word contains both the mark colour
    and an immutable pointer to the metatype.  There is a global list of
    GC objects so the sweeper can find them all.
*/

class obase
{
public:

    bool mark( ocolour check, ocolour mark );


protected:

    obase( ometatype* metatype );
    

private:

    std::atomic< uintptr_t >    word;
    obase*                      next;

};



/*
    Write barriers for normal references.  Slots must be concurrently
    accessible by the garbage collector and by the mutator thread.  The
    write barrier checks the previous value of the slot, and if it is
    unmarked and not grey, colours it grey and communicates it to the
    marker thread.  This ensures the marker will see all objects
    accessible at the snapshot.
*/

template < typename reftype_t >
class owb< reftype_t* >
{
public:

    owb();
    owb( reftype_t* q );
    owb& operator = ( reftype_t* q );
    owb& operator = ( const owb& q );

    operator reftype_t* () const;
    reftype_t& operator * () const;
    reftype_t* operator -> () const;
    

private:

    std::atomic< reftype_t* > p;
    
};


template < typename reftype_t >
struct omark< reftype_t* >
{
    typedef owb< reftype_t* > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};




/*
    Tuples are fixed-length arrays of GC slots.
*/

template < typename element_t >
class otuple : public obase
{
public:

    typedef typename omark< element_t >::wb_type elem_type;

    static otuple* alloc( size_t size );
    
    size_t size() const;
    const elem_type& at( size_t index ) const;
    elem_type& at( size_t index );
    
    
protected:

    static ometatype metatype;
    static void mark_tuple( oworklist* work, obase* object, ocolour colour );

    otuple( ometatype* metatype, size_t size );


private:
    
    size_t  ssize;
    elem_type  slots[];
    
};





/*
    A key-value hash table using only garbage collected memory.  It is itself
    a value type as hash tables are used in various objects and there's no
    need to create more GC objects than necessary.
*/


template < typename key_t, typename value_t >
struct okeyval
{
    okeyval();

    typename omark< key_t >::wb_type    key;
    typename omark< value_t >::wb_type  value;
    okeyval*                            next;
};


template < typename key_t, typename value_t >
class okeytable
{
public:

    typedef okeyval< key_t, value_t > keyval_type;
    
    okeytable();
    
    size_t          size();
    keyval_type*    lookup( const key_t& key );
    void            insert( const key_t& key, const value_t& value );
    void            remove( const key_t& key );
    void            clear();

private:

    void            rehash( size_t new_capacity );
    keyval_type*    main_position( const key_t& key );
    keyval_type*    free_position( keyval_type* near );

    size_t                          count;
    owb< otuple< keyval_type >* >   keyvals;

};


template < typename key_t, typename value_t >
struct omark< okeyval< key_t, value_t > >
{
    typedef okeyval< key_t, value_t > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};


template < typename key_t, typename value_t >
struct omark< okeytable< key_t, value_t > >
{
    typedef okeytable< key_t, value_t > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};




/*
    An ovalue is a NaN-boxed VM value.
*/








/*

*/





/*
    obase
*/

inline obase::obase( ometatype* metatype )
    :   word( (uintptr_t)metatype | ocontext::context->mark_colour )
    ,   next( ocontext::context->allocs )
{
    ocontext::context->allocs = this;
}

inline bool obase::mark( ocolour check, ocolour mark )
{
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ocolour colour = (ocolour)( word & O_COLOUR );
    if ( colour != O_GREY && colour != check )
    {
        word = ( word & ~O_COLOUR ) | mark;
        this->word.store( word, std::memory_order_relaxed );
        return true;
    }
    return false;
}


/*
    owb< reftype_t* >
*/

template < typename reftype_t >
owb< reftype_t* >::owb()
    :   p( nullptr )
{
}

template < typename reftype_t >
owb< reftype_t* >::owb( reftype_t* q )
    :   p( q )
{
}

template < typename reftype_t >
owb< reftype_t* >& owb< reftype_t* >::operator = ( reftype_t* q )
{
    // If old value is not marked, mark grey and submit to mutator.
    reftype_t* p = this->p.load( std::memory_order_relaxed );
    if ( p && p->mark( ocontext::context->mark_colour, O_GREY ) )
    {
        ocontext::context->heap->marked_grey( p );
    }

    // Update value.
    this->p.store( q, std::memory_order_relaxed );
    return *this;
}

template < typename reftype_t >
owb< reftype_t* >& owb< reftype_t* >::operator = ( const owb& q )
{
    return this->operator = ( (reftype_t*)q );
}

template < typename reftype_t >
owb< reftype_t* >::operator reftype_t* () const
{
    return p.load( std::memory_order_relaxed );
}

template < typename reftype_t >
reftype_t& owb< reftype_t* >::operator * () const
{
    return *p.load( std::memory_order_relaxed );
}

template < typename reftype_t >
reftype_t* owb< reftype_t* >::operator -> () const
{
    return p.load( std::memory_order_relaxed );
}



/*
    otuple
*/

template < typename element_t >
otuple< element_t >* otuple< element_t >::alloc( size_t size )
{
    void* p = malloc( sizeof( otuple ) + sizeof( elem_type ) * size );
    return new ( p ) otuple( &metatype, size );
}

template < typename element_t >
otuple< element_t >::otuple( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   ssize( size )
{
    for ( size_t i = 0; i < size; ++i )
    {
        new ( slots + i ) elem_type();
    }
}

template < typename element_t >
size_t otuple< element_t >::size() const
{
    return ssize;
}

template < typename element_t >
const typename otuple< element_t >::elem_type&
                otuple< element_t >::at( size_t index ) const
{
    return slots[ index ];
}

template < typename element_t >
typename otuple< element_t >::elem_type&
                otuple< element_t >::at( size_t index )
{
    return slots[ index ];
}

template < typename element_t >
ometatype otuple< element_t >::metatype = { &mark_tuple };

template < typename element_t >
void otuple< element_t >::mark_tuple(
                oworklist* work, obase* object, ocolour colour )
{
    otuple< element_t >* tuple = (otuple< element_t >*)object;
    tuple->mark( colour, colour );
    for ( size_t i = 0; i < tuple->ssize; ++i )
    {
        omark< element_t >::mark( tuple->at( i ), work, colour );
    }
}



/*
    okeytable
*/


template < typename key_t, typename value_t >
okeyval< key_t, value_t >::okeyval()
    :   key()
    ,   value()
    ,   next( nullptr )
{
}


template < typename key_t, typename value_t >
okeytable< key_t, value_t >::okeytable()
    :   count( 0 )
    ,   keyvals( nullptr )
{
}

template < typename key_t, typename value_t >
size_t okeytable< key_t, value_t >::size()
{
    return count;
}

template < typename key_t, typename value_t >
typename okeytable< key_t, value_t >::keyval_type*
                okeytable< key_t, value_t >::lookup( const key_t& key )
{
    // Lookups fail if keytable is empty.
    if ( ! keyvals )
        return nullptr;

    // Check that something is in this bucket.
    keyval_type* kv = main_position( key );
    if ( kv->next == nullptr )
        return nullptr;
    
    // Check linked list with its head at this bucket.
    keyval_type* final = &keyvals->at( 0 ) + keyvals->size();
    while ( true )
    {
        // Check if we've found it.
        if ( kv->key == key )
            return kv;
        
        // And for the end of the list.
        if ( kv->next == final )
            return nullptr;
        
        // Move to next bucket in list.
        kv = kv->next;
    }
}

template < typename key_t, typename value_t >
void okeytable< key_t, value_t >::insert(
                const key_t& key, const value_t& value )
{
    // Lookup key in case it already exists.
    keyval_type* kv = lookup( key );
    if ( kv )
    {
        kv->value = value;
        return;
    }

    // Grow hash if necessary.
    if ( ! keyvals )
        rehash( 8 );
    if ( count >= keyvals->size() * 7 / 8 )
        rehash( keyvals->size() * 2 );
    
    // Increment keyval count.
    count += 1;
    
    // Find position in which to place keyval.  Have to recalculate since
    // the earlier lookup in case we had to rehash.
    keyval_type* final = &keyvals->at( 0 ) + keyvals->size();
    kv = main_position( key );
    if ( kv->next == nullptr )
    {
        // Hash position is empty, good.  Use it for the element.
        kv->key     = key;
        kv->value   = value;
        kv->next    = final;
    }
    else
    {
        // There is already an element in the required position.  Either it
        // is a colliding element with the same bucket, or it is a 'cuckoo'
        // element displaced from another bucket.
        keyval_type* other = main_position( kv->key );
        if ( other == kv )
        {
            // It's a real collision.  Find a free space in which to place
            // the inserted element and add it to the linked list.
            keyval_type* free = free_position( kv );
            
            free->key   = key;
            free->value = value;
            free->next  = kv->next;
            kv->next    = free;
        }
        else
        {
            // The element at kv is a cuckoo element.  The element at other
            // is at the head of that element's bucket chain. Move the cuckoo
            // element to a free space.
            keyval_type* free = free_position( kv );
            
            free->key   = kv->key;
            free->value = kv->value;
            free->next  = kv->next;
            
            // And relink it into the bucket chain.
            keyval_type* prev = other;
            while ( prev->next != kv )
                prev = prev->next;
            prev->next  = free;
            
            // kv is now empty and avaliable to house our insertion.
            kv->key     = key;
            kv->value   = value;
            kv->next    = free;
        }
    }
}

template < typename key_t, typename value_t >
void okeytable< key_t, value_t >::remove( const key_t& key )
{
    // Nothing to remove if keytable is empty.
    if ( ! keyvals )
        return;
    
    // Find bucket at head of linked list.
    keyval_type* kv = main_position( key );
    if ( kv->next == nullptr )
        return;
    
    // If we match at the head of the list, we must ensure that the next
    // item in the list is moved up to occupy the hash position.
    keyval_type* final = &keyvals->at( 0 ) + keyvals->size();
    if ( kv->key == key )
    {
        if ( kv->next != final )
        {
            // Move next element in chain into this position.
            keyval_type* next = kv->next;
            kv->key     = next->key;
            kv->value   = next->value;
            kv->next    = next->next;

            // And arrange to destroy the element that was so moved.
            kv = next;
        }

        // 'Destroy' in the garbage collected word means clear.
        kv->key     = key_t();
        kv->value   = value_t();
        kv->next    = nullptr;
    }
    else
    {
        // Otherwise, search through the bucket list.
        while ( kv->next != final )
        {
            keyval_type* prev = kv;
            kv = kv->next;
            
            if ( kv->key == key )
            {
                // Unlink from bucket list.
                prev->next  = kv->next;

                // Destroy this element.
                kv->key     = key_t();
                kv->value   = value_t();
                kv->next    = nullptr;
            }
        }
    }
    
}

template < typename key_t, typename value_t >
void okeytable< key_t, value_t >::clear()
{
    count = 0;
    keyvals = nullptr;
}


template < typename key_t, typename value_t >
void okeytable< key_t, value_t >::rehash( size_t new_capacity )
{
    // Round up new capacity.
    new_capacity = std::max( new_capacity, count );
    new_capacity = ceil_pow2( new_capacity );

    // Remember old hash and allocate new one.
    otuple< keyval_type >* oldkv = keyvals;
    keyvals = otuple< keyval_type >::alloc( new_capacity );
    
    // Reinsert elements into new hash.
    count = 0;
    if ( oldkv )
    {
        for ( size_t i = 0; i < oldkv->size(); ++i )
        {
            const keyval_type& kv = oldkv->at( i );
            if ( kv.next != nullptr )
            {
                insert( kv.key, kv.value );
            }
        }
    }
}

template < typename key_t, typename value_t >
typename okeytable< key_t, value_t >::keyval_type*
                okeytable< key_t, value_t >::main_position( const key_t& key )
{
    return &keyvals->at( std::hash< key_t >()( key ) % count );
}

template < typename key_t, typename value_t >
typename okeytable< key_t, value_t >::keyval_type*
                okeytable< key_t, value_t >::free_position( keyval_type* near )
{
    keyval_type* start = &keyvals->at( 0 );
    keyval_type* final = &keyvals->at( 0 ) + keyvals->size();

    for ( keyval_type* kv = near + 1; kv < final; ++kv )
    {
        if ( kv->next == nullptr )
            return kv;
    }

    for ( keyval_type* kv = start; kv < near; ++kv )
    {
        if ( kv->next == nullptr )
            return kv;
    }

    assert( ! "no free bucket" );
    return 0;
}




template class okeytable< obase*, size_t >;
template class okeytable< obase*, obase* >;
    






