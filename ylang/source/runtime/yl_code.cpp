//
//  yl_code.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_code.h"
#include <unordered_map>



yl_opinst::yl_opinst()
    :   i( 0 )
{
}


yl_opinst::yl_opinst( yl_opcode opcode, unsigned r, unsigned a, unsigned b )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint8_t)a << 16
            | (uint8_t)b << 24 )
{
}

yl_opinst::yl_opinst( yl_opcode opcode, unsigned r, unsigned c )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint16_t)c << 16 )
{
}

yl_opinst::yl_opinst( yl_opcode opcode, unsigned r, signed j )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (int16_t)j << 16 )
{
}





namespace
{

class ylop_disasm
{
public:

    ylop_disasm()
    {
        add( YL_NOP,        "nop" );
        add( YL_MOV,        "mov %r, %a" );
        add( YL_SWP,        "swp %r, %a" );

        add( YL_NULL,       "null %r" );
        add( YL_BOOL,       "bool %r, #%a" );
        add( YL_NUMBER,     "number %r, $%c" );
        add( YL_STRING,     "string %r, $%c" );

        add( YL_GLOBAL,     "global %r, $%b" );
        add( YL_SETGLOBAL,  "setglobal %r, $%b" );

        add( YL_NEG,        "neg %r, %a" );
        add( YL_BITNOT,     "bitnot %r, %a" );

        add( YL_MUL,        "mul %r, %a, %b" );
        add( YL_DIV,        "div %r, %a, %b" );
        add( YL_MOD,        "mod %r, %a, %b" );
        add( YL_INTDIV,     "intdiv %r, %a, %b" );
        add( YL_ADD,        "add %r, %a, %b" );
        add( YL_SUB,        "sub %r, %a, %b" );
        add( YL_LSL,        "lsl %r, %a, %b" );
        add( YL_LSR,        "lsr %r, %a, %b" );
        add( YL_ASR,        "asr %r, %a, %b" );
        add( YL_BITAND,     "bitand %r, %a, %b" );
        add( YL_BITXOR,     "bitxor %r, %a, %b" );
        add( YL_BITOR,      "bitor %r, %a, %b" );
        add( YL_CONCAT,     "concat %r, %a, %b" );
    
        add( YL_EQ,         "eq %r, %a, %b" );
        add( YL_NE,         "ne %r, %a, %b" );
        add( YL_LT,         "lt %r, %a, %b" );
        add( YL_GT,         "gt %r, %a, %b" );
        add( YL_LE,         "le %r, %a, %b" );
        add( YL_GE,         "ge %r, %a, %b" );
    
        add( YL_LNOT,       "lnot %r, %a" );
        add( YL_LXOR,       "lxor %r, %a, %b" );
        
        add( YL_JMP,        "jmp @%j" );
        add( YL_JMPT,       "jmpt %r, @%j" );
        add( YL_JMPF,       "jmpf %r, @%j" );
    
        add( YL_VARARG,     "vararg %r, %b" );
        add( YL_FUNCTION,   "function %r, $%c" );
        add( YL_CALL,       "call %r, %a, %b" );
        add( YL_YCALL,      "ycall %r, %a, %b" );
        add( YL_YIELD,      "yield %r, %a, %b" );
        add( YL_RETURN,     "return %r, %a" );
    
        add( YL_ITER,       "iter ~%r, %a" );
        add( YL_ITERKEY,    "iterkey ~%r, %a" );
        add( YL_NEXT1,      "next1 %r, ~%a" );
        add( YL_NEXT2,      "next2 %r, %b, ~%a" );
        add( YL_NEXT,       "next %r, %b, ~%a" );

        add( YL_JMPV,       "jmpv ~%r, @%j" );
        add( YL_JMPN,       "jmpn ~%r, @%j" );

        add( YL_GETUP,      "getup %r, ^%a" );
        add( YL_SETUP,      "setup %r, ^%a" );

        add( YL_CLOSE,      "close *%a, ~%b" );

        add( YL_OBJECT,     "object %r, %a" );
        add( YL_ARRAY,      "array %r, #%c" );
        add( YL_TABLE,      "table %r, #%c" );
    
        add( YL_SUPER,      "super %r, %a" );
    
        add( YL_KEY,        "key %r, %a, $%b" );
        add( YL_INKEY,      "inkey %r, %a, %b" );
        add( YL_INDEX,      "index %r, %a, %b" );
        add( YL_SETKEY,     "setkey %r, %a, $%b" );
        add( YL_SETINKEY,   "setinkey %r, %a, %b" );
        add( YL_SETINDEX,   "setindex %r, %a, %b" );
        add( YL_RESPONDS,   "responds %r, %a, %b" );
        add( YL_DELKEY,     "delkey %a, $%b" );
        add( YL_DELINKEY,   "delinkey %a, %b" );
    
        add( YL_IS,         "is %r, %a, %b" );

        add( YL_APPEND,     "append %r, %a" );
        add( YL_EXTEND,     "extend %r, %a, %b" );
        add( YL_UNPACK,     "unpack %r, %b, %a" );
    
        add( YL_THROW,      "throw %r" );
        add( YL_EXCEPT,     "except %r" );
        add( YL_UNWIND,     "unwind" );

        add( YL_UPLOCAL,    "uplocal -%r, *%a, %b" );
        add( YL_UPUPVAL,    "upupval -%r, ^%a" );
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


const ylop_disasm disasm;


}






void ylop_print( size_t index, yl_opinst* op )
{
    printf( "%04zX ", index );
    
    const char* disasm = ::disasm.lookup( op->opcode() );
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
                    printf( "%u", op->r() );
                    break;
                
                case 'a':
                    printf( "%u", op->a() );
                    break;
                
                case 'b':
                    printf( "%u", op->b() );
                    break;
                
                case 'c':
                    printf( "%u", op->c() );
                    break;
                
                case 'j':
                    printf( "%04zX", index + 1 + op->j() );
                    break;
            }
        }
        else
        {
            printf( "%c", c[ 0 ] );
        }
    }
    
    printf( "\n" );
}



