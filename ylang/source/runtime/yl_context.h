//
//  yl_context.h
//
//  Created by Edmund Kapusniak on 13/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_CONTEXT_H
#define YL_CONTEXT_H


#include "ylang.h"
#include "dlmalloc.h"


class yl_heapobj;
class yl_cothread;
class yl_tagval;
class yl_string;
class yl_object;



/*
    The garbage collector uses three mark colours, which cycle through
    meanings in different cycles.  In the mark phase, objects which are
    in the mark list are marked grey.
*/


enum yl_mark_colour : uint8_t
{
    YL_GREY,    // added to mark list but not yet marked
    YL_YELLOW,  // marked/unmarked/dead
    YL_PURPLE,  // marked/unmarked/dead
    YL_ORANGE,  // marked/unmarked/dead
};




/*
    A ylang context, which consists of the current runtime state of a
    ylang thread and GC heap.  Dynamic langauges are not well suited to
    multithreading, so each context is completely separate from other
    contexts.
 
    The heap supports a single mutator thread.  The garbage collector
    runs concurrently with the mutator thread (save for marking of stack
    roots).
    
    The root set consists of the current execution stack, any functions
    referenced by yl_function handles, and ylang objects which correspond
    to native objects exposed using yl_expose.
 
    Cothread execution stacks are marked lazily.
*/


extern __thread yl_context_impl* yl_current;


class yl_context_impl
{
public:

    yl_context_impl();
    ~yl_context_impl();


    // Garbage collection.

    void*           malloc( size_t size );

    yl_mark_colour  unmarked_colour();
    void            write_barrier( yl_heapobj* object );

    void            add_root( yl_heapobj* root );
    void            remove_root( yl_heapobj* root );


    // Execution context.

    yl_cothread*    get_cothread() const;
    void            set_cothread( yl_cothread* cothread );


    // Object model.

    yl_string*      symbol( yl_string* symbol );
    
    yl_object*      superof( const yl_tagval& value );

    yl_tagval       new_object( const yl_tagval& prototype );
    yl_tagval       new_array( size_t size_hint );
    yl_tagval       new_table( size_t size_hint );


    // Global table.

    yl_tagval       get_global( yl_string* key );
    void            set_global( yl_string* key, const yl_tagval& value );


private:

    yl_cothread*    _cothread;

    mspace          _heap;
    yl_mark_colour  _unmarked_colour;


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

*/


inline yl_mark_colour yl_context_impl::unmarked_colour()
{
    return _unmarked_colour;
}




#endif
