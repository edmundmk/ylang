//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <xec/xec_script.h>





int main( int argc, char* argv[] )
{
    xec_script script;
    const char* script_arguments[] = { "argv0", "..." };
    script.arguments( 2, script_arguments );
    script.parse( argv[ 1 ] );
    
    for ( size_t i = 0; i < script.diagnostic_count(); ++i )
    {
        fprintf( stderr, "%s\n", script.diagnostic( i ) );
    }
    
    return EXIT_SUCCESS;
}