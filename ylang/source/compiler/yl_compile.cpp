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
#include "yssa_builder.h"



yl_invoke yl_compile( const char* path )
{
    yl_diagnostics diagnostics;

    // Parse the source file.
    yl_parser parser( &diagnostics );
    if ( parser.parse( path ) )
    {
        yl_ast_p ast = parser.get_ast();
        yl_ast_print( ast.get() );
        
        // Construct SSA form.
        yssa_builder builder( &diagnostics );
        builder.build( ast.get() );

    }
    
    
    // Report errors.
    for ( size_t i = 0; i < diagnostics.error_count(); ++i )
    {
        fprintf( stderr, "%s\n", diagnostics.error( i ) );
    }

    return yl_invoke();
}
