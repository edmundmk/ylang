//
//  yssa_print.cpp
//
//  Created by Edmund Kapusniak on 21/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa.h"
#include "yl_code.h"



static void yssa_print_function( yssa_function* f );
static void yssa_print_block( yssa_block* b );
static void yssa_print_opinst( yssa_opinst* o );


void yssa_print( yssa_module* module )
{
    for ( size_t i = 0; i < module->functions.size(); ++i )
    {
        yssa_print_function( module->functions.at( i ).get() );
    }
}


void yssa_print_function( yssa_function* f )
{
    printf( "%s\n", f->funcname );
    for ( size_t i = 0; i < f->blocks.size(); ++i )
    {
        yssa_print_block( f->blocks.at( i ).get() );
    }
}


void yssa_print_block( yssa_block* b )
{
    printf( "%p :", b );
    if ( b->flags & YSSA_LOOP )
        printf( " loop" );
    if ( b->flags & YSSA_XCHANDLER )
        printf( " xchandler [ %d %d ]", b->unwind_localups, b->unwind_itercount );
    if ( b->flags & YSSA_UNSEALED )
        printf( " unsealed" );
    if ( b->flags & YSSA_LOOKUP )
        printf( " lookup" );
    printf( "\n" );
    
    printf( "    prev :" );
    for ( size_t i = 0; i < b->prev.size(); ++i )
    {
        printf( " %p", b->prev.at( i ) );
    }
    printf( "\n" );
    
    for ( size_t i = 0; i < b->phi.size(); ++i )
    {
        yssa_opinst* o = b->phi.at( i );
        printf( "%p - ", o );
        yssa_print_opinst( o );
    }
    for ( size_t i = 0; i < b->ops.size(); ++i )
    {
        yssa_opinst* o = b->phi.at( i );
        printf( "%p : ", o );
        yssa_print_opinst( o );
    }
    
    if ( b->test )
    {
        printf( "    test : %p\n", b->test );
        printf( "    next : %p\n", b->next );
        printf( "    fail : %p\n", b->fail );
    }
    else
    {
        printf( "    next : %p\n", b->next );
    }
    
    if ( b->xchandler )
    {
        printf( "    xchandler : %p\n", b->xchandler );
    }
}



namespace
{

class yssa_disasm
{
public:

    yssa_disasm()
    {
        add( YL_NOP,        "nop" );
        add( YL_MOV,        "mov" );
        add( YL_SWP,        "swp" );

        add( YL_NULL,       "null" );
        add( YL_BOOL,       "bool %y" );
        add( YL_NUMBER,     "number %n" );
        add( YL_STRING,     "string %s" );

        add( YL_GLOBAL,     "global %k" );
        add( YL_SETGLOBAL,  "setglobal %k" );

        add( YL_NEG,        "neg" );
        add( YL_BITNOT,     "bitnot" );

        add( YL_MUL,        "mul" );
        add( YL_DIV,        "div" );
        add( YL_MOD,        "mod" );
        add( YL_INTDIV,     "intdiv" );
        add( YL_ADD,        "add" );
        add( YL_SUB,        "sub" );
        add( YL_LSL,        "lsl" );
        add( YL_LSR,        "lsr" );
        add( YL_ASR,        "asr" );
        add( YL_BITAND,     "bitand" );
        add( YL_BITXOR,     "bitxor" );
        add( YL_BITOR,      "bitor" );
        add( YL_CONCAT,     "concat" );
    
        add( YL_EQ,         "eq" );
        add( YL_NE,         "ne" );
        add( YL_LT,         "lt" );
        add( YL_GT,         "gt" );
        add( YL_LE,         "le" );
        add( YL_GE,         "ge" );
    
        add( YL_LNOT,       "lnot" );
        add( YL_LXOR,       "lxor" );
        
        add( YL_JMP,        "jmp" );
        add( YL_JMPT,       "jmpt" );
        add( YL_JMPF,       "jmpf" );
    
        add( YL_VARARG,     "vararg" );
        add( YL_CLOSURE,    "closure %f" );
        add( YL_CALL,       "call" );
        add( YL_YCALL,      "ycall" );
        add( YL_YIELD,      "yield" );
        add( YL_RETURN,     "return" );
    
        add( YL_ITER,       "iterkey %r" );
        add( YL_ITERKEY,    "iterkey %r" );
        add( YL_JMPITER,    "jumpiter" );
        add( YL_NEXT1,      "next1 %b" );
        add( YL_NEXT2,      "next2 %b" );
        add( YL_NEXT,       "next %b" );

        add( YL_GETUP,      "getup %a" );
        add( YL_SETUP,      "setup %a" );

        add( YL_CLOSE,      "close %a %b" );

        add( YL_OBJECT,     "object" );
        add( YL_ARRAY,      "array %c" );
        add( YL_TABLE,      "table %c" );
    
        add( YL_SUPER,      "super" );
    
        add( YL_KEY,        "key %k" );
        add( YL_INKEY,      "inkey" );
        add( YL_INDEX,      "index" );
        add( YL_SETKEY,     "setkey %k" );
        add( YL_SETINKEY,   "setinkey" );
        add( YL_SETINDEX,   "setindex" );
        add( YL_DELKEY,     "delkey %k" );
        add( YL_DELINKEY,   "delinkey" );
    
        add( YL_IN,         "in" );
        add( YL_IS,         "is" );

        add( YL_APPEND,     "append" );
        add( YL_EXTEND,     "extend" );
        add( YL_UNPACK,     "unpack" );
    
        add( YL_THROW,      "throw" );
        add( YL_EXCEPT,     "except" );
        add( YL_UNWIND,     "unwind" );

        add( YL_UPLOCAL,    "uplocal (%t) %r %a" );
        add( YL_UPUPVAL,    "upupval (%t) %r %a" );

        add( YSSA_PARAM,    "param %i" );
        add( YSSA_SELECT,   "select %i" );
        add( YSSA_IMPLICIT, "implicit (%t)" );
        add( YSSA_ITERDONE, "iterdone %b" );
        add( YSSA_PHI,      "phi" );
        add( YSSA_REF,      "ref" );
        add( YSSA_VAR,      "var" );
    }
    
    
    const char* lookup( uint8_t opcode ) const
    {
        return map.at( opcode );
    }


private:

    void add( uint8_t opcode, const char* disasm )
    {
        map.emplace( opcode, disasm );
    }
    
    std::unordered_map< uint8_t, const char* > map;

};


const yssa_disasm disasm;


}



void yssa_print_opinst( yssa_opinst* o )
{
    printf( "[%2d] %2d ", o->r, o->result_count );

    const char* disasm = ::disasm.lookup( o->opcode );
    for ( const char* c = disasm; c[ 0 ] != '\0'; ++c )
    {
        if ( c[ 0 ] == '%' )
        {
            ++c;
            switch ( c[ 0 ] )
            {
                case '\0':
                    --c;
                    break;
                
                case 'r':
                    printf( "%d", o->r );
                    break;
                
                case 'a':
                    printf( "%d", o->a );
                    break;
                
                case 'b':
                    printf( "%d", o->b );
                    break;
                
                case 'k':
                    printf( "%s", o->key );
                    break;
                
                case 'i':
                    printf( "%d", o->select );
                    break;
                
                case 't':
                    printf( "%p", o->associated );
                    break;
                
                case 'y':
                    printf( "%s", o->boolean ? "true" : "false" );
                    break;
                
                case 'n':
                    printf( "%g", o->number );
                    break;
                
                case 's':
                    printf( "\"%*s\"", (int)o->string->length, o->string->string );
                    break;
            }
        }
        else
        {
            printf( "%c", c[ 0 ] );
        }
    }
    
    for ( size_t i = 0; i < o->operand_count; ++i )
    {
        printf( " %p", o->operand[ i ] );
    }
    
    if ( o->opcode == YL_RETURN || o->opcode == YL_EXTEND
            || o->opcode == YL_CALL || o->opcode == YL_YCALL
                    || o->opcode == YL_YIELD )
    {
        if ( o->multival )
        {
            printf( " %p ... ", o->multival );
        }
    }
    
    if ( o->opcode != YL_UPLOCAL &&
            o->opcode != YL_UPUPVAL && o->opcode != YSSA_IMPLICIT )
    {
        if ( o->variable )
        {
            printf( " -> [%d] %s", o->variable->r, o->variable->name );
            if ( o->variable->xcref )
                printf( " xcref" );
            if ( o->variable->localup != yl_opinst::NOVAL )
                printf( " %d", o->variable->localup );
        }
    }
    
    printf( "\n" );
}
















