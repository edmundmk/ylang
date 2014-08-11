//
//  xec_ssa.h
//
//  Created by Edmund Kapusniak on 09/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_H
#define XEC_SSA_H


#include <assert.h>
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
    SSA form for scripts.
    
    Both call and yield operations can result in multiple values, which are
    selected with 'select'.  'vararg' and 'unpack' can either result in a
    single value, or in an unknown number of values.
    
    Exceptions are evil, because they cause all the control-flow to go out of
    the window.  I don't want to express possible exceptions in the normal
    control-flow graph or have the possibility of exceptions affect register
    allocation.  So each block has a link to the catch-finally block that
    should be executed if an exception is thrown in these blocks.
    
    Finally blocks have two possible predecessor blocks - the normal entry,
    or the entry from the exception-handling block.
    
    Instead of ɸ-functions, on entry to the exception-handling block
    Ψ-functions list every possible source for a variable, depending on
    where in the block the exception was thrown.  Ψ-functions only appear in
    exception-handling blocks.  After the exception-handling block, a rethrow
    instruction will rethrow the exception if it was not caught.
 
*/


enum xec_ssa_opcode
{
    // Opcode packings (makes it fast to decode operations)
    XEC_SSA_PACKED      = 0x0000,                   //
    XEC_SSA_PACKED_O    = XEC_SSA_PACKED | 0x0100,  // operand
    XEC_SSA_PACKED_OO   = XEC_SSA_PACKED | 0x0200,  // operand operand
    XEC_SSA_PACKED_L    = XEC_SSA_PACKED | 0x0300,  // literal
    XEC_SSA_PACKED_OL   = XEC_SSA_PACKED | 0x0400,  // operand literal
    XEC_SSA_PACKED_I    = XEC_SSA_PACKED | 0x0500,  // immediate
    XEC_SSA_PACKED_OI   = XEC_SSA_PACKED | 0x0600,  // operand immediate
    XEC_SSA_PACKED_S    = XEC_SSA_PACKED | 0x0700,  // string
    XEC_SSA_PACKED_B    = XEC_SSA_PACKED | 0x0800,  // boolean
    XEC_SSA_PACKED_N    = XEC_SSA_PACKED | 0x0900,  // number
    XEC_SSA_TRIPLE      = 0x1000,                   // operand operand operand
    XEC_SSA_TRIPLE_K    = XEC_SSA_TRIPLE | 0x0100,  // operand key operand
    XEC_SSA_EXPAND      = 0x2000,                   // valcount operands ...
    XEC_SSA_EXPAND_F    = XEC_SSA_EXPAND | 0x0100,  // function operands ...
    
    
    // Opcodes (sorted by packing type, sorry).
    
    XEC_SSA_NOP         // no-op
            = XEC_SSA_PACKED,
    XEC_SSA_NULL,       // null value
    XEC_SSA_CATCH,      // catch currently unwinding exception
    XEC_SSA_RETHROW,    // rethrow currently unwinding exception
    
    XEC_SSA_MOV         // move
            = XEC_SSA_PACKED_O,
    XEC_SSA_ITER,       // make iterator for a list
    XEC_SSA_EACH,       // make iterator for object keys
    XEC_SSA_POS,        // unary +
    XEC_SSA_NEG,        // unary -
    XEC_SSA_NOT,        // unary !
    XEC_SSA_BITNOT,     // unary ~
    XEC_SSA_OBJECT,     // create new object, inheriting from a prototype
    
    XEC_SSA_INKEY       // object.[ key ]
            = XEC_SSA_PACKED_OO,
    XEC_SSA_INDEX,      // container[ index ]
    XEC_SSA_DELINKEY,   // delete object.[ key ]
    XEC_SSA_MUL,        // *
    XEC_SSA_DIV,        // /
    XEC_SSA_MOD,        // %
    XEC_SSA_INTDIV,     // ~
    XEC_SSA_ADD,        // +
    XEC_SSA_SUB,        // -
    XEC_SSA_LSL,        // <<
    XEC_SSA_LSR,        // >>
    XEC_SSA_ASR,        // ~>>
    XEC_SSA_BITAND,     // &
    XEC_SSA_BITXOR,     // ^
    XEC_SSA_BITOR,      // |
    XEC_SSA_CONCAT,     // ..
    XEC_SSA_EQ,         // ==
    XEC_SSA_LT,         // <
    XEC_SSA_LE,         // <=
    XEC_SSA_IN,         // in
    XEC_SSA_IS,         // is
    XEC_SSA_XOR,        // ^^
    XEC_SSA_APPEND,     // append to an array
    
    XEC_SSA_GLOBAL      // global.key
             = XEC_SSA_PACKED_L,
    
    XEC_SSA_KEY         // object.key
             = XEC_SSA_PACKED_OL,
    XEC_SSA_DELKEY,     // delete object.key
    XEC_SSA_SETGLOBAL,  // global.key = value
    
    XEC_SSA_PARAM       // parameter number
            = XEC_SSA_PACKED_I,
    XEC_SSA_VARARG,     // vararg number, or unpack all varargs
    XEC_SSA_REFUP,      // value of upval
    XEC_SSA_CLOSE,      // close upval
    XEC_SSA_ARRAY,      // new array
    XEC_SSA_TABLE,      // new table
    
    XEC_SSA_SELECT      // select a function call result
             = XEC_SSA_PACKED_OI,
    XEC_SSA_UNPACK,     // select array element, or unpack all elements
    XEC_SSA_NEXT,       // produce values from an iterator
    XEC_SSA_NEWUP,      // create new upval, initialized to value
    XEC_SSA_SETUP,      // set upval
    
    XEC_SSA_STRING      // string value
             = XEC_SSA_PACKED_S,
    
    XEC_SSA_BOOL        // boolean value
            = XEC_SSA_PACKED_B,
    
    XEC_SSA_NUMBER      // number value
            = XEC_SSA_PACKED_N,
    
    XEC_SSA_SETINKEY    // object.[ key ] = value
            = XEC_SSA_TRIPLE,
    XEC_SSA_SETINDEX,   // container[ index ] = value
    
    XEC_SSA_SETKEY      // object.key = value
            = XEC_SSA_TRIPLE_K,
    
    XEC_SSA_PHI         // SSA ɸ-functions.
            = XEC_SSA_EXPAND,
    XEC_SSA_PSI,        // Ψ-functions locate values used in catch blocks.
    XEC_SSA_CALL,       // function call
    XEC_SSA_YCALL,      // yield call
    XEC_SSA_YIELD,      // yield
    XEC_SSA_NEW,        // new
    XEC_SSA_EXTEND,     // extend array with multiple values
    XEC_SSA_RETURN,     // return possibly multiple values
    
    XEC_SSA_CLOSURE     // create function closure
            = XEC_SSA_EXPAND_F,
};


unsigned int xec_ssa_decode( xec_ssa_opcode opcode );
unsigned int xec_ssa_optype( xec_ssa_opcode opcode );




typedef std::deque< xec_ssa_func*,
    region_allocator< xec_ssa_func* > > xec_ssa_func_list;
typedef std::deque< xec_ssa_block*,
    region_allocator< xec_ssa_block* > > xec_ssa_block_list;
typedef std::deque< xec_ssa_node*,
    region_allocator< xec_ssa_node* > > xec_ssa_node_list;
typedef std::unordered_multimap
        <
            xec_ssa_node*,
            xec_ssa_name*,
            std::hash< xec_ssa_node* >,
            std::equal_to< xec_ssa_node* >,
            region_allocator< std::pair< xec_ssa_node* const, xec_ssa_name* > >
        >
        xec_ssa_name_map;


struct xec_ssa
{
    xec_ssa();
    ~xec_ssa();

    region              alloc;
    xec_script*         script;
    xec_ssa_func*       function;
    std::deque< xec_ssa_func* > functions;
};


struct xec_ssa_func
{
    xec_ssa_func( int sloc, const char* funcname, xec_ssa_block* block,
            int upvalcount, int paramcount, bool varargs, bool coroutine );

    int                 sloc;
    const char*         funcname;
    xec_ssa_block*      block;
    xec_ssa_block_list  blocks;
    int                 upvalcount;
    int                 localupcount;
    int                 paramcount;
    bool                varargs;
    bool                coroutine;
};


struct xec_ssa_name
{
    xec_ssa_name( int sloc, const char* name );

    int                 sloc;
    const char*         name;
};



struct xec_ssa_block
{
    xec_ssa_block();

    xec_ssa_block_list  previous;
    xec_ssa_node_list   phi;
    xec_ssa_node_list   nodes;
    xec_ssa_name_map    names; // TODO: what if a node has multiple names?
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
                const char* literal );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* operand, const char* literal );
    xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
                int immediate );
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





inline unsigned int xec_ssa_decode( xec_ssa_opcode opcode )
{
    return opcode & 0xFF00;
}

inline unsigned int xec_ssa_optype( xec_ssa_opcode opcode )
{
    return opcode & 0xF000;
}


inline xec_ssa_packed* xec_ssa_node::as_packed()
{
    assert( xec_ssa_optype( opcode ) == XEC_SSA_PACKED );
    return (xec_ssa_packed*)this;
}

inline xec_ssa_triple* xec_ssa_node::as_triple()
{
    assert( xec_ssa_optype( opcode ) == XEC_SSA_TRIPLE );
    return (xec_ssa_triple*)this;
}

inline xec_ssa_expand* xec_ssa_node::as_expand()
{
    assert( xec_ssa_optype( opcode ) == XEC_SSA_EXPAND );
    return (xec_ssa_expand*)this;
}




#endif



