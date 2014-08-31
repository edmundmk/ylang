//
//  xec_seq_astvisitor.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq_astvisitor.h"
#include "xec_seq.h"





xec_seq_astvisitor::xec_seq_astvisitor( xec_seq* seq )
    :   seq( seq )
{
}

void xec_seq_astvisitor::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}

void xec_seq_astvisitor::visit( xec_ast_func* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_null* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_bool* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_number* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_string* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_local* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_global* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_upref* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_objref* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_key* node )
{
    visit( node->object );
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_inkey* node )
{
    visit( node->object );
    visit( node->key );
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_index* node )
{
    visit( node->object );
    visit( node->index );
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_preop* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_postop* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_unary* node )
{
    visit( node->operand );
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_binary* node )
{
    visit( node->lhs );
    visit( node->rhs );
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_compare* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_logical* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_qmark* node )
{
}

void xec_seq_astvisitor::visit( xec_new_new* node )
{
}

void xec_seq_astvisitor::visit( xec_new_object* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_new_array* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_new_table* node )
{
    seq_eval( node );
}

void xec_seq_astvisitor::visit( xec_expr_mono* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_call* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_yield* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_vararg* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_unpack* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_list* node )
{
}

void xec_seq_astvisitor::visit( xec_expr_assign* node )
{
    // Evaluate expressions in the lvalue before evaluating the rvalue.
    lvalue( node->lvalue );


    // If it's a compound assignment then create a fake AST node to perform
    // the operation.
    xec_seqop_kind seqkind = XEC_SEQ_ASSIGN;
    xec_ast_opkind opkind = XEC_ASTOP_ASSIGN;
    switch ( node->assignop )
    {
    case XEC_ASTOP_MULASSIGN:       opkind = XEC_ASTOP_MULTIPLY;    break;
    case XEC_ASTOP_DIVASSIGN:       opkind = XEC_ASTOP_DIVIDE;      break;
    case XEC_ASTOP_MODASSIGN:       opkind = XEC_ASTOP_MODULUS;     break;
    case XEC_ASTOP_INTDIVASSIGN:    opkind = XEC_ASTOP_INTDIV;      break;
    case XEC_ASTOP_ADDASSIGN:       opkind = XEC_ASTOP_ADD;         break;
    case XEC_ASTOP_SUBASSIGN:       opkind = XEC_ASTOP_SUBTRACT;    break;
    case XEC_ASTOP_LSHIFTASSIGN:    opkind = XEC_ASTOP_LSHIFT;      break;
    case XEC_ASTOP_LRSHIFTASSIGN:   opkind = XEC_ASTOP_LRSHIFT;     break;
    case XEC_ASTOP_ARSHIFTASSIGN:   opkind = XEC_ASTOP_ARSHIFT;     break;
    case XEC_ASTOP_BITANDASSIGN:    opkind = XEC_ASTOP_BITAND;      break;
    case XEC_ASTOP_BITXORASSIGN:    opkind = XEC_ASTOP_BITXOR;      break;
    case XEC_ASTOP_BITORASSIGN:     opkind = XEC_ASTOP_BITOR;       break;

    case XEC_ASTOP_DECLARE:
        seqkind = XEC_SEQ_DECLARE;
        break;
    
    case XEC_ASTOP_ASSIGN:
        break;
    
    default:
        assert( ! "invalid op" );
        break;
    }
    
    xec_ast_node* rvalue = node->rvalue;
    if ( opkind != XEC_ASTOP_ASSIGN )
    {
        rvalue = new ( seq->alloc ) xec_expr_binary(
                        node->sloc, opkind, node->lvalue, node->rvalue );
    }
    
    // Add rvalue to sequence.
    visit( rvalue );
    

    // Add assignment.
    seq->ops.emplace_back( seqkind, node->lvalue, rvalue );


    // 'Value' of assignment is the rvalue.
    size_t index = seq->nodemap.at( rvalue );
    seq->nodemap.emplace( node, index );
    
}

void xec_seq_astvisitor::visit( xec_expr_assign_list* node )
{

}





void xec_seq_astvisitor::lvalue( xec_ast_node* lvalue )
{
    // Visit the expressions that are required for the lvalue.

    switch ( lvalue->kind )
    {
    case XEC_EXPR_LOCAL:
    case XEC_EXPR_GLOBAL:
    case XEC_EXPR_UPREF:
        // No subexpressions.
        break;
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* keyexpr = (xec_expr_key*)lvalue;
        visit( keyexpr->object );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)lvalue;
        visit( inkey->object );
        visit( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)lvalue;
        visit( index->object );
        visit( index->index );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}



void xec_seq_astvisitor::seq_eval( xec_ast_node* node )
{
    auto i = seq->nodemap.find( node );
    if ( i != seq->nodemap.end() )
    {
        // Node already evaluated, don't re-evaluate it.
        xec_seqop* op = &seq->ops.at( i->second );
        op->rcount += 1;
        return;
    }
    
    size_t index = seq->ops.size();
    seq->ops.emplace_back( XEC_SEQ_EVAL, node );
    seq->nodemap.emplace( node, index );
}









