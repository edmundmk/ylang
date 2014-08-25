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
#include <xec/parser/xec_ast_print.h>




int main( int argc, char* argv[] )
{
    xec_script  script;
    xec_ast     ast;

    const char* sargv[] = { "argv0", "..." };
    if ( ! xec_parse( &script, &ast, argv[ 1 ], 2, sargv ) )
        goto error;
    
    xec_ast_print( &ast );
    
    if ( ! xec_ssabuild( &ast ) )
        goto error;
    
    return EXIT_SUCCESS;
    
error:

    for ( size_t i = 0; i < script.error_count(); ++i )
    {
        fprintf( stderr, "%s\n", script.error( i ) );
    }
    
    return EXIT_FAILURE;
}