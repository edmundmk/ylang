//
//  xec_ssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_BUILDER_H
#define XEC_SSA_BUILDER_H


#include <unordered_map>
#include "xec_ssa.h"
#include "xec_ast_visitor.h"


class xec_ssa_builder;
class xec_ssa_build_expr;
struct xec_ssa_lvalue;
struct xec_ssa_valist;



/*
    Visit AST expression nodes and generate a single value from them.
*/

class xec_ssa_build_expr
    :   public xec_ast_visitor< xec_ssa_build_expr, xec_ssa_node* >
{
public:

    explicit xec_ssa_build_expr( xec_ssa_builder* b );

    using xec_ast_visitor< xec_ssa_build_expr, xec_ssa_node* >::visit;

    xec_ssa_node* fallback( xec_ast_node* node );
    
    xec_ssa_node* visit( xec_ast_func* node );
    xec_ssa_node* visit( xec_expr_null* node );
    xec_ssa_node* visit( xec_expr_bool* node );
    xec_ssa_node* visit( xec_expr_number* node );
    xec_ssa_node* visit( xec_expr_string* node );
    xec_ssa_node* visit( xec_expr_local* node );
    xec_ssa_node* visit( xec_expr_global* node );
    xec_ssa_node* visit( xec_expr_upref* node );
    xec_ssa_node* visit( xec_expr_objref* node );
    xec_ssa_node* visit( xec_expr_key* node );
    xec_ssa_node* visit( xec_expr_inkey* node );
    xec_ssa_node* visit( xec_expr_index* node );
    xec_ssa_node* visit( xec_expr_preop* node );
    xec_ssa_node* visit( xec_expr_postop* node );
    xec_ssa_node* visit( xec_expr_unary* node );
    xec_ssa_node* visit( xec_expr_binary* node );
    xec_ssa_node* visit( xec_expr_compare* node );
    xec_ssa_node* visit( xec_expr_logical* node );
    xec_ssa_node* visit( xec_expr_qmark* node );
    xec_ssa_node* visit( xec_new_new* node );
    xec_ssa_node* visit( xec_new_object* node );
    xec_ssa_node* visit( xec_new_array* node );
    xec_ssa_node* visit( xec_new_table* node );
    xec_ssa_node* visit( xec_expr_mono* node );
    xec_ssa_node* visit( xec_expr_call* node );
    xec_ssa_node* visit( xec_expr_yield* node );
    xec_ssa_node* visit( xec_expr_vararg* node );
    xec_ssa_node* visit( xec_expr_unpack* node );
    xec_ssa_node* visit( xec_expr_list* node );
    xec_ssa_node* visit( xec_expr_assign* node );
    xec_ssa_node* visit( xec_expr_assign_list* node );


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





/*
    Traverse an AST and build SSA form IR from it.
*/

class xec_ssa_builder
{
public:

    explicit xec_ssa_builder( xec_ssa* root );
    
    bool            build( xec_ast* ast );
    
    template< typename ... arguments_t >
    xec_ssa_packed* packed( arguments_t ... arguments );
    template< typename ... arguments_t >
    xec_ssa_triple* triple( arguments_t ... arguments );
    template< typename ... arguments_t >
    xec_ssa_expand* expand( arguments_t ... arguments );

    xec_ssa_node*   node( xec_ssa_node* node );

    void            define( xec_ast_name* name, xec_ssa_node* node );
    void            define( xec_new_object* object, xec_ssa_node* node );
    void            define( xec_ast_node* temporary, xec_ssa_node* node );
    
    xec_ssa_node*   lookup( xec_ast_name* name );
    xec_ssa_node*   lookup( xec_new_object* object );
    xec_ssa_node*   lookup( xec_ast_node* temporary );
    
    void            close_scope( xec_ast_scope* scope );
    
    void            ifthen( xec_ssa_node* condition );
    void            ifelse();
    void            ifend();
    
    void            switchopen( xec_ssa_node* value );
    void            switchcase( xec_ssa_node* value );
    void            switchbreak();
    void            switchend();
    
    void            loopopen();
    void            loopcontinue();
    void            loopbreak();
    void            loopend();
    
    void            funcreturn();

    xec_ssa_func*   func( xec_ast_func* func );
    xec_ssa_node*   expr( xec_ast_node* node );
    void            unpack( xec_ssa_valist* l, xec_ast_node* node, int count );
    
    void            lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node );
    xec_ssa_node*   lvalue_value( xec_ssa_lvalue* lvalue );
    xec_ssa_node*   lvalue_assign( xec_ssa_lvalue* lvalue, xec_ssa_node* val );
    
    
private:

    void            build_function( xec_ast_func* astf, xec_ssa_func* ssaf );


    xec_ssa* root;
    xec_ssa_build_expr   build_expr;
    xec_ssa_build_unpack build_unpack;
    xec_ssa_build_stmt   build_stmt;
    std::unordered_map< xec_ast_func*, xec_ssa_func* > funcmap;

};



template < typename ... arguments_t >
inline xec_ssa_packed* xec_ssa_builder::packed( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_packed( arguments ... );
}

template < typename ... arguments_t >
inline xec_ssa_triple* xec_ssa_builder::triple( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_triple( arguments ... );
}

template < typename ... arguments_t >
inline xec_ssa_expand* xec_ssa_builder::expand( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_expand( arguments ... );
}


#endif


