//
// region.h
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//



#ifndef REGION_H
#define REGION_H



#include <new>
#include <unordered_set>



class region;
class region_scope;
class region_buffer;
template < class T > struct region_allocator;



extern __thread region* region_current;



class region
{
public:

    static const size_t BLOCK_SIZE  = 4096;
    static const size_t ALIGNMENT   = 8;

    region();
    ~region();

    size_t  align( size_t size );
    size_t  maxalloc();
    void*   malloc( size_t new_size );
    void*   realloc( void* p, size_t old_size, size_t new_size );
    void    free( void* p, size_t old_size );


private:

    void*   _malloc( size_t new_size );
    void*   _realloc( void* p, size_t old_size, size_t new_size );
    void    _free( void* p, size_t old_size );

    std::unordered_set< void* > allocs;
    char*   block;
    size_t  next;

};



class region_scope
{
public:

    region_scope( region* region );
    ~region_scope();

private:

    region* previous;

};



class region_buffer
{
public:

    region_buffer();
    ~region_buffer();

    void    append( char c );
    size_t  size();
    void*   tearoff();

private:

    void    _expand();

    char*   buffer;
    size_t  capacity;
    size_t  index;

};



template < typename T >
class region_allocator
{
public:

    typedef T value_type;

    T*      allocate( size_t n );
    void    deallocate( T* p, size_t n );

};



void* operator new ( size_t size, region* region );
void  operator delete ( void* p, region* region );



inline size_t region::align( size_t size )
{
    return ( size + ( ALIGNMENT - 1 ) ) & ( ALIGNMENT - 1 );
}

inline size_t region::maxalloc()
{
    return BLOCK_SIZE - next;
}

inline void* region::malloc( size_t new_size )
{
    if ( next + new_size <= BLOCK_SIZE )
    {
        void* p = block + next;
        next += new_size;
        return p;
    }
    else
    {
        return _malloc( new_size );
    }
}

inline void* region::realloc( void* p, size_t old_size, size_t new_size )
{
    if ( p == NULL )
        return malloc( new_size );

    if ( new_size == 0 )
        return free( p, old_size ), (void*)NULL;

    size_t prev = next - old_size;
    if ( (char*)p == block + prev && prev + new_size <= BLOCK_SIZE )
    {
        next = prev + new_size;
        return p;
    }
    else
    {
        return _realloc( p, old_size, new_size );
    }
}

inline void region::free( void* p, size_t old_size )
{
    if ( p == NULL )
        return;

    if ( (char*)p + old_size == block + next )
    {
        next = next - old_size;
    }
    else
    {
        _free( p, old_size );
    }
}



inline region_scope::region_scope( region* region )
    :   previous( region_current )
{
    region_current = region;
}

inline region_scope::~region_scope()
{
    region_current = previous;
}



inline void region_buffer::append( char c )
{
    if ( index >= capacity )
    {
        _expand();
    }

    buffer[ index++ ] = c;
}

inline size_t region_buffer::size()
{
    return index;
}



template < typename T >
inline T* region_allocator< T >::allocate( size_t n )
{
    return (T*)region_current->malloc( sizeof( T ) * n );
}

template < typename T >
inline void region_allocator< T >::deallocate( T* p, size_t n )
{
    region_current->free( p, sizeof( T ) * n );
}



#endif

