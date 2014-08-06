//
//  xec_ssa.h
//
//  Created by Edmund Kapusniak on 09/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_H
#define XEC_SSA_H


#include <memory>
#include <unordered_map>
#include <string>
#include <deque>
#include <region.h>


class xec_script;
struct xec_ssa_func;
struct xec_ssa_name;
struct xec_ssa_block;
struct xec_ssa_node;
struct xec_ssa_packed;
struct xec_ssa_triple;
struct xec_ssa_expand;



/*
    SSA form for scripts,.
*/


enum xec_ssa_opcode
{
    XEC_SSA_NOP,

    XEC_SSA_PHI,        // SSA ɸ-functions.

    XEC_SSA_CLOSURE,

    XEC_SSA_CALL,
    XEC_SSA_YIELD,
    
    XEC_SSA_SELECT,     // Select one of the results of a function call.
    XEC_SSA_VARARG,     // One of varargs, or unpack all varargs.
    XEC_SSA_UNPACK,     // An array element selected by immediate, or unpack.
    
    XEC_SSA_NULL,
    XEC_SSA_BOOL,
    XEC_SSA_NUMBER,
    XEC_SSA_STRING,
    
    XEC_SSA_PARAM,
    XEC_SSA_UPREF,
    XEC_SSA_GLOBAL,

    XEC_SSA_KEY,
    XEC_SSA_INKEY,
    XEC_SSA_INDEX,
    
    XEC_SSA_SETKEY,
    XEC_SSA_SETINKEY,
    XEC_SSA_SETINDEX,
    
    XEC_SSA_SETUPVAL,
    XEC_SSA_SETGLOBAL,
    
    XEC_SSA_POS,
    XEC_SSA_NEG,
    XEC_SSA_NOT,
    XEC_SSA_BITNOT,
    XEC_SSA_MUL,
    XEC_SSA_DIV,
    XEC_SSA_MOD,
    XEC_SSA_INTDIV,
    XEC_SSA_ADD,
    XEC_SSA_SUB,
    XEC_SSA_LSL,
    XEC_SSA_LSR,
    XEC_SSA_ASR,
    XEC_SSA_BITAND,
    XEC_SSA_BITXOR,
    XEC_SSA_BITOR,
    XEC_SSA_CONCAT,
    
    XEC_SSA_EQ,
    XEC_SSA_LT,
    XEC_SSA_LE,
    XEC_SSA_IN,
    XEC_SSA_IS,
    
    XEC_SSA_XOR,
    
    XEC_SSA_NEW,
    XEC_SSA_OBJECT,
    XEC_SSA_TABLE,
    XEC_SSA_ARRAY,
    
    XEC_SSA_APPEND,     // Append to an array.
    XEC_SSA_EXTEND,     // Append unpacked values to an array.

};


typedef std::deque< xec_ssa_func*,
    region_allocator< xec_ssa_func* > > xec_ssa_func_list;
typedef std::deque< xec_ssa_node*,
    region_allocator< xec_ssa_node* > > xec_ssa_node_list;



struct xec_ssa
{
    region              alloc;
    xec_script*         script;
    xec_ssa_func*       function;
    std::deque< xec_ssa_func* > functions;
};


struct xec_ssa_func
{
    int                 sloc;
    const char*         funcname;
    xec_ssa_block*      block;
    int                 upvalcount;
    int                 paramcount;
    bool                varargs;
    bool                coroutine;
};


struct xec_ssa_name
{
    int                 sloc;
    const char*         name;
};



struct xec_ssa_block
{
    xec_ssa_block*      prev;
    xec_ssa_node*       node;
    xec_ssa_node*       condition;
    union
    {
    xec_ssa_block*      next;
        struct
        {
    xec_ssa_block*      iftrue;
    xec_ssa_block*      iffalse;
        };
    };
};


struct xec_ssa_node
{
    xec_ssa_node( int sloc, xec_ssa_opcode opcode );

    int                 sloc;
    xec_ssa_opcode      opcode;
    xec_ssa_node*       next;
    
    size_t              operand_count();
    xec_ssa_node*       operand( size_t index );

    xec_ssa_packed*     as_packed();
    xec_ssa_triple*     as_triple();
    xec_ssa_expand*     as_expand();
};


struct xec_ssa_packed : public xec_ssa_node
{
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* operand );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* operanda, xec_ssa_node* operandb );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* operand, const char* literal );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* operand, int immediate );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                const char* string, size_t length );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode, bool boolean );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode, double number );
    

    union
    {
        struct
        {
    xec_ssa_node*       operanda;
            union
            {
    xec_ssa_node*       operandb;
    const char*         literal;
    int                 immediate;
            };
        };
        
        struct
        {
    const char*         string;
    size_t              length;
        };

    bool                boolean;
    double              number;
    };
};



struct xec_ssa_triple : public xec_ssa_node
{
    xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, xec_ssa_node* index, xec_ssa_node* v );
    xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, const char* key, xec_ssa_node* v );

    xec_ssa_node*       object;
    union
    {
    const char*         key;
    xec_ssa_node*       index;
    };
    xec_ssa_node*       value;
};




struct xec_ssa_expand : public xec_ssa_node
{
    xec_ssa_expand( int sloc, xec_ssa_opcode opcode, int valcount );
    xec_ssa_expand( int sloc, xec_ssa_opcode opcode, xec_ssa_func* func );

    xec_ssa_node_list   operands;
    union
    {
        struct
        {
    xec_ssa_node*       unpacked;
    int                 valcount;
        };
    xec_ssa_func*       func;
    };
};




#endif



