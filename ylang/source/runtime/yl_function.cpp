//
//  yl_function.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_function.h"
#include "yl_program.h"





void yl_function::acquire()
{
    if ( _function )
    {
        _function->incref();
    }
}

void yl_function::release()
{
    if ( _function )
    {
        _function->decref();
    }
    _function = nullptr;
}





yl_gctype yl_thunkobj::gctype =
{
    "thunkobj",
    &yl_thunkobj::destroy,
    nullptr,
    nullptr
};


yl_stackref< yl_thunkobj > yl_thunkobj::alloc( yl_thunk_function thunk )
{
    void* p = yl_current->allocate( sizeof( yl_thunkobj ) );
    return new ( p ) yl_thunkobj( thunk );
}

yl_thunkobj::yl_thunkobj( yl_thunk_function thunk )
    :   yl_gcobject( YLOBJ_THUNKOBJ, YL_GCFLAG_LEAF )
    ,   _thunk( thunk )
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
    "funcobj",
    &yl_funcobj::destroy,
    &yl_funcobj::mark,
    nullptr
};


yl_function yl_funcobj::make_function( yl_funcobj* funcobj )
{
    return yl_function( funcobj );
}


yl_stackref< yl_funcobj > yl_funcobj::alloc( yl_program* program )
{
    uint8_t upcount = program->upcount();
    void* p = yl_current->allocate(
        sizeof( yl_funcobj ) + sizeof( yl_heapref< yl_upval > ) * upcount );
    return new ( p ) yl_funcobj( program );
}


yl_funcobj::yl_funcobj( yl_program* program )
    :   yl_gcobject( YLOBJ_FUNCOBJ )
    ,   _upcount( program->upcount() )
    ,   _program( program )
{
    for ( size_t i = 0; i < _upcount; ++i )
    {
        new ( _upval + i ) yl_stackref< yl_upval >();
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








