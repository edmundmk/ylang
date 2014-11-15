//
//  yssa_compile.cpp
//
//  Created by Edmund Kapusniak on 28/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_ast.h"
#include "yssa_builder.h"
#include "yssa_cfganalysis.h"
#include "yssa_linear.h"
#include "yssa_liveness.h"
#include "yssa_regalloc.h"
#include "yssa_ycodeout.h"


ymodule* yl_compile( yl_ast* ast )
{
    yssa_module ssam;
    yssa_builder ssab( &ssam );
    ssab.build( ast );
    
    yssa_ycodeout ycodeout;
    
    for ( size_t i = 0; i < ssam.funcs.size(); ++i )
    {
        yssa_func* func = ssam.funcs.at( i ).get();
        
//        yssa_print( func );
        
        yssa_order_depth_first( func );
//        yssa_print( func );
        
        yssa_compute_loop_forest( func );
//        yssa_print( func );
        
        yssa_liveness( func );
//        yssa_print( func );

        yssa_linear linear;
        yssa_build_linear( &linear, func );
//        yssa_print( &linear );
        
        yssa_regalloc( &linear );
//        yssa_print( &linear );
  
        ycodeout.build_func( &linear );
        
    }
    
    return ycodeout.build_module( ast->script );
}




