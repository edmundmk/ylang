//
//  xssa.cpp
//
//  Created by Edmund Kapusniak on 11/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa.h"
#include <unordered_map>



const char* xssa_opname( xssa_opcode opcode )
{
    switch ( opcode )
    {
    case XSSA_NOP:      return "nop";

    case XSSA_NULL:     return "null";
    case XSSA_NUMBER:   return "number";
    case XSSA_BOOL:     return "bool";
    case XSSA_STRING:   return "string";

    case XSSA_POS:      return "pos";
    case XSSA_NEG:      return "neg";
    case XSSA_NOT:      return "not";
    case XSSA_LNOT:     return "lnot";

    case XSSA_MUL:      return "mul";
    case XSSA_DIV:      return "div";
    case XSSA_MOD:      return "mod";
    case XSSA_INTDIV:   return "intdiv";
    case XSSA_ADD:      return "add";
    case XSSA_SUB:      return "sub";
    case XSSA_LSL:      return "lsl";
    case XSSA_LSR:      return "lsr";
    case XSSA_ASR:      return "asr";
    case XSSA_AND:      return "and";
    case XSSA_XOR:      return "xor";
    case XSSA_OR:       return "or";
    case XSSA_CONCAT:   return "concat";
    
    case XSSA_EQ:       return "eq";
    case XSSA_LT:       return "lt";
    case XSSA_LE:       return "le";
    case XSSA_IN:       return "in";
    case XSSA_IS:       return "is";

    case XSSA_LXOR:     return "lxor";
    
    case XSSA_KEY:      return "key";
    case XSSA_INKEY:    return "inkey";
    case XSSA_INDEX:    return "index";
    case XSSA_GLOBAL:   return "global";

    case XSSA_SETKEY:   return "setkey";
    case XSSA_SETINKEY: return "setinkey";
    case XSSA_SETINDEX: return "setindex";
    case XSSA_SETGLOBAL: return "setglobal";
    
    case XSSA_DELKEY:   return "delkey";
    case XSSA_DELINKEY: return "delinkey";
    
    case XSSA_NEWUP:    return "newup";
    case XSSA_SETUP:    return "setup";
    case XSSA_REFUP:    return "refup";
    case XSSA_CLOUP:    return "cloup";
    
    case XSSA_OBJECT:   return "object";
    case XSSA_TABLE:    return "table";
    case XSSA_ARRAY:    return "array";

    case XSSA_APPEND:   return "append";
    case XSSA_EXTEND:   return "extend";
    
    case XSSA_CLOSURE:  return "closure";

    case XSSA_VARALL:   return "varall";
    case XSSA_UNPACK:   return "unpack";
    case XSSA_CALL:     return "call";
    case XSSA_YCALL:    return "ycall";
    case XSSA_YIELD:    return "yield";
    
    case XSSA_ITER:     return "iter";
    case XSSA_ITERKEY:  return "iterkey";
    case XSSA_POPITER:  return "popiter";
    case XSSA_NEXT:     return "next";
    
    case XSSA_RETURN:   return "return";
    
    case XSSA_PARAM:    return "param";
    case XSSA_VARARG:   return "vararg";
    case XSSA_SELECT:   return "select";
    case XSSA_ELEM:     return "elem";

    case XSSA_PHI:      return "phi";
    case XSSA_REF:      return "(ref)";
    case XSSA_PSI:      return "(psi)";
    }
    
    return "???";
}



void xssa_print( xssa_func* func, xssaop* op )
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
    printf( "%-9s", xssa_opname( op->opcode ) );

    if ( op->result_count == -1 )
    {
        printf( " {...}" );
    }
    else if ( op->result_count != 1 )
    {
        printf( " {%d}", op->result_count );
    }

    
    // Operands
    if ( op->opcode == XSSA_CLOSURE )
    {
        printf( " %p %s", op->func, op->func->funcname );
    }
    
    for ( size_t i = 0; i < op->operand_count; ++i )
    {
        if ( op->operands[ i ] )
        {
            printf( " :%04X", op->operands[ i ]->index );
        }
        else
        {
            printf( " :----" );
        }
    }
    
    if ( xssaop::has_multival( op->opcode ) && op->multival )
    {
        printf( " :%04X...", op->multival->index );
    }
    
    if ( xssaop::has_key( op->opcode ) )
    {
        printf( " '%s'", op->key );
    }
    
    if ( xssaop::has_immed( op->opcode ) )
    {
        printf( " %d", op->immed );
    }
    
    if ( op->opcode == XSSA_NUMBER )
    {
        printf( " %g", op->number );
    }
    
    if ( op->opcode == XSSA_BOOL )
    {
        printf( " %s", op->boolean ? "true" : "false" );
    }
    
    if ( op->opcode == XSSA_STRING )
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





void xssa_print( xssa_module* module )
{
    for ( size_t i = 0; i < module->funcs.size(); ++i )
    {
        xssa_func* func = module->funcs.at( i ).get();
        xssa_print( func );
    }
}


static void print_ops( xssa_func* func, const std::vector< xssaop* > ops )
{
    for ( size_t i = 0; i < ops.size(); ++i )
    {
    
        xssaop* op = ops.at( i );
        if ( ! op )
        {
            continue;
        }
        
        printf( "%04X ", op->index );
        xssa_print( func, op );

    }
}


void xssa_print( xssa_func* func )
{
        
    // First, give each op an index.
    int index = 0;
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xssa_block* block = func->blocks.at( i ).get();
        for ( size_t i = 0; i < block->phi.size(); ++i )
        {
            xssaop* op = block->phi.at( i );
            if ( op )
                op->index = index++;
        }
        for ( size_t i = 0; i < block->ops.size(); ++i )
        {
            xssaop* op = block->ops.at( i );
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
    
        xssa_block* block = func->blocks.at( i ).get();
        
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
                xssaop* op = *i;
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
                xssaop* op = *i;
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




