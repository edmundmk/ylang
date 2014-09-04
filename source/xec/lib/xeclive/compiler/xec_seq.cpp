//
//  xec_seq.cpp
//  xob
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq.h"
#include "xec_seq_astvisitors.h"



xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_node* astnode )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   astnode( astnode )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_name* name )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   name( name )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_args* args )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   args( args )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                const char* k )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   k( k )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                int i )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   i( i )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( NULL )
    ,   v( NULL )
{
    a->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, const char* k )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   k( k )
    ,   v( NULL )
{
    a->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, int i )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   i( i )
    ,   v( NULL )
{
    a->refcount += 1;
}


xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, xec_seq_op* b )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( b )
    ,   v( NULL )
{
    a->refcount += 1;
    b->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_name* name, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   name( name )
    ,   b( NULL )
    ,   v( v )
{
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                const char* k, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   k( k )
    ,   v( v )
{
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, const char* k, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   k( k )
    ,   v( v )
{
    a->refcount += 1;
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, xec_seq_op* b, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( b )
    ,   v( v )
{
    a->refcount += 1;
    b->refcount += 1;
    v->refcount += 1;
}




xec_seq_args::xec_seq_args( int rcount )
    :   rcount( rcount )
    ,   unpacked( NULL )
{
}








class xec_seq_opformats
{
public:

    xec_seq_opformats()
    {
        add( XEC_SEQ_NOP,       "nop"                   );

        add( XEC_SEQ_LITERAL,   "literal   %t"          );
        add( XEC_SEQ_ASTNODE,   "astnode   %t"          );
        
        add( XEC_SEQ_OBJREF,    "objref    %t"          );
        
        add( XEC_SEQ_NULL,      "null"                  );
        add( XEC_SEQ_ONE,       "one"                   );
        
        add( XEC_SEQ_POS,       "unary +   %a"          );
        add( XEC_SEQ_NEG,       "unary -   %a"          );
        add( XEC_SEQ_NOT,       "unary !   %a"          );
        add( XEC_SEQ_BITNOT,    "unary ~   %a"          );
        
        add( XEC_SEQ_MUL,       "*         %a %b"       );
        add( XEC_SEQ_DIV,       "/         %a %b"       );
        add( XEC_SEQ_MOD,       "%         %a %b"       );
        add( XEC_SEQ_INTDIV,    "~         %a %b"       );
        add( XEC_SEQ_ADD,       "+         %a %b"       );
        add( XEC_SEQ_SUB,       "-         %a %b"       );
        add( XEC_SEQ_LSL,       "<<        %a %b"       );
        add( XEC_SEQ_LSR,       ">>        %a %b"       );
        add( XEC_SEQ_ASR,       "~>>       %a %b"       );
        add( XEC_SEQ_BITAND,    "&         %a %b"       );
        add( XEC_SEQ_BITXOR,    "^         %a %b"       );
        add( XEC_SEQ_BITOR,     "|         %a %b"       );
        add( XEC_SEQ_CONCAT,    "..        %a %b"       );
        add( XEC_SEQ_EQ,        "==        %a %b"       );
        add( XEC_SEQ_NE,        "!=        %a %b"       );
        add( XEC_SEQ_LT,        "<         %a %b"       );
        add( XEC_SEQ_LE,        "<=        %a %b"       );
        add( XEC_SEQ_GT,        ">         %a %b"       );
        add( XEC_SEQ_GE,        ">=        %a %b"       );
        add( XEC_SEQ_IN,        "in        %a %b"       );
        add( XEC_SEQ_NOTIN,     "!in       %a %b"       );
        add( XEC_SEQ_IS,        "is        %a %b"       );
        add( XEC_SEQ_NOTIS,     "!is       %a %b"       );
        add( XEC_SEQ_XOR,       "^^        %a %b"       );
        
        add( XEC_SEQ_VAR,       "var       %n"          );
        add( XEC_SEQ_GLOBAL,    "global    %k"          );
        add( XEC_SEQ_KEY,       "key       %a %k"       );
        add( XEC_SEQ_INKEY,     "inkey     %a %b"       );
        add( XEC_SEQ_INDEX,     "index     %a %b"       );
        
        add( XEC_SEQ_TGLOBAL,   "tglobal   %k"          );
        add( XEC_SEQ_TKEY,      "tkey      %a %k"       );
        add( XEC_SEQ_TINKEY,    "tinkey    %a %b"       );

        add( XEC_SEQ_DECLARE,   "declare   %n %v"       );
        add( XEC_SEQ_SETVAR,    "setvar    %n %v"       );
        add( XEC_SEQ_SETGLOBAL, "setglobal %k %v"       );
        add( XEC_SEQ_SETKEY,    "setkey    %a %k %v"    );
        add( XEC_SEQ_SETINKEY,  "setinkey  %a %b %v"    );
        add( XEC_SEQ_SETINDEX,  "setindex  %a %b %v"    );
        
        add( XEC_SEQ_BEGINAND,  ">beginand  %a"         );
        add( XEC_SEQ_BEGINOR,   ">beginor   %a"         );
        add( XEC_SEQ_BEGINIF,   ">beginif   %a"         );
        add( XEC_SEQ_ELSE,      "<>else      %a"        );
        add( XEC_SEQ_END,       "<end       %a"         );
        
        add( XEC_SEQ_NEW,       "new       %g"          );
        add( XEC_SEQ_CALL,      "call      %g"          );
        add( XEC_SEQ_YCALL,     "ycall     %g"          );
        add( XEC_SEQ_YIELD,     "yield     %g"          );
        add( XEC_SEQ_UNPACK,    "unpack    %g"          );
        add( XEC_SEQ_VARALL,    "varall    %g"          );
        
        add( XEC_SEQ_SELECT,    "select    %a %i"       );
        add( XEC_SEQ_SELECT,    "element   %a %i"       );
        add( XEC_SEQ_SELECT,    "vararg    %a %i"       );
    }
    
    const char* lookup( xec_seq_opcode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( xec_seq_opcode o, const char* n )
    {
        map.emplace( o, n );
    }

    std::unordered_map< int, const char* > map;

};


static const xec_seq_opformats opformats;



void xec_seq_print( xec_seq* seq )
{
    std::unordered_map< xec_seq_op*, size_t > index;
    for ( size_t i = 0; i < seq->ops.size(); ++i )
    {
        index.emplace( seq->ops.at( i ), i );
    }
    
    for ( size_t i = 0; i < seq->ops.size(); ++i )
    {
        xec_seq_op* op = seq->ops.at( i );
        const char* format = opformats.lookup( op->opcode );
        
        printf( "%04d [%d]", (int)i, op->refcount );
        
        for ( const char* c = format; c[ 0 ] != '\0'; ++c )
        {
            if ( c[ 0 ] != '%' )
            {
                printf( "%c", c[ 0 ] );
            }
            else
            {
                switch ( c[ 1 ] )
                {
                case 't':
                {
                    printf( "~%p", op->astnode );
                    break;
                }
                
                case 'n':
                {
                    printf( "%s", op->name->name );
                    break;
                }
                
                case 'g':
                {
                    printf( "%d", op->args->rcount );
                    for ( size_t i = 0; i < op->args->values.size(); ++i )
                    {
                        xec_seq_op* arg = op->args->values.at( i );
                        printf( " :%04d", (int)index.at( arg ) );
                    }
                    if ( op->args->unpacked )
                    {
                        xec_seq_op* arg = op->args->unpacked;
                        printf( " :%04d ...", (int)index.at( arg ) );
                    }
                    break;
                }
                
                case 'a':
                {
                    printf( ":%04d", (int)index.at( op->a ) );
                    break;
                }
                
                case 'k':
                {
                    printf( "'%s'", op->k );
                    break;
                }
                
                case 'i':
                {
                    printf( "%d", op->i );
                    break;
                }
                
                case 'b':
                {
                    printf( ":%04d", (int)index.at( op->b ) );
                    break;
                }
                
                case 'v':
                {
                    printf( ":%04d", (int)index.at( op->v ) );
                    break;
                }
                }
            
                c += 1;
            }
        }
        
        printf( "\n" );
    }
}

