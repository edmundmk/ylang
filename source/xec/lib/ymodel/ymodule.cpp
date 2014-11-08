//
//  ymodule.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ymodule.h"



ometatype omodule::metatype = { &mark_module, "module" };


omodule* omodule::alloc()
{
    void* p = malloc( sizeof( omodule ) );
    return new ( p ) omodule( &metatype );
}


omodule::omodule( ometatype* metatype )
    :   obase( metatype )
{
}


void omodule::mark_module( oworklist* work, obase* object, ocolour colour )
{
    omodule* module = (omodule*)object;
    omarkwb( module->mname, work, colour );
    omarkwb( module->symbols, work, colour );
    omarkwb( module->values, work, colour );
    omarkwb( module->functions, work, colour );
}




ometatype ofunction::metatype = { &mark_function, "function" };

ofunction* ofunction::alloc( size_t size )
{
    void* p = malloc( sizeof( ofunction ) + sizeof( oinstruction ) * size );
    return new ( p ) ofunction( &metatype, size );
}


ofunction::ofunction( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   fparamcount( 0 )
    ,   fupvalcount( 0 )
    ,   fnewupcount( 0 )
    ,   fvarargs( false )
    ,   fcoroutine( false )
    ,   fsize( size )
{
    for ( size_t i = 0; i < size; ++i )
    {
        new ( fcode + i ) oinstruction();
    }
}


void ofunction::mark_function( oworklist* work, obase* object, ocolour colour )
{
    ofunction* f = (ofunction*)object;
    omarkwb( f->fmodule, work, colour );
    omarkwb( f->fname, work, colour );
}





namespace
{

class odisasm
{
public:

    odisasm()
    {
        add( O_NOP,       "nop" );
    
        add( O_NULL,      "null      %r" );
        add( O_VALUE,     "value     %r %v" );
    
        add( O_MOV,       "mov       %r %a" );
        add( O_SWP,       "swp       %r %a" );

        add( O_POS,       "pos       %r %a" );
        add( O_NEG,       "neg       %r %a" );
        add( O_NOT,       "not       %r %a" );
    
        add( O_MUL,       "mul       %r %a %b" );
        add( O_DIV,       "div       %r %a %b" );
        add( O_MOD,       "mod       %r %a %b" );
        add( O_INTDIV,    "intdiv    %r %a %b" );
        add( O_ADD,       "add       %r %a %b" );
        add( O_SUB,       "sub       %r %a %b" );
        add( O_LSL,       "lsl       %r %a %b" );
        add( O_LSR,       "lsr       %r %a %b" );
        add( O_ASR,       "asr       %r %a %b" );
        add( O_AND,       "and       %r %a %b" );
        add( O_XOR,       "xor       %r %a %b" );
        add( O_OR,        "or        %r %a %b" );
        add( O_CONCAT,    "concat    %r %a %b" );

        add( O_TEST,      "test      %r %a" );
        add( O_LNOT,      "lnot      %r %a" );
        add( O_LXOR,      "lxor      %r %a %b" );
    
        add( O_KEY,       "key       %r %a %k" );
        add( O_INKEY,     "inkey     %r %a %b" );
        add( O_INDEX,     "index     %r %a %b" );
        add( O_ELEM,      "elem      %r %a $%b" );
        add( O_GLOBAL,    "global    %r %k" );
        
        add( O_SETKEY,    "setkey    %a %k %r" );
        add( O_SETINKEY,  "setinkey  %a %b %r" );
        add( O_SETINDEX,  "setindex  %a %b %r" );
        add( O_SETGLOBAL, "setglobal %k %r" );
    
        add( O_DELKEY,    "delkey    %a %k" );
        add( O_DELINKEY,  "delinkey  %a %b" );
    
        add( O_NEWUP,     "newup     !%c %r" );
        add( O_SETNU,     "setnu     !%c %r" );
        add( O_REFNU,     "refnu     %r !%c" );
        add( O_CLOSE,     "close     ![%c .. %r]" );

        add( O_SETUP,     "setup     ^%c %r" );
        add( O_REFUP,     "refup     %r ^%c" );
    
        add( O_EQ,        "eq        %r %a %b" );
        add( O_LT,        "lt        %r %a %b" );
        add( O_LE,        "le        %r %a %b" );
        add( O_IN,        "in        %r %a %b" );
        add( O_IS,        "is        %r %a %b" );

        add( O_JMP,       "jmp       %j" );
        add( O_IFTRUE,    "iftrue    %r %j" );
        add( O_IFFALSE,   "iffalse   %r %j" );

        add( O_ITER,      "iter      %a" );
        add( O_ITERKEY,   "iterkey   %a" );
        add( O_NEXT1,     "next1     %r" );
        add( O_NEXT2,     "next2     %r %a" );
        add( O_NEXT,      "next      [%r .. %b]" );
        add( O_IFITER,    "ifiter    %j" );
        add( O_IFNITER,   "ifniter   %j" );
        add( O_POPITER,   "popiter" );

        add( O_TABLE,     "table     %r $%c" );
        add( O_OBJECT,    "object    %r %a" );
        
        add( O_ARRAY,     "array     %r $%c" );
        add( O_UNPACK,    "unpack    [%r .. ] %a $%b" );
        add( O_APPEND,    "append    %a %r" );
        add( O_EXTEND,    "extend    %a [%r .. ]" );
    
        add( O_CLOSURE,   "closure   %r %f" );
        add( O_ENVNU,     "envnu     !%c" );
        add( O_ENVUP,     "envup     ^%c" );

        add( O_VARARG,    "vararg    $%c" );
        add( O_VARALL,    "varall    [%r .. ]" );
    
        add( O_CALL,      "call      [%r .. %b] [%r .. %a]" );
        add( O_YCALL,     "ycall     [%r .. %b] [%r .. %a]" );
        add( O_YIELD,     "yield     [%r .. %b] [%r .. %a]" );
        add( O_NEW,       "new       %r [%a .. %b]" );
    
        add( O_RETURN,    "return    [%r .. %a]" );

    }
    
    const char* lookup( ocode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( ocode o, const char* n )
    {
        map.emplace( o, n );
    }

    std::unordered_map< int, const char* > map;

};


const odisasm disasm;


}



void ofunction::print( ofunction* function )
{
    printf( "%s\n", function->name()->c_str() );
    printf( "    param_count  : %u\n", function->param_count() );
    printf( "    upval_count  : %u\n", function->upval_count() );
    printf( "    newup_count  : %u\n", function->newup_count() );
    printf( "    stack_count  : %u\n", function->stack_count() );
    printf( "    is_varargs   : %s\n",
                    function->is_varargs() ? "true" : "false" );
    printf( "    is_coroutine : %s\n",
                    function->is_coroutine() ? "true" : "false" );
    printf( "\n" );
    
    for ( size_t ip = 0; ip < function->size(); ++ip )
    {
        oinstruction i = function->code()[ ip ];
        const char* format = disasm.lookup( i.opcode() );
        
        printf( "%04u ", (unsigned)ip );
        
        for ( const char* c = format; c[ 0 ] != '\0'; c += 1 )
        {
            if ( c[ 0 ] == '%' )
            {
                switch ( c[ 1 ] )
                {
                case 'r':
                    printf( "%u", i.r() );
                    break;
                
                case 'a':
                    if ( i.a() == O_MARK )
                        printf( "@" );
                    else
                        printf( "%u", i.a() );
                    break;

                case 'b':
                    if ( i.b() == O_MARK )
                        printf( "@" );
                    else
                        printf( "%u", i.b() );
                    break;

                case 'c':
                    printf( "%u", i.c() );
                    break;
                
                case 'j':
                    printf( "&%04u", (unsigned)( (int)( ip + 1 ) + i.j() ) );
                    break;
                
                case 'v':
                {
                    ovalue v = function->module()->value( i.c() );
                    if ( v.is_bool() )
                    {
                        printf( "%s", v.as_bool() ? "true" : "false" );
                    }
                    else if ( v.is_number() )
                    {
                        printf( "$%g", v.as_number() );
                    }
                    else if ( v.is_string() )
                    {
                        ostring* string = v.as_string();
                        printf( " \"" );
                        for ( size_t i = 0; i < string->size(); ++i )
                        {
                            char c = string->data()[ i ];
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
                    osymbol key = function->module()->symbol( i.b() );
                    printf( "'%s'", key->c_str() );
                    break;
                }
                
                case 'f':
                {
                    ofunction* f = function->module()->function( i.c() );
                    printf( "function (%u) %s", i.c(), f->name()->c_str() );
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


void omodule::print( omodule* module )
{
    printf( "%s\n\n", module->name()->c_str() );
    
    for ( size_t i = 0; i < module->functions->size(); ++i )
    {
        printf( "(%u) ", (unsigned)i );
        ofunction::print( module->functions->at( i ) );
    }
}


