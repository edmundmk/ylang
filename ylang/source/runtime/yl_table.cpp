//
//  yl_table.cpp
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_table.h"
#include <intmath.h>
#include "yl_function.h"



yl_gctype yl_bucketlist::gctype =
{
    YLOBJ_BUCKETLIST,
    YL_GCFLAG_NONE,
    "bucketlist",
    &yl_bucketlist::destroy,
    &yl_bucketlist::mark,
    nullptr
};


yl_bucketlist::bucket::bucket()
    :   next( YL_EMPTY_BUCKET )
{
}

yl_rootref< yl_bucketlist > yl_bucketlist::alloc( size_t size )
{
    size_t sz = sizeof( yl_bucketlist ) + sizeof( bucket ) * size;
    return yl_gcalloc< yl_bucketlist >( sz, size );
}


yl_bucketlist::yl_bucketlist( size_t size )
    :   _size( size )
{
    for ( size_t i = 0; i < _size; ++i )
    {
        new ( _elements + i ) bucket();
    }
}

yl_bucketlist::~yl_bucketlist()
{
    for ( size_t i = 0; i < _size; ++i )
    {
        _elements[ i ].~bucket();
    }
}

void yl_bucketlist::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_bucketlist* self = (yl_bucketlist*)object;
    self->~yl_bucketlist();
}

void yl_bucketlist::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_bucketlist* self = (yl_bucketlist*)object;
    for ( size_t i = 0; i < self->_size; ++i )
    {
        bucket& bucket = self->_elements[ i ];
        bucket.key.mark( heap );
        bucket.value.mark( heap );
    }
}

size_t yl_bucketlist::size() const
{
    return _size;
}

const yl_bucketlist::bucket& yl_bucketlist::at( size_t index ) const
{
    assert( index < _size );
    return _elements[ index ];
}

yl_bucketlist::bucket& yl_bucketlist::at( size_t index )
{
    assert( index < _size );
    return _elements[ index ];
}




yl_gctype yl_table::gctype =
{
    YLOBJ_TABLE,
    YL_GCFLAG_NONE,
    "table",
    &yl_table::destroy,
    &yl_table::mark,
    nullptr,
};


yl_table::yl_table( size_t capacity )
    :   yl_table( yl_current->proto_table(), capacity )
{
}

yl_table::yl_table( yl_object* prototype, size_t capacity )
    :   yl_object( prototype )
    ,   _occupancy( 0 )
    ,   _buckets( yl_bucketlist::alloc( capacity ).get() )
{
}

void yl_table::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_table* self = (yl_table*)object;
    self->~yl_table();
}

void yl_table::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_object::mark( heap, object );
    yl_table* self = (yl_table*)object;
    self->_buckets.mark( heap );
}


yl_value yl_table::get_index( yl_value key ) const
{
    bucket* b = lookup( key );
    if ( b )
    {
        return b->value.get();
    }
    else
    {
        return yl_undef;
    }
}

void yl_table::set_index( yl_value key, yl_value value )
{
    // Lookup in case key already exists.
    bucket* b = lookup( key );
    if ( b )
    {
        b->value.set( value );
        return;
    }
    
    // Grow hash if necessary.
    yl_bucketlist* buckets = _buckets.get();
    size_t capacity = buckets ? buckets->size() : 0;
    if ( _occupancy >= capacity - ( capacity >> 3 ) )
    {
        size_t new_capacity = _occupancy + _occupancy / 7 + 1;
        assert( _occupancy < new_capacity - ( new_capacity >> 3 ) );
        rehash( new_capacity );
    }
    
    // Occupancy will increase.
    _occupancy += 1;
    
    // Check if element can be placed in its main position.
    bucket* insert = main_position( key );
    if ( insert->next == YL_EMPTY_BUCKET )
    {
        insert->key.set( key );
        insert->value.set( value );
        insert->next = nullptr;
        return;
    }
    
    // Bucket is not empty.  Find main position of occupying element.
    bucket* cuckoo = main_position( insert->key.get() );
    
    // The only way that the occupying element is blocking us is if its main
    // position is occupied, either by itself or another element.
    assert( cuckoo->next != YL_EMPTY_BUCKET );
    
    // Find free bucket near the main position of the occupying element.
    bucket* free = free_position( cuckoo );
    assert( free->next == YL_EMPTY_BUCKET );
    
    if ( cuckoo == insert )
    {
        // Hash collision, as both the occupying element and the new one
        // hash to the same bucket.  Place new element in the free position
        // and link it into the list.
        
        free->key.set( key );
        free->value.set( value );
        free->next = insert->next;
        insert->next = free;
    }
    else
    {
        // The occupying element (stored in insert) isn't in its main position.
        // It's part of the linked list starting at cuckoo.  Move the occupying
        // element in insert to the free bucket, and replace insert with free
        // in the linked list starting from cuckoo.
        
        free->key.set( insert->key.get() );
        free->value.set( insert->value.get() );
        
        bucket* previous = cuckoo;
        while ( previous->next != insert )
        {
            previous = previous->next;
        }
        previous->next = free;
        free->next = insert->next;
        
        // Now insert is free to hold our inserted element.
        insert->key.set( key );
        insert->value.set( value );
        insert->next = nullptr;
    }
    
}


yl_bucketlist::bucket* yl_table::lookup( yl_value key ) const
{
    // Check for value in main position.
    bucket* lookup = main_position( key );
    if ( lookup && lookup->next == YL_EMPTY_BUCKET )
    {
        return nullptr;
    }
    
    // Follow linked list of buckets from main position.
    for ( ; lookup; lookup = lookup->next )
    {
        if ( equal( lookup->key.get(), key ) )
        {
            return lookup;
        }
    }
    
    // Not found.
    return nullptr;
}


yl_bucketlist::bucket* yl_table::main_position( yl_value key ) const
{
    yl_bucketlist* buckets = _buckets.get();
    if ( buckets && buckets->size() )
    {
        return &buckets->at( hash( key ) % buckets->size() );
    }
    else
    {
        return nullptr;
    }
}

yl_bucketlist::bucket* yl_table::free_position( bucket* near )
{
    yl_bucketlist* buckets = _buckets.get();

    bucket* begin   = &buckets->at( 0 );
    bucket* end     = &buckets->at( 0 ) + buckets->size();

    for ( bucket* free = near + 1; free < end; ++free )
    {
        if ( free->next == YL_EMPTY_BUCKET )
        {
            return free;
        }
    }
    
    for ( bucket* free = near - 1; free >= begin; --free )
    {
        if ( free->next == YL_EMPTY_BUCKET )
        {
            return free;
        }
    }
    
    assert( ! "no free bucket" );
    return nullptr;
    
}

void yl_table::rehash( size_t capacity )
{
    // Grab old bucket list.
    yl_bucketlist* old_buckets = _buckets.get();
    
    // Allocate new bucket list.
    capacity = ceil_pow2( std::max( _occupancy, capacity ) );
    _occupancy = 0;
    _buckets.set( yl_bucketlist::alloc( capacity ).get() );
    
    // Reinsert elements.
    if ( old_buckets )
    {
        for ( size_t i = 0; i < old_buckets->size(); ++i )
        {
            const bucket& old_bucket = old_buckets->at( i );
            set_index( old_bucket.key.get(), old_bucket.value.get() );
        }
    }
    
    /*
        Allow the old bucket list to be garbage collected.  Note that
        old_buckets won't be GCed in this function because if the GC is
        initiated before (or at) the allocation of the new bucket list, the
        old one will be marked, and a new GC cycle cannot be initiated until
        the _next_ allocation after this function returns.
    */
}




yl_rootref< yl_object > yl_table::make_prototype()
{
    yl_rootref< yl_object > proto = yl_gcnew< yl_object >();
    proto->set_key( "length", yl_gcnew< yl_thunkobj >( &thunk_length ).get() );
    return proto;
}

yl_table* yl_table::thunk_this( yl_callframe& xf )
{
    yl_gcobject* gcobject = xf.get_gcobject( 1 );
    if ( ! gcobject || gcobject->kind() != YLOBJ_TABLE )
    {
        throw yl_exception( "expected table" );
    }
    return (yl_table*)gcobject;
}

void yl_table::thunk_length( yl_callframe& xf )
{
    yl_table* table = thunk_this( xf );
    xf.clear();
    xf.push( table->length() );
}







