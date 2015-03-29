//
//  yssa_liveness.cpp
//
//  Created by Edmund Kapusniak on 29/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_liveness.h"
#include "yssa.h"


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


/*
    We work on linearized SSA with analysis information.  Live ranges are
    represented as ranges of op indexes in the linearized SSA.
    
    Live ranges for each op and each variable are calculated.
    
     -  Our modified SSA form mandates that the live ranges of the definitions
            of a variable do not overlap.
     -  Live ranges can have holes, where ops are not live in certain blocks.
     -  Variables references from exception handlers are live throughout all
            blocks protected by that handler.

*/




typedef std::unordered_map< yssa_opinst*, size_t > yssa_live;


static void successor( yssa_function* function,
                yssa_live* live, yssa_block* prev, yssa_block* block );
static void live_loop( yssa_module* module,
                yssa_loop* loop, const yssa_live& live );



static void add_live_range( yssa_module* module,
                yssa_opinst* op, size_t start, size_t final );
static void union_live_range( yssa_module* module,
                yssa_live_range** pnext, size_t start, size_t final );





void yssa_liveness( yssa_module* module, yssa_function* function )
{
    // Calculate live ranges for each op.

    for ( int i = (int)function->blocks.size() - 1; i >= 0; --i )
    {
        yssa_block* block = function->blocks.at( i ).get();
    
        // The end of the live range for each op.
        std::unordered_map< yssa_opinst*, size_t > live;


        // At the bottom of the block, all variables that are live on
        // entry to successor blocks are live.  This includes all
        // references in phi-functions from this code path.
        if ( block->test )
        {
            successor( function, &live, block, block->next );
            successor( function, &live, block, block->fail );
            
            // The condition is also live at the bottom of the block.
            live.emplace( block->test, block->lfinal );
        }
        else
        {
            successor( function, &live, block, block->next );
        }
        
        
        // Visit each op in the block, building liveness information.
        for ( size_t i = block->lfinal; i > block->lstart; --i )
        {
            size_t index = i - 1;
            yssa_opinst* op = function->ops.at( index );
            
            // If this op is live, then this is the start of its live range.
            auto j = live.find( op );
            if ( j != live.end() )
            {
                // Add range.
                add_live_range( module, op, index, j->second );
                
                // Op is no longer live.
                live.erase( j );
            }
            
            
            // Phi function operands do not create liveness here, but
            // rather at the end of predecessor blocks.
            if ( op->opcode == YSSA_PHI || op->opcode == YSSA_REF )
            {
                continue;
            }
            
            
            // All operands which are not already live are live now.
            for ( size_t i = 0; i < op->operand_count; ++i )
            {
                if ( ! op->operand[ i ] )
                    continue;
                
                live.emplace( op->operand[ i ], index );
            }
            
            if ( op->has_multival() && op->multival )
            {
                live.emplace( op->multival, index );
            }
        }
        

        // This is the live in set of the block.  All live ranges
        // end at the top of the block.
        for ( auto liveop : live )
        {
            yssa_opinst* op = liveop.first;
            add_live_range( module, op, block->lstart, liveop.second );
            block->livein.insert( op );
        }
        
        
        // If this block is a loop header, then all ops live at the start
        // of the loop are live throughout the entire loop.
        if ( block->loop && block->loop->header == block )
        {
            live_loop( module, block->loop, live );
        }

    }


    // Live range for a variable is the union of live ranges of its
    // definitions (which must not overlap), plus the range of all
    // protected blocks if the variable is used in an xchandler.
    
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );
        if ( ! op->has_associated() && op->variable )
        {
            for ( yssa_live_range* live = op->live; live; live = live->next )
            {
                yssa_live_range** pnext = &op->variable->live;
                union_live_range( module, pnext, live->start, live->final );
            }
        }
    }
    
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        
    }


}


static void successor( yssa_function* function,
                yssa_live* live, yssa_block* prev, yssa_block* block )
{
    // Successor might not exist.
    if ( ! block )
    {
        return;
    }
    
    
    // Add all values that were live at the start of this successor.
    // If it's a back reference, we haven't calculated liveness yet.
    if ( prev->index < block->index )
    {
        for ( yssa_opinst* op : block->livein )
        {
            live->emplace( op, prev->lfinal );
        }
    }
    
    
    // Find index of this code path in the block block's previous link.
    // This relies on phi-function operands being correctly ordered.
    size_t index = 0;
    for ( ; index < block->prev.size(); ++index )
    {
        if ( block->prev.at( index ) == prev )
        {
            break;
        }
    }
    assert( index < block->prev.size() );
    
    
    // Variables that are live at the start of block include those
    // variables referenced by phi-functions coming from this code path.
    for ( size_t i = block->lstart; i < block->lfiphi; ++i )
    {
        yssa_opinst* op = function->ops.at( i );
        assert( op->opcode != YSSA_REF );
        
        if ( op->opcode == YSSA_PHI )
        {
            live->emplace( op->operand[ index ], prev->lfinal );
        }
    }

}


static void live_loop( yssa_module* module,
                yssa_loop* loop, const yssa_live& live )
{
    // Each op is live during each block of the loop.
    for ( auto liveop : live )
    {
        yssa_opinst* op = liveop.first;
        for ( yssa_block* block : loop->blocks )
        {
            union_live_range( module, &op->live, block->lstart, block->lfinal );
        }
    }
    
    // And live during any sub-loops.
    for ( yssa_loop* child : loop->children )
    {
        live_loop( module, child, live );
    }
}


static void add_live_range( yssa_module* module,
                yssa_opinst* op, size_t start, size_t final )
{
    assert( ! op->live || op->live->start >= final );
    if ( op->live && op->live->start == final )
    {
        op->live->start = start;
    }
    else
    {
        yssa_live_range* next = op->live;
        op->live = new ( module->alloc ) yssa_live_range( start, final, next );
    }
}


static void union_live_range( yssa_module* module,
                yssa_live_range** pnext, size_t start, size_t final )
{
    // Skip all existing ranges that lie completely before this range.
    while ( *pnext && ( *pnext )->final < start )
    {
        pnext = &( *pnext )->next;
    }
    
    // The current range is either overlapping, or after.
    if ( *pnext && ( *pnext )->start <= final )
    {
        // Overlapping.
        yssa_live_range* overlap = *pnext;
        overlap->start = std::min( overlap->start, start );
        overlap->final = std::max( overlap->final, final );
        
        while ( overlap->next && overlap->next->start <= final )
        {
            overlap->final = std::max( overlap->next->final, final );
            overlap->next  = overlap->next->next;
        }
    }
    else
    {
        // Next range is after, so insert new range.
        *pnext = new ( module->alloc ) yssa_live_range( start, final, *pnext );
    }
}





