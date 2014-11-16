//
//  yssa.cpp
//
//  Created by Edmund Kapusniak on 11/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yssa.h"
#include <unordered_map>



const char* yssa_opname( yssa_opcode opcode )
{
    switch ( opcode )
    {
    case YSSA_NOP:      return "nop";

    case YSSA_NULL:     return "null";
    case YSSA_NUMBER:   return "number";
    case YSSA_BOOL:     return "bool";
    case YSSA_STRING:   return "string";

    case YSSA_POS:      return "pos";
    case YSSA_NEG:      return "neg";
    case YSSA_NOT:      return "not";
    case YSSA_LNOT:     return "lnot";

    case YSSA_MUL:      return "mul";
    case YSSA_DIV:      return "div";
    case YSSA_MOD:      return "mod";
    case YSSA_INTDIV:   return "intdiv";
    case YSSA_ADD:      return "add";
    case YSSA_SUB:      return "sub";
    case YSSA_LSL:      return "lsl";
    case YSSA_LSR:      return "lsr";
    case YSSA_ASR:      return "asr";
    case YSSA_AND:      return "and";
    case YSSA_XOR:      return "xor";
    case YSSA_OR:       return "or";
    case YSSA_CONCAT:   return "concat";
    
    case YSSA_EQ:       return "eq";
    case YSSA_LT:       return "lt";
    case YSSA_LE:       return "le";
    case YSSA_IN:       return "in";
    case YSSA_IS:       return "is";

    case YSSA_LXOR:     return "lxor";
    
    case YSSA_SUPER:    return "super";
    case YSSA_KEY:      return "key";
    case YSSA_INKEY:    return "inkey";
    case YSSA_INDEX:    return "index";
    case YSSA_GLOBAL:   return "global";

    case YSSA_SETKEY:   return "setkey";
    case YSSA_SETINKEY: return "setinkey";
    case YSSA_SETINDEX: return "setindex";
    case YSSA_SETGLOBAL: return "setglobal";
    
    case YSSA_DELKEY:   return "delkey";
    case YSSA_DELINKEY: return "delinkey";
    
    case YSSA_NEWUP:    return "newup";
    case YSSA_SETUP:    return "setup";
    case YSSA_REFUP:    return "refup";
    case YSSA_CLOUP:    return "cloup";
    
    case YSSA_OBJECT:   return "object";
    case YSSA_TABLE:    return "table";
    case YSSA_ARRAY:    return "array";

    case YSSA_APPEND:   return "append";
    case YSSA_EXTEND:   return "extend";
    
    case YSSA_CLOSURE:  return "closure";

    case YSSA_VARALL:   return "varall";
    case YSSA_UNPACK:   return "unpack";
    case YSSA_CALL:     return "call";
    case YSSA_YCALL:    return "ycall";
    case YSSA_YIELD:    return "yield";
    
    case YSSA_ITER:     return "iter";
    case YSSA_ITERKEY:  return "iterkey";
    case YSSA_POPITER:  return "popiter";
    case YSSA_NEXT:     return "next";
    
    case YSSA_RETURN:   return "return";
    
    case YSSA_PARAM:    return "param";
    case YSSA_VARARG:   return "vararg";
    case YSSA_SELECT:   return "select";
    case YSSA_ELEM:     return "elem";

    case YSSA_PHI:      return "phi";
    case YSSA_REF:      return "(ref)";
    case YSSA_PSI:      return "(psi)";
    }
    
    return "???";
}



void yssa_print( yssa_func* func, yssaop* op )
{
    // Register.
    if ( op->r != (uint8_t)-1 || op->stacktop != (uint8_t)-1 )
    {
        printf( "<" );
        if ( op->r != (uint8_t)-1 )
            printf( "%d", (int)op->r );
        if ( op->stacktop != (uint8_t)-1 )
            printf( "/%d", (int)op->stacktop );
        printf( "> " );
    }
    
    
    // Opcode
    printf( "%-9s", yssa_opname( op->opcode ) );

    if ( op->result_count == -1 )
    {
        printf( " {...}" );
    }
    else if ( op->result_count != 1 )
    {
        printf( " {%d}", op->result_count );
    }

    
    // Operands
    if ( op->opcode == YSSA_CLOSURE )
    {
        printf( " %p %s", op->func, op->func->funcname );
    }
    
    for ( size_t i = 0; i < op->operand_count; ++i )
    {
        if ( op->operand[ i ] )
        {
            printf( " :%04X", op->operand[ i ]->index );
        }
        else
        {
            printf( " :----" );
        }
    }
    
    if ( yssaop::has_multival( op->opcode ) && op->multival )
    {
        printf( " :%04X...", op->multival->index );
    }
    
    if ( yssaop::has_key( op->opcode ) )
    {
        printf( " '%s'", op->key );
    }
    
    if ( yssaop::has_immed( op->opcode ) )
    {
        printf( " %d", op->immed );
    }
    
    if ( op->opcode == YSSA_NUMBER )
    {
        printf( " %g", op->number );
    }
    
    if ( op->opcode == YSSA_BOOL )
    {
        printf( " %s", op->boolean ? "true" : "false" );
    }
    
    if ( op->opcode == YSSA_STRING )
    {
        printf( " \"" );
        for ( size_t i = 0; i < op->string->length; ++i )
        {
            char c = op->string->string[ i ];
            if ( c >= 0x20 && c <= 0x7E )
            {
                printf( "%c", c );
            }
            else switch ( c )
            {
            case 0x07:  printf( "\\a" );        break;
            case 0x08:  printf( "\\b" );        break;
            case 0x09:  printf( "\\t" );        break;
            case 0x0A:  printf( "\\n" );        break;
            case 0x0B:  printf( "\\v" );        break;
            case 0x0C:  printf( "\\f" );        break;
            case 0x0D:  printf( "\\r" );        break;
            case '"':   printf( "\\\"" );       break;
            default:    printf( "\\x%02X", c ); break;
            }
        }
        printf( "\"" );
    }
    
    
    // Names
    auto ii = func->names.equal_range( op );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        if ( i == ii.first )
            printf( " ( " );
        else
            printf( ", " );
        printf( "%.*s", (int)i->second->length, i->second->string );
    }
    if ( ii.first != ii.second )
    {
        printf( " )" );
    }
    
    
    printf( "\n" );
    
}





void yssa_print( yssa_module* module )
{
    for ( size_t i = 0; i < module->funcs.size(); ++i )
    {
        yssa_func* func = module->funcs.at( i ).get();
        yssa_print( func );
    }
}


static void print_ops( yssa_func* func, const std::vector< yssaop* > ops )
{
    for ( size_t i = 0; i < ops.size(); ++i )
    {
    
        yssaop* op = ops.at( i );
        if ( ! op )
        {
            continue;
        }
        
        printf( "%04X ", op->index );
        yssa_print( func, op );

    }
}


void yssa_print( yssa_func* func )
{
        
    // First, give each op an index.
    int index = 0;
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        yssa_block* block = func->blocks.at( i ).get();
        for ( size_t i = 0; i < block->phi.size(); ++i )
        {
            yssaop* op = block->phi.at( i );
            if ( op )
                op->index = index++;
        }
        for ( size_t i = 0; i < block->ops.size(); ++i )
        {
            yssaop* op = block->ops.at( i );
            if ( op )
                op->index = index++;
        }
    }


    // Write out function header.
    printf( "function %p %s\n", func, func->funcname );
    printf( "    upvalcount : %d\n", func->upvalcount );
    printf( "    newupcount : %d\n", func->newupcount );
    printf( "    paramcount : %d\n", func->paramcount );
    printf( "    varargs    : %s\n", func->varargs ? "true" : "false" );
    printf( "    coroutine  : %s\n", func->coroutine ? "true" : "false" );
    
    // Write each block.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
    
        yssa_block* block = func->blocks.at( i ).get();
        
        printf( "\n" );
        printf( "[%04X]\n", block->index );
        
        if ( block->previous.size() )
        {
            printf( "  <" );
            for ( size_t i = 0; i < block->previous.size(); ++i )
            {
                printf( " [%04X]", block->previous.at( i )->index );
            }
            printf( "\n" );
        }
        
        if ( block->loop )
        {
            printf( "    loop : %p\n", block->loop );
            if ( block->loop->header == block )
            {
                printf( "        children :" );
                for ( auto i = block->loop->children.begin(); i != block->loop->children.end(); ++i )
                {
                    printf( " %p", *i );
                }
                printf( "\n" );
                printf( "        blocks :" );
                for ( auto i = block->loop->blocks.begin(); i != block->loop->blocks.end(); ++i )
                {
                    printf( " [%04X]", (*i)->index );
                }
                printf( "\n" );
            }
        }
        
        if ( block->live_in.size() )
        {
            printf( "  !" );
            for ( auto i = block->live_in.begin(); i != block->live_in.end(); ++i )
            {
                yssaop* op = *i;
                printf( " :%04X", op->index );
            }
            printf( "\n" );
        }
        
        print_ops( func, block->phi );
        print_ops( func, block->ops );
        
        if ( block->live_out.size() )
        {
            printf( "  !" );
            for ( auto i = block->live_out.begin(); i != block->live_out.end(); ++i )
            {
                yssaop* op = *i;
                printf( " :%04X", op->index );
            }
            printf( "\n" );
        }

        if ( block->condition )
        {
            printf
            (
                "  :%04X ? >[%04X] : >[%04X]\n",
                block->condition->index,
                block->iftrue->index,
                block->iffalse->index
            );
        }
        else if ( block->next )
        {
            printf( "  >[%04X]\n", block->next->index );
        }

    }


    printf( "\n\n" );

}




