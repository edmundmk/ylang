//
//  yl_function.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_function.h"



yl_function* yl_function::alloc( yl_program* program )
{
    uint8_t upcount = program->upcount();
    void* p = yl_heap_current->malloc(
        sizeof( yl_function ) + sizeof( yl_heapref< yl_upval > ) * upcount );
    return new ( p ) yl_function( program );
}


yl_function::yl_function( yl_program* program )
    :   yl_heapobj( YLOBJ_FUNCTION )
    ,   _upcount( program->upcount() )
    ,   _refcount( 0 )
    ,   _program( program )
{
    for ( size_t i = 0; i < _upcount; ++i )
    {
        new ( _upval + i ) yl_heapref< yl_upval >();
    }
}



void yl_function::acquire()
{
    assert( _refcount < 255 );
    _refcount += 1;
    if ( _refcount == 1 )
    {
        // TODO: add to root set.
    }
}

void yl_function::release()
{
    assert( _refcount > 0 );
    _refcount -= 1;
    if ( _refcount == 0 )
    {
        // TODO: remove from root set.
    }
}



yl_program* yl_program::alloc(
        uint16_t valcount, size_t opcount, size_t xfcount, size_t varcount )
{
    size_t size = sizeof( yl_program );
    size += sizeof( yl_value ) * valcount;
    size += sizeof( yl_opinst ) * opcount;
    size += sizeof( yl_xframe ) * xfcount;
    size += sizeof( yl_varname ) * varcount;
    void* p = yl_heap_current->malloc( size );
    return new ( p ) yl_program( valcount, opcount, xfcount, varcount );
}


yl_program::yl_program(
        uint16_t valcount, size_t opcount, size_t xfcount, size_t varcount )
    :   yl_heapobj( YLOBJ_PROGRAM )
    ,   _valcount( valcount )
    ,   _opcount( opcount )
    ,   _xfcount( xfcount )
    ,   _varcount( varcount )
    ,   _paramcount( 0 )
    ,   _upcount( 0 )
    ,   _stackcount( 0 )
    ,   _varargs( false )
    ,   _coroutine( false )
{
    yl_value* values = (yl_value*)( this + 1 );
    for ( size_t i = 0; i < _valcount; ++i )
    {
        new ( values + i ) yl_value();
    }
    
    yl_opinst* ops = (yl_opinst*)( values + _valcount );
    for ( size_t i = 0; i < _opcount; ++i )
    {
        new ( ops + i ) yl_opinst();
    }
    
    yl_xframe* xframes = (yl_xframe*)( ops + _opcount );
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        new ( xframes + i ) yl_xframe();
    }
    
    yl_varname* varnames = (yl_varname*)( xframes + _xfcount );
    for ( size_t i = 0; i < _varcount; ++i )
    {
        new ( varnames + i ) yl_varname();
    }
}

yl_program::~yl_program()
{
    yl_value* values = (yl_value*)( this + 1 );
    for ( size_t i = 0; i < _valcount; ++i )
    {
        values[ i ].~yl_value();
    }
    
    yl_opinst* ops = (yl_opinst*)( values + _valcount );
    for ( size_t i = 0; i < _opcount; ++i )
    {
        ops[ i ].~yl_opinst();
    }
    
    yl_xframe* xframes = (yl_xframe*)( ops + _opcount );
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        xframes[ i ].~yl_xframe();
    }
    
    yl_varname* varnames = (yl_varname*)( xframes + _xfcount );
    for ( size_t i = 0; i < _varcount; ++i )
    {
        varnames[ i ].~yl_varname();
    }
}




