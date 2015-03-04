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
    YL_STRING,      // string or symbol
    YL_OBJECT,      // object
    YL_NATIVE,      // object linked to native yl_expose
    YL_ARRAY,       // dynamic array
    YL_TABLE,       // hashtable
    YL_FUNCTION,    // function
    YL_COTHREAD,    // coroutine with stack
    
    // Compiled code
    YL_MODULE,      // result of compiling a .yl source file
    YL_PROGRAM,     // code for a single function

    // Internal
    YL_VALARRAY,    // fixed-size array of yl_values
    YL_SLOT,        // node in an object's class tree
    YL_UPVAL,       // implements function closures
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
