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
#include "yl_upval.h"
#include "yl_function.h"
#include "yl_program.h"
#include "yl_exception.h"





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
    :   _klassid( 1 )
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
    register_type( &yl_object::gctype );
    register_type( &yl_array::gctype );
    register_type( &yl_table::gctype );
    register_type( &yl_string::gctype );
    register_type( &yl_funcobj::gctype );
    register_type( &yl_thunkobj::gctype );
    register_type( &yl_cothread::gctype );
    register_type( &yl_program::gctype );
    register_type( &yl_valarray::gctype );
    register_type( &yl_bucketlist::gctype );
    register_type( &yl_slot::gctype );
    register_type( &yl_upval::gctype );

    // Initialize context.
    yl_scope scope( this );
    
    // Prototypes.
    _root_klass     = yl_gcnew< yl_slot >( nullptr );
    _proto_object   = yl_gcnew< yl_object >( nullptr );
    _proto_array    = yl_array::make_prototype();
    _proto_table    = yl_table::make_prototype();
    _proto_bool     = yl_gcnew< yl_object >( _proto_object.get() );
    _proto_number   = yl_gcnew< yl_object >( _proto_object.get() );
    _proto_string   = yl_gcnew< yl_object >( _proto_object.get() );
    _proto_function = yl_gcnew< yl_object >( _proto_object.get() );
    _proto_cothread = yl_gcnew< yl_object >( _proto_object.get() );
    
    // Globals.
    _globals = yl_gcnew< yl_object >( _proto_object.get() );
    _globals->set_key( "global", _globals.get() );
    _globals->set_key( "object", _proto_object.get() );
    _globals->set_key( "array", _proto_array.get() );
    _globals->set_key( "table", _proto_table.get() );
    _globals->set_key( "bool", _proto_bool.get() );
    _globals->set_key( "number", _proto_number.get() );
    _globals->set_key( "string", _proto_string.get() );
    _globals->set_key( "function", _proto_function.get() );
    _globals->set_key( "cothread", _proto_cothread.get() );

    // Default cothread.
    push_cothread( yl_gcnew< yl_cothread >().get() );
}

yl_context_impl::~yl_context_impl()
{
    yl_scope scope( this );
    _root_klass.reset();
    _proto_object.reset();
    _proto_array.reset();
    _proto_table.reset();
    _proto_bool.reset();
    _proto_number.reset();
    _proto_string.reset();
    _proto_function.reset();
    _proto_cothread.reset();
    _globals.reset();
    _cothreads.clear();
}




void yl_context_impl::push_cothread( yl_cothread* cothread )
{
    if ( _cothreads.size() )
    {
        eager_unlock( _cothreads.back().get() );
    }
    _cothreads.push_back( cothread );
    eager_lock( cothread );
}

yl_rootref< yl_cothread > yl_context_impl::pop_cothread()
{
    eager_unlock( _cothreads.back().get() );
    yl_rootref< yl_cothread > cothread = std::move( _cothreads.back() );
    _cothreads.pop_back();
    if ( _cothreads.size() )
    {
        eager_lock( _cothreads.back().get() );
    }
    return cothread;
}



yl_rootref< yl_string > yl_context_impl::symbol( yl_string* string )
{
    weak_lock();

    // Check if we already have an equivalent symbol.
    symkey key( string->hash(), string->c_str(), string->size() );
    auto i = _symbols.find( key );
    if ( i != _symbols.end() )
    {
        yl_rootref< yl_string > result = weak_obtain( i->second );
        if ( result )
        {
            weak_unlock();
            return result;
        }
        else
        {
            assert( ! "unable to resurrect weak" );
        }
    }

    // Otherwise, promote the string to a symbol.
    string->set_gcflags( yl_string::SYMBOL );
    _symbols.emplace( key, weak_create( string ) );
    
    weak_unlock();
    
    return string;
}

void yl_context_impl::destroy_symbol( yl_string* symbol )
{
    // Destroying a string used as a symbol.  GC should have locked the
    // weak lock already.
    
    symkey key( symbol->hash(), symbol->c_str(), symbol->size() );
    assert( _symbols.at( key ) == symbol );
    _symbols.erase( key );
    
}


uintptr_t yl_context_impl::klassid()
{
    return _klassid++;
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
        return _proto_number.get();
    }
    
    switch ( value.kind() )
    {
    case YLOBJ_OBJECT:
    case YLOBJ_EXPOBJ:
    case YLOBJ_ARRAY:
    case YLOBJ_TABLE:
        return ( (yl_object*)value.gcobject() )->superof();

    case YLOBJ_STRING:
        return _proto_string.get();

    case YLOBJ_FUNCOBJ:
    case YLOBJ_THUNKOBJ:
        return _proto_function.get();

    case YLOBJ_COTHREAD:
        return _proto_cothread.get();

    case YLOBJ_BOOLEAN:
        return _proto_bool.get();
        
    case YLOBJ_NULLUNDEF:
        return nullptr;
        
    default:
        assert( ! "invalid value in superof()" );
        return nullptr;
    }
}


yl_slot* yl_context_impl::klassof( yl_object* prototype )
{
    if ( ! prototype )
    {
        return _root_klass.get();
    }

    yl_gcobject* klass = get_oolref( prototype );
    if ( klass )
    {
        assert( klass->kind() == YLOBJ_SLOT );
        return (yl_slot*)klass;
    }

    yl_rootref< yl_slot > new_klass = yl_gcnew< yl_slot >( prototype );
    set_oolref( prototype, new_klass.get() );
    return new_klass.get();
}


yl_rootref< yl_object > yl_context_impl::new_object( yl_object* prototype )
{
    /*
        Any object with the array prototype in its prototype chain is an array.
        Any object with the table prototype in its prototype chain is a table.
    */
    
    yl_objkind kind = YLOBJ_OBJECT;
    for ( yl_object* p = prototype; p != nullptr; p = p->superof() )
    {
        assert( p->kind() >= YLOBJ_TABLE );

        if ( p == _proto_array.get() )
        {
            kind = YLOBJ_ARRAY;
            break;
        }
        if ( p == _proto_table.get() )
        {
            kind = YLOBJ_TABLE;
            break;
        }
    }
    
    if ( kind == YLOBJ_OBJECT )
    {
        return yl_gcnew< yl_object >( prototype );
    }
    else if ( kind == YLOBJ_ARRAY )
    {
        return yl_gcnew< yl_array>( prototype, 0 );
    }
    else if ( kind == YLOBJ_TABLE )
    {
        return yl_gcnew< yl_table >( prototype, 0 );
    }
    
    throw yl_exception( "invalid prototype object" );
}






void _yl_global_function( const char* name, yl_thunk_function thunk )
{
    yl_current->globals()->set_key( name, yl_gcnew< yl_thunkobj >( thunk ).get() );
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


