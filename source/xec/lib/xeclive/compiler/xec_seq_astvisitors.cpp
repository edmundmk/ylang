//
//  xec_seq_astvisitors.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq_astvisitors.h"
#include "xec_seq.h"



xec_seq_build_expr::xec_seq_build_expr( xec_seq_builder* b )
    :   b( b )
{
}


xec_seq_op* xec_seq_build_expr::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}


xec_seq_op* xec_seq_build_expr::visit( xec_ast_func* node )
{
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_null* node )
{
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_number* node )
{
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_string* node )
{
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_local* node )
{
    return b->op( node->sloc, XEC_SEQ_VAR, node->name );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_global* node )
{
    return b->op( node->sloc, XEC_SEQ_GLOBAL, node->name );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_upref* node )
{
    xec_ast_upval* uv = b->upval( node );

    if ( uv->kind == XEC_UPVAL_LOCAL )
    {
        return b->op( node->sloc, XEC_SEQ_VAR, uv->local );
    }
    else if ( uv->kind == XEC_UPVAL_OBJECT )
    {
        return b->op( node->sloc, XEC_SEQ_OBJREF, uv->object );
    }
    
    assert( ! "invalid upval" );
    return NULL;
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_objref* node )
{
    return b->op( node->sloc, XEC_SEQ_OBJREF, node->object );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_key* node )
{
    xec_seq_op* object = b->expr( node->object );
    return b->op( node->sloc, XEC_SEQ_KEY, object, node->key );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_inkey* node )
{
    xec_seq_op* object = b->expr( node->object );
    xec_seq_op* key = b->expr( node->key );
    return b->op( node->sloc, XEC_SEQ_INKEY, object, key );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_index* node )
{
    xec_seq_op* object = b->expr( node->object );
    xec_seq_op* index = b->expr( node->index );
    return b->op( node->sloc, XEC_SEQ_INDEX, object, index );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_preop* node )
{
    xec_seq_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    
    xec_seq_op* value = b->lvalue_value( &lvalue );
    xec_seq_op* literal = b->op( node->sloc, XEC_SEQ_ONE );
    
    xec_seq_opcode opcode = XEC_SEQ_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_PREINC:   opcode = XEC_SEQ_ADD;   break;
    case XEC_ASTOP_PREDEC:   opcode = XEC_SEQ_SUB;   break;
    default:
        assert( ! "invalid operator" );
        break;
    }
    
    xec_seq_op* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return updated;
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_postop* node )
{
    xec_seq_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    
    xec_seq_op* value = b->lvalue_value( &lvalue );
    xec_seq_op* literal = b->op( node->sloc, XEC_SEQ_ONE );
    
    xec_seq_opcode opcode = XEC_SEQ_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSTINC:  opcode = XEC_SEQ_ADD;   break;
    case XEC_ASTOP_POSTDEC:  opcode = XEC_SEQ_SUB;   break;
    default: assert( ! "invalid operator" ); break;
    }
    
    xec_seq_op* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return value;
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_unary* node )
{
    xec_seq_op* operand = b->expr( node->operand );
    
    xec_seq_opcode opcode = XEC_SEQ_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSITIVE: opcode = XEC_SEQ_POS;       break;
    case XEC_ASTOP_NEGATIVE: opcode = XEC_SEQ_NEG;       break;
    case XEC_ASTOP_LOGICNOT: opcode = XEC_SEQ_NOT;       break;
    case XEC_ASTOP_BITNOT:   opcode = XEC_SEQ_BITNOT;    break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operand );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_binary* node )
{
    xec_seq_op* operanda = b->expr( node->lhs );
    xec_seq_op* operandb = b->expr( node->rhs );
    
    xec_seq_opcode opcode = XEC_SEQ_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_MULTIPLY:     opcode = XEC_SEQ_MUL;       break;
    case XEC_ASTOP_DIVIDE:       opcode = XEC_SEQ_DIV;       break;
    case XEC_ASTOP_MODULUS:      opcode = XEC_SEQ_MOD;       break;
    case XEC_ASTOP_INTDIV:       opcode = XEC_SEQ_INTDIV;    break;
    case XEC_ASTOP_ADD:          opcode = XEC_SEQ_ADD;       break;
    case XEC_ASTOP_SUBTRACT:     opcode = XEC_SEQ_SUB;       break;
    case XEC_ASTOP_LSHIFT:       opcode = XEC_SEQ_LSL;       break;
    case XEC_ASTOP_LRSHIFT:      opcode = XEC_SEQ_LSR;       break;
    case XEC_ASTOP_ARSHIFT:      opcode = XEC_SEQ_ASR;       break;
    case XEC_ASTOP_BITAND:       opcode = XEC_SEQ_BITAND;    break;
    case XEC_ASTOP_BITXOR:       opcode = XEC_SEQ_BITXOR;    break;
    case XEC_ASTOP_BITOR:        opcode = XEC_SEQ_BITOR;     break;
    case XEC_ASTOP_CONCATENATE:  opcode = XEC_SEQ_CONCAT;    break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operanda, operandb );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_compare* node )
{
    if ( node->opkinds.size() == 1 )
    {
        // Simple comparison.
        xec_seq_op* operanda = b->expr( node->first );
        xec_seq_op* operandb = b->expr( node->terms.at( 0 ) );
        
        xec_seq_opcode opcode = XEC_SEQ_NOP;
        switch ( node->opkinds.at( 0 ) )
        {
        case XEC_ASTOP_EQUAL:           opcode = XEC_SEQ_EQ;        break;
        case XEC_ASTOP_NOTEQUAL:        opcode = XEC_SEQ_NE;        break;
        case XEC_ASTOP_LESS:            opcode = XEC_SEQ_LT;        break;
        case XEC_ASTOP_GREATER:         opcode = XEC_SEQ_GT;        break;
        case XEC_ASTOP_LESSEQUAL:       opcode = XEC_SEQ_LE;        break;
        case XEC_ASTOP_GREATEREQUAL:    opcode = XEC_SEQ_GE;        break;
        case XEC_ASTOP_IN:              opcode = XEC_SEQ_IN;        break;
        case XEC_ASTOP_NOTIN:           opcode = XEC_SEQ_NOTIN;     break;
        case XEC_ASTOP_IS:              opcode = XEC_SEQ_IS;        break;
        case XEC_ASTOP_NOTIS:           opcode = XEC_SEQ_NOTIS;     break;
        default: assert( ! "invalid operator" ); break;
        }
        
        return b->op( node->sloc, opcode, operanda, operandb );
    }
    else
    {
        // Chained comparisons are a nightmare.  Can't express the required
        // control flow in seq form, so delay worrying about it until later...
        return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
    }
}





/*
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
    xec_seq_opcode seqkind = XEC_SEQ_ASSIGN;
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
        xec_seq_op* op = &seq->ops.at( i->second );
        op->rcount += 1;
        return;
    }
    
    size_t index = seq->ops.size();
    seq->ops.emplace_back( XEC_SEQ_EVAL, node );
    seq->nodemap.emplace( node, index );
}



*/





