//
//  gc_alloc.h
//
//  Created by Edmund Kapusniak on 21/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef GC_ALLOC_H
#define GC_ALLOC_H


/*
    This allocator supports sweeping by providing low-overhead traversal of
    every allocated block, optionally freeing them along the way.  The actual
    GC heap is built on top of the services provided by this allocator.
*/


#include <stdint.h>
#include <vector>
#include <mutex>

struct gc_header;
class gc_alloc;
class gc_sweep;



/*
    GC object header, at head of every allocation.
*/

struct gc_header
{
    std::atomic< uint8_t >  colour;
    uint8_t                 kind;
};



/*
    GC allocator based on dlmalloc.  Unlike dlmalloc, does make use of the
    system malloc to simplify bookkeeping.
*/

class gc_alloc
{
public:

    gc_alloc();
    ~gc_alloc();
    
    void*   malloc( uint8_t colour, size_t size );
    
    
private:

    friend class gc_sweep;

    static const size_t NSMALLBINS  = 32;
    static const size_t NTREEBINS   = 32;

    struct chunk;
    struct tree_chunk;
    
    struct segment
    {
        void*       base;
        size_t      size;
    };
    
    struct chunk_link
    {
        chunk_link* next;
        chunk_link* prev;
    };
    
    
    void free( void* p );
    
    
    std::mutex              _mutex;
    std::vector< segment >  _segments;
    chunk_link              _smallbins[ NSMALLBINS ];
    tree_chunk*             _treebins[ NTREEBINS ];
    chunk*                  _dv;
    size_t                  _dvsize;
    chunk*                  _top;
    
};



/*
    A sweep through all chunks in an allocator.  This is the only way to
    free chunks.
*/

class gc_sweep
{
public:

    explicit gc_sweep( gc_alloc* alloc );
    ~gc_sweep();

    void* next();
    void* free();
    
    
private:

    struct sweep_segment
    {
        gc_alloc::chunk*    chunk;
        void*               limit;
    };

    gc_alloc*                       _alloc;
    std::vector< sweep_segment >    _segments;
    size_t                          _segindex;
    gc_alloc::chunk*                _chunk;
    void*                           _limit;
    void*                           _p;

};




#endif
