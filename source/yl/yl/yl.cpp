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
#include <ymodel/ythunk.h>
#include <ymodel/yfunction.h>



void y_atoi( yframe& frame )
{
    double n = 0;
    if ( frame.count() >= 2 )
        n = frame[ 1 ].as_number();
    if ( ! frame[ 0 ].is_null() )
    {
        const char* a = frame[ 0 ].c_str();
        char* endp = nullptr;
        long i = strtol( a, &endp, 10 );
        if ( endp[ 0 ] == '\0' )
            n = i;
    }
    frame.result( n );
}

void y_printf( yframe& frame )
{
    const char* format = frame[ 0 ].c_str();
    if ( frame.count() == 1 )
    {
        printf( "%s", format );
    }
    else if ( frame.count() == 2 )
    {
        double n = frame[ 1 ].as_number();
        printf( format, n );
    }
    else if ( frame.count() == 3 )
    {
        double n = frame[ 1 ].as_number();
        double m = frame[ 2 ].as_number();
        printf( format, n, m );
    }
}

void y_sqrt( yframe& frame )
{
    double n = frame[ 0 ].as_number();
    frame.result( sqrt( n ) );
}

void y_max( yframe& frame )
{
    double a = frame[ 0 ].as_number();
    double b = frame[ 1 ].as_number();
    frame.result( std::max( a, b ) );
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


    yarray* args = yarray::alloc();
    for ( int argi = 2; argi < argc; ++argi )
    {
        args->append( ystring::alloc( argv[ argi ] ) );
    }
    global->setkey( "args", args );


    yfunction* f = yfunction::alloc( global, module->script() );
    yinvoke( f );
    


    return EXIT_SUCCESS;
}