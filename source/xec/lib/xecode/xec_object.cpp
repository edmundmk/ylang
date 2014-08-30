//
//  xec_object.cpp
//
//  Created by Edmund Kapusniak on 29/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_object.h"
#include <stdlib.h>
#include "xec_code.h"



xec_objkey xec_objkey::create( const char* key )
{
    // TODO.
    return xec_objkey();
}

xec_objkey xec_objkey::create( const char* key, size_t size )
{
    // TODO.
    return xec_objkey();
}

void xec_objkey::destroy( xec_objkey key )
{
}


const char* xec_objkey::c_str() const
{
    return NULL;
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








