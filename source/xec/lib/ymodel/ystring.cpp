//
//  ystring.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ystring.h"


ometatype ostring::metatype = { nullptr, "string" };


ostring* ostring::alloc( const char* string )
{
    ostring* s = alloc( strlen( string ) );
    memcpy( s->buffer(), string, s->size() );
    return s;
}


ostring* ostring::alloc( size_t size )
{
    void* p = malloc( sizeof( ostring ) + size + 1 );
    return new ( p ) ostring( &metatype, size );
}


ostring::ostring( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   shash( 0 )
    ,   ssize( size )
    ,   shashed( false )
    ,   ssymbol( false )
{
    sdata[ ssize ] = '\0';
}


ostring* ostring::strcat( ostring* a, ostring* b )
{
    ostring* result = alloc( a->size() + b->size() );
    memcpy( result->buffer(), a->data(), a->size() );
    memcpy( result->buffer() + a->size(), b->data(), b->size() );
    return result;
}


int ostring::strcmp( ostring* a, ostring* b )
{
    if ( a == b )
        return 0;
    size_t size = std::min( a->size(), b->size() );
    int result = memcmp( a->data(), b->data(), size );
    if ( result != 0 )
        return result;
    else if ( a->size() < b->size() )
        return -1;
    else if ( a->size() > b->size() )
        return 1;
    else
        return 0;
}



