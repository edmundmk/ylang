//
//  yl_compile.cpp
//
//  Created by Edmund Kapusniak on 04/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_compile.h"
#include "yl_diagnostics.h"
#include "yl_parser.h"
#include "yl_ast.h"



yl_invoke yl_compile( const char* path )
{
    yl_diagnostics diagnostics;
    yl_parser parser( &diagnostics );
    if ( parser.parse( path ) )
    {
        std::unique_ptr< yl_ast > ast = parser.get_ast();
        yl_ast_print( ast.get() );
    }
    
    for ( size_t i = 0; i < diagnostics.error_count(); ++i )
    {
        fprintf( stderr, "%s\n", diagnostics.error( i ) );
    }

    return yl_invoke();
}
