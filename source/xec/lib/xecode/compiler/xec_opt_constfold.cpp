//
//  xec_opt_constfold.cpp
//
//  Created by Edmund Kapusniak on 20/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_opt_constfold.h"
#include "xec_ssa.h"
#include "xec_ssa_cfganalysis.h"



xec_opt_constfold::xec_opt_constfold( xec_ssa* root )
    :   root( root )
    ,   func( NULL )
    ,   dfo( NULL )
{
}


void xec_opt_constfold::optimize( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    this->func  = func;
    this->dfo   = dfo;

    for ( size_t i = 0; i < dfo->size(); ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        for ( size_t i = 0; i < block->ops->ops.size(); ++i )
        {
            xec_ssa_op& op = block->ops->ops.at( i );
            optimize_op( op );
        }
    }

    this->func  = NULL;
    this->dfo   = NULL;
}


xec_ssa_op* xec_opt_constfold::operand( xec_ssa_opref opref )
{
    return func->getop( func->operand( opref ) );
}


bool xec_opt_constfold::number( xec_ssa_opref opref, double* number )
{
    xec_ssa_op* op = operand( opref );
    if ( op && op->opcode == XEC_SSA_NUMBER )
    {
        *number = op->number;
        return true;
    }
    return false;
}

bool xec_opt_constfold::string( xec_ssa_opref opref, xec_ssa_string** string )
{
    xec_ssa_op* op = operand( opref );
    if ( op && op->opcode == XEC_SSA_STRING )
    {
        *string = op->string;
        return true;
    }
    return false;
}




void xec_opt_constfold::optimize_op( xec_ssa_op& op )
{
    switch ( op.opcode )
    {
    case XEC_SSA_POS:
    {
        double operand;
        if ( number( op.operanda, &operand ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = operand;
        }
        break;
    }
    
    case XEC_SSA_NEG:
    {
        double operand;
        if ( number( op.operanda, &operand ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = -operand;
        }
        break;
    }
    
    case XEC_SSA_BITNOT:
    {
        double operand;
        if ( number( op.operanda, &operand ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = ~(uint32_t)operand;
        }
        break;
    }

    case XEC_SSA_MUL:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = lhs * rhs;
        }
        break;
    }

    case XEC_SSA_DIV:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = lhs / rhs;
        }
        break;
    }
    
    case XEC_SSA_MOD:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = lhs - floor( lhs / rhs ) * rhs;
        }
        break;
    }

    case XEC_SSA_INTDIV:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = floor( lhs / rhs );
        }
        break;
    }

    case XEC_SSA_ADD:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = lhs + rhs;
        }
        break;
    }

    case XEC_SSA_SUB:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = lhs - rhs;
        }
        break;
    }

    case XEC_SSA_LSL:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)lhs << (uint32_t)rhs;
        }
        break;
    }

    case XEC_SSA_LSR:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)lhs >> (uint32_t)rhs;
        }
        break;
    }

    case XEC_SSA_ASR:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)( (int32_t)(uint32_t)lhs >> (int32_t)rhs );
        }
        break;
    }

    case XEC_SSA_BITAND:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)lhs & (uint32_t)rhs;
        }
        break;
    }

    case XEC_SSA_BITXOR:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)lhs ^ (uint32_t)rhs;
        }
        break;
    }

    case XEC_SSA_BITOR:
    {
        double lhs, rhs;
        if ( number( op.operanda, &lhs ) && number( op.operandb, &rhs ) )
        {
            op.opcode = XEC_SSA_NUMBER;
            op.number = (uint32_t)lhs | (uint32_t)rhs;
        }
        break;
    }

    case XEC_SSA_CONCAT:
    {
        xec_ssa_string *lhs, *rhs;
        if ( string( op.operanda, &lhs ) && string( op.operandb, &rhs ) )
        {
            size_t  length = lhs->length + rhs->length;
            char*   string = (char*)root->alloc.malloc( length + 1 );
            memcpy( string, lhs->string, lhs->length );
            memcpy( string + lhs->length, rhs->string, rhs->length );
            string[ length ] = '\0';
            
            op.opcode = XEC_SSA_STRING;
            op.string = new ( root->alloc ) xec_ssa_string( string, length );
        }
        break;
    }
    
    default:
        break;
    }
    
}








