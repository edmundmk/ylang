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



xec_ast_scope* xec_parser::get_scope()
{
    return scopes.back();
}


xec_ast_scope* xec_parser::block_scope( xec_ast_node* node )
{
    xec_ast_scope* outer = get_scope();
    xec_ast_scope* scope = alloc< xec_ast_scope >(
                    XEC_SCOPE_BLOCK, outer, node, outer->func );
    scopes.push_back( scope );
    return scope;
}


xec_ast_scope* xec_parser::object_scope( xec_ast_node* name )
{
    // TODO.
    return NULL;
}


xec_ast_func* xec_parser::function( int sloc, xec_ast_node* name,
                xec_expr_list* params, bool coroutine, bool thisdot )
{
    // TODO.
    return NULL;
}


void xec_parser::close_scope( xec_ast_scope* scope )
{
    assert( scope == get_scope() );
    scopes.pop_back();
}


xec_ast_name* xec_parser::declare( xec_unqual_name* name )
{
    // TODO.
    return NULL;
}


void xec_parser::declare_list( xec_unqual_list* list, xec_ast_name_list* names )
{
    // TODO.
}


void xec_parser::statement( xec_ast_node* stmt )
{
    xec_ast_scope* scope = get_scope();
    assert( scope->block );
    scope->block->stmts.push_back( stmt );
}


void xec_parser::objmember( xec_ast_node* decl )
{
    // TODO.
}




xec_ast_node* xec_parser::continue_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope->node;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    script->diagnostic( sloc, "continue outside loop" );
    return NULL;
}


xec_ast_node* xec_parser::break_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_SWITCH
                || scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope->node;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    script->diagnostic( sloc, "break outside loop or switch" );
    return NULL;
}





xec_ast_node* xec_parser::resolve( xec_ast_node* name )
{
    // A name or proto is used in an expression.  Perform lookup of unqualified
    // names, and replace protos with call expressions.

    return NULL;
}


xec_expr_call* xec_parser::resolve( xec_unqual_proto* name )
{
    // A name or proto is used in an expression.  Perform lookup of unqualified
    // names, and replace protos with call expressions.

    return NULL;
}




xec_ast_node* xec_parser::compare(
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



xec_ast_node* xec_parser::append( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->values.push_back( expr );
    return l;
}


xec_ast_node* xec_parser::final( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->final = expr;
    return l;
}


xec_expr_list* xec_parser::list( xec_ast_node* list )
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




xec_ast_node* xec_parser::lvalue( xec_ast_node* lvalue )
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


void xec_parser::lvalue_list(
        xec_ast_node* list, xec_ast_node_list* lvalues )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)list;
        for ( size_t i = 0; i < l->values.size(); ++l )
            lvalues->push_back( lvalue( l->values[ i ] ) );
        if ( l->final )
            lvalues->push_back( lvalue( l->final ) );
    }
    else
    {
        lvalues->push_back( lvalue( list ) );
    }
}



xec_ast_node* xec_parser::delval( xec_ast_node* delval )
{
    // Expression must be a single deletable lvalue.
    
    switch ( delval->kind )
    {
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
    {
        break;
    }
    
    default:
    {
        script->diagnostic( delval->sloc, "expression is not deletable" );
        break;
    }
    }
    
    return delval;
}


void xec_parser::delval_list(
        xec_ast_node* list, xec_ast_node_list* delvals )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)list;
        for ( size_t i = 0; i < l->values.size(); ++l )
            delvals->push_back( delval( l->values[ i ] ) );
        if ( l->final )
            delvals->push_back( delval( l->final ) );
    }
    else
    {
        delvals->push_back( delval( list ) );
    }
}



xec_ast_node* xec_parser::assign(
        xec_token* op, xec_ast_node* lv, xec_ast_node* rv )
{
    if ( lv->kind == XEC_EXPR_LIST )
    {
        xec_ast_assign_list* a;
        a = alloc< xec_ast_assign_list >( lv->sloc, op->kind );
        lvalue_list( (xec_expr_list*)lv, &a->lvalues );
        a->rvalues = rv;
        return a;
    }
    else
    {
        xec_ast_assign* a;
        a = alloc< xec_ast_assign >( lv->sloc, op->kind );
        a->lvalue = lvalue( lv );
        a->rvalue = rv;
        return a;
    }
}



xec_ast_node* xec_parser::nodecl( xec_ast_node* stmt )
{
    switch ( stmt->kind )
    {
    case XEC_AST_DECLARE:
    case XEC_AST_DECLARE_LIST:
        script->diagnostic( stmt->sloc, "declaration is not allowed here" );
        break;
        
    default:
        break;
    }
    
    return stmt;
}




