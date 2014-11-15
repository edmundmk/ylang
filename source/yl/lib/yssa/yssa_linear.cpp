//
//  yssa_linear.cpp
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yssa_linear.h"
#include "yssa.h"




yssalop::yssalop( yssa_lopkind kind, yssa_block* block )
    :   live_head( -1 )
    ,   live_next( -1 )
    ,   live_until( -1 )
    ,   kind( kind )
    ,   block( block )
{
}

yssalop::yssalop( yssa_lopkind kind, yssaop* op )
    :   live_head( -1 )
    ,   live_next( -1 )
    ,   live_until( -1 )
    ,   kind( kind )
    ,   op( op )
{
}



/*
    Ops that have side effects.
*/

static bool is_action( yssa_opcode opcode )
{
    return opcode == YSSA_SETKEY
        || opcode == YSSA_SETINKEY
        || opcode == YSSA_SETINDEX
        || opcode == YSSA_SETGLOBAL
        || opcode == YSSA_DELKEY
        || opcode == YSSA_DELINKEY
        || opcode == YSSA_NEWUP
        || opcode == YSSA_SETUP
        || opcode == YSSA_CLOUP
        || opcode == YSSA_APPEND
        || opcode == YSSA_EXTEND
        || opcode == YSSA_CALL
        || opcode == YSSA_YCALL
        || opcode == YSSA_YIELD
        || opcode == YSSA_ITER
        || opcode == YSSA_ITERKEY
        || opcode == YSSA_POPITER
        || opcode == YSSA_NEXT
        || opcode == YSSA_RETURN;
}




/*
    Build linear liveness spans.
*/

void yssa_build_linear( yssa_linear* linear, yssa_func* func )
{
    linear->func = func;
    
    
    // Track liveness.
    std::unordered_map< yssaop*, int > prev;
    std::unordered_map< yssaop*, int > open;
    
    
    // Assume that func is in depth-first order and has liveness information.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
    
    yssa_block* block = func->blocks.at( i ).get();
    

    
    
    // Find last occurrence of each operand in the block.  Remember that
    // emplace will not overwrite an existing entry in the map.
    std::unordered_map< yssaop*, yssaop* > last;
    
    for ( auto i = block->live_out.begin(); i != block->live_out.end(); ++i )
    {
        last.emplace( *i, nullptr );
    }

    for ( int i = (int)block->ops.size() - 1; i >= 0; --i )
    {
        yssaop* op = block->ops.at( i );
        if ( ! op )
            continue;
        
        for ( size_t i = 0; i < op->operand_count; ++i )
        {
            if ( op->operand[ i ] )
            {
                last.emplace( op->operand[ i ], op );
            }
        }
        
        if ( yssaop::has_multival( op->opcode ) && op->multival )
        {
            last.emplace( op->multival, op );
        }
    }
    
    
    
    // Invert map.
    std::unordered_multimap< yssaop*, yssaop* > diesat;
    for ( auto i = last.begin(); i != last.end(); ++i )
    {
        if ( i->second )
            diesat.emplace( i->second, i->first );
    }
    


    // Begin block.
    linear->lops.emplace_back( YSSA_LOP_BEGIN, block );
        
    
    
    // Add phi ops.
    for ( size_t i = 0; i < block->phi.size(); ++i )
    {
        yssaop* op = block->phi.at( i );
        if ( ! op )
            continue;
    
        assert( op->opcode == YSSA_PHI );
        
        
        // Add op.
        int index = (int)linear->lops.size();
        linear->lops.emplace_back( YSSA_LOP_OP, op );
        yssalop& lop = linear->lops.back();
    
    
        // A phi op should come before all of its references, even with loops.
        assert( open.find( op ) == open.end() );
        assert( prev.find( op ) == prev.end() );
        lop.live_head = index;
        open.emplace( op, index );
        prev[ op ] = index;
    }


    
    // Add live spans for each op which is now live due to the block opening.
    for ( auto i = block->live_in.begin(); i != block->live_in.end(); ++i )
    {
        yssaop* op = *i;
        if ( ! op )
            continue;
        
        if ( open.find( op ) == open.end() )
        {
            // Add new span.
            int index = (int)linear->lops.size();
            linear->lops.emplace_back( YSSA_LOP_LIVE, op );
            yssalop& lop = linear->lops.back();


            // If there was a previous span for this op, link to it.
            auto i = prev.find( op );
            if ( i != prev.end() )
            {
                yssalop& prev_lop = linear->lops.at( i->second );
                lop.live_head = prev_lop.live_head;
                prev_lop.live_next = index;
            }
            else
            {
                lop.live_head = index;
            }
            

            // Op is now live, and we haven't reached the end of the span.
            open.emplace( op, index );
            prev[ op ] = index;
        }
    }
    

    // Go through each op.
    for ( size_t i = 0; i < block->ops.size(); ++i )
    {
        yssaop* op = block->ops.at( i );
        if ( ! op )
            continue;
        
        
        // Close spans of all open ops which die at this op.
        auto ii = diesat.equal_range( op );
        for ( auto i = ii.first; i != ii.second; ++i )
        {
            int open_index = open.at( i->second );
            yssalop& open_lop = linear->lops.at( open_index );
            open_lop.live_until = (int)linear->lops.size();
            open.erase( i->second );
        }


        if ( last.find( op ) != last.end() )
        {
            // Add op.
            int index = (int)linear->lops.size();
            linear->lops.emplace_back( YSSA_LOP_OP, op );
            yssalop& lop = linear->lops.back();
            
            // An op should come before all its references, since the CFG
            // is reducible and there are no undefined variables.
            assert( open.find( op ) == open.end() );
            assert( prev.find( op ) == prev.end() );
            lop.live_head = index;
            open.emplace( op, index );
            prev[ op ] = index;
            
        }
        else
        {
            // Op is not referenced.  Don't open it.  And only output it
            // at all if it has side effects.
            if ( is_action( op->opcode ) )
            {
                linear->lops.emplace_back( YSSA_LOP_OP, op );
            }
        }
        

    
    }
    
    
    
    // Close all open live spans which are not live at the head of next block.
    yssa_block* next = nullptr;
    if ( i + 1 < func->blocks.size() )
        next = func->blocks.at( i + 1 ).get();
        
    for ( auto i = open.begin(); i != open.end(); )
    {
        yssaop* op = i->first;
        yssalop& lop = linear->lops.at( i->second );
        
        if ( ! next || next->live_in.find( op ) == next->live_in.end() )
        {
            lop.live_until = (int)linear->lops.size();
            i = open.erase( i );
        }
        else
        {
            ++i;
        }
    }
    

    // End of block.
    linear->lops.emplace_back( YSSA_LOP_END, block );
    

    
    
    }
    
 
    assert( open.empty() );

}


static void print_liveness( yssalop* lop )
{
    if ( lop->live_head >= 0 )
        printf( "|%04X", lop->live_head );
    else
        printf( "|----" );
    
    if ( lop->live_next >= 0 )
        printf( ":%04X", lop->live_next );
    else
        printf( ":----" );
    
    if ( lop->live_until >= 0 )
        printf( ":%04X|", lop->live_until );
    else
        printf( ":----|" );
}


void yssa_print( yssa_linear* linear )
{
    // Number all ops.
    for ( size_t i = 0; i < linear->lops.size(); ++i )
    {
        yssalop& lop = linear->lops.at( i );
        if ( lop.kind == YSSA_LOP_OP )
            lop.op->index = (int)i;
    }
    
    
    // Write out.
    yssa_func* func = linear->func;
    printf( "function %p %s\n", func, func->funcname );
    printf( "    upvalcount : %d\n", func->upvalcount );
    printf( "    newupcount : %d\n", func->newupcount );
    printf( "    paramcount : %d\n", func->paramcount );
    printf( "    varargs    : %s\n", func->varargs ? "true" : "false" );
    printf( "    coroutine  : %s\n", func->coroutine ? "true" : "false" );
    printf( "\n" );
    
    for ( size_t i = 0; i < linear->lops.size(); ++i )
    {
        yssalop& lop = linear->lops.at( i );
        
        printf( "%04X ", (int)i );

        switch ( lop.kind )
        {
        case YSSA_LOP_BEGIN:
        {
            printf( "  [%04X]\n", lop.block->index );
            break;
        }
        
        case YSSA_LOP_END:
        {
            if ( lop.block->condition )
            {
                printf
                (
                    "  :%04X ? >[%04X] : >[%04X]\n",
                    lop.block->condition->index,
                    lop.block->iftrue->index,
                    lop.block->iffalse->index
                );
            }
            else if ( lop.block->next )
            {
                printf( "  >[%04X]\n", lop.block->next->index );
            }
            break;
        }
        
        case YSSA_LOP_LIVE:
        {
            print_liveness( &lop );
            printf( " (live)\n" );
            break;
        }
        
        
        case YSSA_LOP_OP:
        {
            print_liveness( &lop );
            printf( " " );
            yssa_print( linear->func, lop.op );
            break;
        }
        }
        
    }

    printf( "\n\n" );

}

