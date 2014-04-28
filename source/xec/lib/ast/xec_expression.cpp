//
//  xec_expression.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_expression.h"
#include <stddef.h>



xec_expression::~xec_expression()
{
}
    
xec_expression_list* xec_expression::as_list()
{
    return NULL;
}

xec_expression* xec_expression::as_mono()
{
    return NULL;
}

xec_expression_comparison* xec_expression::as_comparison()
{
    return NULL;
}




xec_expression_null::xec_expression_null( xec_token* token )
{
}




xec_expression_bool::xec_expression_bool( xec_token* token )
{
}



xec_expression_number::xec_expression_number( xec_token* token )
{
}




xec_expression_string::xec_expression_string( xec_token* token )
{
}




xec_expression_identifier::xec_expression_identifier( xec_token* token )
{
}



xec_expression_lookup::xec_expression_lookup(
                                xec_expression* expr, xec_token* token )
{
}





xec_expression_indexkey::xec_expression_indexkey(
                                xec_expression* expr, xec_expression* index )
{
}





xec_expression_index::xec_expression_index(
                                xec_expression* expr, xec_expression* index )
{
}
    





xec_expression_yield::xec_expression_yield(
                                xec_token* token, xec_expression_list* args )
{
}
    
void xec_expression_yield::set_unpack( bool unpack )
{
}






xec_expression_call::xec_expression_call(
                        xec_expression* expr, xec_expression_list* args )
{
}

void xec_expression_call::set_yieldcall( bool yieldcall )
{
    
}

void xec_expression_call::set_unpack( bool unpack )
{
}

xec_declaration_prototype* xec_expression_call::as_prototype()
{
    return NULL;
}

xec_declaration_function* xec_expression_call::as_function()
{
    return NULL;
}





xec_expression_unary::xec_expression_unary(
                                    xec_expression* expr, xec_token* token )
{
}






xec_expression_binary::xec_expression_binary(
        xec_expression* expr_a, xec_token* token, xec_expression* expr_b )
{
}




xec_expression_comparison::xec_expression_comparison( xec_expression* expr )
{
}
    
void xec_expression_comparison::add_comparison(
                                xec_token* token, xec_expression* expr )
{
}




xec_expression_logical::xec_expression_logical(
        xec_expression* expr_a, xec_token* token, xec_expression* expr_b )
{
}




xec_expression_conditional::xec_expression_conditional(
        xec_expression* condition,
        xec_expression* iftrue,
        xec_expression* iffalse )
{
}




xec_expression_varargs::xec_expression_varargs( xec_token* token )
{
}





xec_expression_unpack::xec_expression_unpack( xec_expression* expr )
{
}




xec_expression_list::xec_expression_list()
{
}

void xec_expression_list::append_expression( xec_expression* expression )
{
}

void xec_expression_list::append_final( xec_expression* expression )
{
}





xec_expression_assign::xec_expression_assign(
            xec_expression* lvalue, xec_token* op, xec_expression* rvalue )
{
}




// xec_expression_mono




xec_expression_condition::xec_expression_condition(
            xec_token* token,
            xec_expression_list* name_list,
            xec_expression_list* expr_list )
{
}





