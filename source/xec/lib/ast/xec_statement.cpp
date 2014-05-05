//
//  xec_statement.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_statement.h"
#include "xec_declaration.h"
#include "xec_expression.h"
#include "xec_token.h"




xec_statement::~xec_statement()
{
}



/*
    decl;
*/

xec_statement_declaration::xec_statement_declaration( xec_declaration* decl )
    :   decl( decl )
{
}

xec_statement_dispatch xec_statement_declaration::visitor_dispatch()
{
    return XEC_STATEMENT_DECLARATION;
}

int xec_statement_declaration::get_location()
{
    return decl->get_location();
}

xec_declaration* xec_statement_declaration::get_decl()
{
    return decl.get();
}



/*
    expr, expr;
*/

xec_statement_expression::xec_statement_expression( xec_expression* expr )
    :   expr( expr )
{
}

xec_statement_dispatch xec_statement_expression::visitor_dispatch()
{
    return XEC_STATEMENT_EXPRESSION;
}

int xec_statement_expression::get_location()
{
    return expr->get_location();
}

xec_expression* xec_statement_expression::get_expr()
{
    return expr.get();
}



/*
    { stmt; stmt; }
*/

xec_statement_compound::xec_statement_compound()
    :   token( NULL )
{
}

xec_statement_dispatch xec_statement_compound::visitor_dispatch()
{
    return XEC_STATEMENT_COMPOUND;
}

int xec_statement_compound::get_location()
{
    if ( token )
        return token->sloc;
    else if ( statements.size() )
        return statements.at( 0 )->get_location();
    else
        return 0;
}

void xec_statement_compound::set_token( xec_token* token )
{
    this->token = token;
}

void xec_statement_compound::append_statement( xec_statement* stmt )
{
    statements.push_back( std::unique_ptr< xec_statement >( stmt ) );
}

size_t xec_statement_compound::get_count()
{
    return statements.size();
}

xec_statement* xec_statement_compound::get_stmt( size_t index )
{
    return statements.at( index ).get();
}




/*
    delete expr;
*/

xec_statement_delete::xec_statement_delete(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

xec_statement_dispatch xec_statement_delete::visitor_dispatch()
{
    return XEC_STATEMENT_DELETE;
}

int xec_statement_delete::get_location()
{
    return token->sloc;
}

xec_expression_list* xec_statement_delete::get_expr_list()
{
    return expr_list.get();
}



/*
    if ( expr ) stmt; else stmt;
    if ( condition ) stmt; else stmt;
*/

xec_statement_if::xec_statement_if( xec_token* token, xec_expression* expr,
                    xec_statement* iftrue, xec_statement* iffalse )
    :   token( token )
    ,   expr( expr )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}

xec_statement_dispatch xec_statement_if::visitor_dispatch()
{
    return XEC_STATEMENT_IF;
}

int xec_statement_if::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_if::get_expr()
{
    return expr.get();
}

xec_statement* xec_statement_if::get_iftrue()
{
    return iftrue.get();
}

xec_statement* xec_statement_if::get_iffalse()
{
    return iffalse.get();
}



/*
    switch ( expr ) { stmt; stmt; }
    switch ( condition ) { stmt; stmt; }
*/

xec_statement_switch::xec_statement_switch( xec_token* token,
                    xec_expression* expr, xec_statement_compound* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

xec_statement_dispatch xec_statement_switch::visitor_dispatch()
{
    return XEC_STATEMENT_SWITCH;
}

int xec_statement_switch::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_switch::get_expr()
{
    return expr.get();
}

xec_statement_compound* xec_statement_switch::get_body()
{
    return body.get();
}



/*
    case expr:
    default:
*/

xec_statement_case::xec_statement_case(
                            xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

xec_statement_dispatch xec_statement_case::visitor_dispatch()
{
    return XEC_STATEMENT_CASE;
}

int xec_statement_case::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_case::get_expr()
{
    return expr.get();
}



/*
    while ( expr ) stmt;
    while ( condition ) stmt;
*/

xec_statement_while::xec_statement_while(
            xec_token* token, xec_expression* expr, xec_statement* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

xec_statement_dispatch xec_statement_while::visitor_dispatch()
{
    return XEC_STATEMENT_WHILE;
}

int xec_statement_while::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_while::get_expr()
{
    return expr.get();
}

xec_statement* xec_statement_while::get_body()
{
    return body.get();
}



/*
    do stmt; while ( expr );
*/

xec_statement_do::xec_statement_do(
            xec_token* token, xec_statement* body, xec_expression* expr )
    :   token( token )
    ,   body( body )
    ,   expr( expr )
{
}

xec_statement_dispatch xec_statement_do::visitor_dispatch()
{
    return XEC_STATEMENT_DO;
}

int xec_statement_do::get_location()
{
    return token->sloc;
}

xec_statement* xec_statement_do::get_body()
{
    return body.get();
}

xec_expression* xec_statement_do::get_expr()
{
    return expr.get();
}




/*
    for ( expr, expr : expr ) stmt;
    for ( expr, expr :: expr ) stmt;
    for ( condition : expr ) stmt;
    for ( condition :: expr ) stmt;
*/

xec_statement_foreach::xec_statement_foreach( xec_token* token,
            xec_expression_list* expr_list, xec_expression* expr,
                    xec_statement* body )
    :   token( token )
    ,   expr_list( expr_list )
    ,   expr( expr )
    ,   body( body )
    ,   eachkey( false )
    ,   declare( false )
{
}

xec_statement_dispatch xec_statement_foreach::visitor_dispatch()
{
    return XEC_STATEMENT_FOREACH;
}

int xec_statement_foreach::get_location()
{
    return token->sloc;
}

void xec_statement_foreach::set_eachkey( bool eachkey )
{
    this->eachkey = eachkey;
}

void xec_statement_foreach::set_declare( bool declare )
{
    this->declare = declare;
}

bool xec_statement_foreach::get_eachkey()
{
    return eachkey;
}

bool xec_statement_foreach::get_declare()
{
    return declare;
}

xec_expression_list* xec_statement_foreach::get_expr_list()
{
    return expr_list.get();
}

xec_expression* xec_statement_foreach::get_expr()
{
    return expr.get();
}

xec_statement* xec_statement_foreach::get_body()
{
    return body.get();
}




/*
    for ( expr; expr; expr ) stmt;
    for ( condition; expr; expr ) stmt;
*/

xec_statement_for::xec_statement_for( xec_token* token, xec_expression* init,
            xec_expression* expr, xec_expression* update, xec_statement* body )
    :   token( token )
    ,   init( init )
    ,   expr( expr )
    ,   update( update )
    ,   body( body )
{
}

xec_statement_dispatch xec_statement_for::visitor_dispatch()
{
    return XEC_STATEMENT_FOR;
}

int xec_statement_for::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_for::get_init()
{
    return init.get();
}

xec_expression* xec_statement_for::get_expr()
{
    return expr.get();
}

xec_expression* xec_statement_for::get_update()
{
    return update.get();
}

xec_statement* xec_statement_for::get_body()
{
    return body.get();
}



/*
    continue;
*/

xec_statement_continue::xec_statement_continue( xec_token* token )
    :   token( token )
{
}

xec_statement_dispatch xec_statement_continue::visitor_dispatch()
{
    return XEC_STATEMENT_CONTINUE;
}

int xec_statement_continue::get_location()
{
    return token->sloc;
}



/*
    break;
*/

xec_statement_break::xec_statement_break( xec_token* token )
    :   token( token )
{
}

xec_statement_dispatch xec_statement_break::visitor_dispatch()
{
    return XEC_STATEMENT_BREAK;
}

int xec_statement_break::get_location()
{
    return token->sloc;
}



/*
    return;
    return expr, expr;
*/

xec_statement_return::xec_statement_return(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

xec_statement_dispatch xec_statement_return::visitor_dispatch()
{
    return XEC_STATEMENT_RETURN;
}

int xec_statement_return::get_location()
{
    return token->sloc;
}

xec_expression_list* xec_statement_return::get_expr_list()
{
    return expr_list.get();
}



/*
    yield;
    yield expr, expr;
*/

xec_statement_yield::xec_statement_yield(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

xec_statement_dispatch xec_statement_yield::visitor_dispatch()
{
    return XEC_STATEMENT_YIELD;
}

int xec_statement_yield::get_location()
{
    return token->sloc;
}

xec_expression_list* xec_statement_yield::get_expr_list()
{
    return expr_list.get();
}



/*
    using expr, expr;
    using condition;
*/

xec_statement_using::xec_statement_using(
                    xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

xec_statement_dispatch xec_statement_using::visitor_dispatch()
{
    return XEC_STATEMENT_USING;
}

int xec_statement_using::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_using::get_expr()
{
    return expr.get();
}



/*
    using ( expr, expr ) stmt;
    using ( condition ) stmt;
*/

xec_statement_usingscope::xec_statement_usingscope(
            xec_token* token, xec_expression* expr, xec_statement* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

xec_statement_dispatch xec_statement_usingscope::visitor_dispatch()
{
    return XEC_STATEMENT_USINGSCOPE;
}

int xec_statement_usingscope::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_usingscope::get_expr()
{
    return expr.get();
}

xec_statement* xec_statement_usingscope::get_body()
{
    return body.get();
}



/*
    try stmt; catch ( : expr ) stmt; finally stmt;
    try stmt; catch ( expr : expr ) stmt; finally stmt;
    try stmt; catch ( var name : expr ) stmt; finally stmt;
*/

xec_statement_try::xec_statement_try()
    :   token( NULL )
    ,   ftoken( NULL )
{
}

xec_statement_dispatch xec_statement_try::visitor_dispatch()
{
    return XEC_STATEMENT_TRY;
}

int xec_statement_try::get_location()
{
    return token->sloc;
}

void xec_statement_try::set_body( xec_token* token, xec_statement* body )
{
    this->token = token;
    this->body.reset( body );
}

void xec_statement_try::append_catch( xec_statement_catch* cstmt )
{
    catches.push_back( std::unique_ptr< xec_statement_catch >( cstmt ) );
}

void xec_statement_try::set_finally( xec_token* token, xec_statement* final )
{
    this->ftoken = token;
    this->final.reset( final );
}

xec_statement* xec_statement_try::get_body()
{
    return body.get();
}

size_t xec_statement_try::get_catch_count()
{
    return catches.size();
}

xec_statement_catch* xec_statement_try::get_catch( size_t index )
{
    return catches.at( index ).get();
}

xec_statement* xec_statement_try::get_final()
{
    return final.get();
}




/*
    catch ( : expr ) stmt;
    catch ( name : expr ) stmt;
    catch ( var name : expr ) stmt;
*/

xec_statement_catch::xec_statement_catch( xec_token* token,
          xec_expression* lvalue, xec_expression* proto, xec_statement* body )
    :   token( token )
    ,   lvalue( lvalue )
    ,   proto( proto )
    ,   body( body )
{
}

xec_statement_dispatch xec_statement_catch::visitor_dispatch()
{
    return XEC_STATEMENT_CATCH;
}

int xec_statement_catch::get_location()
{
    return token->sloc;
}

void xec_statement_catch::set_declare( bool declare )
{
    this->declare = declare;
}

bool xec_statement_catch::get_declare()
{
    return declare;
}

xec_expression* xec_statement_catch::get_lvalue()
{
    return lvalue.get();
}

xec_expression* xec_statement_catch::get_proto()
{
    return proto.get();
}

xec_statement* xec_statement_catch::get_body()
{
    return body.get();
}



/*
    throw expr;
*/

xec_statement_throw::xec_statement_throw(
                xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

xec_statement_dispatch xec_statement_throw::visitor_dispatch()
{
    return XEC_STATEMENT_THROW;
}

int xec_statement_throw::get_location()
{
    return token->sloc;
}

xec_expression* xec_statement_throw::get_expr()
{
    return expr.get();
}




