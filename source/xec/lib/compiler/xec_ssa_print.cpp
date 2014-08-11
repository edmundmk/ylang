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

    void print_node( xec_ssa_block* block, xec_ssa_node* node );
    void print_names( xec_ssa_block* block, xec_ssa_node* node );

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
        blockid.emplace( block, (int)i + 1 );
        
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
            print_node( block, block->phi.at( i ) );
        }

        for ( size_t i = 0; i < block->nodes.size(); ++i )
        {
            print_node( block, block->nodes.at( i ) );
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





class xec_ssa_opnames
{
public:

    xec_ssa_opnames()
    {
        add( XEC_SSA_NOP,       "nop"       );
        add( XEC_SSA_MOV,       "mov"       );
        add( XEC_SSA_PHI,       "phi"       );
        add( XEC_SSA_PSI,       "psi"       );
        add( XEC_SSA_CLOSURE,   "closure"   );
        add( XEC_SSA_PARAM,     "param"     );
        add( XEC_SSA_CALL,      "call"      );
        add( XEC_SSA_YCALL,     "ycall"     );
        add( XEC_SSA_YIELD,     "yield"     );
        add( XEC_SSA_SELECT,    "select"    );
        add( XEC_SSA_VARARG,    "vararg"    );
        add( XEC_SSA_UNPACK,    "unpack"    );
        add( XEC_SSA_ITER,      "iter"      );
        add( XEC_SSA_EACH,      "each"      );
        add( XEC_SSA_NEXT,      "next"      );
        add( XEC_SSA_NULL,      "null"      );
        add( XEC_SSA_BOOL,      "bool"      );
        add( XEC_SSA_NUMBER,    "number"    );
        add( XEC_SSA_STRING,    "string"    );
        add( XEC_SSA_NEWUP,     "newup"     );
        add( XEC_SSA_REFUP,     "refup"     );
        add( XEC_SSA_SETUP,     "setup"     );
        add( XEC_SSA_CLOSE,     "close"     );
        add( XEC_SSA_GLOBAL,    "global"    );
        add( XEC_SSA_KEY,       "key"       );
        add( XEC_SSA_INKEY,     "inkey"     );
        add( XEC_SSA_INDEX,     "index"     );
        add( XEC_SSA_SETGLOBAL, "setglobal" );
        add( XEC_SSA_SETKEY,    "setkey"    );
        add( XEC_SSA_SETINKEY,  "setinkey"  );
        add( XEC_SSA_SETINDEX,  "setindex"  );
        add( XEC_SSA_DELKEY,    "delkey"    );
        add( XEC_SSA_DELINKEY,  "delinkey"  );
        add( XEC_SSA_POS,       "pos"       );
        add( XEC_SSA_NEG,       "neg"       );
        add( XEC_SSA_NOT,       "not"       );
        add( XEC_SSA_BITNOT,    "bitnot"    );
        add( XEC_SSA_MUL,       "mul"       );
        add( XEC_SSA_DIV,       "div"       );
        add( XEC_SSA_MOD,       "mod"       );
        add( XEC_SSA_INTDIV,    "intdiv"    );
        add( XEC_SSA_ADD,       "add"       );
        add( XEC_SSA_SUB,       "sub"       );
        add( XEC_SSA_LSL,       "lsl"       );
        add( XEC_SSA_LSR,       "lsr"       );
        add( XEC_SSA_ASR,       "asr"       );
        add( XEC_SSA_BITAND,    "bitand"    );
        add( XEC_SSA_BITXOR,    "bitxor"    );
        add( XEC_SSA_BITOR,     "bitor"     );
        add( XEC_SSA_CONCAT,    "concat"    );
        add( XEC_SSA_EQ,        "eq"        );
        add( XEC_SSA_LT,        "lt"        );
        add( XEC_SSA_LE,        "le"        );
        add( XEC_SSA_IN,        "in"        );
        add( XEC_SSA_IS,        "is"        );
        add( XEC_SSA_XOR,       "xor"       );
        add( XEC_SSA_NEW,       "new"       );
        add( XEC_SSA_OBJECT,    "object"    );
        add( XEC_SSA_TABLE,     "table"     );
        add( XEC_SSA_ARRAY,     "array"     );
        add( XEC_SSA_APPEND,    "append"    );
        add( XEC_SSA_EXTEND,    "extend"    );
        add( XEC_SSA_RETURN,    "return"    );
        add( XEC_SSA_CATCH,     "catch"     );
        add( XEC_SSA_RETHROW,   "rethrow"   );
    }
    
    const char* lookup( xec_ssa_opcode o ) const
    {
        return map.at( o );
    }
    
private:

    void add( xec_ssa_opcode o, const char* n )
    {
        map.emplace( o, n );
    }

    std::unordered_map< int, const char* > map;

};


static const xec_ssa_opnames opnames;




void xec_ssa_printer::print_node( xec_ssa_block* block, xec_ssa_node* node )
{
    const char* opname = opnames.lookup( node->opcode );

    printf( ":%04X %-10s", nodeid.at( node ), opname );
    
    switch ( xec_ssa_decode( node->opcode ) )
    {
    case XEC_SSA_PACKED:
        break;
        
    case XEC_SSA_PACKED_O:
        printf( ":%04X", nodeid.at( node->as_packed()->operanda ) );
        break;
    
    case XEC_SSA_PACKED_OO:
        printf( ":%04X :%04X",
                    nodeid.at( node->as_packed()->operanda ),
                    nodeid.at( node->as_packed()->operandb ) );
        break;

    case XEC_SSA_PACKED_L:
        printf( "'%s'", node->as_packed()->literal );
        break;
        
    case XEC_SSA_PACKED_OL:
        printf( ":%04X '%s'",
                    nodeid.at( node->as_packed()->operanda ),
                    node->as_packed()->literal );
        break;
        
    case XEC_SSA_PACKED_I:
        printf( "$%d", node->as_packed()->immediate );
        break;
        
    case XEC_SSA_PACKED_OI:
        printf( ":%04X, $%d",
                    nodeid.at( node->as_packed()->operanda ),
                    node->as_packed()->immediate );
        break;
        
    case XEC_SSA_PACKED_S:
        printf( "\"%*s\"",
                    (int)node->as_packed()->length,
                    node->as_packed()->string );
        break;
        
    case XEC_SSA_PACKED_B:
        printf( "%s", node->as_packed()->boolean ? "true" : "false" );
        break;
        
    case XEC_SSA_PACKED_N:
        printf( "%g", node->as_packed()->number );
        break;
        
    case XEC_SSA_TRIPLE:
        printf( ":%04X :%04X :%04X",
                    nodeid.at( node->as_triple()->object ),
                    nodeid.at( node->as_triple()->index ),
                    nodeid.at( node->as_triple()->value ) );
        break;
        
    case XEC_SSA_TRIPLE_K:
        printf( ":%04X '%s' :%04X",
                    nodeid.at( node->as_triple()->object ),
                    node->as_triple()->key,
                    nodeid.at( node->as_triple()->value ) );
        break;
        
    case XEC_SSA_EXPAND:
    {
        xec_ssa_expand* expand = node->as_expand();
        printf( "$%d", expand->valcount );
        print_names( block, node );
        for ( size_t i = 0; i < expand->operands.size(); ++i )
        {
            printf( "\n    :%04X", nodeid.at( expand->operands.at( i ) ) );
        }
        if ( expand->unpacked )
        {
            printf( "\n    :%04X (unpacked)", nodeid.at( expand->unpacked ) );
        }
        printf( "\n" );
        return;
    }
    
    case XEC_SSA_EXPAND_F:
    {
        xec_ssa_expand* expand = node->as_expand();
        printf( "%p", expand->func );
        print_names( block, node );
        for ( size_t i = 0; i < expand->operands.size(); ++i )
        {
            printf( "\n    :%04X", nodeid.at( expand->operands.at( i ) ) );
        }
        printf( "\n" );
        return;
    }
    }


    print_names( block, node );
    printf( "\n" );
}



void xec_ssa_printer::print_names( xec_ssa_block* block, xec_ssa_node* node )
{
    auto ii = block->names.equal_range( node );
    size_t count = 0;
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        if ( count == 0 )
            printf( "( " );
        else
            printf( ", " );
        printf( "%s", i->second->name );
        count += 1;
    }
    if ( count > 0 )
    {
        printf( " )" );
    }
}





void xec_ssa_print( xec_ssa* ssa )
{
    for ( size_t i = 0; i < ssa->functions.size(); ++i )
    {
        xec_ssa_printer printer;
        printer.print_func( ssa->functions.at( i ) );
    }
}





