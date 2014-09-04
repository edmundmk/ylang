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

}

