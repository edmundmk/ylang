//
//  ystring.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ystring.h"


ymetatype ystring::metatype = { nullptr, "string" };



ystring* ystring::alloc( size_t size )
{
    void* p = malloc( sizeof( ystring ) + size + 1 );
    return new ( p ) ystring( &metatype, size );
}

ystring* ystring::alloc( size_t size, const char* data )
{
    ystring* s = alloc( size );
    memcpy( s->buffer(), data, s->size() );
    return s;
}

ystring* ystring::alloc( const char* string )
{
    return alloc( strlen( string ), string );
}



ystring::ystring( ymetatype* metatype, size_t size )
    :   yobject( metatype )
    ,   shash( 0 )
    ,   ssize( size )
    ,   shashed( false )
    ,   ssymbol( false )
{
    sdata[ ssize ] = '\0';
}


ystring* ystring::strcat( ystring* a, ystring* b )
{
    ystring* result = alloc( a->size() + b->size() );
    memcpy( result->buffer(), a->data(), a->size() );
    memcpy( result->buffer() + a->size(), b->data(), b->size() );
    return result;
}


int ystring::strcmp( ystring* a, ystring* b )
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



