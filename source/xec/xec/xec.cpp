//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <xec/xec_script.h>
#include <xec/xec_ast_print.h>




int main( int argc, char* argv[] )
{
    xec_script script;

    const char* script_arguments[] = { "argv0", "..." };
    script.parameters( 2, script_arguments );

    if ( script.parse( argv[ 1 ] ) )
    {
        xec_ast_print( script.get_script() );
    }
    
    for ( size_t i = 0; i < script.diagnostic_count(); ++i )
    {
        fprintf( stderr, "%s\n", script.diagnostic( i ) );
    }
    
    
    return EXIT_SUCCESS;
}