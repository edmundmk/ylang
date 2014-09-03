//
//  xec_seq_astvisitors.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_seq_astvisitors.h"
#include "xec_seq.h"
#include "xec_seq_builder.h"



static xec_seq_opcode assign_opcode( xec_ast_opkind assignop )
{
    switch ( assignop )
    {
    case XEC_ASTOP_MULASSIGN:       return XEC_SEQ_MUL;
    case XEC_ASTOP_DIVASSIGN:       return XEC_SEQ_DIV;
    case XEC_ASTOP_MODASSIGN:       return XEC_SEQ_MOD;
    case XEC_ASTOP_INTDIVASSIGN:    return XEC_SEQ_INTDIV;
    case XEC_ASTOP_ADDASSIGN:       return XEC_SEQ_ADD;
    case XEC_ASTOP_SUBASSIGN:       return XEC_SEQ_SUB;
    case XEC_ASTOP_LSHIFTASSIGN:    return XEC_SEQ_LSL;
    case XEC_ASTOP_LRSHIFTASSIGN:   return XEC_SEQ_LSR;
    case XEC_ASTOP_ARSHIFTASSIGN:   return XEC_SEQ_ASR;
    case XEC_ASTOP_BITANDASSIGN:    return XEC_SEQ_BITAND;
    case XEC_ASTOP_BITXORASSIGN:    return XEC_SEQ_BITXOR;
    case XEC_ASTOP_BITORASSIGN:     return XEC_SEQ_BITOR;
    default: assert( ! "invalid operator" ); break;
    }

    return XEC_SEQ_NOP;
}





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
    return b->op( node->sloc, XEC_SEQ_LITERAL, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_bool* node )
{
    return b->op( node->sloc, XEC_SEQ_LITERAL, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_number* node )
{
    return b->op( node->sloc, XEC_SEQ_LITERAL, node );
}

xec_seq_op* xec_seq_build_expr::visit( xec_expr_string* node )
{
    return b->op( node->sloc, XEC_SEQ_LITERAL, node );
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

    switch ( uv->kind )
    {
    case XEC_UPVAL_LOCAL:
        return b->op( node->sloc, XEC_SEQ_VAR, uv->local );

    case XEC_UPVAL_OBJECT:
        return b->op( node->sloc, XEC_SEQ_OBJREF, uv->object );

    default:
        assert( ! "invalid upval" );
        return NULL;
    }
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
    /*
        a
        b
        <
        result : beginand <
            c
            >
            beginand >
                d
                ==
                beginand ==
                    e
                    !=
                end : !=
            end : beginand ==
        end : beginand >
    */

    xec_seq_op* lhs = b->expr( node->first );
    xec_seq_op* result = NULL;
    
    std::vector< xec_seq_op* > end;
    for ( size_t i = 0; i < node->opkinds.size(); ++i )
    {
        xec_seq_op* rhs = b->expr( node->terms.at( i ) );
    
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
        
        xec_seq_op* compare = b->op( node->sloc, opcode, lhs, rhs );
        
        if ( i < ( node->opkinds.size() - 1 ) )
        {
            compare = b->op( node->sloc, XEC_SEQ_BEGINAND, compare );
        }
        
        if ( i == 0 )
        {
            result = compare;
        }
        else
        {
            end.push_back( compare );
        }
    }
    
    for ( auto i = end.rbegin(); i != end.rbegin(); ++i )
    {
        b->op( node->sloc, XEC_SEQ_END, *i );
    }
    
    return result;
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_logical* node )
{
    switch ( node->opkind )
    {
    case XEC_ASTOP_LOGICAND:
    {
        /*
            lhs
            beginand lhs
                rhs
            end : rhs
        */
        xec_seq_op* lhs = b->expr( node->lhs );
        xec_seq_op* result = b->op( node->sloc, XEC_SEQ_BEGINAND, lhs );
        xec_seq_op* rhs = b->expr( node->rhs );
        b->op( node->sloc, XEC_SEQ_END, rhs );
        return result;
    }
    
    case XEC_ASTOP_LOGICXOR:
    {
        // No shortcut evaluation...
        xec_seq_op* lhs = b->expr( node->lhs );
        xec_seq_op* rhs = b->expr( node->rhs );
        return b->op( node->sloc, XEC_SEQ_XOR, lhs, rhs );
        break;
    }
    
    case XEC_ASTOP_LOGICOR:
    {
        /*
            lhs
            beginor lhs
                rhs
            end : rhs
        */
        xec_seq_op* lhs = b->expr( node->lhs );
        xec_seq_op* result = b->op( node->sloc, XEC_SEQ_BEGINOR, lhs );
        xec_seq_op* rhs = b->expr( node->rhs );
        b->op( node->sloc, XEC_SEQ_END, rhs );
        return result;
    }
        
    default:
        assert( ! "invalid operator" );
        return NULL;
    }
    
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_qmark* node )
{
    xec_seq_op* condition = b->expr( node->condition );
    xec_seq_op* qmark = b->op( node->sloc, XEC_SEQ_BEGINIF, condition );
    xec_seq_op* iftrue = b->expr( node->iftrue );
    b->op( node->sloc, XEC_SEQ_ELSE, iftrue );
    xec_seq_op* iffalse = b->expr( node->iffalse );
    b->op( node->sloc, XEC_SEQ_END, iffalse );
    return qmark;
}


xec_seq_op* xec_seq_build_expr::visit( xec_new_new* node )
{
    xec_seq_op* proto = b->expr( node->proto );
    
    // Get arguments (unpacked).
    xec_seq_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );
    
    // Build operand list.
    xec_seq_args* args = b->args( 1 );
    b->reserve( args, 1, &arguments );
    b->push( args, proto );
    b->push( args, &arguments );
    
    return b->op( node->sloc, XEC_SEQ_NEW, args );
}


xec_seq_op* xec_seq_build_expr::visit( xec_new_object* node )
{
    // Don't recurse into the object.
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}


xec_seq_op* xec_seq_build_expr::visit( xec_new_array* node )
{
    // Similarly with arrays.
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}


xec_seq_op* xec_seq_build_expr::visit( xec_new_table* node )
{
    // And similarly again.
    return b->op( node->sloc, XEC_SEQ_ASTNODE, node );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_mono* node )
{
    return visit( node->expr );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_call* node )
{
    xec_seq_valist values;
    b->unpack( &values, node, 1 );
    return values.values.at( 0 );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_yield* node )
{
    xec_seq_valist values;
    b->unpack( &values, node, 1 );
    return values.values.at( 0 );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_vararg* node )
{
    return b->op( node->sloc, XEC_SEQ_VARARG, 0 );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_unpack* node )
{
    return b->op( node->sloc, XEC_SEQ_ELEMENT, 0 );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_list* node )
{
    xec_seq_valist values;
    b->unpack( &values, node, 1 );
    return values.values.at( 0 );
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_assign* node )
{
    xec_seq_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    
    if ( node->assignop == XEC_ASTOP_DECLARE
            || node->assignop == XEC_ASTOP_ASSIGN )
    {
        // Normal assignment.
        xec_seq_op* rvalue = b->expr( node->rvalue );
        b->lvalue_assign( &lvalue, rvalue );
        return rvalue;
    }
    else
    {
        // Assignment with operation.
        xec_seq_opcode opcode = assign_opcode( node->assignop );
        xec_seq_op* lhs = b->lvalue_value( &lvalue );
        xec_seq_op* rhs = b->expr( node->rvalue );
        xec_seq_op* rvalue = b->op( node->sloc, opcode, lhs, rhs );
        b->lvalue_assign( &lvalue, rvalue );
        return rvalue;
    }
}


xec_seq_op* xec_seq_build_expr::visit( xec_expr_assign_list* node )
{
    xec_seq_valist values;
    b->unpack( &values, node, 1 );
    return values.values.at( 0 );
}



xec_seq_build_unpack::xec_seq_build_unpack( xec_seq_builder* b )
    :   b( b )
{
}


void xec_seq_build_unpack::fallback(
                xec_ast_node* node, xec_seq_valist* valist, int rcount )
{
    // It's an expression that doesn't unpack.  Evaluate it.
    xec_seq_op* val = b->expr( node );
    
    // Return the required number of results.
    if ( rcount > 0 )
    {
        valist->values.reserve( rcount );

        // Starting with the expression result.
        valist->values.push_back( val );
        
        for ( int i = 1; i < rcount; ++i )
        {
            valist->values.push_back( b->op( node->sloc, XEC_SEQ_NULL ) );
        }
    }
}


void xec_seq_build_unpack::visit(
                xec_expr_call* node, xec_seq_valist* valist, int rcount )
{
    assert( node->unpack );

    // Get function and 'this'.
    xec_seq_op* function = NULL;
    xec_seq_op* thisval  = NULL;
    
    switch ( node->function->kind )
    {
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)node->function;
        thisval = b->expr( key->object );
        function = b->op( key->sloc, XEC_SEQ_KEY, thisval, key->key );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
        thisval = b->expr( inkey->object );
        xec_seq_op* key = b->expr( inkey->key );
        function = b->op( inkey->sloc, XEC_SEQ_INKEY, thisval, key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)node->function;
        thisval = b->expr( index->object );
        xec_seq_op* idxval = b->expr( index->index );
        function = b->op( index->sloc, XEC_SEQ_INDEX, thisval, idxval );
        break;
    }
    
    default:
    {
        // Call function without implicit this.
        function = b->expr( node->function );
        break;
    }
    }
    
    // Unpack arguments.
    xec_seq_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }
    
    // Construct call.
    xec_seq_args* args = b->args( rcount );
    b->reserve( args, thisval ? 2 : 1, &arguments );
    b->push( args, function );
    if ( thisval )
    {
        b->push( args, thisval );
    }
    b->push( args, &arguments );
    
    xec_seq_opcode opcode = node->yieldcall ? XEC_SEQ_YCALL : XEC_SEQ_CALL;
    xec_seq_op* call = b->op( node->sloc, opcode, args );
    
    select( node->sloc, valist, rcount, call );
}


void xec_seq_build_unpack::visit(
                xec_expr_yield* node, xec_seq_valist* valist, int rcount )
{
    assert( node->unpack );

    // Unpack arguments.
    xec_seq_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }
    
    // Construct yield.
    xec_seq_args* args = b->args( rcount );
    b->reserve( args, 0, &arguments );
    b->push( args, &arguments );
    
    xec_seq_op* yield = b->op( node->sloc, XEC_SEQ_YIELD, args );
    
    select( node->sloc, valist, rcount, yield );
}


void xec_seq_build_unpack::visit(
                xec_expr_vararg* node, xec_seq_valist* valist, int rcount )
{
    if ( rcount >= 0 )
    {
        valist->values.reserve( rcount );
        for ( int i = 0; i < rcount; ++i )
        {
            xec_seq_op* varg = b->op( node->sloc, XEC_SEQ_VARARG, i );
            valist->values.push_back( varg );
        }
    }
    else
    {
        valist->unpacked = b->op( node->sloc, XEC_SEQ_VARALL, b->args( -1 ) );
    }
}


void xec_seq_build_unpack::visit(
                xec_expr_unpack* node, xec_seq_valist* valist, int rcount )
{
    xec_seq_op* array = b->expr( node->array );

    if ( rcount >= 0 )
    {
        valist->values.reserve( rcount );
        for ( int i = 0; i < rcount; ++i )
        {
            xec_seq_op* elem = b->op( node->sloc, XEC_SEQ_ELEMENT, array, i );
            valist->values.push_back( elem );
        }
    }
    else
    {
        valist->unpacked = b->op( node->sloc, XEC_SEQ_UNPACK, b->args( -1 ) );
    }
}


void xec_seq_build_unpack::visit(
                xec_expr_list* node, xec_seq_valist* valist, int rcount )
{
    if ( rcount >= 0 )
    {
        valist->values.reserve( rcount );
    }

    // Evaluate non-final values.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xec_seq_op* val = b->expr( node->values.at( i ) );
        if ( rcount < 0 || (int)i < rcount )
        {
            valist->values.push_back( val );
        }
    }
    
    // Work out the number of results that remain unfilled.
    int fcount;
    if ( rcount >= 0 )
    {
        fcount = rcount - (int)node->values.size();
        fcount = fcount >= 0 ? fcount : 0;
    }
    else
    {
        fcount = -1;
    }
    
    if ( node->final )
    {
        // Unpack the final value, requesting the number of values remaining.
        xec_seq_valist final;
        b->unpack( &final, node->final, fcount );
        valist->values.insert( valist->values.end(),
                        final.values.begin(), final.values.end() );
        valist->unpacked = final.unpacked;
    }
    else
    {
        // Unpack null into remaining values.
        for ( int i = 0; i < fcount; ++i )
        {
            valist->values.push_back( b->op( node->sloc, XEC_SEQ_NULL ) );
        }
    }
}


void xec_seq_build_unpack::visit(
                xec_expr_assign_list* node, xec_seq_valist* valist, int rcount )
{
    if ( rcount >= 0 )
    {
        valist->values.reserve( rcount );
    }

    // Evaluate all lvalues ready for assignment.
    std::vector< xec_seq_lvalue > lvalues;
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        xec_seq_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        lvalues.push_back( lvalue );
    }
    
    // Unpack the correct number of rvalues.
    xec_seq_valist rvalues;
    b->unpack( &rvalues, node->rvalues, (int)lvalues.size() );

    // Perform assignments and add values to the valist.
    for ( int i = 0; i < lvalues.size(); ++i )
    {
        xec_seq_lvalue& lvalue = lvalues.at( i );
        
        xec_seq_op* val = NULL;
        if ( node->assignop == XEC_ASTOP_DECLARE
                || node->assignop == XEC_ASTOP_ASSIGN )
        {
            // Straightforward assignment.
            val = rvalues.values.at( i );
        }
        else
        {
            // It's assignment with an operation.
            xec_seq_opcode opcode = assign_opcode( node->assignop );
            xec_seq_op* lhs = b->lvalue_value( &lvalue );
            xec_seq_op* rhs = rvalues.values.at( i );
            val = b->op( node->sloc, opcode, lhs, rhs );
            b->lvalue_assign( &lvalue, val );
        }
        
        b->lvalue_assign( &lvalue, val );
        if ( i < rcount )
        {
            valist->values.push_back( val );
        }
    }
    
    // And fill in the rest with null.
    for ( int i = (int)lvalues.size(); i < rcount; ++i )
    {
        valist->values.push_back( b->op( node->sloc, XEC_SEQ_NULL ) );
    }
}



void xec_seq_build_unpack::select(
                int sloc, xec_seq_valist* valist, int rcount, xec_seq_op* op )
{
    if ( rcount == 0 || rcount == 1 )
    {
        // One result.
        valist->values.push_back( op );
    }
    else if ( rcount > 1 )
    {
        // Select finite number of values.
        valist->values.reserve( rcount );
        for ( int i = 0; i < rcount; ++i )
        {
            xec_seq_op* sel = b->op( sloc, XEC_SEQ_SELECT, op, i );
            valist->values.push_back( sel );
        }
    }
    else
    {
        // Unpack all results.
        valist->unpacked = op;
    }
}


