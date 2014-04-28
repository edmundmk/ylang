//
//  xec_statement.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_statement.h"





xec_statement::~xec_statement()
{
}




xec_statement_declaration::xec_statement_declaration( xec_declaration* decl )
{
}



xec_statement_expression::xec_statement_expression( xec_expression* expr )
{
}




xec_statement_compound::xec_statement_compound()
{
}
    
void xec_statement_compound::append_statement( xec_statement* stmt )
{
}





xec_statement_delete::xec_statement_delete(
                xec_token* token, xec_expression_list* expr_list )
{
}




xec_statement_if::xec_statement_if( xec_token* token, xec_expression* expr,
                    xec_statement* iftrue, xec_statement* iffalse )
{
}




xec_statement_switch::xec_statement_switch( xec_token* token,
                    xec_expression* expr, xec_statement_compound* body )
{
}




xec_statement_case::xec_statement_case( xec_token* token, xec_expression* expr )
{
}




xec_statement_while::xec_statement_while(
            xec_token* token, xec_expression* expr, xec_statement* body )
{
}




xec_statement_do::xec_statement_do(
            xec_token* token, xec_expression* expr, xec_statement* body )
{
}




xec_statement_foreach::xec_statement_foreach(
        xec_token* token,
        xec_expression_list* expr_list,
        xec_expression* expr,
        xec_statement* body )
{
}

void xec_statement_foreach::set_eachkey( bool eachkey )
{
}

void xec_statement_foreach::set_condition( bool condition )
{
}





xec_statement_for::xec_statement_for( xec_token* token, xec_expression* init,
        xec_expression* expr, xec_expression* update, xec_statement* body )
{
}




xec_statement_continue::xec_statement_continue( xec_token* token )
{
}




xec_statement_break::xec_statement_break( xec_token* token )
{
}





xec_statement_return::xec_statement_return(
                xec_token* token, xec_expression_list* expr_list )
{
}




xec_statement_yield::xec_statement_yield(
                xec_token* token, xec_expression_list* expr_list )
{
}




xec_statement_using::xec_statement_using(
                    xec_token* token, xec_expression* expr )
{
}



xec_statement_using_scope::xec_statement_using_scope(
            xec_token* token, xec_expression* expr, xec_statement* body )
{
}




xec_statement_try::xec_statement_try()
{
}
    
void xec_statement_try::set_body( xec_token* token, xec_statement* body )
{
}


void xec_statement_try::append_catch( xec_statement_catch* cstmt )
{
}

void xec_statement_try::set_finally( xec_token* token, xec_statement* final )
{
}



xec_statement_catch::xec_statement_catch( xec_token* token,
        xec_expression* lvalue, xec_expression* proto, xec_statement* body )
{
}

void xec_statement_catch::set_condition( bool condition )
{
}




xec_statement_throw::xec_statement_throw( xec_expression* expr )
{
}

