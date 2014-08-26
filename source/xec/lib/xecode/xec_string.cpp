//
//  xec_string.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_string.h"
#include <string.h>
#include <stdlib.h>
#include <new>


xec_string* xec_string::create( const char* s )
{
    return create( s, strlen( s ) );
}

xec_string* xec_string::create( const char* s, size_t size )
{
    char* buffer;
    xec_string* result = create( size, &buffer );
    memcpy( buffer, s, size );
    return result;
}

xec_string* xec_string::create( size_t size, char** buffer )
{
    size_t totalsize = sizeof( xec_string ) + size + 1;
    xec_string* result = new ( malloc( totalsize ) ) xec_string();
    result->slength = (int)size;
    result->schars[ size ] = '\0';
    *buffer = result->schars;
    return result;
}


void xec_string::incref()
{
    refcount += 1;
}

void xec_string::decref()
{
    refcount -= 1;
    if ( refcount == 0 )
    {
        this->~xec_string();
        free( this );
    }
}


xec_string::xec_string()
    :   refcount( 1 )
    ,   slength( 0 )
{
}