//
//  xec_semantics.cpp
//
//  Created by Edmund Kapusniak on 05/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_semantics.h"
#include "xec_scope.h"
#include "xec_astvisitor.h"
#include "xec_parser.h"



class xec_sema;
class xec_sema_reuse_scope;
class xec_sema_declare_name;
class xec_sema_declare_var;
class xec_sema_lvalue;




/*
    Compound statements normally require a new scope, except when they use
    the existing scope for a loop statement.  Bare declarations are not
    allowed in this context either.
*/

class xec_sema_reuse_scope
    :   public xec_astvisitor< xec_sema_reuse_scope, void, xec_scope* >
{
public:

    xec_sema_reuse_scope( xec_parser* p, xec_sema* sema );

    using xec_astvisitor< xec_sema_reuse_scope, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_reuse_scope, void, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope );
    void fallback( xec_expression* expr, xec_scope* scope );
    void fallback( xec_statement* stmt, xec_scope* scope );
    
    void visit( xec_statement_declaration* stmt, xec_scope* scope );
    void visit( xec_statement_compound* stmt, xec_scope* scope );


protected:

    xec_parser* p;
    xec_sema*   sema;
    
};



/*
    Also, the bare statements in an if aren't allowed to be declarations.
*/

class xec_sema_forbid_declaration
    :   public xec_astvisitor< xec_sema_forbid_declaration, void, xec_scope* >
{
public:

    xec_sema_forbid_declaration( xec_parser* p, xec_sema* sema );
    
    using xec_astvisitor< xec_sema_forbid_declaration, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_forbid_declaration, void, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope );
    void fallback( xec_expression* expr, xec_scope* scope );
    void fallback( xec_statement* stmt, xec_scope* scope );
    
    void visit( xec_statement_declaration* stmt, xec_scope* scope );
    
    
protected:

    xec_parser* p;
    xec_sema*   sema;
    
};




/*
    Resolves an expression representing the name of a declaration.
*/

class xec_sema_declare_name
    :   public xec_astvisitor< xec_sema_declare_name, void, xec_scope* >
{
public:

    explicit xec_sema_declare_name( xec_parser* p );

protected:

    xec_parser* p;
    
};



/*
    Declares an expression list containing parameter declarations
*/

class xec_sema_declare_params
    :   public xec_astvisitor< xec_sema_declare_params,
                        void, xec_scope*, xec_constructor_function* >
{
public:

    explicit xec_sema_declare_params( xec_parser* p );

};



/*
    Declares an expression list containing var declarations.
*/

class xec_sema_declare_var
    :   public xec_astvisitor< xec_sema_declare_var, void, xec_scope* >
{
public:

    explicit xec_sema_declare_var( xec_parser* p );

};


/*
    Ensures lvalues are assignable, and looks up identifiers.
*/

class xec_sema_lvalue
    :   public xec_astvisitor< xec_sema_lvalue, void, xec_scope* >
{
public:

    explicit xec_sema_lvalue( xec_parser* p );

};




/*
    Traverses AST and performs semantic analysis.
*/

class xec_sema
    :   public xec_astvisitor< xec_sema, void, xec_scope* >
{
public:

    explicit xec_sema( xec_parser* p );
    
    using xec_astvisitor< xec_sema, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema, void, xec_scope* >::visit;

    void visit( xec_declaration_var* decl, xec_scope* scope );
    void visit( xec_declaration_object* decl, xec_scope* scope );
    void visit( xec_declaration_prototype* decl, xec_scope* scope );
    void visit( xec_declaration_function* decl, xec_scope* scope );

    void visit( xec_expression_identifier* expr, xec_scope* scope );
    void visit( xec_expression_lookup* expr, xec_scope* scope );
    void visit( xec_expression_indexkey* expr, xec_scope* scope );
    void visit( xec_expression_index* expr, xec_scope* scope );
    void visit( xec_expression_yield* expr, xec_scope* scope );
    void visit( xec_expression_call* expr, xec_scope* scope );
    void visit( xec_expression_preop* expr, xec_scope* scope );
    void visit( xec_expression_postop* expr, xec_scope* scope );
    void visit( xec_expression_unary* expr, xec_scope* scope );
    void visit( xec_expression_binary* expr, xec_scope* scope );
    void visit( xec_expression_comparison* expr, xec_scope* scope );
    void visit( xec_expression_logical* expr, xec_scope* scope );
    void visit( xec_expression_conditional* expr, xec_scope* scope );
    void visit( xec_expression_varargs* expr, xec_scope* scope );
    void visit( xec_expression_unpack* expr, xec_scope* scope );
    void visit( xec_expression_list* expr, xec_scope* scope );
    void visit( xec_expression_assign* expr, xec_scope* scope );
    void visit( xec_expression_mono* expr, xec_scope* scope );
    void visit( xec_expression_declare* expr, xec_scope* scope );
    void visit( xec_constructor_new* expr, xec_scope* scope );
    void visit( xec_constructor_list* expr, xec_scope* scope );
    void visit( xec_constructor_table* expr, xec_scope* scope );
    void visit( xec_constructor_object* expr, xec_scope* scope );
    void visit( xec_constructor_function* expr, xec_scope* scope );
    
    void visit( xec_statement_declaration* stmt, xec_scope* scope );
    void visit( xec_statement_expression* stmt, xec_scope* scope );
    void visit( xec_statement_compound* stmt, xec_scope* scope );
    void visit( xec_statement_delete* stmt, xec_scope* scope );
    void visit( xec_statement_if* stmt, xec_scope* scope );
    void visit( xec_statement_switch* stmt, xec_scope* scope );
    void visit( xec_statement_case* stmt, xec_scope* scope );
    void visit( xec_statement_while* stmt, xec_scope* scope );
    void visit( xec_statement_do* stmt, xec_scope* scope );
    void visit( xec_statement_foreach* stmt, xec_scope* scope );
    void visit( xec_statement_for* stmt, xec_scope* scope );
    void visit( xec_statement_continue* stmt, xec_scope* scope );
    void visit( xec_statement_break* stmt, xec_scope* scope );
    void visit( xec_statement_return* stmt, xec_scope* scope );
    void visit( xec_statement_yield* stmt, xec_scope* scope );
    void visit( xec_statement_using* stmt, xec_scope* scope );
    void visit( xec_statement_usingscope* stmt, xec_scope* scope );
    void visit( xec_statement_try* stmt, xec_scope* scope );
    void visit( xec_statement_catch* stmt, xec_scope* scope );
    void visit( xec_statement_throw* stmt, xec_scope* scope );


protected:

    xec_constructor_function* get_function( xec_scope* scope );
    xec_scope* get_continue_scope( xec_scope* scope );
    xec_scope* get_break_scope( xec_scope* scope );


    xec_parser*                 p;
    xec_sema_reuse_scope        sema_reuse_scope;
    xec_sema_forbid_declaration sema_forbid_declaration;
    xec_sema_declare_name       sema_declare_name;
    xec_sema_declare_params     sema_declare_params;
    xec_sema_declare_var        sema_declare_var;
    xec_sema_lvalue             sema_lvalue;
    
};



xec_sema::xec_sema( xec_parser* p )
    :   p( p )
    ,   sema_reuse_scope( p, this )
    ,   sema_forbid_declaration( p, this )
    ,   sema_declare_name( p )
    ,   sema_declare_params( p )
    ,   sema_declare_var( p )
    ,   sema_lvalue( p )
{
}




void xec_sema::visit( xec_declaration_var* decl, xec_scope* scope )
{
    // Visit expression.
    visit( decl->get_expr_list(), scope );

    // Declare names into scope.
    sema_declare_var.visit( decl->get_name_list(), scope );
}

void xec_sema::visit( xec_declaration_object* decl, xec_scope* scope )
{
    // Declare object name.
    sema_declare_name.visit( decl->get_name(), scope );
    
    // Visit object.
    visit( decl->get_object(), scope );
}

void xec_sema::visit( xec_declaration_prototype* decl, xec_scope* scope )
{
    // Handle prototype declaration.
}

void xec_sema::visit( xec_declaration_function* decl, xec_scope* scope )
{
    // Declare function name (need to handle prototypes).

    // Visit function.
    visit( decl->get_function(), scope );
}




void xec_sema::visit( xec_expression_identifier* expr, xec_scope* scope )
{
    // Resolve identifier.
}

void xec_sema::visit( xec_expression_lookup* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_indexkey* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
    visit( expr->get_index(), scope );
}

void xec_sema::visit( xec_expression_index* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
    visit( expr->get_index(), scope );
}

void xec_sema::visit( xec_expression_yield* expr, xec_scope* scope )
{
    // Check that we are inside a coroutine.
    if ( ! get_function( scope )->get_coroutine() )
    {
        p->diagnostic( expr->get_location(), "cannot yield from here" );
    }

    // Visit arguments.
    visit( expr->get_arguments(), scope );
}

void xec_sema::visit( xec_expression_call* expr, xec_scope* scope )
{
    // Check that yieldcalls are inside coroutines.
    if ( expr->get_yieldcall() && ! get_function( scope )->get_coroutine() )
    {
        p->diagnostic( expr->get_location(), "cannot yield call here" );
    }

    // Visit expression.
    visit( expr->get_expr(), scope );
    visit( expr->get_arguments(), scope );
}

void xec_sema::visit( xec_expression_preop* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_postop* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_unary* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_binary* expr, xec_scope* scope )
{
    visit( expr->get_lhs(), scope );
    visit( expr->get_rhs(), scope );
}

void xec_sema::visit( xec_expression_comparison* expr, xec_scope* scope )
{
    visit( expr->get_first_expr(), scope );
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), scope );
}

void xec_sema::visit( xec_expression_logical* expr, xec_scope* scope )
{
    visit( expr->get_lhs(), scope );
    visit( expr->get_rhs(), scope );
}

void xec_sema::visit( xec_expression_conditional* expr, xec_scope* scope )
{
    visit( expr->get_condition(), scope );
    visit( expr->get_iftrue(), scope );
    visit( expr->get_iffalse(), scope );
}

void xec_sema::visit( xec_expression_varargs* expr, xec_scope* scope )
{
    // Check that the function is varargs.
    if ( ! get_function( scope )->get_varargs() )
    {
        p->diagnostic( expr->get_location(), "no variable argument list" );
    }
}

void xec_sema::visit( xec_expression_unpack* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_list* expr, xec_scope* scope )
{
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), scope );
    if ( expr->get_final() )
        visit( expr->get_final(), scope );
}

void xec_sema::visit( xec_expression_assign* expr, xec_scope* scope )
{
    // Resolve lvalues on left hand side of assignment.
    sema_lvalue.visit( expr->get_lhs(), scope );
    visit( expr->get_rhs(), scope );
}

void xec_sema::visit( xec_expression_mono* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema::visit( xec_expression_declare* expr, xec_scope* scope )
{
    // Visit values on right.
    visit( expr->get_expr_list(), scope );

    // Declare variables on left.
    sema_declare_var.visit( expr->get_name_list(), scope );
}

void xec_sema::visit( xec_constructor_new* expr, xec_scope* scope )
{
    visit( expr->get_proto(), scope );
    visit( expr->get_arguments(), scope );
}

void xec_sema::visit( xec_constructor_list* expr, xec_scope* scope )
{
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), scope );
    if ( expr->get_final() )
        visit( expr->get_final(), scope );
}

void xec_sema::visit( xec_constructor_table* expr, xec_scope* scope )
{
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        visit( expr->get_key( i ), scope );
        visit( expr->get_value( i ), scope );
    }
}

void xec_sema::visit( xec_constructor_object* expr, xec_scope* scope )
{
    // Resolve prototype.
    if ( expr->get_proto() )
        visit( expr->get_proto(), scope );
    
    // Create new object scope.
    scope = scope->make_child_scope( XEC_SCOPE_OBJECT );
    expr->set_scope( scope );
    
    // Visit object declarations.
    for ( size_t i = 0; i < expr->get_member_count(); ++i )
        visit( expr->get_member( i ), scope );
}

void xec_sema::visit( xec_constructor_function* expr, xec_scope* scope )
{
    // Give function a new scope.
    scope = scope->make_child_scope( XEC_SCOPE_FUNCTION );
    scope->set_function( expr );
    expr->set_scope( scope );
    
    // Declare this, if the function is thiscall.
    if ( expr->get_thiscall() )
    {
    
    }
    
    // Declare parameters.
    sema_declare_params.visit( expr->get_parameters(), scope, expr );
    
    // Visit body.
    sema_reuse_scope.visit( expr->get_body(), scope );
}




void xec_sema::visit( xec_statement_declaration* stmt, xec_scope* scope )
{
    visit( stmt->get_decl(), scope );
}

void xec_sema::visit( xec_statement_expression* stmt, xec_scope* scope )
{
    visit( stmt->get_expr(), scope );
}

void xec_sema::visit( xec_statement_compound* stmt, xec_scope* scope )
{
    // New scope for compound.
    scope = scope->make_child_scope( XEC_SCOPE_BLOCK );
    stmt->set_scope( scope );
    
    // Visit statements in block.
    for ( size_t i = 0; i < stmt->get_count(); ++i )
        visit( stmt->get_stmt( i ), scope );
}

void xec_sema::visit( xec_statement_delete* stmt, xec_scope* scope )
{
    visit( stmt->get_expr_list(), scope );
}

void xec_sema::visit( xec_statement_if* stmt, xec_scope* scope )
{
    // New scope for if.
    scope = scope->make_child_scope( XEC_SCOPE_BLOCK );
    stmt->set_scope( scope );
    
    // Visit condition.
    visit( stmt->get_expr(), scope );
    
    // Visit statements.
    sema_forbid_declaration.visit( stmt->get_iftrue(), scope );
    sema_forbid_declaration.visit( stmt->get_iffalse(), scope );
}

void xec_sema::visit( xec_statement_switch* stmt, xec_scope* scope )
{
    // New scope for switch.
    scope = scope->make_child_scope( XEC_SCOPE_SWITCH );
    stmt->set_scope( scope );
    
    // Visit condition.
    visit( stmt->get_expr(), scope );
    
    // Visit statements.
    sema_reuse_scope.visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_case* stmt, xec_scope* scope )
{
    // case statements are only allowed inside a switch.
    if ( scope->get_kind() != XEC_SCOPE_SWITCH )
    {
        const char* stmtname = stmt->get_expr() ? "case" : "default";
        p->diagnostic( stmt->get_location(), "%s outside switch", stmtname );
    }
    
    // Visit expression.
    if ( stmt->get_expr() )
        visit( stmt->get_expr(), scope );
}

void xec_sema::visit( xec_statement_while* stmt, xec_scope* scope )
{
    // New scope for while loop.
    scope = scope->make_child_scope( XEC_SCOPE_LOOP );
    stmt->set_scope( scope );
    
    // Visit condition.
    visit( stmt->get_expr(), scope );
    
    // Visit body.
    sema_reuse_scope.visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_do* stmt, xec_scope* scope )
{
    // New scope for do loop.
    scope = scope->make_child_scope( XEC_SCOPE_LOOP );
    stmt->set_scope( scope );
    
    // Visit body.
    sema_reuse_scope.visit( stmt->get_body(), scope );

    // Visit condition (deliberately inside the loop scope).
    visit( stmt->get_expr(), scope );
}

void xec_sema::visit( xec_statement_foreach* stmt, xec_scope* scope )
{
    // New scope for loop.
    scope = scope->make_child_scope( XEC_SCOPE_LOOP );
    stmt->set_scope( scope );
    
    if ( stmt->get_declare() )
    {
        // Visit expression to be iterated over.
        visit( stmt->get_expr(), scope );

        // Declare names.
        sema_declare_var.visit( stmt->get_expr_list(), scope );
    }
    else
    {
        // Visit lvalues.
        sema_lvalue.visit( stmt->get_expr_list(), scope );
        
        // Visit expression to be iterated over.
        visit( stmt->get_expr(), scope );
    }
    
    // Visit body.
    sema_reuse_scope.visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_for* stmt, xec_scope* scope )
{
    // New scope for loop.
    scope = scope->make_child_scope( XEC_SCOPE_LOOP );
    stmt->set_scope( scope );
    
    // Visit header expressions.
    visit( stmt->get_init(), scope );
    visit( stmt->get_expr(), scope );
    visit( stmt->get_update(), scope );
    
    // Visit body.
    sema_reuse_scope.visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_continue* stmt, xec_scope* scope )
{
    // Find loop to continue.
    xec_scope* continue_scope = get_continue_scope( scope );
    if ( ! continue_scope )
    {
        p->diagnostic( stmt->get_location(), "continue outside loop" );
    }
}

void xec_sema::visit( xec_statement_break* stmt, xec_scope* scope )
{
    // Find loop or switch to break.
    xec_scope* break_scope = get_break_scope( scope );
    if ( ! break_scope )
    {
        p->diagnostic( stmt->get_location(), "break outside loop or switch" );
    }
}

void xec_sema::visit( xec_statement_return* stmt, xec_scope* scope )
{
    visit( stmt->get_expr_list(), scope );
}

void xec_sema::visit( xec_statement_yield* stmt, xec_scope* scope )
{
    // Check that we are inside a coroutine.
    if ( ! get_function( scope )->get_coroutine() )
    {
        p->diagnostic( stmt->get_location(), "cannot yield from here" );
    }

    // Visit expression.
    visit( stmt->get_expr_list(), scope );
}

void xec_sema::visit( xec_statement_using* stmt, xec_scope* scope )
{
    // Visit expression.
    visit( stmt->get_expr(), scope );
}

void xec_sema::visit( xec_statement_usingscope* stmt, xec_scope* scope )
{
    // Create new scope for using.
    scope = scope->make_child_scope( XEC_SCOPE_BLOCK );
    stmt->set_scope( scope );
    
    // Visit expression.
    visit( stmt->get_expr(), scope );
    
    // Visit body.
    visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_try* stmt, xec_scope* scope )
{
    sema_forbid_declaration.visit( stmt->get_body(), scope );
    for ( size_t i = 0; i < stmt->get_catch_count(); ++i )
        visit( stmt->get_catch( i ), scope );
    if ( stmt->get_final() )
        sema_forbid_declaration.visit( stmt->get_final(), scope );
}

void xec_sema::visit( xec_statement_catch* stmt, xec_scope* scope )
{
    // Catch header.
    if ( stmt->get_declare() )
    {
        // Visit prototype expression.
        visit( stmt->get_proto(), scope );
        
        // Declare exception name.
        visit( stmt->get_lvalue(), scope );
    }
    else
    {
        // Visit lvalues.
        sema_lvalue.visit( stmt->get_lvalue(), scope );
        
        // Visit prototype expression.
        visit( stmt->get_proto(), scope );
    }
    
    // Visit body.
    visit( stmt->get_body(), scope );
}

void xec_sema::visit( xec_statement_throw* stmt, xec_scope* scope )
{
    visit( stmt->get_expr(), scope );
}





xec_sema_reuse_scope::xec_sema_reuse_scope( xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
{
}

void xec_sema_reuse_scope::fallback( xec_declaration* decl, xec_scope* scope )
{
    sema->visit( decl, scope );
}

void xec_sema_reuse_scope::fallback( xec_expression* expr, xec_scope* scope )
{
    sema->visit( expr, scope );
}

void xec_sema_reuse_scope::fallback( xec_statement* stmt, xec_scope* scope )
{
    sema->visit( stmt, scope );
}

void xec_sema_reuse_scope::visit(
                xec_statement_declaration* stmt, xec_scope* scope )
{
    // This is the body of a loop or switch - it cannot be a declaration.
    p->diagnostic( stmt->get_location(), "declaration is forbidden here" );
}
    
void xec_sema_reuse_scope::visit(
                xec_statement_compound* stmt, xec_scope* scope )
{
    // Visit sub-statements.
    for ( size_t i = 0; stmt->get_count(); ++i )
        sema->visit( stmt->get_stmt( i ), scope );
}




xec_sema_forbid_declaration::xec_sema_forbid_declaration(
                                    xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
{
}
    
void xec_sema_forbid_declaration::fallback(
                xec_declaration* decl, xec_scope* scope )
{
    sema->visit( decl, scope );
}

void xec_sema_forbid_declaration::fallback(
                xec_expression* expr, xec_scope* scope )
{
    sema->visit( expr, scope );
}

void xec_sema_forbid_declaration::fallback(
                xec_statement* stmt, xec_scope* scope )
{
    sema->visit( stmt, scope );
}
    
void xec_sema_forbid_declaration::visit(
                xec_statement_declaration* stmt, xec_scope* scope )
{
    // This is the body of an if or try - it cannot be a declaration.
    p->diagnostic( stmt->get_location(), "declaration is forbidden here" );
}



xec_sema_declare_name::xec_sema_declare_name( xec_parser* p )
    :   p( p )
{
}




xec_sema_declare_params::xec_sema_declare_params( xec_parser* p )
{
}




xec_sema_declare_var::xec_sema_declare_var( xec_parser* p )
{
}




xec_sema_lvalue::xec_sema_lvalue( xec_parser* p )
{
}






void xec_semantics( xec_parser* p )
{
    xec_sema sema( p );
    sema.visit( p->get_script(), p->get_global_scope() );
}




