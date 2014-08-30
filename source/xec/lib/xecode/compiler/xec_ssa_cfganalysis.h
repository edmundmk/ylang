//
//  xec_ssa_cfganalysis.h
//
//  Created by Edmund Kapusniak on 19/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_CFGANALYSIS_H
#define XEC_SSA_CFGANALYSIS_H


#include <vector>
#include <unordered_set>
#include <unordered_map>


struct xec_ssa;
struct xec_ssa_func;
struct xec_ssa_block;


/*
    Find a depth first ordering of blocks in the control flow graph.  Note
    that the original 'program order' of blocks could serve as a valid
    ordering as only loops have back-references to a previous block.
*/

class xec_ssa_dfo
{
public:

    explicit xec_ssa_dfo( xec_ssa* root );

    void            build_ordering( xec_ssa_func* func );
    
    size_t          size();
    xec_ssa_block*  at( size_t index );
    
private:

    void visit( xec_ssa_block* block, int* index );

    xec_ssa* root;
    std::vector< xec_ssa_block* > ordering;
    
};




/*
    Compute a loop nesting forest for a CFG.  This identifies loop headers
    and classifies blocks by the loop they belong to.  Note that we assume
    a reducible CFG.
*/

struct xec_ssa_loop
{
    xec_ssa_loop( xec_ssa_block* header );

    xec_ssa_loop*   outer;
    xec_ssa_block*  header;
    std::unordered_set< xec_ssa_loop* > loops;
    std::unordered_set< xec_ssa_block* > blocks;
    int             last;
};


class xec_ssa_loop_forest
{
public:

    explicit xec_ssa_loop_forest( xec_ssa* root );
    
    void build_forest( xec_ssa_func* func, xec_ssa_dfo* dfo );
    void print();
    
    xec_ssa_loop* loopof( xec_ssa_block* block );
    bool inloop( xec_ssa_block* block, xec_ssa_loop* loop );
    
    
private:

    void print_node( xec_ssa_loop* node, int indent );

    xec_ssa* root;
    std::vector< std::unique_ptr< xec_ssa_loop > > loops;
    std::unordered_map< xec_ssa_block*, xec_ssa_loop* > loopmap;

};




#endif



