//
//  yssa.cpp
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa.h"
#include "escstring.h"


/*
    Constructors
*/

yssa_opinst::yssa_opinst(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
    :   sloc( sloc )
    ,   stacktop( yl_opinst::NOVAL )
    ,   opcode( opcode )
    ,   operand_count( operand_count )
    ,   result_count( result_count )
    ,   r( yl_opinst::NOVAL )
    ,   live( nullptr )
    ,   variable( nullptr )
    ,   number( 0.0 )
{
    for ( size_t i = 0; i < operand_count; ++i )
    {
        operand[ i ] = nullptr;
    }
}


yssa_string::yssa_string( const char* string, size_t length )
    :   string( string )
    ,   length( length )
{
}


yssa_variable::yssa_variable(
                const char* name, int sloc, bool xcref, uint8_t localup )
    :   name( name )
    ,   sloc( sloc )
    ,   xcref( xcref )
    ,   localup( localup )
    ,   r( yl_opinst::NOVAL )
    ,   live( nullptr )
    ,   argof( nullptr )
{
}


yssa_function::yssa_function( int sloc, const char* funcname,
                bool varargs, bool coroutine, size_t paramcount )
    :   sloc( sloc )
    ,   funcname( funcname )
    ,   varargs( varargs )
    ,   coroutine( coroutine )
    ,   paramcount( paramcount )
{
}


yssa_block::yssa_block( unsigned flags )
    :   flags( flags )
    ,   xclocalups( 0 )
    ,   xcitercount( 0 )
    ,   index( -1 )
    ,   loop( nullptr )
    ,   test( nullptr )
    ,   next( nullptr )
    ,   fail( nullptr )
    ,   xchandler( nullptr )
    ,   lstart( -1 )
    ,   lfinal( -1 )
{
}


yssa_loop::yssa_loop( yssa_loop* parent, yssa_block* header )
    :   parent( parent )
    ,   header( header )
{
}


yssa_live_range::yssa_live_range(
                size_t start, size_t final, yssa_live_range* next )
    :   start( start )
    ,   final( final )
    ,   next( next )
{
}




/*
    Linearization.
*/

void yssa_linearize( yssa_function* function )
{
    size_t opcount = 0;
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        opcount += block->phi.size();
        opcount += block->ops.size();
    }
    
    function->ops.clear();
    function->ops.reserve( opcount );
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        block->lstart = function->ops.size();
        function->ops.insert
        (
            function->ops.end(),
            block->phi.begin(),
            block->phi.end()
        );
        block->lfiphi = function->ops.size();
        function->ops.insert
        (
            function->ops.end(),
            block->ops.begin(),
            block->ops.end()
        );
        block->lfinal = function->ops.size();
        
        block->flags |= YSSA_LINEARIZED;
        block->phi.clear();
        block->ops.clear();
    }
}



/*
    Printing
*/


static void yssa_print_function( yssa_function* f );
static void yssa_print_block( yssa_function* f, yssa_block* b );
static void yssa_print_opinst( yssa_function* f, yssa_opinst* o );


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
    printf( "-- loops:\n" );
    for ( size_t i = 0; i < f->loops.size(); ++i )
    {
        yssa_loop* loop = f->loops.at( i ).get();
        printf
        (
            "    %p parent: %p header: %p\n",
            loop,
            loop->parent,
            loop->header
        );
    }
    for ( size_t i = 0; i < f->blocks.size(); ++i )
    {
        yssa_print_block( f, f->blocks.at( i ).get() );
    }
    for ( size_t i = 0; i < f->ops.size(); ++i )
    {
        yssa_opinst* o = f->ops.at( i );
        printf( "%04zX : ", i );
        printf( "%p : ", o );
        yssa_print_opinst( f, o );
    }
    printf( "\n" );
}


void yssa_print_block( yssa_function* f, yssa_block* b )
{
    printf( "-- %p", b );
    if ( b->flags & YSSA_LOOP )
        printf( " loop" );
    if ( b->flags & YSSA_XCHANDLER )
        printf( " xchandler [ %d %d ]", b->xclocalups, b->xcitercount );
    if ( b->flags & YSSA_UNSEALED )
        printf( " unsealed" );
    if ( b->flags & YSSA_LOOKUP )
        printf( " lookup" );
    if ( b->flags & YSSA_PREFER_FAIL )
        printf( " prefer-fail" );
    if ( b->flags & YSSA_LINEARIZED )
        printf( " linearized" );
    printf( "\n" );
    
    printf( "    loop : %p\n", b->loop );
    
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
        yssa_print_opinst( f, o );
    }
    for ( size_t i = 0; i < b->ops.size(); ++i )
    {
        yssa_opinst* o = b->ops.at( i );
        printf( "%p : ", o );
        yssa_print_opinst( f, o );
    }
    
    if ( b->flags & YSSA_LINEARIZED )
    {
        printf( "        %04zX:%04zX\n", b->lstart, b->lfinal );
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
        add( YL_FUNCTION,   "function %f" );
        add( YL_CALL,       "call" );
        add( YL_YCALL,      "ycall" );
        add( YL_YIELD,      "yield" );
        add( YL_RETURN,     "return" );
    
        add( YL_ITER,       "iterkey %r" );
        add( YL_ITERKEY,    "iterkey %r" );
        add( YL_NEXT1,      "next1 %a" );
        add( YL_NEXT2,      "next2 %a" );
        add( YL_NEXT,       "next %a" );

        add( YL_JMPV,       "jmpv" );
        add( YL_JMPN,       "jmpn" );

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
        add( YSSA_ITEREACH, "itereach %r" );
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



void yssa_print_opinst( yssa_function* f, yssa_opinst* o )
{
    if ( o->r != yl_opinst::NOVAL )
    {
        printf( "[%2d]", o->r );
    }
    else
    {
        printf( "[--]" );
    }
    
    if ( o->stacktop != yl_opinst::NOVAL )
    {
        printf( "[%2d] ", o->stacktop );
    }
    else
    {
        printf( " " );
    }
    
    if ( o->result_count != yl_opinst::MARK )
    {
        printf( "%2d ", o->result_count );
    }
    else
    {
        printf( "** " );
    }

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
                {
                    std::string s( o->string->string, o->string->length );
                    printf( "\"%s\"", escstring( s ).c_str() );
                    break;
                }
                    
                case 'f':
                    printf( "%s", o->function->funcname );
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
        yssa_opinst* operand = o->operand[ i ];
        if ( operand->opcode != YSSA_REF )
        {
            printf( " %p", operand );
        }
        else
        {
            printf( " %p->%p", operand, operand->operand[ 0 ] );
        }
    }
    
    if ( o->has_multival() && o->multival )
    {
        if ( o->multival->opcode != YSSA_REF )
        {
            printf( " %p ... ", o->multival );
        }
        else
        {
            printf( " %p->%p ... ",
                    o->multival, o->multival->operand[ 0 ] );
        }
    }
    
    if ( o->live )
    {
        printf( " ~" );
    }
    
    for ( yssa_live_range* live = o->live; live; live = live->next )
    {
        printf( " %04zX:%04zX", live->start, live->final );
    }
    
    auto i = f->argof.find( o );
    if ( i != f->argof.end() )
    {
        printf( " >>%p", i->second );
    }
    
    if ( ! o->has_associated() && o->variable )
    {
        printf( " ~" );
        if ( o->variable->r != yl_opinst::NOVAL )
            printf( " [%2d]", o->variable->r );
        else
            printf( " [--]" );
        printf( " %s", o->variable->name );
        if ( o->variable->xcref )
            printf( " xcref" );
        if ( o->variable->localup != yl_opinst::NOVAL )
            printf( " %d", o->variable->localup );
        for ( yssa_live_range* live = o->variable->live; live; live = live->next )
            printf( " %04zX:%04zX", live->start, live->final );
        if ( o->variable->argof )
            printf( " >>%p", o->variable->argof );
    }
    
    printf( "\n" );
}






