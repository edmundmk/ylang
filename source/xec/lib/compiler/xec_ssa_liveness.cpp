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
            
    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.57.5381&rep=rep1&type=pdf
    
    Identifying Loops in Almost Linear Time
        by G. Ramalingam
        
    http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=232236A4A060126D1728AAA80F94FA21?doi=10.1.1.13.202&rep=rep1&type=pdf
    
    On Loops, Dominators, and Dominance Frontiers
        by G. Ramalingam
        
    http://www.cs.ucr.edu/~gupta/teaching/201-12/My2.pdf
    
    CS 201 Compiler Construction - Lecture 2 Control Flow Analysis
 
*/




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


xec_ssa_dfo::xec_ssa_dfo( xec_ssa* root )
    :   root( root )
{
}


size_t xec_ssa_dfo::size()
{
    return ordering.size();
}

xec_ssa_block* xec_ssa_dfo::at( size_t index )
{
    return ordering.at( index );
}


void xec_ssa_dfo::build_ordering( xec_ssa_func* func )
{
    ordering.clear();

    // Clear any existing ordering information.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );
        block->index = -1;
    }
    
    // Start.
    ordering.resize( func->blocks.size() );
    int index = (int)func->blocks.size();
    visit( func->block, &index );
}

void xec_ssa_dfo::visit( xec_ssa_block* block, int* index )
{
    // Ensure this node exists and hasn't been visited.
    if ( ! block || block->index != -1 )
    {
        return;
    }
    
    // Mark visited.
    block->index = 0;
    
    // Visit successors.
    if ( block->condition )
    {
        // Prefer true branch by visting it last.
        visit( block->iffalse, index );
        visit( block->iftrue, index );
    }
    else
    {
        visit( block->next, index );
    }
    
    // Bless with final index.
    *index -= 1;
    assert( *index >= 0 );
    block->index = *index;
    ordering[ block->index ] = block;
}







/*
    Compute a loop nesting forest for a CFG.
*/

struct xec_ssa_loop_node
{
    xec_ssa_loop_node( xec_ssa_block* header );

    xec_ssa_block* header;
    std::unordered_set< xec_ssa_loop_node* >    loops;
    std::unordered_set< xec_ssa_block* >        blocks;
    bool nested;
};


class xec_ssa_loop_forest
{
public:

    explicit xec_ssa_loop_forest( xec_ssa* root );
    
    void build_forest( xec_ssa_func* func, xec_ssa_dfo* dfo );
    void print();
    
    xec_ssa_loop_node* loopof( xec_ssa_block* block );
    
    
private:

    void print_node( xec_ssa_loop_node* node, int indent );

    xec_ssa* root;
    std::vector< std::unique_ptr< xec_ssa_loop_node > > loops;
    std::unordered_map< xec_ssa_block*, xec_ssa_loop_node* > loopmap;

};


xec_ssa_loop_node::xec_ssa_loop_node( xec_ssa_block* header )
    :   header( header )
    ,   nested( false )
{
}


xec_ssa_loop_forest::xec_ssa_loop_forest( xec_ssa* root )
    :   root( root )
{
}

void xec_ssa_loop_forest::build_forest( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    loops.clear();
    loopmap.clear();

    std::deque< xec_ssa_block* > work;

    // Visit every block in reverse depth-first order.
    for ( int i = (int)dfo->size() - 1; i >= 0; --i )
    {
        xec_ssa_block* block = dfo->at( i );

        // If this block has any incoming back-edges (including edges that
        // point to itself), then it is a loop header.
        for ( size_t i = 0; i < block->previous.size(); ++i )
        {
            xec_ssa_block* previous = block->previous.at( i );
            if ( previous->index >= block->index )
            {
                work.push_back( previous );
            }
        }
        
        // Or maybe not.
        if ( work.size() == 0 )
        {
            continue;
        }
        
        // This is a loop header.
        auto loop = std::make_unique< xec_ssa_loop_node >( block );
        
        // Blocks in the work list are those at the 'continue' points of the
        // loop.  They are the start of our search back up the graph.
        while ( work.size() )
        {
            xec_ssa_block* child = work.back();
            work.pop_back();
            
            // If we've reached the loop header, stop.
            if ( child == block )
            {
                continue;
            }
            
            // This block might be part of a nested loop.  If so, we will
            // already have encountered the loop in question (since the loop
            // headers of nested loops must come after the loop headers of
            // outer loops in the DFO).
            auto i = loopmap.find( child );
            if ( i != loopmap.end() )
            {
                xec_ssa_loop_node* nested = i->second;
            
                // Yes it is.  Add the child loop.
                nested->nested = true;
                auto inserted = loop->loops.insert( nested );
                
                if ( inserted.second )
                {
                    // This was the first time we've encountered this nested
                    // loop, so continue searching from the loop header (since
                    // all nodes we'll encounter between here and the loop
                    // header will be part of the nested loop).
                    child = i->second->header;
                }
                else
                {
                    // Already looked at this nested loop, nothing more to do.
                    continue;
                }
            }
            else
            {
                // Block is part of this loop but not any nested loops.
                loop->blocks.insert( child );
            }
            
            // Continue search up the graph.
            for ( size_t i = 0; i < child->previous.size(); ++i )
            {
                xec_ssa_block* previous = child->previous.at( i );
                
                // Ignore back edges (should only happen if child is the loop
                // header of an already-identified nested loop).
                if ( previous->index >= child->index )
                {
                    continue;
                }
                
                // This block is also part of the loop.
                work.push_back( previous );
            }
        }
        
        // Loop structure contains all nested loops and blocks in this loop.
        loopmap.emplace( loop->header, loop.get() );
        for ( auto i = loop->blocks.begin(); i != loop->blocks.end(); ++i )
        {
            loopmap.emplace( *i, loop.get() );
        }
        
        
        // Add to loop list.
        loops.push_back( std::move( loop ) );

    }
}


xec_ssa_loop_node* xec_ssa_loop_forest::loopof( xec_ssa_block* block )
{
    auto i = loopmap.find( block );
    if ( i != loopmap.end() )
        return i->second;
    else
        return NULL;
}


void xec_ssa_loop_forest::print()
{
    for ( size_t i = 0; i < loops.size(); ++i )
    {
        xec_ssa_loop_node* loop = loops.at( i ).get();
        if ( ! loop->nested )
        {
            print_node( loop, 0 );
        }
    }
}

void xec_ssa_loop_forest::print_node( xec_ssa_loop_node* node, int indent )
{
    printf( "%*s*[%04X]\n", indent, "", node->header->index );
    indent += 4;
    for ( auto i = node->loops.begin(); i != node->loops.end(); ++i )
    {
        print_node( *i, indent );
    }
    for ( auto i = node->blocks.begin(); i != node->blocks.end(); ++i )
    {
        printf( "%*s[%04X]\n", indent, "", (*i)->index );
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



