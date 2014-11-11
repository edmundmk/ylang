//
//  xssa.h
//
//  Created by Edmund Kapusniak on 11/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_H
#define XSSA_H


#include <memory>
#include <vector>
#include <unordered_map>
#include <region.h>


class xec_script;



/*
    The AST is converted to this SSA form, allowing us to run a real register
    allocation algorithm for VM registers, and potentially perform other
    optimizations.  Although the language is dynamic, it is still amenable to
    static analysis.
    
    
    Multivals are used to represent ops that produce an unknown number of
    arguments.  Only a single multival can be live in the program at any
    point, and it must be consumed immediately.  Opcodes that can produce
    multivals:
    
        varall      all varargs
        unpack      all elements of an array
        call        all results of a function call
        ycall       all results of a function call (with yield semantics)
        yield       all arguments passed into a coroutine
    
    Opcodes that can consume multivals (through the special operand):
    
        extend      extends an array with multiple values
        call        final members of argument list
        ycall       final members of argument list
        yield       final values to yield
        return      final values to return
        
    Some opcodes can produce more than one result (but the number of results
    is fixed at compile time).  To use one of the results, insert a 'select'
    op which identifies the result to use (numbered from 0).  The following
    ops can produce more than one result:
    
        next        next set of results from iterator
        call        first n results of function call
        ycall       first n results of function call (with yield semantics)
        yield       firs tn arguments passed into coroutine
 
    The 'next' opcode both produces more than one result and can also be
    referenced directly as the control flow condition, to detect iterator
    completion.
 
 
*/


struct xssaop;
struct xssa_string;
struct xssa_module;
struct xssa_func;
struct xssa_block;


typedef std::unique_ptr< xssa_func >    xssa_func_p;
typedef std::unique_ptr< xssa_block >   xssa_block_p;



enum xssa_opcode : uint8_t
{
    XSSA_NOP,       // no-op
    
    // constants
    XSSA_NULL,      // null value
    XSSA_NUMBER,    // number
    XSSA_BOOL,      // boolean
    XSSA_STRING,    // string

    // unary operations
    XSSA_POS,       // unary +
    XSSA_NEG,       // unary -
    XSSA_NOT,       // unary ~
    XSSA_LNOT,      // unary !

    // binary operations
    XSSA_MUL,       // *
    XSSA_DIV,       // /
    XSSA_MOD,       // %
    XSSA_INTDIV,    // ~
    XSSA_ADD,       // +
    XSSA_SUB,       // -
    XSSA_LSL,       // <<
    XSSA_LSR,       // >>
    XSSA_ASR,       // ~>>
    XSSA_AND,       // &
    XSSA_XOR,       // ^
    XSSA_OR,        // |
    XSSA_CONCAT,    // ..
    
    // comparisons
    XSSA_EQ,        // ==
    XSSA_LT,        // <
    XSSA_LE,        // <=
    XSSA_IN,        // in
    XSSA_IS,        // is

    // logical operations
    XSSA_LXOR,      // ^^
    
    // indexing
    XSSA_KEY,       // object.key
    XSSA_INKEY,     // object.[ key ]
    XSSA_INDEX,     // container[ index ]
    XSSA_GLOBAL,    // global.key

    // assignment
    XSSA_SETKEY,    // object.key = value
    XSSA_SETINKEY,  // object.[ key ] = value
    XSSA_SETINDEX,  // container[ index ] = value
    XSSA_SETGLOBAL, // global.key = value
    
    // key deletion
    XSSA_DELKEY,    // delete object.key
    XSSA_DELINKEY,  // delete object.[ key ]
    
    // upvals (loads/stores are represented explicitly)
    XSSA_NEWUP,     // upvals[ index ] = new upval( value )
    XSSA_SETUP,     // upvals[ index ].set( value )
    XSSA_REFUP,     // upvals[ index ].get()
    XSSA_CLOUP,     // upvals[ index ] = null
    
    // allocation
    XSSA_OBJECT,    // create object with prototype
    XSSA_TABLE,     // create table (with space for n elements)
    XSSA_ARRAY,     // create array (with space for n elements)

    // array operations.
    XSSA_APPEND,    // append to an array
    XSSA_EXTEND,    // extend an array (consumes multival)
    
    // closures
    XSSA_CLOSURE,   // create a function closure (closing over upvals)

    // multival
    XSSA_VARALL,    // all varargs
    XSSA_UNPACK,    // all elements of an array
    XSSA_CALL,      // function call (can also consume multival)
    XSSA_YCALL,     // yield call (can also consume multival)
    XSSA_YIELD,     // yield (can also conume multival)
    
    // iterators
    XSSA_ITER,      // push iterator over elements of container
    XSSA_ITERKEY,   // push iterator over keys of object
    XSSA_POPITER,   // pop iterator
    XSSA_NEXT,      // produces exactly n values, also used as branch flag
    
    // flow control
    XSSA_RETURN,    // return from procedure (can conume multival)
    
    // selection
    XSSA_PARAM,     // nth parameter (numbered from 0).
    XSSA_VARARG,    // nth variable argument (numbered from 0).
    XSSA_SELECT,    // nth result of a multival op.
    XSSA_ELEM,      // nth element of an array.

    // SSA-form
    XSSA_PHI,       // SSA ɸ-function.
    XSSA_REF,       // A reference used when building SSA.
    XSSA_PSI,       // An 'incomplete' ɸ-function used when building SSA.
    
};




/*
    SSA ops and strings are allocated from the current region.
*/


struct xssaop
{
    static bool has_multival( xssa_opcode opcode );


    static xssaop* op( int sloc, xssa_opcode opcode, size_t operand_count );
    static xssaop* op( int sloc, xssa_opcode opcode );
    static xssaop* op( int sloc, xssa_opcode opcode, xssaop* operand );
    static xssaop* op( int sloc, xssa_opcode opcode, xssaop* operanda, xssaop* operandb );
    static xssaop* op( int sloc, xssa_opcode opcode, xssaop* operanda, xssaop* operandb, xssaop* operandv );
    

    int             sloc;
    xssa_opcode     opcode;
    int8_t          operand_count;
    int8_t          result_count;
    uint8_t         r;
    
    union
    {
    double          number;
    bool            boolean;
    xssa_string*    string;
    xssaop*         multival;
    const char*     key;
    int             index;
    xssa_func*      func;
    };

    xssaop*         operands[];


private:
    static xssaop* op( int sloc, xssa_opcode opcode, int immediate );

};





struct xssa_string
{
    static xssa_string* s( int sloc, const char* string );
    static xssa_string* s( int sloc, size_t length, const char* string );

    int             sloc;
    size_t          length;
    const char      string[];
};







/*
    Control-flow graph representation of scripts in SSA form.
*/


struct xssa_module
{
    region                      alloc;
    xec_script*                 script;
    std::vector< xssa_func_p >  funcs;
};


struct xssa_func
{
    int                         sloc;
    const char*                 funcname;
    std::unordered_multimap< xssaop*, xssa_string* > names;
    std::vector< xssa_block_p > blocks;
    int                         upvalcount;
    int                         newupcount;
    int                         paramcount;
    bool                        varargs;
    bool                        coroutine;
};


struct xssa_block
{
    int                         index;
    std::vector< xssa_block* >  previous;
    std::vector< xssaop* >      phi;
    std::vector< xssaop* >      ops;
    xssaop*                     condition;
    union
    {
    xssa_block*                 next;
        struct
        {
    xssa_block*                 iftrue;
    xssa_block*                 iffalse;
        };
    };
};










#endif
