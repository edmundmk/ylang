//
//  yl_compile.cpp
//
//  Created by Edmund Kapusniak on 04/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "ylang.h"
#include <stdarg.h>
#include "yl_diagnostics.h"
#include "yl_parser.h"
#include "yl_ast.h"
#include "yssa_builder.h"
#include "yssa_constfold.h"
#include "yssa_analyze.h"
#include "yssa_liveness.h"
#include "yssa_regalloc.h"
#include "yssa_codegen.h"



static yl_function yl_compile_failure( const yl_diagnostics& diagnostics );



yl_function yl_compile( const char* path, size_t paramc, const char* paramv[] )
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
//    yl_ast_print( ast );
    
    
    // Construct SSA form.
    yssa_builder builder( &diagnostics );
    if ( ! builder.build( ast ) )
    {
        return yl_compile_failure( diagnostics );
    }
    
    yssa_module* module = builder.get_module();
    for ( size_t i = 0; i < module->functions.size(); ++i )
    {
        yssa_function* function = module->functions.at( i ).get();
        
        yssa_constfold( module, function );
        yssa_analyze_depth_first_order( function );
        yssa_analyze_loop_forest( function );
        yssa_linearize( function );
        yssa_liveness( module, function );
        yssa_regalloc( module, function );
    }
        
//    yssa_print( module );
    
    yl_function function = yssa_codegen( module );
    return function;
}



yl_function yl_compile_failure( const yl_diagnostics& diagnostics )
{
    for ( size_t i = 0; i < diagnostics.error_count(); ++i )
    {
        fprintf( stderr, "%s\n", diagnostics.error( i ) );
    }

    return yl_function();
}



