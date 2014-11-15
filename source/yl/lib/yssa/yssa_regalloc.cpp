//
//  yssa_regalloc.cpp
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yssa_regalloc.h"
#include "yssa_linear.h"
#include "yssa.h"



/*
    Check if the live ranges of two ops overlap.
*/

static bool interfere( yssa_linear* l, yssaop* a, yssaop* b )
{
    // Find linear ops for a and b.
    int aindex = a->index;
    int bindex = b->index;
    yssalop* alop = &l->lops[ aindex ];
    yssalop* blop = &l->lops[ bindex ];
    assert( alop->kind == YSSA_LOP_OP && alop->op == a );
    assert( blop->kind == YSSA_LOP_OP && blop->op == b );


    // Find heads of live span lists.
    aindex = alop->live_head;
    bindex = blop->live_head;

    if ( aindex > bindex )
    {
        std::swap( aindex, bindex );
    }


    // If either doesn't have a live range they don't interfere.
    if ( aindex == -1 || bindex == -1 )
    {
        return false;
    }


    // If they are the same value then they don't interfere.
    if ( aindex == bindex )
    {
        return false;
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


static void phi_equivalence( yssa_linear* l, yssaop* a, yssaop* b )
{
    // Can't merge ops if they interfere.
    if ( interfere( l, a, b ) )
    {
        return;
    }
    

    // Find linear ops for a and b.
    int aindex = a->index;
    int bindex = b->index;
    yssalop* alop = &l->lops[ aindex ];
    yssalop* blop = &l->lops[ bindex ];
    assert( alop->kind == YSSA_LOP_OP && alop->op == a );
    assert( blop->kind == YSSA_LOP_OP && blop->op == b );


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



static void phi_equivalence( yssa_linear* l )
{

    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
        yssalop& lop = l->lops.at( i );
        if ( lop.kind == YSSA_LOP_OP && lop.op->opcode == YSSA_PHI )
        {
            yssaop* phi = lop.op;
        
            // Attempt to merge the live ranges of this phi operation and
            // each of its operands.  Start with the last operand, as this
            // is likely to be at the 'bottom' of a loop and therefore the
            // most frequently encountered.
            
            for ( int i = phi->operand_count - 1; i >= 0; --i )
            {
                if ( phi->operand[ i ] )
                {
                    phi_equivalence( l, phi, phi->operand[ i ] );
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


struct yssa_revlist;
struct yssarev;


enum yssa_revkind
{
    YSSA_REV_LIVE,
    YSSA_REV_DEAD,
    YSSA_REV_WAKE,
    YSSA_REV_SLEEP,
    YSSA_REV_STACK,
};


struct yssa_revlist
{
    yssa_linear* l;
    std::vector< yssarev > revs;
};


struct yssarev
{
    yssarev( yssa_revkind kind, int head, int value, int at );

    yssa_revkind    kind;       // event kind.
    int             head;       // address of head of live span of value.
    int             value;      // value that is live or dead.
    int             at;         // address of event in linear.
};



yssarev::yssarev( yssa_revkind kind, int head, int value, int at )
    :   kind( kind )
    ,   head( head )
    ,   value( value )
    ,   at( at )
{
}




static bool has_stack_args( yssa_opcode opcode )
{
    return opcode == YSSA_EXTEND
        || opcode == YSSA_CALL
        || opcode == YSSA_YCALL
        || opcode == YSSA_YIELD
        || opcode == YSSA_RETURN;
}




static bool is_stack( yssaop* op )
{
    return op->result_count == -1
        || op->result_count > ( op->opcode != YSSA_NEXT ? 1 : 2 )
        || has_stack_args( op->opcode );
}




static void build_revlist( yssa_revlist* v, yssa_linear* l )
{
    v->l = l;
    
    // Maps linear indexes to the values which sleep or die at that index.
    std::unordered_multimap< int, int > sleepat;
    std::unordered_multimap< int, int > deadat;
    
    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
    
    yssalop* lop = &l->lops.at( i );
    int index = (int)i;
    
    
    // Kill all values dead at this index.
    auto ii = sleepat.equal_range( index );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        int value = i->second;
        int head = l->lops.at( value ).live_head;
        v->revs.emplace_back( YSSA_REV_SLEEP, head, value, index );
    }
    
    ii = deadat.equal_range( index );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        int value = i->second;
        int head = l->lops.at( value ).live_head;
        v->revs.emplace_back( YSSA_REV_DEAD, head, value, index );
    }
    
    
    // If this op requires command of the stack then emit a stack event.
    if ( lop->kind == YSSA_LOP_OP && is_stack( lop->op ) )
    {
        v->revs.emplace_back( YSSA_REV_STACK, -1, index, index );
    }
    
    
    // Block start and end ops don't have live ranges.
    if ( lop->kind == YSSA_LOP_BEGIN || lop->kind == YSSA_LOP_END )
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
    if ( lop->kind == YSSA_LOP_OP
            && lop->live_head == lop->op->index
            && lop->live_next == -1 )
    {
        // Any NEXT instruction (the real results must be selected).
        if ( lop->op->opcode == YSSA_NEXT )
        {
            continue;
        }
        
        // Any REFUP instruction which is the argument of a closure.  Might
        // be a better idea to change the representation of closures so these
        // REFUPs are inline.
        if ( lop->op->opcode == YSSA_REFUP )
        {
            yssalop* close = &l->lops.at( lop->live_until );
            if ( close->kind == YSSA_LOP_OP
                    && close->op->opcode == YSSA_CLOSURE
                    && close->op->operand_count
                    && close->op->operand[ 0 ] != lop->op )
            {
                continue;
            }
        }

        // Any instruction whose entire live range is filled with select ops.
        bool selected = true;
        for ( size_t i = index; i < l->lops.size(); ++i )
        {
            yssalop* sel = &l->lops.at( i );
            if ( sel->kind != YSSA_LOP_OP || sel->op->opcode != YSSA_SELECT )
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
    

    // Find index of value.  Note that due to phi-equivalence the value and
    // the head of the live range are not necessarily the same.
    int head = lop->live_head;
    int value = lop->op->index;


    // Add live or wake event for this value.
    if ( head == index )
    {
        v->revs.emplace_back( YSSA_REV_LIVE, head, value, index );
    }
    else
    {
        v->revs.emplace_back( YSSA_REV_WAKE, head, value, index );
    }
    
    
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




/*
    This is the part that actually does the register allocation.
*/


struct yssa_regasleep
{
    yssaop*         value;
    yssa_regasleep* prev;
};


struct yssa_regstate
{
    yssa_revlist*                   v;
    std::vector< yssaop* >          r;
    std::vector< yssa_regasleep* >  asleep;
};




static void ensure_reg( yssa_regstate* reg, int r )
{
    if ( reg->r.size() <= r )
    {
        reg->r.insert( reg->r.end(), r + 1 - reg->r.size(), nullptr );
    }
    
    if ( reg->asleep.size() <= r )
    {
        reg->asleep.insert( reg->asleep.end(), r + 1 - reg->asleep.size(), nullptr );
    }
}



static bool check_reg( yssa_regstate* reg, int r, yssaop* op )
{
    ensure_reg( reg, r );
    
    
    // Can't allocate into a register which is occupied.
    if ( reg->r[ r ] )
    {
        return false;
    }
    
    
    // Can't allocate into a register where sleeping values interfere with
    // the incoming value.
    for ( yssa_regasleep* asleep = reg->asleep[ r ];
                    asleep != nullptr; asleep = asleep->prev )
    {
        if ( interfere( reg->v->l, asleep->value, op ) )
        {
            return false;
        }
    }


    return true;
}



static int check_stack_argument( yssaop* stack, yssaop* arg )
{
    // Check if the op is an argument to a stack instruction, and if so
    // which register does it need to end up in.
    
    if ( ! has_stack_args( stack->opcode ) )
    {
        return -1;
    }
    
    assert( stack->stacktop >= 0 );
    
    for ( size_t i = 0; i < stack->operand_count; ++i )
    {
        if ( stack->operand[ i ] == arg )
        {
            return stack->stacktop + (int)i;
        }
    }
    
    if ( yssaop::has_multival( stack->opcode ) && stack->multival == arg )
    {
        return stack->stacktop + stack->operand_count;
    }
    
    return -1;
}




static void linear_scan( yssa_revlist* v )
{
    // Clear all register information.
    for ( size_t i = 0; i < v->l->lops.size(); ++i )
    {
        yssalop* lop = &v->l->lops.at( i );
        if ( lop->kind == YSSA_LOP_OP )
        {
            lop->op->r = -1;
            lop->op->stacktop = -1;
        }
    }


    // Each register can be free, or it can be allocated.  Values which are
    // asleep are pushed into a per-register stack.

    yssa_regstate reg;
    reg.v = v;


    // Go through the list of live events from the bottom up and actually
    // finally perform register allocation.  Note that because we do
    // allocation backwards DEAD <-> LIVE and SLEEP <-> WAKE.

    for ( int i = (int)v->revs.size() - 1; i >= 0; --i )
    {
    
    
    yssarev* rev = &v->revs.at( i );
    yssalop* value = &v->l->lops.at( rev->value );
    assert( value->kind == YSSA_LOP_OP );



    // If this is a stacklike instruction, then we must find the top
    // of the stack at this point.
    if ( rev->kind == YSSA_REV_STACK )
    {
        // Find index of highest non-empty register.
        int r = (int)reg.r.size() - 1;
        for ( ; r >= 0; --r )
        {
            if ( reg.r[ r ] != nullptr )
            {
                break;
            }
        }
        
        
        // And index of free slot at top of stack.
        r += 1;
        
        
        /*
            Stack instructions become live in the revlist after the rev that
            ssigns their stack top.  So by the time we get here it's likely
            that this value has already been allocated a register which is now
            free.
         
            Ensure that we prefer the stack location where we expect the
            results.  This is required to make multival/unpacks work correctly.
        */
        if ( value->op->r != (uint8_t)-1 )
        {
            r = std::max( (int)value->op->r, r );
        }
        
        
        // Assign stack top.
        value->op->stacktop = r;

        continue;
    }
    
    
    
    // Otherwise its a liveness event.
    yssalop* head = &v->l->lops.at( rev->head );
    yssalop* at = &v->l->lops.at( rev->at );

    assert( head->kind == YSSA_LOP_OP );
    assert( head->live_head == rev->head );
    assert( value->live_head == rev->head );
    
    
    switch ( rev->kind )
    {
    case YSSA_REV_DEAD: /* LIVE */
    {
        // Attempt to choose an ideal register.
        int r = -1;
        

        // If it's dead _at_ a stacklike instruction that takes it as one
        // of its stack arguments, attempt to allocate it to the register
        // that it needs to be in to make the stacklike instruction work.
        if ( r == -1 && at->kind == YSSA_LOP_OP )
        {
            r = check_stack_argument( at->op, value->op );
        }
    
    
        // If it's a parameter, then attempt to allocate it to the register
        // the parameter would be in.  Note that register 0 will be the
        // closure itself on entry to the function.
        if ( r == -1 && value->op->opcode == YSSA_PARAM )
        {
            r = 1 + value->op->immed;
        }
        
        
        // If we picked an ideal register, ensure we can allocate it.
        if ( r != -1 && ! check_reg( &reg, r, value->op ) )
        {
            r = -1;
        }
        
        
        // Otherwise, find lowest numbered free register.
        if ( r == -1 )
        {
            for ( r = 0; ; r += 1 )
            {
                if ( check_reg( &reg, r, value->op ) )
                {
                    break;
                }
            }
        }
        
    
        // Allocate register.
        ensure_reg( &reg, r );
        assert( ! reg.r[ r ] );
        reg.r[ r ] = value->op;


        // And assign to op.
        for ( int index = rev->head; index != -1; )
        {
            yssalop* lop = &v->l->lops.at( index );
            assert( lop->kind == YSSA_LOP_OP || lop->kind == YSSA_LOP_LIVE );
            assert( lop->live_head == rev->head );
            lop->op->r = r;
            index = lop->live_next;
        }

//        printf( "assign %d -> %d\n", rev->head, r );

        break;
    }
    
    case YSSA_REV_WAKE: /* SLEEP */
    {
        int r = value->op->r;
        assert( r != -1 );
        assert( reg.r[ r ] == value->op );

        // No longer live but sleeping.
        yssa_regasleep* asleep = new yssa_regasleep();
        asleep->value = value->op;
        asleep->prev = reg.asleep[ r ];
        reg.asleep[ r ] = asleep;
        reg.r[ r ] = nullptr;

//        printf( "sleep  %d -> %d\n", rev->head, r );

        break;
    }
    
    case YSSA_REV_SLEEP: /* WAKE */
    {
        int r = value->op->r;
        assert( r != -1 );
        assert( reg.r[ r ] == nullptr );
    
        // No longer sleeping but live.
        yssa_regasleep* asleep = reg.asleep[ r ];
        yssalop* wake = &v->l->lops.at( asleep->value->index );
        assert( wake->live_head == rev->head );
        reg.r[ r ] = value->op;
        reg.asleep[ r ] = asleep->prev;
        delete asleep;

//        printf( "wake   %d -> %d\n", rev->head, r );

        break;
    }
    
    
    case YSSA_REV_LIVE: /* DEAD */
    {
        int r = value->op->r;
        assert( r != -1 );
        assert( reg.r[ r ] == value->op );

        // Dead.
        reg.r[ r ] = nullptr;

//        printf( "dead   %d -> %d\n", rev->head, r );

        break;
    }
    
    
    
    case YSSA_REV_STACK:
    {
        assert( ! "unreachable" );
        break;
    }
    
    
    }
        
    
    }


    for ( size_t i = 0; i < reg.asleep.size(); ++i )
    {
        assert( reg.asleep.at( i ) == nullptr );
    }

}








void yssa_print( yssa_revlist* v )
{
    for ( size_t i = 0; i < v->revs.size(); ++i )
    {
    
        yssarev* rev = &v->revs.at( i );
        
        printf( "%04X ", (int)i );
        
        switch ( rev->kind )
        {
        case YSSA_REV_LIVE:     printf( "live " );  break;
        case YSSA_REV_DEAD:     printf( "dead " );  break;
        case YSSA_REV_WAKE:     printf( "wake " );  break;
        case YSSA_REV_SLEEP:    printf( "sleep" );  break;
        case YSSA_REV_STACK:    printf( "stack" );  break;
        }
        
        if ( rev->head != -1 )
            printf( " :%04X", rev->head );
        else
            printf( " :----" );
        
    
        if ( rev->value != -1 )
            printf( " :%04X", rev->value );
        else
            printf( " :----" );
        
        if ( rev->at != -1 )
            printf( " :%04X\n", rev->at );
        else
            printf( " :----\n" );
        
    }
}






void yssa_regalloc( yssa_linear* l )
{
    // Number all ops.
    for ( size_t i = 0; i < l->lops.size(); ++i )
    {
        yssalop& lop = l->lops.at( i );
        if ( lop.kind == YSSA_LOP_OP )
            lop.op->index = (int)i;
    }


    // Attempt as much phi equivalence as possible.
    phi_equivalence( l );
//    yssa_print( l );
    
    
    // Perform forward scan and generate live/sleep/wake/dead events.
    yssa_revlist revlist;
    build_revlist( &revlist, l );
//    yssa_print( &revlist );
    
    
    // Perform actual register allocation by scanning event buffer in reverse.
    linear_scan( &revlist );
    
}








