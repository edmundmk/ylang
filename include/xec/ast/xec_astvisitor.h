//
//  xec_astvisitor.h
//
//  Created by Edmund Kapusniak on 01/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_ASTVISITOR_H
#define XEC_ASTVISITOR_H


#include "xec_declaration.h"
#include "xec_expression.h"
#include "xec_constructor.h"
#include "xec_statement.h"



template < typename visitor_t, typename return_t, typename ... arguments_t >
class xec_astvisitor
{
public:

    return_t visit( xec_declaration* expr, arguments_t ... arguments );
    template < typename declaration_t > return_t dispatch_decl( xec_declaration* decl, arguments_t ... arguments );

    return_t visit( xec_declaration_var* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_declaration_object* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_declaration_prototype* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_declaration_function* expr, arguments_t ... arguments ) { return return_t(); }

    return_t visit( xec_expression* expr, arguments_t ... arguments );
    template < typename expression_t > return_t dispatch_expr( xec_expression* expr, arguments_t ... arguments );

    return_t visit( xec_expression_null* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_number* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_string* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_identifier* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_lookup* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_indexkey* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_index* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_yield* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_call* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_unary* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_comparison* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_logical* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_conditional* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_varargs* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_unpack* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_list* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_assign* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_mono* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_expression_declare* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_constructor_new* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_constructor_list* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_constructor_table* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_constructor_function* expr, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_constructor_object* expr, arguments_t ... arguments ) { return return_t(); }
    
    return_t visit( xec_statement* stmt, arguments_t ... arguments );
    template < typename statement_t > return_t dispatch_stmt( xec_statement* stmt, arguments_t ... arguments );
    
    return_t visit( xec_statement_declaration* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_expression* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_compound* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_delete* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_if* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_switch* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_case* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_while* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_do* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_foreach* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_for* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_continue* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_break* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_return* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_yield* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_using* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_usingscope* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_try* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_catch* decl, arguments_t ... arguments ) { return return_t(); }
    return_t visit( xec_statement_throw* decl, arguments_t ... arguments ) { return return_t(); }

};


template < typename visitor_t, typename return_t, typename ... arguments_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::visit( xec_declaration* decl, arguments_t ... arguments )
{
    typedef return_t (xec_astvisitor::*visit_t)( xec_declaration*, arguments_t ... );
    static visit_t dispatch[] =
    {
        &visitor_t::template dispatch_decl< xec_declaration_var >,
        &visitor_t::template dispatch_decl< xec_declaration_object >,
        &visitor_t::template dispatch_decl< xec_declaration_prototype >,
        &visitor_t::template dispatch_decl< xec_declaration_function >,

    };
    
    xec_declaration_dispatch index = decl->visitor_dispatch();
    return ( this->*( dispatch[ index ] ) )( decl, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename declaration_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_decl( xec_declaration* decl, arguments_t ... arguments )
{
    return visitor_t::visit( (declaration_t*)decl, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::visit( xec_expression* expr, arguments_t ... arguments )
{
    typedef return_t (xec_astvisitor::*visit_t)( xec_expression*, arguments_t ... );
    static visit_t dispatch[] =
    {
        &visitor_t::template dispatch_expr< xec_expression_null >,
        &visitor_t::template dispatch_expr< xec_expression_number >,
        &visitor_t::template dispatch_expr< xec_expression_string >,
        &visitor_t::template dispatch_expr< xec_expression_identifier >,
        &visitor_t::template dispatch_expr< xec_expression_lookup >,
        &visitor_t::template dispatch_expr< xec_expression_indexkey >,
        &visitor_t::template dispatch_expr< xec_expression_index >,
        &visitor_t::template dispatch_expr< xec_expression_yield >,
        &visitor_t::template dispatch_expr< xec_expression_call >,
        &visitor_t::template dispatch_expr< xec_expression_unary >,
        &visitor_t::template dispatch_expr< xec_expression_comparison >,
        &visitor_t::template dispatch_expr< xec_expression_logical >,
        &visitor_t::template dispatch_expr< xec_expression_conditional >,
        &visitor_t::template dispatch_expr< xec_expression_varargs >,
        &visitor_t::template dispatch_expr< xec_expression_unpack >,
        &visitor_t::template dispatch_expr< xec_expression_list >,
        &visitor_t::template dispatch_expr< xec_expression_assign >,
        &visitor_t::template dispatch_expr< xec_expression_mono >,
        &visitor_t::template dispatch_expr< xec_expression_declare >,
    };
    
    xec_expression_dispatch index = expr->visitor_dispatch();
    return ( this->*( dispatch[ index ] ) )( expr, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename expression_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_expr( xec_expression* expr, arguments_t ... arguments )
{
    return visitor_t::visit( (expression_t*)expr, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::visit( xec_statement* stmt, arguments_t ... arguments )
{
    typedef return_t (xec_astvisitor::*visit_t)( xec_statement*, arguments_t ... );
    static visit_t dispatch[] =
    {
        &visitor_t::template dispatch_stmt< xec_statement_declaration >,
        &visitor_t::template dispatch_stmt< xec_statement_expression >,
        &visitor_t::template dispatch_stmt< xec_statement_compound >,
        &visitor_t::template dispatch_stmt< xec_statement_delete >,
        &visitor_t::template dispatch_stmt< xec_statement_if >,
        &visitor_t::template dispatch_stmt< xec_statement_switch >,
        &visitor_t::template dispatch_stmt< xec_statement_case >,
        &visitor_t::template dispatch_stmt< xec_statement_while >,
        &visitor_t::template dispatch_stmt< xec_statement_do >,
        &visitor_t::template dispatch_stmt< xec_statement_foreach >,
        &visitor_t::template dispatch_stmt< xec_statement_for >,
        &visitor_t::template dispatch_stmt< xec_statement_continue >,
        &visitor_t::template dispatch_stmt< xec_statement_break >,
        &visitor_t::template dispatch_stmt< xec_statement_return >,
        &visitor_t::template dispatch_stmt< xec_statement_yield >,
        &visitor_t::template dispatch_stmt< xec_statement_using >,
        &visitor_t::template dispatch_stmt< xec_statement_usingscope >,
        &visitor_t::template dispatch_stmt< xec_statement_try >,
        &visitor_t::template dispatch_stmt< xec_statement_catch >,
        &visitor_t::template dispatch_stmt< xec_statement_throw >,
    };
    
    xec_statement_dispatch index = stmt->visitor_dispatch();
    return ( this->*( dispatch[ index ] ) )( stmt, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename statement_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_stmt( xec_statement* stmt, arguments_t ... arguments )
{
    return visitor_t::visit( (statement_t*)stmt, arguments ... );
}


#endif
