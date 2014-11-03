//
//  ostring.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ostring.h"


ometatype ostring::metatype = { &mark_string };


void ostring::mark_string( oworklist* work, obase* object, ocolour colour )
{
    assert( ! "ostring added to work queue" );
}



/*

#include <algorithm>
#include "omodel_impl.h"



oimpl::ogctype ostring::gctype =
{
    nullptr,
    nullptr,
    nullptr,
    "string"
};



ostring* ostring::alloc( const char* string )
{
    ostring* result = alloc( strlen( string ) );
    memcpy( result->buffer(), string, result->size() );
    return result;
}


ostring* ostring::alloc( size_t size )
{
    ostring* s = oimpl::context->model->alloc< ostring >(
            oimpl::context, &gctype, sizeof( ostring ) + size + 1 );
    s->shash    = 0;
    s->ssize    = (uint32_t)size;
    s->shashed  = false;
    s->ssymbol  = false;
    s->sdata[ size ] = '\0';
    return s;
}


ostring* ostring::strcat( ostring* a, ostring* b )
{
    ostring* result = alloc( a->size() + b->size() );
    memcpy( result->buffer(), a->data(), a->size() );
    memcpy( result->buffer() + a->size(), b->data(), b->size() );
    assert( result->data()[ a->size() + b->size() ] == '\0' );
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




osymbol::osymbol( const char* s )
{
    string = oimpl::context->model->symbol( oimpl::context, s );
}

osymbol::osymbol( ostring* s )
{
    if ( s->ssymbol )
        string = s;
    else
        string = oimpl::context->model->symbol( oimpl::context, s );
}




*/



