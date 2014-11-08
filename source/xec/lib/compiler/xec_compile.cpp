//
//  xec_compile.cpp
//
//  Created by Edmund Kapusniak on 28/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast.h"
#include "xec_ssa_builder.h"
#include "xec_ssa_print.h"
#include "xec_ssa_cfganalysis.h"
#include "xec_opt_constfold.h"
#include "xec_ssa_liveness.h"
#include "xec_ssa_regalloc.h"
#include "xec_ssa_buildcode.h"



ymodule* xec_compile( xec_ast* ast )
{
    // Build SSA form.

    xec_ssa ssa;
    xec_ssa_builder builder( &ssa );
    builder.build( ast );
    

    // Perform optimizations.

    xec_ssa_dfo dfo( &ssa );
    xec_opt_constfold constfold( &ssa );

    for ( size_t i = 0; i < ssa.functions.size(); ++i )
    {
        xec_ssa_func* func = ssa.functions.at( i );
        dfo.build_ordering( func );
        constfold.optimize( func, &dfo );
    }


    // Calculate liveness, do register allocation, and build module

    xec_ssa_loop_forest loops( &ssa );
    xec_ssa_liveness liveness( &ssa );
    xec_ssa_regalloc regalloc( &ssa );
    xec_ssa_buildcode buildcode( &ssa );
    
    for ( size_t i = 0; i < ssa.functions.size(); ++i )
    {
        xec_ssa_func* func = ssa.functions.at( i );
        dfo.build_ordering( func );
        loops.build_forest( func, &dfo );
        liveness.analyze_func( func, &dfo, &loops );
        regalloc.allocate( func, &dfo );
        buildcode.build_func( func, &dfo );
    }


    xec_ssa_print( &ssa );

    
    return buildcode.build_module();
}


