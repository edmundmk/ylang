//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <xec/parser/xec_script.h>
#include <xec/parser/xec_ast.h>
#include <xec/parser/xec_srcbuf.h>
#include <xec/ymodel/ymodel.h>
#include <xec/ymodel/yarray.h>
#include <xec/ymodel/ymodule.h>
#include <xec/ymodel/ythunk.h>
#include <xec/ymodel/yfunction.h>



void y_atoi( yframe frame )
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

void y_printf( yframe frame )
{
    const char* format = frame[ 0 ].c_str();
    if ( frame.count() == 2 )
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

void y_sqrt( yframe frame )
{
    double n = frame[ 0 ].as_number();
    frame.result( sqrt( n ) );
}

void y_max( yframe frame )
{
    double a = frame[ 0 ].as_number();
    double b = frame[ 1 ].as_number();
    frame.result( std::max( a, b ) );
}


int main( int argc, char* argv[] )
{
    const char* sargv[] = { "argv0", "..." };
    std::unique_ptr< xec_script > script( xec_parse( argv[ 1 ], 2, sargv ) );
    if ( script->error_count() )
    {
        for ( size_t i = 0; i < script->error_count(); ++i )
        {
            fprintf( stderr, "%s\n", script->error( i ) );
        }
        return EXIT_FAILURE;
    }


    yvalue y( 1 );
    yvalue m( nullptr );


/*
    std::unique_ptr< xec_srcbuf > srcbuf( xec_compile_lj( script->get_ast() ) );
    if ( srcbuf )
    {
        printf( "%s", srcbuf->c_str() );
    }
*/

    ymodel model;
    yscope scope( &model );
    
    ymodule* module = xec_compile( script->get_ast() );
    if ( ! module )
    {
        return EXIT_FAILURE;
    }
    

    ymodule::print( module );


    yexpand* global = yexpand::alloc();
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