//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_context.h"
#include "yl_cothread.h"
#include "yl_array.h"
#include "yl_table.h"
#include "yl_object.h"





yl_context::yl_context()
    :   _impl( new yl_context_impl() )
{
}

yl_context::~yl_context()
{
}






yl_context_impl* yl_context_impl::unwrap( yl_context* context )
{
    return context->_impl.get();
}


yl_context_impl::yl_context_impl()
    :   _cothread( nullptr )
    ,   _proto_object( nullptr )
    ,   _proto_array( nullptr )
    ,   _proto_table( nullptr )
    ,   _proto_bool( nullptr )
    ,   _proto_number( nullptr )
    ,   _proto_string( nullptr )
    ,   _proto_function( nullptr )
    ,   _proto_cothread( nullptr )
    ,   _globals( nullptr )
{
    // Register GC types.


    // Initialize context.
    yl_scope scope( this );
    
    // Default cothread.
    _cothread       = yl_cothread::alloc().incref();
    
    // Prototypes.
    _proto_object   = yl_object::alloc( nullptr ).incref();
    _proto_array    = yl_array::make_prototype().incref();
    _proto_table    = yl_table::make_prototype().incref();
    _proto_bool     = yl_object::alloc( _proto_object ).incref();
    _proto_number   = yl_object::alloc( _proto_object ).incref();
    _proto_string   = yl_object::alloc( _proto_object ).incref();
    _proto_function = yl_object::alloc( _proto_object ).incref();
    _proto_cothread = yl_object::alloc( _proto_object ).incref();
    
    // Globals.
    _globals = yl_object::alloc( _proto_object ).incref();
    set_global( "global", yl_value( YLOBJ_OBJECT, _globals ) );
    set_global( "object", yl_value( YLOBJ_OBJECT, _proto_object ) );
    set_global( "array", yl_value( YLOBJ_OBJECT, _proto_array ) );
    set_global( "table", yl_value( YLOBJ_OBJECT, _proto_table ) );
    set_global( "bool", yl_value( YLOBJ_OBJECT, _proto_bool ) );
    set_global( "number", yl_value( YLOBJ_OBJECT, _proto_number ) );
    set_global( "string", yl_value( YLOBJ_OBJECT, _proto_string ) );
    set_global( "function", yl_value( YLOBJ_OBJECT, _proto_function ) );
    set_global( "cothread", yl_value( YLOBJ_OBJECT, _proto_cothread ) );
}

yl_context_impl::~yl_context_impl()
{
}




void yl_context_impl::set_cothread( yl_cothread* cothread )
{
    // TODO: interact with GC by 'unlocking' cothread.
    _cothread = cothread;
}



yl_stackref< yl_string > yl_context_impl::symbol( yl_string* string )
{
    // Check if the string is already a symbol.
    if ( string->check_gcflags( yl_string::SYMBOL ) )
    {
        return string;
    }

    weak_lock();

    // Check if we already have an equivalent symbol.
    symkey key( string->hash(), string->c_str(), string->size() );
    auto i = _symbols.find( key );
    if ( i != _symbols.end() )
    {
        yl_stackref< yl_string > result = weak_obtain( i->second );
        weak_unlock();
        return result;
    }

    // Otherwise, promote the string to a symbol.
    string->set_gcflags( yl_string::SYMBOL );
    _symbols.emplace( key, weak_create( string ) );
    
    weak_unlock();
    
    return string;
}



yl_object* yl_context_impl::superof( yl_value value )
{
    /*
        All values except null, undef, and the root object have a prototype.
        All prototypes are objects.  The prototypes of non-object values are
        built-in to the context.
    */
  
    if ( value.is_number() )
    {
        return _proto_number;
    }
    
    switch ( value.kind() )
    {
    case YLOBJ_OBJECT:
    case YLOBJ_EXPOBJ:
    case YLOBJ_ARRAY:
    case YLOBJ_TABLE:
        return ( (yl_object*)value.gcobject() )->superof();

    case YLOBJ_STRING:
        return _proto_string;

    case YLOBJ_FUNCOBJ:
    case YLOBJ_THUNKOBJ:
        return _proto_function;

    case YLOBJ_COTHREAD:
        return _proto_cothread;

    case YLOBJ_SINGULAR:
        if ( value.is_true() || value.is_false() )
            return _proto_bool;
        else
            return nullptr;
        
    default:
        assert( ! "invalid value in superof()" );
        return nullptr;
    }
}


yl_slot* yl_context_impl::klassof( yl_object* prototype )
{
    yl_gcobject* klass = get_oolref( prototype );
    if ( klass )
    {
        assert( klass->kind() == YLOBJ_SLOT );
        return (yl_slot*)klass;
    }

    yl_stackref< yl_slot > newklass = yl_slot::alloc( prototype );
    set_oolref( prototype, newklass.get() );
    return newklass.get();
}


yl_stackref< yl_object > yl_context_impl::new_object( yl_object* prototype )
{
    /*
        Any object with the array prototype in its prototype chain is an array.
        Any object with the table prototype in its prototype chain is a table.
    */
    
    yl_objkind kind = YLOBJ_OBJECT;
    for ( yl_object* p = prototype; p != nullptr; p = p->superof() )
    {
        if ( p == _proto_array )
        {
            kind = YLOBJ_ARRAY;
            break;
        }
        if ( p == _proto_table )
        {
            kind = YLOBJ_TABLE;
            break;
        }
    }
    
    if ( kind == YLOBJ_OBJECT )
    {
        return yl_object::alloc( prototype );
    }
    else if ( kind == YLOBJ_ARRAY )
    {
        return yl_array::alloc( prototype, 0 );
    }
    else if ( kind == YLOBJ_TABLE )
    {
        return yl_table::alloc( prototype, 0 );
    }
    
    throw yl_exception( "invalid prototype object" );
}


void yl_context_impl::set_global( const char* name, yl_value value )
{
    yl_stackref< yl_string > key = yl_string::alloc( name )->symbol();
    _globals->set_key( key.get(), value );
}







void _yl_call_thunk( const char* name, yl_thunk_function thunk )
{
    yl_stackref< yl_thunkobj > thunkobj = yl_thunkobj::alloc( thunk );
    yl_current->set_global( name, yl_value( YLOBJ_THUNKOBJ, thunkobj.get() ) );
}











yl_scope::yl_scope( yl_context* context )
    :   _context( yl_context_impl::unwrap( context ) )
    ,   _previous( yl_current )
{
    yl_current_gcheap = _context;
}

yl_scope::yl_scope( yl_context_impl* context )
    :   _context( context )
    ,   _previous( yl_current )
{
    yl_current_gcheap = _context;
}

yl_scope::~yl_scope()
{
    assert( yl_current_gcheap == _context );
    yl_current_gcheap = _previous;
}


