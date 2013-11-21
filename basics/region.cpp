//
// region.cpp
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#include "region.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <intmath.h>


__thread region* region_current = NULL;


const size_t region::ALIGNMENT      = 8;
const size_t region::BLOCK_SIZE     = 4096;
const size_t region::HEADER_SIZE    = align( sizeof( block ), ALIGNMENT );
const size_t region::MAX_ALLOC      = BLOCK_SIZE - HEADER_SIZE;


region::block region::LAST_BLOCK = { NULL, BLOCK_SIZE };


region::region()
    :   rhead( &LAST_BLOCK )
{
}

region::~region()
{
    for ( auto i = rlarge.begin(); i != rlarge.end(); ++i )
    {
        ::free( *i );
    }

    while ( rhead != &LAST_BLOCK )
    {
        block* old_block = rhead;
        rhead = rhead->next;
        ::free( old_block );
    }
}

void* region::malloc( size_t size )
{
    size = align( size, ALIGNMENT );
    
    if ( size <= MAX_ALLOC )
    {
        if ( rhead->offset + size > BLOCK_SIZE )
        {
            block* new_block = (block*)::malloc( BLOCK_SIZE );
            new_block->next   = rhead;
            new_block->offset = HEADER_SIZE;
            rhead = new_block;
        }
        
        void* p = (char*)rhead + rhead->offset;
        rhead->offset += size;
        return p;
    }
    else
    {
        void* p = ::malloc( size );
        rlarge.insert( p );
        return p;
    }
}

void region::free( void* p, size_t size )
{
    size = align( size, ALIGNMENT );
    
    if ( size <= MAX_ALLOC )
    {
        if ( (char*)p + size == (char*)rhead + rhead->offset )
        {
            // This was the previous allocation.
            rhead->offset -= size;
        }
    }
    else
    {
        rlarge.erase( p );
        ::free( p );
    }
}

void* region::realloc( void* p, size_t old_size, size_t new_size )
{
    old_size = align( old_size, ALIGNMENT );
    new_size = align( new_size, ALIGNMENT );

    if ( (char*)p + old_size == (char*)rhead + rhead->offset
            && rhead->offset - old_size + new_size <= BLOCK_SIZE )
    {
        // This is the last allocation and there is space in the block.
        rhead->offset = rhead->offset - old_size + new_size;
        return p;
    }
    else if ( old_size <= MAX_ALLOC )
    {
        // Make a new allocation (potentially in a new block, or large).
        void* q = malloc( new_size );
        if ( p )
        {
            memcpy( q, p, old_size );
            free( p, old_size );
        }
        return q;
    }
    else
    {
        // This is a large allocation.
        void* q = ::realloc( p, new_size );
        if ( p != q )
        {
            rlarge.erase( p );
            rlarge.insert( q );
        }
        return q;
    }
}




region_buffer::region_buffer( region& region )
    :   rregion( region )
    ,   rbuffer( NULL )
    ,   roffset( 0 )
    ,   rcapacity( 0 )
{
}

region_buffer::~region_buffer()
{
    rregion.free( rbuffer, rcapacity );
}

size_t region_buffer::size()
{
    return roffset;
}

void* region_buffer::tearoff()
{
    void* p = rregion.realloc( rbuffer, rcapacity, roffset );
    rbuffer     = NULL;
    rcapacity   = 0;
    roffset     = 0;
    return p;
}


void region_buffer::grow()
{
    // Work out new capacity - try and fill up a region block.
    size_t new_capacity;
    if ( rcapacity >= region::MAX_ALLOC )
        new_capacity = ceil_pow2( rcapacity ) * 2;
    else if ( rcapacity >= region::BLOCK_SIZE - rregion.rhead->offset )
        new_capacity = region::MAX_ALLOC;
    else
        new_capacity = region::BLOCK_SIZE - rregion.rhead->offset;
    
    // Reallocate buffer with new capacity.
    rbuffer = (char*)rregion.realloc( rbuffer, rcapacity, new_capacity );
    rcapacity = new_capacity;
}



