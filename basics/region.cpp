//
// region.cpp
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#include "region.h"
#include <stdlib.h>
#include <string.h>



__thread region* region_current = NULL;



region::region()
    :   block( NULL )
    ,   next( BLOCK_SIZE )
{
}

region::~region()
{
    for ( auto i = allocs.begin(); i != allocs.end(); ++i )
    {
        ::free( *i );
    }
}

void* region::_malloc( size_t new_size )
{
    // Fast case (allocate in current block) handled already.

    if ( new_size <= BLOCK_SIZE )
    {
        // Allocate a new block.
        block = (char*)::malloc( BLOCK_SIZE );
        next  = 0;
        allocs.insert( block );

        // Return allocation from this block.
        void* p = block + next;
        next += new_size;
        return p;
    }
    else
    {
        // Large allocation - allocate using malloc.
        void* p = ::malloc( new_size );
        allocs.insert( p );
        return p;
    }
}

void* region::_realloc( void* p, size_t old_size, size_t new_size )
{
    // Fast case (reallocate at end of current block) handled already.

    if ( old_size <= BLOCK_SIZE && new_size <= BLOCK_SIZE )
    {
        // Reallocate by copying.
        void* q = malloc( new_size );
        memcpy( q, p, std::min( old_size, new_size ) );
        free( p, old_size );
        return q;
    }
    else
    {
        // Large allocation - reallocate using realloc.
        void* q = ::realloc( p, new_size );
        allocs.erase( p );
        allocs.insert( q );
        return q;
    }
}

void region::_free( void* p, size_t old_size )
{
    // Fast case (free from end of current block) handled already.

    if ( old_size <= BLOCK_SIZE )
    {
        // Can't deallocate.
    }
    else
    {
        // Large allocation.
        ::free( p );
        allocs.erase( p );
    }
}



region_buffer::region_buffer()
    :   buffer( NULL )
    ,   capacity( 0 )
    ,   index( 0 )
{
}

region_buffer::~region_buffer()
{
    region_current->free( buffer, capacity );
}

void* region_buffer::tearoff()
{
    // Reallocate the buffer.
    size_t final_size = region_current->align( index );
    buffer = (char*)region_current->realloc( buffer, capacity, final_size );

    // Return the memory.
    void* p = buffer;
    buffer      = NULL;
    capacity    = 0;
    index       = 0;
    return p;
}

void region_buffer::_expand()
{
    // Find a new capacity larger than the current capacity.
    size_t new_capacity;
    if ( capacity < region_current->maxalloc() )
        new_capacity = region_current->maxalloc();
    else if ( capacity < region::BLOCK_SIZE )
        new_capacity = region::BLOCK_SIZE;
    else
        new_capacity = capacity * 2;

    // Reallocate.
    buffer = (char*)region_current->realloc( buffer, capacity, new_capacity );
    capacity = new_capacity;
}



