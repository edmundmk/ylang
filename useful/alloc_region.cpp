//
// alloc_region.cpp
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#include "alloc_region.h"
#include <assert.h>
#include <string.h>
#include <intmath.h>


alloc_region::block alloc_region::LAST_BLOCK = { NULL, BLOCK_SIZE };


alloc_region::alloc_region()
    :   head( &LAST_BLOCK )
{
    for ( size_t i = 0; i < FREE_SIZE; ++i )
    {
        free[ i ] = NULL;
    }
}

alloc_region::~alloc_region()
{
    while ( head != &LAST_BLOCK )
    {
        block* old_block = head;
        head = head->next;
        free( old_block );
    }
}

void* alloc_region::alloc( size_t size )
{
    assert( size <= BLOCK_SIZE - sizeof( block ) );

    if ( is_pow2( size ) )
    {
        size_t log_size = log2i( size );
        if ( log_size < free_count && free[ log_size ] )
        {
            void* p = free[ log_size ];
            free[ log_size ] = free[ log_size ]->next;
            return p;
        }
    }

    if ( head->offset + size > BLOCK_SIZE )
    {
        block* new_block = (block*)malloc( BLOCK_SIZE );
        new_block->next   = head;
        new_block->offset = sizeof( block );
        head = new_block;
    }
    
    void* p = (char*)head + head->offset;
    head->offset += size;
    return p;
}

void* alloc_region::alloc_max( size_t min_size, size_t* out_size )
{
    assert( min_size <= BLOCK_SIZE - sizeof( block ) );
    
    if ( head->offset + min_size > BLOCK_SIZE )
    {
        block* new_block = (block*)malloc( BLOCK_SIZE );
        new_block->next   = head;
        new_block->offset = sizeof( block );
        head = new_block;
    }

    void* p = (char*)head + head->offset;
    *out_size = BLOCK_SIZE - head_offset;
    head->offset = BLOCK_SIZE;
    return p;
}


void alloc_region::free( void* p, size_t size )
{
    if ( is_pow2( size ) )
    {
        size_t log_size = log2i( size );
        if ( log_size < FREE_COUNT )
        {
            slot* s = (slot*)p;
            s->next = free[ log_size ];
            free[ log_size ] = s;
        }
    }
}


void* alloc_region::realloc( void* p, size_t old_size, size_t new_size )
{
    if ( p == (char*)head + head->offset - old_size
            && head->offset - old_size + new_size <= BLOCK_SIZE )
    {
        // This is the last allocation and there is space in the block.
        head->offset = head->offset - old_size + new_size;
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



void* operator new( size_t size, alloc_region& region )
{
    return region.alloc( size );
}

void  operator delete( void* p, alloc_region& region )
{
}

