//
//  xec_ssa_regalloc.cpp
//  xob
//
//  Created by Edmund Kapusniak on 23/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_regalloc.h"
#include "xec_ssa_cfganalysis.h"
#include <queue>



xec_ssa_regalloc::xec_ssa_regalloc( xec_ssa* root )
    :   root( root )
{
}

void xec_ssa_regalloc::allocate( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    this->func = func;
    this->dfo  = dfo;
    equivmap.clear();
    events.clear();


    // Build ɸ equivalence.
    build_phiequiv();

    
    // The dfo gives a global ordering of ops.  Build list of live/dead
    // events in order by scanning live ranges.
    forward_scan();
    
    
    // Print event list for debugging.
    print_events();
    
    
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
            
            for ( int i = (int)op.phi->definitions.size() - 1; i >= 0; --i )
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
    xec_ssa_opref* prev = &func->getop( a )->lnext;
    while ( b )
    {
        while ( *prev && refcmp( *prev, b ) < 0 /* *prev < b */ )
        {
            // Next span in a comes before span b.  Move to next span in a.
            prev = &func->getop( *prev )->lnext;
        }
        
        
        // b is now in a's equiv class.
        xec_ssa_op* bop = func->getop( b );
        if ( bop->opcode != XEC_SSA_LIVE )
        {
            equivmap[ b ] = a;
        }
        
        
        // Link span b into a.
        xec_ssa_opref bnext = bop->lnext;
        bop->lnext = *prev;
        *prev = b;
        prev = &bop->lnext;
        b = bnext;
    }

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




/*
    Forward scan over the live ranges, building events.
*/


struct xec_ssa_fvalue
{
    uint64_t        index;
    xec_ssa_opref   value;
    xec_ssa_opref   span;
};


bool operator < ( const xec_ssa_fvalue& a, const xec_ssa_fvalue& b )
{
    // Sort into priority queue so that lower-addressed ops appear first.
    return a.index > b.index;
}



struct xec_ssa_fscan
{
    std::unordered_map< xec_ssa_opref, int > prev;
    std::priority_queue< xec_ssa_fvalue > dead;
    std::priority_queue< xec_ssa_fvalue > live;
};



void xec_ssa_regalloc::forward_scan()
{
    xec_ssa_fscan fscan;
 
    
    // Go through in order and build list of live events.
    for ( size_t i = 0; i < dfo->size(); ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        forward_slice( &fscan, block->phi );
        forward_slice( &fscan, block->ops );
    }
    
    
    // All remaining values are dead in the end.
    while ( fscan.dead.size() )
    {
        xec_ssa_fvalue value = fscan.dead.top();
        fscan.dead.pop();
        
        event e;
        e.kind  = DEAD;
        e.at    = XEC_SSA_INVALID;
        e.value = value.value;
        e.prev  = -1;
        
        auto i = fscan.prev.find( value.value );
        if ( i != fscan.prev.end() )
        {
            e.prev = i->second;
        }

    }



}


void xec_ssa_regalloc::forward_slice(
                xec_ssa_fscan* scan, xec_ssa_slice* slice )
{
    for ( size_t i = 0; i < slice->ops.size(); ++i )
    {
        xec_ssa_op* op = &slice->ops.at( i );
        xec_ssa_opref opref;
        opref.slice = slice->index;
        opref.index = (int)i;
        uint64_t index = opindex( opref );


        // If this op introduces a new live range then add it to live.
        xec_ssa_opref opequiv = equiv( opref );
        if ( opequiv == opref && op->until &&
                ( op->opcode < XEC_SSA_FIRST_SET
                        || op->opcode > XEC_SSA_LAST_SET ) )
        {
            xec_ssa_fvalue fvalue;
            fvalue.index = index;
            fvalue.value = opequiv;
            fvalue.span  = opequiv;
            scan->live.push( fvalue );
        }


        // Kill any live values that are dead at this op.
        while ( scan->dead.size() && scan->dead.top().index <= index )
        {
            xec_ssa_fvalue value = scan->dead.top();
            xec_ssa_op* spanop = func->getop( value.span );
            scan->dead.pop();
            
            // Add dead/sleep event.
            event e;
            e.kind  = spanop->lnext ? SLEEP : DEAD;
            e.at    = opref;
            e.value = value.value;
            e.prev  = -1;
            
            auto i = scan->prev.find( value.value );
            if ( i != scan->prev.end() )
            {
                e.prev = i->second;
            }
            if ( e.kind != DEAD )
            {
                scan->prev[ value.value ] = (int)events.size();
            }
            events.push_back( e );
            
            // Add next live span to the live list.
            if ( spanop->lnext )
            {
                xec_ssa_fvalue fvalue;
                fvalue.index = opindex( spanop->lnext );
                fvalue.value = value.value;
                fvalue.span  = spanop->lnext;
                scan->live.push( fvalue );
            }
        }
        
        
        // Wake any values that are live at this op.
        while ( scan->live.size() && scan->live.top().index <= index )
        {
            xec_ssa_fvalue value = scan->live.top();
            xec_ssa_op* spanop = func->getop( value.span );
            scan->live.pop();
            
            // Add live/sleep event.
            event e;
            e.kind  = ( value.value == value.span ) ? LIVE : WAKE;
            e.at    = opref;
            e.value = value.value;
            e.prev  = -1;
            
            auto i = scan->prev.find( value.value );
            if ( i != scan->prev.end() )
            {
                e.prev = i->second;
            }
            scan->prev[ value.value ] = (int)events.size();
            events.push_back( e );
            
            // Skip dead/sleep events which are pointless.
            xec_ssa_opref span = value.span;
            while ( spanop->lnext && spanop->until == XEC_SSA_FOREVER )
            {
                xec_ssa_op* nextop = func->getop( spanop->lnext );
                xec_ssa_slice* spanslice =
                            func->slices.at( span.slice );
                xec_ssa_slice* nextslice =
                            func->slices.at( spanop->lnext.slice );
                
                if (    ( nextop->opcode == XEC_SSA_LIVE
                            || nextop->opcode == XEC_SSA_PHI )
                     && ( nextslice->block->index ==
                                spanslice->block->index + 1 ) )
                {
                    // Next live range begins at the beginning of the next
                    // block, no need to close and reopen.
                    span = spanop->lnext;
                    spanop = nextop;
                }
                else
                {
                    break;
                }
            }

            // Add end of live span to the dead list.
            xec_ssa_fvalue fvalue;
            if ( spanop->until != XEC_SSA_FOREVER )
            {
                fvalue.index = opindex( spanop->until );
            }
            else
            {
                // End of block.
                xec_ssa_slice* spanslice = func->slices.at( span.slice );
                xec_ssa_block* spanblock = spanslice->block;
                xec_ssa_opref until;
                until.slice = spanblock->ops->index;
                until.index = (int)spanblock->ops->ops.size();
                fvalue.index = opindex( until );
            }
            fvalue.value = value.value;
            fvalue.span  = span;
            scan->dead.push( fvalue );
        }
    }
        
}







void xec_ssa_regalloc::reverse_scan()
{
    // Go through the list of live events from the bottom up and actually
    // finally perform register allocation.
    
    



}





void xec_ssa_regalloc::print_events()
{
    for ( size_t i = 0; i < events.size(); ++i )
    {
        const event& e = events.at( i );
        
        printf( "%04X ", (int)i );
        if ( e.prev >= 0 )
        {
            printf( "^%04X ", e.prev );
        }
        else
        {
            printf( "      " );
        }
        printf( "%04X:%02X ", e.at.slice, e.at.index );
        switch ( e.kind )
        {
        case LIVE:      printf( "live  " );     break;
        case DEAD:      printf( "dead  " );     break;
        case SLEEP:     printf( "sleep " );     break;
        case WAKE:      printf( "wake  " );     break;
        }
        printf( "%04X:%02X\n", e.value.slice, e.value.index );
    }
}




