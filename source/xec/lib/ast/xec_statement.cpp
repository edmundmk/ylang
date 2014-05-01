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

int xec_statement::get_location()
{
    return -1;
}




xec_statement_declaration::xec_statement_declaration( xec_declaration* decl )
    :   decl( decl )
{
}

int xec_statement_declaration::get_location()
{
    return decl->get_location();
}



xec_statement_expression::xec_statement_expression( xec_expression* expr )
    :   expr( expr )
{
}

int xec_statement_expression::get_location()
{
    return expr->get_location();
}




xec_statement_compound::xec_statement_compound()
    :   token( NULL )
{
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





xec_statement_delete::xec_statement_delete(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

int xec_statement_delete::get_location()
{
    return token->sloc;
}




xec_statement_if::xec_statement_if( xec_token* token, xec_expression* expr,
                    xec_statement* iftrue, xec_statement* iffalse )
    :   token( token )
    ,   expr( expr )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}

int xec_statement_if::get_location()
{
    return token->sloc;
}




xec_statement_switch::xec_statement_switch( xec_token* token,
                    xec_expression* expr, xec_statement_compound* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

int xec_statement_switch::get_location()
{
    return token->sloc;
}




xec_statement_case::xec_statement_case(
                            xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

int xec_statement_case::get_location()
{
    return token->sloc;
}




xec_statement_while::xec_statement_while(
            xec_token* token, xec_expression* expr, xec_statement* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

int xec_statement_while::get_location()
{
    return token->sloc;
}




xec_statement_do::xec_statement_do(
            xec_token* token, xec_statement* body, xec_expression* expr )
    :   token( token )
    ,   body( body )
    ,   expr( expr )
{
}

int xec_statement_do::get_location()
{
    return token->sloc;
}





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





xec_statement_for::xec_statement_for( xec_token* token, xec_expression* init,
            xec_expression* expr, xec_expression* update, xec_statement* body )
    :   token( token )
    ,   init( init )
    ,   expr( expr )
    ,   update( update )
    ,   body( body )
{
}

int xec_statement_for::get_location()
{
    return token->sloc;
}



xec_statement_continue::xec_statement_continue( xec_token* token )
    :   token( token )
{
}

int xec_statement_continue::get_location()
{
    return token->sloc;
}




xec_statement_break::xec_statement_break( xec_token* token )
    :   token( token )
{
}

int xec_statement_break::get_location()
{
    return token->sloc;
}



xec_statement_return::xec_statement_return(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

int xec_statement_return::get_location()
{
    return token->sloc;
}




xec_statement_yield::xec_statement_yield(
                xec_token* token, xec_expression_list* expr_list )
    :   token( token )
    ,   expr_list( expr_list )
{
}

int xec_statement_yield::get_location()
{
    return token->sloc;
}



xec_statement_using::xec_statement_using(
                    xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

int xec_statement_using::get_location()
{
    return token->sloc;
}



xec_statement_usingscope::xec_statement_usingscope(
            xec_token* token, xec_expression* expr, xec_statement* body )
    :   token( token )
    ,   expr( expr )
    ,   body( body )
{
}

int xec_statement_usingscope::get_location()
{
    return token->sloc;
}



xec_statement_try::xec_statement_try()
    :   token( NULL )
    ,   ftoken( NULL )
{
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



xec_statement_catch::xec_statement_catch( xec_token* token,
          xec_expression* lvalue, xec_expression* proto, xec_statement* body )
    :   token( token )
    ,   lvalue( lvalue )
    ,   proto( proto )
    ,   body( body )
{
}

int xec_statement_catch::get_location()
{
    return token->sloc;
}

void xec_statement_catch::set_declare( bool declare )
{
    this->declare = declare;
}




xec_statement_throw::xec_statement_throw(
                xec_token* token, xec_expression* expr )
    :   token( token )
    ,   expr( expr )
{
}

int xec_statement_throw::get_location()
{
    return token->sloc;
}



