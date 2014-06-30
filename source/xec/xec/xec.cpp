//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <xec/xec_parser.h>
#include <xec/ast/xec_astprinter.h>
#include <xec/ast/xec_scope.h>





int main( int argc, char* argv[] )
{
    xec_parser parser;
    const char* script_arguments[] = { "argv0", "..." };
    parser.set_arguments( 2, script_arguments );
    parser.parse( argv[ 1 ] );
    
    for ( size_t i = 0; i < parser.diagnostic_count(); ++i )
    {
        fprintf( stderr, "%s\n", parser.diagnostic( i ) );
    }
    
    xec_astprinter printer;
    printer.print( parser.get_script() );
    parser.get_global_scope()->print();
    
    return EXIT_SUCCESS;
}