//
//  xec_lj_expr.h
//
//  Created by Edmund Kapusniak on 07/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_LJ_EXPRESSION_H
#define XEC_LJ_EXPRESSION_H


#include "xec_ast_visitor.h"


class xec_lj_scope;
class xec_lj_value;
class xec_lj_builder;


/*
    xec expressions can contain embedded assignments and sub-expressions
    are referenced more than once and therefore must be pulled out into
    temporaries.  This visitor tells us if a sub-expression will produce
    any assignments in the prologue.
*/


class xec_lj_exprhasprologue
    :   public xec_ast_visitor< xec_lj_exprhasprologue, bool >
{
public:

    xec_lj_exprhasprologue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent );

    using xec_ast_visitor< xec_lj_exprhasprologue, bool >::visit;
    
    bool fallback( xec_ast_node* node );
    
    bool visit( xec_ast_func* node );
    
    bool visit( xec_expr_null* node );
    bool visit( xec_expr_bool* node );
    bool visit( xec_expr_number* node );
    bool visit( xec_expr_string* node );
    bool visit( xec_expr_local* node );
    bool visit( xec_expr_global* node );
    bool visit( xec_expr_upref* node );
    bool visit( xec_expr_objref* node );
    bool visit( xec_expr_key* node );
    bool visit( xec_expr_inkey* node );
    bool visit( xec_expr_index* node );
    bool visit( xec_expr_preop* node );
    bool visit( xec_expr_postop* node );
    bool visit( xec_expr_unary* node );
    bool visit( xec_expr_binary* node );
    bool visit( xec_expr_compare* node );
    bool visit( xec_expr_logical* node );
    bool visit( xec_expr_qmark* node );
    
    bool visit( xec_new_new* node );
    bool visit( xec_new_object* node );
    bool visit( xec_new_array* node );
    bool visit( xec_new_table* node );

    bool visit( xec_expr_mono* node );
    bool visit( xec_expr_call* node );
    bool visit( xec_expr_yield* node );
    bool visit( xec_expr_vararg* node );
    bool visit( xec_expr_unpack* node );
    bool visit( xec_expr_list* node );
    bool visit( xec_expr_assign* node );
    bool visit( xec_expr_assign_list* node );


private:

    xec_lj_builder* b;
    xec_lj_value*   v;
    xec_lj_scope*   scope;
    int             indent;


};



/*
    Many xec expressions cannot be expressed in a single lua expression.
    Visits an expression and produces a 'prologue' of assignments and also
    the actual expression returning a result.
*/


class xec_lj_exprprologue
    :   public xec_ast_visitor< xec_lj_exprprologue, void >
{
public:

    xec_lj_exprprologue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent );

    using xec_ast_visitor< xec_lj_exprprologue, void >::visit;

    void fallback( xec_ast_node* node );
    
    void visit( xec_ast_func* node );
    
    void visit( xec_expr_null* node );
    void visit( xec_expr_bool* node );
    void visit( xec_expr_number* node );
    void visit( xec_expr_string* node );
    void visit( xec_expr_local* node );
    void visit( xec_expr_global* node );
    void visit( xec_expr_upref* node );
    void visit( xec_expr_objref* node );
    void visit( xec_expr_key* node );
    void visit( xec_expr_inkey* node );
    void visit( xec_expr_index* node );
    void visit( xec_expr_preop* node );
    void visit( xec_expr_postop* node );
    void visit( xec_expr_unary* node );
    void visit( xec_expr_binary* node );
    void visit( xec_expr_compare* node );
    void visit( xec_expr_logical* node );
    void visit( xec_expr_qmark* node );
    
    void visit( xec_new_new* node );
    void visit( xec_new_object* node );
    void visit( xec_new_array* node );
    void visit( xec_new_table* node );

    void visit( xec_expr_mono* node );
    void visit( xec_expr_call* node );
    void visit( xec_expr_yield* node );
    void visit( xec_expr_vararg* node );
    void visit( xec_expr_unpack* node );
    void visit( xec_expr_list* node );
    void visit( xec_expr_assign* node );
    void visit( xec_expr_assign_list* node );


private:

    bool has_prologue( xec_ast_node* node );
    
    int temporary();
    int temporary( xec_ast_node* node );
    

    xec_lj_builder* b;
    xec_lj_value*   v;
    xec_lj_scope*   scope;
    int             indent;
    
    
};



/*
    And the value is the part of the expression which is inline in
    the wider expression.
*/


enum xec_lj_precedence
{
    XEC_LJ_NONE,
    XEC_LJ_MULTIPLE,
    XEC_LJ_POSTFIX,
    XEC_LJ_EXP,
    XEC_LJ_UNARY,
    XEC_LJ_MULTIPLY,
    XEC_LJ_ADD,
    XEC_LJ_CONCAT,
    XEC_LJ_COMPARE,
    XEC_LJ_AND,
    XEC_LJ_OR,
};


class xec_lj_exprvalue
    :   public xec_ast_visitor< xec_lj_exprvalue, void, int >
{
public:

    xec_lj_exprvalue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent );

    using xec_ast_visitor< xec_lj_exprvalue, void, int >::visit;
    
    void value( xec_ast_node* node, int precedence );

    void fallback( xec_ast_node* node, int precedence );
    
    void visit( xec_ast_func* node, int precedence );
    
    void visit( xec_expr_null* node, int precedence );
    void visit( xec_expr_bool* node, int precedence );
    void visit( xec_expr_number* node, int precedence );
    void visit( xec_expr_string* node, int precedence );
    void visit( xec_expr_local* node, int precedence );
    void visit( xec_expr_global* node, int precedence );
    void visit( xec_expr_upref* node, int precedence );
    void visit( xec_expr_objref* node, int precedence );
    void visit( xec_expr_key* node, int precedence );
    void visit( xec_expr_inkey* node, int precedence );
    void visit( xec_expr_index* node, int precedence );
    void visit( xec_expr_preop* node, int precedence );
    void visit( xec_expr_postop* node, int precedence );
    void visit( xec_expr_unary* node, int precedence );
    void visit( xec_expr_binary* node, int precedence );
    void visit( xec_expr_compare* node, int precedence );
    void visit( xec_expr_logical* node, int precedence );
    void visit( xec_expr_qmark* node, int precedence );
    
    void visit( xec_new_new* node, int precedence );
    void visit( xec_new_object* node, int precedence );
    void visit( xec_new_array* node, int precedence );
    void visit( xec_new_table* node, int precedence );

    void visit( xec_expr_mono* node, int precedence );
    void visit( xec_expr_call* node, int precedence );
    void visit( xec_expr_yield* node, int precedence );
    void visit( xec_expr_vararg* node, int precedence );
    void visit( xec_expr_unpack* node, int precedence );
    void visit( xec_expr_list* node, int precedence );
    void visit( xec_expr_assign* node, int precedence );
    void visit( xec_expr_assign_list* node, int precedence );


private:

    bool parentheses( xec_ast_node* node, int precedence, int exprprec );


    xec_lj_builder* b;
    xec_lj_value*   v;
    xec_lj_scope*   scope;
    int             indent;


};






#endif




