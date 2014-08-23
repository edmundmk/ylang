//
//  xec_ssa_regalloc.cpp
//  xob
//
//  Created by Edmund Kapusniak on 23/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_regalloc.h"
#include "xec_ssa_cfganalysis.h"



xec_ssa_regalloc::xec_ssa_regalloc( xec_ssa* root )
    :   root( root )
{
}

void xec_ssa_regalloc::allocate( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    this->func = func;
    this->dfo  = dfo;
    equivmap.clear();


    // Build ɸ equivalence.
    build_phiequiv();

    
    // The dfo gives a global ordering of ops.  Build list of live/dead
    // events in order by scanning live ranges.
    forward_scan();
    
    
    // Actually perform register allocation by scanning the event list
    // _from the bottom up_, which allows us to place stack-op (e.g. calls)
    // parameters (well, most of them) after placing the stack-op itself,
    // meaning fewer moves.
    reverse_scan();
    
}



xec_ssa_opref xec_ssa_regalloc::equiv( xec_ssa_opref opref )
{
    auto i = equivmap.find( opref );
    if ( i != equivmap.end() )
        return i->second;
    else
        return opref;
}

    

void xec_ssa_regalloc::build_phiequiv()
{
    for ( size_t i = 0; i < dfo->size(); ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        
        for ( size_t i = 0; i < block->phi->ops.size(); ++i )
        {
            xec_ssa_op& op = block->phi->ops.at( i );
            
            if ( op.opcode != XEC_SSA_PHI )
            {
                continue;
            }
            
            xec_ssa_opref phiref;
            phiref.slice = block->phi->index;
            phiref.index = (int)i;
            
            // Attempt to merge the live ranges of this phi operation and
            // each of its operands.  Start with the last operand, as this
            // is likely to be at the 'bottom' of a loop and therefore the
            // most frequently encountered.
            
            for ( int i = (int)op.phi->definitions.size() - 1; i >= 0; ++i )
            {
                attempt_equiv( phiref, op.phi->definitions.at( i ) );
            }
        }
    }
}


void xec_ssa_regalloc::attempt_equiv( xec_ssa_opref a, xec_ssa_opref b )
{
    // If either op has been added to an equiv class before, then the head
    // of its live range may be different.
    a = equiv( a );
    b = equiv( b );

    // Ensure that a comes before b in the dfo order.
    int order = refcmp( a, b );
    if ( order == 0 /* a == b */ )
    {
        // Ops are in the same equiv class, nothing to do.
        return;
    }
    if ( order > 0 /* a > b */ )
    {
        // Swap to ensure order.
        std::swap( a, b );
    }
    
    
    // Check if live ranges interfere.
    if ( interfere( a, b ) )
    {
        return;
    }
    
    
    // Live ranges don't interfere, merge the two live ranges.  Do this by
    // adding b's live range to a, and setting the equiv class of each def
    // in b to the head of a.


}


bool xec_ssa_regalloc::interfere( xec_ssa_opref a, xec_ssa_opref b )
{
    // Check each live span in a.
    for ( ; a; a = func->getop( a )->lnext )
    {
        // Ignore spans in b that are wholly before this span.
        while ( true )
        {
            assert( a != b );
            int order = spancmp( a, b );
            if ( order == 0 )
            {
                // Spans interfere.
                return true;
            }
            else if ( order < 0 /* a < b */ )
            {
                // Move onto next a span.
                break;
            }
            else
            {
                // Move to next b span.
                b = func->getop( b )->lnext;
                if ( ! b )
                {
                    // Reached end of b, does not interfere.
                    return false;
                }
            }
        }
    }
    
    // Reached end of a, does not interfere.
    return false;
}


int xec_ssa_regalloc::refcmp( xec_ssa_opref a, xec_ssa_opref b )
{
    if ( a == b )
        return 0;

    uint64_t aindex = opindex( a );
    uint64_t bindex = opindex( b );
    
    if ( aindex < bindex )
    {
        return -1;
    }
    if ( aindex > bindex )
    {
        return 1;
    }
    
    assert( ! "ops are equal but didn't test equal" );
    return 0;
}


int xec_ssa_regalloc::spancmp( xec_ssa_opref a, xec_ssa_opref b )
{
    // Check if one span is wholly before/after the other, or if they overlap.

    uint64_t auntil = opuntil( a );
    uint64_t bindex = opindex( b );
    
    if ( bindex >= auntil )
    {
        // b span begins after a span ends.
        return -1;
    }
    
    uint64_t buntil = opuntil( b );
    uint64_t aindex = opindex( a );
    
    if ( aindex >= buntil )
    {
        // a span begins after b span ends.
        return 1;
    }
    
    // spans overlap.
    return 0;
}


uint64_t xec_ssa_regalloc::opindex( xec_ssa_opref opref )
{
    // Assume that slices are numbered linearly within a block.  It would be
    // nice if we could guarantee that oprefs were laid out with the slice
    // index in the high bits...
    
    xec_ssa_block* block = func->slices.at( opref.slice )->block;
    return (uint64_t)block->index << 32
         | (uint64_t)opref.slice << xec_ssa_opref::INDEX_BITS
         | (uint64_t)opref.index;
}


uint64_t xec_ssa_regalloc::opuntil( xec_ssa_opref live )
{
    xec_ssa_opref until = func->getop( live )->until;
    if ( until != XEC_SSA_FOREVER )
    {
        // Live until op.
        return opindex( until );
    }
    else
    {
        // Live until end of block.
        xec_ssa_block* block = func->slices.at( live.slice )->block;
        return (uint64_t)block->index << 32
             | (uint64_t)0xFFFFFFFF;
    }
}



void xec_ssa_regalloc::forward_scan()
{
}


void xec_ssa_regalloc::reverse_scan()
{
}