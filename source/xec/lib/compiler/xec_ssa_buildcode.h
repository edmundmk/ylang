//
//  xec_ssa_buildcode.h
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_BUILDCODE_H
#define XEC_SSA_BUILDCODE_H


#include <vector>
#include <unordered_map>
#include "omodel.h"
#include "xec_ssa.h"


/*
    Translate out of SSA form (which has had a register allocation pass)
    and into xecode.  By now it should be fairly straightforward - the
    only remaining issue is resolving the parallel copies at phi and
    select ops.
*/


class xec_ssa_dfo;
struct xec_ssa_rtgraph;
class xec_module;
class xec_function;


class xec_ssa_buildcode
{
public:

    explicit xec_ssa_buildcode( xec_ssa* root );
    
    void build_func( xec_ssa_func* func, xec_ssa_dfo* dfo );
    xec_module* build_module();


private:

    struct jumplabel
    {
        int                 label;
        std::vector< int >  jumps;
    };

    void build_ops( xec_ssa_block* block );
    void build_cfg( xec_ssa_block* block );
    
    int k( int immkey );
    int o( xec_ssa_opref operand );
    int o( int r );

    void stack( int r, int b );
    
    void inst( xec_opcode opcode, int r, int a, int b );
    void inst( xec_opcode opcode, int r, int c );

    void call( xec_opcode opcode, xec_ssa_slice* slice, int index );
    void arguments( xec_ssa_op* op );
    void select( xec_ssa_slice* slice, size_t index );
    void move( xec_ssa_rtgraph* rtg );
    
    void phi( xec_ssa_rtgraph* rtg, xec_ssa_block* from, xec_ssa_block* to );
    
    void label( void* label );
    void jump( xec_opcode opcode, int r, void* label );
    void branch( xec_ssa_opref condition, bool iftrue, void* label );
    

    xec_ssa* root;
    xec_ssa_func* func;
    xec_ssa_dfo* dfo;

    std::vector< xec_value > values;
    std::vector< xec_objkey > keys;
    std::vector< xec_function* > functions;
    
    std::unordered_map< void*, jumplabel > labels;
    std::vector< xec_opinst > code;
    int maxstack;

};



#endif


