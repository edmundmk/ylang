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

    explicit xec_ssa_printer( xec_ssa* root );

    void print( xec_ssa_func* func );


private:

    void print_slice( xec_ssa_func* func, xec_ssa_slice* slice );
    void print_op( xec_ssa_func* func, xec_ssa_op& op );
    void print_names( xec_ssa_func* func, xec_ssa_opref opref );

    xec_ssa* root;

};



xec_ssa_printer::xec_ssa_printer( xec_ssa* root )
    :   root( root )
{
}


void xec_ssa_printer::print( xec_ssa_func* func )
{
    // Build ids (if blocks haven't already been ordered).
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );
        if ( block->index == -1 )
        {
            block->index = (int)i;
        }
    }
    
    
    // Print function.
    printf( "function %p %s\n", func, func->funcname );
    printf( "    block        : [%04X]\n", (int)func->block->index );
    printf( "    upvalcount   : %d\n", func->upvalcount );
    printf( "    localupcount : %d\n", func->localupcount );
    printf( "    paramcount   : %d\n", func->paramcount );
    printf( "    varargs      : %s\n", func->varargs ? "true" : "false" );
    printf( "    coroutine    : %s\n", func->coroutine ? "true" : "false" );
    
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xec_ssa_block* block = func->blocks.at( i );

        printf( "\n" );
        printf( "[%04X]\n", block->index );

        print_slice( func, block->live );
        print_slice( func, block->phi );
        print_slice( func, block->ops );
        
        if ( block->condition )
        {
            printf( "  ?? %04X:%02X\n",
                    block->condition.slice, block->condition.index );
            printf( "    -> [%04X]\n", block->iftrue->index );
            printf( "    -> [%04X]\n", block->iffalse->index );
        }
        else if ( block->next )
        {
            printf( "  -> [%04X]\n", block->next->index );
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

        add( XEC_SSA_NULL,      "null"      );
        add( XEC_SSA_NUMBER,    "number"    );
        add( XEC_SSA_BOOL,      "bool"      );
        add( XEC_SSA_STRING,    "string"    );

        add( XEC_SSA_REF,       "ref"       );
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
        add( XEC_SSA_INKEY,     "inkey"     );
        add( XEC_SSA_INDEX,     "index"     );
        add( XEC_SSA_DELINKEY,  "delinkey"  );
        add( XEC_SSA_OBJECT,    "object"    );
        add( XEC_SSA_ITER,      "iter"      );
        add( XEC_SSA_EACH,      "each"      );
        add( XEC_SSA_APPEND,    "append"    );
        add( XEC_SSA_EXTEND,    "extend"    );
        add( XEC_SSA_CATCH,     "catch"     );
        add( XEC_SSA_RETHROW,   "rethrow"   );

        add( XEC_SSA_KEY,       "key"       );
        add( XEC_SSA_DELKEY,    "delkey"    );
        add( XEC_SSA_GLOBAL,    "global"    );
        add( XEC_SSA_SETGLOBAL, "setglobal" );

        add( XEC_SSA_PARAM,     "param"     );
        add( XEC_SSA_VARARG,    "vararg"    );
        add( XEC_SSA_SELECT,    "select"    );
        add( XEC_SSA_UNPACK,    "unpack"    );
        add( XEC_SSA_NEWUP,     "newup"     );
        add( XEC_SSA_REFUP,     "refup"     );
        add( XEC_SSA_SETUP,     "setup"     );
        add( XEC_SSA_CLOSE,     "close"     );
        add( XEC_SSA_ARRAY,     "array"     );
        add( XEC_SSA_TABLE,     "table"     );
        add( XEC_SSA_NEXT,      "next"      );

        add( XEC_SSA_SETINKEY,  "setinkey"  );
        add( XEC_SSA_SETINDEX,  "setindex"  );
        add( XEC_SSA_SETKEY,    "setkey"    );

        add( XEC_SSA_CALL,      "call"      );
        add( XEC_SSA_YCALL,     "ycall"     );
        add( XEC_SSA_YIELD,     "yield"     );
        add( XEC_SSA_NEW,       "new"       );
        add( XEC_SSA_RETURN,    "return"    );

        add( XEC_SSA_LAMBDA,    "lambda"    );
        
        add( XEC_SSA_PHI,       "phi"       );

        add( XEC_SSA_LIVE,      "live"      );
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



void xec_ssa_printer::print_slice( xec_ssa_func* func, xec_ssa_slice* slice )
{
    for ( size_t i = 0; i < slice->ops.size(); ++i )
    {
        xec_ssa_opref opref;
        opref.slice = slice->index;
        opref.index = (int)i;
    
        printf( "%04X:%02X ", opref.slice, opref.index );
        print_op( func, slice->ops.at( i ) );
        print_names( func, opref );
        printf( "\n" );
    }
}


void xec_ssa_printer::print_op( xec_ssa_func* func, xec_ssa_op& op )
{
    printf( "[" );
    if ( op.opcode >= XEC_SSA_FIRST_SET
                && op.opcode <= XEC_SSA_LAST_SET )
    {
        printf( "----:--|----:--" );
    }
    else
    {
        if ( op.until == XEC_SSA_FOREVER )
        {
            printf( "vvvv:vv" );
        }
        else if ( op.until )
        {
            printf( "%04X:%02X", op.until.slice, op.until.index );
        }
        else
        {
            printf( "----:--" );
        }
        printf( "|" );
        if ( op.lnext )
        {
            printf( "%04X:%02X", op.lnext.slice, op.lnext.index );
        }
        else
        {
            printf( "----:--" );
        }
    }
    printf( "] " );

    const char* opname = opnames.lookup( op.opcode );
    printf( "%-9s", opname );


    if ( ( op.opcode >= XEC_SSA_FIRST_REF
                && op.opcode <= XEC_SSA_LAST_REF )
            || op.opcode == XEC_SSA_LIVE )
    {
        if ( op.operanda )
        {
            printf( " %04X:%02X", op.operanda.slice, op.operanda.index );
        }
        if ( op.operandb )
        {
            printf( " %04X:%02X", op.operandb.slice, op.operandb.index );
        }
    }
    else if ( op.opcode >= XEC_SSA_FIRST_KEY
                && op.opcode <= XEC_SSA_LAST_KEY )
    {
        if ( op.operanda )
        {
            printf( " %04X:%02X", op.operanda.slice, op.operanda.index );
        }
        const char* key = root->keys.at( op.immkey );
        printf( " '%s'", key );
    }
    else if ( op.opcode >= XEC_SSA_FIRST_IMM
                && op.opcode <= XEC_SSA_LAST_IMM )
    {
        if ( op.operanda )
        {
            printf( " %04X:%02X", op.operanda.slice, op.operanda.index );
        }
        printf( " $%d", op.immkey );
    }
    else if ( op.opcode >= XEC_SSA_FIRST_SET
                && op.opcode <= XEC_SSA_LAST_SET )
    {
        printf( " %04X:%02X", op.operanda.slice, op.operanda.index );
        if ( op.opcode == XEC_SSA_SETKEY )
        {
            const char* key = root->keys.at( op.immkey );
            printf( " '%s'", key );
        }
        else
        {
            printf( " %04X:%02X", op.operandb.slice, op.operandb.index );
        }
        printf( " %04X:%02X", op.operandv.slice, op.operandv.index );
    }
    else if ( op.opcode >= XEC_SSA_FIRST_ARG
                && op.opcode <= XEC_SSA_LAST_ARG )
    {
        printf( " $%d", op.args->resultcount );
        for ( size_t i = 0; i < op.args->args.size(); ++i )
        {
            xec_ssa_opref arg = op.args->args.at( i );
            printf( " %04X:%02X", arg.slice, arg.index );
        }
        if ( op.args->unpacked )
        {
            xec_ssa_opref unpacked = op.args->unpacked;
            printf( " %04X:%02X ...", unpacked.slice, unpacked.index );
        }
    }
    else switch ( op.opcode )
    {
    case XEC_SSA_NOP:
    case XEC_SSA_NULL:
        break;
        
    case XEC_SSA_NUMBER:
        printf( " %g", op.number );
        break;
        
    case XEC_SSA_BOOL:
        printf( " %s", op.boolean ? "true" : "false" );
        break;
        
    case XEC_SSA_STRING:
    {
        printf( " \"" );
        for ( size_t i = 0; i < op.string->length; ++i )
        {
            char c = op.string->string[ i ];
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
        break;
    }
    
    case XEC_SSA_PHI:
    {
        for ( size_t i = 0; i < op.phi->definitions.size(); ++i )
        {
            xec_ssa_opref def = op.phi->definitions.at( i );
            printf( " %04X:%02X", def.slice, def.index );
        }
        break;
    }
        
    case XEC_SSA_LAMBDA:
    {
        printf( " %p", op.lambda->function );
        for ( size_t i = 0; i < op.lambda->upvals.size(); ++i )
        {
            xec_ssa_opref upval = op.lambda->upvals.at( i );
            printf( " %04X:%02X", upval.slice, upval.index );
        }
        break;
    }
    
    case XEC_SSA_LIVE:
        break;
    
    default:
        break;
    }

}



void xec_ssa_printer::print_names( xec_ssa_func* func, xec_ssa_opref opref )
{
    auto ii = func->names.equal_range( opref );
    for ( auto i = ii.first; i != ii.second; ++i )
    {
        if ( i == ii.first )
            printf( " ( " );
        else
            printf( ", " );
        printf( "%s", i->second->name );
    }
    if ( ii.first != ii.second )
    {
        printf( " )" );
    }
}





void xec_ssa_print( xec_ssa* ssa )
{
    for ( size_t i = 0; i < ssa->functions.size(); ++i )
    {
        xec_ssa_printer printer( ssa );
        printer.print( ssa->functions.at( i ) );
    }
}





