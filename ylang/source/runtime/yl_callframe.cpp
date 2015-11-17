//
//  yl_callframe.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "ylang.h"
#include "yl_context.h"
#include "yl_cothread.h"



yl_callframe::yl_callframe()
    :   _cothread( yl_current->get_cothread() )
    ,   _base( _cothread->get_top() )
    ,   _size( 0 )
{
}

yl_callframe::yl_callframe( yl_cothread* cothread, unsigned base, unsigned size )
    :   _cothread( cothread )
    ,   _base( base )
    ,   _size( size )
{
}

yl_callframe::~yl_callframe()
{
}

void yl_callframe::push( std::nullptr_t )
{
    assert( _base + _size == _cothread->get_top() );
    _cothread->_stack.push_back( yl_null );
    _size += 1;
}

void yl_callframe::push_bool( bool value )
{
    assert( _base + _size == _cothread->get_top() );
    _cothread->_stack.push_back( value ? yl_true : yl_false );
    _size += 1;
}

void yl_callframe::push( const char* value )
{
    assert( _base + _size == _cothread->get_top() );
    yl_rootref< yl_string > string = yl_string::alloc( value );
    _cothread->_stack.push_back( yl_value( string.get() ) );
    _size += 1;
}

void yl_callframe::push( double value )
{
    assert( _base + _size == _cothread->get_top() );
    _cothread->_stack.push_back( value );
    _size += 1;
}

void yl_callframe::push( yl_expose* value )
{
    // TODO.
}

void yl_callframe::push( const yl_function& function )
{
    assert( _base + _size == _cothread->get_top() );
    yl_value value = yl_value( function._function );
    _cothread->_stack.push_back( value );
    _size += 1;
}


size_t yl_callframe::size() const
{
    return _size;
}


yl_valkind yl_callframe::at( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is_number() )
    {
        return YLVAL_NUMBER;
    }
    switch ( value.kind() )
    {
    case YLOBJ_NULLUNDEF:
    case YLOBJ_BOOLEAN:
    case YLOBJ_STRING:
    case YLOBJ_EXPOBJ:
        return (yl_valkind)value.kind();
        
    case YLOBJ_FUNCOBJ:
    case YLOBJ_THUNKOBJ:
        return YLVAL_FUNCTION;
    
    default:
        return YLVAL_UNKNOWN;
    }
}

bool yl_callframe::get_bool( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is_true() )
    {
        return true;
    }
    else if ( value.is_false() )
    {
        return false;
    }
    throw yl_exception( "expected boolean" );
}

double yl_callframe::get_number( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is_number() )
    {
        return value.number();
    }
    throw yl_exception( "expected number" );
}

int yl_callframe::get_integer( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is_number() )
    {
        double number = value.number();
        if ( is_integer( number ) && number >= INT_MIN && number <= INT_MAX )
        {
            return (int)number;
        }
    }
    throw yl_exception( "expected integer" );
}

const char* yl_callframe::get_string( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is( YLOBJ_STRING ) )
    {
        yl_string* string = (yl_string*)value.gcobject();
        return string->c_str();
    }
    throw yl_exception( "expected string" );
}

yl_expose* yl_callframe::get_expose( size_t index ) const
{
    // TODO.
    return nullptr;
}

yl_function yl_callframe::get_function( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is( YLOBJ_FUNCOBJ ) || value.is( YLOBJ_THUNKOBJ ) )
    {
        return yl_function( value.gcobject() );
    }
    throw yl_exception( "expected function" );
}

yl_gcobject* yl_callframe::get_gcobject( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "missing argument" );
    }
    yl_value value = _cothread->_stack.at( _base + index );
    if ( value.is_gcobject() )
    {
        return value.gcobject();
    }
    return nullptr;
}


void yl_callframe::clear()
{
    assert( _base + _size == _cothread->get_top() );
    _cothread->stack_trim( _base, 0 );
    _size = 0;
}


