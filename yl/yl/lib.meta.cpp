//
//  lib.meta.cpp
//
//  Created by Edmund Kapusniak on 04/05/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "lib.meta.h"
#include "lib.h"




static void atoi_thunk( yl_callframe& xf )
{
    const char* a0 = xf.get_string( 1 );
    int result = lib::atoi( a0 );
    xf.clear();
    xf.push( result );
}


static void max_thunk( yl_callframe& xf )
{
    double a0 = xf.get_number( 1 );
    double a1 = xf.get_number( 2 );
    double result = lib::max( a0, a1 );
    xf.clear();
    xf.push( result );
}


static void sqrt_thunk( yl_callframe& xf )
{
    double a0 = xf.get_number( 1 );
    double result = lib::sqrt( a0 );
    xf.clear();
    xf.push( result );
}



void lib_expose( yl_context* context )
{
    yl_scope ylscope( context );
    _yl_global_function( "atoi", &atoi_thunk );
    _yl_global_function( "max", &max_thunk );
    _yl_global_function( "printf", &lib::printf );
    _yl_global_function( "sqrt", &sqrt_thunk );
}


