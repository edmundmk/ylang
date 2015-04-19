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
    return alloc( string, (unsigned)strlen( string ) );
}

yl_string* yl_string::alloc( const char* string, unsigned length )
{
    void* p = yl_current->malloc( sizeof( yl_string ) + length + 1 );
    yl_string* s = new ( p ) yl_string( length );
    memcpy( s->_s, string, length );
    s->_s[ length ] = '\0';
    return s;
}



yl_string::yl_string( unsigned length )
    :   yl_heapobj( YLOBJ_STRING )
    ,   _is_symbol( false )
    ,   _has_hash( false )
    ,   _hash( 0 )
    ,   _length( (uint32_t)length )
{
}



