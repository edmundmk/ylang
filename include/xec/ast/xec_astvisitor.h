//
//  xec_astvisitor.h
//
//  Created by Edmund Kapusniak on 01/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_ASTVISITOR_H
#define XEC_ASTVISITOR_H


class xec_declaration;
class xec_declaration_var;
class xec_declaration_object;
class xec_declaration_prototype;
class xec_declaration_function;

class xec_expression;
class xec_expression_null;
class xec_expression_number;
class xec_expression_string;
class xec_expression_identifier;
class xec_expression_lookup;
class xec_expression_indexkey;
class xec_expression_index;
class xec_expression_yield;
class xec_expression_call;
class xec_expression_unary;
class xec_expression_binary;
class xec_expression_comparison;
class xec_expression_logical;
class xec_expression_conditional;
class xec_expression_varargs;
class xec_expression_unpack;
class xec_expression_list;
class xec_expression_assign;
class xec_expression_mono;
class xec_expression_declare;
class xec_constructor_new;
class xec_constructor_list;
class xec_constructor_table;
class xec_constructor_function;
class xec_constructor_object;

class xec_statement;
class xec_statement_declaration;
class xec_statement_expression;
class xec_statement_compound;
class xec_statement_delete;
class xec_statement_if;
class xec_statement_switch;
class xec_statement_case;
class xec_statement_while;
class xec_statement_do;
class xec_statement_foreach;
class xec_statement_for;
class xec_statement_continue;
class xec_statement_break;
class xec_statement_return;
class xec_statement_yield;
class xec_statement_using;
class xec_statement_usingscope;
class xec_statement_try;
class xec_statement_catch;
class xec_statement_throw;


enum xec_declaration_dispatch
{
    XEC_DECLARATION_VAR,
    XEC_DECLARATION_OBJECT,
    XEC_DECLARATION_PROTOTYPE,
    XEC_DECLARATION_FUNCTION,
};


enum xec_expression_dispatch
{
    XEC_EXPRESSION_NULL,
    XEC_EXPRESSION_NUMBER,
    XEC_EXPRESSION_STRING,
    XEC_EXPRESSION_IDENTIFIER,
    XEC_EXPRESSION_LOOKUP,
    XEC_EXPRESSION_INDEXKEY,
    XEC_EXPRESSION_INDEX,
    XEC_EXPRESSION_YIELD,
    XEC_EXPRESSION_CALL,
    XEC_EXPRESSION_UNARY,
    XEC_EXPRESSION_BINARY,
    XEC_EXPRESSION_COMPARISON,
    XEC_EXPRESSION_LOGICAL,
    XEC_EXPRESSION_CONDITIONAL,
    XEC_EXPRESSION_VARARGS,
    XEC_EXPRESSION_UNPACK,
    XEC_EXPRESSION_LIST,
    XEC_EXPRESSION_ASSIGN,
    XEC_EXPRESSION_MONO,
    XEC_EXPRESSION_DECLARE,
    XEC_CONSTRUCTOR_NEW,
    XEC_CONSTRUCTOR_LIST,
    XEC_CONSTRUCTOR_TABLE,
    XEC_CONSTRUCTOR_OBJECT,
    XEC_CONSTRUCTOR_FUNCTION,
};


enum xec_statement_dispatch
{
    XEC_STATEMENT_DECLARATION,
    XEC_STATEMENT_EXPRESSION,
    XEC_STATEMENT_COMPOUND,
    XEC_STATEMENT_DELETE,
    XEC_STATEMENT_IF,
    XEC_STATEMENT_SWITCH,
    XEC_STATEMENT_CASE,
    XEC_STATEMENT_WHILE,
    XEC_STATEMENT_DO,
    XEC_STATEMENT_FOREACH,
    XEC_STATEMENT_FOR,
    XEC_STATEMENT_CONTINUE,
    XEC_STATEMENT_BREAK,
    XEC_STATEMENT_RETURN,
    XEC_STATEMENT_YIELD,
    XEC_STATEMENT_USING,
    XEC_STATEMENT_USINGSCOPE,
    XEC_STATEMENT_TRY,
    XEC_STATEMENT_CATCH,
    XEC_STATEMENT_THROW,
};



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
    
    return ( this->*( dispatch[ 0 ] ) )( decl, arguments ... );
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
    
    return ( this->*( dispatch[ 0 ] ) )( expr, arguments ... );
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
    
    return ( this->*( dispatch[ 0 ] ) )( stmt, arguments ... );
}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename statement_t >
return_t xec_astvisitor< visitor_t, return_t, arguments_t ... >::dispatch_stmt( xec_statement* stmt, arguments_t ... arguments )
{
    return visitor_t::visit( (statement_t*)stmt, arguments ... );
}


#endif
