//
//  xssa_cfganalysis.h
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_CFGANALYSIS_H
#define XSSA_CFGANALYSIS_H


struct xssa_func;



/*
    Find a depth first ordering of blocks in the control flow graph. Since
    our CFG is reducible, this ordering ensures that each block comes after
    all of its dominators.
*/

void xssa_order_depth_first( xssa_func* func );



/*
    Compute a loop nesting forest for a CFG.  This identifies loop headers
    and classifies blocks by the loop they belong to.  Note that we assume
    a reducible CFG in depth-first order.
*/

void xssa_compute_loop_forest( xssa_func* func );



#endif



