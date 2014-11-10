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
#include <xec/ymodel/ymodule.h>
#include <xec/ymodel/ythunk.h>
#include <xec/ymodel/yfunction.h>



void y_atoi( yframe frame )
{
    frame.result( 1000.0 );
}

void y_printf( yframe frame )
{
    const char* format = frame.argument( 0 ).c_str();
    double n = frame.argument( 1 ).as_number();
    printf( format, n );
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
    ymodule::print( module );


    yexpand* global = yexpand::alloc();
    global->setkey( "atoi", y_atoi );
    global->setkey( "printf", y_printf );
    yfunction* f = yfunction::alloc( global, module->script() );
    yinvoke( f );
    


    return EXIT_SUCCESS;
}