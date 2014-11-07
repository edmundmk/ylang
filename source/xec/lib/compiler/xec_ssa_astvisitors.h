//
//  xec_ssa_astvisitors.h
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_ASTVISITORS_H
#define XEC_SSA_ASTVISITORS_H


#include "xec_ast_visitor.h"
#include "xec_ssa.h"

class xec_ssa_builder;
struct xec_ssa_valist;


/*
    Visit AST expression nodes and generate a single value from them.
*/

class xec_ssa_build_expr
    :   public xec_ast_visitor< xec_ssa_build_expr, xec_ssa_opref >
{
public:

    explicit xec_ssa_build_expr( xec_ssa_builder* b );

    using xec_ast_visitor< xec_ssa_build_expr, xec_ssa_opref >::visit;

    xec_ssa_opref fallback( xec_ast_node* node );
    
    xec_ssa_opref visit( xec_ast_func* node );
    xec_ssa_opref visit( xec_expr_null* node );
    xec_ssa_opref visit( xec_expr_bool* node );
    xec_ssa_opref visit( xec_expr_number* node );
    xec_ssa_opref visit( xec_expr_string* node );
    xec_ssa_opref visit( xec_expr_local* node );
    xec_ssa_opref visit( xec_expr_global* node );
    xec_ssa_opref visit( xec_expr_upref* node );
    xec_ssa_opref visit( xec_expr_objref* node );
    xec_ssa_opref visit( xec_expr_key* node );
    xec_ssa_opref visit( xec_expr_inkey* node );
    xec_ssa_opref visit( xec_expr_index* node );
    xec_ssa_opref visit( xec_expr_preop* node );
    xec_ssa_opref visit( xec_expr_postop* node );
    xec_ssa_opref visit( xec_expr_unary* node );
    xec_ssa_opref visit( xec_expr_binary* node );
    xec_ssa_opref visit( xec_expr_compare* node );
    xec_ssa_opref visit( xec_expr_logical* node );
    xec_ssa_opref visit( xec_expr_qmark* node );
    xec_ssa_opref visit( xec_new_new* node );
    xec_ssa_opref visit( xec_new_object* node );
    xec_ssa_opref visit( xec_new_array* node );
    xec_ssa_opref visit( xec_new_table* node );
    xec_ssa_opref visit( xec_expr_mono* node );
    xec_ssa_opref visit( xec_expr_call* node );
    xec_ssa_opref visit( xec_expr_yield* node );
    xec_ssa_opref visit( xec_expr_vararg* node );
    xec_ssa_opref visit( xec_expr_unpack* node );
    xec_ssa_opref visit( xec_expr_list* node );
    xec_ssa_opref visit( xec_expr_assign* node );
    xec_ssa_opref visit( xec_expr_assign_list* node );


private:

    xec_ssa_builder* b;

};



/*
    Visit AST expression nodes that unpack to multiple values.
*/

class xec_ssa_build_unpack
    :   public xec_ast_visitor< xec_ssa_build_unpack, void, xec_ssa_valist*, int >
{
public:

    explicit xec_ssa_build_unpack( xec_ssa_builder* b );

    using xec_ast_visitor< xec_ssa_build_unpack, void, xec_ssa_valist*, int >::visit;

    void fallback( xec_ast_node* node, xec_ssa_valist* values, int valcount );

    void visit( xec_expr_null* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_call* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_yield* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_vararg* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_unpack* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_list* node, xec_ssa_valist* values, int valcount );
    void visit( xec_expr_assign_list* node, xec_ssa_valist* values, int valcount );


private:

    xec_ssa_builder* b;

};


/*
    Visit AST statements and generate SSA form.
*/

class xec_ssa_build_stmt
    :   public xec_ast_visitor< xec_ssa_build_stmt, void >
{
public:

    xec_ssa_build_stmt( xec_ssa_builder* b );

    using xec_ast_visitor< xec_ssa_build_stmt, void >::visit;

    void fallback( xec_ast_node* node );
    
    void visit( xec_stmt_block* node );
    void visit( xec_stmt_if* node );
    void visit( xec_stmt_switch* node );
    void visit( xec_stmt_while* node );
    void visit( xec_stmt_do* node );
    void visit( xec_stmt_foreach* node );
    void visit( xec_stmt_for* node );
    void visit( xec_stmt_using* node );
    void visit( xec_stmt_try* node );
    void visit( xec_stmt_catch* node );
    void visit( xec_stmt_delete* node );
    void visit( xec_stmt_case* node );
    void visit( xec_stmt_continue* node );
    void visit( xec_stmt_break* node );
    void visit( xec_stmt_return* node );
    void visit( xec_stmt_throw* node );


private:

    xec_ssa_builder* b;
    
};




#endif

