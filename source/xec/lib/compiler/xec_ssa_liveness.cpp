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
    
    Useful papers about liveness analysis:

    http://www.christianwimmer.at/Publications/Wimmer10a/Wimmer10a.pdf

    Linear Scan Register Allocation on SSA Form
        by Christian Wimmer, Michael Franz

    http://hal.archives-ouvertes.fr/docs/00/55/85/09/PDF/RR-7503.pdf

    Computing Liveness Sets for SSA-Form Programs (2011)
        by Florian Brandner, Benoit Boissinot, Alain Darte,
            Benoît Dupont De Dinechin, Fabrice Rastello
            
*/




xec_ssa_liveness::livespan::livespan(
                xec_ssa_opref value, xec_ssa_opref until, xec_ssa_opref lnext )
    :   value( value )
    ,   until( until )
    ,   lnext( lnext )
{
}



xec_ssa_liveness::xec_ssa_liveness( xec_ssa* root )
    :   root( root )
{
}


void xec_ssa_liveness::analyze_func(
        xec_ssa_func* func, xec_ssa_dfo* dfo, xec_ssa_loop_forest* loops )
{
    // Go through blocks in reverse dfo order.
    for ( int i = (int)dfo->size() - 1; i >= 0; --i )
    {
        xec_ssa_block* block = dfo->at( i );
        analyze_block( block, dfo, loops );
    }
}




void xec_ssa_liveness::analyze_block(
        xec_ssa_block* block, xec_ssa_dfo* dfo, xec_ssa_loop_forest* loops )
{
    // Find lowest point of use of all live values.
    liveset live;

    
    // At the bottom of the block, all variables that are live on entry to
    // successor blocks are live.  This includes all references in ɸ-functions
    // from this code path.
    if ( block->condition )
    {
        add_successor( &live, block, block->iftrue );
        add_successor( &live, block, block->iffalse );
    }
    else
    {
        add_successor( &live, block, block->next );
    }


    // Add all values used in the block.  Don't include phi functions as those
    // will be dealt with by the calculation for predecessor blocks.
    std::vector< xec_ssa_opref > oprefs;
    for ( int i = (int)block->ops->ops.size() - 1; i >= 0; --i )
    {
        xec_ssa_op& op = block->ops->ops.at( i );

        xec_ssa_opref useref;
        useref.slice = block->ops->index;
        useref.index = i;
        
        op.get_operands( &oprefs );
        for ( size_t i = 0; i < oprefs.size(); ++i )
        {
            xec_ssa_opref operand = oprefs.at( i );
            livespan s( operand, useref, XEC_SSA_INVALID );
            live.emplace( operand, s );
        }
    }
    
    
    // Remove values defined in this block.
    for ( auto i = live.begin(); i != live.end(); )
    {
        livespan& span = i->second;
        
        // Check if this value is defined locally.
        xec_ssa_op* def = local_def( block, span.value );
        if ( ! def )
        {
            ++i;
            continue;
        }
        
        // Live until lowest point of use.
        def->until = span.until;
        def->lnext = span.lnext;
        
        // Not live at start of block.
        auto erase = i;
        ++i;
        live.erase( erase );
    }


    // Find loop header for this block.
    xec_ssa_loop* loop = loops->loopof( block );


    // All values remaining in live are live at the start of the block.  Add
    // live range ops to head of block.
    for ( auto i = live.begin(); i != live.end(); )
    {
        livespan& span = i->second;
        
        xec_ssa_op lop( -1, XEC_SSA_LIVE, span.value );
        lop.until = span.until;
        lop.lnext = span.lnext;
        
        xec_ssa_opref lref;
        lref.slice = block->live->index;
        lref.index = (int)block->live->ops.size();
        
        block->live->ops.push_back( lop );
        
        // When we grab this span as part of the live set at the bottom of a
        // predecessor block, the value is live until the end of that block,
        // and the next live range is the one we've just made the op for.
        span.until = XEC_SSA_FOREVER;
        span.lnext = lref;
        
        
        // Also if this block is a loop header then the value is live through
        // the entire loop.
        if ( loop && loop->header == block )
        {
            live_loop( &span, dfo, loops, loop );
        }
    }
    
    
    // This is the live-in set for this block.
    livein.emplace( block, std::move( live ) );
    
}



void xec_ssa_liveness::add_successor(
        liveset* live, xec_ssa_block* block, xec_ssa_block* successor )
{
    // Successor might not exist.
    if ( ! successor )
    {
        return;
    }
    
    // If it's a back reference, we haven't calculated liveness yet.
    if ( block->index >= successor->index )
    {
        return;
    }
    
    // Add all values that were live at the start of this successor.
    liveset& slive = livein.at( successor );
    live->insert( slive.begin(), slive.end() );
    
    // Find index of this code path in the successor block's previous link.
    // This relies on ɸ-function operands being correctly ordered.
    size_t index = 0;
    for ( ; index < successor->previous.size(); ++index )
    {
        if ( successor->previous.at( index ) == block )
        {
            break;
        }
    }
    assert( index < successor->previous.size() );
    
    // Variables that are live at the start of successor include those
    // variables referenced by ɸ-functions coming from this code path.
    for ( size_t i = 0; i < block->phi->ops.size(); ++i )
    {
        xec_ssa_op& op = block->phi->ops.at( i );
        xec_ssa_opref value = XEC_SSA_INVALID;
        
        if ( op.opcode == XEC_SSA_PHI )
        {
            value = op.phi->definitions.at( index );
        }
        else if ( op.opcode == XEC_SSA_REF )
        {
            value = op.operanda;
        }
        
        if ( value )
        {
            livespan s( value, XEC_SSA_FOREVER, XEC_SSA_INVALID );
            live->emplace( value, s );
        }
        
    }
}


xec_ssa_op* xec_ssa_liveness::local_def(
                xec_ssa_block* block, xec_ssa_opref value )
{
    // Return a pointer to the op in block which defines the value, or NULL.

    if ( value.slice == block->phi->index )
    {
        return &block->phi->ops.at( value.index );
    }
    
    if ( value.slice == block->ops->index )
    {
        return &block->ops->ops.at( value.index );
    }

    return NULL;
}




void xec_ssa_liveness::live_loop( livespan* span,
        xec_ssa_dfo* dfo, xec_ssa_loop_forest* loops, xec_ssa_loop* loop )
{
    // Go from loop header to loop bottom, and add a live span for each block
    // that is inside the loop.
    for ( size_t i = loop->header->index; i <= loop->last; ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        
        // Ignore blocks that aren't in the loop.
        if ( loops->inloop( block, loop ) )
        {
            continue;
        }
        
        // TODO.
    }
}





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



