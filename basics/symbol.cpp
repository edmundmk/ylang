//
// symbol.cpp
//
// Created 8th October 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
// 


#include "symbol.h"
#include <string.h>


symbol::symbol()
    :   hash( 0 )
    ,   name( NULL )
{
}

symbol::symbol( symbol&& s )
    :   hash( s.hash )
    ,   name( s.name )
{
    s.name = NULL;
}

symbol::symbol( const symbol& s )
    :   hash( s.hash )
    ,   name( strdup( s.name ) )
{
}

symbol::symbol( const char* s )
    :   hash( ::hash( s ) )
    ,   name( strdup( s ) )
{
}

symbol::symbol( const std::string& s )
    :   hash( ::hash( s.c_str(), s.size() ) )
    ,   name( strdup( s.c_str() )
{
}

symbol::symbol& operator = ( symbol&& s )
{
    hash = s.hash;
    free( name );
    name = s.name;
    s.name = NULL;
}

symbol::symbol& operator = ( const symbol& s )
{
    hash = s.hash;
    free( name );
    name = s.name ? strdup( s.name ) : NULL;
}

symbol::symbol& operator = ( const char* s )
{
    hash = ::hash( s );
    free( name );
    name = strdup( s );
}

symbol::symbol& operator = ( const std::string& s )
{
    hash = ::hash( s.c_str(), s.size() );
    free( name );
    name = strdup( s.c_str() );
}

symbol::~symbol()
{
    free( name );
}


symbol::operator bool () const
{
    return name != NULL;
}

symbol::operator std::string () const
{
    return std::string( name ? name : "" );
}

const char* symbol::c_str() const
{
    return name ? name : "";
}

hash_t symbol::hash()
{
    return hash;
}
