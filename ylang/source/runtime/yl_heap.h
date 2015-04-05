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



/*
    Garbage-collected ylang heap.  Each heap supports a single mutator
    thread.  The garbage collector runs concurrently with the mutator
    thread (save for marking of stack roots).
    
    The root set consists of the current execution stack, closures
    referenced by yl_invoke objects, and ylang objects which are the
    script part of a yl_expose.
    
    Cothread execution stacks are marked lazily.
*/


class yl_heap
{
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

    


private:

    std::atomic< object_t* > _p;

};






#endif
