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
#include <unordered_set>
#include <unordered_map>
#include <symkey.h>


class yl_heapobj;
class yl_cothread;
class yl_value;
class yl_string;
class yl_object;
class yl_array;
class yl_table;
class yl_slot;



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

    void            write_barrier( yl_heapobj* object );

    void            add_root( yl_heapobj* root );
    void            remove_root( yl_heapobj* root );


    // Execution context.

    yl_cothread*    get_cothread() const;
    void            set_cothread( yl_cothread* cothread );


    // Object model.

    yl_string*      symbol( yl_string* symbol );
    yl_object*      superof( const yl_value& value );
    yl_slot*        klassof( yl_object* prototype );

    yl_object*      proto_object();
    yl_object*      proto_array();
    yl_object*      proto_table();

    yl_value        new_object( yl_object* prototype );


    // Global table.

    yl_value        get_global( yl_string* key );
    void            set_global( yl_string* key, const yl_value& value );


private:

    void            write_barrier_mark( yl_heapobj* object );
    

    mspace          _heap;
    yl_mark_colour  _unmarked_colour;
    std::unordered_set< yl_heapobj* > _roots;

    yl_cothread*    _cothread;
    
    std::unordered_map< symkey, yl_string* > _symbols;
    yl_object*      _proto_object;
    yl_object*      _proto_array;
    yl_object*      _proto_table;
    yl_object*      _proto_bool;
    yl_object*      _proto_number;
    yl_object*      _proto_string;
    yl_object*      _proto_funcobj;
    yl_object*      _proto_cothread;
    yl_table*       _globals;

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


inline yl_cothread* yl_context_impl::get_cothread() const
{
    return _cothread;
}


inline yl_object* yl_context_impl::proto_object()
{
    return _proto_object;
}

inline yl_object* yl_context_impl::proto_array()
{
    return _proto_array;
}

inline yl_object* yl_context_impl::proto_table()
{
    return _proto_table;
}




#endif
