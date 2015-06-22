//
//  yl_string.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_string.h"
#include <string.h>
#include <new>



yl_gctype yl_string::gctype =
{
    &yl_string::destroy,
    nullptr,
    nullptr,
};



yl_stackref< yl_string > yl_string::alloc( const char* string )
{
    return alloc( string, strlen( string ) );
}

yl_stackref< yl_string > yl_string::alloc( const char* string, size_t size )
{
    void* p = yl_current->allocate( sizeof( yl_string ) + size + 1 );
    yl_string* s = new ( p ) yl_string( size );
    memcpy( s->_s, string, size );
    s->_s[ size ] = '\0';
    return s;
}



yl_string* yl_string::concat( yl_string* a, yl_string* b )
{
    size_t size = a->size() + b->size();
    void* p = yl_current->allocate( sizeof( yl_string ) + size + 1 );
    yl_string* s = new ( p ) yl_string( size );
    memcpy( s->_s, a->c_str(), a->size() );
    memcpy( s->_s + a->size(), b->c_str(), b->size() );
    s->_s[ size ] = '\0';
    return s;
}



yl_string::yl_string( size_t size )
    :   yl_gcobject( YLOBJ_STRING, YL_GCFLAG_LEAF )
    ,   _hash( 0 )
    ,   _size( (uint32_t)size )
{
}


void yl_string::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_string* self = (yl_string*)object;
    if ( self->check_gcflags( SYMBOL ) )
    {
        yl_context_impl* context = (yl_context_impl*)heap;
        context->destroy_symbol( self );
    }
}




