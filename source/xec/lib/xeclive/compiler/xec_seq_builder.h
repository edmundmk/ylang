//
//  xec_seq_builder.h
//
//  Created by Edmund Kapusniak on 03/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SEQ_BUILDER_H
#define XEC_SEQ_BUILDER_H


/*
    Builder that uses visitors to build seq-form of expression.
*/


#include "xec_seq.h"
#include "xec_seq_astvisitors.h"



struct xec_seq_lvalue
{
    xec_seq_lvalue();

    int             sloc;
    xec_seq_opcode  opcode;
    union
    {
    xec_ast_name*   name;
    xec_seq_op*     a;
    };
    union
    {
    const char*     k;
    xec_seq_op*     b;
    };
};



struct xec_seq_valist
{
    xec_seq_valist();

    std::vector< xec_seq_op* > values;
    xec_seq_op*      unpacked;
};



class xec_seq_builder
{
public:

    explicit xec_seq_builder( xec_seq* seq );
    
    xec_seq_op*     build( xec_ast_node* expr );
    
    template < typename ... arguments_t >
    xec_seq_op*     op( arguments_t ... arguments );
    
    xec_seq_args*   args( int rcount );
    void            reserve( xec_seq_args* a, int acount, xec_seq_valist* l );
    void            push( xec_seq_args* a, xec_seq_op* op );
    void            push( xec_seq_args* a, xec_seq_valist* l );

    xec_ast_upval*  upval( xec_expr_upref* upref );
    
    xec_seq_op*     expr( xec_ast_node* expr );
    void            unpack( xec_seq_valist* l, xec_ast_node* node, int count );

    void            declare( xec_seq_lvalue* lvalue, xec_ast_node* node );
    void            lvalue( xec_seq_lvalue* lvalue, xec_ast_node* node );
    xec_seq_op*     lvalue_value( xec_seq_lvalue* lvalue );
    void            lvalue_assign( xec_seq_lvalue* lvalue, xec_seq_op* v );


private:

    xec_seq* seq;
    xec_seq_build_expr      build_expr;
    xec_seq_build_unpack    build_unpack;

};


template < typename ... arguments_t >
inline xec_seq_op* xec_seq_builder::op( arguments_t ... arguments )
{
    xec_seq_op* op = new ( seq->alloc ) xec_seq_op( arguments ... );
    seq->ops.push_back( op );
    return op;
}

inline xec_seq_args* xec_seq_builder::args( int rcount )
{
    return new ( seq->alloc ) xec_seq_args( rcount );
}




#endif
