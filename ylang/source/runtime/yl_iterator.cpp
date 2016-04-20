//
//  yl_iterator.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_iterator.h"
#include "yl_gcheap.h"
#include "yl_object.h"
#include "yl_array.h"
#include "yl_table.h"
#include "yl_cothread.h"
#include "yl_function.h"
#include "yl_cothread.h"



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
        _genvalues  = { 0, (unsigned)-1 };
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
        generate();
        return _generator->has_frames();

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
        *r = _slot->_symbol.get();
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
        generate();
        yl_value* s = _generator->stack_peek( _genvalues.base, _genvalues.count );
        *r = _genvalues.count >= 1 ? s[ 0 ] : yl_null;
        _genvalues = { 0, (unsigned)-1 };
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
        *r = _slot->_symbol.get();
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
        generate();
        yl_value* s = _generator->stack_peek( _genvalues.base, _genvalues.count );
        *r = _genvalues.count >= 1 ? s[ 0 ] : yl_null;
        *b = _genvalues.count >= 2 ? s[ 1 ] : yl_null;
        _genvalues = { 0, (unsigned)-1 };
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
        next.values[ 0 ] = _slot->_symbol.get();
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
        generate();
        next.values = _generator->stack_peek( _genvalues.base, _genvalues.count );;
        next.vcount = _genvalues.count;
        _genvalues = { 0, (unsigned)-1 };
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


void yl_iterator::generate()
{
    if ( _genvalues.count == -1 )
    {
        _genvalues = yl_generate( _generator );
    }
}




