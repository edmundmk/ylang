//
//  xec_object.cpp
//
//  Created by Edmund Kapusniak on 29/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_object.h"
#include <unordered_map>
#include <stdlib.h>
#include <symbol.h>
#include "xec_code.h"



struct xec_objkey_key
{
    hash32_t    khash;
    unsigned    ksize;
    char        kkey[];
};

static std::unordered_map< symkey, xec_objkey_key* > objkeys;


xec_objkey xec_objkey::create( const char* key )
{
    return create( key, strlen( key ) );
}

xec_objkey xec_objkey::create( const char* key, size_t size )
{
    hash32_t hash = hash32( key, size );

    auto i = objkeys.find( symkey( hash, key, size ) );
    if ( i != objkeys.end() )
    {
        return xec_objkey( i->second );
    }
    
    xec_objkey_key* k = (xec_objkey_key*)malloc(
                    sizeof( xec_objkey_key ) + size + 1 );
    k->khash = hash;
    k->ksize = (unsigned)size;
    memcpy( k->kkey, key, size );
    k->kkey[ size ] = '\0';
    objkeys.emplace( symkey( k->khash, k->kkey, k->ksize ), k );
    
    return xec_objkey( k );
}

void xec_objkey::destroy( xec_objkey key )
{
    // They live forever at the moment...
}


const char* xec_objkey::c_str() const
{
    return ( (xec_objkey_key*)k )->kkey;
}

xec_objkey::xec_objkey( void* k )
    :   k( k )
{
}







inline xec_upval::xec_upval( xec_value value )
    :   refcount( 1 )
    ,   mvalue( value )
{
}

inline void xec_upval::incref()
{
    refcount += 1;
}

inline void xec_upval::decref()
{
    refcount -= 1;
    if ( refcount == 0 )
    {
        delete this;
    }
}



xec_object::xec_object()
    :   refcount( 1 )
    ,   mkind( OBJECT )
{
}

xec_object::xec_object( kind kind )
    :   refcount( 1 )
    ,   mkind( kind )
{
}



xec_closure::xec_closure( xec_function* function )
    :   xec_object( CLOSURE )
    ,   mfunction( function )
    ,   mupvals( NULL )
{
    unsigned upvalcount = function->upval_count();
    mupvals = (xec_upval**)malloc( sizeof( xec_upval* ) * upvalcount );
}

xec_closure::~xec_closure()
{
    for ( unsigned i = 0; i < mfunction->upval_count(); ++i )
    {
        mupvals[ i ]->decref();
    }
    
    free( mupvals );
}


void xec_closure::envup( unsigned i, xec_upval* upval )
{
    mupvals[ i ] = upval;
    upval->incref();
}








