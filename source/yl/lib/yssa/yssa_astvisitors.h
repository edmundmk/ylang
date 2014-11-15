//
//  xssa_astvisitors.h
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_ASTVISITORS_H
#define XSSA_ASTVISITORS_H


#include "xec_ast_visitor.h"
#include "xssa.h"


class xssa_builder;
struct xssa_valist;



/*
    Visit AST expression nodes and generate a single value from them.
*/

class xssa_build_expr
    :   public xec_ast_visitor< xssa_build_expr, xssaop* >
{
public:

    explicit xssa_build_expr( xssa_builder* b );

    using xec_ast_visitor< xssa_build_expr, xssaop* >::visit;

    xssaop* fallback( xec_ast_node* node );
    
    xssaop* visit( xec_ast_func* node );
    xssaop* visit( xec_expr_null* node );
    xssaop* visit( xec_expr_bool* node );
    xssaop* visit( xec_expr_number* node );
    xssaop* visit( xec_expr_string* node );
    xssaop* visit( xec_expr_local* node );
    xssaop* visit( xec_expr_global* node );
    xssaop* visit( xec_expr_upref* node );
    xssaop* visit( xec_expr_objref* node );
    xssaop* visit( xec_expr_key* node );
    xssaop* visit( xec_expr_inkey* node );
    xssaop* visit( xec_expr_index* node );
    xssaop* visit( xec_expr_preop* node );
    xssaop* visit( xec_expr_postop* node );
    xssaop* visit( xec_expr_unary* node );
    xssaop* visit( xec_expr_binary* node );
    xssaop* visit( xec_expr_compare* node );
    xssaop* visit( xec_expr_logical* node );
    xssaop* visit( xec_expr_qmark* node );
    xssaop* visit( xec_new_new* node );
    xssaop* visit( xec_new_object* node );
    xssaop* visit( xec_new_array* node );
    xssaop* visit( xec_new_table* node );
    xssaop* visit( xec_expr_mono* node );
    xssaop* visit( xec_expr_call* node );
    xssaop* visit( xec_expr_yield* node );
    xssaop* visit( xec_expr_vararg* node );
    xssaop* visit( xec_expr_unpack* node );
    xssaop* visit( xec_expr_list* node );
    xssaop* visit( xec_expr_assign* node );
    xssaop* visit( xec_expr_assign_list* node );


private:

    xssa_builder* b;

};



/*
    Visit AST expression nodes that unpack to multiple values.
*/

class xssa_build_unpack
    :   public xec_ast_visitor< xssa_build_unpack, void, xssa_valist*, int >
{
public:

    explicit xssa_build_unpack( xssa_builder* b );

    using xec_ast_visitor< xssa_build_unpack, void, xssa_valist*, int >::visit;

    void fallback( xec_ast_node* node, xssa_valist* values, int valcount );

    void visit( xec_expr_null* node, xssa_valist* values, int valcount );
    void visit( xec_expr_call* node, xssa_valist* values, int valcount );
    void visit( xec_expr_yield* node, xssa_valist* values, int valcount );
    void visit( xec_expr_vararg* node, xssa_valist* values, int valcount );
    void visit( xec_expr_unpack* node, xssa_valist* values, int valcount );
    void visit( xec_expr_list* node, xssa_valist* values, int valcount );
    void visit( xec_expr_assign_list* node, xssa_valist* values, int valcount );


private:

    xssa_builder* b;

};


/*
    Visit AST statements and generate SSA form.
*/

class xssa_build_stmt
    :   public xec_ast_visitor< xssa_build_stmt, void >
{
public:

    xssa_build_stmt( xssa_builder* b );

    using xec_ast_visitor< xssa_build_stmt, void >::visit;

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

    xssa_builder* b;
    
};




#endif

