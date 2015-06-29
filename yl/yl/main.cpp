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
    // Set up ylang context.
    yl_context ylcontext;
    yl_scope ylscope( &ylcontext );
    lib_expose( &ylcontext );
    
    // Compile ylang function.
    yl_function function = yl_compile( argv[ 1 ], "argv0", "..." );
    if ( ! function )
    {
        return EXIT_FAILURE;
    }
    
    // Execute it.
    yl_callframe callframe;
    callframe.push( function );
    for ( int i = 1; i < argc; ++i )
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



