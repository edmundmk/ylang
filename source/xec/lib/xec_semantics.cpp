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
class xec_sema_forbid_declaration;
class xec_sema_declare_param;
class xec_sema_declare_var;
class xec_sema_lvalue;
class xec_sema_nonglobal;




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
    Declares a parameter declaration.
*/

class xec_sema_declare_param
    :   public xec_astvisitor< xec_sema_declare_param,
                        void, xec_scope*, xec_constructor_function* >
{
public:

    explicit xec_sema_declare_param( xec_parser* p );

    using xec_astvisitor< xec_sema_declare_param, void, xec_scope*, xec_constructor_function* >::fallback;
    using xec_astvisitor< xec_sema_declare_param, void, xec_scope*, xec_constructor_function* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope, xec_constructor_function* function );
    void fallback( xec_expression* expr, xec_scope* scope, xec_constructor_function* function );
    void fallback( xec_statement* stmt, xec_scope* scope, xec_constructor_function* function );
    
    void visit( xec_expression_identifier* expr, xec_scope* scope, xec_constructor_function* function );
    void visit( xec_expression_varargs* expr, xec_scope* scope, xec_constructor_function* function );
    

private:

    xec_parser* p;

};



/*
    Declares a var declaration.
*/

class xec_sema_declare_var
    :   public xec_astvisitor< xec_sema_declare_var, void, xec_scope* >
{
public:

    explicit xec_sema_declare_var( xec_parser* p );
    
    using xec_astvisitor< xec_sema_declare_var, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_declare_var, void, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope );
    void fallback( xec_expression* expr, xec_scope* scope );
    void fallback( xec_statement* stmt, xec_scope* scope );
    
    void visit( xec_expression_identifier* expr, xec_scope* scope );


private:

    xec_parser* p;

};





/*
    Ensures that the leftmost identifier in an lvalue is either not a global
    name or is the special name 'global'.  This rule helps avoid typos.
*/

class xec_sema_nonglobal
    :   public xec_astvisitor< xec_sema_nonglobal, void, xec_scope* >
{
public:

    xec_sema_nonglobal( xec_parser* p, xec_sema* sema );
    
    using xec_astvisitor< xec_sema_nonglobal, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_nonglobal, void, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope );
    void fallback( xec_expression* expr, xec_scope* scope );
    void fallback( xec_statement* stmt, xec_scope* scope );
    
    void visit( xec_expression_identifier* expr, xec_scope* scope );
    void visit( xec_expression_lookup* expr, xec_scope* scope );
    void visit( xec_expression_indexkey* expr, xec_scope* scope );
    void visit( xec_expression_index* expr, xec_scope* scope );
    void visit( xec_expression_list* expr, xec_scope* scope );
    void visit( xec_expression_mono* expr, xec_scope* scope );


private:

    xec_parser* p;
    xec_sema*   sema;

};




/*
    Ensures lvalues are assignable, and looks up identifiers.
*/

class xec_sema_lvalue
    :   public xec_astvisitor< xec_sema_lvalue, void, xec_scope* >
{
public:

    xec_sema_lvalue( xec_parser* p, xec_sema* sema );

    using xec_astvisitor< xec_sema_lvalue, void, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_lvalue, void, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl, xec_scope* scope );
    void fallback( xec_expression* expr, xec_scope* scope );
    void fallback( xec_statement* stmt, xec_scope* scope );
    
    void visit( xec_expression_identifier* expr, xec_scope* scope );
    void visit( xec_expression_lookup* expr, xec_scope* scope );
    void visit( xec_expression_indexkey* expr, xec_scope* scope );
    void visit( xec_expression_index* expr, xec_scope* scope );
    void visit( xec_expression_conditional* expr, xec_scope* scope );
    void visit( xec_expression_list* expr, xec_scope* scope );
    void visit( xec_expression_mono* expr, xec_scope* scope );


private:

    xec_parser*         p;
    xec_sema*           sema;
    xec_sema_nonglobal  sema_nonglobal;
    

};



/*
    Declare prototype name.
*/

class xec_sema_declare_prototype
    :   public xec_astvisitor< xec_sema_declare_prototype, void,
                                    xec_declaration_prototype*, xec_scope* >
{
public:

    xec_sema_declare_prototype( xec_parser* p );
    
    using xec_astvisitor< xec_sema_declare_prototype, void,
                            xec_declaration_prototype*, xec_scope* >::fallback;
    using xec_astvisitor< xec_sema_declare_prototype, void,
                            xec_declaration_prototype*, xec_scope* >::visit;
    
    void fallback( xec_declaration* decl,
                    xec_declaration_prototype* prototype, xec_scope* scope );
    void fallback( xec_expression* expr,
                    xec_declaration_prototype* prototype, xec_scope* scope );
    void fallback( xec_statement* stmt,
                    xec_declaration_prototype* prototype, xec_scope* scope );
    
    void visit( xec_expression_identifier* expr,
                    xec_declaration_prototype* prototype, xec_scope* scope );


private:

    xec_parser* p;

};



/*
    Check prototype parameters.
*/

class xec_sema_check_params
    :   public xec_astvisitor<
                xec_sema_check_params, void, xec_declaration_prototype* >
{
public:

    explicit xec_sema_check_params( xec_parser* p );
    
    using xec_astvisitor< xec_sema_check_params,
                    void, xec_declaration_prototype* >::fallback;
    using xec_astvisitor< xec_sema_check_params,
                    void, xec_declaration_prototype* >::visit;
    
    void fallback( xec_declaration* decl, xec_declaration_prototype* p );
    void fallback( xec_expression* expr, xec_declaration_prototype* p );
    void fallback( xec_statement* stmt, xec_declaration_prototype* p );

    void visit( xec_expression_identifier* expr, xec_declaration_prototype* p );
    void visit( xec_expression_varargs* expr, xec_declaration_prototype* p );

private:

    xec_parser* p;
    
};



/*
    Declare object name.
*/

class xec_sema_declare_object
    :   public xec_astvisitor<
                    xec_sema_declare_object, xec_scope*, xec_scope*, bool >
{
public:

    explicit xec_sema_declare_object( xec_parser* p, xec_sema* sema );

    using xec_astvisitor<
            xec_sema_declare_object, xec_scope*, xec_scope*, bool >::fallback;
    using xec_astvisitor<
            xec_sema_declare_object, xec_scope*, xec_scope*, bool >::visit;
    
    xec_scope* fallback( xec_declaration* decl,
                                    xec_scope* scope, bool declare );
    xec_scope* fallback( xec_expression* expr,
                                    xec_scope* scope, bool declare );
    xec_scope* fallback( xec_statement* stmt,
                                    xec_scope* scope, bool declare );
    
    xec_scope* visit( xec_expression_identifier* expr,
                                    xec_scope* scope, bool declare );
    xec_scope* visit( xec_expression_lookup* expr,
                                    xec_scope* scope, bool declare );
    
private:

    xec_parser* p;
    xec_sema*   sema;
    
};




/*
    Declare function name.
*/

class xec_sema_declare_function
    :   public xec_astvisitor
                <
                    xec_sema_declare_function,
                    xec_scope*,
                    xec_constructor_function*,
                    xec_scope*,
                    bool
                >
{
public:

    xec_sema_declare_function( xec_parser* p, xec_sema* sema );
    
    using xec_astvisitor< xec_sema_declare_function,
        xec_scope*, xec_constructor_function*, xec_scope*, bool >::fallback;
    using xec_astvisitor< xec_sema_declare_function,
        xec_scope*, xec_constructor_function*, xec_scope*, bool >::visit;

    xec_scope* fallback( xec_declaration* decl,
        xec_constructor_function* function, xec_scope* scope, bool declare );
    xec_scope* fallback( xec_expression* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare );
    xec_scope* fallback( xec_statement* stmt,
        xec_constructor_function* function, xec_scope* scope, bool declare );
    
    xec_scope* visit( xec_expression_identifier* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare );
    xec_scope* visit( xec_expression_lookup* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare );

private:

    bool resolve_prototype( xec_scope* scope,
            const char* identifier, xec_constructor_function* function );

    xec_parser* p;
    xec_sema*   sema;

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

    friend class xec_sema_lvalue;
    friend class xec_sema_nonglobal;
    friend class xec_sema_declare_object;
    friend class xec_sema_declare_function;

    xec_name* lookup_name( xec_scope* scope, const char* name );

    xec_constructor_function* get_function( xec_scope* scope );
    xec_scope* get_continue_scope( xec_scope* scope );
    xec_scope* get_break_scope( xec_scope* scope );


    xec_parser*                 p;
    xec_sema_reuse_scope        sema_reuse_scope;
    xec_sema_forbid_declaration sema_forbid_declaration;
    xec_sema_declare_param      sema_declare_param;
    xec_sema_declare_var        sema_declare_var;
    xec_sema_lvalue             sema_lvalue;
    xec_sema_declare_object     sema_declare_object;
    xec_sema_declare_prototype  sema_declare_prototype;
    xec_sema_declare_function   sema_declare_function;
    xec_sema_check_params       sema_check_params;
    
    std::unordered_set< xec_declaration_prototype* > prototypes;
    
};



xec_sema::xec_sema( xec_parser* p )
    :   p( p )
    ,   sema_reuse_scope( p, this )
    ,   sema_forbid_declaration( p, this )
    ,   sema_declare_param( p )
    ,   sema_declare_var( p )
    ,   sema_lvalue( p, this )
    ,   sema_declare_prototype( p )
    ,   sema_declare_object( p, this )
    ,   sema_declare_function( p, this )
    ,   sema_check_params( p )
{
}




void xec_sema::visit( xec_declaration_var* decl, xec_scope* scope )
{
    // Visit expression.
    if ( decl->get_expr_list() )
        visit( decl->get_expr_list(), scope );

    // Declare names into scope.
    xec_expression_list* vars = decl->get_name_list();
    for ( size_t i = 0; i < vars->get_count(); ++i )
        sema_declare_var.visit( vars->get_expr( i ), scope );
    if ( vars->get_final() )
        sema_declare_var.visit( vars->get_final(), scope );
}

void xec_sema::visit( xec_declaration_object* decl, xec_scope* scope )
{
    // Declare object.
    xec_constructor_object* object = decl->get_object();
    
    // Resolve prototype.
    if ( object->get_proto() )
        visit( object->get_proto(), scope );
    
    // Declare object name (also declares object scope).
    scope = sema_declare_object.visit( decl->get_name(), scope, true );
    
    // Visit object declarations.
    for ( size_t i = 0; i < object->get_member_count(); ++i )
        visit( object->get_member( i ), scope );
}

void xec_sema::visit( xec_declaration_prototype* decl, xec_scope* scope )
{
    // Check prototype parameters.
    xec_expression_list* parameters = decl->get_parameters();
    for ( size_t i = 0; i < parameters->get_count(); ++i )
        sema_check_params.visit( parameters->get_expr( i ), decl );
    if ( parameters->get_final() )
        sema_check_params.visit( parameters->get_final(), decl );

    // Declare prototype name.
    sema_declare_prototype.visit( decl->get_name(), decl, scope );
    
    // Remember prototype as it hasn't yet been linked with a definition.
    prototypes.insert( decl );
}

void xec_sema::visit( xec_declaration_function* decl, xec_scope* scope )
{
    // Declare function name (need to handle prototypes).
    //      If it's a single identifier, then declare the name.  Resolve
    //          against implied prototypes in the same scope.
    //      If it's compound, look up in implied scopes, and if it
    //          matches a prototype, then resolve against that prototype.
    
    //      If the function matches a prototype, then the parameter lists
    //          must match.  The scope for the function is child scope
    //          of the object containing a prototype.

    //      Functions in object scopes (including those with compound names)
    //          must be marked as thiscall.

    // Declaring function.
    xec_constructor_function* function = decl->get_function();

    // Declare function name (also finds scope lookup should proceed from).
    xec_scope* prototype_scope = sema_declare_function.visit(
                            decl->get_name(), function, scope, true );
    scope = prototype_scope ? prototype_scope : scope;

    // Visit function.
    visit( decl->get_function(), scope );
}




void xec_sema::visit( xec_expression_identifier* expr, xec_scope* scope )
{
    // Resolve identifier.
    xec_name* name = lookup_name( scope, expr->get_identifier() );
    expr->set_name( name );
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
    xec_expression_list* vars = expr->get_name_list();
    for ( size_t i = 0; i < vars->get_count(); ++i )
        sema_declare_var.visit( vars->get_expr( i ), scope );
    if ( vars->get_final() )
        sema_declare_var.visit( vars->get_final(), scope );
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
    
    // Declare this and super, if the function is thiscall.
    if ( expr->get_thiscall() )
    {
        scope->declare_name( XEC_NAME_THIS, "this" );
        scope->declare_name( XEC_NAME_BASE, "base" );
    }
    
    // Declare parameters.
    xec_expression_list* params = expr->get_parameters();
    for ( size_t i = 0; i < params->get_count(); ++i )
        sema_declare_param.visit( params->get_expr( i ), scope, expr );
    if ( params->get_final() )
        sema_declare_param.visit( params->get_final(), scope, expr );
    
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
        xec_expression_list* vars = stmt->get_expr_list();
        for ( size_t i = 0; i < vars->get_count(); ++i )
            sema_declare_var.visit( vars->get_expr( i ), scope );
        if ( vars->get_final() )
            sema_declare_var.visit( vars->get_final(), scope );
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



xec_name* xec_sema::lookup_name( xec_scope* scope, const char* name )
{
    // Lookup name in outer scopes.
    for ( ; scope != NULL; scope = scope->get_outer() )
    {
        xec_name* result = scope->lookup_name( name );
        if ( result )
            return result;
    }
    
    // Otherwise 'declare' it in global scope.
    scope = p->get_global_scope();
    return scope->declare_name( XEC_NAME_GLOBAL, name );
}



xec_constructor_function* xec_sema::get_function( xec_scope* scope )
{
    for ( ; scope != NULL; scope = scope->get_outer() )
    {
        if ( scope->get_kind() == XEC_SCOPE_FUNCTION )
        {
            return scope->get_function();
        }
    }
    
    return NULL;
}

xec_scope* xec_sema::get_continue_scope( xec_scope* scope )
{
    for ( ; scope != NULL; scope = scope->get_outer() )
    {
        if ( scope->get_kind() == XEC_SCOPE_LOOP )
        {
            return scope;
        }
    }
    
    return NULL;
}

xec_scope* xec_sema::get_break_scope( xec_scope* scope )
{
    for ( ; scope != NULL; scope = scope->get_outer() )
    {
        if ( scope->get_kind() == XEC_SCOPE_LOOP
                || scope->get_kind() == XEC_SCOPE_SWITCH )
        {
            return scope;
        }
    }
    
    return NULL;
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
    for ( size_t i = 0; i < stmt->get_count(); ++i )
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





xec_sema_declare_param::xec_sema_declare_param( xec_parser* p )
    :   p( p )
{
}


void xec_sema_declare_param::fallback( xec_declaration* decl,
                xec_scope* scope, xec_constructor_function* function )
{
    p->diagnostic( decl->get_location(), "invalid parameter name" );
}

void xec_sema_declare_param::fallback( xec_expression* expr,
                xec_scope* scope, xec_constructor_function* function )
{
    p->diagnostic( expr->get_location(), "invalid parameter name" );
}

void xec_sema_declare_param::fallback( xec_statement* stmt,
                xec_scope* scope, xec_constructor_function* function )
{
    p->diagnostic( stmt->get_location(), "invalid parameter name" );
}
    

void xec_sema_declare_param::visit( xec_expression_identifier* expr,
                xec_scope* scope, xec_constructor_function* function )
{
    // Check for duplicate declaration.
    if ( scope->lookup_name( expr->get_identifier() ) )
    {
        p->diagnostic( expr->get_location(),
                "parameter '%s' already declared", expr->get_identifier() );
        return;
    }
    
    // Declare.
    xec_name* name =
            scope->declare_name( XEC_NAME_PARAMETER, expr->get_identifier() );
    expr->set_name( name );
}

void xec_sema_declare_param::visit( xec_expression_varargs* expr,
                xec_scope* scope, xec_constructor_function* function )
{
    // Grammar ensures that this is the last expression.
    function->set_varargs( true );
}









xec_sema_declare_var::xec_sema_declare_var( xec_parser* p )
    :   p( p )
{
}

void xec_sema_declare_var::fallback( xec_declaration* decl, xec_scope* scope )
{
    p->diagnostic( decl->get_location(), "malformed variable name" );
}

void xec_sema_declare_var::fallback( xec_expression* expr, xec_scope* scope )
{
    p->diagnostic( expr->get_location(), "malformed variable name" );
}

void xec_sema_declare_var::fallback( xec_statement* stmt, xec_scope* scope )
{
    p->diagnostic( stmt->get_location(), "malformed variable name" );
}

void xec_sema_declare_var::visit(
                xec_expression_identifier* expr, xec_scope* scope )
{
    // Check for duplicate declarations.
    if ( scope->lookup_name( expr->get_identifier() ) )
    {
        p->diagnostic( expr->get_location(),
                "variable '%s' already declared", expr->get_identifier() );
        return;
    }
    
    // Declare variable.
    xec_name* name =
            scope->declare_name( XEC_NAME_VARIABLE, expr->get_identifier() );
    expr->set_name( name );
}





xec_sema_nonglobal::xec_sema_nonglobal( xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
{
}

void xec_sema_nonglobal::fallback( xec_declaration* decl, xec_scope* scope )
{
    sema->visit( decl, scope );
}

void xec_sema_nonglobal::fallback( xec_expression* expr, xec_scope* scope )
{
    sema->visit( expr, scope );
}

void xec_sema_nonglobal::fallback( xec_statement* stmt, xec_scope* scope )
{
    sema->visit( stmt, scope );
}
    
void xec_sema_nonglobal::visit(
                xec_expression_identifier* expr, xec_scope* scope )
{
    // Look up name.
    xec_name* name = sema->lookup_name( scope, expr->get_identifier() );

    // Globals are not allowed, except for special name 'global'.
    if ( name->get_kind() == XEC_NAME_GLOBAL &&
                        strcmp( expr->get_identifier(), "global" ) != 0 )
    {
        p->diagnostic( expr->get_location(),
                "unknown variable '%s'", expr->get_identifier() );
        return;
    }
    
    // Resolve name.
    expr->set_name( name );
}

void xec_sema_nonglobal::visit( xec_expression_lookup* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}

void xec_sema_nonglobal::visit(
                xec_expression_indexkey* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
    sema->visit( expr->get_index(), scope );
}

void xec_sema_nonglobal::visit( xec_expression_index* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
    sema->visit( expr->get_index(), scope );
}

void xec_sema_nonglobal::visit( xec_expression_list* expr, xec_scope* scope )
{
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), scope );
    if ( expr->get_final() )
        visit( expr->get_final(), scope );
}

void xec_sema_nonglobal::visit( xec_expression_mono* expr, xec_scope* scope )
{
    visit( expr->get_expr(), scope );
}






xec_sema_lvalue::xec_sema_lvalue( xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
    ,   sema_nonglobal( p, sema )
{
}

void xec_sema_lvalue::fallback( xec_declaration* decl, xec_scope* scope )
{
    p->diagnostic( decl->get_location(), "expression is not assignable" );
}

void xec_sema_lvalue::fallback( xec_expression* expr, xec_scope* scope )
{
    p->diagnostic( expr->get_location(), "expression is not assignable" );
}

void xec_sema_lvalue::fallback( xec_statement* stmt, xec_scope* scope )
{
    p->diagnostic( stmt->get_location(), "expression is not assignable" );
}


void xec_sema_lvalue::visit(
                xec_expression_identifier* expr, xec_scope* scope )
{
    sema_nonglobal.visit( expr, scope );
}

void xec_sema_lvalue::visit( xec_expression_lookup* expr, xec_scope* scope )
{
    sema_nonglobal.visit( expr, scope );
}

void xec_sema_lvalue::visit( xec_expression_indexkey* expr, xec_scope* scope )
{
    sema_nonglobal.visit( expr, scope );
}

void xec_sema_lvalue::visit( xec_expression_index* expr, xec_scope* scope )
{
    sema_nonglobal.visit( expr, scope );
}

void xec_sema_lvalue::visit(
                xec_expression_conditional* expr, xec_scope* scope )
{
    // Conditional expressions are assignable if both branches are.
    sema->visit( expr->get_condition(), scope );
    visit( expr->get_iftrue(), scope );
    visit( expr->get_iffalse(), scope );
}

void xec_sema_lvalue::visit( xec_expression_list* expr, xec_scope* scope )
{
    // Every expression in the list must be assignable.
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), scope );
    if ( expr->get_final() )
        visit( expr->get_final(), scope );
}

void xec_sema_lvalue::visit( xec_expression_mono* expr, xec_scope* scope )
{
    // The mono expression must be assignable.
    visit( expr->get_expr(), scope );
}





xec_sema_declare_prototype::xec_sema_declare_prototype( xec_parser* p )
    :   p( p )
{
}

void xec_sema_declare_prototype::fallback( xec_declaration* decl,
                xec_declaration_prototype* prototype, xec_scope* scope )
{
    p->diagnostic( decl->get_location(), "invalid prototype name" );
}

void xec_sema_declare_prototype::fallback( xec_expression* expr,
                xec_declaration_prototype* prototype, xec_scope* scope )
{
    p->diagnostic( expr->get_location(), "invalid prototype name" );
}

void xec_sema_declare_prototype::fallback( xec_statement* stmt,
                xec_declaration_prototype* prototype, xec_scope* scope )
{
    p->diagnostic( stmt->get_location(), "invalid prototype name" );
}
    
void xec_sema_declare_prototype::visit( xec_expression_identifier* expr,
                xec_declaration_prototype* prototype, xec_scope* scope )
{
    if ( scope->get_kind() != XEC_SCOPE_OBJECT )
    {
        p->diagnostic( expr->get_location(), "prototype outside object" );
        return;
    }
    
    if ( scope->lookup_name( expr->get_identifier() ) )
    {
        p->diagnostic( expr->get_location(),
            "object member '%s' already declared", expr->get_identifier() );
    }
    
    xec_name* name = scope->declare_name(
                    XEC_NAME_PROTOTYPE, expr->get_identifier() );
    name->set_prototype( prototype );
    expr->set_name( name );
}




xec_sema_check_params::xec_sema_check_params( xec_parser* p )
    :   p( p )
{
}
    
void xec_sema_check_params::fallback(
                xec_declaration* decl, xec_declaration_prototype* prototype )
{
    p->diagnostic( decl->get_location(), "invalid parameter name" );
}

void xec_sema_check_params::fallback(
                xec_expression* expr, xec_declaration_prototype* prototype )
{
    p->diagnostic( expr->get_location(), "invalid parameter name" );
}

void xec_sema_check_params::fallback(
                xec_statement* stmt, xec_declaration_prototype* prototype )
{
    p->diagnostic( stmt->get_location(), "invalid parameter name" );
}


void xec_sema_check_params::visit(
        xec_expression_identifier* expr, xec_declaration_prototype* prototype )
{
    // Valid parameter name.
}

void xec_sema_check_params::visit(
        xec_expression_varargs* expr, xec_declaration_prototype* prototype )
{
    // Set varargs.
    prototype->set_varargs( true );
}





xec_sema_declare_object::xec_sema_declare_object(
                                xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
{
}


xec_scope* xec_sema_declare_object::fallback(
                xec_declaration* decl, xec_scope* scope, bool declare )
{
    p->diagnostic( decl->get_location(), "invalid object name" );
    return scope;
}

xec_scope* xec_sema_declare_object::fallback(
                xec_expression* expr, xec_scope* scope, bool declare )
{
    p->diagnostic( expr->get_location(), "invalid object name" );
    return scope;
}

xec_scope* xec_sema_declare_object::fallback(
                xec_statement* stmt, xec_scope* scope, bool declare )
{
    p->diagnostic( stmt->get_location(), "invalid object name" );
    return scope;
}
    
xec_scope* xec_sema_declare_object::visit(
        xec_expression_identifier* expr, xec_scope* scope, bool declare )
{
    const char* identifier = expr->get_identifier();
    if ( declare )
    {
        // Object declared with a single identifier actually declares a var.
        // Check for already-declared variable.
        if ( scope->lookup_name( identifier ) )
        {
            p->diagnostic( expr->get_location(),
                "variable '%s' already declared", identifier );
        }
        
        // Declare.
        xec_name* name = scope->declare_name( XEC_NAME_VARIABLE, identifier );
        expr->set_name( name );
    
        // Final name opens new scope to declare object members in.
        return scope->declare_scope( XEC_SCOPE_OBJECT, identifier );
    }
    else
    {
        // No globals except 'global' allowed, as for any other lvalue.
        xec_name* name = sema->lookup_name( scope, identifier );
        if ( name->get_kind() == XEC_NAME_GLOBAL
                        && strcmp( identifier, "global" ) != 0 )
        {
            p->diagnostic( expr->get_location(),
                            "unknown variable '%s'", identifier );
        }
        expr->set_name( name );
    
        // Leftmost identifier in a compound name - just (re)open scope.
        xec_scope* reopen = scope->lookup_scope( identifier );
        if ( reopen )
            return reopen;
        else
            return scope->declare_scope( XEC_SCOPE_IMPLIED, identifier );
    }
}

xec_scope* xec_sema_declare_object::visit(
        xec_expression_lookup* expr, xec_scope* scope, bool declare )
{
    // Reopen scope on left.
    scope = visit( expr->get_expr(), scope, false );
    
    const char* identifier = expr->get_identifier();
    if ( declare )
    {
        // This is the final name that declares a new object.  Even if the
        // scope was implied by a previous declaration, declare a fresh scope
        // to contain the object members.
        return scope->declare_scope( XEC_SCOPE_OBJECT, identifier );
    }
    else
    {
        // Attempt to reopen the scope.
        xec_scope* reopen = scope->lookup_scope( identifier );
        if ( reopen )
            return reopen;
        else
            return scope->declare_scope( XEC_SCOPE_IMPLIED, identifier );
    }
}





xec_sema_declare_function::xec_sema_declare_function(
                                xec_parser* p, xec_sema* sema )
    :   p( p )
    ,   sema( sema )
{
}
    
xec_scope* xec_sema_declare_function::fallback( xec_declaration* decl,
        xec_constructor_function* function, xec_scope* scope, bool declare )
{
    p->diagnostic( decl->get_location(), "invalid function name" );
    return scope;
}

xec_scope* xec_sema_declare_function::fallback( xec_expression* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare )
{
    p->diagnostic( expr->get_location(), "invalid function name" );
    return scope;
}

xec_scope* xec_sema_declare_function::fallback( xec_statement* stmt,
        xec_constructor_function* function, xec_scope* scope, bool declare )
{
    p->diagnostic( stmt->get_location(), "invalid function name" );
    return scope;
}
    
xec_scope* xec_sema_declare_function::visit( xec_expression_identifier* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare )
{
    const char* identifier = expr->get_identifier();
    if ( declare )
    {
        // Single name, so declare the name.
        if ( scope->lookup_name( identifier ) )
        {
            p->diagnostic( expr->get_location(),
                        "function '%s' already declared", identifier );
        }
        
        // Declare.
        xec_name* name = scope->declare_name( XEC_NAME_VARIABLE, identifier );
        expr->set_name( name );
     
        // Functions declared in object scopes are thiscall.
        if ( scope->get_kind() == XEC_SCOPE_OBJECT )
            function->set_thiscall( true );
    
        // Lookups of names inside the function happen in this scope.
        if ( resolve_prototype( scope, expr->get_identifier(), function ) )
            return scope;
        else
            return NULL;
    }
    else
    {
        // No globals except 'global' allowed, as for any other lvalue.
        xec_name* name = sema->lookup_name( scope, identifier );
        if ( name->get_kind() == XEC_NAME_GLOBAL
                        && strcmp( identifier, "global" ) != 0 )
        {
            p->diagnostic( expr->get_location(),
                            "unknown variable '%s'", identifier );
        }
        expr->set_name( name );
    
        // Leftmost identifier in a compound name - just (re)open scope.
        xec_scope* reopen = scope->lookup_scope( identifier );
        if ( reopen )
            return reopen;
        else
            return scope->declare_scope( XEC_SCOPE_IMPLIED, identifier );
    }
}

xec_scope* xec_sema_declare_function::visit( xec_expression_lookup* expr,
        xec_constructor_function* function, xec_scope* scope, bool declare )
{
    // Reopen scope on left.
    scope = visit( expr->get_expr(), function, scope, false );
    
    const char* identifier = expr->get_identifier();
    if ( declare )
    {
        // Was declared with a compound identifier, so it's thiscall.
        function->set_thiscall( true );
    
        // Final name that declares the function.
        if ( resolve_prototype( scope, expr->get_identifier(), function ) )
            return scope;
        else
            return NULL;
    }
    else
    {
        // Attempt to reopen the scope.
        xec_scope* reopen = scope->lookup_scope( identifier );
        if ( reopen )
            return reopen;
        else
            return scope->declare_scope( XEC_SCOPE_IMPLIED, identifier );
    }

}



bool xec_sema_declare_function::resolve_prototype( xec_scope* scope,
                const char* identifier, xec_constructor_function* function )
{
    // Look up prototype.
    xec_name* name = scope->lookup_name( identifier );
    if ( ! name )
        return false;
    xec_declaration_prototype* prototype = name->get_prototype();
    if ( ! prototype )
        return false;
    
    // Check that parameters match.
    if ( prototype->get_varargs() != function->get_varargs()
            || prototype->get_parameters()->get_count()
                            != function->get_parameters()->get_count() )
    {
        p->diagnostic( function->get_location(),
                            "parameter list does not match prototype" );
        return false;
    }
    
    // Check if the prototype has been matched.
    if ( sema->prototypes.find( prototype ) == sema->prototypes.end() )
    {
        p->diagnostic( function->get_location(),
                            "multiple implementations for prototype" );
        return false;
    }
    
    // Matched a prototype.
    sema->prototypes.erase( prototype );
    return true;
}






void xec_semantics( xec_parser* p )
{
    xec_sema sema( p );
    sema.visit( p->get_script(), p->get_global_scope() );
}




