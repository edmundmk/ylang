//
//  xec_ssa_builder.cpp
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_builder.h"
#include <make_unique>
#include "xec_script.h"
#include "xec_ssa.h"
#include "xec_ast_visitor.h"




template < typename containera_t, typename containerb_t >
static inline void extend( containera_t* a, const containerb_t& b )
{
    a->insert( a->end(), b.begin(), b.end() );
}



/*
    An lvalue is not completely evaluated as it must be assigned to.
*/


struct xec_ssa_lvalue
{
    xec_ssa_lvalue();

    int             sloc;
    xec_ssa_opcode  opcode;
    xec_ssa_node*   object;
    union
    {
    xec_ssa_node*   index;
    xec_ast_name*   local;
    const char*     literal;
    int             upval;
    };
};


xec_ssa_lvalue::xec_ssa_lvalue()
    :   opcode( (xec_ssa_opcode)-1 )
    ,   object( NULL )
    ,   index( NULL )
{
}



/*
    A list of unpacked values.
*/

struct xec_ssa_valist
{
    xec_ssa_valist();

    std::deque< xec_ssa_node* > values;
    xec_ssa_node*   unpacked;
};


xec_ssa_valist::xec_ssa_valist()
    :   unpacked( NULL )
{
}







/*
    xec_ssa_build_expr
*/

xec_ssa_build_expr::xec_ssa_build_expr( xec_ssa_builder* b )
    :   b( b )
{
}

xec_ssa_node* xec_ssa_build_expr::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_ast_func* node )
{
    // Locate (possibly unbuilt) SSA of function.
    xec_ssa_func* func = b->func( node );
    
    // Construct closure and bind upvals.
    xec_ssa_expand* closure = b->expand( node->sloc, XEC_SSA_CLOSURE, func );
    for ( size_t i = 0 ; i < node->upvals.size(); ++i )
    {
        const xec_ast_upval& upval = node->upvals.at( i );
        switch ( upval.kind )
        {
        case XEC_UPVAL_LOCAL:
        {
            xec_ssa_node* local = b->lookup( upval.local );
            closure->operands.push_back( local );
            break;
        }
        
        case XEC_UPVAL_OBJECT:
        {
            xec_ssa_node* object = b->lookup( upval.object );
            closure->operands.push_back( object );
            break;
        }
        
        case XEC_UPVAL_UPVAL:
        {
            xec_ssa_node* uv = b->node( b->packed(
                        node->sloc, XEC_SSA_REFUP, nullptr, upval.upval ) );
            closure->operands.push_back( uv );
            break;
        }
        }
    }
    
    return b->node( closure );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_null* node )
{
    return b->node( b->packed( node->sloc, XEC_SSA_NULL ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_bool* node )
{
    return b->node( b->packed( node->sloc, XEC_SSA_BOOL, node->value ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_number* node )
{
    return b->node( b->packed( node->sloc, XEC_SSA_NUMBER, node->value ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_string* node )
{
    return b->node( b->packed( node->sloc,
                XEC_SSA_STRING, node->string, node->length ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_local* node )
{
    return b->lookup( node->name );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_global* node )
{
    return b->node( b->packed( node->sloc,
                XEC_SSA_GLOBAL, nullptr, node->name ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_upref* node )
{
    return b->node( b->packed( node->sloc,
                XEC_SSA_REFUP, nullptr, node->index ) );
                
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_objref* node )
{
    return b->lookup( node->object );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_key* node )
{
    xec_ssa_node* object = visit( node->object );
    return b->node( b->packed( node->sloc, XEC_SSA_KEY, object, node->key ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_inkey* node )
{
    xec_ssa_node* object = visit( node->object );
    xec_ssa_node* key    = visit( node->key );
    return b->node( b->packed( node->sloc, XEC_SSA_INKEY, object, key ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_index* node )
{
    xec_ssa_node* object = visit( node->object );
    xec_ssa_node* index  = visit( node->index );
    return b->node( b->packed( node->sloc, XEC_SSA_INDEX, object, index ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_preop* node )
{
    xec_ssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    xec_ssa_node* value   = b->lvalue_value( &lvalue );
    xec_ssa_node* literal = b->node( b->packed(
                node->sloc, XEC_SSA_NUMBER, 1.0 ) );
    
    xec_ssa_opcode opcode = XEC_SSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_PREINC:   opcode = XEC_SSA_ADD;   break;
    case XEC_ASTOP_PREDEC:   opcode = XEC_SSA_SUB;   break;
    default:
        assert( ! "invalid operator" );
        break;
    }
    
    xec_ssa_node* updated = b->node( b->packed(
                node->sloc, opcode, value, literal ) );
    return b->lvalue_assign( &lvalue, updated );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_postop* node )
{
    xec_ssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    xec_ssa_node* value   = b->lvalue_value( &lvalue );
    xec_ssa_node* literal = b->node( b->packed(
                node->sloc, XEC_SSA_NUMBER, 1.0 ) );
    
    xec_ssa_opcode opcode = XEC_SSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSTINC:  opcode = XEC_SSA_ADD;   break;
    case XEC_ASTOP_POSTDEC:  opcode = XEC_SSA_SUB;   break;
    default: assert( ! "invalid operator" ); break;
    }
    
    xec_ssa_node* updated = b->node( b->packed(
                node->sloc, opcode, value, literal ) );
    b->lvalue_assign( &lvalue, updated );
    return value;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_unary* node )
{
    xec_ssa_node* operand = visit( node->operand );
    
    xec_ssa_opcode opcode = XEC_SSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSITIVE: opcode = XEC_SSA_POS;       break;
    case XEC_ASTOP_NEGATIVE: opcode = XEC_SSA_NEG;       break;
    case XEC_ASTOP_LOGICNOT: opcode = XEC_SSA_NOT;       break;
    case XEC_ASTOP_BITNOT:   opcode = XEC_SSA_BITNOT;    break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->node( b->packed( node->sloc, opcode, operand ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_binary* node )
{
    xec_ssa_node* operanda = visit( node->lhs );
    xec_ssa_node* operandb = visit( node->rhs );
    
    xec_ssa_opcode opcode = XEC_SSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_MULTIPLY:     opcode = XEC_SSA_MUL;       break;
    case XEC_ASTOP_DIVIDE:       opcode = XEC_SSA_DIV;       break;
    case XEC_ASTOP_MODULUS:      opcode = XEC_SSA_MOD;       break;
    case XEC_ASTOP_INTDIV:       opcode = XEC_SSA_INTDIV;    break;
    case XEC_ASTOP_ADD:          opcode = XEC_SSA_ADD;       break;
    case XEC_ASTOP_SUBTRACT:     opcode = XEC_SSA_SUB;       break;
    case XEC_ASTOP_LSHIFT:       opcode = XEC_SSA_LSL;       break;
    case XEC_ASTOP_LRSHIFT:      opcode = XEC_SSA_LSR;       break;
    case XEC_ASTOP_ARSHIFT:      opcode = XEC_SSA_ASR;       break;
    case XEC_ASTOP_BITAND:       opcode = XEC_SSA_BITAND;    break;
    case XEC_ASTOP_BITXOR:       opcode = XEC_SSA_BITXOR;    break;
    case XEC_ASTOP_BITOR:        opcode = XEC_SSA_BITOR;     break;
    case XEC_ASTOP_CONCATENATE:  opcode = XEC_SSA_CONCAT;    break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->node( b->packed( node->sloc, opcode, operanda, operandb ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_compare* node )
{
    xec_ssa_node* lhs = visit( node->first );
    xec_ssa_node* result = NULL;
    
    size_t ifcount = 0;
    for ( size_t i = 0; i < node->opkinds.size(); ++i )
    {
        // Only evaluate this term if previous term(s) were true.
        if ( result )
        {
            b->ifthen( result );
            ifcount += 1;
        }
    
    
        // Evaluate term.
        xec_ast_opkind opkind = node->opkinds.at( i );
        xec_ssa_node* rhs = visit( node->terms.at( i ) );
        
        /*
            We have a limited set of comparison opcodes:
                l > r   -> r < l
                l >= r  -> r <= l
                l != r  -> not ( l == r )
                l !in r -> not ( l in r )
                l !is r -> not ( l is r )
        */
        
        xec_ssa_opcode opcode = XEC_SSA_NOP;
        bool inv = false, neg = false;
        switch ( opkind )
        {
        case XEC_ASTOP_EQUAL:        opcode = XEC_SSA_EQ;               break;
        case XEC_ASTOP_NOTEQUAL:     opcode = XEC_SSA_EQ; neg = true;   break;
        case XEC_ASTOP_LESS:         opcode = XEC_SSA_LT;               break;
        case XEC_ASTOP_GREATER:      opcode = XEC_SSA_LE; inv = true;   break;
        case XEC_ASTOP_LESSEQUAL:    opcode = XEC_SSA_LE;               break;
        case XEC_ASTOP_GREATEREQUAL: opcode = XEC_SSA_LT; inv = true;   break;
        case XEC_ASTOP_IN:           opcode = XEC_SSA_IN;               break;
        case XEC_ASTOP_NOTIN:        opcode = XEC_SSA_IN; neg = true;   break;
        case XEC_ASTOP_IS:           opcode = XEC_SSA_IS;               break;
        case XEC_ASTOP_NOTIS:        opcode = XEC_SSA_IS; neg = true;   break;
        default: assert( ! "invalid operator" ); break;
        }
        
        if ( ! inv )
            result = b->node( b->packed( node->sloc, opcode, lhs, rhs ) );
        else
            result = b->node( b->packed( node->sloc, opcode, rhs, lhs ) );
        if ( neg )
            result = b->node( b->packed( node->sloc, XEC_SSA_NOT, result ) );
        
        // Keep the result in a temporary, as if it is false we shorcut
        // evaluation of later terms.
        b->define( node, result );
        
        // The rhs becomes the lhs of the next chained comparison.
        lhs = rhs;
    }
    
    for ( size_t i = 0; i < ifcount; ++i )
    {
        b->ifend();
    }
    
    // Lookup the result, as it is defined in multiple blocks.
    return b->lookup( node );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_logical* node )
{
    switch ( node->opkind )
    {
    case XEC_ASTOP_LOGICAND:
    {
        // Only evaluate rhs if lhs is true.
        xec_ssa_node* lhs = visit( node->lhs );
        b->define( node, lhs );
        b->ifthen( lhs );
        xec_ssa_node* rhs = visit( node->rhs );
        b->define( node, rhs );
        b->ifend();
        return b->lookup( node );
    }
    
    case XEC_ASTOP_LOGICXOR:
    {
        // Does not perform shortcut evaluation.
        xec_ssa_node* lhs = visit( node->lhs );
        xec_ssa_node* rhs = visit( node->rhs );
        return b->node( b->packed( node->sloc, XEC_SSA_XOR, lhs, rhs ) );
    }
    
    case XEC_ASTOP_LOGICOR:
    {
        // Only evaluate rhs if the lhs is false.
        xec_ssa_node* lhs = visit( node->lhs );
        b->define( node, lhs );
        xec_ssa_node* invlhs = b->node( b->packed(
                    node->sloc, XEC_SSA_NOT, lhs ) );
        b->ifthen( invlhs );
        xec_ssa_node* rhs = visit( node->rhs );
        b->define( node, rhs );
        b->ifend();
        return b->lookup( node );
    }
    
    default:
        assert( ! "invalid operator" );
        break;
    }
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_qmark* node )
{
    xec_ssa_node* condition = visit( node->condition );
    b->ifthen( condition );
    xec_ssa_node* iftrue = visit( node->iftrue );
    b->define( node, iftrue );
    b->ifelse();
    xec_ssa_node* iffalse = visit( node->iffalse );
    b->define( node, iffalse );
    b->ifend();
    return b->lookup( node );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_new* node )
{
    // Get prototype.
    xec_ssa_node* proto = visit( node->proto );

    // Get arguments (unpacked).
    xec_ssa_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );

    // SSA instruction has both prototype and arguments as operands.
    xec_ssa_expand* newval = b->expand( node->sloc, XEC_SSA_NEW, 1 );
    newval->operands.push_back( proto );
    extend( &newval->operands, arguments.values );
    newval->unpacked = arguments.unpacked;

    return b->node( newval );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_object* node )
{
    // Construct object.
    xec_ssa_node* proto = node->proto ? visit( node->proto ) : NULL;
    xec_ssa_node* object = b->node( b->packed(
                    node->sloc, XEC_SSA_OBJECT, proto ) );
    
    // Define object so objrefs can find it with lookup.
    b->define( node, object );
    
    // Visit object members, which should all be assignments.
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        visit( node->members.at( i ) );
    }
    
    b->close_scope( node->scope );
    return object;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_array* node )
{
    // Construct array.
    xec_ssa_node* array = b->node( b->packed( node->sloc,
                    XEC_SSA_ARRAY, nullptr, (int)node->values.size() ) );
    
    // Append all values.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xec_ssa_node* value = visit( node->values.at( i ) );
        b->node( b->packed( value->sloc, XEC_SSA_APPEND, array, value ) );
    }
    if ( node->final )
    {
        // Unpack final values and extend the array with them.
        xec_ssa_valist values;
        b->unpack( &values, node->final, -1 );
        xec_ssa_expand* final = b->expand( node->sloc, XEC_SSA_EXTEND, 0 );
        extend( &final->operands, values.values );
        final->unpacked = values.unpacked;
        b->node( final );
    }

    return array;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_table* node )
{
    // Construct table.
    xec_ssa_node* table = b->node( b->packed( node->sloc,
                    XEC_SSA_TABLE, nullptr, (int)node->elements.size() ) );
    
    // Add elements.
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const xec_key_value& e = node->elements.at( i );
        xec_ssa_node* key   = visit( e.key );
        xec_ssa_node* value = visit( e.value );
        b->node( b->triple( node->sloc, XEC_SSA_SETKEY, table, key, value ) );
    }
    
    return table;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_mono* node )
{
    return visit( node->expr );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_call* node )
{
    xec_ssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_yield* node )
{
    xec_ssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_vararg* node )
{
    // Fetch the first variable argument.
    return b->node( b->packed( node->sloc, XEC_SSA_VARARG, nullptr, 0 ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_unpack* node )
{
    xec_ssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_list* node )
{
    xec_ssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_assign* node )
{
    xec_ssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    xec_ssa_node* rvalue = visit( node->rvalue );
    return b->lvalue_assign( &lvalue, rvalue );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_assign_list* node )
{
    xec_ssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}





xec_ssa_build_unpack::xec_ssa_build_unpack( xec_ssa_builder* b )
    :   b( b )
{
}

void xec_ssa_build_unpack::fallback(
                xec_ast_node* node, xec_ssa_valist* values, int valcount )
{
    // Expression produces only one value.
    values->values.push_back( b->expr( node ) );
    
    // Unpack NULL for remaining values.
    xec_ssa_node* nullval = b->node( b->packed( node->sloc, XEC_SSA_NULL ) );
    for ( int i = 1; i < valcount; ++i )
    {
        values->values.push_back( nullval );
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_null* node, xec_ssa_valist* values, int valcount )
{
    // Special case to unpack NULL.
    xec_ssa_node* nullval = b->expr( node );
    values->values.push_back( nullval );
    for ( int i = 1; i < valcount; ++i )
    {
        values->values.push_back( nullval );
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_call* node, xec_ssa_valist* values, int valcount )
{
    // Get function and this value.
    xec_ssa_node* function = NULL;
    xec_ssa_node* thisval  = NULL;
    switch ( node->function->kind )
    {
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)node->function;
        thisval = b->expr( key->object );
        function = b->node( b->packed(
                        key->sloc, XEC_SSA_KEY, thisval, key->key ) );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
        thisval = b->expr( inkey->object );
        xec_ssa_node* key = b->expr( inkey->key );
        function = b->node( b->packed(
                        inkey->sloc, XEC_SSA_INKEY, thisval, key ) );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)node->function;
        thisval = b->expr( index->object );
        xec_ssa_node* idxval = b->expr( index->index );
        function = b->node( b->packed(
                        index->sloc, XEC_SSA_INDEX, thisval, idxval ) );
        break;
    }
    
    default:
    {
        // Call function without implicit this.
        function = b->expr( node->function );
        break;
    }
    }

    // Get arguments (unpacked).
    xec_ssa_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );

    // Construct call.
    xec_ssa_expand* call = b->expand( node->sloc, XEC_SSA_CALL, valcount );
    call->operands.push_back( function );
    if ( thisval )
        call->operands.push_back( thisval );
    extend( &call->operands, arguments.values );
    call->unpacked = arguments.unpacked;
    xec_ssa_node* result = b->node( call );
    
    // If we requested a finite number of values, select them.
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xec_ssa_node* value = b->node( b->packed(
                            node->sloc, XEC_SSA_SELECT, result, i ) );
            values->values.push_back( value );
        }
    }
    else
    {
        values->unpacked = result;
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_yield* node, xec_ssa_valist* values, int valcount )
{
    // Get arguments (unpacked).
    xec_ssa_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );
    
    // Construct yield.
    xec_ssa_expand* yield = b->expand( node->sloc, XEC_SSA_YIELD, valcount );
    extend( &yield->operands, arguments.values );
    yield->unpacked = arguments.unpacked;
    xec_ssa_node* result = b->node( yield );

    // If we requested a finite number of values, select them.
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xec_ssa_node* value = b->node( b->packed(
                            node->sloc, XEC_SSA_SELECT, result, i ) );
            values->values.push_back( value );
        }
    }
    else
    {
        values->unpacked = result;
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_vararg* node, xec_ssa_valist* values, int valcount )
{
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xec_ssa_node* value = b->node( b->packed(
                            node->sloc, XEC_SSA_VARARG, nullptr, i ) );
            values->values.push_back( value );
        }
    }
    else
    {
        values->unpacked = b->node( b->packed(
                        node->sloc, XEC_SSA_VARARG, nullptr, -1 ) );
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_unpack* node, xec_ssa_valist* values, int valcount )
{
    xec_ssa_node* array = b->expr( node->array );
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xec_ssa_node* value = b->node( b->packed(
                            node->sloc, XEC_SSA_UNPACK, array, i ) );
            values->values.push_back( value );
        }
    }
    else
    {
        values->unpacked = b->node( b->packed(
                        node->sloc, XEC_SSA_UNPACK, array, -1 ) );
    }
}

void xec_ssa_build_unpack::visit(
                xec_expr_list* node, xec_ssa_valist* values, int valcount )
{
    // Only assign as many values as were requested.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xec_ssa_node* value = b->expr( node->values.at( i ) );
        if ( valcount == -1 || (int)i < valcount )
        {
            values->values.push_back( value );
        }
    }
    
    // And unpack the final values.
    int required;
    if ( valcount != -1 )
    {
        required = valcount - (int)node->values.size();
        required = required > 0 ? required : 0;
    }
    else
    {
        required = -1;
    }

    if ( node->final )
    {
        xec_ssa_valist final;
        visit( node->final, &final, required );
        extend( &values->values, final.values );
        values->unpacked = final.unpacked;
    }
    else if ( required > 0 )
    {
        // Fill in the rest with null.
        xec_ssa_node* nullval = b->node( b->packed( node->sloc, XEC_SSA_NULL ) );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}

void xec_ssa_build_unpack::visit(
            xec_expr_assign_list* node, xec_ssa_valist* values, int valcount )
{
    // Evaluate lvalues ready for assignment.
    std::deque< xec_ssa_lvalue > lvalues;
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        xec_ssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        lvalues.push_back( lvalue );
    }

    // Evaluate exactly the number of rvalues we require.
    xec_ssa_valist rvalues;
    b->unpack( &rvalues, node->rvalues, (int)lvalues.size() );
    
    // An assignment list only has a maximum number of values.
    for ( int i = 0; i < lvalues.size(); ++i )
    {
        xec_ssa_lvalue lvalue = lvalues.at( i );
        xec_ssa_node* v = b->lvalue_assign( &lvalue, rvalues.values.at( i ) );
        if ( valcount != -1 && (int)i < valcount )
        {
            values->values.push_back( v );
        }
    }
    
    // And fill in the rest with null.
    if ( valcount != -1 && valcount > (int)lvalues.size() )
    {
        int required = valcount - (int)lvalues.size();
        xec_ssa_node* nullval = b->node( b->packed( node->sloc, XEC_SSA_NULL ) );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}



xec_ssa_build_stmt::xec_ssa_build_stmt( xec_ssa_builder* b )
    :   b( b )
{
}

void xec_ssa_build_stmt::fallback( xec_ast_node* node )
{
    assert( ! "expected statement" );
}

void xec_ssa_build_stmt::visit( xec_stmt_block* node )
{
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        visit( node->stmts.at( i ) );
    }
    
    if ( node->scope )
    {
        b->close_scope( node->scope );
    }
}

void xec_ssa_build_stmt::visit( xec_stmt_if* node )
{
    b->ifthen( b->expr( node->condition ) );
    if ( node->iftrue )
    {
        visit( node->iftrue );
    }
    if ( node->iffalse )
    {
        b->ifelse();
        visit( node->iffalse );
    }
    b->ifend();
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_switch* node )
{
    b->switchopen( b->expr( node->value ) );
    visit( node->body );
    b->switchend();
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_while* node )
{
    b->loopopen();
    b->ifthen( b->expr( node->condition ) );
    visit( node->body );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->ifend();
    b->loopend();
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_do* node )
{
    b->loopopen();
    visit( node->body );
    b->ifthen( b->expr( node->condition ) );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->loopend();
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_foreach* node )
{
    // Construct iterator.
    xec_ssa_opcode opcode = node->eachkey ? XEC_SSA_EACH : XEC_SSA_ITER;
    xec_ssa_node* list = b->expr( node->list );
    xec_ssa_node* iter = b->node( b->packed( node->sloc, opcode, list ) );

    // Begin loop.
    b->loopopen();
    
    // Get values for this iteration.
    int request = 1 + (int)node->lvalues.size();
    xec_ssa_node* next = b->node( b->packed(
                    node->sloc, XEC_SSA_NEXT, iter, request ) );
    xec_ssa_node* cond = b->node( b->packed(
                    node->sloc, XEC_SSA_SELECT, next, 0 ) );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        xec_ssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        xec_ssa_node* rvalue = b->node( b->packed(
                        node->sloc, XEC_SSA_SELECT, next, 1 + (int)i ) );
        b->lvalue_assign( &lvalue, rvalue );
    }
    
    // Loop body.
    b->ifthen( cond );
    visit( node->body );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->ifend();
    
    // Close scopes.
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_for* node )
{
    if ( node->init )
    {
        visit( node->init );
    }
    b->loopopen();
    if ( node->condition )
    {
        b->ifthen( b->expr( node->condition ) );
    }
    visit( node->body );
    if ( node->update )
    {
        visit( node->update );
    }
    b->loopcontinue();
    if ( node->condition )
    {
        b->ifelse();
        b->loopbreak();
    }
    b->loopend();
    b->close_scope( node->scope );
}

void xec_ssa_build_stmt::visit( xec_stmt_using* node )
{
}

void xec_ssa_build_stmt::visit( xec_stmt_try* node )
{
}

void xec_ssa_build_stmt::visit( xec_stmt_catch* node )
{
}

void xec_ssa_build_stmt::visit( xec_stmt_delete* node )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        xec_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == XEC_EXPR_KEY )
        {
            xec_expr_key* expr = (xec_expr_key*)node;
            xec_ssa_node* object = b->expr( expr->object );
            b->node( b->packed( delval->sloc, XEC_SSA_DELKEY, object, expr->key ) );
        }
        else if ( delval->kind == XEC_EXPR_INKEY )
        {
            xec_expr_inkey* expr = (xec_expr_inkey*)node;
            xec_ssa_node* object = b->expr( expr->object );
            xec_ssa_node* key = b->expr( expr->key );
            b->node( b->packed( delval->sloc, XEC_SSA_DELINKEY, object, key ) );
        }
    }
}

void xec_ssa_build_stmt::visit( xec_stmt_case* node )
{
    b->switchcase();
    xec_ssa_node* value = b->expr( node->value );
    b->switchcase( value );
}

void xec_ssa_build_stmt::visit( xec_stmt_continue* node )
{
    // If we're in a for loop, update the for before continuing.
    if ( node->target->node->kind == XEC_STMT_FOR )
    {
        xec_stmt_for* forstmt = (xec_stmt_for*)node->target;
        if ( forstmt->update )
        {
            visit( forstmt->update );
        }
    }
    
    // Close scopes we're exiting.
    for ( xec_ast_scope* scope = node->scope;
                    scope != node->target; scope = scope->outer )
    {
        b->close_scope( scope );
    }
    
    // Continue with the loop.
    b->loopcontinue();
}

void xec_ssa_build_stmt::visit( xec_stmt_break* node )
{
    // Close scopes we're exiting.
    for ( xec_ast_scope* scope = node->scope;
                    scope != node->target; scope = scope->outer )
    {
        b->close_scope( scope );
    }
    
    // Break out of either for loop or switch block.
    if ( node->target->node->kind == XEC_STMT_SWITCH )
    {
        b->switchbreak();
    }
    else
    {
        b->loopbreak();
    }
}

void xec_ssa_build_stmt::visit( xec_stmt_return* node )
{
    // Close scopes we're exiting.
    xec_ast_scope* scope = node->scope;
    while ( true )
    {
        b->close_scope( scope );
        if ( scope->node->kind == XEC_AST_FUNC )
            break;
        scope = scope->outer;
    }
    
    // Evaluate return values.
    xec_ssa_valist values;
    b->unpack( &values, node->values, -1 );
    
    // Emit return operation.
    xec_ssa_expand* retstmt = b->expand( node->sloc, XEC_SSA_RETURN, 0 );
    extend( &retstmt->operands, values.values );
    retstmt->unpacked = values.unpacked;
    b->node( retstmt );
}

void xec_ssa_build_stmt::visit( xec_stmt_throw* node )
{
}








/*
    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/



/*
    Information about currently building function.
*/

struct xec_ssa_build_if
{
    xec_ssa_build_if();

    xec_ssa_build_block*    prev;
    xec_ssa_build_block*    then;
};

xec_ssa_build_if::xec_ssa_build_if()
    :   prev( NULL )
    ,   then( NULL )
{
}



struct xec_ssa_build_switch
{
    xec_ssa_build_switch();
    
    xec_ssa_node*           value;
    xec_ssa_build_block*    head;
    xec_ssa_build_block*    prev;
    std::deque< xec_ssa_build_block* > breaks;
};

xec_ssa_build_switch::xec_ssa_build_switch()
    :   head( NULL )
{
}


struct xec_ssa_build_loop
{
    xec_ssa_build_loop();
    
    xec_ssa_build_block*    loop;
    std::deque< xec_ssa_build_block* > breaks;
};


xec_ssa_build_loop::xec_ssa_build_loop()
    :   loop( NULL )
{
}


struct xec_ssa_build_func
{
    explicit xec_ssa_build_func( xec_ssa_func* func );

    xec_ssa_func*                       func;
    xec_ssa_build_block*                block;
    std::deque< xec_ssa_build_if >      ifstack;
    std::deque< xec_ssa_build_switch >  switchstack;
    std::deque< xec_ssa_build_loop >    loopstack;
    std::unordered_map< xec_ssa_block*, xec_ssa_build_block* > blockmap;
    std::deque< std::unique_ptr< xec_ssa_build_block > > blocks;
};


xec_ssa_build_func::xec_ssa_build_func( xec_ssa_func* func )
    :   func( func )
    ,   block( NULL )
{
}



/*
    A block which is being built.
*/

struct xec_ssa_build_block
{
    xec_ssa_build_block();

    xec_ssa_block*                              block;
    std::unordered_map< void*, xec_ssa_node* >  defined;
    std::unordered_set< void* >                 incomplete;
    int                                         visitmark;
    bool                                        sealed;
};


xec_ssa_build_block::xec_ssa_build_block()
    :   block( NULL )
    ,   visitmark( -1 )
    ,   sealed( false )
{
}




/*
    The SSA builder itself.
*/


xec_ssa_builder::xec_ssa_builder( xec_ssa* root )
    :   root( root )
    ,   build_expr( this )
    ,   build_unpack( this )
    ,   build_stmt( this )
{
}

xec_ssa_builder::~xec_ssa_builder()
{
}


bool xec_ssa_builder::build( xec_ast* ast )
{
    region_scope rscope( root->alloc );

    // Set script.
    root->script = ast->script;
    
    // Construct function objects.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        xec_ast_func* func = ast->functions.at( i );
        xec_ssa_func* ssaf = new ( root->alloc ) xec_ssa_func(
                func->sloc, func->funcname, NULL, (int)func->upvals.size(),
                (int)func->parameters.size(), func->varargs, func->coroutine );
        root->functions.push_back( ssaf );
        funcmap.emplace( func, ssaf );
    }
    
    // Perform AST -> SSA conversion, one function at a time.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        xec_ast_func* func = ast->functions.at( i );
        xec_ssa_func* ssaf = funcmap.at( func );
        build_function( func, ssaf );
    }
    
    root->function = funcmap.at( ast->function );
    return root->script->error_count() == 0;
}

xec_ssa_node* xec_ssa_builder::node( xec_ssa_node* node )
{
    if ( b->block )
    {
        b->block->block->nodes.push_back( node );
        return node;
    }
    else
    {
        // Unreachable code.
        return node;
    }
}


void xec_ssa_builder::define( xec_ast_name* name, xec_ssa_node* node )
{
}

void xec_ssa_builder::define( xec_new_object* object, xec_ssa_node* node )
{
}

void xec_ssa_builder::define( xec_ast_node* temporary, xec_ssa_node* node )
{
}


xec_ssa_node* xec_ssa_builder::lookup( xec_ast_name* name )
{
    return NULL;
}

xec_ssa_node* xec_ssa_builder::lookup( xec_new_object* object )
{
    return NULL;
}

xec_ssa_node* xec_ssa_builder::lookup( xec_ast_node* temporary )
{
    return NULL;
}


void xec_ssa_builder::close_scope( xec_ast_scope* scope )
{
    // Close all upvals in scope.
    for ( auto i = scope->decls.rbegin(); i != scope->decls.rend(); ++i )
    {
        xec_ast_name* decl = *i;
        if ( decl->upval )
        {
            // TODO.
        }
    }
    
    // Close object upval if there is one.
    if ( scope->node->kind == XEC_NEW_OBJECT )
    {
        xec_new_object* object = (xec_new_object*)scope->node;
        if ( object->upval )
        {
            // TODO.
        }
    }
}


void xec_ssa_builder::ifthen( xec_ssa_node* condition )
{
    b->ifstack.emplace_back();
    xec_ssa_build_if& buildif = b->ifstack.back();
    xec_ssa_build_block* iftrue = make_block();
    
    if ( b->block )
    {
        // Link new block as the iftrue branch from the current block.
        buildif.prev = b->block;
        link_iftrue( buildif.prev, condition, iftrue );
    }
    else
    {
        // Unreachable code.
    }
    
    seal_block( iftrue );
    b->block = iftrue;
}

void xec_ssa_builder::ifelse()
{
    xec_ssa_build_if& buildif = b->ifstack.back();
    xec_ssa_build_block* iffalse = make_block();
    
    if ( b->block )
    {
        // Current block is the end of the iftrue branch, and it will be
        // linked to the after block.
        buildif.then = b->block;
    }

    if ( buildif.prev )
    {
        // Link new block as iffalse branch from the block before the if.
        link_iffalse( buildif.prev, iffalse );
    }
    else
    {
        // Unreachable code.
    }
    
    seal_block( iffalse );
    b->block = iffalse;
}

void xec_ssa_builder::ifend()
{
    xec_ssa_build_if& buildif = b->ifstack.back();
    xec_ssa_build_block* after = make_block();
    
    if ( buildif.then )
    {
        // Link end of iftrue branch to this block.
        link_next( buildif.then, after );
    }
    
    if ( b->block )
    {
        // Current block is the end of the iffalse branch.
        link_next( b->block, after );
    }
    
    seal_block( after );
    b->ifstack.pop_back();
    b->block = after;
}


void xec_ssa_builder::switchopen( xec_ssa_node* value )
{
    b->switchstack.emplace_back();
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    buildswitch.value = value;
    buildswitch.head = b->block;
    b->block = NULL;
}


void xec_ssa_builder::switchcase()
{
    xec_ssa_build_switch& buildswitch = b->switchstack.back();

    // Remember previous current block (for fallthrough).
    buildswitch.prev = b->block;
    
    if ( buildswitch.head )
    {
        if ( buildswitch.head->block->condition )
        {
            // There has already been another comparison.
            xec_ssa_build_block* testblock = make_block();
            link_iffalse( buildswitch.head, testblock );
            seal_block( testblock );
            b->block = testblock;
        }
        else
        {
            // This is the first comparison.
            assert( ! buildswitch.head->block->next );
            b->block = buildswitch.head;
        }
    }
    else
    {
        // Unreachable code.
        b->block = NULL;
    }
}


void xec_ssa_builder::switchcase( xec_ssa_node* value )
{
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    
    xec_ssa_build_block* caseblock = NULL;
    if ( buildswitch.prev )
    {
        if ( buildswitch.prev->block->nodes.empty() )
        {
            // Reuse empty case block from previous case.
            assert( ! buildswitch.prev->block->condition );
            assert( ! buildswitch.prev->block->next );
            assert( buildswitch.prev->block->phi.empty() );
            caseblock = buildswitch.prev;
            
            // Can only do this because the block is totally empty.
            caseblock->sealed = false;
        }
        else
        {
            // Make new case block (with fallthrough).
            caseblock = make_block();
            link_next( buildswitch.prev, caseblock );
        }
        
        buildswitch.prev = NULL;
    }
    else
    {
        // Make new case block (no fallthrough).
        caseblock = make_block();
    }
    
    
    // Add comparison.
    xec_ssa_node* compare = node( packed(
                value->sloc, XEC_SSA_EQ, buildswitch.value, value ) );
    

    // Link case block.
    if ( b->block )
    {
        link_iftrue( b->block, compare, caseblock );
    }
    else
    {
        // Unreachable code.
    }
    
    
    // Next case, the head of the switch will be the current block, so
    // we can chain more cases.
    buildswitch.head = b->block;
    
    
    // And we want to add statements to the case block.
    seal_block( caseblock );
    b->block = caseblock;
}

void xec_ssa_builder::switchbreak()
{
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    
    if ( b->block )
    {
        // This block's next block is the one after the switch.
        buildswitch.breaks.push_back( b->block );
    }
    
    // Further statements are unreachable.
    b->block = NULL;
}

void xec_ssa_builder::switchend()
{
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    xec_ssa_build_block* after = make_block();
    
    if ( buildswitch.head )
    {
        // Failed comparisons end up outside of the switch.
        if ( buildswitch.head->block->condition )
        {
            link_iffalse( buildswitch.head, after );
        }
        else
        {
            link_next( buildswitch.head, after );
        }
    }
    
    for ( size_t i = 0; i < buildswitch.breaks.size(); ++i )
    {
        // Each break block jumps to after the switch.
        xec_ssa_build_block* breakblock = buildswitch.breaks.at( i );
        link_next( breakblock, after );
    }
    
    seal_block( after );
    b->switchstack.pop_back();
    b->block = after;
}


void xec_ssa_builder::loopopen()
{
    b->loopstack.emplace_back();
    xec_ssa_build_loop& buildloop = b->loopstack.back();
    
    // The entry to the loop is the only kind of block that isn't
    // sealed immediately after creation (we don't know how many
    // continues are going to loop back to it).
    buildloop.loop = make_block();
    
    if ( b->block )
    {
        // Loop block follows current block.
        link_next( b->block, buildloop.loop );
    }
    else
    {
        // Unreachable code.
    }
    
    b->block = buildloop.loop;
}

void xec_ssa_builder::loopcontinue()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();

    if ( b->block )
    {
        link_next( b->block, buildloop.loop );
    }
    else
    {
        // Unreachable code.
    }
    
    // Further statements are unreachable.
    b->block = NULL;
}

void xec_ssa_builder::loopbreak()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();

    if ( b->block )
    {
        // This block's next block is the one after the loop.
        buildloop.breaks.push_back( b->block );
    }

    // Further statements are unreachable.
    b->block = NULL;
}

void xec_ssa_builder::loopend()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();

    // We should either have broken or continued by now.
    assert( ! b->block );
    
    // No more continues.
    seal_block( buildloop.loop );
    
    // Create after block and link breaks to it.
    xec_ssa_build_block* after = make_block();
    for ( size_t i = 0; i < buildloop.breaks.size(); ++i )
    {
        // Each break block jumps to after the switch.
        xec_ssa_build_block* breakblock = buildloop.breaks.at( i );
        link_next( breakblock, after );
    }
    
    seal_block( after );
    b->loopstack.pop_back();
    b->block = after;
}


void xec_ssa_builder::funcreturn()
{
    // Further statements are unreachable.
    b->block = NULL;
}


xec_ssa_func* xec_ssa_builder::func( xec_ast_func* func )
{
    return funcmap.at( func );
}

xec_ssa_node* xec_ssa_builder::expr( xec_ast_node* node )
{
    return build_expr.visit( node );
}

void xec_ssa_builder::unpack( xec_ssa_valist* l, xec_ast_node* node, int count )
{
    build_unpack.visit( node, l, count );
    if ( count != -1 )
    {
        assert( (int)l->values.size() == count );
        assert( l->unpacked = NULL );
    }
}



void xec_ssa_builder::lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node )
{
    lvalue->sloc = node->sloc;

    switch ( node->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        xec_expr_local* local = (xec_expr_local*)node;
        lvalue->opcode = XEC_SSA_NOP;
        lvalue->local  = local->name;
        break;
    }
    
    case XEC_EXPR_UPREF:
    {
        xec_expr_upref* upref = (xec_expr_upref*)node;
        lvalue->opcode = XEC_SSA_SETUP;
        lvalue->upval  = upref->index;
        break;
    }
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)node;
        lvalue->opcode = XEC_SSA_SETKEY;
        lvalue->object = expr( key->object );
        lvalue->literal = key->key;
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node;
        lvalue->opcode = XEC_SSA_SETINKEY;
        lvalue->object = expr( inkey->object );
        lvalue->index = expr( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)node;
        lvalue->opcode = XEC_SSA_SETINDEX;
        lvalue->object = expr( index->object );
        lvalue->index = expr( index->index );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

xec_ssa_node* xec_ssa_builder::lvalue_value( xec_ssa_lvalue* lvalue )
{
    switch ( lvalue->opcode )
    {
    case XEC_SSA_NOP:
        return lookup( lvalue->local );
    
    case XEC_SSA_SETUP:
        return node( packed( lvalue->sloc,
                        XEC_SSA_REFUP, nullptr, lvalue->upval ) );
    
    case XEC_SSA_SETKEY:
        return node( packed( lvalue->sloc,
                        XEC_SSA_KEY, lvalue->object, lvalue->literal ) );
    
    case XEC_SSA_SETINKEY:
        return node( packed( lvalue->sloc,
                        XEC_SSA_INKEY, lvalue->object, lvalue->index ) );
    
    case XEC_SSA_SETINDEX:
        return node( packed( lvalue->sloc,
                        XEC_SSA_INDEX, lvalue->object, lvalue->index ) );
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

xec_ssa_node* xec_ssa_builder::lvalue_assign(
            xec_ssa_lvalue* lvalue, xec_ssa_node* val )
{
    switch ( lvalue->opcode )
    {
    case XEC_SSA_NOP:
        define( lvalue->local, val );
        break;
        
    case XEC_SSA_SETUP:
        node( packed( lvalue->sloc, XEC_SSA_SETUP, val, lvalue->upval ) );
        break;
        
    case XEC_SSA_SETKEY:
        node( triple( lvalue->sloc,
                    XEC_SSA_SETKEY, lvalue->object, lvalue->literal, val ) );
        break;
        
    case XEC_SSA_SETINKEY:
    case XEC_SSA_SETINDEX:
        node( triple( lvalue->sloc,
                    lvalue->opcode, lvalue->object, lvalue->index, val ) );
        break;
        
    default:
        assert( ! "invalid lvalue" );
    }

    return val;
}




void xec_ssa_builder::build_function( xec_ast_func* astf, xec_ssa_func* ssaf )
{
    // Set up function building.
    std::unique_ptr< xec_ssa_build_func > c =
            std::make_unique< xec_ssa_build_func >( ssaf );
    std::swap( b, c );
    
    
    // Open block and add parameters.
    b->block = make_block();
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        xec_ast_name* param = astf->parameters.at( i );
        xec_ssa_node* n = node( packed(
                        param->sloc, XEC_SSA_PARAM, nullptr, (int)i ) );
        define( param, n );
    }


    // Visit function body.
    build_stmt.visit( astf->block );
    
    
    // Close scope and return null.
    close_scope( astf->scope );
    xec_ssa_node* nullval = node( packed( astf->sloc, XEC_SSA_NULL ) );
    xec_ssa_expand* retstmt = expand( astf->sloc, XEC_SSA_RETURN, 0 );
    retstmt->operands.push_back( nullval );
    node( retstmt );
    funcreturn();
    
    
    // Clean up after building.
    ssaf->block = b->block->block;
    std::swap( b, c );
}



xec_ssa_build_block* xec_ssa_builder::make_block()
{
    std::unique_ptr< xec_ssa_build_block > block =
                    std::make_unique< xec_ssa_build_block >();
    block->block = new ( root->alloc ) xec_ssa_block();
    b->func->blocks.push_back( block->block );
    b->blockmap.emplace( block->block, block.get() );
    b->blocks.push_back( std::move( block ) );
    return b->blocks.back().get();
}


void xec_ssa_builder::link_next(
            xec_ssa_build_block* block, xec_ssa_build_block* next )
{
    assert( ! block->block->condition );
    assert( ! block->block->next );
    assert( ! next->sealed );
    block->block->next = next->block;
    next->block->previous.push_back( block->block );
}

void xec_ssa_builder::link_iftrue( xec_ssa_build_block* block,
            xec_ssa_node* condition, xec_ssa_build_block* iftrue )
{
    assert( ! block->block->condition );
    assert( ! block->block->next );
    assert( ! iftrue->sealed );
    block->block->condition = condition;
    block->block->iftrue = iftrue->block;
    iftrue->block->previous.push_back( block->block );
}

void xec_ssa_builder::link_iffalse(
            xec_ssa_build_block* block, xec_ssa_build_block* iffalse )
{
    assert( block->block->condition );
    assert( block->block->iftrue );
    assert( ! block->block->iffalse );
    assert( ! iffalse->sealed );
    block->block->iffalse = iffalse->block;
    iffalse->block->previous.push_back( block->block );
}




void xec_ssa_builder::seal_block( xec_ssa_build_block* block )
{
    assert( ! block->sealed );
    
    // TODO: Lookup incomplete phi instructions.
    
    block->sealed = true;
}


