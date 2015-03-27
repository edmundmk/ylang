//
//  yssa_analyze.h
//
//  Created by Edmund Kapusniak on 27/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_ANALYZE_H
#define YSSA_ANALYZE_H


struct yssa_function;



/*
    Find a depth first ordering of blocks in the control flow graph. Since
    our CFG is reducible, this ordering ensures that each block comes after
    all of its dominators.
*/

void yssa_analyze_depth_first_order( yssa_function* function );



/*
    Compute a loop nesting forest for a CFG.  This identifies loop headers
    and classifies blocks by the loop they belong to.  Note that we assume
    a reducible CFG in depth-first order.
*/

void yssa_analyze_loop_forest( yssa_function* function );



/*
    Calculate inter-block liveness (the in set and the out set).  Values
    that are local to a particular block are easy to deal with.
*/

void yssa_analyze_live_in_out( yssa_function* function );




#endif



