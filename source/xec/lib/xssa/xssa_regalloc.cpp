//
//  xssa_regalloc.cpp
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_regalloc.h"
#include "xssa_linear.h"
#include "xssa.h"



/*
    Check if the live ranges of two ops overlap.
*/

static bool interfere( xssa_linear* l, xssaop* a, xssaop* b )
{
    // Find linear ops for a and b.
    int aindex = a->index;
    int bindex = b->index;
    xssalop* alop = &l->lops[ aindex ];
    xssalop* blop = &l->lops[ bindex ];
    assert( alop->kind == XSSA_LOP_OP && alop->op == a );
    assert( blop->kind == XSSA_LOP_OP && blop->op == b );


    // Find heads of live span lists.
    aindex = alop->live_head;
    bindex = blop->live_head;

    if ( aindex > bindex )
    {
        std::swap( aindex, bindex );
    }


    // If either doesn't have a live range they don't interfere (I guess...)
    if ( aindex == -1 || bindex == -1 )
    {
        return false;
    }


    // If they share a live range then they interfere (I guess...).
    if ( aindex == bindex )
    {
        return true;
    }
    

    alop = &l->lops[ aindex ];
    blop = &l->lops[ bindex ];
    
    
    while ( true )
    {
        assert( alop != blop );
        
        
        // Ignore b spans that are wholly before the current a span.
        while ( blop->live_until <= aindex )
        {
            // Move to next b span.
            bindex = blop->live_next;
            if ( bindex == -1 )
                break;
            blop = &l->lops[ bindex ];
            continue;
        }
        
        
        // Check if the current a span is wholly before the b span.
        if ( alop->live_until <= bindex )
        {
            // Move to next a span.
            aindex = alop->live_next;
            if ( aindex == -1 )
                break;
            alop = &l->lops[ aindex ];
            continue;
        }
        
        
        // Otherwise, spans overlap.
        return true;
    }


    // Spans do not overlap.
    return false;
}




/*
    Build phi-equivalence classes by merging the live ranges of values
    referenced in phi ops.  The more often we can assign such values into
    the same register, the fewer moves are required when translating out.
*/


static void phi_equivalence( xssa_linear* l, xssaop* a, xssaop* b )
{
    // Can't merge ops if they interfere.
    if ( interfere( l, a, b ) )
    {
        printf( "PHI INTERFERE: :%04X :%04X\n", a->index, b->index );
        return;
    }
    

    // Find linear ops for a and b.
    int aindex = a->index;
    int bindex = b->index;
    xssalop* alop = &l->lops[ aindex ];
    xssalop* blop = &l->lops[ bindex ];
    assert( alop->kind == XSSA_LOP_OP && alop->op == a );
    assert( blop->kind == XSSA_LOP_OP && blop->op == b );


    // Find heads of live span lists.
    aindex = alop->live_head;
    bindex = blop->live_head;

    if ( aindex == bindex )
    {
        return;
    }

    if ( aindex > bindex )
    {
        std::swap( aindex, bindex );
    }
    
    
    printf( "PHI EQUIV: :%04X :%04X\n", aindex, bindex );


    alop = &l->lops[ aindex ];
    blop = &l->lops[ bindex ];
    

    // Add live ranges of b to a, and change the head index in b's spans to
    // point to a's head.
    
    while ( alop->live_next >= 0 )
    {
        if ( bindex < alop->live_next )
        {
            assert( blop->live_until <= alop->live_next );
            
            // This span in b comes between span a and the next span in a.
            // Splice it into the span list of a.
            int bnext = blop->live_next;
            blop->live_head = alop->live_head;
            blop->live_next = alop->live_next;
            alop->live_next = bindex;
            
            // The lowest span in a above all spans in b is the former b span.
            aindex = bindex;
            alop = blop;
            
            // Move to next b span.
            bindex = bnext;
            if ( bindex == -1 )
                break;
            blop = &l->lops.at( bindex );
        }
        else
        {
            assert( bindex != alop->live_next );
        
            // Span in b comes after the next span in a.
            aindex = alop->live_next;
            alop = &l->lops.at( aindex );
        }
    }
    
    while ( bindex >= 0 )
    {
        assert( alop->live_next == -1 );
        assert( bindex >= alop->live_until );
    
        blop = &l->lops.at( bindex );

        int bnext = blop->live_next;
        blop->live_head = alop->live_head;
        blop->live_next = -1;
        alop->live_next = bindex;
        
        aindex = bindex;
        alop = blop;
        
        bindex = bnext;
    }
    
}



static void phi_equivalence( xssa_linear* l )
{

    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
        xssalop& lop = l->lops.at( i );
        if ( lop.kind == XSSA_LOP_OP && lop.op->opcode == XSSA_PHI )
        {
            xssaop* phi = lop.op;
        
            // Attempt to merge the live ranges of this phi operation and
            // each of its operands.  Start with the last operand, as this
            // is likely to be at the 'bottom' of a loop and therefore the
            // most frequently encountered.
            
            for ( int i = phi->operand_count - 1; i >= 0; --i )
            {
                if ( phi->operands[ i ] )
                {
                    phi_equivalence( l, phi, phi->operands[ i ] );
                }
            }
            
        }
    
    }


}




/*
    Ours is a two-pass linear-scan-style register allocator.  First, we
    scan forwards to build an event list, indicating at which addresses
    values are live and dead.  'Holes' in the list are indicated with sleep
    and wake events.  Values which must command the stack top are picked out.
    
    Then the event list is scanned in reverse.  This allows us to us to place
    stack-op (e.g. calls) parameters after placing the stack-op itself,
    meaning many can be placed directly into the required location, requiring
    fewer moves (hopefully).  Doing it top-down would have allowed the same
    for stack-op results, but returning multiple results is far less common
    than taking multiple parameters.
*/


struct xssa_revlist;
struct xssarev;


enum xssa_revkind
{
    XSSA_REV_LIVE,
    XSSA_REV_DEAD,
    XSSA_REV_WAKE,
    XSSA_REV_SLEEP,
    XSSA_REV_STACK,
};


struct xssa_revlist
{
    xssa_linear* linear;
    std::vector< xssarev > revs;
};


struct xssarev
{
    xssarev( xssa_revkind kind, int value, int at, int prev );

    xssa_revkind    kind;       // event kind.
    int             value;      // address of head of live chain in linear.
    int             at;         // address of event in linear.
    int             prev;       // previous event for this value in revs.
};



xssarev::xssarev( xssa_revkind kind, int value, int at, int prev )
    :   kind( kind )
    ,   value( value )
    ,   at( at )
    ,   prev( prev )
{
}




static bool is_stack( xssaop* op )
{
    return op->result_count == -1
        || op->result_count > ( op->opcode != XSSA_NEXT ? 1 : 2 )
        || op->opcode == XSSA_EXTEND
        || op->opcode == XSSA_CALL
        || op->opcode == XSSA_YCALL
        || op->opcode == XSSA_YIELD
        || op->opcode == XSSA_RETURN;
}




static void build_revlist( xssa_revlist* v, xssa_linear* l )
{
    v->linear = l;
    
    // Maps values to the index of their previous occurrence in revs.
    std::unordered_map< int, int > prev;
    
    // Maps linear indexes to the values which sleep or die at that index.
    std::unordered_multimap< int, int > sleepat;
    std::unordered_multimap< int, int > deadat;
    
    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
    
    xssalop* lop = &l->lops.at( i );
    int index = (int)i;
    
    
    // Kill all values dead at this index.
    auto ii = sleepat.equal_range( index );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        int value = i->second;
        v->revs.emplace_back( XSSA_REV_SLEEP, value, index, prev.at( value ) );    // This is now the previous event for this value.
        prev[ value ] = (int)v->revs.size() - 1;
    }
    
    ii = deadat.equal_range( index );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        int value = i->second;
        v->revs.emplace_back( XSSA_REV_DEAD, value, index, prev.at( value ) );
        prev.erase( value );
    }
    
    
    // If this op requires command of the stack then emit a stack event.
    if ( lop->kind == XSSA_LOP_OP && is_stack( lop->op ) )
    {
        v->revs.emplace_back( XSSA_REV_STACK, -1, index, -1 );
    }
    
    
    // Block start and end ops don't have live ranges.
    if ( lop->kind == XSSA_LOP_BEGIN || lop->kind == XSSA_LOP_END )
    {
        continue;
    }
    
    
    // And some ops are actions without live ranges.
    if ( lop->live_head < 0 )
    {
        continue;
    }
    
    
    // Some instructions either do not place their results in registers or
    // are 'virtual' and will not end up as real instructions.
    if ( lop->kind == XSSA_LOP_OP
            && lop->live_head == lop->op->index
            && lop->live_next == -1 )
    {
        // Any NEXT instruction (the real results must be selected).
        if ( lop->op->opcode == XSSA_NEXT )
        {
            continue;
        }
        
        // Any REFUP instruction which is the argument of a closure.  Might
        // be a better idea to change the representation of closures so these
        // REFUPs are inline.
        if ( lop->op->opcode == XSSA_REFUP )
        {
            xssalop* close = &l->lops.at( lop->live_until );
            if ( close->kind == XSSA_LOP_OP
                    && close->op->opcode == XSSA_CLOSURE
                    && close->op->operand_count
                    && close->op->operands[ 0 ] != lop->op )
            {
                continue;
            }
        }

        // Any instruction whose entire live range is filled with select ops.
        bool selected = true;
        for ( size_t i = index; i < l->lops.size(); ++i )
        {
            xssalop* sel = &l->lops.at( i );
            if ( sel->kind != XSSA_LOP_OP || sel->op->opcode != XSSA_SELECT )
            {
                selected = false;
                break;
            }
        }
        
        if ( selected )
        {
            continue;
        }
    
    }

    
    // Add live or wake event for this value.
    int value = lop->live_head;
    auto j = prev.find( value );
    if ( j == prev.end() )
    {
        v->revs.emplace_back( XSSA_REV_LIVE, value, index, -1 );
    }
    else
    {
        v->revs.emplace_back( XSSA_REV_WAKE, value, index, j->second );
    }
    
    
    // This is now the previous event for this value.
    prev[ value ] = (int)v->revs.size() - 1;
    
    
    // Work out when the value dies/goes to sleep.
    assert( lop->live_until > index );
    if ( lop->live_next < 0 )
    {
        deadat.emplace( lop->live_until, value );
    }
    else
    {
        sleepat.emplace( lop->live_until, value );
    }
    
    
    
    }
    
    


}







void xssa_regalloc( xssa_linear* l )
{
    // Number all ops.
    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
        xssalop& lop = l->lops.at( i );
        if ( lop.kind == XSSA_LOP_OP )
            lop.op->index = (int)i;
    }


    // Attempt as much phi equivalence as possible.
    phi_equivalence( l );
    
    
    // Perform forward scan and generate live/sleep/wake/dead events.
    xssa_revlist revlist;
    build_revlist( &revlist, l );
    
    
    // Perform actual register allocation by scanning event buffer in reverse.
    
    
}








