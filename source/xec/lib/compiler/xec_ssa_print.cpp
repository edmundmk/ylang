//
//  xec_ssa_print.cpp
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_print.h"
#include <unordered_map>
#include "xec_ssa.h"



class xec_ssa_printer
{
public:

    xec_ssa_printer();
    
    void print_func( xec_ssa_func* func );


private:

    void print_node( xec_ssa_node* node );

    std::unordered_map< xec_ssa_block*, int > blockid;
    std::unordered_map< xec_ssa_node*, int > nodeid;

};



xec_ssa_printer::xec_ssa_printer()
{
}


void xec_ssa_printer::print_func( xec_ssa_func* func )
{
    // Build ids.
    int nextid = 1;
    nodeid.emplace( nullptr, 0 );
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );
        blockid.emplace( block, (int)i );
        
        for ( size_t i = 0; i < block->phi.size(); ++i )
        {
            xec_ssa_node* phi = block->phi.at( i );
            nodeid.emplace( phi, nextid );
            nextid += 1;
        }
        
        for ( size_t i = 0; i < block->nodes.size(); ++i )
        {
            xec_ssa_node* node = block->nodes.at( i );
            nodeid.emplace( node, nextid );
            nextid += 1;
        }
    }
    
    
    // Print function.
    printf( "function %p %s\n", func, func->funcname );
    printf( "    block        : [%04X]\n", blockid.at( func->block ) );
    printf( "    upvalcount   : %d\n", func->upvalcount );
    printf( "    localupcount : %d\n", func->localupcount );
    printf( "    paramcount   : %d\n", func->paramcount );
    printf( "    varargs      : %s\n", func->varargs ? "true" : "false" );
    printf( "    coroutine    : %s\n", func->coroutine ? "true" : "false" );
    
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );

        printf( "\n" );
        printf( "[%04X]\n", blockid.at( block ) );

        for ( size_t i = 0; i < block->phi.size(); ++i )
        {
            print_node( block->phi.at( i ) );
        }

        for ( size_t i = 0; i < block->nodes.size(); ++i )
        {
            print_node( block->nodes.at( i ) );
        }
        
        if ( block->condition )
        {
            printf( "  ?? :%04X\n", nodeid.at( block->condition ) );
            printf( "    -> [%04X]\n", blockid.at( block->iftrue ) );
            printf( "    -> [%04X]\n", blockid.at( block->iffalse ) );
        }
        else if ( block->next )
        {
            printf( "  -> [%04X]\n", blockid.at( block->next ) );
        }
    }
    
    printf( "\n\n" );
}



enum xec_ssa_decode_kind
{
    XEC_SSA_PACKED_NULL,
    XEC_SSA_PACKED_OPERAND,
    XEC_SSA_PACKED_BINARY,
    XEC_SSA_PACKED_LITERAL,
    XEC_SSA_PACKED_OPLITERAL,
    XEC_SSA_PACKED_IMMED,
    XEC_SSA_PACKED_OPIMMED,
    XEC_SSA_PACKED_STRING,
    XEC_SSA_PACKED_BOOLEAN,
    XEC_SSA_PACKED_NUMBER,
    
    XEC_SSA_TRIPLE,
    XEC_SSA_TRIPLE_KEY,
    
    XEC_SSA_EXPAND,
    XEC_SSA_EXPAND_FUNC,
};


struct xec_ssa_decode_info
{
    xec_ssa_decode_info( const char* name, xec_ssa_decode_kind kind )
        :   name( name )
        ,   kind( kind )
    {
    }
    
    const char* name;
    xec_ssa_decode_kind kind;
    
};


class xec_ssa_decode
{
public:

    xec_ssa_decode()
    {
        add( XEC_SSA_NOP,       "nop",          XEC_SSA_PACKED_NULL );
        add( XEC_SSA_PHI,       "phi",          XEC_SSA_EXPAND );
        add( XEC_SSA_PSI,       "psi",          XEC_SSA_EXPAND );
        add( XEC_SSA_CLOSURE,   "closure",      XEC_SSA_EXPAND_FUNC );
        add( XEC_SSA_PARAM,     "param",        XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_CALL,      "call",         XEC_SSA_EXPAND );
        add( XEC_SSA_YIELD,     "yield",        XEC_SSA_EXPAND );
        add( XEC_SSA_SELECT,    "select",       XEC_SSA_PACKED_OPIMMED );
        add( XEC_SSA_VARARG,    "vararg",       XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_UNPACK,    "unpack",       XEC_SSA_PACKED_OPIMMED );
        add( XEC_SSA_ITER,      "iter",         XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_EACH,      "each",         XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_NEXT,      "next",         XEC_SSA_PACKED_OPIMMED );
        add( XEC_SSA_NULL,      "null",         XEC_SSA_PACKED_NULL );
        add( XEC_SSA_BOOL,      "bool",         XEC_SSA_PACKED_BOOLEAN );
        add( XEC_SSA_NUMBER,    "number",       XEC_SSA_PACKED_NUMBER );
        add( XEC_SSA_STRING,    "string",       XEC_SSA_PACKED_STRING );
        add( XEC_SSA_NEWUP,     "newup",        XEC_SSA_PACKED_OPIMMED );
        add( XEC_SSA_REFUP,     "refup",        XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_SETUP,     "setup",        XEC_SSA_PACKED_OPIMMED );
        add( XEC_SSA_CLOSE,     "close",        XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_GLOBAL,    "global",       XEC_SSA_PACKED_LITERAL );
        add( XEC_SSA_KEY,       "key",          XEC_SSA_PACKED_OPLITERAL );
        add( XEC_SSA_INKEY,     "inkey",        XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_INDEX,     "index",        XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_SETGLOBAL, "setglobal",    XEC_SSA_PACKED_OPLITERAL );
        add( XEC_SSA_SETKEY,    "setkey",       XEC_SSA_TRIPLE_KEY );
        add( XEC_SSA_SETINKEY,  "setinkey",     XEC_SSA_TRIPLE );
        add( XEC_SSA_SETINDEX,  "setindex",     XEC_SSA_TRIPLE );
        add( XEC_SSA_DELKEY,    "delkey",       XEC_SSA_PACKED_OPLITERAL );
        add( XEC_SSA_DELINKEY,  "delinkey",     XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_POS,       "pos",          XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_NEG,       "neg",          XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_NOT,       "not",          XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_BITNOT,    "bitnot",       XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_MUL,       "mul",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_DIV,       "div",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_MOD,       "mod",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_INTDIV,    "intdiv",       XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_ADD,       "add",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_SUB,       "sub",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_LSL,       "lsl",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_LSR,       "lsr",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_ASR,       "asr",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_BITAND,    "bitand",       XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_BITXOR,    "bitxor",       XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_BITOR,     "bitor",        XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_CONCAT,    "concat",       XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_EQ,        "eq",           XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_LT,        "lt",           XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_LE,        "le",           XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_IN,        "in",           XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_IS,        "is",           XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_XOR,       "xor",          XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_NEW,       "new",          XEC_SSA_EXPAND );
        add( XEC_SSA_OBJECT,    "object",       XEC_SSA_PACKED_OPERAND );
        add( XEC_SSA_TABLE,     "table",        XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_ARRAY,     "array",        XEC_SSA_PACKED_IMMED );
        add( XEC_SSA_APPEND,    "append",       XEC_SSA_PACKED_BINARY );
        add( XEC_SSA_EXTEND,    "extend",       XEC_SSA_EXPAND );
        add( XEC_SSA_RETURN,    "return",       XEC_SSA_EXPAND );
        add( XEC_SSA_CATCH,     "catch",        XEC_SSA_PACKED_NULL );
        add( XEC_SSA_RETHROW,   "rethrow",      XEC_SSA_PACKED_NULL );
    }
    
    const xec_ssa_decode_info& lookup( xec_ssa_opcode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( xec_ssa_opcode o, const char* n, xec_ssa_decode_kind k )
    {
        map.emplace( o, xec_ssa_decode_info( n, k ) );
    }

    std::unordered_map< int, xec_ssa_decode_info > map;

};


static const xec_ssa_decode decode;




void xec_ssa_printer::print_node( xec_ssa_node* node )
{
    const xec_ssa_decode_info& d = decode.lookup( node->opcode );
    printf( ":%04X %-10s", nodeid.at( node ), d.name );
    
    switch ( d.kind )
    {
    case XEC_SSA_PACKED_NULL:
        break;
        
    case XEC_SSA_PACKED_OPERAND:
        printf( ":%04X", nodeid.at( node->as_packed()->operanda ) );
        break;
    
    case XEC_SSA_PACKED_BINARY:
        printf( ":%04X, :%04X",
                    nodeid.at( node->as_packed()->operanda ),
                    nodeid.at( node->as_packed()->operandb ) );
        break;

    case XEC_SSA_PACKED_LITERAL:
        printf( "'%s'", node->as_packed()->literal );
        break;
        
    case XEC_SSA_PACKED_OPLITERAL:
        printf( ":%04X, '%s'",
                    nodeid.at( node->as_packed()->operanda ),
                    node->as_packed()->literal );
        break;
        
    case XEC_SSA_PACKED_IMMED:
        printf( "$%d", node->as_packed()->immediate );
        break;
        
    case XEC_SSA_PACKED_OPIMMED:
        printf( ":%04X, $%d",
                    nodeid.at( node->as_packed()->operanda ),
                    node->as_packed()->immediate );
        break;
        
    case XEC_SSA_PACKED_STRING:
        printf( "\"%*s\"",
                    (int)node->as_packed()->length,
                    node->as_packed()->string );
        break;
        
    case XEC_SSA_PACKED_BOOLEAN:
        printf( "%s", node->as_packed()->boolean ? "true" : "false" );
        break;
        
    case XEC_SSA_PACKED_NUMBER:
        printf( "%g", node->as_packed()->number );
        break;
        
    case XEC_SSA_TRIPLE:
        printf( ":%04X, :%04X, :%04X",
                    nodeid.at( node->as_triple()->object ),
                    nodeid.at( node->as_triple()->index ),
                    nodeid.at( node->as_triple()->value ) );
        break;
        
    case XEC_SSA_TRIPLE_KEY:
        printf( ":%04X, '%s', :%04X",
                    nodeid.at( node->as_triple()->object ),
                    node->as_triple()->key,
                    nodeid.at( node->as_triple()->value ) );
        break;
        
    case XEC_SSA_EXPAND:
    case XEC_SSA_EXPAND_FUNC:
        break;
    }
    
    printf( "\n" );
}








void xec_ssa_print( xec_ssa* ssa )
{
    for ( size_t i = 0; i < ssa->functions.size(); ++i )
    {
        xec_ssa_printer printer;
        printer.print_func( ssa->functions.at( i ) );
    }
}





