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


/*
    yl_exception
*/


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




/*
    yl_callframe
*/


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
    yl_stackref< yl_string > string = yl_string::alloc( value );
    _cothread->_stack.push_back( yl_value( YLOBJ_STRING, string.get() ) );
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
    yl_value value = yl_value( function._function->kind(), function._function );
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
    case YLOBJ_EXPOBJ:
        return YLVAL_EXPOSE;
        
    case YLOBJ_STRING:
        return YLVAL_STRING;
        
    case YLOBJ_FUNCOBJ:
    case YLOBJ_THUNKOBJ:
        return YLVAL_FUNCTION;
        
    case YLOBJ_SINGULAR:
        if ( value.is_true() || value.is_false() )
            return YLVAL_BOOL;
        else if ( value.is_null() )
            return YLVAL_NULL;
        else
            return YLVAL_UNKNOWN;
    
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



/*
    yl_invoke
*/


void yl_invoke( yl_callframe& xf )
{
    // Enter interpreter.
    yl_interp( xf._cothread, xf._base, xf._size, yl_opinst::MARK );
    
    // Recover number of results and update callframe.
    xf._size = xf._cothread->get_top() - xf._base;
}





/*
    yl_upval
*/

yl_gctype yl_upval::gctype =
{
    "upval",
    &yl_upval::destroy,
    &yl_upval::mark,
    nullptr
};


yl_stackref< yl_upval > yl_upval::alloc( unsigned index )
{
    void* p = yl_current->allocate( sizeof( yl_upval ) );
    return new ( p ) yl_upval( index );
}

yl_upval::yl_upval( unsigned index )
    :   yl_gcobject( YLOBJ_UPVAL, OPEN )
    ,   _index( index )
{
}

void yl_upval::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_upval* self = (yl_upval*)object;
    self->~yl_upval();
}

void yl_upval::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_upval* self = (yl_upval*)object;
    self->_value.mark( heap );
}





/*
    yl_iterator
*/

yl_iterator::yl_iterator()
    :   _kind( YLITER_CLOSED )
    ,   _object( nullptr )
    ,   _index( 0 )
{
}

void yl_iterator::mark( yl_gcheap* heap ) const
{
    switch ( _kind )
    {
    case YLITER_KEYS:
        heap->mark( _object );
        heap->mark( _slot );
        break;
        
    case YLITER_ARRAY:
        heap->mark( _array );
        break;
        
    case YLITER_TABLE:
        heap->mark( _buckets );
        break;
        
    case YLITER_GENERATOR:
        heap->mark( _generator );
        break;

    default:
        assert( ! "invalid iterator" );
        break;
    }
}

void yl_iterator::eager_mark( yl_gcheap* heap ) const
{
    switch ( _kind )
    {
    case YLITER_KEYS:
        heap->eager_mark( _object );
        heap->eager_mark( _slot );
        break;
        
    case YLITER_ARRAY:
        heap->eager_mark( _array );
        break;
        
    case YLITER_TABLE:
        heap->eager_mark( _buckets );
        break;
        
    case YLITER_GENERATOR:
        heap->eager_mark( _generator );
        break;

    default:
        assert( ! "invalid iterator" );
        break;
    }
}

void yl_iterator::open_vals( yl_value value )
{
    assert( _kind == YLITER_CLOSED );
    if ( value.is( YLOBJ_ARRAY ) )
    {
        _kind       = YLITER_ARRAY;
        _array      = (yl_array*)value.gcobject();
        _index      = 0;
    }
    else if ( value.is( YLOBJ_TABLE ) )
    {
        yl_table* table = (yl_table*)value.gcobject();
        _kind       = YLITER_TABLE;
        _buckets    = table->_buckets.get();
        _index      = 0;
        next_bucket();
    }
    else if ( value.is( YLOBJ_COTHREAD ) )
    {
        _kind       = YLITER_GENERATOR;
        _generator  = (yl_cothread*)value.gcobject();
    }
    else
    {
        throw yl_exception( "cannot iterate over object with no elements" );
    }
}

void yl_iterator::open_keys( yl_value value )
{
    assert( _kind == YLITER_CLOSED );
    if ( value.is_object() )
    {
        _kind       = YLITER_KEYS;
        _object     = (yl_object*)value.gcobject();
        _slot       = _object->_klass.get();
    }
    else
    {
        throw yl_exception( "cannot iterate over unkeyable object" );
    }
}

void yl_iterator::close()
{
    if ( _kind != YLITER_CLOSED )
    {
        _kind   = YLITER_CLOSED;
        _object = nullptr;
        _index  = 0;
    }
}


bool yl_iterator::has_values()
{
    switch ( _kind )
    {
    case YLITER_KEYS:
        return _slot->_index != yl_slot::EMPTY_KLASS;
        
    case YLITER_ARRAY:
        return _index < _array->length();
        
    case YLITER_TABLE:
        return _buckets && _index < _buckets->size();
        
    case YLITER_GENERATOR:
        // TODO.
        return false;

    default:
        assert( ! "invalid iterator" );
        return false;
    }
}

void yl_iterator::next1( yl_value* r )
{
    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        *r = yl_value( YLOBJ_STRING, _slot->_symbol.get() );
        break;
    }
        
    case YLITER_ARRAY:
    {
        *r = _array->get_index( _index );
        _index += 1;
        break;
    }
        
    case YLITER_TABLE:
    {
        *r = _buckets->at( _index ).key.get();
        _index += 1;
        next_bucket();
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
        *r = yl_null;
        break;
    }
    
    }
}

void yl_iterator::next2( yl_value* r, yl_value* b )
{
    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        *r = yl_value( YLOBJ_STRING, _slot->_symbol.get() );
        *b = _object->_slots.get()->at( _slot->_index ).get();
        assert( _slot->_index != yl_slot::EMPTY_KLASS );
        _slot = (yl_slot*)_slot->_parent.get();
        break;
    }
        
    case YLITER_ARRAY:
    {
        *r = _array->get_index( _index );
        *b = yl_null;
        _index += 1;
        break;
    }
        
    case YLITER_TABLE:
    {
        yl_bucketlist::bucket& bucket = _buckets->at( _index );
        *r = bucket.key.get();
        *b = bucket.value.get();
        _index += 1;
        next_bucket();
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
        *r = yl_null;
        *b = yl_null;
        break;
    }
    
    }
}

yl_iternext yl_iterator::next( yl_value vspace[ 2 ] )
{
    yl_iternext next;
    next.values = vspace;
    next.vcount = 0;

    switch ( _kind )
    {
    case YLITER_KEYS:
    {
        next.values[ 0 ] = yl_value( YLOBJ_STRING, _slot->_symbol.get() );
        next.values[ 1 ] = _object->_slots.get()->at( _slot->_index ).get();
        next.vcount = 2;
        assert( _slot->_index != yl_slot::EMPTY_KLASS );
        _slot = (yl_slot*)_slot->_parent.get();
        break;
    }
    
    case YLITER_ARRAY:
    {
        next.values[ 0 ] = _array->get_index( _index );
        next.vcount = 1;
        _index += 1;
        break;
    }
    
    case YLITER_TABLE:
    {
        yl_bucketlist::bucket& bucket = _buckets->at( _index );
        next.values[ 0 ] = bucket.key.get();
        next.values[ 1 ] = bucket.value.get();
        next.vcount = 2;
        _index += 1;
        next_bucket();
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
        break;
    }
    
    }
    
    return next;
}


void yl_iterator::next_bucket()
{
    if ( ! _buckets )
    {
        return;
    }
    
    while ( _index < _buckets->size() )
    {
        if ( _buckets->at( _index ).next != YL_EMPTY_BUCKET )
        {
            break;
        }
        
        _index += 1;
    }
}






/*
    yl_cothread
*/


yl_gctype yl_cothread::gctype =
{
    "cothread",
    &yl_cothread::destroy,
    &yl_cothread::mark,
    &yl_cothread::eager_mark,
};


yl_stackref< yl_cothread > yl_cothread::alloc()
{
    void* p = yl_current->allocate( sizeof( yl_cothread ) );
    return new ( p ) yl_cothread();
}

yl_cothread::yl_cothread()
    :   yl_gcobject( YLOBJ_COTHREAD )
{
}


void yl_cothread::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    self->~yl_cothread();
}

void yl_cothread::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    
    for ( const auto& frame : self->_frames )
    {
        heap->mark( frame.funcobj );
    }
    
    for ( const auto& value : self->_stack )
    {
        if ( value.is_gcobject() )
        {
            heap->mark( value.gcobject() );
        }
    }
    
    for ( const auto& upval : self->_locup )
    {
        heap->mark( upval );
    }
    
    for ( const auto& iterator : self->_iters )
    {
        iterator.mark( heap );
    }
}

void yl_cothread::eager_mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    
    for ( const auto& frame : self->_frames )
    {
        heap->eager_mark( frame.funcobj );
    }
    
    for ( const auto& value : self->_stack )
    {
        if ( value.is_gcobject() )
        {
            heap->eager_mark( value.gcobject() );
        }
    }
    
    for ( const auto& upval : self->_locup )
    {
        heap->eager_mark( upval );
    }
    
    for ( const auto& iterator : self->_iters )
    {
        iterator.eager_mark( heap );
    }
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


void yl_cothread::close_locup( size_t base, unsigned index )
{
    for ( size_t i = _locup.size(); i-- > base + index; )
    {
        yl_upval*& upval = _locup.at( i );
        if ( upval )
        {
            upval->close( this );
        }
    }
    _locup.resize( base + index );
}

void yl_cothread::close_iterator( size_t base, unsigned index )
{
    for ( size_t i = _iters.size(); i-- > base + index; )
    {
        yl_iterator& iterator = _iters.at( i );
        iterator.close();
    }
    _iters.resize( base + index );
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



