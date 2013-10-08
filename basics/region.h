//
// region.h
//
// Created 17th September 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#ifndef REGION_H
#define REGION_H


#include <stdlib.h>


class region
{
public:

    region();
    ~region();

    void* alloc( size_t size );
    void* alloc_max( size_t min_size, size_t* out_size );
    void  free( void* p );
    void* realloc( void* p, size_t old_size, size_t new_size );


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


void* operator new( size_t size, region& region );
void  operator delete( void* p, region& region );


#endif /* REGION_H */

