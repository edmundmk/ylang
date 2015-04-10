//
//  yl_string.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_string.h"
#include <string.h>
#include <new>



yl_string* yl_string::alloc( const char* string )
{
    return alloc( string, strlen( string ) );
}

yl_string* yl_string::alloc( const char* string, size_t size )
{
    void* p = yl_heap_current->malloc( sizeof( yl_string ) + size + 1 );
    yl_string* s = new ( p ) yl_string( size );
    memcpy( s->_s, string, size );
    s->_s[ size ] = '\0';
    return s;
}



yl_string::yl_string( size_t size )
    :   yl_heapobj( YLOBJ_STRING )
    ,   _is_symbol( false )
    ,   _has_hash( false )
    ,   _hash( 0 )
    ,   _size( (uint32_t)size )
{
}



