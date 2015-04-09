//
//  yl_heap.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_HEAP_H
#define YL_HEAP_H


#include <stdint.h>
#include <atomic>
#include "dlmalloc.h"


class yl_heap;
class yl_heapobj;
template < typename object_t > class yl_heapref;



/*
    Garbage-collected ylang heap.  Each heap supports a single mutator
    thread.  The garbage collector runs concurrently with the mutator
    thread (save for marking of stack roots).
    
    The root set consists of the current execution stack, closures
    referenced by yl_invoke objects, and ylang objects which are the
    script part of a yl_expose.
    
    Cothread execution stacks are marked lazily.
*/


extern __thread yl_heap* yl_heap_current;


class yl_heap
{
public:

    yl_heap();
    ~yl_heap();

    void*   malloc( size_t size );
    
    void    write_barrier( yl_heapobj* obj );


private:

    mspace _mspace;


};



/*
    When we are constructing a set of objects (e.g. when compiling a
    script), then we don't want those constructed objects to be garbage
    collected until they are reachable by the GC.
    
    Note that allocating a single object and assigning it directly to a
    GC-reachable reference is always safe.
*/

class yl_alloc_scope
{
public:

    yl_alloc_scope();
    ~yl_alloc_scope();
    
};




/*
    List of all ylang heap object types.
*/

enum yl_heapobj_kind : uint8_t
{
    // User-visible
    YLOBJ_STRING,       // string or symbol
    YLOBJ_OBJECT,       // object
    YLOBJ_NATIVE,       // object linked to native yl_expose
    YLOBJ_ARRAY,        // dynamic array
    YLOBJ_TABLE,        // hashtable
    YLOBJ_FUNCTION,     // function
    YLOBJ_COTHREAD,     // coroutine with stack
    
    // Compiled code
    YLOBJ_PROGRAM,      // code for a single function

    // Internal
    YLOBJ_VALARRAY,     // fixed-size array of yl_values
    YLOBJ_SLOT,         // node in an object's class tree
    YLOBJ_UPVAL,        // implements function closures
};



/*
    Garbage collector colour.
*/

enum yl_mark_colour : uint8_t
{
    YL_GREY,    // added to greylist but not yet marked
    YL_YELLOW,  // marked/unmarked/dead
    YL_PURPLE,  // marked/unmarked/dead
    YL_ORANGE,  // marked/unmarked/dead
};



/*
    The base class of ylang heap objects.
*/

class yl_heapobj
{
protected:

    explicit yl_heapobj( yl_heapobj_kind kind );


private:

    yl_heapobj_kind                 _kind;
    std::atomic< yl_mark_colour >   _colour;


};



/*
    Write-barriered references to heapobjs.
*/

template < typename object_t >
class yl_heapref
{
public:

    yl_heapref();
    yl_heapref( const yl_heapref& p );
    yl_heapref( object_t* p );
    yl_heapref& operator = ( const yl_heapref& p );
    yl_heapref& operator = ( object_t* p );
    
    object_t* get();
    

private:

    std::atomic< object_t* > _p;

};



/*

*/


template < typename object_t >
inline yl_heapref< object_t >::yl_heapref()
    :   _p( nullptr )
{
}

template < typename object_t >
inline yl_heapref< object_t >::yl_heapref( const yl_heapref& p )
    :   _p( p.get() )
{
}

template < typename object_t >
inline yl_heapref< object_t >::yl_heapref( object_t* p )
    :   _p( p )
{
}

template < typename object_t >
inline yl_heapref< object_t >&
                yl_heapref< object_t >::operator = ( const yl_heapref& p )
{
    return this->operator = ( p.get() );
}

template < typename object_t >
inline yl_heapref< object_t >&
                yl_heapref< object_t >::operator = ( object_t* p )
{
    object_t* obj = _p.load( std::memory_order_relaxed );
    if ( obj )
    {
        yl_heap_current->write_barrier( obj );
    }
    
    _p.store( p, std::memory_order_relaxed );
    return *this;
}

template < typename object_t >
object_t* yl_heapref< object_t >::get()
{
    return _p.load( std::memory_order_relaxed );
}





#endif
