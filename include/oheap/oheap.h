//
//  oheap.h
//
//  Created by Edmund Kapusniak on 23/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OHEAP_H
#define OHEAP_H


/*
    This provides a garbage-collected heap.  It requires the slot layouts of
    objects to be provided, and for mutator threads to indicate safepoints
    at which garbage collection work can be performed.
    
    The heap is thread-safe (multiple mutator threads can manipulate references
    to the same heap object without locking), but reference updates are not.
    If multiple threads can access the same reference slots, such accesses
    should be protected by a mutex.
*/


#include <stddef.h>
#include <limits.h>
#include <new>
#include <atomic>
#include <seglist.h>


class oheap;
class oscope;
struct otype;

template < typename object_t > class oroot;
template < typename object_t > class oauto;
template < typename object_t > class oslot;
template < typename object_t > class oxref;
template < typename object_t > class oweak;

namespace oimpl
{
class heap;
}



/*
    The garbage collected heap itself.
*/


class oheap
{
public:

    oheap();
    oheap( const oheap& ) = delete;
    oheap& operator = ( const oheap& ) = delete;
    ~oheap();
    
private:

    friend class oscope;

    oimpl::heap* p;

};




/*
    A scope inside which GC objects can be manipulated.  No interaction with
    a garbage-collected heap (allocations, slot updates, etc.) is allowed
    outside a scope.
*/

class oscope
{
public:

    explicit oscope( oheap* heap );
    oscope( const oscope& ) = delete;
    oscope& operator = ( const oscope& ) = delete;
    ~oscope();

};




/*
    Request a collection.
*/

void ocollect();



/*
    A safepoint at which GC work may (or may not) be performed.  Note that
    other threads may be waiting on this thread reaching a safepoint.
    
    In future we may need a mechanism to indicate that threads waiting on
    blocking operations are 'safe' throughout the entirety of the blocking
    call.
*/

void osafepoint();



/*
    Slot layout information.
*/

struct otype
{
    static const ptrdiff_t END = -1;

    size_t      size;
    size_t      align       : sizeof( size_t ) * CHAR_BIT - 1;
    bool        acyclic     : 1;
    ptrdiff_t*  slots;
};


template < typename object_t >
otype* otypeof();




/*
    Allocation routines.
*/

template < typename object_t, typename ... arguments_t >
oauto< object_t > onew( arguments_t&& ... arguments );

template < typename object_t >
oauto< object_t > onew_array( size_t length );






/*
    Smart pointers used for references to garbage collected objects.
*/



namespace oimpl
{

class heap;
class slot_buffer;

enum colour
{
    GREEN   = 0x00,
    PURPLE  = 0x01,
    ORANGE  = 0x02,
    BLACK   = 0x03,
    COLOUR  = 0x03,
};

struct context
{
    colour              colour;     // current mark colour.
    heap*               heap;       // current heap.
    seglist< void** >   autos;      // pointers to oauto<>s.
    slot_buffer*        slots;      // current slot buffer.
    seglist< void* >    allocs;     // newly allocated objects.
    seglist< void* >    marked;     // marked objects.
    std::atomic_bool    handshake;  // handshake request flag.
};

extern __thread context* current;

}

#define OREF_DECLARE( oref ) \
    public: \
    \
        oref() : oref( (object_t*)nullptr ) {} \
        oref( std::nullptr_t ) : oref( (object_t*)nullptr ) {} \
        oref( const oref& p ) : oref( p.get() ) {} \
        template < typename derived_t > oref( const oroot< derived_t >& p ) : oref( p.get() ) {} \
        template < typename derived_t > oref( const oauto< derived_t >& p ) : oref( p.get() ) {} \
        template < typename derived_t > oref( const oslot< derived_t >& p ) : oref( p.get() ) {} \
        template < typename derived_t > oref( const oxref< derived_t >& p ) : oref( p.get() ) {} \
        template < typename derived_t > oref( const oweak< derived_t >& p ) : oref( p.get() ) {} \
        oref& operator = ( std::nullptr_t ) { return operator = ( (object_t*)nullptr ); } \
        oref& operator = ( const oref& p ) { return operator = ( p.get() ); } \
        template < typename derived_t > oref& operator = ( const oroot< derived_t >& p ) { return operator = ( p.get() ); } \
        template < typename derived_t > oref& operator = ( const oauto< derived_t >& p ) { return operator = ( p.get() ); } \
        template < typename derived_t > oref& operator = ( const oslot< derived_t >& p ) { return operator = ( p.get() ); } \
        template < typename derived_t > oref& operator = ( const oxref< derived_t >& p ) { return operator = ( p.get() ); } \
        template < typename derived_t > oref& operator = ( const oweak< derived_t >& p ) { return operator = ( p.get() ); } \
        ~oref(); \
    \
        explicit operator bool() const { return get() != nullptr; } \
        object_t& operator * () const { return *get(); } \
        object_t* operator -> () const { return get(); } \
        object_t& operator [] ( size_t index ) { return get()[ index ]; } \
    \
        object_t* get() const; \
    \
    private: \
    \
        explicit oref( object_t* p ); \
        oref& operator = ( object_t* p ); \



/*
    oroot<> is a global reference to a managed object.
*/

namespace oimpl
{

struct root
{
    root() : p( nullptr ), heap( nullptr ), next( nullptr ), prev( nullptr ) {}

    void*   p;
    heap*   heap;
    root*   next;
    root*   prev;
};

void root_update( root* proot, void* p );
void root_delete( root* proot );

}

template < typename object_t >
class oroot
{
    OREF_DECLARE( oroot );
    oimpl::root     r;
};


/*
    oauto<> is a stack reference to a managed object.  They must be created on
    the stack of a mutator thread.
*/

template < typename object_t >
class oauto
{
    template < typename friend_object_t, typename ... arguments_t >
    friend oauto< friend_object_t > onew( arguments_t&& ... arguments );
    template < typename friend_object_t >
    friend oauto< friend_object_t > onew_array( size_t length );
    OREF_DECLARE( oauto );
    union
    {
        object_t*   p;
        void*       v;
    };
};



/*
    oslot<> is a heap reference to a managed object.  They must only be present
    as members of managed objects, and their offsets must be described by the
    object's o_type information.
*/

namespace oimpl
{

struct slot
{
    slot( uintptr_t p ) : p( p ) {}

    std::atomic_uintptr_t p;
};

void slot_barrier( slot* pslot, void* p );
void slot_record( slot* pslot, uintptr_t oldp );

}

template < typename object_t >
class oslot
{
    OREF_DECLARE( oslot );
    oimpl::slot     s;
};


/*
    oxref<> is a special variant of oslot<> used in xob/xoi files to reference
    objects in other sections of the data image.
*/

template < typename object_t >
class oxref
{
    OREF_DECLARE( oxref );
    oimpl::slot     s;
};


/*
    oweak<> can be used in place of oslot<> to break cycles and ensure that
    object graphs are acyclic.  The collector can avoid unnecessary work if
    clients can guarantee that types will never participate in reference
    cycles.  oweak<> should _not_ appear in an o_type's slot list.
*/

template < typename object_t >
class oweak
{
    OREF_DECLARE( oweak );
    object_t*       p;
};




#undef OREF_DECLARE





/*
    The slot write barrier.
*/

inline void oimpl::slot_barrier( oimpl::slot* pslot, void* p )
{
    uintptr_t oldp = pslot->p.load( std::memory_order_relaxed );
    oimpl::colour mark_colour = oimpl::current->colour;
    oimpl::colour slot_colour = (oimpl::colour)( oldp & oimpl::COLOUR );
    assert( slot_colour != oimpl::BLACK );
    
    if ( slot_colour != mark_colour )
    {
        slot_record( pslot, oldp );
    }
    
    assert( ( (uintptr_t)p & oimpl::COLOUR ) == 0 );
    uintptr_t newp = (uintptr_t)p | mark_colour;
    pslot->p.store( newp, std::memory_order_relaxed );
}




/*
*/




namespace oimpl
{

void* alloc( otype* type, size_t length );

}


template < typename object_t, typename ... arguments_t >
oauto< object_t > onew( arguments_t&& ... arguments )
{
    void* p = oimpl::alloc( otypeof< object_t >(), 1 );
    object_t* o = new ( p ) object_t(
                    std::forward< arguments_t ... >( arguments ... ) );
    return oauto< object_t >( o );
}

template < typename object_t >
oauto< object_t > onew_array( size_t length )
{
    otype* type = otypeof< object_t >();
    void* p = oimpl::alloc( type, length );
    for ( size_t i = 0; i < length; ++i )
    {
        void* element = (char*)p + type->size * i;
        new ( element ) object_t();
    }
    return oauto< object_t >( (object_t*)p );
}




template < typename object_t >
oroot< object_t >::oroot( object_t* p )
{
    oimpl::root_update( &r, p );
}

template < typename object_t >
oroot< object_t >& oroot< object_t >::operator = ( object_t* p )
{
    oimpl::root_update( &r, p );
    return *this;
}

template < typename object_t >
oroot< object_t >::~oroot()
{
    oimpl::root_delete( &r );
}

template < typename object_t >
object_t* oroot< object_t >::get() const
{
    return (object_t*)r.p;
}



template < typename object_t >
oauto< object_t >::oauto( object_t* p )
    :   p( p )
{
    oimpl::current->autos.push_back( &v );
}

template < typename object_t >
oauto< object_t >& oauto< object_t >::operator = ( object_t* p )
{
    this->p = p;
    return *this;
}

template < typename object_t >
oauto< object_t >::~oauto()
{
    assert( oimpl::current->autos.back() == &v );
    oimpl::current->autos.pop_back();
}

template < typename object_t >
object_t* oauto< object_t >::get() const
{
    return p;
}



template < typename object_t >
oslot< object_t >::oslot( object_t* p )
    :   s( (uintptr_t)p | oimpl::current->colour )
{
}

template < typename object_t >
oslot< object_t >& oslot< object_t >::operator = ( object_t* p )
{
    oimpl::slot_barrier( &s, p );
    return *this;
}

template < typename object_t >
oslot< object_t >::~oslot()
{
}

template < typename object_t >
object_t* oslot< object_t >::get() const
{
    uintptr_t p = s.p.load( std::memory_order_relaxed );
    return (object_t*)( p & ~oimpl::COLOUR );
}



template < typename object_t >
oxref< object_t >::oxref( object_t* p )
    :   s( (uintptr_t)p | oimpl::current->colour )
{
}

template < typename object_t >
oxref< object_t >& oxref< object_t >::operator = ( object_t* p )
{
    oimpl::slot_barrier( &s, p );
    return *this;
}

template < typename object_t >
oxref< object_t >::~oxref()
{
}

template < typename object_t >
object_t* oxref< object_t >::get() const
{
    uintptr_t p = s.p.load( std::memory_order_relaxed );
    return (object_t*)( p & ~oimpl::COLOUR );
}




template < typename object_t >
oweak< object_t >::oweak( object_t* p )
    :   p( p )
{
}

template < typename object_t >
oweak< object_t >& oweak< object_t >::operator = ( object_t* p )
{
    this->p = p;
    return *this;
}

template < typename object_t >
oweak< object_t >::~oweak()
{
}

template < typename object_t >
object_t* oweak< object_t >::get() const
{
    return p;
}





#endif
