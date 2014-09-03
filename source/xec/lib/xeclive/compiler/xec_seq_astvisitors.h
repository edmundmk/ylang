//
//  xec_seq_astvisitors.h
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SEQ_ASTVISITOR_H
#define XEC_SEQ_ASTVISITOR_H


#include "xec_ast_visitor.h"


struct xec_seq;
struct xec_seq_op;
struct xec_seq_valist;
class xec_seq_builder;



/*
    Visit AST expression nodes and generate a single value from them.
*/

class xec_seq_build_expr
    :   public xec_ast_visitor< xec_seq_build_expr, xec_seq_op* >
{
public:

    explicit xec_seq_build_expr( xec_seq_builder* b );

    using xec_ast_visitor< xec_seq_build_expr, xec_seq_op* >::visit;

    xec_seq_op* fallback( xec_ast_node* node );
    
    xec_seq_op* visit( xec_ast_func* node );
    xec_seq_op* visit( xec_expr_null* node );
    xec_seq_op* visit( xec_expr_bool* node );
    xec_seq_op* visit( xec_expr_number* node );
    xec_seq_op* visit( xec_expr_string* node );
    xec_seq_op* visit( xec_expr_local* node );
    xec_seq_op* visit( xec_expr_global* node );
    xec_seq_op* visit( xec_expr_upref* node );
    xec_seq_op* visit( xec_expr_objref* node );
    xec_seq_op* visit( xec_expr_key* node );
    xec_seq_op* visit( xec_expr_inkey* node );
    xec_seq_op* visit( xec_expr_index* node );
    xec_seq_op* visit( xec_expr_preop* node );
    xec_seq_op* visit( xec_expr_postop* node );
    xec_seq_op* visit( xec_expr_unary* node );
    xec_seq_op* visit( xec_expr_binary* node );
    xec_seq_op* visit( xec_expr_compare* node );
    xec_seq_op* visit( xec_expr_logical* node );
    xec_seq_op* visit( xec_expr_qmark* node );
    xec_seq_op* visit( xec_new_new* node );
    xec_seq_op* visit( xec_new_object* node );
    xec_seq_op* visit( xec_new_array* node );
    xec_seq_op* visit( xec_new_table* node );
    xec_seq_op* visit( xec_expr_mono* node );
    xec_seq_op* visit( xec_expr_call* node );
    xec_seq_op* visit( xec_expr_yield* node );
    xec_seq_op* visit( xec_expr_vararg* node );
    xec_seq_op* visit( xec_expr_unpack* node );
    xec_seq_op* visit( xec_expr_list* node );
    xec_seq_op* visit( xec_expr_assign* node );
    xec_seq_op* visit( xec_expr_assign_list* node );


private:

    xec_seq_builder* b;

};




/*
    Visit AST expression nodes that unpack to multiple values.
*/

class xec_seq_build_unpack
    :   public xec_ast_visitor< xec_seq_build_unpack, void, xec_seq_valist*, int >
{
public:

    explicit xec_seq_build_unpack( xec_seq_builder* b );

    using xec_ast_visitor< xec_seq_build_unpack, void, xec_seq_valist*, int >::visit;

    void fallback( xec_ast_node* node, xec_seq_valist* valist, int rcount );

    void visit( xec_expr_call* node, xec_seq_valist* valist, int rcount );
    void visit( xec_expr_yield* node, xec_seq_valist* valist, int rcount );
    void visit( xec_expr_vararg* node, xec_seq_valist* valist, int rcount );
    void visit( xec_expr_unpack* node, xec_seq_valist* valist, int rcount );
    void visit( xec_expr_list* node, xec_seq_valist* valist, int rcount );
    void visit( xec_expr_assign_list* node, xec_seq_valist* valist, int rcount );


private:

    void select( int sloc, xec_seq_valist* valist, int rcount, xec_seq_op* op );

    xec_seq_builder* b;

};







#endif


