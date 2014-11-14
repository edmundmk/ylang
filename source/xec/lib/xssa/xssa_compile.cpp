//
//  xssa_compile.cpp
//
//  Created by Edmund Kapusniak on 28/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast.h"
#include "xssa_builder.h"
#include "xssa_cfganalysis.h"
#include "xssa_linear.h"
#include "xssa_liveness.h"
#include "xssa_regalloc.h"
#include "xssa_ycodeout.h"


ymodule* xec_compile( xec_ast* ast )
{
    xssa_module ssam;
    xssa_builder ssab( &ssam );
    ssab.build( ast );
    
    xssa_ycodeout ycodeout;
    
    for ( size_t i = 0; i < ssam.funcs.size(); ++i )
    {
        xssa_func* func = ssam.funcs.at( i ).get();
        
//        xssa_print( func );
        
        xssa_order_depth_first( func );
//        xssa_print( func );
        
        xssa_compute_loop_forest( func );
//        xssa_print( func );
        
        xssa_liveness( func );
//        xssa_print( func );

        xssa_linear linear;
        xssa_build_linear( &linear, func );
//        xssa_print( &linear );
        
        xssa_regalloc( &linear );
//        xssa_print( &linear );
  
        ycodeout.build_func( &linear );
        
    }
    
    return ycodeout.build_module( ast->script );
}




