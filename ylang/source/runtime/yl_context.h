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
#include "yl_gcheap.h"


class yl_cothread;
class yl_value;
class yl_string;
class yl_object;
class yl_array;
class yl_table;
class yl_slot;




/*
    List of all ylang object types.
*/

enum yl_objkind : uint8_t
{
    // User-visible types (with tags that must match the NaN-boxing scheme).
    YLOBJ_OBJECT        = 0xFF,     // object
    YLOBJ_EXPOBJ        = 0xFE,     // object for exposed native object
    YLOBJ_ARRAY         = 0xFD,     // object derived from array prototype
    YLOBJ_TABLE         = 0xFC,     // object derived from table prototype
//                        0xFB,
//                        0xFA,
//                        0xF9,
    YLOBJ_QNAN          = 0xF8,     // number: quiet NaN
    YLOBJ_STRING        = 0xF7,     // string
    YLOBJ_FUNCOBJ       = 0xF6,     // bytecode function
    YLOBJ_THUNKOBJ      = 0xF5,     // native function
    YLOBJ_COTHREAD      = 0xF4,     // ylang stack object/coroutine
    YLOBJ_PROGRAM       = 0xF3,     // bytecode program (not user-visible)
//                        0xF2,
    YLOBJ_SINGULAR      = 0xF1,     // null/undefined/true/false
    YLOBJ_INFINITY      = 0xF0,     // number: infinity

    // Not user-visible and never NaN-boxed.
    YLOBJ_VALARRAY      = 0xEF,     // fixed-size array of values
    YLOBJ_BUCKETLIST    = 0xEE,     // fixed-size array of table buckets
    YLOBJ_SLOT          = 0xED,     // node in an object's class tree
    YLOBJ_UPVAL         = 0xEC,     // variable referenced from closure
    
};



/*
    A ylang context, which consists of the current runtime state of a
    ylang thread and GC heap.  Dynamic langauges are not well suited to
    multithreading, so each context is completely separate from other
    contexts.
*/


#define yl_current ((yl_context_impl*)yl_current_gcheap)



class yl_context_impl : public yl_gcheap
{
public:

    static yl_context_impl* unwrap( yl_context* context );

    yl_context_impl();
    ~yl_context_impl();


    // Execution context.

    yl_cothread*                get_cothread() const;
    void                        set_cothread( yl_cothread* cothread );


    // Object model.

    yl_stackref< yl_string >    symbol( yl_string* symbol );
    void                        destroy_symbol( yl_string* symbol );
    
    yl_object*                  superof( yl_value value );
    yl_slot*                    klassof( yl_object* prototype );

    yl_object*                  proto_object();
    yl_object*                  proto_array();
    yl_object*                  proto_table();

    yl_stackref< yl_object >    new_object( yl_object* prototype );


    // Global table.

    yl_object*                  globals();
    void                        set_global( const char* name, yl_value value );


private:


    yl_cothread*    _cothread;
    
    std::unordered_map< symkey, yl_string* > _symbols;
    
    yl_slot*        _root_klass;
    yl_object*      _proto_object;
    yl_object*      _proto_array;
    yl_object*      _proto_table;
    yl_object*      _proto_bool;
    yl_object*      _proto_number;
    yl_object*      _proto_string;
    yl_object*      _proto_function;
    yl_object*      _proto_cothread;
    yl_object*      _globals;

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

inline yl_object* yl_context_impl::globals()
{
    return _globals;
}




#endif
