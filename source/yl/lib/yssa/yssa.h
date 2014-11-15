//
//  yssa.h
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


class yl_script;



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


struct yssaop;
struct yssa_string;
struct yssa_module;
struct yssa_func;
struct yssa_block;
struct yssa_loop;


typedef std::unique_ptr< yssa_func >        yssa_func_p;
typedef std::unique_ptr< yssa_block >       yssa_block_p;
typedef std::unique_ptr< yssa_loop >        yssa_loop_p;
typedef std::unordered_set< yssaop* >       yssa_opset;
typedef std::unordered_set< yssa_loop* >    yssa_loopset;
typedef std::unordered_set< yssa_block* >   yssa_blockset;


enum yssa_opcode : uint16_t
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


struct yssaop
{
    static bool has_multival( yssa_opcode opcode );
    static bool has_key( yssa_opcode opcode );
    static bool has_immed( yssa_opcode opcode );


    static yssaop* op( int sloc, yssa_opcode opcode, size_t operand_count );
    static yssaop* op( int sloc, yssa_opcode opcode );
    static yssaop* op( int sloc, yssa_opcode opcode, yssaop* operand );
    static yssaop* op( int sloc, yssa_opcode opcode, yssaop* operanda, yssaop* operandb );
    static yssaop* op( int sloc, yssa_opcode opcode, yssaop* operanda, yssaop* operandb, yssaop* operandv );
    

    int             index;
    int             sloc;
    
    yssa_opcode     opcode;
    int16_t         operand_count;
    int16_t         result_count;
    uint8_t         r;
    uint8_t         stacktop;
    
    union
    {
    double          number;
    bool            boolean;
    yssa_string*    string;
    yssaop*         multival;
    const char*     key;
    int             immed;
    yssa_func*      func;
    };

    yssaop*         operand[];


};





struct yssa_string
{
    static yssa_string* s( int sloc, const char* string );
    static yssa_string* s( int sloc, size_t length, const char* string );

    int             sloc;
    size_t          length;
    const char      string[];
};







/*
    Control-flow graph representation of scripts in SSA form.
*/


struct yssa_module
{
    region                      alloc;
    yl_script*                 script;
    std::vector< yssa_func_p >  funcs;
};


struct yssa_func
{
    yssa_module*                module;
    int                         sloc;
    const char*                 funcname;
    std::unordered_multimap< yssaop*, yssa_string* > names;
    std::vector< yssa_block_p > blocks;
    std::vector< yssa_loop_p >  loops;
    int                         upvalcount;
    int                         newupcount;
    int                         paramcount;
    bool                        varargs;
    bool                        coroutine;
};


struct yssa_block
{
    int                         index;
    yssa_loop*                  loop;
    yssa_opset                  live_in;
    yssa_opset                  live_out;
    std::vector< yssa_block* >  previous;
    std::vector< yssaop* >      phi;
    std::vector< yssaop* >      ops;
    yssaop*                     condition;
    union
    {
    yssa_block*                 next;
        struct
        {
    yssa_block*                 iftrue;
    yssa_block*                 iffalse;
        };
    };
};


struct yssa_loop
{
    yssa_loop*                  parent;
    yssa_loopset                children;
    yssa_block*                 header;
    yssa_blockset               blocks;
};




/*
    Print SSA form for debugging.
*/

const char* yssa_opname( yssa_opcode opcode );
void yssa_print( yssa_module* module );
void yssa_print( yssa_func* func );
void yssa_print( yssa_func* func, yssaop* op );




/*

*/


inline bool yssaop::has_multival( yssa_opcode opcode )
{
    return opcode == XSSA_EXTEND
        || opcode == XSSA_CALL
        || opcode == XSSA_YCALL
        || opcode == XSSA_YIELD
        || opcode == XSSA_RETURN;
}

inline bool yssaop::has_key( yssa_opcode opcode )
{
    return opcode == XSSA_KEY
        || opcode == XSSA_GLOBAL
        || opcode == XSSA_GLOBAL
        || opcode == XSSA_SETKEY
        || opcode == XSSA_SETGLOBAL
        || opcode == XSSA_DELKEY;
}

inline bool yssaop::has_immed( yssa_opcode opcode )
{
    return opcode == XSSA_NEWUP
        || opcode == XSSA_SETUP
        || opcode == XSSA_REFUP
        || opcode == XSSA_CLOUP
        || opcode == XSSA_TABLE
        || opcode == XSSA_ARRAY
        || opcode == XSSA_PARAM
        || opcode == XSSA_VARARG
        || opcode == XSSA_SELECT
        || opcode == XSSA_ELEM;
}


inline yssaop* yssaop::op( int sloc, yssa_opcode opcode, size_t operand_count )
{
    size_t size = sizeof( yssaop ) + sizeof( yssaop* ) * operand_count;
    yssaop* o = (yssaop*)region_current->malloc( size );
    o->index            = -1;
    o->sloc             = sloc;
    o->opcode           = opcode;
    o->operand_count    = operand_count;
    o->result_count     = 1;
    o->r                = -1;
    o->stacktop         = -1;
    o->number           = 0.0;
    return o;
}

inline yssaop* yssaop::op( int sloc, yssa_opcode opcode )
{
    return op( sloc, opcode, (size_t)0 );
}

inline yssaop* yssaop::op( int sloc, yssa_opcode opcode, yssaop* operand )
{
    yssaop* o = op( sloc, opcode, 1 );
    o->operand[ 0 ] = operand;
    return o;
}

inline yssaop* yssaop::op( int sloc, yssa_opcode opcode, yssaop* operanda, yssaop* operandb )
{
    yssaop* o = op( sloc, opcode, 2 );
    o->operand[ 0 ] = operanda;
    o->operand[ 1 ] = operandb;
    return o;
}

inline yssaop* yssaop::op( int sloc, yssa_opcode opcode, yssaop* operanda, yssaop* operandb, yssaop* operandv )
{
    yssaop* o = op( sloc, opcode, 3 );
    o->operand[ 0 ] = operanda;
    o->operand[ 1 ] = operandb;
    o->operand[ 2 ] = operandv;
    return o;
}



inline yssa_string* yssa_string::s( int sloc, const char* string )
{
    return s( sloc, strlen( string ), string );
}

inline yssa_string* yssa_string::s( int sloc, size_t length, const char* string )
{
    size_t size = sizeof( yssa_string ) + length + 1;
    yssa_string* s = (yssa_string*)region_current->malloc( size );
    s->sloc = sloc;
    s->length = length;
    memcpy( (char*)s->string, string, length );
    ( (char*)s->string )[ length ] = '\0';
    return s;
}





#endif
