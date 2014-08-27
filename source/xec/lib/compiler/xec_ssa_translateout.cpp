//
//  xec_ssa_translateout.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_translateout.h"
#include "xec_ssa.h"
#include "xec_ssa_cfganalysis.h"
#include "xec_string.h"





struct xec_ssa_rtgraph
{
    std::vector< std::pair< int, int > > moves;
};





xec_ssa_translateout::xec_ssa_translateout( xec_ssa* root )
    :   root( root )
{
}



void xec_ssa_translateout::translateout( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    i.clear();
    
    this->func = func;
    this->dfo  = dfo;

    // And with all blocks.
    for ( size_t i = 0; i < dfo->size(); ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        translateout( block );
    }
}



void xec_ssa_translateout::translateout( xec_ssa_block* block )
{
    for ( size_t i = 0; i < block->ops->ops.size(); ++i )
    {
        xec_ssa_op* op = &block->ops->ops.at( i );
        
        switch ( op->opcode )
        {
        case XEC_SSA_NOP:
            break;
        case XEC_SSA_REF:
            break;
        
        case XEC_SSA_NULL:
            inst( XEC_NULL, op->r, 0, 0 );
            break;
            
        case XEC_SSA_NUMBER:
        {
            xec_value v = xec_value( op->number );
            
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                if ( values.at( i ) == v )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                values.push_back( v );
            }
            
            inst( XEC_VALUE, op->r, (int)i );
            break;
        }
        
        case XEC_SSA_BOOL:
        {
            xec_value v = xec_value( op->boolean );
        
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                if ( values.at( i ) == v )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                values.push_back( v );
            }
            
            inst( XEC_VALUE, op->r, (int)i );
            break;
        }
        
        case XEC_SSA_STRING:
        {
            xec_ssa_string* s = op->string;
        
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                xec_value v = values.at( i );
                if ( v.isstring() && v.string()->size() == s->length &&
                    memcmp( v.string()->c_str(), s->string, s->length ) == 0 )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                xec_string* t = xec_string::create( s->string, s->length );
                values.push_back( t );
            }
            
            inst( XEC_VALUE, op->r, (int)i );
            break;
        }
        
        case XEC_SSA_POS:
            inst( XEC_POS, op->r, o( op->operanda ), 0 );
            break;
        case XEC_SSA_NEG:
            inst( XEC_NEG, op->r, o( op->operanda ), 0 );
            break;
        case XEC_SSA_NOT:
            inst( XEC_LNOT, op->r, o( op->operanda ), 0 );
            break;
        case XEC_SSA_BITNOT:
            inst( XEC_NOT, op->r, o( op->operanda ), 0 );
            break;
        
        case XEC_SSA_MUL:
            inst( XEC_MUL, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_DIV:
            inst( XEC_DIV, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_MOD:
            inst( XEC_DIV, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_INTDIV:
            inst( XEC_INTDIV, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_ADD:
            inst( XEC_ADD, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_SUB:
            inst( XEC_SUB, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LSL:
            inst( XEC_LSL, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LSR:
            inst( XEC_LSR, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_ASR:
            inst( XEC_ASR, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITAND:
            inst( XEC_AND, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITXOR:
            inst( XEC_XOR, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITOR:
            inst( XEC_OR, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_CONCAT:
            inst( XEC_CONCAT, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        
        case XEC_SSA_EQ:
            inst( XEC_EQ, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LT:
            inst( XEC_LT, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LE:
            inst( XEC_LE, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_IN:
            inst( XEC_IN, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_IS:
            inst( XEC_IS, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_XOR:
            inst( XEC_LXOR, op->r, o( op->operanda ), o( op->operandb ) );
            break;

        case XEC_SSA_INKEY:
            inst( XEC_INKEY, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_INDEX:
            inst( XEC_INDEX, op->r, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_DELINKEY:
            inst( XEC_DELINKEY, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_OBJECT:
            inst( XEC_OBJECT, op->r, o( op->operanda ), 0 );
            break;
        case XEC_SSA_ITER:
            inst( XEC_ITER, op->r, o( op->operanda ), 0 );
            break;
        case XEC_SSA_EACH:
            inst( XEC_ITERKEY, op->r, o( op->operanda ), 0 );
            break;

        case XEC_SSA_APPEND:
            inst( XEC_ITERKEY, o( op->operandb ), o( op->operanda ), 0 );
            break;
        
        case XEC_SSA_KEY:
            inst( XEC_KEY, op->r, o( op->operanda ), k( op->immkey ) );
            break;
        
        case XEC_SSA_DELKEY:
            inst( XEC_DELKEY, 0, o( op->operanda ), k( op->immkey ) );
            break;
            
        case XEC_SSA_GLOBAL:
            inst( XEC_GLOBAL, op->r, 0, k( op->immkey ) );
            break;
        
        case XEC_SSA_SETGLOBAL:
            inst( XEC_SETGLOBAL, o( op->operanda ), 0, k( op->immkey ) );
            break;
            
        case XEC_SSA_PARAM:
        {
            // Build register transfer graph to get parameters in correct
            // registers.
            xec_ssa_rtgraph rtg;
            while ( op->opcode == XEC_SSA_PARAM && i < block->ops->ops.size() )
            {
                rtg.moves.emplace_back( op->immkey, (int)op->r );
                ++i;
            }
            --i;
            move( &rtg );
            break;
        }
        
        case XEC_SSA_SELECT:
            // Will have compiled selects as part of the op they select from.
            break;
            
        case XEC_SSA_NEWUP:
            inst( XEC_NEWUP, o( op->operanda ), op->immkey );
            break;
        
        case XEC_SSA_SETUP:
            inst( XEC_SETUP, o( op->operanda ), op->immkey );
            break;
            
        case XEC_SSA_REFUP:
            inst( XEC_REFUP, op->r, op->immkey );
            break;
            
        case XEC_SSA_CLOSE:
        {
            // Build ranges of upvals to close from adjacent close ops.
            std::unordered_set< int > close;
            int minclose = INT_MAX;
            int maxclose = INT_MIN;
            
            while ( op->opcode == XEC_SSA_CLOSE && i < block->ops->ops.size() )
            {
                close.insert( op->immkey );
                minclose = std::min( minclose, op->immkey );
                maxclose = std::max( maxclose, op->immkey );
                op = &block->ops->ops.at( i );
                ++i;
            }
            --i;
            
            for ( int c = minclose; c < maxclose; ++c )
            {
                if ( close.find( c ) == close.end() )
                {
                    continue;
                }
                
                int start = c;
                while ( close.find( c ) != close.end() )
                {
                    ++c;
                }
                
                inst( XEC_CLOSE, ( c - 1 ) - start, start );
            }
            
            break;
        }
        
        case XEC_SSA_ARRAY:
            inst( XEC_ARRAY, op->r, op->immkey );
            break;
            
        case XEC_SSA_TABLE:
            inst( XEC_TABLE, op->r, op->immkey );
            break;

        case XEC_SSA_SETINKEY:
            inst( XEC_SETINKEY, o( op->operandv ),
                            o( op->operanda ), o( op->operandb ) );
            break;
            
        case XEC_SSA_SETINDEX:
            inst( XEC_SETINDEX, o( op->operandv ),
                            o( op->operanda ), o( op->operandb ) );
            break;
            
        case XEC_SSA_SETKEY:
            inst( XEC_SETKEY, o( op->operandv ),
                            o( op->operanda ), k( op->immkey ) );
            break;
            
        case XEC_SSA_VARARG:
        {
            if ( op->args->rcount == -1 )
            {
                // Stacklike unpack of all varargs.
                assert( op->r == op->args->stacktop );
                inst( XEC_VARALL, op->r, 0 );
            }
            else
            {
                // Recover single vararg.
                inst( XEC_VARARG, op->r, op->args->rcount );
            }
            break;
        }
        
        case XEC_SSA_UNPACK:
        {
            if ( op->args->rcount == -1 )
            {
                // Stacklike unpack of all results.
                assert( op->r == op->args->stacktop );
                inst( XEC_UNPACK, op->r, 0 );
            }
            else
            {
                // Recover single array element.
                inst( XEC_ELEM, op->r, op->args->rcount );
            }
            break;
        }
        
        case XEC_SSA_CALL:
            callinst( XEC_CALL, block->ops, (int)i );
            break;
        
        case XEC_SSA_YCALL:
            callinst( XEC_YCALL, block->ops, (int)i );
            break;
            
        case XEC_SSA_YIELD:
            callinst( XEC_YIELD, block->ops, (int)i );
            break;
            
        case XEC_SSA_NEW:
        {
            // Build register transfer to get arguments in correct positions.
            arguments( op );
            
            // Instruction places new object in the correct place.
            int b = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
            inst( XEC_NEW, op->r, op->args->stacktop, b );
            break;
        }
        
        case XEC_SSA_EXTEND:
        {
            // Extend should have only the array and unpacked args.
            assert( op->args->unpacked );
            inst( XEC_EXTEND, op->args->stacktop,
                            o( op->args->args.at( 0 ) ), 0 );
            break;
        }
        
        case XEC_SSA_NEXT:
        {
            // Should have a single argument, the iterator.
            assert( op->args->args.size() == 1 );
            int iter = o( op->args->args.at( 0 ) );
 
            // There are special cases for next ops returning 1 and 2 values.
            // This is the vast majority of cases (iterating through arrays,
            // tables, or object keys).
            if ( op->args->rcount == 1 )
            {
                // Simplest version.
                inst( XEC_NEXT1, op->r, iter, 0 );
            }
            else if ( op->args->rcount == 2 )
            {
                // Find the two following selects.
                int select[ 2 ] = { op->r, -1 };
                for ( size_t j = i; j < block->ops->ops.size(); ++j )
                {
                    xec_ssa_op* selop = &block->ops->ops.at( j );

                    if ( selop->opcode != XEC_SSA_SELECT
                            || func->getop( selop->operanda ) != op )
                    {
                        break;
                    }
                    
                    if ( selop->immkey >= 0 && selop->immkey <= 1 )
                    {
                        select[ op->immkey ] = op->r;
                    }
                }
                
                // Two-result version.
                inst( XEC_NEXT2, select[ 0 ], iter, select[ 1 ] );
            }
            else
            {
                // Generic version which uses stack top.
                int b = op->args->rcount == -1 ? XEC_MARK : op->args->rcount;
                inst( XEC_NEXT, op->args->stacktop, iter, b );
            
                // Build register transfer graph from following selects to get
                // results into the correct locations.
                select( block->ops, (int)i );
            }
        }

        case XEC_SSA_RETURN:
        {
            // Build register transfer to get arguments in correct position.
            arguments( op );
            
            // Return them.
            int a = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
            inst( XEC_RETURN, op->args->stacktop, a, 0 );
            break;
        }
        
        case XEC_SSA_CLOSURE:
        {
            // Assume that functions will end up in the module in the same
            // order as in the SSA form.
            size_t index = 0;
            for ( ; index < root->functions.size(); ++index )
            {
                if ( root->functions.at( index ) == op->closure->function )
                {
                    break;
                }
            }
            assert( index < root->functions.size() );
            
            // Output instruction.
            inst( XEC_CLOSURE, op->r, (int)index );
            
            // Add upvals to closure.
            assert( op->closure->upvals.size() ==
                            op->closure->function->upvalcount );
            for ( size_t i = 0; i < op->closure->upvals.size(); ++i )
            {
                xec_ssa_opref upref = op->closure->upvals.at( i );
                xec_ssa_op* upop = func->getop( func->operand( upref ) );
                assert( upop->opcode == XEC_SSA_REFUP );
                inst( XEC_ENVUP, 0, upop->immkey );
            }
            
            break;
        }
        
        case XEC_SSA_PHI:
        case XEC_SSA_LIVE:
        {
            assert( ! "invalid opcode" );
            break;
        }
        }
        
    
    }
}



int xec_ssa_translateout::k( int immkey )
{
    const char* ssakey = root->keys.at( immkey );
    for ( size_t i = 0; i < keys.size(); ++i )
    {
        xec_key* key = keys.at( i );
        if ( strcmp( key->c_str(), ssakey ) == 0 )
        {
            return (int)i;
        }
    }
    
    int result = (int)keys.size();
    keys.push_back( xec_key::create( ssakey ) );
    return result;
}

int xec_ssa_translateout::o( xec_ssa_opref operand )
{
    operand = func->operand( operand );
    xec_ssa_op* op = func->getop( operand );
    return op->r;
}


void xec_ssa_translateout::inst( xec_opcode opcode, int r, int a, int b )
{
    if ( r == -1 || a == -1 || b == -1 )
    {
        return;
    }
    
    i.emplace_back( opcode, (unsigned)r, (unsigned)a, (unsigned)b );
}

void xec_ssa_translateout::inst( xec_opcode opcode, int r, int c )
{
    if ( r == -1 || c == -1 )
    {
        return;
    }
    
    i.emplace_back( opcode, (unsigned)r, (unsigned)c );
}


void xec_ssa_translateout::callinst(
                xec_opcode opcode, xec_ssa_slice* slice, int index )
{
    xec_ssa_op* op = &slice->ops.at( index );

    // Build register transfer to get arguments in correct positions.
    arguments( op );
    
    // Call.
    int a = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
    int b = op->args->rcount == -1 ? XEC_MARK : op->args->rcount;
    inst( opcode, op->args->stacktop, a, b );

    // Build register transfer graph from following selects to get results in
    // the correct location.
    select( slice, index );
}


void xec_ssa_translateout::arguments( xec_ssa_op* op )
{
    assert( op->opcode >= XEC_SSA_FIRST_ARG
                && op->opcode <= XEC_SSA_LAST_ARG );

    // Build rtg from arguments.
    xec_ssa_rtgraph rtg;
    for ( size_t i = 0; i < op->args->args.size(); ++i )
    {
        int operand = o( op->args->args.at( i ) );
        rtg.moves.emplace_back( operand, op->args->stacktop + (int)i );
    }

    // Rely on register allocator to place unpacked ops correctly.
    if ( op->args->unpacked )
    {
        xec_ssa_op* unpackop = func->getop( op->args->unpacked );
        assert( unpackop->opcode >= XEC_SSA_FIRST_ARG
                        && unpackop->opcode <= XEC_SSA_LAST_ARG );
        assert( unpackop->r == unpackop->args->stacktop );
        assert( unpackop->r == op->args->stacktop + op->args->args.size() );
    }
    
    // Perform moves.
    move( &rtg );
}

void xec_ssa_translateout::select( xec_ssa_slice* slice, size_t index )
{
    // The op that we're at is the one that generates the values to select.
    xec_ssa_op* op = &slice->ops.at( index );
    assert( op->opcode >= XEC_SSA_FIRST_ARG
                && op->opcode <= XEC_SSA_LAST_ARG );
    int stacktop = op->args->stacktop;

    // Might not have any selects, in which case it might have a single
    // value we want to place in the allocated register.
    xec_ssa_rtgraph rtg;
    if ( op->r != -1 )
    {
        rtg.moves.emplace_back( stacktop, (int)op->r );
    }

    // Or there might be selects after all.
    for ( size_t i = index + 1; i < slice->ops.size(); ++i )
    {
        xec_ssa_op* selop = &slice->ops.at( index );
        
        if ( selop->opcode != XEC_SSA_SELECT )
        {
            break;
        }
        
        assert( selop->immkey < op->args->rcount );
        rtg.moves.emplace_back( stacktop + selop->immkey, (int)selop->r );
    }
    
    // Perform moves.
    move( &rtg );
}


void xec_ssa_translateout::move( xec_ssa_rtgraph* rtg )
{
}





