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
#include <xec/xecode/xec_code.h>




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

    xec_ast_print( script->get_ast() );

/*
    std::unique_ptr< xec_module > module( xec_compile( script->get_ast() ) );
    if ( ! module )
    {
        return EXIT_FAILURE;
    }

    xec_module_print( module.get() );
*/

    xec_compile_js( script->get_ast() );


    return EXIT_SUCCESS;
}