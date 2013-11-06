//
// region.cpp
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#include "region.h"
#include <assert.h>
#include <string.h>
#include <intmath.h>


region::block region::LAST_BLOCK = { NULL, BLOCK_SIZE };


region::region()
    :   rhead( &LAST_BLOCK )
{
    for ( size_t i = 0; i < FREE_COUNT; ++i )
    {
        rfree[ i ] = NULL;
    }
}

region::~region()
{
    while ( rhead != &LAST_BLOCK )
    {
        block* old_block = rhead;
        rhead = rhead->next;
        ::free( old_block );
    }
}

void* region::alloc( size_t size )
{
    assert( size <= BLOCK_SIZE - sizeof( block ) );

    if ( is_pow2( size ) )
    {
        size_t log_size = log2i( size );
        if ( log_size < FREE_COUNT && rfree[ log_size ] )
        {
            void* p = rfree[ log_size ];
            rfree[ log_size ] = rfree[ log_size ]->next;
            return p;
        }
    }

    if ( rhead->offset + size > BLOCK_SIZE )
    {
        block* new_block = (block*)malloc( BLOCK_SIZE );
        new_block->next   = rhead;
        new_block->offset = sizeof( block );
        rhead = new_block;
    }
    
    void* p = (char*)rhead + rhead->offset;
    rhead->offset += size;
    return p;
}

void* region::alloc_max( size_t min_size, size_t* out_size )
{
    assert( min_size <= BLOCK_SIZE - sizeof( block ) );
    
    if ( rhead->offset + min_size > BLOCK_SIZE )
    {
        block* new_block = (block*)malloc( BLOCK_SIZE );
        new_block->next   = rhead;
        new_block->offset = sizeof( block );
        rhead = new_block;
    }

    void* p = (char*)rhead + rhead->offset;
    *out_size = BLOCK_SIZE - rhead->offset;
    rhead->offset = BLOCK_SIZE;
    return p;
}


void region::free( void* p, size_t size )
{
    if ( is_pow2( size ) )
    {
        size_t log_size = log2i( size );
        if ( log_size < FREE_COUNT )
        {
            block* s = (block*)p;
            s->next = rfree[ log_size ];
            rfree[ log_size ] = s;
        }
    }
}


void* region::realloc( void* p, size_t old_size, size_t new_size )
{
    if ( p == (char*)rhead + rhead->offset - old_size
            && rhead->offset - old_size + new_size <= BLOCK_SIZE )
    {
        // This is the last allocation and there is space in the block.
        rhead->offset = rhead->offset - old_size + new_size;
        return p;
    }
    else
    {
        // Make a new allocation (potentially in a new block).
        void* q = alloc( new_size );
        if ( p )
        {
            memcpy( q, p, old_size );
            free( p, old_size );
        }
        return q;
    }
}



void* operator new( size_t size, region& region )
{
    return region.alloc( size );
}

void  operator delete( void* p, region& region )
{
}

