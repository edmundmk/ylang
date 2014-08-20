//
//  xec_ssa_liveness.h
//
//  Created by Edmund Kapusniak on 14/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_LIVENESS_H
#define XEC_SSA_LIVENESS_H


#include "xec_ssa.h"


/*
    Liveness information for programs in SSA form.  Live ranges are
    represented as ranges of instructions, possibly with holes.  Control
    flow is not relevant - it is assumed that a global ordering has been
    generated for the program before liveness analysis occurs, and branches
    in which a value is not used are simply represented as holes in the live
    range.
 
    Most SSA ops generate a value.  Liveness for this value is stored inline,
    as it is required for almost every op.  Each op has two members which
    store liveness information:
 
        [   live   |  lnext   ]
             |          |
             v          |
         ----------     |
                        |
                        v
        [   live   |  lnext   ]
             |
             v
         ----------
 
    'live' contains the index of the SSA op at which this value is dead.
    Values die just before this op.  'lnext' contains the index of an op
    that heads up the next sub-range - i.e. the op at which the value is
    resurrected.  Frequently this will be a synthetic op inserted at the
    head of the next block where the value is used.
    
*/


class xec_ssa_dfo;
class xec_ssa_loop_forest;
struct xec_ssa_loop;


class xec_ssa_liveness
{
public:

    explicit xec_ssa_liveness( xec_ssa* root );
    
    void analyze_func( xec_ssa_func* func,
                xec_ssa_dfo* dfo, xec_ssa_loop_forest* loops );

private:

    struct livespan
    {
        livespan( xec_ssa_opref value,
                        xec_ssa_opref until, xec_ssa_opref lnext );
    
        xec_ssa_opref value;
        xec_ssa_opref until;
        xec_ssa_opref lnext;
    };

    typedef std::unordered_map< xec_ssa_opref, livespan > liveset;

    void        analyze_block( xec_ssa_block* block );
    void        add_successor( liveset* live,
                        xec_ssa_block* block, xec_ssa_block* successor );
    xec_ssa_op* local_def( xec_ssa_block* block, xec_ssa_opref value );
    void        live_loop( livespan* span, xec_ssa_loop* loop );
    xec_ssa_block* blockof( xec_ssa_opref opref );
    

    xec_ssa_func*        func;
    xec_ssa_dfo*         dfo;
    xec_ssa_loop_forest* loops;
    std::unordered_map< xec_ssa_block*, liveset > livein;

    xec_ssa* root;

};




#endif
