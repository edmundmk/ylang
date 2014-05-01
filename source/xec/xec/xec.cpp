//
//  main.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <xec/xec_parser.h>
#include <xec/ast/xec_astvisitor.h>



class xec_astprint : public xec_astvisitor< xec_astprint, void >
{
};



int main( int argc, char* argv[] )
{
    xec_parser parser;
    parser.parse( argv[ 1 ] );
    
    xec_astprint x;
    x.visit( (xec_declaration*)nullptr );
    x.visit( (xec_expression*)nullptr );
    x.visit( (xec_statement*)nullptr );
    
    for ( size_t i = 0; i < parser.diagnostic_count(); ++i )
    {
        fprintf( stderr, "%s\n", parser.diagnostic( i ) );
    }
    
    return EXIT_SUCCESS;
}