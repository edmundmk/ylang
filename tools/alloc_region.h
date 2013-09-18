//
// alloc_region.h
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#ifndef ALLOC_REGION_H
#define ALLOC_REGION_H


#include <stdlib.h>


class alloc_region
{
public:

    alloc_region();
    ~alloc_region();

    void* alloc( size_t size );
    void  free( void* p );
    void* realloc( void* p, size_t oldsize, size_t size );


private:

    struct block
    {
        block*  next;
        size_t  offset;
    };

    static const size_t BLOCK_SIZE;
    static block LAST_BLOCK;
    block* head;


};


void* operator new( size_t size, alloc_region& region );
void  operator delete( void* p, alloc_region& region );


#endif /* ALLOC_REGION_H */

