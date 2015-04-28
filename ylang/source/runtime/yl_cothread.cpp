//
//  yl_cothread.cpp
//
//  Created by Edmund Kapusniak on 12/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_cothread.h"
#include "ylang.h"
#include <stringf.h>
#include "yl_interp.h"
#include "yl_array.h"
#include "yl_table.h"



// ----- yl_exception


yl_exception::yl_exception( const char* format, ... )
{
    va_list arguments;
    va_start( arguments, format );
    message = vstringf( format, arguments );
    va_end( arguments );
}


const char* yl_exception::what() const throw()
{
    return message.c_str();
}




// ----- yl_callframe


yl_callframe::yl_callframe()
    :   _cothread( yl_current->get_cothread() )
    ,   _base( 0 /* TODO */ )
    ,   _size( 0 )
{
}

yl_callframe::~yl_callframe()
{
}


void yl_callframe::push( std::nullptr_t )
{
    yl_tagval* s = _cothread->stack( _base, _size + 1 );
    s[ _size ] = yl_tagval( YLOBJ_NULL, yl_null );
    _size += 1;
}

void yl_callframe::push( bool value )
{
    yl_tagval* s = _cothread->stack( _base, _size + 1 );
    s[ _size ] = yl_tagval( YLOBJ_BOOL, value ? yl_true : yl_false );
    _size += 1;
}

void yl_callframe::push( const char* value )
{
    yl_tagval* s = _cothread->stack( _base, _size + 1 );
    s[ _size ] = yl_tagval( YLOBJ_STRING, yl_string::alloc( value ) );
    _size += 1;
}

void yl_callframe::push( double value )
{
    yl_tagval* s = _cothread->stack( _base, _size + 1 );
    s[ _size ] = value;
    _size += 1;
}

void yl_callframe::push( yl_expose* value )
{
    // TODO.
}

void yl_callframe::push( const yl_function& function )
{
    yl_tagval* s = _cothread->stack( _base, _size + 1 );
    s[ _size ] = yl_tagval( function._function->kind(), function._function );
    _size += 1;
}


size_t yl_callframe::size() const
{
    return _size;
}


yl_valkind yl_callframe::at( size_t index ) const
{
    assert( index < _size );
    yl_tagval* s = _cothread->stack( _base, _size );
    yl_objkind kind = s[ index ].kind();
    if ( kind <= YLOBJ_STRING )
        return (yl_valkind)kind;
    else if ( kind == YLOBJ_NATIVE )
        return YLVAL_EXPOSE;
    else if ( kind == YLOBJ_FUNCOBJ || kind == YLOBJ_THUNK )
        return YLVAL_FUNCTION;
    return YLVAL_NULL;
}

bool yl_callframe::get_bool( size_t index ) const
{
    yl_tagval* s = _cothread->stack( _base, _size );
    if ( s[ index ].kind() != YLOBJ_BOOL )
    {
        throw yl_exception( "expected boolean" );
    }
    return s[ index ].heapobj() == yl_true;
}

double yl_callframe::get_number( size_t index ) const
{
    yl_tagval* s = _cothread->stack( _base, _size );
    if ( s[ index ].kind() != YLOBJ_NUMBER )
    {
        throw yl_exception( "expected number" );
    }
    return s[ index ].number();
}

const char* yl_callframe::get_string( size_t index ) const
{
    yl_tagval* s = _cothread->stack( _base, _size );
    if ( s[ index ].kind() != YLOBJ_STRING )
    {
        throw yl_exception( "expected string" );
    }
    yl_string* string = (yl_string*)s[ index ].heapobj();
    return string->c_str();
}

yl_expose* yl_callframe::get_expose( size_t index ) const
{
    // TODO.
    return nullptr;
}

yl_function yl_callframe::get_function( size_t index ) const
{
    yl_tagval* s = _cothread->stack( _base, _size );
    if ( s[ index ].kind() != YLOBJ_FUNCOBJ && s[ index ].kind() != YLOBJ_THUNK )
    {
        throw yl_exception( "expected function" );
    }
    return yl_function( (yl_funcbase*)s[ index ].heapobj() );
}


void yl_callframe::clear()
{
    _size = 0;
}




// ---- yl_invoke


void yl_invoke( yl_callframe& xf )
{
    // Enter interpreter.
    yl_interp( xf._cothread, xf._base, xf._size, yl_opinst::MARK );
    
    // Recover number of results and update callframe.
    xf._size = xf._cothread->get_mark() - xf._base;
}





// ---- yl_iterator


yl_iterator::yl_iterator()
    :   _kind( YLITER_CLOSED )
    ,   _object( nullptr )
    ,   _index( 0 )
{
}

void yl_iterator::open_vals( const yl_tagval& value )
{
    assert( _kind == YLITER_CLOSED );
    if ( value.kind() == YLOBJ_ARRAY )
    {
        _kind       = YLITER_ARRAY;
        _array      = (yl_array*)value.heapobj();
        _index      = 0;
    }
    else if ( value.kind() == YLOBJ_TABLE )
    {
        _kind       = YLITER_TABLE;
        _table      = (yl_table*)value.heapobj();
        _index      = 0;
    }
    else if ( value.kind() == YLOBJ_COTHREAD )
    {
        _kind       = YLITER_GENERATOR;
        _generator  = (yl_cothread*)value.heapobj();
    }
    else
    {
        throw yl_exception( "cannot iterate over object with no elements" );
    }
}

void yl_iterator::open_keys( const yl_tagval& value )
{
    assert( _kind == YLITER_CLOSED );
    if ( value.kind() & YLOBJ_IS_OBJECT )
    {
        _kind       = YLITER_KEYS;
        _object     = (yl_object*)value.heapobj();
        _index      = 0;
    }
    else
    {
        throw yl_exception( "cannot iterate over unkeyable object" );
    }
}

void yl_iterator::close()
{
    assert( _kind != YLITER_CLOSED );
    _kind   = YLITER_CLOSED;
    _object = nullptr;
    _index  = 0;
}


bool yl_iterator::has_values()
{
    switch ( _kind )
    {
    case YLITER_KEYS:
        // TODO.
        return false;
        
    case YLITER_ARRAY:
        return _index < _array->size();
        
    case YLITER_TABLE:
        // TODO.
        return false;
        
    case YLITER_GENERATOR:
        // TODO.
        return false;

    default:
        assert( ! "invalid iterator" );
        return false;
    }
}

void yl_iterator::next1( yl_tagval* r )
{
    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        // TODO.
        break;
    }
        
    case YLITER_ARRAY:
    {
        *r = _array->get( _index );
        _index += 1;
        break;
    }
        
    case YLITER_TABLE:
    {
        // TODO.
        break;
    }
    
    case YLITER_GENERATOR:
    {
        // TODO.
        break;
    }

    default:
    {
        assert( ! "invalid iterator" );
        *r = yl_tagval( YLOBJ_NULL, yl_null );
        break;
    }
    
    }
}

void yl_iterator::next2( yl_tagval* r, yl_tagval* b )
{
    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        // TODO.
        break;
    }
        
    case YLITER_ARRAY:
    {
        *r = _array->get( _index );
        *b = yl_tagval( YLOBJ_NULL, yl_null );
        _index += 1;
        break;
    }
        
    case YLITER_TABLE:
    {
        // TODO.
        break;
    }
    
    case YLITER_GENERATOR:
    {
        // TODO.
        break;
    }

    default:
    {
        assert( ! "invalid iterator" );
        *r = yl_tagval( YLOBJ_NULL, yl_null );
        *b = yl_tagval( YLOBJ_NULL, yl_null );
        break;
    }
    
    }
}

void yl_iterator::next( yl_cothread *t, unsigned r, unsigned b )
{
    if ( b == 0 )
    {
        return;
    }

    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        // TODO.
        break;
    }
    
    case YLITER_ARRAY:
    {
        yl_tagval v = _array->get( _index );
        _index += 1;
    
        if ( b != yl_opinst::MARK )
        {
            yl_tagval* s = t->stack( r, b );
            s[ 0 ] = v;
            for ( size_t i = 1; i < b; ++i )
            {
                s[ i ] = yl_tagval( YLOBJ_NULL, yl_null );
            }
        }
        else
        {
            yl_tagval* s = t->stack( r, 1 );
            s[ 0 ] = v;
            t->set_mark( r + 1 );
        }
    }
    
    case YLITER_TABLE:
    {
        // TODO.
        break;
    }
    
    case YLITER_GENERATOR:
    {
        // TODO.
        break;
    }
    
    default:
    {
        assert( ! "invalid iterator" );
        if ( b != yl_opinst::MARK )
        {
            yl_tagval* s = t->stack( r, b );
            for ( size_t i = 0; i < b; ++i )
            {
                s[ i ] = yl_tagval( YLOBJ_NULL, yl_null );
            }
        }
        else
        {
            t->set_mark( r );
        }
        break;
    }
    
    }
}








// ----- yl_cothread


yl_cothread* yl_cothread::alloc()
{
    void* p = yl_current->malloc( sizeof( yl_cothread ) );
    return new ( p ) yl_cothread();
}

yl_cothread::yl_cothread()
    :   yl_heapobj( YLOBJ_COTHREAD )
{
}


void yl_cothread::push_frame( const yl_stackframe& frame )
{
    _frames.push_back( frame );
}

yl_stackframe yl_cothread::pop_frame()
{
    yl_stackframe frame = _frames.back();
    _frames.pop_back();
    return frame;
}

yl_tagval* yl_cothread::stack( size_t base, size_t count )
{
    if ( _stack.capacity() < base + count )
    {
        _stack.reserve( _stack.capacity() * 2 );
    }
    if ( _stack.size() < base + count )
    {
        _stack.resize( base + count );
    }
    return _stack.data() + base;
}

yl_upval** yl_cothread::locup( size_t base, size_t count )
{
    if ( _locup.capacity() < base + count )
    {
        _locup.reserve( _stack.capacity() * 2 );
    }
    if ( _locup.size() < base + count )
    {
        _locup.resize( base + count );
    }
    return _locup.data() + base;
}

yl_iterator* yl_cothread::iters( size_t base, size_t count )
{
    if ( _iters.capacity() < base + count )
    {
        _iters.reserve( _stack.capacity() * 2 );
    }
    if ( _iters.size() < base + count )
    {
        _iters.resize( base + count );
    }
    return _iters.data() + base;
}
    
unsigned yl_cothread::get_locup_base() const
{
    if ( _frames.size() )
    {
        const yl_stackframe& frame = _frames.back();
        return frame.locup_base + frame.funcobj->program()->locupcount();
    }
    return 0;
}

unsigned yl_cothread::get_iters_base() const
{
    if ( _frames.size() )
    {
        const yl_stackframe& frame = _frames.back();
        return frame.iters_base + frame.funcobj->program()->iterscount();
    }
    return 0;
}



