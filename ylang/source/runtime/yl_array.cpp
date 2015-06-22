//
//  yl_array.cpp
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_array.h"
#include "yl_function.h"



yl_stackref< yl_array > yl_array::alloc( size_t capacity )
{
    return alloc( yl_current->proto_array(), capacity );
}

yl_stackref< yl_array > yl_array::alloc( yl_object* prototype, size_t capacity )
{
    void* p = yl_current->allocate( sizeof( yl_array ) );
    return new ( p ) yl_array( yl_current->proto_array(), capacity );
}



yl_array::yl_array( yl_object* prototype, size_t capacity )
    :   yl_object( YLOBJ_ARRAY, prototype )
    ,   _elements( capacity ? yl_valarray::alloc( capacity ).get() : nullptr )
    ,   _length( 0 )
{
}


void yl_array::reserve( size_t capacity )
{
    capacity = std::max( capacity, _length );

    yl_valarray* elements = _elements.get();
    size_t size = elements ? elements->size() : 0;
    if ( capacity == size )
    {
        return;
    }
    
    yl_stackref< yl_valarray > newelems = yl_valarray::alloc( capacity );
    assert( elements || _length == 0 );
    for ( size_t i = 0; i < _length; ++i )
    {
        newelems->at( i ).set( elements->at( i ).get() );
    }
    
    _elements.set( newelems.get() );
}


void yl_array::resize( size_t length )
{
    if ( _length > length )
    {
        _length = length;
    }
    
    reserve( length );
    
    if ( _length < length )
    {
        yl_valarray* elements = _elements.get();
        
        for ( size_t i = _length; i < length; ++i )
        {
            elements->at( i ).set( yl_null );
        }
        
        _length = length;
    }
}



yl_stackref< yl_object > yl_array::make_prototype()
{
    yl_stackref< yl_object > proto = yl_object::alloc( yl_current->proto_object() );
    proto->set_key
    (
        yl_string::alloc( "length" )->symbol().get(),
        yl_value( YLOBJ_THUNKOBJ, yl_thunkobj::alloc( &thunk_length ).get() )
    );
    proto->set_key
    (
        yl_string::alloc( "resize" )->symbol().get(),
        yl_value( YLOBJ_THUNKOBJ, yl_thunkobj::alloc( &thunk_resize ).get() )
    );
    return proto;
}


yl_array* yl_array::thunk_this( yl_callframe& xf )
{
    yl_gcobject* gcobject = xf.get_gcobject( 1 );
    if ( ! gcobject || gcobject->kind() != YLOBJ_ARRAY )
    {
        throw yl_exception( "expected array" );
    }
    return (yl_array*)gcobject;
}

void yl_array::thunk_length( yl_callframe& xf )
{
    yl_array* array = thunk_this( xf );
    xf.clear();
    xf.push( array->length() );
}


void yl_array::thunk_resize( yl_callframe& xf )
{
    yl_array* array = thunk_this( xf );
    array->resize( xf.get_integer( 2 ) );
    xf.clear();
}



