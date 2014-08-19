//
//  xec_ssa_liveness.cpp
//
//  Created by Edmund Kapusniak on 17/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_liveness.h"
#include <make_unique>
#include "xec_ssa.h"
#include "xec_ssa_print.h"
#include "xec_ssa_cfganalysis.h"


/*
    
    Note that the CFG for xec programs is always reducible, which makes things
    massively easier.  A great many algorithms are simpler on reducible CFGs,
    not least the definition of loops.
    
    Useful papers about control flow graphs and liveness analysis:

    http://www.christianwimmer.at/Publications/Wimmer10a/Wimmer10a.pdf

    Linear Scan Register Allocation on SSA Form
        by Christian Wimmer, Michael Franz

    http://hal.archives-ouvertes.fr/docs/00/55/85/09/PDF/RR-7503.pdf

    Computing Liveness Sets for SSA-Form Programs (2011)
        by Florian Brandner, Benoit Boissinot, Alain Darte,
            Benoît Dupont De Dinechin, Fabrice Rastello
            
*/







void xec_ssa_liveness( xec_ssa* ssa )
{
    std::vector< xec_ssa_loop_forest* > forests;

    for ( size_t i = 0; i < ssa->functions.size(); ++i )
    {
        xec_ssa_func* func = ssa->functions.at( i );
    
        xec_ssa_dfo dfo( ssa );
        dfo.build_ordering( func );
        
        xec_ssa_loop_forest* loops = new xec_ssa_loop_forest( ssa );
        loops->build_forest( func, &dfo );
        forests.push_back( loops );
    }

    xec_ssa_print( ssa );

    for ( size_t i = 0; i < forests.size(); ++i )
    {
        forests.at( i )->print();
        printf( "\n\n" );
        delete forests.at( i );
    }
}






#if MUSINGS




struct xec_ssa_liveness_loop
{
    explicit xec_ssa_liveness_loop( xec_ssa_block* loop_header );

    xec_ssa_block* header;
    std::vector< xec_ssa_liveness_loop* > loops;
    std::vector< xec_ssa_block* > blocks;
};


struct xec_ssa_liveness_block
{
    explicit xec_ssa_liveness_block( xec_ssa_block* block );

    xec_ssa_block* block;
    std::unordered_set< xec_ssa_opref > livein;
    std::unordered_set< xec_ssa_opref > liveout;
};


class xec_ssa_liveness_analyzer
{
public:

    xec_ssa_liveness_analyzer( xec_ssa* root, xec_ssa_func* func );
    void analyze();

private:

    void partial_liveness( xec_ssa_block* block );

    std::unordered_map< xec_ssa_block*, xec_ssa_liveness_block > blocklive;

    xec_ssa*        root;
    xec_ssa_func*   func;

};


xec_ssa_liveness_analyzer::xec_ssa_liveness_analyzer(
                xec_ssa* root, xec_ssa_func* func )
    :   root( root )
    ,   func( func )
{
}

void xec_ssa_liveness_analyzer::analyze()
{
    /*
    
    
        xec deals with reducible CFGs (there are no entries into loops other
        than through the loop header).
    */

    /*
        Caculate the CFG dominator tree.
    */
    
        The first step is to perform a depth-first ordering of blocks,
        which is the order that nodes in the control-flow graph are visited
        in during a depth-first search.  This allows CFG edges to be classified
        as either back edges (where a node later in the depth-first ordering
        has an edge back to an ancestor node), or not.
        
     
    
        The first step is to find an ordering of blocks where the only back
        edges are those for loops.
     
        The first step is to perform a depth-first-search ordering of blocks,
        which is the order that nodes in the control-flow-graph are visited
        in during a depth-first-search.  Given such an ordering of a
        reducible control flow graph, the only back edges are loops.
    
    */




    // Number blocks with respect to ordering.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );
        block->index = (int)i;
    }


    // Build list of blocks which belong to each loop.
    


    // Calculate partial liveness for each block.
    for ( auto i = func->blocks.rbegin(); i != func->blocks.rend(); ++i )
    {
        partial_liveness( *i );
    }
    
}



/*
    We assume the CFG is reducible, and that the only back-edges (when
    looking at the block ordering) are entries to loop headers.  We need a
    list of every block dominated by the loop header that is part of the
    loop - i.e. branches which end in break or return are excluded.
    
    During liveness analysis all variables live on entry to the loop header
    must be live throughout the entire loop (if the loop loops, we will still
    need them).
*/




/*
    The partial liveness step calculates livein and liveout for each block,
    ignoring loops.  The main part of the BuildIntervals function in the
    Wimmer paper corresponds to the postorder traversal in the Brandner paper,
    given that blocks are already ordered and visited bottom-up.
*/

void xec_ssa_liveness_analyzer::partial_liveness( xec_ssa_block* block )
{
    assert( blocklive.find( block ) == blocklive.end() );
}






#endif



