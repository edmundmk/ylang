//
//  yssa_linear.h
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_LINEAR_H
#define XSSA_LINEAR_H


#include <vector>
#include <memory>


struct yssaop;
struct yssa_func;
struct yssa_block;
struct yssalop;




/*
    A linear ordering of all ops, including live spans for each op.
*/


struct yssa_linear
{
    yssa_func*              func;
    std::vector< yssalop >  lops;
};


enum yssa_lopkind : int
{
    XSSA_LOP_BEGIN,                 // start of block.
    XSSA_LOP_END,                   // end of block.
    XSSA_LOP_LIVE,                  // start of live range (without op).
    XSSA_LOP_OP,                    // actual op.
};


struct yssalop
{
    yssalop( yssa_lopkind kind, yssa_block* block );
    yssalop( yssa_lopkind kind, yssaop* op );


    int             live_head;      // index of op at the head of live chain.
    int             live_next;      // next op in live chain.
    int             live_until;     // op where this span dies.

    yssa_lopkind    kind;
    
    union
    {
    yssa_block*     block;
    yssaop*         op;
    };

};




void yssa_build_linear( yssa_linear* linear, yssa_func* func );
void yssa_print( yssa_linear* linear );



#endif
