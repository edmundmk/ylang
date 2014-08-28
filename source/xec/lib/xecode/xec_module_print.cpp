//
//  xec_module_print.cpp
//
//  Created by Edmund Kapusniak on 28/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_module.h"
#include <stdio.h>
#include <unordered_map>
#include "xec_string.h"



class xec_opprint
{
public:

    xec_opprint()
    {
        add( XEC_NOP,       "nop" );
    
        add( XEC_NULL,      "null      %r" );
        add( XEC_VALUE,     "value     %r %v" );
    
        add( XEC_MOV,       "mov       %r %a" );
        add( XEC_SWP,       "swp       %r %a" );

        add( XEC_POS,       "pos       %r %a" );
        add( XEC_NEG,       "neg       %r %a" );
        add( XEC_NOT,       "not       %r %a" );
    
        add( XEC_MUL,       "mul       %r %a %b" );
        add( XEC_DIV,       "div       %r %a %b" );
        add( XEC_MOD,       "mod       %r %a %b" );
        add( XEC_INTDIV,    "intdiv    %r %a %b" );
        add( XEC_ADD,       "add       %r %a %b" );
        add( XEC_SUB,       "sub       %r %a %b" );
        add( XEC_LSL,       "lsl       %r %a %b" );
        add( XEC_LSR,       "lsr       %r %a %b" );
        add( XEC_ASR,       "asr       %r %a %b" );
        add( XEC_AND,       "and       %r %a %b" );
        add( XEC_XOR,       "xor       %r %a %b" );
        add( XEC_OR,        "or        %r %a %b" );
        add( XEC_CONCAT,    "concat    %r %a %b" );

        add( XEC_TEST,      "test      %r %a" );
        add( XEC_LNOT,      "lnot      %r %a" );
        add( XEC_LXOR,      "lxor      %r %a %b" );
    
        add( XEC_KEY,       "key       %r %a %k" );
        add( XEC_INKEY,     "inkey     %r %a %b" );
        add( XEC_INDEX,     "index     %r %a %b" );
        add( XEC_ELEM,      "elem      %r %a $%b" );
        add( XEC_GLOBAL,    "global    %r %k" );
        
        add( XEC_SETKEY,    "setkey    %a %k %r" );
        add( XEC_SETINKEY,  "setinkey  %a %b %r" );
        add( XEC_SETINDEX,  "setindex  %a %b %r" );
        add( XEC_SETGLOBAL, "setglobal %k %r" );
    
        add( XEC_DELKEY,    "delkey    %a %k" );
        add( XEC_DELINKEY,  "delinkey  %a %b" );
    
        add( XEC_NEWUP,     "newup     ^%c %r" );
        add( XEC_SETUP,     "setup     ^%c %r" );
        add( XEC_REFUP,     "refup     $r ^%c" );
        add( XEC_CLOSE,     "close     ^[%c .. %r]" );
    
        add( XEC_EQ,        "eq        %r %a %b" );
        add( XEC_LT,        "lt        %r %a %b" );
        add( XEC_LE,        "le        %r %a %b" );
        add( XEC_IN,        "in        %r %a %b" );
        add( XEC_IS,        "is        %r %a %b" );

        add( XEC_JMP,       "jmp       %j" );
        add( XEC_IFTRUE,    "iftrue    %r %j" );
        add( XEC_IFFALSE,   "iffalse   %r %j" );
        add( XEC_IFITER,    "ifiter    %j" );
        add( XEC_IFDONE,    "ifdone    %j" );

        add( XEC_ITER,      "iter      %r %a" );
        add( XEC_ITERKEY,   "iterkey   %r %a" );
        add( XEC_NEXT1,     "next1     %r %a" );
        add( XEC_NEXT2,     "next2     %r %a %b" );
        add( XEC_NEXT,      "next      [%r .. %b] %a" );

        add( XEC_TABLE,     "table     %r $%c" );
        add( XEC_OBJECT,    "object    %r %a" );
        
        add( XEC_ARRAY,     "array     %r $%c" );
        add( XEC_UNPACK,    "unpack    [%r .. ] %a $%b" );
        add( XEC_APPEND,    "append    %a %r" );
        add( XEC_EXTEND,    "extend    %a [%r .. ]" );
    
        add( XEC_CLOSURE,   "closure   %r %f" );
        add( XEC_ENVUP,     "envup     ^%c" );

        add( XEC_VARARG,    "vararg    $%c" );
        add( XEC_VARALL,    "varall    [%r .. ]" );
    
        add( XEC_CALL,      "call      [%r .. %b] [%r .. %a]" );
        add( XEC_YCALL,     "ycall     [%r .. %b] [%r .. %a]" );
        add( XEC_YIELD,     "yield     [%r .. %b] [%r .. %a]" );
        add( XEC_NEW,       "new       %r [%a .. %b]" );
    
        add( XEC_RETURN,    "return    [%r .. %a]" );

    }
    
    const char* lookup( xec_opcode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( xec_opcode o, const char* n )
    {
        map.emplace( o, n );
    }

    std::unordered_map< int, const char* > map;

};


static const xec_opprint opprint;



static void xec_function_print( unsigned findex, xec_function* function )
{
    printf( "(%u) %s\n", findex, function->name() );
    printf( "    param_count  : %u\n", function->param_count() );
    printf( "    upval_count  : %u\n", function->upval_count() );
    printf( "    newup_count  : %u\n", function->newup_count() );
    printf( "    stack_count  : %u\n", function->stack_count() );
    printf( "    is_varargs   : %s\n",
                    function->is_varargs() ? "true" : "false" );
    printf( "    is_coroutine : %s\n",
                    function->is_coroutine() ? "true" : "false" );
    printf( "\n" );
    
    for ( unsigned i = 0; i < function->code_count(); ++i )
    {
        xec_opinst code = function->code( i );
        const char* format = opprint.lookup( code.opcode() );
        
        printf( "%04u ", i );
        
        for ( const char* c = format; c[ 0 ] != '\0'; c += 1 )
        {
            if ( c[ 0 ] == '%' )
            {
                switch ( c[ 1 ] )
                {
                case 'r':
                    printf( "%u", code.r() );
                    break;
                
                case 'a':
                    if ( code.a() == XEC_MARK )
                        printf( "@" );
                    else
                        printf( "%u", code.a() );
                    break;

                case 'b':
                    if ( code.b() == XEC_MARK )
                        printf( "@" );
                    else
                        printf( "%u", code.b() );
                    break;

                case 'c':
                    printf( "%u", code.c() );
                    break;
                
                case 'j':
                    printf( "&%04u", (unsigned)( (int)( i + 1 ) + code.j() ) );
                    break;
                
                case 'v':
                {
                    xec_value v = function->module()->value( code.c() );
                    if ( v.isboolean() )
                    {
                        printf( "%s", v.boolean() ? "true" : "false" );
                    }
                    else if ( v.isnumber() )
                    {
                        printf( "$%g", v.number() );
                    }
                    else if ( v.isstring() )
                    {
                        xec_string* string = v.string();
                        printf( " \"" );
                        for ( size_t i = 0; i < string->size(); ++i )
                        {
                            char c = string->at( i );
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
                    break;
                }
                
                case 'k':
                {
                    xec_key* key = function->module()->key( code.b() );
                    printf( "'%s'", key->c_str() );
                    break;
                }
                
                case 'f':
                {
                    xec_function* f = function->module()->function( code.c() );
                    printf( "function (%u) %s", code.c(), f->name() );
                    break;
                }
                
                default:
                    continue;
                }
                c += 1;
            }
            else
            {
                putchar( c[ 0 ] );
            }
        }
        
        printf( "\n" );
    }
    
    printf( "\n" );
}




void xec_module_print( xec_module* module )
{
    printf( "%s\n\n", module->name() );
    
    for ( unsigned i = 0; i < module->function_count(); ++i )
    {
        xec_function_print( i, module->function( i ) );
    }
}
