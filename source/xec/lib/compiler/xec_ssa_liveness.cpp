//
//  xec_ssa_liveness.cpp
//
//  Created by Edmund Kapusniak on 17/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_liveness.h"
#include <make_unique>
#include "xec_script.h"
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
    ,   func( NULL )
    ,   dfo( NULL )
    ,   loops( NULL )
{
}


void xec_ssa_liveness::analyze_func(
        xec_ssa_func* func, xec_ssa_dfo* dfo, xec_ssa_loop_forest* loops )
{
    region_scope rscope( root->alloc );

    this->func  = func;
    this->dfo   = dfo;
    this->loops = loops;
    livein.clear();

    // Go through blocks in reverse dfo order.
    for ( int i = (int)dfo->size() - 1; i >= 0; --i )
    {
        xec_ssa_block* block = dfo->at( i );
        analyze_block( block );
    }
    
    
    this->func  = NULL;
    this->dfo   = NULL;
    this->loops = NULL;
    livein.clear();
}




void xec_ssa_liveness::analyze_block( xec_ssa_block* block )
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
        
        // The condition is also live at the bottom of the block.
        livespan s( block->condition, XEC_SSA_FOREVER, XEC_SSA_INVALID );
        live.emplace( block->condition, s );
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
            operand = func->operand( operand );
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
    for ( auto i = live.begin(); i != live.end(); ++i )
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
            live_loop( &span, loop );
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
    
    
    // Add all values that were live at the start of this successor.
    // If it's a back reference, we haven't calculated liveness yet.
    if ( block->index < successor->index )
    {
        liveset& slive = livein.at( successor );
        live->insert( slive.begin(), slive.end() );
    }
    
    
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
    for ( size_t i = 0; i < successor->phi->ops.size(); ++i )
    {
        xec_ssa_op& op = successor->phi->ops.at( i );
        xec_ssa_opref value = XEC_SSA_INVALID;
        
        if ( op.opcode == XEC_SSA_PHI )
        {
            value = op.phi->definitions.at( index );
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




void xec_ssa_liveness::live_loop( livespan* span, xec_ssa_loop* loop )
{
    // Pointer to last link in live span list.
    xec_ssa_opref* prev = &span->lnext;
    
    
    // First block in live span list should be a live instruction at the head
    // of the loop header block.
    assert( *prev );
    assert( func->slices.at( prev->slice )->block == loop->header );
    assert( func->getop( *prev )->opcode == XEC_SSA_LIVE );


    // Go from loop header to loop bottom, and add a live span for each block
    // that is inside the loop.
    for ( size_t i = loop->header->index; i <= loop->last; ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        
        
        // Ignore blocks that aren't in the loop.
        if ( ! loops->inloop( block, loop ) )
        {
            continue;
        }
        
    
        // While block of the next live range is before this block, move to
        // next in chain.
        xec_ssa_block* liveblock = blockof( *prev );
        while ( liveblock && liveblock->index < block->index )
        {
            xec_ssa_op* op = func->getop( *prev );
            prev = &op->lnext;
            liveblock = blockof( *prev );
        }
        
        
        // Possibly there is already a live range at the beginning of the
        // block which can be reused.
        if ( liveblock == block )
        {
            xec_ssa_op* op = func->getop( *prev );
            if ( op->opcode == XEC_SSA_LIVE )
            {
                // Great, extend the op to the end of the block and continue.
                assert( op->operanda == span->value );
                op->until = XEC_SSA_FOREVER;
                prev = &op->lnext;
                continue;
            }
            else
            {
                // This live range starts at the definition of the value.
                // Take it out of the equation so it can be replaced with
                // a range starting at the start of the block.
                assert( *prev == span->value );
                op->until = XEC_SSA_INVALID;
                op->lnext = span->lnext;
                
                // This situation means a mess as the definition of a value
                // required at the start of the loop is defined only after
                // the loop has already begun...
                root->script->error( op->sloc, "undefined value" );
            }
        }
        
        
        // Add new range spanning this block.
        xec_ssa_op lop( -1, XEC_SSA_LIVE, span->value );
        lop.until = XEC_SSA_FOREVER;
        lop.lnext = *prev;
        
        xec_ssa_opref lref;
        lref.slice = block->live->index;
        lref.index = (int)block->live->ops.size();
        
        block->live->ops.push_back( lop );
        
        *prev = lref;
        xec_ssa_op& op = block->live->ops.back();
        prev = &op.lnext;
    }
}


xec_ssa_block* xec_ssa_liveness::blockof( xec_ssa_opref opref )
{
    if ( opref )
    {
        xec_ssa_slice* slice = func->slices.at( opref.slice );
        return slice->block;
    }
    return NULL;
}









