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
#include <list>



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
    // meaning fewer moves (hopefully).  Doing it top-down would have allowed
    // the same for stack-op results, but returning multiple results is far
    // less common than taking multiple parameters.
    reverse_scan();
    
}



xec_ssa_opref xec_ssa_regalloc::equiv( xec_ssa_opref opref )
{
    opref = func->operand( opref );
    auto i = equivmap.find( opref );
    if ( i != equivmap.end() )
        return i->second;
    else
        return opref;
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




bool xec_ssa_regalloc::is_stack( xec_ssa_op* op )
{
    // Return true if the op compiles to an instruction that works on the
    // top of the register 'stack'.
    return is_stack_results( op ) || is_stack_args( op );
}

bool xec_ssa_regalloc::is_stack_results( xec_ssa_op* op )
{
    // Return true if this op compiles to an instruction which returns its
    // results on the stack.
    return ( op->opcode == XEC_SSA_VARARG && op->args->rcount == -1 )
        || ( op->opcode == XEC_SSA_UNPACK && op->args->rcount == -1 )
        || ( op->opcode == XEC_SSA_CALL )
        || ( op->opcode == XEC_SSA_YCALL )
        || ( op->opcode == XEC_SSA_YIELD )
        || ( op->opcode == XEC_SSA_NEXT &&
                    op->args->rcount != 1 && op->args->rcount != 2 );
}

bool xec_ssa_regalloc::is_stack_args( xec_ssa_op* op )
{
    // Return true if this op compiles to an instruction which takes multiple
    // arguments on the stack.
    return ( op->opcode == XEC_SSA_CALL )
        || ( op->opcode == XEC_SSA_YCALL )
        || ( op->opcode == XEC_SSA_YIELD )
        || ( op->opcode == XEC_SSA_EXTEND )
        || ( op->opcode == XEC_SSA_RETURN );
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
                xec_ssa_opref def = op.phi->definitions.at( i );
                attempt_equiv( phiref, def );
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



/*
    Forward scan over the live ranges, building events.
*/


struct xec_ssa_fvalue
{
    uint64_t        index;  // point at which value becomes live or dead.
    xec_ssa_opref   value;  // op that is live
    xec_ssa_opref   span;   // op that holds current live span.
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
        
        events.push_back( e );

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


        // Find until.
        xec_ssa_opref until = XEC_SSA_INVALID;
        if ( op->opcode < XEC_SSA_FIRST_SET || op->opcode > XEC_SSA_LAST_SET )
        {
            until = op->until;
        }



        // If this op introduces a new live range then add it to live.
        xec_ssa_opref opequiv = equiv( opref );
        if ( until && opref == opequiv )
        {
            xec_ssa_fvalue fvalue;
            fvalue.index = index;
            fvalue.value = opequiv;
            fvalue.span  = opequiv;
            scan->live.push( fvalue );
        }


        // Kill any values that are dead at this op.
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
        
        
        // If it's a stacklike op, we must find the stack top for it.  Also
        // next ops with more than two results return their results on the
        // top of the stack.
        if ( is_stack( op ) )
        {
            event e;
            e.kind  = STACK;
            e.at    = opref;
            e.value = opref;
            e.prev  = -1;
            events.push_back( e );
        }
        
        
        // Wake any values that are live at this op.
        while ( scan->live.size() && scan->live.top().index <= index )
        {
            xec_ssa_fvalue value = scan->live.top();
            xec_ssa_op* spanop = func->getop( value.span );
            scan->live.pop();
            
            // Add live/wake event.
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


struct xec_ssa_ralloc
{
    xec_ssa_opref   value;
    int             index;
    xec_ssa_ralloc* asleep;
};



void xec_ssa_regalloc::reverse_scan()
{
    // Go through the list of live events from the bottom up and actually
    // finally perform register allocation.  Note that because we do
    // allocation backwards DEAD <-> LIVE and SLEEP <-> WAKE.

    std::vector< xec_ssa_ralloc > allocs;
    
    for ( int i = (int)events.size() - 1; i >= 0; --i )
    {
        const event& e = events.at( i );
        xec_ssa_op* op = func->getop( e.at );
        
        switch ( e.kind )
        {
        case DEAD: /* LIVE */
        {
            int r = -1;
        
            // If it's dead _at_ a stacklike instruction that takes it as one
            // of its stack arguments, attempt to allocate it to the register
            // that it needs to be in to make the stacklike instruction work.
            if ( op && is_stack_args( op ) )
            {
                assert( op->args->stacktop != -1 );
                assert( op->opcode >= XEC_SSA_FIRST_ARG
                            && op->opcode <= XEC_SSA_LAST_ARG );

                // Find register.
                int stackr = -1;
                for ( size_t i = 0; i < op->args->args.size(); ++i )
                {
                    xec_ssa_opref operand = op->args->args.at( i );
                    operand = func->operand( operand );
                    if ( operand == e.value )
                    {
                        stackr = op->args->stacktop + (int)i;
                        break;
                    }
                }
                if ( op->args->unpacked == e.value )
                {
                    stackr = op->args->stacktop + (int)op->args->args.size();
                }
                
                
                // Check if the desired register is in use.
                if ( stackr != -1 )
                {
                    r = check_alloc( allocs, stackr, i );
                }
            }
            
            
            // If the value is a parameter, then attempt to allocate it the
            // register the parameter would be in.
            xec_ssa_op* valop = func->getop( e.value );
            if ( r == -1 && valop->opcode == XEC_SSA_PARAM )
            {
                r = check_alloc( allocs, valop->immkey + 1, i );
            }
            
            
            // Find first value where we can allocate this value.
            for ( int searchr = 0;
                        r == -1 && searchr < allocs.size(); ++searchr )
            {
                r = check_alloc( allocs, searchr, i );
            }
            
            if ( r == -1 )
            {
                r = (int)allocs.size();
            }
            
            while ( r >= allocs.size() )
            {
                xec_ssa_ralloc empty;
                empty.value  = XEC_SSA_INVALID;
                empty.index  = -1;
                empty.asleep = NULL;
                allocs.push_back( empty );
            }
            
            // Actually allocate.
            xec_ssa_ralloc* alloc = &allocs.at( r );
            alloc->value = e.value;
            alloc->index = i;
        
            break;
        }
        
        case WAKE: /* SLEEP */
        {
            // No longer live but sleeping.
            int r = find_alloc( allocs, e.value );
            xec_ssa_ralloc* alloc = &allocs.at( r );
            alloc->asleep = new xec_ssa_ralloc( *alloc );
            alloc->value  = XEC_SSA_INVALID;
            alloc->index  = -1;
            break;
        }
        
        case SLEEP: /* WAKE */
        {
            // No longer sleeping but live.
            int r = find_asleep( allocs, e.value );
            xec_ssa_ralloc* alloc = &allocs.at( r );
            assert( ! alloc->value );
            xec_ssa_ralloc* asleep = alloc->asleep;
            *alloc = *asleep;
            delete asleep;
            break;
        }
        
        case LIVE: /* DEAD */
        {
            // Dead.
            int r = find_alloc( allocs, e.value );
            xec_ssa_ralloc* alloc = &allocs.at( r );
            assert( alloc->value );
            alloc->value  = XEC_SSA_INVALID;
            alloc->index  = -1;
            
            // But remember register.
            op->r = r;
            break;
        }
                
        case STACK:
        {
            // If this is a stacklike instruction, then we must find the top
            // of the stack at this point.  However if it's _also_ a mark
            // instruction then it should already have been allocated the
            // 'stack top' it requires as the SSA ops should be ordered such
            // that it dies at the next stacklike instruction.
            
            assert( op->opcode >= XEC_SSA_FIRST_ARG
                        && op->opcode <= XEC_SSA_LAST_ARG );

            int r = (int)allocs.size() - 1;
            for ( ; r >= 0; --r )
            {
                xec_ssa_ralloc* alloc = &allocs.at( r );
                if ( alloc->value )
                {
                    // It's live.
                    break;
                }
            }
            
            op->args->stacktop = r + 1;
            break;
        }
        }
    
    }
    
    
    
    // Ensure that all equiv ops get their registers set.
    for ( auto i = equivmap.begin(); i != equivmap.end(); ++i )
    {
        xec_ssa_op* op = func->getop( i->first );
        xec_ssa_op* equiv = func->getop( i->second );
  
        assert( op->r == -1 );
        assert( equiv->r != -1 );
/*
        printf( "%04X:%02X (%2d) -> %04X:%02X (%2d)\n",
                i->first.slice, i->first.index, op->r,
                i->second.slice, i->second.index, equiv->r );
*/
        op->r = equiv->r;
    }
    

}


int xec_ssa_regalloc::check_alloc(
            const std::vector< xec_ssa_ralloc >& allocs, int r, int index )
{
    if ( r >= allocs.size() )
    {
        // Register hasn't been allocated ever, so it's fine.
        return r;
    }

    // Check alloc.
    const xec_ssa_ralloc* alloc = &allocs.at( r );
    
    if ( alloc->value )
    {
        // Alloc is live.
        return -1;
    }
 
    for ( xec_ssa_ralloc* asleep = alloc->asleep;
                    asleep; asleep = asleep->asleep )
    {
        // Check if new value interferes with asleep value.
        if ( interfere( asleep->index, index ) )
        {
            return -1;
        }
    }
    
    return r;
}


int xec_ssa_regalloc::find_alloc(
            const std::vector< xec_ssa_ralloc >& allocs, xec_ssa_opref value )
{
    for ( int i = 0; i < allocs.size(); ++i )
    {
        const xec_ssa_ralloc* alloc = &allocs.at( i );
        if ( alloc->value == value )
        {
            return i;
        }
    }
    return -1;
}




int xec_ssa_regalloc::find_asleep(
            const std::vector< xec_ssa_ralloc >& allocs, xec_ssa_opref value )
{
    for ( int i = 0; i < allocs.size(); ++i )
    {
        const xec_ssa_ralloc* alloc = &allocs.at( i );
        if ( alloc->asleep && alloc->asleep->value == value )
        {
            return i;
        }
    }
    return -1;
}





bool xec_ssa_regalloc::interfere( int a, int b )
{
    xec_ssa_opref aref = events[ a ].value;
    xec_ssa_opref bref = events[ b ].value;

    assert( events[ a ].kind == DEAD );
    assert( events[ b ].kind == DEAD );
    int aprev = events[ a ].prev;
    int bprev = events[ b ].prev;
    assert( aprev != -1 );
    assert( bprev != -1 );

    /*
        Spans:
            aprev -> a
            bprev -> b
    */
    
    // Check each span in a.
    while ( true )
    {
        assert( a != b );
        
        // Ignore spans in b that are wholly after the a span.
        if ( bprev > a )
        {
            // Move onto previous b span.
            b = events[ bprev ].prev;
            if ( b == -1 )
            {
                break;
            }
            assert( events[ b ].kind == SLEEP );
            bprev = events[ b ].prev;
            assert( bprev != -1 );
            continue;
        }
        
            
        // If the a span is wholly after the b span.
        if ( aprev > b )
        {
            // Move onto previous a span.
            a = events[ aprev ].prev;
            if ( a == -1 )
            {
                break;
            }
            assert( events[ a ].kind == SLEEP );
            aprev = events[ a ].prev;
            assert( aprev != -1 );
            continue;
        }
            
            
        // Otherwise the spans overlap.
        return true;
    }
    
    
    // Ranges do not overlap.
    assert( ! interfere( aref, bref ) );
    return false;
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
        case STACK:     printf( "stack " );     break;
        }
        printf( "%04X:%02X\n", e.value.slice, e.value.index );
    }
}




