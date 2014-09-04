//
//  xec_seq_builder.cpp
//
//  Created by Edmund Kapusniak on 03/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq_builder.h"




xec_seq_lvalue::xec_seq_lvalue()
    :   sloc( -1 )
    ,   opcode( XEC_SEQ_NOP )
    ,   a( NULL )
    ,   b( NULL )
{
}



xec_seq_valist::xec_seq_valist()
    :   unpacked( NULL )
{
}




xec_seq_builder::xec_seq_builder( xec_seq* seq )
    :   seq( seq )
    ,   build_expr( this )
    ,   build_unpack( this )
{
}


xec_seq_op* xec_seq_builder::build( xec_ast_node* expr )
{
    return build_expr.visit( expr );
}


void xec_seq_builder::reserve( xec_seq_args* a, int acount, xec_seq_valist* l )
{
    a->values.reserve( acount + l->values.size() );
}

void xec_seq_builder::push( xec_seq_args* a, xec_seq_op* op )
{
    op->refcount += 1;
    a->values.push_back( op );
}

void xec_seq_builder::push( xec_seq_args* a, xec_seq_valist* l )
{
    for ( size_t i = 0; i < l->values.size(); ++i )
    {
        push( a, l->values.at( i ) );
    }
    a->unpacked = l->unpacked;
}


xec_ast_upval* xec_seq_builder::upval( xec_expr_upref* upref )
{
    xec_ast_func* func = upref->func;
    xec_ast_upval* uv = &func->upvals.at( upref->index );
    while ( uv->kind == XEC_UPVAL_UPVAL )
    {
        func = func->scope->outer->func;
        uv = &func->upvals.at( uv->upval );
    }
    return uv;
}


xec_seq_op* xec_seq_builder::expr( xec_ast_node* expr )
{
    return build_expr.visit( expr );
}


void xec_seq_builder::unpack( xec_seq_valist* l, xec_ast_node* node, int count )
{
    build_unpack.visit( node, l, count );
    if ( count != -1 )
    {
        assert( (int)l->values.size() == count );
        assert( ! l->unpacked );
    }
}


void xec_seq_builder::declare( xec_seq_lvalue* lvalue, xec_ast_node* node )
{
    lvalue->sloc = node->sloc;

    switch ( node->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        xec_expr_local* local = (xec_expr_local*)node;
        lvalue->opcode  = XEC_SEQ_DECLARE;
        lvalue->name    = local->name;
        break;
    }
            
    default:
        assert( ! "invalid declaration name" );
        break;
    }
}


void xec_seq_builder::lvalue( xec_seq_lvalue* lvalue, xec_ast_node* node )
{
    lvalue->sloc = node->sloc;

    switch ( node->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        xec_expr_local* local = (xec_expr_local*)node;
        lvalue->opcode  = XEC_SEQ_SETVAR;
        lvalue->name    = local->name;
        break;
    }
    
    case XEC_EXPR_GLOBAL:
    {
        xec_expr_global* global = (xec_expr_global*)node;
        lvalue->opcode  = XEC_SEQ_SETGLOBAL;
        lvalue->k       = global->name;
        break;
    }
    
    case XEC_EXPR_UPREF:
    {
        xec_expr_upref* upref = (xec_expr_upref*)node;
        xec_ast_upval* uv = upval( upref );
        assert( uv->kind == XEC_UPVAL_LOCAL );
        lvalue->opcode  = XEC_SEQ_SETVAR;
        lvalue->name    = uv->local;
        break;
    }
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* keyexpr = (xec_expr_key*)node;
        lvalue->opcode  = XEC_SEQ_SETKEY;
        lvalue->a       = expr( keyexpr->object );
        lvalue->k       = keyexpr->key;
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node;
        lvalue->opcode  = XEC_SEQ_SETINKEY;
        lvalue->a       = expr( inkey->object );
        lvalue->b       = expr( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)node;
        lvalue->opcode  = XEC_SEQ_SETINDEX;
        lvalue->a       = expr( index->object );
        lvalue->b       = expr( index->index );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

xec_seq_op* xec_seq_builder::lvalue_value( xec_seq_lvalue* lvalue )
{
    switch ( lvalue->opcode )
    {
    case XEC_SEQ_DECLARE:
        assert( ! "undeclared value" );
        break;
    
    case XEC_SEQ_SETVAR:
        return op( lvalue->sloc, XEC_SEQ_VAR, lvalue->name );
    
    case XEC_SEQ_SETGLOBAL:
        return op( lvalue->sloc, XEC_SEQ_GLOBAL, lvalue->k );
    
    case XEC_SEQ_SETKEY:
        return op( lvalue->sloc, XEC_SEQ_KEY, lvalue->a, lvalue->k );
    
    case XEC_SEQ_SETINKEY:
        return op( lvalue->sloc, XEC_SEQ_INKEY, lvalue->a, lvalue->b );
    
    case XEC_SEQ_SETINDEX:
        return op( lvalue->sloc, XEC_SEQ_INDEX, lvalue->a, lvalue->b );
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }
    
}

void xec_seq_builder::lvalue_assign(
                xec_seq_lvalue* lvalue, xec_seq_op* v )
{
    switch ( lvalue->opcode )
    {
    case XEC_SEQ_DECLARE:
        op( lvalue->sloc, XEC_SEQ_DECLARE, lvalue->name, v );
        break;
    
    case XEC_SEQ_SETVAR:
        op( lvalue->sloc, XEC_SEQ_SETVAR, lvalue->name, v );
        break;
        
    case XEC_SEQ_SETGLOBAL:
        op( lvalue->sloc, XEC_SEQ_SETGLOBAL, lvalue->k, v );
        break;
        
    case XEC_SEQ_SETKEY:
        op( lvalue->sloc, XEC_SEQ_SETKEY, lvalue->a, lvalue->k, v );
        break;
    
    case XEC_SEQ_SETINKEY:
        op( lvalue->sloc, XEC_SEQ_SETINKEY, lvalue->a, lvalue->b, v );
        break;
        
    case XEC_SEQ_SETINDEX:
        op( lvalue->sloc, XEC_SEQ_SETINDEX, lvalue->a, lvalue->b, v );
        break;
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}


