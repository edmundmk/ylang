//
//  xec_ssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_BUILDER_H
#define XEC_SSA_BUILDER_H


#include "xec_ssa.h"
#include "xec_ast_visitor.h"


class xec_ssa_builder;
class xec_ssa_build_expr;
struct xec_ssa_lvalue;



/*
    Visit AST expression nodes and generate a single value from them.
*/

class xec_ssa_build_expr
    :   public xec_ast_visitor< xec_ssa_build_expr, xec_ssa_node* >
{
public:

    xec_ssa_build_expr( xec_ssa_builder* b );

    using xec_ast_visitor< xec_ssa_build_expr, xec_ssa_node* >::visit;

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
    Visit AST statements and generate SSA form.
*/

class xec_ssa_build_stmt
    :   public xec_ast_visitor< xec_ssa_build_stmt, void >
{
public:

    xec_ssa_build_stmt( xec_ssa_builder* b );
    
    

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
    xec_ssa_node*   packed( arguments_t ... arguments );

    xec_ssa_node*   node( xec_ssa_node* node );

    void            define( xec_ast_name* name, xec_ssa_node* node );
    void            define( void* object, xec_ssa_node* node );
    xec_ssa_node*   lookup( xec_ast_name* name );
    xec_ssa_node*   lookup( void* object );

    void            ifthen( xec_ssa_node* condition );
    void            ifelse();
    void            ifend();


    xec_ssa_node*   expr( xec_ast_node* node );
    void            eval( xec_ast_node* node );
    
    void            lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node );
    xec_ssa_node*   lvalue_value( xec_ssa_lvalue* lvalue );
    void            lvalue_assign( xec_ssa_lvalue* lvalue, xec_ssa_node* node );
    
    
private:

    xec_ssa* root;
    xec_ssa_build_expr build_expr;
    xec_ssa_build_stmt build_stmt;

};

/*


x < 3 < 5 < 10

result = x < 3
if ( result )
    result = 3 < 5
    if ( result )
        result = 5 < 10
    endif
endif



class xec_ssa_builder
{
public:

    




    xec_buildssa();

    xec_ssaname     declare( const char* text );
    void            retire( xec_ssaname name );

    void            write( xec_ssaname name, xec_ssavalue value );
    xec_ssavalue    read( xec_ssaname );

    void            loop_begin();
    void            loop_break();
    void            loop_continue();
    void            loop_end();
    void            if_true( xec_ssavalue condition );
    void            if_false( xec_ssavalue condition );
    void            if_else();
    void            if_end();
    
    xec_ssavalue    null();
    xec_ssavalue    literal( bool boolean );
    xec_ssavalue    literal( double number );
    xec_ssavalue    literal( const char* string, size_t length );
    xec_ssavalue    emit( int opcode, xec_ssavalue a );
    xec_ssavalue    emit( int opcode, xec_ssavalue a, xec_ssavalue b );
    void            parameter( xec_ssavalue a );
    xec_ssavalue    multiple( int opcode, int count );
    xec_ssavalue    result( xec_ssavalue v, int n );


private:

};

*/



#endif
