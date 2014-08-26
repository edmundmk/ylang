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



xec_ssa_translateout::xec_ssa_translateout( xec_ssa* root )
    :   root( root )
{
}



void xec_ssa_translateout::translateout( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    i.clear();
    
    this->func = func;
    this->dfo  = dfo;

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
            inst( XEC_NULL, op->r );
            break;
            
        case XEC_SSA_NUMBER:
        {
            xec_value v = xec_value( op->number );
            
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                if ( values.at( i ).raweq( v ) )
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
                if ( values.at( i ).raweq( v ) )
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
            // Build register transfer graph and solve it to get parameters
            // in the correct positions.
            break;
        }
        
        case XEC_SSA_SELECT:
            assert( ! "should have compiled select already" );
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
            if ( op->args->resultcount == -1 )
            {
                // Stacklike unpack of all varargs.
                assert( op->r == op->args->stacktop );
                inst( XEC_VARALL, op->r, 0 );
            }
            else
            {
                // Recover single vararg.
                inst( XEC_VARARG, op->r, op->args->resultcount );
            }
            break;
        }
        
        case XEC_SSA_UNPACK:
        {
            if ( op->args->resultcount == -1 )
            {
                // Stacklike unpack of all results.
                assert( op->r == op->args->stacktop );
                inst( XEC_UNPACK, op->r, 0 );
            }
            else
            {
                // Recover single array element.
                inst( XEC_ELEM, op->r, op->args->resultcount );
            }
            break;
        }
        
        case XEC_SSA_CALL:
            callinst( XEC_CALL, op );
            break;
        
        case XEC_SSA_YCALL:
            callinst( XEC_YCALL, op );
            break;
            
        case XEC_SSA_YIELD:
            callinst( XEC_YIELD, op );
            break;
            
        case XEC_SSA_NEW:
        {
            // Build register transfer to get arguments in correct positions.
            // TODO.
            
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
            // There are special cases for next ops returning 1 and 2 values.
            // This is the vast majority of cases (iterating through arrays,
            // tables, or object keys).
            if ( op->args->resultcount == 1 )
            {
                inst( XEC_NEXT1, op->r, o( op->args->args.at( 0 ) ), 0 );
            }
            else if ( op->args->resultcount == 2 )
            {
                // Find the two following selects.
                
            }
            else
            {
                // Deal similar to call-like instruction.
            }
        }

        case XEC_SSA_RETURN:
        {
            break;
        }
        
        case XEC_SSA_CLOSURE:
        {
            break;
        }
        
        default:
            assert( ! "invalid opcode" );
            break;
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


void xec_ssa_translateout::inst( xec_opcode opcode, int r )
{
}

void xec_ssa_translateout::inst( xec_opcode opcode, int r, int a, int b )
{
}

void xec_ssa_translateout::inst( xec_opcode opcode, int r, int c )
{
}


void xec_ssa_translateout::callinst( xec_opcode opcode, xec_ssa_op* op )
{
}






