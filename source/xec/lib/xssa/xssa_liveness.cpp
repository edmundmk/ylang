//
//  xssa_liveness.cpp
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_liveness.h"
#include "xssa.h"



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





static void successor( xssa_opset* live, xssa_block* prev, xssa_block* block );
static void loop( xssa_loop* l, const xssa_opset& live );


void xssa_liveness( xssa_func* func )
{

    // Assume blocks are in depth-first order and go in reverse.
    for ( int i = (int)func->blocks.size() - 1; i >= 0; --i )
    {
    

    xssa_block* block = func->blocks.at( i ).get();
    xssa_opset live;
    
    
    // At the bottom of the block, all variables that are live on entry to
    // successor blocks are live.  This includes all references in ɸ-functions
    // from this code path.
    if ( block->condition )
    {
        successor( &live, block, block->iftrue );
        successor( &live, block, block->iffalse );
    
        // The condition is also live at the bottom of the block.  However
        // next ops don't require live ranges as they return their continue
        // result in special 'register' @done.
        if ( block->condition->opcode != XSSA_NEXT )
        {
            live.insert( block->condition );
        }
    }
    else
    {
        successor( &live, block, block->next );
    }
    
    
    // This is the live out set of this block (so far).
    block->live_out = live;
    
    
    // Go through all ops, adding ops used in the block, and removing those
    // which are defined.
    for ( int i = (int)block->ops.size() - 1; i >= 0; --i )
    {
        xssaop* op = block->ops.at( i );
        if ( ! op )
            continue;

        for ( size_t i = 0; i < op->operand_count; ++i )
        {
            if ( op->operands[ i ] )
                live.insert( op->operands[ i ] );
        }
        
        if ( xssaop::has_multival( op->opcode ) && op->multival )
        {
            live.insert( op->multival );
        }
        
        live.erase( op );
    }
    
    
    // And remove phi ops as they are defined.  Phi op operands don't create
    // liveness here but rather at the _end_ of predecessor blocks.
    for ( int i = (int)block->phi.size() - 1; i >= 0; --i )
    {
        xssaop* op = block->phi.at( i );
        if ( ! op )
            continue;
        
        live.erase( op );
    }
    
    
    // This is the live in of this block.
    block->live_in = live;
    
    
    // If this block is a loop header, then all ops live at the start of the
    // loop are live throughout the entire loop.
    if ( block->loop && block->loop->header == block )
    {
        loop( block->loop, live );
    }

    
    
    }


}


static void successor( xssa_opset* live, xssa_block* prev, xssa_block* block )
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
        live->insert( block->live_in.begin(), block->live_in.end() );
    }
    
    
    // Find index of this code path in the block block's previous link.
    // This relies on ɸ-function operands being correctly ordered.
    size_t index = 0;
    for ( ; index < block->previous.size(); ++index )
    {
        if ( block->previous.at( index ) == prev )
        {
            break;
        }
    }
    assert( index < block->previous.size() );
    
    
    // Variables that are live at the start of block include those
    // variables referenced by ɸ-functions coming from this code path.
    for ( size_t i = 0; i < block->phi.size(); ++i )
    {
        xssaop* op = block->phi.at( i );
        if ( op && op->opcode == XSSA_PHI )
        {
            live->insert( op->operands[ index ] );
        }
    }

}


static void loop( xssa_loop* l, const xssa_opset& live )
{
    for ( auto i = l->blocks.begin(); i != l->blocks.end(); ++i )
    {
        xssa_block* block = *i;
        block->live_in.insert( live.begin(), live.end() );
        block->live_out.insert( live.begin(), live.end() );
    }
    
    for ( auto i = l->children.begin(); i != l->children.end(); ++i )
    {
        loop( *i, live );
    }
}






