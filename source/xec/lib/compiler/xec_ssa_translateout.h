//
//  xec_ssa_translateout.h
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_TRANSLATEOUT_H
#define XEC_SSA_TRANSLATEOUT_H


#include <vector>
#include <unordered_map>
#include "xec_code.h"
#include "xec_ssa.h"
#include "xec_value.h"


/*
    Translate out of SSA form (which has had a register allocation pass)
    and into xecode.  By now it should be fairly straightforward - the
    only remaining issue is resolving the parallel copies at phi and
    select ops.
*/


class xec_ssa_dfo;
struct xec_ssa_rtgraph;


class xec_ssa_translateout
{
public:

    explicit xec_ssa_translateout( xec_ssa* root );
    
    void translateout( xec_ssa_func* func, xec_ssa_dfo* dfo );
    void print();


private:

    void translateout( xec_ssa_block* block );
    
    int k( int immkey );
    int o( xec_ssa_opref operand );
    
    void inst( xec_opcode opcode, int r, int a, int b );
    void inst( xec_opcode opcode, int r, int c );

    void callinst( xec_opcode opcode, xec_ssa_slice* slice, int index );
    void arguments( xec_ssa_op* op );
    void select( xec_ssa_slice* slice, size_t index );
    void move( xec_ssa_rtgraph* rtg );

    xec_ssa* root;
    xec_ssa_func* func;
    xec_ssa_dfo* dfo;

    std::vector< xec_value > values;
    std::vector< xec_key* > keys;
    std::vector< xec_opinst > i;

};



#endif


