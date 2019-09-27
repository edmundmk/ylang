//
//  alloc_region.h
//
//  Created by Edmund Kapusniak on 17/09/2013.
//  Copyright © Edmund Kapusniak 2013-2019.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef ALLOC_REGION_H
#define ALLOC_REGION_H

#include <new>
#include <unordered_set>

/*
    Memory allocated from the region is freed all in one go.  Can make memory
    management much faster and more straightforward.
*/

class alloc_region
{
public:

    static const size_t BLOCK_SIZE  = 4 * 1024 * 1024;
    static const size_t ALIGNMENT   = 8;

    alloc_region();
    alloc_region( const alloc_region& ) = delete;
    alloc_region& operator = ( const alloc_region& ) = delete;
    ~alloc_region();

    size_t max_alloc();
    void* malloc( size_t new_size );
    void* realloc( void* p, size_t old_size, size_t new_size );
    void free( void* p, size_t old_size );

private:

    void* _malloc( size_t new_size );
    void* _realloc( void* p, size_t old_size, size_t new_size );
    void _free( void* p, size_t old_size );

    std::unordered_set< void* > _allocs;
    char* _block;
    size_t _next;

};

void* operator new ( size_t size, alloc_region& region );
void operator delete ( void* p, alloc_region& region );

#endif

