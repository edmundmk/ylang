//
//  xec_parser.cpp
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_parser.h"



xec_parser::xec_parser( xec_script* script )
    :   script( script )
{
}



xec_script* xec_parser::get_script()
{
    return script;
}



double xec_parser::parse_number( xec_token* token )
{
    return 0;
}


xec_ast_node* xec_parser::expr_name( xec_ast_node* name )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_proto( xec_ast_node* proto )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_call( xec_ast_node* expr, xec_ast_node* args )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_compare(
        xec_token* token, xec_ast_node* lhs, xec_ast_node* rhs )
{
    return NULL;
}



xec_ast_node* xec_parser::expr_append( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->values.push_back( expr );
    return l;
}


xec_ast_node* xec_parser::expr_final( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->unpack = expr;
    return l;
}



xec_ast_node* xec_parser::expr_lvalue( xec_ast_node* lvalue )
{
    return NULL;
}

void xec_parser::expr_lvalue_list(
        xec_expr_list* list, xec_ast_node_list* lvalues )
{
}




xec_ast_node* xec_parser::expr_assign(
        xec_token* op, xec_ast_node* lv, xec_ast_node* rv )
{
    if ( lv->kind == XEC_EXPR_LIST )
    {
        xec_ast_assign_list* a;
        a = alloc< xec_ast_assign_list >( lv->sloc, op->kind );
        expr_lvalue_list( (xec_expr_list*)lv, &a->lvalues );
        a->rvalues = rv;
        return a;
    }
    else
    {
        xec_ast_assign* a;
        a = alloc< xec_ast_assign >( lv->sloc, op->kind );
        a->lvalue = expr_lvalue( lv );
        a->rvalue = rv;
        return a;
    }
}