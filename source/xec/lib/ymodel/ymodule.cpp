//
//  ymodule.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ymodule.h"



ymetatype ymodule::metatype = { &mark_module, "module" };


ymodule* ymodule::alloc()
{
    void* p = malloc( sizeof( ymodule ) );
    return new ( p ) ymodule( &metatype );
}


ymodule::ymodule( ymetatype* metatype )
    :   yobject( metatype )
{
}


void ymodule::mark_module( yobject* object, yworklist* work, ycolour colour )
{
    ymodule* module = (ymodule*)object;
    ymark( module->mname, work, colour );
    ymark( module->symbols, work, colour );
    ymark( module->values, work, colour );
    ymark( module->recipes, work, colour );
}




ymetatype yrecipe::metatype = { &mark_block, "routine" };

yrecipe* yrecipe::alloc( size_t size )
{
    void* p = malloc( sizeof( yrecipe ) + sizeof( yinstruction ) * size );
    return new ( p ) yrecipe( &metatype, size );
}


yrecipe::yrecipe( ymetatype* metatype, size_t size )
    :   yobject( metatype )
    ,   fparamcount( 0 )
    ,   fupvalcount( 0 )
    ,   fnewupcount( 0 )
    ,   fvarargs( false )
    ,   fcoroutine( false )
    ,   fsize( size )
{
    for ( size_t i = 0; i < size; ++i )
    {
        new ( fcode + i ) yinstruction();
    }
}


void yrecipe::mark_block( yobject* object, yworklist* work, ycolour colour )
{
    yrecipe* f = (yrecipe*)object;
    ymark( f->fmodule, work, colour );
    ymark( f->fname, work, colour );
}





namespace
{

class odisasm
{
public:

    odisasm()
    {
        add( Y_NOP,       "nop" );
    
        add( Y_NULL,      "null      %r" );
        add( Y_VALUE,     "value     %r %v" );
    
        add( Y_MOV,       "mov       %r %a" );
        add( Y_SWP,       "swp       %r %a" );

        add( Y_POS,       "pos       %r %a" );
        add( Y_NEG,       "neg       %r %a" );
        add( Y_NOT,       "not       %r %a" );
    
        add( Y_MUL,       "mul       %r %a %b" );
        add( Y_DIV,       "div       %r %a %b" );
        add( Y_MOD,       "mod       %r %a %b" );
        add( Y_INTDIV,    "intdiv    %r %a %b" );
        add( Y_ADD,       "add       %r %a %b" );
        add( Y_SUB,       "sub       %r %a %b" );
        add( Y_LSL,       "lsl       %r %a %b" );
        add( Y_LSR,       "lsr       %r %a %b" );
        add( Y_ASR,       "asr       %r %a %b" );
        add( Y_AND,       "and       %r %a %b" );
        add( Y_XOR,       "xor       %r %a %b" );
        add( Y_OR,        "or        %r %a %b" );
        add( Y_CONCAT,    "concat    %r %a %b" );

        add( Y_EQ,        "eq        %r %a %b" );
        add( Y_LT,        "lt        %r %a %b" );
        add( Y_LE,        "le        %r %a %b" );
        add( Y_IN,        "in        %r %a %b" );
        add( Y_IS,        "is        %r %a %b" );

        add( Y_TEST,      "test      %r %a" );
        add( Y_LNOT,      "lnot      %r %a" );
        add( Y_LXOR,      "lxor      %r %a %b" );
    
        add( Y_KEY,       "key       %r %a %k" );
        add( Y_INKEY,     "inkey     %r %a %b" );
        add( Y_INDEX,     "index     %r %a %b" );
        add( Y_ELEM,      "elem      %r %a $%b" );
        add( Y_GLOBAL,    "global    %r %k" );
        
        add( Y_SETKEY,    "setkey    %a %k %r" );
        add( Y_SETINKEY,  "setinkey  %a %b %r" );
        add( Y_SETINDEX,  "setindex  %a %b %r" );
        add( Y_SETGLOBAL, "setglobal %k %r" );
    
        add( Y_DELKEY,    "delkey    %a %k" );
        add( Y_DELINKEY,  "delinkey  %a %b" );
    
        add( Y_NEWNU,     "newnu     !%c %r" );
        add( Y_SETNU,     "setnu     !%c %r" );
        add( Y_REFNU,     "refnu     %r !%c" );
        add( Y_CLONU,     "clonu     ![%c .. %r]" );

        add( Y_SETUP,     "setup     ^%c %r" );
        add( Y_REFUP,     "refup     %r ^%c" );
    
        add( Y_JMP,       "jmp       %j" );
        add( Y_IFTRUE,    "iftrue    %r %j" );
        add( Y_IFFALSE,   "iffalse   %r %j" );

        add( Y_ITER,      "iter      %a" );
        add( Y_ITERKEY,   "iterkey   %a" );
        add( Y_NEXT1,     "next1     %r" );
        add( Y_NEXT2,     "next2     %r %a" );
        add( Y_NEXT,      "next      [%r .. %b]" );
        add( Y_IFITER,    "ifiter    %j" );
        add( Y_IFNITER,   "ifniter   %j" );
        add( Y_POPITER,   "popiter   %c" );

        add( Y_TABLE,     "table     %r $%c" );
        add( Y_OBJECT,    "object    %r %a" );
        
        add( Y_ARRAY,     "array     %r $%c" );
        add( Y_UNPACK,    "unpack    [%r .. ] %a $%b" );
        add( Y_APPEND,    "append    %a %r" );
        add( Y_EXTEND,    "extend    %a [%r .. ]" );
    
        add( Y_CLOSURE,   "closure   %r %f" );
        add( Y_ENVNU,     "envnu     !%c" );
        add( Y_ENVUP,     "envup     ^%c" );

        add( Y_VARARG,    "vararg    $%c" );
        add( Y_VARALL,    "varall    [%r .. @]" );
    
        add( Y_CALL,      "call      [%r .. %b] [%r .. %a]" );
        add( Y_YCALL,     "ycall     [%r .. %b] [%r .. %a]" );
        add( Y_YIELD,     "yield     [%r .. %b] [%r .. %a]" );
    
        add( Y_RETURN,    "return    [%r .. %a]" );

    }
    
    const char* lookup( ycode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( ycode o, const char* n )
    {
        map.emplace( o, n );
    }

    std::unordered_map< int, const char* > map;

};


const odisasm disasm;


}



void yrecipe::print( yrecipe* routine )
{
    printf( "%s\n", routine->name()->c_str() );
    printf( "    param_count  : %u\n", routine->param_count() );
    printf( "    upval_count  : %u\n", routine->upval_count() );
    printf( "    newup_count  : %u\n", routine->newup_count() );
    printf( "    stack_count  : %u\n", routine->stack_count() );
    printf( "    is_varargs   : %s\n",
                    routine->is_varargs() ? "true" : "false" );
    printf( "    is_coroutine : %s\n",
                    routine->is_coroutine() ? "true" : "false" );
    printf( "\n" );
    
    for ( size_t ip = 0; ip < routine->size(); ++ip )
    {
        yinstruction i = routine->code()[ ip ];
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
                    if ( i.a() == Y_MARK )
                        printf( "@" );
                    else
                        printf( "%u", i.a() );
                    break;

                case 'b':
                    if ( i.b() == Y_MARK )
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
                    yvalue v = routine->module()->value( i.c() );
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
                        ystring* string = v.as_string();
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
                    ysymbol key = routine->module()->symbol( i.b() );
                    printf( "'%s'", key->c_str() );
                    break;
                }
                
                case 'f':
                {
                    yrecipe* f = routine->module()->recipe( i.c() );
                    printf( "routine (%u) %s", i.c(), f->name()->c_str() );
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


void ymodule::print( ymodule* module )
{
    printf( "%s\n\n", module->name()->c_str() );
    
    for ( size_t i = 0; i < module->recipes->size(); ++i )
    {
        printf( "(%u) ", (unsigned)i );
        yrecipe::print( module->recipes->get( i ) );
    }
}


