//
//  xec_seq.h
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SEQ_H
#define XEC_SEQ_H


/*
    We want to compile xec to other dynamic languages such as JavaScript or
    Lua.  However the semantics of expressions in xec - while similar - do
    not map exactly to either of these languages.

    Lua does not support nested assignments or pre/post-operations.  The
    ordering of these operations with respect to other parts of the expression
    must be correctly preserved.  Also Lua has no conditional operator.
    
    JavaScript does not support multiple assignment, or returning multiple
    values from a function.
    
    Therefore to compile a xec expression it is first transformed to this
    intermediate representation, which has the following properties:
    
        -   The ordering of operations (including calls and assignments)
            is made explicit.
            
        -   Values which are operands to more than one operation (which
            occurs in xec pre/post increments and the central terms in
            comparison sequences) are identified.  They must only appear
            once in the compiled expression.
            
        -   Multiple-valued expressions are split by selecting the
            appropriate result as the rvalue of an assignment, or they
            are consumed immediately as the last 'unpack' argument to
            a function call.
            
        -   Control flow (for &&, ||, ?:, and chained comparisons) is made
            explicit.
       
 
    The seq form is similar to the SSA form used when compiling xec to
    bytecode, but since it expresses only a single expression and does not
    need to undergo any transformations it can be much simpler.
 
*/


#include <vector>
#include <region.h>


struct xec_ast_node;
struct xec_ast_name;

struct xec_seq_op;
struct xec_seq_args;


typedef std::vector< xec_seq_op*,
                region_allocator< xec_seq_op* > > xec_seq_list;


enum xec_seq_opcode
{
    XEC_SEQ_NOP,

    // References to AST nodes.
    XEC_SEQ_LITERAL,        // null, bool, number, string
    XEC_SEQ_ASTNODE,        // func, new object, new array, new table
    
    // Reference to an object under construction (may be an upval).
    XEC_SEQ_OBJREF,

    // Literals.
    XEC_SEQ_NULL,           // null
    XEC_SEQ_ONE,            // 1.0

    // Unary operators.
    XEC_SEQ_POS,
    XEC_SEQ_NEG,
    XEC_SEQ_NOT,
    XEC_SEQ_BITNOT,
    
    // Binary operators.
    XEC_SEQ_MUL,
    XEC_SEQ_DIV,
    XEC_SEQ_MOD,
    XEC_SEQ_INTDIV,
    XEC_SEQ_ADD,
    XEC_SEQ_SUB,
    XEC_SEQ_LSL,
    XEC_SEQ_LSR,
    XEC_SEQ_ASR,
    XEC_SEQ_BITAND,
    XEC_SEQ_BITXOR,
    XEC_SEQ_BITOR,
    XEC_SEQ_CONCAT,
    XEC_SEQ_EQ,
    XEC_SEQ_NE,
    XEC_SEQ_LT,
    XEC_SEQ_LE,
    XEC_SEQ_GT,
    XEC_SEQ_GE,
    XEC_SEQ_IN,
    XEC_SEQ_NOTIN,
    XEC_SEQ_IS,
    XEC_SEQ_NOTIS,
    XEC_SEQ_XOR,
    
    // Lookups.
    XEC_SEQ_VAR,
    XEC_SEQ_GLOBAL,
    XEC_SEQ_KEY,
    XEC_SEQ_INKEY,
    XEC_SEQ_INDEX,
    
    // Assignments.
    XEC_SEQ_DECLARE,
    XEC_SEQ_SETVAR,
    XEC_SEQ_SETGLOBAL,
    XEC_SEQ_SETKEY,
    XEC_SEQ_SETINKEY,
    XEC_SEQ_SETINDEX,

    // Control flow.
    XEC_SEQ_BEGINAND,       // if ! a then a else ...
    XEC_SEQ_BEGINOR,        // if a then a else ...
    XEC_SEQ_BEGINIF,        // if a then ...
    XEC_SEQ_ELSE,           // result of block is a, else ...
    XEC_SEQ_END,            // result of block is a

    // Calls and other multi-value operations.
    XEC_SEQ_NEW,
    XEC_SEQ_CALL,
    XEC_SEQ_YCALL,
    XEC_SEQ_YIELD,
    XEC_SEQ_UNPACK,
    XEC_SEQ_VARALL,
    
    // Select one of the multiple-valued results.
    XEC_SEQ_SELECT,
    XEC_SEQ_ELEMENT,
    XEC_SEQ_VARARG,

};


struct xec_seq_op
{
    xec_seq_op( int sloc, xec_seq_opcode opcode );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_ast_node* astnode );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_ast_name* name );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_args* args );
    xec_seq_op( int sloc, xec_seq_opcode opcode, const char* k );
    xec_seq_op( int sloc, xec_seq_opcode opcode, int i );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a, const char* k );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a, int i );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a, xec_seq_op* b );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_ast_name* name, xec_seq_op* v );
    xec_seq_op( int sloc, xec_seq_opcode opcode, const char* k, xec_seq_op* v );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a, const char* k, xec_seq_op* v );
    xec_seq_op( int sloc, xec_seq_opcode opcode, xec_seq_op* a, xec_seq_op* b, xec_seq_op* v );

    int             sloc;
    xec_seq_opcode  opcode      : 8;
    int             refcount    : 24;
    
    union
    {
    xec_ast_node*   astnode;
    xec_ast_name*   name;
    xec_seq_args*   args;
    xec_seq_op*     a;
    };
    
    union
    {
    const char*     k;
    int             i;
    xec_seq_op*     b;
    };

    xec_seq_op*     v;
};


struct xec_seq_args
{
    explicit xec_seq_args( int rcount );

    int             rcount;
    xec_seq_list    values;
    xec_seq_op*     unpacked;
};


struct xec_seq
{
    region alloc;
    std::vector< xec_seq_op* > ops;
};




void xec_seq_print( xec_seq* seq );



#endif
