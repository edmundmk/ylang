//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_heapobj.h"
#include "yl_array.h"
#include "yl_table.h"





yl_context::yl_context()
    :   _impl( new yl_context_impl() )
{
}

yl_context::~yl_context()
{
}




__thread yl_context_impl* yl_current = nullptr;


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
    ,   _proto_funcobj( nullptr )
    ,   _proto_cothread( nullptr )
    ,   _globals( nullptr )
{
    mspace_track_large_chunks( _heap, 1 );
    
    
    _globals = yl_table::alloc( 16 );
}

yl_context_impl::~yl_context_impl()
{
    destroy_mspace( _heap );
}


void* yl_context_impl::malloc( size_t size )
{
    return mspace_malloc( _heap, size );
}

void yl_context_impl::write_barrier( yl_heapobj* object )
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



yl_object* yl_context_impl::superof( const yl_value& value )
{
    /*
        All values except null, undef, and the root object have a prototype.
        All prototypes are objects.  The prototypes of non-object values are
        built-in to the context.
    */

    switch ( value.kind() )
    {
    case YLOBJ_NULL:
    case YLOBJ_UNDEF:
        return nullptr;
        
    case YLOBJ_BOOL:
        return _proto_bool;
        
    case YLOBJ_NUMBER:
        return _proto_number;
        
    case YLOBJ_STRING:
        return _proto_string;
        
    case YLOBJ_OBJECT:
    case YLOBJ_NATIVE:
    case YLOBJ_ARRAY:
    case YLOBJ_TABLE:
        return ( (yl_object*)value.heapobj() )->superof();

    case YLOBJ_FUNCOBJ:
    case YLOBJ_THUNK:
        return _proto_funcobj;
        
    case YLOBJ_COTHREAD:
        return _proto_cothread;

    default:
        assert( ! "invalid value in superof()" );
        return nullptr;
    }
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
    
    return yl_value( YLOBJ_NULL, yl_null );
}




yl_value yl_context_impl::get_global( yl_string* key )
{
    return _globals->get_index( yl_value( YLOBJ_STRING, key ) );
}

void yl_context_impl::set_global( yl_string* key, const yl_value& value )
{
    _globals->set_index( yl_value( YLOBJ_STRING, key ), value );
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
    :   _context( context->_impl.get() )
    ,   _previous( yl_current )
{
    yl_current = _context;
}

yl_scope::~yl_scope()
{
    assert( yl_current == _context );
    yl_current = _previous;
}


