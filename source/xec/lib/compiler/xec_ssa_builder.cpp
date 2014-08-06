//
//  xec_ssa_builder.cpp
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_builder.h"
#include "xec_ssa.h"
#include "xec_ast_visitor.h"



/*
    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/



template < typename containera_t, typename containerb_t >
static inline void extend( containera_t* a, const containerb_t& b )
{
    a->insert( a->end(), b.begin(), b.end() );
}





/*
    xec_ssa_lvalue
*/

struct xec_ssa_lvalue
{
    xec_ssa_opcode  opcode;
    xec_ssa_node*   object;
    union
    {
    xec_ssa_node*   index;
    const char*     literal;
    int             upval;
    };
};




/*
    xec_ssa_valist
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
    // Build SSA of function recursively.
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
                        node->sloc, XEC_SSA_UPREF, nullptr, upval.upval ) );
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
                XEC_SSA_UPREF, nullptr, node->index ) );
                
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
    
    // !! TODO: close upvals in scope (if object is an upval, close it...)
    
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
    :   b(b )
{
}

void xec_ssa_build_unpack::fallback(
                xec_ast_node* node, xec_ssa_valist* values, int valcount )
{
    // Expression produces only one value.
    values->values.push_back( b->expr( node ) );
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
    if ( node->final )
    {
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
    
        xec_ssa_valist final;
        visit( node->final, &final, required );
        extend( &values->values, final.values );
        values->unpacked = final.unpacked;
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
        xec_ssa_node* null = b->node( b->packed( node->sloc, XEC_SSA_NULL ) );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( null );
        }
    }
}










