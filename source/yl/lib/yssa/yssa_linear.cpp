//
//  xssa_linear.cpp
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_linear.h"
#include "xssa.h"




xssalop::xssalop( xssa_lopkind kind, xssa_block* block )
    :   live_head( -1 )
    ,   live_next( -1 )
    ,   live_until( -1 )
    ,   kind( kind )
    ,   block( block )
{
}

xssalop::xssalop( xssa_lopkind kind, xssaop* op )
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

static bool is_action( xssa_opcode opcode )
{
    return opcode == XSSA_SETKEY
        || opcode == XSSA_SETINKEY
        || opcode == XSSA_SETINDEX
        || opcode == XSSA_SETGLOBAL
        || opcode == XSSA_DELKEY
        || opcode == XSSA_DELINKEY
        || opcode == XSSA_NEWUP
        || opcode == XSSA_SETUP
        || opcode == XSSA_CLOUP
        || opcode == XSSA_APPEND
        || opcode == XSSA_EXTEND
        || opcode == XSSA_CALL
        || opcode == XSSA_YCALL
        || opcode == XSSA_YIELD
        || opcode == XSSA_ITER
        || opcode == XSSA_ITERKEY
        || opcode == XSSA_POPITER
        || opcode == XSSA_NEXT
        || opcode == XSSA_RETURN;
}




/*
    Build linear liveness spans.
*/

void xssa_build_linear( xssa_linear* linear, xssa_func* func )
{
    linear->func = func;
    
    
    // Track liveness.
    std::unordered_map< xssaop*, int > prev;
    std::unordered_map< xssaop*, int > open;
    
    
    // Assume that func is in depth-first order and has liveness information.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
    
    xssa_block* block = func->blocks.at( i ).get();
    

    
    
    // Find last occurrence of each operand in the block.  Remember that
    // emplace will not overwrite an existing entry in the map.
    std::unordered_map< xssaop*, xssaop* > last;
    
    for ( auto i = block->live_out.begin(); i != block->live_out.end(); ++i )
    {
        last.emplace( *i, nullptr );
    }

    for ( int i = (int)block->ops.size() - 1; i >= 0; --i )
    {
        xssaop* op = block->ops.at( i );
        if ( ! op )
            continue;
        
        for ( size_t i = 0; i < op->operand_count; ++i )
        {
            if ( op->operand[ i ] )
            {
                last.emplace( op->operand[ i ], op );
            }
        }
        
        if ( xssaop::has_multival( op->opcode ) && op->multival )
        {
            last.emplace( op->multival, op );
        }
    }
    
    
    
    // Invert map.
    std::unordered_multimap< xssaop*, xssaop* > diesat;
    for ( auto i = last.begin(); i != last.end(); ++i )
    {
        if ( i->second )
            diesat.emplace( i->second, i->first );
    }
    


    // Begin block.
    linear->lops.emplace_back( XSSA_LOP_BEGIN, block );
        
    
    
    // Add phi ops.
    for ( size_t i = 0; i < block->phi.size(); ++i )
    {
        xssaop* op = block->phi.at( i );
        if ( ! op )
            continue;
    
        assert( op->opcode == XSSA_PHI );
        
        
        // Add op.
        int index = (int)linear->lops.size();
        linear->lops.emplace_back( XSSA_LOP_OP, op );
        xssalop& lop = linear->lops.back();
    
    
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
        xssaop* op = *i;
        if ( ! op )
            continue;
        
        if ( open.find( op ) == open.end() )
        {
            // Add new span.
            int index = (int)linear->lops.size();
            linear->lops.emplace_back( XSSA_LOP_LIVE, op );
            xssalop& lop = linear->lops.back();


            // If there was a previous span for this op, link to it.
            auto i = prev.find( op );
            if ( i != prev.end() )
            {
                xssalop& prev_lop = linear->lops.at( i->second );
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
        xssaop* op = block->ops.at( i );
        if ( ! op )
            continue;
        
        
        // Close spans of all open ops which die at this op.
        auto ii = diesat.equal_range( op );
        for ( auto i = ii.first; i != ii.second; ++i )
        {
            int open_index = open.at( i->second );
            xssalop& open_lop = linear->lops.at( open_index );
            open_lop.live_until = (int)linear->lops.size();
            open.erase( i->second );
        }


        if ( last.find( op ) != last.end() )
        {
            // Add op.
            int index = (int)linear->lops.size();
            linear->lops.emplace_back( XSSA_LOP_OP, op );
            xssalop& lop = linear->lops.back();
            
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
                linear->lops.emplace_back( XSSA_LOP_OP, op );
            }
        }
        

    
    }
    
    
    
    // Close all open live spans which are not live at the head of next block.
    xssa_block* next = nullptr;
    if ( i + 1 < func->blocks.size() )
        next = func->blocks.at( i + 1 ).get();
        
    for ( auto i = open.begin(); i != open.end(); )
    {
        xssaop* op = i->first;
        xssalop& lop = linear->lops.at( i->second );
        
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
    linear->lops.emplace_back( XSSA_LOP_END, block );
    

    
    
    }
    
 
    assert( open.empty() );

}


static void print_liveness( xssalop* lop )
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


void xssa_print( xssa_linear* linear )
{
    // Number all ops.
    for ( size_t i = 0; i < linear->lops.size(); ++i )
    {
        xssalop& lop = linear->lops.at( i );
        if ( lop.kind == XSSA_LOP_OP )
            lop.op->index = (int)i;
    }
    
    
    // Write out.
    xssa_func* func = linear->func;
    printf( "function %p %s\n", func, func->funcname );
    printf( "    upvalcount : %d\n", func->upvalcount );
    printf( "    newupcount : %d\n", func->newupcount );
    printf( "    paramcount : %d\n", func->paramcount );
    printf( "    varargs    : %s\n", func->varargs ? "true" : "false" );
    printf( "    coroutine  : %s\n", func->coroutine ? "true" : "false" );
    printf( "\n" );
    
    for ( size_t i = 0; i < linear->lops.size(); ++i )
    {
        xssalop& lop = linear->lops.at( i );
        
        printf( "%04X ", (int)i );

        switch ( lop.kind )
        {
        case XSSA_LOP_BEGIN:
        {
            printf( "  [%04X]\n", lop.block->index );
            break;
        }
        
        case XSSA_LOP_END:
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
        
        case XSSA_LOP_LIVE:
        {
            print_liveness( &lop );
            printf( " (live)\n" );
            break;
        }
        
        
        case XSSA_LOP_OP:
        {
            print_liveness( &lop );
            printf( " " );
            xssa_print( linear->func, lop.op );
            break;
        }
        }
        
    }

    printf( "\n\n" );

}

