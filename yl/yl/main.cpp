//
//  main.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <ylang.h>
#include "lib.meta.h"


int main( int argc, const char* argv[] )
{
    // Check arguments.
    unsigned debug = 0;
    int argi = 1;
    while ( true )
    {
        if ( strcmp( argv[ argi ], "--ast" ) == 0 )
        {
            debug |= YL_COMPILE_PRINT_AST;
            argi += 1;
        }
        else if ( strcmp( argv[ argi ], "--ssa" ) == 0 )
        {
            debug |= YL_COMPILE_PRINT_SSA;
            argi += 1;
        }
        else if ( strcmp( argv[ argi ], "--ops" ) == 0 )
        {
            debug |= YL_COMPILE_PRINT_OPS;
            argi += 1;
        }
        else
        {
            break;
        }
    }

    // Set up ylang context.
    yl_context ylcontext;
    yl_scope ylscope( &ylcontext );
    lib_expose( &ylcontext );
    
    // Compile ylang function.
    yl_function function = yl_compile( debug, argv[ argi ], "argv0", "..." );
    if ( ! function )
    {
        return EXIT_FAILURE;
    }
    
    // Execute it.
    yl_callframe callframe;
    callframe.push( function );
    for ( int i = argi; i < argc; ++i )
    {
        callframe.push( argv[ i ] );
    }
    
    yl_invoke( callframe );
    
    if ( callframe.size() && callframe.at( 0 ) == YLVAL_NUMBER )
    {
        return (int)callframe.get_number( 0 );
    }

    return EXIT_SUCCESS;
}



