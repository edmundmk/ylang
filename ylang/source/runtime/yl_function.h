//
//  yl_function.h
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#ifndef YL_FUNCTION_H
#define YL_FUNCTION_H


#include <vector>
#include "yl_code.h"
#include "yl_context.h"
#include "yl_value.h"
#include "yl_upval.h"


class yl_funcobj;
class yl_program;
class yl_upval;




/*
    A native thunk.
*/

typedef void (*yl_thunk_function)( yl_callframe& xf );

class yl_thunkobj : public yl_gcobject
{
public:

    static yl_gctype gctype;

    explicit yl_thunkobj( yl_thunk_function thunk );

    yl_thunk_function thunk();


private:

    static void destroy( yl_gcheap* heap, yl_gcobject* object );

    yl_thunk_function _thunk;

};



/*
    A bytecode function.
*/

class yl_funcobj : public yl_gcobject
{
public:

    static yl_gctype gctype;

    static yl_function make_function( yl_funcobj* funcobj );

    static yl_rootref< yl_funcobj > alloc( yl_program* program );

    yl_program*         program();
    
    void                set_upval( size_t index, yl_upval* upval );
    yl_upval*           get_upval( size_t index );


private:

    friend_yl_gcalloc;

    explicit yl_funcobj( yl_program* program );

    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

    unsigned                    _upcount;
    yl_heapref< yl_program >    _program;
    yl_heapref< yl_upval >      _upval[ 0 ];

};



/*

*/




inline yl_program* yl_funcobj::program()
{
    return _program.get();
}

inline void yl_funcobj::set_upval( size_t index, yl_upval* upval )
{
    assert( index < _upcount );
    _upval[ index ].set( upval );
}

inline yl_upval* yl_funcobj::get_upval( size_t index )
{
    assert( index < _upcount );
    return _upval[ index ].get();
}







#endif




