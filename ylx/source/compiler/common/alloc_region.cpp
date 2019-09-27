//
//  alloc_region.cpp
//
//  Created by Edmund Kapusniak on 17/09/2013.
//  Copyright © Edmund Kapusniak 2013-2019.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "alloc_region.h"
#include <stdlib.h>


alloc_region::alloc_region()
    :   _block( nullptr )
    ,   _next( BLOCK_SIZE )
{
}

alloc_region::~alloc_region()
{
    for ( void* alloc : _allocs )
    {
        ::free( alloc );
    }
}

void* alloc_region::_malloc( size_t new_size )
{
    // Fast case (allocate in current _block) handled already.

    if ( new_size <= BLOCK_SIZE )
    {
        // Allocate a new _block.
        _block = (char*)::malloc( BLOCK_SIZE );
        _next = 0;
        _allocs.insert( _block );

        // Return allocation from this _block.
        void* p = _block + _next;
        _next += new_size;
        return p;
    }
    else
    {
        // Large allocation - allocate using malloc.
        void* p = ::malloc( new_size );
        _allocs.insert( p );
        return p;
    }
}

void* alloc_region::_realloc( void* p, size_t old_size, size_t new_size )
{
    // Fast case (reallocate at end of current _block) handled already.

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
        _allocs.erase( p );
        _allocs.insert( q );
        return q;
    }
}

void alloc_region::_free( void* p, size_t old_size )
{
    // Fast case (free from end of current _block) handled already.

    if ( old_size <= BLOCK_SIZE )
    {
        // Can't deallocate.
    }
    else
    {
        // Large allocation.
        ::free( p );
        _allocs.erase( p );
    }
}

