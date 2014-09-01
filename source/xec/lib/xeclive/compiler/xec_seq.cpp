//
//  xec_seq.cpp
//  xob
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq.h"
#include "xec_seq_astvisitors.h"



xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_node* astnode )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   astnode( astnode )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_name* name )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   name( name )
    ,   b( NULL )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                const char* k )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   k( k )
    ,   v( NULL )
{
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( NULL )
    ,   v( NULL )
{
    a->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, const char* k )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   k( k )
    ,   v( NULL )
{
    a->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, xec_seq_op* b )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( b )
    ,   v( NULL )
{
    a->refcount += 1;
    b->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_ast_name* name, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   name( name )
    ,   b( NULL )
    ,   v( v )
{
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                const char* k, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( NULL )
    ,   k( k )
    ,   v( v )
{
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, const char* k, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   k( k )
    ,   v( v )
{
    a->refcount += 1;
    v->refcount += 1;
}

xec_seq_op::xec_seq_op( int sloc, xec_seq_opcode opcode,
                xec_seq_op* a, xec_seq_op* b, xec_seq_op* v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   refcount( 0 )
    ,   a( a )
    ,   b( b )
    ,   v( v )
{
    a->refcount += 1;
    b->refcount += 1;
    v->refcount += 1;
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

xec_seq_op* xec_seq_builder::lvalue_assign(
                xec_seq_lvalue* lvalue, xec_seq_op* v )
{
    switch ( lvalue->opcode )
    {
    case XEC_SEQ_SETVAR:
        return op( lvalue->sloc, XEC_SEQ_SETVAR, lvalue->name, v );
    
    case XEC_SEQ_SETGLOBAL:
        return op( lvalue->sloc, XEC_SEQ_SETGLOBAL, lvalue->k, v );
        
    case XEC_SEQ_SETKEY:
        return op( lvalue->sloc, XEC_SEQ_SETKEY, lvalue->a, lvalue->k, v );
    
    case XEC_SEQ_SETINKEY:
        return op( lvalue->sloc, XEC_SEQ_SETINKEY, lvalue->a, lvalue->b, v );
        
    case XEC_SEQ_SETINDEX:
        return op( lvalue->sloc, XEC_SEQ_SETINDEX, lvalue->a, lvalue->b, v );
        break;
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}









/*


class xec_seq_printnode : public xec_ast_visitor< xec_seq_printnode, void >
{
public:


    using xec_ast_visitor< xec_seq_printnode, void >::visit;
    
    void fallback( xec_ast_node* node )
    {
        printf( "!!" );
    }
    
    
    void visit( xec_ast_func* node )
    {
        printf( "%p func %s", node, node->funcname );
    }
    
    void visit( xec_expr_null* node )
    {
        printf( "%p null", node );
    }
    
    void visit( xec_expr_bool* node )
    {
        printf( "%p %s", node, node->value ? "true" : "false" );
    }
    
    void visit( xec_expr_number* node )
    {
        printf( "%p %g", node, node->value );
    }
    
    void visit( xec_expr_string* node )
    {
        printf( "%p \"%.5s\"", node, node->string );
    }
    
    void visit( xec_expr_local* node )
    {
        printf( "%p %s", node, node->name->name );
    }
    
    void visit( xec_expr_global* node )
    {
        printf( "%p global %s", node, node->name );
    }
    
    void visit( xec_expr_upref* node )
    {
        printf( "%p upval %d", node, node->index );
    }
    
    void visit( xec_expr_objref* node )
    {
        printf( "%p objref %p", node, node->object );
    }
    
    void visit( xec_expr_key* node )
    {
        printf( "%p [%p].%s", node, node->object, node->key );
    }
    
    void visit( xec_expr_inkey* node )
    {
        printf( "%p [%p].[ [%p] ]", node, node->object, node->key );
    }
    
    void visit( xec_expr_index* node )
    {
        printf( "%p [%p][ [%p] ]", node, node->object, node->index );
    }
    
    void visit( xec_expr_preop* node )
    {
        printf( "%p %s[%p]", node,
                xec_operator_name( node->opkind ), node->lvalue );
    }
    
    void visit( xec_expr_postop* node )
    {
        printf( "%p [%p]%s", node,
                node->lvalue, xec_operator_name( node->opkind ) );
    }
    
    void visit( xec_expr_unary* node )
    {
        printf( "%p %s[%p]", node,
                xec_operator_name( node->opkind ), node->operand );
    }
    
    void visit( xec_expr_binary* node )
    {
        printf( "%p [%p] %s [%p]", node,
                node->lhs, xec_operator_name( node->opkind ), node->rhs );
    }
    
    void visit( xec_expr_compare* node )
    {
        printf( "%p compare ...", node );
    }
    
    void visit( xec_expr_logical* node )
    {
        printf( "%p [%p] %s [%p]", node,
                node->lhs, xec_operator_name( node->opkind ), node->rhs );
    }
    
    void visit( xec_expr_qmark* node )
    {
        printf( "%p [%p] ? [%p] : [%p]", node,
                node->condition, node->iftrue, node->iffalse );
    }
    
    void visit( xec_new_new* node )
    {
        printf( "%p new ...", node );
    }
    
    void visit( xec_new_object* node )
    {
        printf( "%p object ...", node );
    }
    
    void visit( xec_new_array* node )
    {
        printf( "%p array ...", node );
    }

    void visit( xec_new_table* node )
    {
        printf( "%p table ...", node );
    }
    
    void visit( xec_expr_mono* node )
    {
        printf( "%p mono ...", node );
    }

    void visit( xec_expr_call* node )
    {
        printf( "%p call ...", node );
    }
    
    void visit( xec_expr_yield* node )
    {
        printf( "%p yield ...", node );
    }
    
    void visit( xec_expr_vararg* node )
    {
        printf( "%p vararg ...", node );
    }
    
    void visit( xec_expr_unpack* node )
    {
        printf( "%p unpack ...", node );
    }
    
    void visit( xec_expr_list* node )
    {
        printf( "%p list ...", node );
    }
    
    void visit( xec_expr_assign* node )
    {
        printf( "%p assign ...", node );
    }
    
    void visit( xec_expr_assign_list* node )
    {
        printf( "%p assign-list ...", node );
    }

};




static void print_astnode( xec_ast_node* node )
{
    xec_seq_printnode p;
    p.visit( node );
}

*/

void xec_seq_print( xec_seq* seq )
{

/*
    for ( size_t i = 0; i < seq->ops.size(); ++i )
    {
        xec_seq_op* op = &seq->ops.at( i );
        
        switch ( op->kind )
        {
        case XEC_SEQ_EVAL:
        {
            printf( "[%d] eval ", op->rcount );
            print_astnode( op->rvalue );
            printf( "\n" );
            break;
        }
        
        case XEC_SEQ_ASSIGN:
        {
            printf( "[%d] assign ", op->rcount );
            print_astnode( op->lvalue );
            printf( " = %p\n", op->rvalue );
            break;
        }
        
        case XEC_SEQ_DECLARE:
        {
            printf( "[%d] declare ", op->rcount );
            print_astnode( op->lvalue );
            printf( " = %p\n", op->rvalue );
            break;
        }
        }
    }
*/
}

