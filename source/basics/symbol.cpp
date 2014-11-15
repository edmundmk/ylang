//
// symbol.cpp
//
// Created 8th October 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
// 


#include "symbol.h"
#include <string.h>
/*

symbol::symbol()
    :   shash( 0 )
    ,   sname( NULL )
{
}

symbol::symbol( symbol&& s )
    :   shash( s.shash )
    ,   sname( s.sname )
{
    s.sname = NULL;
}

symbol::symbol( const symbol& s )
    :   shash( s.shash )
    ,   sname( strdup( s.sname ) )
{
}

symbol::symbol( const char* s )
    :   shash( hash32( s ) )
    ,   sname( strdup( s ) )
{
}

symbol::symbol( const std::string& s )
    :   shash( hash32( s.c_str(), s.size() ) )
    ,   sname( strdup( s.c_str() ) )
{
}

symbol& symbol::operator = ( symbol&& s )
{
    shash = s.shash;
    free( sname );
    sname = s.sname;
    s.sname = NULL;
    return *this;
}

symbol& symbol::operator = ( const symbol& s )
{
    shash = s.shash;
    free( sname );
    sname = s.sname ? strdup( s.sname ) : NULL;
    return *this;
}

symbol& symbol::operator = ( const char* s )
{
    shash = hash32( s );
    free( sname );
    sname = strdup( s );
    return *this;
}

symbol& symbol::operator = ( const std::string& s )
{
    shash = hash32( s.c_str(), s.size() );
    free( sname );
    sname = strdup( s.c_str() );
    return *this;
}

symbol::~symbol()
{
    free( sname );
}
*/



