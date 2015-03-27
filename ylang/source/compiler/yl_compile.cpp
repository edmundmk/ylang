//
//  yl_compile.cpp
//
//  Created by Edmund Kapusniak on 04/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_compile.h"
#include <stdarg.h>
#include "yl_diagnostics.h"
#include "yl_parser.h"
#include "yl_ast.h"
#include "yssa_builder.h"
#include "yssa_constfold.h"



static yl_invoke yl_compile_failure( const yl_diagnostics& diagnostics );



yl_invoke yl_compile( const char* path, size_t paramc, const char* paramv[] )
{
    yl_diagnostics diagnostics;

    // Parse the source file.
    yl_parser parser( &diagnostics );
    
    for ( size_t i = 0; i < paramc; ++i )
    {
        const char* param = paramv[ i ];
        if ( strcmp( param, "..." ) != 0 )
            parser.parameter( param );
        else
            parser.varargs();
    }
    
    if ( ! parser.parse( path ) )
    {
        return yl_compile_failure( diagnostics );
    }
    
    yl_ast* ast = parser.get_ast();
    yl_ast_print( ast );
    
    
    // Construct SSA form.
    yssa_builder builder( &diagnostics );
    if ( ! builder.build( ast ) )
    {
        return yl_compile_failure( diagnostics );
    }
    
    yssa_module* module = builder.get_module();
    yssa_constfold( module );
    yssa_print( module );


    return yl_invoke();
}



yl_invoke yl_compile_failure( const yl_diagnostics& diagnostics )
{
    for ( size_t i = 0; i < diagnostics.error_count(); ++i )
    {
        fprintf( stderr, "%s\n", diagnostics.error( i ) );
    }

    return yl_invoke();
}



