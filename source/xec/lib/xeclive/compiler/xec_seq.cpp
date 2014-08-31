//
//  xec_seq.cpp
//  xob
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq.h"
#include "xec_seq_astvisitor.h"



xec_seqop::xec_seqop( xec_seqop_kind kind, xec_ast_node* node )
    :   kind( kind )
    ,   rcount( 1 )
    ,   lvalue( NULL )
    ,   rvalue( node )
{
}

xec_seqop::xec_seqop( xec_seqop_kind kind,
                xec_ast_node* lval, xec_ast_node* rval )
    :   kind( kind )
    ,   rcount( 1 )
    ,   lvalue( lval )
    ,   rvalue( rval )
{
}



void xec_seq_build( xec_seq* seq, xec_ast_node* node )
{
    region_scope rscope( seq->alloc );
    xec_seq_astvisitor v( seq );
    v.visit( node );
}







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



void xec_seq_print( xec_seq* seq )
{
    for ( size_t i = 0; i < seq->ops.size(); ++i )
    {
        xec_seqop* op = &seq->ops.at( i );
        
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
}

