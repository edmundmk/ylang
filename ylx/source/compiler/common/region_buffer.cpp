//
//  region_buffer.cpp
//
//  Created by Edmund Kapusniak on 17/09/2013.
//  Copyright © Edmund Kapusniak 2013-2019.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "region_buffer.h"

region_buffer::region_buffer( alloc_region& region )
    :   _region( &region )
    ,   _buffer( nullptr )
    ,   _capacity( 0 )
    ,   _index( 0 )
{
}

region_buffer::~region_buffer()
{
    region->free( _buffer, _capacity );
}

void region_buffer::shrink()
{
    // Shrink the allocation to be only the size of the contained data.  This
    // is useful if we are going to allocate something else from the region
    // but this buffer is not yet finished - it will force a copy on the next
    // buffer realloc, but avoids consuming the entirety of a block.
    _buffer = (char*)_region->realloc( _buffer, _capacity, _index );
    _capacity = _index;
}

void* region_buffer::tearoff()
{
    // Reallocate the buffer.
    _buffer = (char*)_region->realloc( _buffer, _capacity, _index );

    // Return the memory.
    void* p = _buffer;
    _buffer = nullptr;
    _capacity = 0;
    _index = 0;
    return p;
}

void region_buffer::_expand( size_t new_capacity )
{
    assert( new_capacity > _capacity );

    // Find a new capacity larger than the current capacity.
    if ( new_capacity <= _region->maxalloc() )
        new_capacity = _region->maxalloc();
    else if ( new_capacity <= alloc_region::BLOCK_SIZE )
        new_capacity = alloc_region::BLOCK_SIZE;
    else if ( new_capacity <= _capacity * 2 )
        new_capacity = _capacity * 2;

    // Reallocate.
    _buffer = (char*)region->realloc( _buffer, _capacity, new_capacity );
    _capacity = new_capacity;
}

