//
//  yl_function.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_function.h"
#include "yl_program.h"




yl_function::yl_function( yl_gcobject* function )
    :   _function( nullptr )
{
    assert( function->kind() == YLOBJ_FUNCOBJ
            || function->kind() == YLOBJ_THUNKOBJ );
    reset( function );
}


void yl_function::reset( yl_gcobject* function )
{
    if ( _function )
    {
        _function->decref();
    }
    _function = function;
    if ( _function )
    {
        _function->incref();
    }
}






yl_gctype yl_thunkobj::gctype =
{
    YLOBJ_THUNKOBJ,
    YL_GCFLAG_LEAF,
    "thunkobj",
    &yl_thunkobj::destroy,
    nullptr,
    nullptr
};


yl_thunkobj::yl_thunkobj( yl_thunk_function thunk )
    :   _thunk( thunk )
{
}

void yl_thunkobj::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_thunkobj* self = (yl_thunkobj*)object;
    self->~yl_thunkobj();
}

yl_thunk_function yl_thunkobj::thunk()
{
    return _thunk;
}





yl_gctype yl_funcobj::gctype =
{
    YLOBJ_FUNCOBJ,
    YL_GCFLAG_NONE,
    "funcobj",
    &yl_funcobj::destroy,
    &yl_funcobj::mark,
    nullptr
};


yl_function yl_funcobj::make_function( yl_funcobj* funcobj )
{
    return yl_function( funcobj );
}


yl_rootref< yl_funcobj > yl_funcobj::alloc( yl_program* program )
{
    uint8_t upcount = program->upcount();
    size_t sz = sizeof( yl_funcobj ) + sizeof( yl_heapref< yl_upval > ) * upcount;
    return yl_gcalloc< yl_funcobj >( sz, program );
}


yl_funcobj::yl_funcobj( yl_program* program )
    :   _upcount( program->upcount() )
    ,   _program( program )
{
    for ( size_t i = 0; i < _upcount; ++i )
    {
        new ( _upval + i ) yl_heapref< yl_upval >();
    }
}

void yl_funcobj::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_funcobj* self = (yl_funcobj*)object;
    self->~yl_funcobj();
}

void yl_funcobj::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_funcobj* self = (yl_funcobj*)object;
    self->_program.mark( heap );
    for ( size_t i = 0; i < self->_upcount; ++i )
    {
        self->_upval[ i ].mark( heap );
    }
}








