//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <strpath.h>
#include <yl/yl_script.h>
#include <yl/yl_ast.h>
#include <yl/yl_srcbuf.h>
#include <ymodel/ymodel.h>
#include <ymodel/yarray.h>
#include <ymodel/ymodule.h>
#include <ymodel/yfunction.h>



void y_atoi( yframe& frame )
{
    double n = 0;
    if ( frame.size() >= 3 )
        n = frame[ 2 ].as_number();
    if ( ! frame[ 1 ].is_null() )
    {
        const char* a = frame[ 1 ].c_str();
        char* endp = nullptr;
        long i = strtol( a, &endp, 10 );
        if ( endp[ 0 ] == '\0' )
            n = i;
    }
    frame.reset();
    frame.push( n );
}

void y_printf( yframe& frame )
{
    const char* format = frame[ 1 ].c_str();
    if ( frame.size() == 2 )
    {
        printf( "%s", format );
    }
    else if ( frame.size() == 3 )
    {
        double n = frame[ 2 ].as_number();
        printf( format, n );
    }
    else if ( frame.size() == 4 )
    {
        double n = frame[ 2 ].as_number();
        double m = frame[ 3 ].as_number();
        printf( format, n, m );
    }
    frame.reset();
}

void y_sqrt( yframe& frame )
{
    double n = frame[ 1 ].as_number();
    frame.reset();
    frame.push( sqrt( n ) );
}

void y_max( yframe& frame )
{
    double a = frame[ 1 ].as_number();
    double b = frame[ 2 ].as_number();
    frame.reset();
    frame.push( std::max( a, b ) );
}


int main( int argc, char* argv[] )
{
    if ( argc <= 1 )
    {
        printf
        (
            "usage: %s script.yl [arguments ...]\n",
            argc ? path_bare_filename( argv[ 0 ] ).c_str() : "yl"
        );
        return EXIT_FAILURE;
    }

    const char* sargv[] = { "argv0", "..." };
    std::unique_ptr< yl_script > script( yl_parse( argv[ 1 ], 2, sargv ) );
    if ( script->error_count() )
    {
        for ( size_t i = 0; i < script->error_count(); ++i )
        {
            fprintf( stderr, "%s\n", script->error( i ) );
        }
        return EXIT_FAILURE;
    }


/*
    std::unique_ptr< yl_srcbuf > srcbuf( yl_compile_lj( script->get_ast() ) );
    if ( srcbuf )
    {
        printf( "%s", srcbuf->c_str() );
    }
*/

    ymodel model;
    yscope scope( &model );
    
    ymodule* module = yl_compile( script->get_ast() );
    if ( ! module )
    {
        return EXIT_FAILURE;
    }
    

    ymodule::print( module );


    yexpand* global = model.make_global();
    global->setkey( "atoi", y_atoi );
    global->setkey( "printf", y_printf );
    global->setkey( "sqrt", y_sqrt );
    global->setkey( "max", y_max );


    yfunction* f = yfunction::alloc( global, module->script() );


    yframe frame;
    frame.push( f );
    for ( int argi = 1; argi < argc; ++argi )
    {
        frame.push( argv[ argi ] );
    }
    yinvoke( frame );
    yvalue result = frame[ 0 ];

    
    return result.is_null() ? EXIT_SUCCESS : (int)result.as_number();
}



