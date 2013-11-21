//
// region.h
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#ifndef REGION_H
#define REGION_H


#include <stdlib.h>
#include <utility>
#include <unordered_set>


class region;
class region_scope;
class region_buffer;
template < typename alloc_t > class region_allocator;



extern __thread region* region_current;



class region
{
public:

    region();
    ~region();

    void* malloc( size_t size );
    void  free( void* p, size_t size );
    void* realloc( void* p, size_t old_size, size_t new_size );


private:

    friend class region_buffer;

    struct block
    {
        block*  next;
        size_t  offset;
    };

    static const size_t ALIGNMENT;
    static const size_t BLOCK_SIZE;
    static const size_t HEADER_SIZE;
    static const size_t MAX_ALLOC;
    static block LAST_BLOCK;

    std::unordered_set< void* > rlarge;
    block* rhead;

};



class region_scope
{
public:

    region_scope( region& region );
    ~region_scope();

private:

    region* rprevious;

};



class region_buffer
{
public:

    region_buffer( region& region );
    ~region_buffer();
    
    void    append( char byte );
    size_t  size();
    void*   tearoff();


private:

    void    grow();

    region& rregion;
    char*   rbuffer;
    size_t  roffset;
    size_t  rcapacity;

};



template < typename alloc_t >
class region_allocator
{
public:

    typedef alloc_t value_type;

    alloc_t*    allocate( size_t n );
    void        deallocate( alloc_t* p, size_t n );

};



void* operator new( size_t size, region& region );
void  operator delete( void* p, region& region );






inline region_scope::region_scope( region& region )
    :   rprevious( region_current )
{
    region_current = &region;
}

inline region_scope::~region_scope()
{
    region_current = rprevious;
}



template < typename alloc_t >
inline alloc_t* region_allocator< alloc_t >::allocate( size_t n )
{
    return (alloc_t*)region_current->malloc( sizeof( alloc_t ) * n );
}

template < typename alloc_t >
inline void region_allocator< alloc_t >::deallocate( alloc_t* p, size_t n )
{
    region_current->free( p, sizeof( alloc_t ) * n );
}



inline void region_buffer::append( char byte )
{
    if ( roffset >= rcapacity )
    {
        grow();
    }
    
    rbuffer[ roffset ] = byte;
    roffset += 1;
}




inline void* operator new( size_t size, region& region )
{
    return region.malloc( size );
}

inline void operator delete( void* p, region& region )
{
}





#endif /* REGION_H */

