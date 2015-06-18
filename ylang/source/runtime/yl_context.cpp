//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_heapobj.h"
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




__thread yl_context_impl* yl_current = nullptr;



yl_context_impl* yl_context_impl::unwrap( yl_context* context )
{
    return context->_impl.get();
}


yl_context_impl::yl_context_impl()
    :   _heap( create_mspace( 0, 0 ) )
    ,   _unmarked_colour( YL_YELLOW )
    ,   _cothread( nullptr )
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
    mspace_track_large_chunks( _heap, 1 );
    
    yl_scope scope( this );
    yl_alloc_scope alloc_scope;
    
    // Default cothread.
    _cothread       = yl_cothread::alloc();
    
    // Prototypes.
    _proto_object   = yl_object::alloc( nullptr );
    _proto_array    = yl_array::make_prototype();
    _proto_table    = yl_table::make_prototype();
    _proto_bool     = yl_object::alloc( _proto_object );
    _proto_number   = yl_object::alloc( _proto_object );
    _proto_string   = yl_object::alloc( _proto_object );
    _proto_function = yl_object::alloc( _proto_object );
    _proto_cothread = yl_object::alloc( _proto_object );
    
    // Globals.
    _globals = yl_object::alloc( _proto_object );
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
    destroy_mspace( _heap );
}


void* yl_context_impl::malloc( size_t size )
{
    return mspace_malloc( _heap, size );
}

void yl_context_impl::write_barrier_mark( yl_heapobj* object )
{
    // TODO.
}

void yl_context_impl::add_root( yl_heapobj* root )
{
    _roots.insert( root );
}

void yl_context_impl::remove_root( yl_heapobj* root )
{
    _roots.erase( root );
}



void yl_context_impl::set_cothread( yl_cothread* cothread )
{
    // TODO: interact with GC by 'unlocking' cothread.
    _cothread = cothread;
}



yl_string* yl_context_impl::symbol( yl_string* string )
{
    // Check if the string is already a symbol.
    if ( string->_is_symbol )
    {
        return string;
    }

    // Check if we already have an equivalent symbol.
    symkey key( string->hash(), string->c_str(), string->size() );
    auto i = _symbols.find( key );
    if ( i != _symbols.end() )
    {
        // TODO: interact with GC as this might ressurect this symbol.
        return i->second;
    }

    // Otherwise, promote the string to a symbol.
    string->_is_symbol = true;
    _symbols.emplace( key, string );
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
    case YLOBJ_EXPOSE:
    case YLOBJ_ARRAY:
    case YLOBJ_TABLE:
        return ( (yl_object*)value.heapobj() )->superof();

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
    // TODO: Interact with garbage collector.  Objects used as prototypes
    //      keep their klass alive.
    
    auto i = _klasses.find( prototype );
    if ( i != _klasses.end() )
    {
        return i->second;
    }
    
    yl_slot* klass = yl_slot::alloc( prototype );
    _klasses.emplace( prototype, klass );
    return klass;
}


yl_value yl_context_impl::new_object( yl_object* prototype )
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
        return yl_value( YLOBJ_OBJECT, yl_object::alloc( prototype ) );
    }
    else if ( kind == YLOBJ_ARRAY )
    {
        return yl_value( YLOBJ_ARRAY, yl_array::alloc( prototype, 0 ) );
    }
    else if ( kind == YLOBJ_TABLE )
    {
        return yl_value( YLOBJ_TABLE, yl_table::alloc( prototype, 0 ) );
    }
    
    return yl_null;
}


void yl_context_impl::set_global( const char* name, yl_value value )
{
    yl_string* key = yl_string::alloc( name )->symbol();
    _globals->set_key( key, value );
}







void _yl_call_thunk( const char* name, yl_thunk_function thunk )
{
    yl_alloc_scope ascope;
    yl_thunkobj* thunkobj = yl_thunkobj::alloc( thunk );
    yl_current->set_global( name, yl_value( YLOBJ_THUNKOBJ, thunkobj ) );
}
















yl_alloc_scope::yl_alloc_scope()
{
    // TODO.
}

yl_alloc_scope::~yl_alloc_scope()
{
    // TODO.
}





yl_scope::yl_scope( yl_context* context )
    :   _context( yl_context_impl::unwrap( context ) )
    ,   _previous( yl_current )
{
    yl_current = _context;
}

yl_scope::yl_scope( yl_context_impl* context )
    :   _context( context )
    ,   _previous( yl_current )
{
    yl_current = _context;
}

yl_scope::~yl_scope()
{
    assert( yl_current == _context );
    yl_current = _previous;
}


