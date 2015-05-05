//
//  yssa_constfold.cpp
//
//  Created by Edmund Kapusniak on 26/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_constfold.h"
#include <divmod.h>
#include "yssa.h"


static void yssa_constfold_op( yssa_module* module, yssa_opinst* op );
static yssa_opinst* yssa_fold_refs( yssa_opinst* op );


void yssa_constfold( yssa_module* module, yssa_function* function )
{
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        
        for ( size_t i = 0; i < block->phi.size(); ++i )
        {
            yssa_constfold_op( module, block->phi.at( i ) );
        }
        
        for ( size_t i = 0; i < block->ops.size(); ++i )
        {
            yssa_constfold_op( module, block->ops.at( i ) );
        }
        
        block->test = yssa_fold_refs( block->test );
    }
}


void yssa_constfold_op( yssa_module* module, yssa_opinst* op )
{
    // Fold references.
    if ( op->has_associated() )
    {
        op->associated = yssa_fold_refs( op->associated );
    }
    
    if ( op->has_multival() )
    {
        op->multival = yssa_fold_refs( op->multival );
    }
    
    for ( size_t i = 0; i < op->operand_count; ++i )
    {
        op->operand[ i ] = yssa_fold_refs( op->operand[ i ] );
        assert( op->operand[ i ] );
    }
    
    
    // Fold arithmetic operations with constant operands.
    yssa_opinst** o = op->operand;
    switch ( op->opcode )
    {
    case YL_NEG:
        if ( o[ 0 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = -o[ 0 ]->number;
        }
        break;
    
    case YL_BITNOT:
        if ( o[ 0 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = ~(uint32_t)o[ 0 ]->number;
        }
        break;

    case YL_MUL:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = o[ 0 ]->number * o[ 1 ]->number;
        }
        break;
    
    case YL_DIV:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = o[ 0 ]->number / o[ 1 ]->number;
        }
        break;
        
    case YL_MOD:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = emod( o[ 0 ]->number, o[ 1 ]->number );
        }
        break;

    case YL_INTDIV:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = intdiv( o[ 0 ]->number, o[ 1 ]->number );
        }
        break;
        
    case YL_ADD:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = o[ 0 ]->number + o[ 1 ]->number;
        }
        break;

    case YL_SUB:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = o[ 0 ]->number - o[ 1 ]->number;
        }
        break;

    case YL_LSL:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)o[ 0 ]->number << (int)o[ 1 ]->number;
        }
        break;

    case YL_LSR:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)o[ 0 ]->number >> (int)o[ 1 ]->number;
        }
        break;

    case YL_ASR:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)(
                    (int32_t)o[ 0 ]->number >> (int)o[ 1 ]->number );
        }
        break;

    case YL_BITAND:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)o[ 0 ]->number & (uint32_t)o[ 1 ]->number;
        }
        break;
        
    case YL_BITXOR:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)o[ 0 ]->number ^ (uint32_t)o[ 1 ]->number;
        }
        break;

    case YL_BITOR:
        if ( o[ 0 ]->opcode == YL_NUMBER && o[ 1 ]->opcode == YL_NUMBER )
        {
            op->opcode = YL_NUMBER;
            op->operand_count = 0;
            op->number = (uint32_t)o[ 0 ]->number | (uint32_t)o[ 1 ]->number;
        }
        break;

    case YL_CONCAT:
        if ( o[ 0 ]->opcode == YL_STRING && o[ 1 ]->opcode == YL_STRING )
        {
            yssa_string* a = o[ 0 ]->string;
            yssa_string* b = o[ 1 ]->string;
            
            size_t length = a->length + b->length;
            char* string = (char*)module->alloc.malloc( length + 1 );
            memcpy( string, a->string, a->length );
            memcpy( string + a->length, b->string, b->length );
            string[ length ] = '\0';

            op->opcode = YL_STRING;
            op->operand_count = 0;
            op->string = new ( module->alloc ) yssa_string( string, length );
        }
        break;
    
    }
    
    
}


yssa_opinst* yssa_fold_refs( yssa_opinst* op )
{
    while ( op && op->opcode == YSSA_REF )
    {
        op = op->operand[ 0 ];
    }
    
    return op;
}










