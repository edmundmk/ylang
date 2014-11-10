//
//  ymap.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YMAP_H
#define YMAP_H


#include "yobject.h"
#include "ytuple.h"



/*
    A hash table using only garbage collected memory.  It is itself a value
    type as hash tables are used in various objects and there's no need to
    create more GC objects than necessary.
*/


template < typename key_t, typename value_t >
struct ykeyval
{
    ykeyval();

    typename ywbtraits< key_t >::wb     key;
    typename ywbtraits< value_t >::wb   value;
    ykeyval*                            next;
};


template < typename key_t, typename value_t >
class ymap
{
public:

    typedef ykeyval< key_t, value_t > keyval_type;
    
    ymap();
    
    size_t          size() const;
    keyval_type*    lookup( const key_t& key ) const;
    void            insert( const key_t& key, const value_t& value );
    void            remove( const key_t& key );
    void            clear();

    keyval_type*    iter();
    keyval_type*    next( keyval_type* prev );


private:

    friend struct ymarktraits< ymap< key_t, value_t > >;

    void            rehash( size_t new_capacity );
    keyval_type*    main_position( const key_t& key ) const;
    keyval_type*    free_position( keyval_type* near );

    size_t                          count;
    ywb< ytuple< keyval_type >* >   keyvals;

};



template < typename key_t, typename value_t >
struct ymarktraits< ykeyval< key_t, value_t > >
{
    static void mark( ykeyval< key_t, value_t >& wb, yworklist* work, ycolour colour );
};

template < typename key_t, typename value_t >
struct ymarktraits< ymap< key_t, value_t > >
{
    static void mark( ymap< key_t, value_t >& wb, yworklist* work, ycolour colour );
};




/*

*/

#include <algorithm>
#include <intmath.h>



/*
    ykeyval
*/


template < typename key_t, typename value_t >
ykeyval< key_t, value_t >::ykeyval()
    :   key()
    ,   value()
    ,   next( nullptr )
{
}


/*
    ymap
*/


template < typename key_t, typename value_t >
ymap< key_t, value_t >::ymap()
    :   count( 0 )
    ,   keyvals()
{
}

template < typename key_t, typename value_t >
inline size_t ymap< key_t, value_t >::size() const
{
    return count;
}

template < typename key_t, typename value_t >
inline typename ymap< key_t, value_t >::keyval_type*
                ymap< key_t, value_t >::lookup( const key_t& key ) const
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
void ymap< key_t, value_t >::insert(
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
void ymap< key_t, value_t >::remove( const key_t& key )
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
void ymap< key_t, value_t >::clear()
{
    count = 0;
    keyvals = nullptr;
}


template < typename key_t, typename value_t >
typename ymap< key_t, value_t >::keyval_type*
                ymap< key_t, value_t >::iter()
{
    // Empty list contains no items.
    if ( ! keyvals )
        return nullptr;

    // Find first non-empty bucket.
    for ( size_t i = 0; i < keyvals->size(); ++i )
    {
        keyval_type& kv = keyvals->at( i );
        if ( kv.next != nullptr )
        {
            return &kv;
        }
    }
    
    return nullptr;
}

template < typename key_t, typename value_t >
typename ymap< key_t, value_t >::keyval_type*
                ymap< key_t, value_t >::next( keyval_type* prev )
{
    keyval_type* final = &keyvals->at( 0 ) + keyvals->size();
    for ( keyval_type* kv = prev + 1; kv < final; ++kv )
    {
        if ( kv->next != nullptr )
        {
            return kv;
        }
    }
    
    return nullptr;
}


template < typename key_t, typename value_t >
void ymap< key_t, value_t >::rehash( size_t new_capacity )
{
    // Round up new capacity.
    new_capacity = std::max( new_capacity, count );
    new_capacity = ceil_pow2( new_capacity );

    // Remember old hash and allocate new one.
    ytuple< keyval_type >* oldkv = keyvals;
    keyvals = ytuple< keyval_type >::alloc( new_capacity );
    
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
inline typename ymap< key_t, value_t >::keyval_type*
        ymap< key_t, value_t >::main_position( const key_t& key ) const
{
    return &keyvals->at( std::hash< key_t >()( key ) % keyvals->size() );
}

template < typename key_t, typename value_t >
typename ymap< key_t, value_t >::keyval_type*
        ymap< key_t, value_t >::free_position( keyval_type* near )
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



template < typename key_t, typename value_t >
inline void ymarktraits< ykeyval< key_t, value_t > >::mark(
        ykeyval< key_t, value_t >& wb, yworklist* work, ycolour colour )
{
    ymark( wb.key, work, colour );
    ymark( wb.value, work, colour );
}

template < typename key_t, typename value_t >
inline void ymarktraits< ymap< key_t, value_t > >::mark(
        ymap< key_t, value_t >& wb, yworklist* work, ycolour colour )
{
    ymark( wb.keyvals, work, colour );
}





#endif



