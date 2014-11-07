//
//  xec_ssa_cfganalysis.cpp
//
//  Created by Edmund Kapusniak on 19/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_cfganalysis.h"
#include <make_unique>
#include "xec_ssa.h"


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







xec_ssa_loop::xec_ssa_loop( xec_ssa_block* header )
    :   outer( NULL )
    ,   header( header )
    ,   last( -1 )
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
        auto loop = std::make_unique< xec_ssa_loop >( block );
        loop->last = block->index;
        
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
                xec_ssa_loop* nested = i->second;
            
                // Yes it is.  Add the child loop.
                nested->outer = loop.get();
                auto inserted = loop->loops.insert( nested );
                loop->last = std::max( loop->last, nested->last );
                
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
                loop->last = std::max( loop->last, child->index );
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


xec_ssa_loop* xec_ssa_loop_forest::loopof( xec_ssa_block* block )
{
    auto i = loopmap.find( block );
    if ( i != loopmap.end() )
        return i->second;
    else
        return NULL;
}


bool xec_ssa_loop_forest::inloop( xec_ssa_block* block, xec_ssa_loop* loop )
{
    for ( xec_ssa_loop* l = loopof( block ); l != NULL; l = l->outer )
    {
        if ( l == loop )
        {
            return true;
        }
    }
    
    return false;
}


void xec_ssa_loop_forest::print()
{
    for ( size_t i = 0; i < loops.size(); ++i )
    {
        xec_ssa_loop* loop = loops.at( i ).get();
        if ( ! loop->outer )
        {
            print_node( loop, 0 );
        }
    }
}

void xec_ssa_loop_forest::print_node( xec_ssa_loop* node, int indent )
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




















