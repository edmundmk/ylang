//
//  xec_module_print.cpp
//
//  Created by Edmund Kapusniak on 28/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_module.h"
#include <stdio.h>



static void xec_function_print( xec_function* function )
{
    printf( "%s\n", function->name() );
    printf( "    param_count  : %u\n", function->param_count() );
    printf( "    upval_count  : %u\n", function->upval_count() );
    printf( "    newup_count  : %u\n", function->newup_count() );
    printf( "    stack_count  : %u\n", function->stack_count() );
    printf( "    is_varargs   : %s\n",
                    function->is_varargs() ? "true" : "false" );
    printf( "    is_coroutine : %s\n",
                    function->is_coroutine() ? "true" : "false" );
    printf( "\n" );
    
    for ( unsigned i = 0; i < function->code_count(); ++i )
    {
        xec_opinst code = function->code( i );
    }
}




void xec_module_print( xec_module* module )
{
    printf( "%s\n\n", module->name() );
    
    for ( unsigned i = 0; i < module->function_count(); ++i )
    {
        xec_function_print( module->function( i ) );
    }
}
