//
//  xec_srcbuf.cpp
//
//  Created by Edmund Kapusniak on 11/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_srcbuf.h"
#include <stdlib.h>
#include <stdio.h>



xec_srcbuf::xec_srcbuf()
    :   bcapacity( 0 )
    ,   bsize( 0 )
    ,   btext( NULL )
{
}

xec_srcbuf::~xec_srcbuf()
{
    free( btext );
}



size_t xec_srcbuf::size() const
{
    return bsize;
}

const char* xec_srcbuf::c_str() const
{
    return btext ? btext : "";
}

void xec_srcbuf::printf( const char* format, ... )
{
    va_list arguments;
    va_start( arguments, format );
    vprintf( format, arguments );
    va_end( arguments );
}

void xec_srcbuf::vprintf( const char* format, va_list arguments )
{
    // Find length of string to append.
    va_list a;
    va_copy( a, arguments );
    int length = vsnprintf( NULL, 0, format, a );
    va_end( a );
    
    // Ensure we have space.
    if ( bsize + length >= bcapacity )
    {
        bcapacity = bcapacity ? bcapacity * 2 : 64;
        btext = (char*)realloc( btext, bcapacity );
    }
    
    // Print.
    bsize += vsnprintf( btext + bsize, bcapacity - bsize, format, arguments );
}

