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
       
 
    Translating out of seq-form produces a sequence of calls, assignments,
    and declarations, which capture the semantics of the original xec
    expression.  To perform this translation, conduct a linear scan of the
    sequence ops, doing a simple liveness analysis/alias analysis:
    
        -   Any op with a single ref is live.  Don't actually write out code
            to evaluate it until it is used in a call or assign, or it needs
            to be assigned to a temporary.
        
        -   Any op with more than one ref must be assigned to a temporary,
            to avoid evaluating it more than once.
            
        -   Before assign and call ops (NOT declarations, as declarations
            cannot clobber any values), any ops which are live (and not dead
            at the assign/call) may need to be assigned to temporaries.  We
            can avoid this if we can prove that the assignment does not
            clobber any value reachable from the live op.  The simplest
            solution is to always assign to a temporary.
 
        -   If the target language supports multiple-assignment, sequences
            of assignments may be merged into a single expression (and no need
            to generate temporaries for ops that die between the first and
            last assignment in the sequence, as long as the semantics of
            multiple assignment means that all assignments happen after all
            rvalues are evaluated, which is the case in Lua).
            
        -   Calls can be inlined into an expression (including when used as
            call arguments) as long as there are no assignments or temporaries
            inbetween the call and the op where the expression is realised.
            Therefore assignments/temporaries 'flush' pending calls, in order.
 
    You can see why compiler people like functional languages which have no
    assignments and no side-effects!
 
    &&, || and ?: introduce control flow, but the semantics of the xec
    operations are the same as both Lua and Javascript.  Lua lacks a ?:
    construction, so it compiles to an if statement assigning into a temporary.
 
    The seq form is similar to the SSA form used when compiling xec to
    bytecode, but since it expresses only a single expression and does not
    need to undergo any transformations it is much simpler.
 
*/


#include <vector>
#include <unordered_map>
#include <region.h>


struct xec_ast_node;



enum xec_seqop_kind
{
    XEC_SEQ_EVAL,
    XEC_SEQ_ASSIGN,
    XEC_SEQ_DECLARE,
};


struct xec_seqop
{
    xec_seqop( xec_seqop_kind kind, xec_ast_node* node );
    xec_seqop( xec_seqop_kind kind, xec_ast_node* lval, xec_ast_node* rval );

    xec_seqop_kind  kind;
    int             rcount;
    xec_ast_node*   lvalue;
    xec_ast_node*   rvalue;
};



struct xec_seq
{
    region alloc;
    std::vector< xec_seqop > ops;
    std::unordered_map< xec_ast_node*, size_t > nodemap;
};



void xec_seq_build( xec_seq* seq, xec_ast_node* expr );



void xec_seq_print( xec_seq* seq );



#endif
