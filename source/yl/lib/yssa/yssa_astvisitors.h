//
//  yssa_astvisitors.h
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_ASTVISITORS_H
#define YSSA_ASTVISITORS_H


#include "yl_ast_visitor.h"
#include "yssa.h"


class yssa_builder;
struct yssa_valist;



/*
    Visit AST expression nodes and generate a single value from them.
*/

class yssa_build_expr
    :   public yl_ast_visitor< yssa_build_expr, yssaop* >
{
public:

    explicit yssa_build_expr( yssa_builder* b );

    using yl_ast_visitor< yssa_build_expr, yssaop* >::visit;

    yssaop* fallback( yl_ast_node* node );
    
    yssaop* visit( yl_ast_func* node );
    yssaop* visit( yl_expr_null* node );
    yssaop* visit( yl_expr_bool* node );
    yssaop* visit( yl_expr_number* node );
    yssaop* visit( yl_expr_string* node );
    yssaop* visit( yl_expr_local* node );
    yssaop* visit( yl_expr_global* node );
    yssaop* visit( yl_expr_upref* node );
    yssaop* visit( yl_expr_objref* node );
    yssaop* visit( yl_expr_key* node );
    yssaop* visit( yl_expr_inkey* node );
    yssaop* visit( yl_expr_index* node );
    yssaop* visit( yl_expr_preop* node );
    yssaop* visit( yl_expr_postop* node );
    yssaop* visit( yl_expr_unary* node );
    yssaop* visit( yl_expr_binary* node );
    yssaop* visit( yl_expr_compare* node );
    yssaop* visit( yl_expr_logical* node );
    yssaop* visit( yl_expr_qmark* node );
    yssaop* visit( yl_new_new* node );
    yssaop* visit( yl_new_object* node );
    yssaop* visit( yl_new_array* node );
    yssaop* visit( yl_new_table* node );
    yssaop* visit( yl_expr_mono* node );
    yssaop* visit( yl_expr_call* node );
    yssaop* visit( yl_expr_yield* node );
    yssaop* visit( yl_expr_vararg* node );
    yssaop* visit( yl_expr_unpack* node );
    yssaop* visit( yl_expr_list* node );
    yssaop* visit( yl_expr_assign* node );
    yssaop* visit( yl_expr_assign_list* node );


private:

    yssa_builder* b;

};



/*
    Visit AST expression nodes that unpack to multiple values.
*/

class yssa_build_unpack
    :   public yl_ast_visitor< yssa_build_unpack, void, yssa_valist*, int >
{
public:

    explicit yssa_build_unpack( yssa_builder* b );

    using yl_ast_visitor< yssa_build_unpack, void, yssa_valist*, int >::visit;

    void fallback( yl_ast_node* node, yssa_valist* values, int valcount );

    void visit( yl_expr_null* node, yssa_valist* values, int valcount );
    void visit( yl_expr_call* node, yssa_valist* values, int valcount );
    void visit( yl_expr_yield* node, yssa_valist* values, int valcount );
    void visit( yl_expr_vararg* node, yssa_valist* values, int valcount );
    void visit( yl_expr_unpack* node, yssa_valist* values, int valcount );
    void visit( yl_expr_list* node, yssa_valist* values, int valcount );
    void visit( yl_expr_assign_list* node, yssa_valist* values, int valcount );


private:

    yssa_builder* b;

};


/*
    Visit AST statements and generate SSA form.
*/

class yssa_build_stmt
    :   public yl_ast_visitor< yssa_build_stmt, void >
{
public:

    yssa_build_stmt( yssa_builder* b );

    using yl_ast_visitor< yssa_build_stmt, void >::visit;

    void fallback( yl_ast_node* node );
    
    void visit( yl_stmt_block* node );
    void visit( yl_stmt_if* node );
    void visit( yl_stmt_switch* node );
    void visit( yl_stmt_while* node );
    void visit( yl_stmt_do* node );
    void visit( yl_stmt_foreach* node );
    void visit( yl_stmt_for* node );
    void visit( yl_stmt_using* node );
    void visit( yl_stmt_try* node );
    void visit( yl_stmt_catch* node );
    void visit( yl_stmt_delete* node );
    void visit( yl_stmt_case* node );
    void visit( yl_stmt_continue* node );
    void visit( yl_stmt_break* node );
    void visit( yl_stmt_return* node );
    void visit( yl_stmt_throw* node );


private:

    yssa_builder* b;
    
};




#endif

