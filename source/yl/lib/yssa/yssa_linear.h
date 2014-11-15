//
//  xssa_linear.h
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_LINEAR_H
#define XSSA_LINEAR_H


#include <vector>
#include <memory>


struct xssaop;
struct xssa_func;
struct xssa_block;
struct xssalop;




/*
    A linear ordering of all ops, including live spans for each op.
*/


struct xssa_linear
{
    xssa_func*              func;
    std::vector< xssalop >  lops;
};


enum xssa_lopkind : int
{
    XSSA_LOP_BEGIN,                 // start of block.
    XSSA_LOP_END,                   // end of block.
    XSSA_LOP_LIVE,                  // start of live range (without op).
    XSSA_LOP_OP,                    // actual op.
};


struct xssalop
{
    xssalop( xssa_lopkind kind, xssa_block* block );
    xssalop( xssa_lopkind kind, xssaop* op );


    int             live_head;      // index of op at the head of live chain.
    int             live_next;      // next op in live chain.
    int             live_until;     // op where this span dies.

    xssa_lopkind    kind;
    
    union
    {
    xssa_block*     block;
    xssaop*         op;
    };

};




void xssa_build_linear( xssa_linear* linear, xssa_func* func );
void xssa_print( xssa_linear* linear );



#endif
