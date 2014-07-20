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
    char* endp = NULL;
    double number = strtod( token->text, &endp );
    if ( endp[ 0 ] != '\0' )
    {
        script->diagnostic(
            token->sloc, "unable to parse number '%s'", token->text );
    }
    else if ( number == HUGE_VAL || number == -HUGE_VAL )
    {
        script->diagnostic(
            token->sloc, "number out of range '%s'", token->text );
    }
    return number;
}


xec_ast_node* xec_parser::expr_name( xec_ast_node* name )
{
    // A name is used in an expression.  Perform lookup of unqualified names.

    return NULL;
}


xec_ast_node* xec_parser::expr_proto( xec_ast_node* proto )
{
    // A prototype is used in an expression.  Perform lookup of unqualified
    // names and replace the proto with a call expression.

    return NULL;
}



xec_ast_node* xec_parser::expr_compare(
        xec_token* token, xec_ast_node* lhs, xec_ast_node* rhs )
{
    // Build a compare expression.
    xec_expr_compare* c;
    if ( lhs->kind == XEC_EXPR_COMPARE )
        c = (xec_expr_compare*)lhs;
    else
        c = alloc< xec_expr_compare >( lhs->sloc, lhs );
    
    c->opkinds.push_back( token->kind );
    c->terms.push_back( rhs );

    return c;
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
    l->final = expr;
    return l;
}


xec_expr_list* xec_parser::expr_list( xec_ast_node* list )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        return (xec_expr_list*)list;
    }
    else
    {
        xec_expr_list* l = alloc< xec_expr_list >( list->sloc );
        l->values.push_back( list );
        return l;
    }
}




xec_ast_node* xec_parser::expr_lvalue( xec_ast_node* lvalue )
{
    // The expression must be a single lvalue.  Only a restricted set of
    // expressions are lvalues, and global variables must be scoped using
    // 'global.'
    
    switch ( lvalue->kind )
    {
    case XEC_EXPR_LOCAL:
    case XEC_EXPR_UPVAL:
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
    case XEC_EXPR_INDEX:
    {
        break;
    }
    
    case XEC_EXPR_GLOBAL:
    {
        xec_expr_global* g = (xec_expr_global*)lvalue;
        if ( ! g->gexplicit )
            script->diagnostic( lvalue->sloc, "unexpected identifier" );
        break;
    }
    
    default:
    {
        script->diagnostic( lvalue->sloc, "invalid lvalue" );
        break;
    }
    }

    return lvalue;
}


void xec_parser::expr_lvalue_list(
        xec_ast_node* list, xec_ast_node_list* lvalues )
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