//
//  yssa_analyze.cpp
//  ylang
//
//  Created by Edmund Kapusniak on 27/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_analyze.h"
#include <make_unique.h>
#include "yssa.h"



/*

    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.57.5381&rep=rep1&type=pdf
    
    Identifying Loops in Almost Linear Time
        by G. Ramalingam
        
    http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=232236A4A060126D1728AAA80F94FA21?doi=10.1.1.13.202&rep=rep1&type=pdf
    
    On Loops, Dominators, and Dominance Frontiers
        by G. Ramalingam
        
    http://www.cs.ucr.edu/~gupta/teaching/201-12/My2.pdf
    
    CS 201 Compiler Construction - Lecture 2 Control Flow Analysis
 
*/




static void visit_depth_first( yssa_block* block, int* index )
{
    // Ensure this node exists and hasn't been visited.
    if ( ! block || block->index != -1 )
    {
        return;
    }
    
    // Mark visited.
    block->index = 0;
    
    
    // Visit exception block, if any.
    visit_depth_first( block->xchandler, index );

    
    // Visit successors.
    if ( block->test )
    {
        if ( block->flags & YSSA_PREFER_FAIL )
        {
            // Prefer false branch by visting it last.
            visit_depth_first( block->next, index );
            visit_depth_first( block->fail, index );
        }
        else
        {
            // Prefer true branch by visting it last.
            visit_depth_first( block->fail, index );
            visit_depth_first( block->next, index );
        }
    }
    else
    {
        visit_depth_first( block->next, index );
    }
    
    
    // Bless with final index.
    *index -= 1;
    assert( *index >= 0 );
    block->index = *index;
}


void yssa_analyze_depth_first_order( yssa_function* function )
{
    // Clear existing order.
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        block->index = -1;
    }

    // Start at the initial block and visit recursively.
    int index = (int)function->blocks.size();
    visit_depth_first( function->blocks[ 0 ].get(), &index );
    
    // Reorder blocks.
    std::vector< yssa_block_p > blocks;
    blocks.resize( function->blocks.size() );
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        size_t index = function->blocks[ i ]->index;
        assert( blocks[ index ] == nullptr );
        blocks[ index ] = std::move( function->blocks[ i ] );
    }
    
    std::swap( function->blocks, blocks );
}





void yssa_analyze_loop_forest( yssa_function* function )
{
    // Remove previous loop calculation.
    function->loops.clear();
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        block->loop = nullptr;
    }
    
    
    
    // Assume that blocks are in depth-first order, visit them in reverse.
    std::vector< yssa_block* > work;
    for ( int i = (int)function->blocks.size() - 1; i >= 0; --i )
    {


    yssa_block* block = function->blocks.at( i ).get();


    // If this block has any incoming back-edges (including edges that
    // point to itself), then it is a loop header.
    for ( size_t i = 0; i < block->prev.size(); ++i )
    {
        yssa_block* previous = block->prev.at( i );
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
    yssa_loop_p loop = std::make_unique< yssa_loop >( nullptr, block );
    loop->blocks.insert( block );
    
    
    // Blocks in the work list are those at the 'continue' points of the
    // loop.  They are the start of our search back up the graph.
    while ( work.size() )
    {
        yssa_block* child = work.back();
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
        if ( child->loop )
        {
            yssa_loop* nested = child->loop;
        
            // Yes it is.  Add the child loop.
            nested->parent = loop.get();
            auto inserted = loop->children.insert( nested );
            
            if ( inserted.second )
            {
                // This was the first time we've encountered this nested
                // loop, so continue searching from the loop header (since
                // all nodes we'll encounter between here and the loop
                // header will be part of the nested loop).
                child = nested->header;
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
            assert( child->loop == nullptr );
            loop->blocks.insert( child );
        }
        

        // Continue search up the graph.
        for ( size_t i = 0; i < child->prev.size(); ++i )
        {
            yssa_block* previous = child->prev.at( i );
            
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
    assert( loop->header->loop == nullptr );
    for ( auto i = loop->blocks.begin(); i != loop->blocks.end(); ++i )
    {
        yssa_block* block = *i;
        assert( block->loop == nullptr );
        block->loop = loop.get();
    }
    
    
    // Add to loop list.
    function->loops.push_back( std::move( loop ) );
    
    }
}







