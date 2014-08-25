//
//  xec_code.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_code.h"
#include <new>
#include <string.h>


xec_key* xec_key::create( const char* key )
{
    size_t len = strlen( key );
    xec_key* k = new ( malloc( sizeof( key ) + len + 1 ) ) xec_key();
    strcpy( k->kkey, key );
    k->khash = hash32( key, len );
    return k;
}


void xec_key::destroy( xec_key* key )
{
    free( key );
}


xec_key::xec_key()
    :   khash( 0 )
{
}
