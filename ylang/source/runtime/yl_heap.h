//
//  yl_heap.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_HEAP_H
#define YL_HEAP_H


#include <stdint.h>



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
    YL_STRING,
    YL_OBJECT,
    YL_ARRAY,
    YL_TABLE,
    YL_FUNCTION,
    YL_COTHREAD,
    
    // Compiled code
    YL_MODULE,
    YL_PROGRAM,

    // Internal
    YL_VALARRAY,
    YL_SLOT,
    YL_UPVAL,
};



/*
    Garbage collector colour.
*/

enum yl_mark_colour : uint8_t
{
    YL_YELLOW,  // marked/unmarked/dead
    YL_PURPLE,  // marked/unmarked/dead
    YL_ORANGE,  // marked/unmarked/dead
    YL_GREY,    // added to greylist but not yet marked
};



/*
    The base class of ylang heap objects.
*/

class yl_heapobj
{
protected:

    explicit yl_heapobj( yl_heapobj_kind kind );


private:

    yl_heapobj_kind                 kind;
    std::atomic< yl_mark_colour >   colour;


};



/*
    Write-barriered references to heapobjs.
*/

template < typename object_t >
class yl_heapref
{
public:

    


private:

    std::atomic< object_t* >    p;

};






#endif
