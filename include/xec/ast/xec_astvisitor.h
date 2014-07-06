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

    return_t fallback( xec_declaration* decl, arguments_t ... arguments ) { return return_t(); }

    return_t visit( xec_declaration_var* decl, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( decl, arguments ... ); }
    return_t visit( xec_declaration_object* decl, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( decl, arguments ... ); }
    return_t visit( xec_declaration_prototype* decl, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( decl, arguments ... ); }
    return_t visit( xec_declaration_function* decl, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( decl, arguments ... ); }

    return_t visit( xec_expression* expr, arguments_t ... arguments );
    template < typename expression_t > return_t dispatch_expr( xec_expression* expr, arguments_t ... arguments );

    return_t fallback( xec_expression* expr, arguments_t ... arguments ) { return return_t(); }

    return_t visit( xec_expression_null* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_bool* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_number* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_string* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_identifier* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_lookup* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_indexkey* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_index* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_yield* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_call* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_preop* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_postop* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_unary* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_binary* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_comparison* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_logical* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_conditional* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_varargs* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_unpack* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_list* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_assign* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_mono* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_expression_declare* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_constructor_new* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_constructor_list* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_constructor_table* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_constructor_object* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    return_t visit( xec_constructor_function* expr, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( expr, arguments ... ); }
    
    return_t visit( xec_statement* stmt, arguments_t ... arguments );
    template < typename statement_t > return_t dispatch_stmt( xec_statement* stmt, arguments_t ... arguments );
    
    return_t fallback( xec_statement* stmt, arguments_t ... arguments ) { return return_t(); }
    
    return_t visit( xec_statement_declaration* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_expression* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_compound* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_delete* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_if* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_switch* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_case* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_while* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_do* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_foreach* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_for* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_continue* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_break* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_return* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_yield* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_using* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_usingscope* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_try* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_catch* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }
    return_t visit( xec_statement_throw* stmt, arguments_t ... arguments ) { return ( (visitor_t*)this )->fallback( stmt, arguments ... ); }

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
    
    xec_declaration_kind index = decl->get_kind();
    return ( this->*( dispatch[ index ] ) )( decl, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename declaration_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_decl( xec_declaration* decl, arguments_t ... arguments )
{
    return ( (visitor_t*)this )->visit( (declaration_t*)decl, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::visit( xec_expression* expr, arguments_t ... arguments )
{
    typedef return_t (xec_astvisitor::*visit_t)( xec_expression*, arguments_t ... );
    static visit_t dispatch[] =
    {
        &visitor_t::template dispatch_expr< xec_expression_null >,
        &visitor_t::template dispatch_expr< xec_expression_bool >,
        &visitor_t::template dispatch_expr< xec_expression_number >,
        &visitor_t::template dispatch_expr< xec_expression_string >,
        &visitor_t::template dispatch_expr< xec_expression_identifier >,
        &visitor_t::template dispatch_expr< xec_expression_lookup >,
        &visitor_t::template dispatch_expr< xec_expression_indexkey >,
        &visitor_t::template dispatch_expr< xec_expression_index >,
        &visitor_t::template dispatch_expr< xec_expression_yield >,
        &visitor_t::template dispatch_expr< xec_expression_call >,
        &visitor_t::template dispatch_expr< xec_expression_preop >,
        &visitor_t::template dispatch_expr< xec_expression_postop >,
        &visitor_t::template dispatch_expr< xec_expression_unary >,
        &visitor_t::template dispatch_expr< xec_expression_binary >,
        &visitor_t::template dispatch_expr< xec_expression_comparison >,
        &visitor_t::template dispatch_expr< xec_expression_logical >,
        &visitor_t::template dispatch_expr< xec_expression_conditional >,
        &visitor_t::template dispatch_expr< xec_expression_varargs >,
        &visitor_t::template dispatch_expr< xec_expression_unpack >,
        &visitor_t::template dispatch_expr< xec_expression_list >,
        &visitor_t::template dispatch_expr< xec_expression_assign >,
        &visitor_t::template dispatch_expr< xec_expression_mono >,
        &visitor_t::template dispatch_expr< xec_expression_declare >,
        &visitor_t::template dispatch_expr< xec_constructor_new >,
        &visitor_t::template dispatch_expr< xec_constructor_list >,
        &visitor_t::template dispatch_expr< xec_constructor_table >,
        &visitor_t::template dispatch_expr< xec_constructor_object >,
        &visitor_t::template dispatch_expr< xec_constructor_function >,
    };
    
    xec_expression_kind index = expr->get_kind();
    return ( this->*( dispatch[ index ] ) )( expr, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename expression_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_expr( xec_expression* expr, arguments_t ... arguments )
{
    return ( (visitor_t*)this )->visit( (expression_t*)expr, arguments ... );
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
    
    xec_statement_kind index = stmt->get_kind();
    return ( this->*( dispatch[ index ] ) )( stmt, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename statement_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_stmt( xec_statement* stmt, arguments_t ... arguments )
{
    return ( (visitor_t*)this )->visit( (statement_t*)stmt, arguments ... );
}


#endif
