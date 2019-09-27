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

    size_t align( size_t x, size_t alignment );

    void* _malloc( size_t new_size );
    void* _realloc( void* p, size_t old_size, size_t new_size );
    void _free( void* p, size_t old_size );

    std::unordered_set< void* > _allocs;
    char* _block;
    size_t _next;

};

void* operator new ( size_t size, alloc_region& alloc_region );
void operator delete ( void* p, alloc_region& alloc_region );

/*

*/

inline size_t alloc_region::max_alloc()
{
    return BLOCK_SIZE - _next;
}

inline void* alloc_region::malloc( size_t new_size )
{
    new_size = align( new_size, ALIGNMENT );

    if ( _next + new_size <= BLOCK_SIZE )
    {
        void* p = _block + _next;
        _next += new_size;
        return p;
    }
    else
    {
        return _malloc( new_size );
    }
}

inline void* alloc_region::realloc( void* p, size_t old_size, size_t new_size )
{
    old_size = align( old_size, ALIGNMENT );
    new_size = align( new_size, ALIGNMENT );

    if ( p == nullptr )
        return malloc( new_size );

    if ( new_size == 0 )
        return free( p, old_size ), (void*)nullptr;

    if ( old_size < _next
            && (char*)p == _block + _next - old_size
            && _next - old_size + new_size <= BLOCK_SIZE )
    {
        _next -= old_size;
        _next += new_size;
        return p;
    }
    else
    {
        return _realloc( p, old_size, new_size );
    }
}

inline void alloc_region::free( void* p, size_t old_size )
{
    if ( p == nullptr )
        return;

    old_size = align( old_size, ALIGNMENT );

    if ( (char*)p + old_size == _block + _next )
    {
        _next = _next - old_size;
    }
    else
    {
        _free( p, old_size );
    }
}

inline size_t alloc_region::align( size_t x, size_t alignment )
{
    return ( x + ( alignment - 1 ) ) & ~( alignment - 1 );
}

inline void* operator new ( size_t size, alloc_region& region )
{
    return region.malloc( size );
}

inline void operator delete ( void* p, alloc_region& region )
{
    // Just leave it.
}

#endif

