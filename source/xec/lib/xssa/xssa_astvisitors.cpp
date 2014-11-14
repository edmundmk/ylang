//
//  xssa_astvisitors.cpp
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_astvisitors.h"
#include "xssa_builder.h"
#include <vector>





/*
    Common between visitors.
*/

static xssa_opcode assign_opcode( xec_ast_opkind assignop )
{
    switch ( assignop )
    {
    case XEC_ASTOP_MULASSIGN:       return XSSA_MUL;
    case XEC_ASTOP_DIVASSIGN:       return XSSA_DIV;
    case XEC_ASTOP_MODASSIGN:       return XSSA_MOD;
    case XEC_ASTOP_INTDIVASSIGN:    return XSSA_INTDIV;
    case XEC_ASTOP_ADDASSIGN:       return XSSA_ADD;
    case XEC_ASTOP_SUBASSIGN:       return XSSA_SUB;
    case XEC_ASTOP_LSHIFTASSIGN:    return XSSA_LSL;
    case XEC_ASTOP_LRSHIFTASSIGN:   return XSSA_LSR;
    case XEC_ASTOP_ARSHIFTASSIGN:   return XSSA_ASR;
    case XEC_ASTOP_BITANDASSIGN:    return XSSA_AND;
    case XEC_ASTOP_BITXORASSIGN:    return XSSA_XOR;
    case XEC_ASTOP_BITORASSIGN:     return XSSA_OR;
    default: assert( ! "invalid operator" ); break;
    }

    return XSSA_NOP;
}





/*
    xssa_build_expr
*/

xssa_build_expr::xssa_build_expr( xssa_builder* b )
    :   b( b )
{
}

xssaop* xssa_build_expr::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}

xssaop* xssa_build_expr::visit( xec_ast_func* node )
{
    // Locate (possibly unbuilt) SSA of function.
    xssa_func* func = b->func( node );
    
    // Retrieve upvals.
    xssaop* l = xssaop::op( node->sloc, XSSA_CLOSURE, node->upvals.size() );
    l->func = func;
    for ( size_t i = 0; i < node->upvals.size(); ++i )
    {
        const xec_ast_upval& upval = node->upvals.at( i );
        switch ( upval.kind )
        {
        case XEC_UPVAL_LOCAL:
        {
            l->operand[ i ] = b->lookup( node->sloc, upval.local );
            break;
        }
        
        case XEC_UPVAL_OBJECT:
        {
            l->operand[ i ] = b->lookup( upval.object );
            break;
        }
        
        case XEC_UPVAL_UPVAL:
        {
            l->operand[ i ] = b->op( node->sloc, XSSA_REFUP );
            l->operand[ i ]->index = upval.upval;
            break;
        }
        }
    }
    
    // Construct closure.
    return b->op( l );
}

xssaop* xssa_build_expr::visit( xec_expr_null* node )
{
    return b->op( node->sloc, XSSA_NULL );
}

xssaop* xssa_build_expr::visit( xec_expr_bool* node )
{
    xssaop* boolean = b->op( node->sloc, XSSA_BOOL );
    boolean->boolean = node->value;
    return boolean;
}

xssaop* xssa_build_expr::visit( xec_expr_number* node )
{
    xssaop* number = b->op( node->sloc, XSSA_NUMBER );
    number->number = node->value;
    return number;
}

xssaop* xssa_build_expr::visit( xec_expr_string* node )
{
    xssaop* s = b->op( node->sloc, XSSA_STRING );
    s->string = xssa_string::s( node->sloc, node->length, node->string );
    return s;
}

xssaop* xssa_build_expr::visit( xec_expr_local* node )
{
    return b->lookup( node->sloc, node->name );
}

xssaop* xssa_build_expr::visit( xec_expr_global* node )
{
    xssaop* global = b->op( node->sloc, XSSA_GLOBAL );
    global->key = b->key( node->name );
    return global;
}

xssaop* xssa_build_expr::visit( xec_expr_upref* node )
{
    xssaop* refup = b->op( node->sloc, XSSA_REFUP );
    refup->index = node->index;
    return refup;
}

xssaop* xssa_build_expr::visit( xec_expr_objref* node )
{
    return b->lookup( node->object );
}

xssaop* xssa_build_expr::visit( xec_expr_key* node )
{
    xssaop* object = visit( node->object );
    xssaop* key = b->op( node->sloc, XSSA_KEY, object );
    key->key = b->key( node->key );
    return key;
}

xssaop* xssa_build_expr::visit( xec_expr_inkey* node )
{
    xssaop* object = visit( node->object );
    xssaop* key    = visit( node->key );
    return b->op( node->sloc, XSSA_INKEY, object, key );
}

xssaop* xssa_build_expr::visit( xec_expr_index* node )
{
    xssaop* object = visit( node->object );
    xssaop* index  = visit( node->index );
    return b->op( node->sloc, XSSA_INDEX, object, index );
}

xssaop* xssa_build_expr::visit( xec_expr_preop* node )
{
    xssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    xssaop* value   = b->lvalue_value( &lvalue );
    xssaop* literal = b->op( node->sloc, XSSA_NUMBER );
    literal->number = 1.0;
    
    xssa_opcode opcode = XSSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_PREINC:   opcode = XSSA_ADD;   break;
    case XEC_ASTOP_PREDEC:   opcode = XSSA_SUB;   break;
    default:
        assert( ! "invalid operator" );
        break;
    }
    
    xssaop* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return updated;
}

xssaop* xssa_build_expr::visit( xec_expr_postop* node )
{
    xssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    xssaop* value   = b->lvalue_value( &lvalue );
    xssaop* literal = b->op( node->sloc, XSSA_NUMBER );
    literal->number = 1.0;
    
    xssa_opcode opcode = XSSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSTINC:  opcode = XSSA_ADD;   break;
    case XEC_ASTOP_POSTDEC:  opcode = XSSA_SUB;   break;
    default: assert( ! "invalid operator" ); break;
    }
    
    xssaop* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return value;
}

xssaop* xssa_build_expr::visit( xec_expr_unary* node )
{
    xssaop* operand = visit( node->operand );
    
    xssa_opcode opcode = XSSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_POSITIVE: opcode = XSSA_POS;         break;
    case XEC_ASTOP_NEGATIVE: opcode = XSSA_NEG;         break;
    case XEC_ASTOP_BITNOT:   opcode = XSSA_NOT;         break;
    case XEC_ASTOP_LOGICNOT: opcode = XSSA_LNOT;        break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operand );
}

xssaop* xssa_build_expr::visit( xec_expr_binary* node )
{
    xssaop* operanda = visit( node->lhs );
    xssaop* operandb = visit( node->rhs );
    
    xssa_opcode opcode = XSSA_NOP;
    switch ( node->opkind )
    {
    case XEC_ASTOP_MULTIPLY:     opcode = XSSA_MUL;     break;
    case XEC_ASTOP_DIVIDE:       opcode = XSSA_DIV;     break;
    case XEC_ASTOP_MODULUS:      opcode = XSSA_MOD;     break;
    case XEC_ASTOP_INTDIV:       opcode = XSSA_INTDIV;  break;
    case XEC_ASTOP_ADD:          opcode = XSSA_ADD;     break;
    case XEC_ASTOP_SUBTRACT:     opcode = XSSA_SUB;     break;
    case XEC_ASTOP_LSHIFT:       opcode = XSSA_LSL;     break;
    case XEC_ASTOP_LRSHIFT:      opcode = XSSA_LSR;     break;
    case XEC_ASTOP_ARSHIFT:      opcode = XSSA_ASR;     break;
    case XEC_ASTOP_BITAND:       opcode = XSSA_AND;     break;
    case XEC_ASTOP_BITXOR:       opcode = XSSA_XOR;     break;
    case XEC_ASTOP_BITOR:        opcode = XSSA_OR;      break;
    case XEC_ASTOP_CONCATENATE:  opcode = XSSA_CONCAT;  break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operanda, operandb );
}

xssaop* xssa_build_expr::visit( xec_expr_compare* node )
{
    xssaop* lhs = visit( node->first );
    xssaop* result = nullptr;
    
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
        xssaop* rhs = visit( node->terms.at( i ) );
        
        /*
            We have a limited set of comparison opcodes:
                l > r   -> r < l
                l >= r  -> r <= l
                l != r  -> not ( l == r )
                l !in r -> not ( l in r )
                l !is r -> not ( l is r )
        */
        
        xssa_opcode opcode = XSSA_NOP;
        bool inv = false, neg = false;
        switch ( opkind )
        {
        case XEC_ASTOP_EQUAL:        opcode = XSSA_EQ;               break;
        case XEC_ASTOP_NOTEQUAL:     opcode = XSSA_EQ; neg = true;   break;
        case XEC_ASTOP_LESS:         opcode = XSSA_LT;               break;
        case XEC_ASTOP_GREATER:      opcode = XSSA_LE; inv = true;   break;
        case XEC_ASTOP_LESSEQUAL:    opcode = XSSA_LE;               break;
        case XEC_ASTOP_GREATEREQUAL: opcode = XSSA_LT; inv = true;   break;
        case XEC_ASTOP_IN:           opcode = XSSA_IN;               break;
        case XEC_ASTOP_NOTIN:        opcode = XSSA_IN; neg = true;   break;
        case XEC_ASTOP_IS:           opcode = XSSA_IS;               break;
        case XEC_ASTOP_NOTIS:        opcode = XSSA_IS; neg = true;   break;
        default: assert( ! "invalid operator" ); break;
        }
        
        if ( ! inv )
            result = b->op( node->sloc, opcode, lhs, rhs );
        else
            result = b->op( node->sloc, opcode, rhs, lhs );
        if ( neg )
            result = b->op( node->sloc, XSSA_NOT, result );
        
        // Keep the result in a temporary, as if it is false we shortcut
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

xssaop* xssa_build_expr::visit( xec_expr_logical* node )
{
    switch ( node->opkind )
    {
    case XEC_ASTOP_LOGICAND:
    {
        // Only evaluate rhs if lhs is true.
        xssaop* lhs = visit( node->lhs );
        b->define( node, lhs );
        b->ifthen( lhs );
        xssaop* rhs = visit( node->rhs );
        b->define( node, rhs );
        b->ifend();
        return b->lookup( node );
    }
    
    case XEC_ASTOP_LOGICXOR:
    {
        // Does not perform shortcut evaluation.
        xssaop* lhs = visit( node->lhs );
        xssaop* rhs = visit( node->rhs );
        return b->op( node->sloc, XSSA_XOR, lhs, rhs );
    }
    
    case XEC_ASTOP_LOGICOR:
    {
        // Only evaluate rhs if the lhs is false.
        xssaop* lhs = visit( node->lhs );
        b->define( node, lhs );
        xssaop* invlhs = b->op( node->sloc, XSSA_NOT, lhs );
        b->ifthen( invlhs );
        xssaop* rhs = visit( node->rhs );
        b->define( node, rhs );
        b->ifend();
        return b->lookup( node );
    }
    
    default:
        assert( ! "invalid operator" );
        break;
    }
    
    return nullptr;
}

xssaop* xssa_build_expr::visit( xec_expr_qmark* node )
{
    xssaop* condition = visit( node->condition );
    b->ifthen( condition );
    xssaop* iftrue = visit( node->iftrue );
    b->define( node, iftrue );
    b->ifelse();
    xssaop* iffalse = visit( node->iffalse );
    b->define( node, iffalse );
    b->ifend();
    return b->lookup( node );
}

xssaop* xssa_build_expr::visit( xec_new_new* node )
{
    // Get prototype.
    xssaop* proto = visit( node->proto );

    // Create object inheriting from prototype.
    xssaop* object = b->op( node->sloc, XSSA_OBJECT, proto );
    
    // Lookup 'this' method.
    xssaop* constructor = b->op( node->sloc, XSSA_KEY, object );
    constructor->key = b->key( "this" );

    // Get arguments (unpacked).
    xssa_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );

    // Call constructor.
    xssaop* call = b->op( node->sloc, XSSA_CALL, 2 + arguments.values.size() );
    call->result_count = 0;
    call->operand[ 0 ] = constructor;
    call->operand[ 1 ] = object;
    for ( size_t i = 0; i < arguments.values.size(); ++i )
        call->operand[ 2 + i ] = arguments.values.at( i );
    call->multival = arguments.multival;
    
    // The result is the object.
    return object;
}

xssaop* xssa_build_expr::visit( xec_new_object* node )
{
    // Construct object.
    xssaop* proto = node->proto ? visit( node->proto ) : nullptr;
    xssaop* object = b->op( node->sloc, XSSA_OBJECT, proto );
    
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

xssaop* xssa_build_expr::visit( xec_new_array* node )
{
    // Construct array.
    xssaop* array = b->op( node->sloc, XSSA_ARRAY );
    array->index = (int)node->values.size();
    
    // Append all values.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xec_ast_node* elem = node->values.at( i );
        xssaop* value = visit( node->values.at( i ) );
        b->op( elem->sloc, XSSA_APPEND, array, value );
    }
    
    if ( node->final )
    {
        // Unpack final values and extend the array with them.
        xssa_valist values;
        b->unpack( &values, node->final, -1 );
        
        for ( size_t i = 0; i < values.values.size(); ++i )
        {
            xssaop* value = values.values.at( i );
            b->op( node->final->sloc, XSSA_APPEND, array, value );
        }
        
        if ( values.multival )
        {
            xssaop* extend = b->op( node->final->sloc, XSSA_EXTEND, array );
            extend->multival = values.multival;
        }
    }

    return array;
}

xssaop* xssa_build_expr::visit( xec_new_table* node )
{
    // Construct table.
    xssaop* table = b->op( node->sloc, XSSA_TABLE );
    table->index = (int)node->elements.size();
    
    // Add elements.
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const xec_key_value& e = node->elements.at( i );
        xssaop* key   = visit( e.key );
        xssaop* value = visit( e.value );
        b->op( node->sloc, XSSA_SETINDEX, table, key, value );
    }
    
    return table;
}

xssaop* xssa_build_expr::visit( xec_expr_mono* node )
{
    return visit( node->expr );
}

xssaop* xssa_build_expr::visit( xec_expr_call* node )
{
    xssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xssaop* xssa_build_expr::visit( xec_expr_yield* node )
{
    xssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xssaop* xssa_build_expr::visit( xec_expr_vararg* node )
{
    // Fetch the first variable argument.
    xssaop* vararg = b->op( node->sloc, XSSA_VARARG );
    vararg->index = 0;
    return vararg;
}

xssaop* xssa_build_expr::visit( xec_expr_unpack* node )
{
    xssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xssaop* xssa_build_expr::visit( xec_expr_list* node )
{
    xssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

xssaop* xssa_build_expr::visit( xec_expr_assign* node )
{
    xssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    
    if ( node->assignop == XEC_ASTOP_DECLARE
            || node->assignop == XEC_ASTOP_ASSIGN )
    {
        xssaop* rvalue = visit( node->rvalue );
        b->lvalue_assign( &lvalue, rvalue );
        return rvalue;
    }
    
    xssa_opcode opcode = assign_opcode( node->assignop );
    xssaop* lhs = b->lvalue_value( &lvalue );
    xssaop* rhs = visit( node->rvalue );
    xssaop* rvalue = b->op( node->sloc, opcode, lhs, rhs );
    b->lvalue_assign( &lvalue, rvalue );
    return rvalue;
}

xssaop* xssa_build_expr::visit( xec_expr_assign_list* node )
{
    xssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}





xssa_build_unpack::xssa_build_unpack( xssa_builder* b )
    :   b( b )
{
}

void xssa_build_unpack::fallback(
                xec_ast_node* node, xssa_valist* values, int valcount )
{
    // Expression produces only one value.
    values->values.push_back( b->expr( node ) );
    
    // Unpack NULL for remaining values.
    if ( valcount > 1 )
    {
        xssaop* nullval = b->op( node->sloc, XSSA_NULL );
        for ( int i = 1; i < valcount; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}

void xssa_build_unpack::visit(
                xec_expr_null* node, xssa_valist* values, int valcount )
{
    // Special case to unpack NULL.
    xssaop* nullval = b->expr( node );
    values->values.push_back( nullval );
    for ( int i = 1; i < valcount; ++i )
    {
        values->values.push_back( nullval );
    }
}

void xssa_build_unpack::visit(
                xec_expr_call* node, xssa_valist* values, int valcount )
{
    // Only actually return multiple values if this was an unpack call.
    if ( ! node->unpack && valcount != 1 )
    {
        fallback( node, values, valcount );
        return;
    }

    // Get function and this value.
    xssaop* function = nullptr;
    xssaop* thisval  = nullptr;
    switch ( node->function->kind )
    {
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)node->function;
        thisval = b->expr( key->object );
        function = b->op( key->sloc, XSSA_KEY, thisval );
        function->key = b->key( key->key );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
        thisval = b->expr( inkey->object );
        xssaop* key = b->expr( inkey->key );
        function = b->op( inkey->sloc, XSSA_INKEY, thisval, key );
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
    xssa_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }

    // Construct call.
    xssa_opcode opcode = node->yieldcall ? XSSA_YCALL : XSSA_CALL;
    size_t operand_count = 1 + ( thisval ? 1 : 0 ) + arguments.values.size();
    xssaop* call = b->op( node->sloc, opcode, operand_count );
    call->result_count = valcount;
    size_t arg = 0;
    call->operand[ arg++ ] = function;
    if ( thisval )
        call->operand[ arg++ ] = thisval;
    for ( size_t i = 0; i < arguments.values.size(); ++i )
        call->operand[ arg++ ] = arguments.values.at( i );
    call->multival = arguments.multival;
    
    if ( valcount == 0 || valcount == 1 )
    {
        // There's at most one result.
        values->values.push_back( call );
    }
    else if ( valcount != -1 )
    {
        // We requested a finite number of values, select them.
        for ( int i = 0; i < valcount; ++i )
        {
            xssaop* sel = b->op( node->sloc, XSSA_SELECT, call );
            sel->immed = i;
            values->values.push_back( sel );
        }
    }
    else
    {
        // Call unpacked multiple results.
        values->multival = call;
    }
}

void xssa_build_unpack::visit(
                xec_expr_yield* node, xssa_valist* values, int valcount )
{
    // Only actually return multiple values if this is an unpack yield.
    if ( ! node->unpack && valcount != 1 )
    {
        fallback( node, values, valcount );
        return;
    }

    // Get arguments (unpacked).
    xssa_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }
    
    // Construct yield.
    xssaop* yield = b->op( node->sloc, XSSA_YIELD, arguments.values.size() );
    yield->result_count = valcount;
    for ( size_t i = 0; i < arguments.values.size(); ++i )
        yield->operand[ i ] = arguments.values.at( i );
    yield->multival = arguments.multival;

    if ( valcount == 0 || valcount == 1 )
    {
        // At most one value.
        values->values.push_back( yield );
    }
    else if ( valcount != -1 )
    {
        // We requested a finite number of values, select them.
        for ( int i = 0; i < valcount; ++i )
        {
            xssaop* sel = b->op( node->sloc, XSSA_SELECT, yield );
            sel->immed = i;
            values->values.push_back( sel );
        }
    }
    else
    {
        // Unpacked multiple results.
        values->multival = yield;
    }
}

void xssa_build_unpack::visit(
                xec_expr_vararg* node, xssa_valist* values, int valcount )
{
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xssaop* vararg = b->op( node->sloc, XSSA_VARARG );
            vararg->index = i;
            values->values.push_back( vararg );
        }
    }
    else
    {
        xssaop* varall = b->op( node->sloc, XSSA_VARALL );
        varall->result_count = -1;
        values->multival = varall;
    }
}

void xssa_build_unpack::visit(
                xec_expr_unpack* node, xssa_valist* values, int valcount )
{
    xssaop* array = b->expr( node->array );
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            xssaop* elem = b->op( node->sloc, XSSA_ELEM, array );
            elem->index = i;
            values->values.push_back( elem );
        }
    }
    else
    {
        xssaop* unpack = b->op( node->sloc, XSSA_UNPACK, array );
        unpack->result_count = -1;
        values->multival = unpack;
    }
}

void xssa_build_unpack::visit(
                xec_expr_list* node, xssa_valist* values, int valcount )
{
    // Only assign as many values as were requested.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xssaop* value = b->expr( node->values.at( i ) );
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
        xssa_valist final;
        visit( node->final, &final, required );
        values->values.insert( values->values.end(),
                    final.values.begin(), final.values.end() );
        values->multival = final.multival;
    }
    else if ( required > 0 )
    {
        // Fill in the rest with null.
        xssaop* nullval = b->op( node->sloc, XSSA_NULL );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}

void xssa_build_unpack::visit(
            xec_expr_assign_list* node, xssa_valist* values, int valcount )
{
    // Evaluate lvalues ready for assignment.
    std::deque< xssa_lvalue > lvalues;
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        xssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        lvalues.push_back( lvalue );
    }

    // Evaluate exactly the number of rvalues we require.
    xssa_valist rvalues;
    b->unpack( &rvalues, node->rvalues, (int)lvalues.size() );
    
    // An assignment list only has a maximum number of values.
    for ( int i = 0; i < lvalues.size(); ++i )
    {
        xssa_lvalue lvalue = lvalues.at( i );
        
        xssaop* v = nullptr;
        if ( node->assignop == XEC_ASTOP_DECLARE
                || node->assignop == XEC_ASTOP_ASSIGN )
        {
            v = rvalues.values.at( i );
            b->lvalue_assign( &lvalue, v );
        }
        else
        {
            xssa_opcode opcode = assign_opcode( node->assignop );
            xssaop* lhs = b->lvalue_value( &lvalue );
            xssaop* rhs = rvalues.values.at( i );
            v = b->op( node->sloc, opcode, lhs, rhs );
            b->lvalue_assign( &lvalue, v );
        }
        
        if ( valcount != -1 && (int)i < valcount )
        {
            values->values.push_back( v );
        }
    }
    
    // And fill in the rest with null.
    if ( valcount != -1 && valcount > (int)lvalues.size() )
    {
        int required = valcount - (int)lvalues.size();
        xssaop* nullval = b->op( node->sloc, XSSA_NULL );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}



xssa_build_stmt::xssa_build_stmt( xssa_builder* b )
    :   b( b )
{
}

void xssa_build_stmt::fallback( xec_ast_node* node )
{
    // Could be an expression statement.
    b->expr( node );
}

void xssa_build_stmt::visit( xec_stmt_block* node )
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

void xssa_build_stmt::visit( xec_stmt_if* node )
{
    b->ifthen( b->expr( node->condition ) );
    if ( node->iftrue )
    {
        visit( node->iftrue );
    }
    b->ifelse();
    if ( node->iffalse )
    {
        visit( node->iffalse );
    }
    b->ifend();
    b->close_scope( node->scope );
}

void xssa_build_stmt::visit( xec_stmt_switch* node )
{
    b->switchopen( b->expr( node->value ) );
    visit( node->body );
    b->switchend();
    b->close_scope( node->scope );
}

void xssa_build_stmt::visit( xec_stmt_while* node )
{
    b->loopopen( false );
    b->ifthen( b->expr( node->condition ) );
    visit( node->body );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->ifend();
    b->loopend();
    b->close_scope( node->scope );
}

void xssa_build_stmt::visit( xec_stmt_do* node )
{
    b->loopopen( true );
    visit( node->body );
    b->loopdowhile();
    b->ifthen( b->expr( node->condition ) );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->loopend();
    b->close_scope( node->scope );
}

void xssa_build_stmt::visit( xec_stmt_foreach* node )
{
    // Construct iterator.
    xssa_opcode opcode = node->eachkey ? XSSA_ITERKEY : XSSA_ITER;
    xssaop* list = b->expr( node->list );
    b->op( node->sloc, opcode, list );

    // Begin loop.
    b->loopopen( false );
    
    // Get values for this iteration.
    int request = (int)node->lvalues.size();
    xssaop* next = b->op( node->sloc, XSSA_NEXT );
    next->result_count = request;
    for ( int i = 0; i < node->lvalues.size(); ++i )
    {
        xssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        xssaop* rvalue = b->op( node->sloc, XSSA_SELECT, next );
        rvalue->immed = i;
        b->lvalue_assign( &lvalue, rvalue );
    }
    
    // Loop body.
    b->ifthen( next );
    visit( node->body );
    b->loopcontinue();
    b->ifelse();
    b->loopbreak();
    b->ifend();
    b->loopend();
    
    // Close scopes.
    b->close_scope( node->scope );
    
    // Pop iterator.
    b->op( node->sloc, XSSA_POPITER );
}

void xssa_build_stmt::visit( xec_stmt_for* node )
{
    if ( node->init )
    {
        visit( node->init );
    }
    b->loopopen( false );
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
        b->ifend();
    }
    b->loopend();
    b->close_scope( node->scope );
}

void xssa_build_stmt::visit( xec_stmt_using* node )
{
    assert( ! "not implemented" );
}

void xssa_build_stmt::visit( xec_stmt_try* node )
{
    assert( ! "not implemented" );
}

void xssa_build_stmt::visit( xec_stmt_catch* node )
{
    assert( ! "not implemented" );
}

void xssa_build_stmt::visit( xec_stmt_delete* node )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        xec_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == XEC_EXPR_KEY )
        {
            xec_expr_key* expr = (xec_expr_key*)node;
            xssaop* object = b->expr( expr->object );
            xssaop* delkey = b->op( delval->sloc, XSSA_DELKEY, object );
            delkey->key = b->key( expr->key );
            
        }
        else if ( delval->kind == XEC_EXPR_INKEY )
        {
            xec_expr_inkey* expr = (xec_expr_inkey*)node;
            xssaop* object = b->expr( expr->object );
            xssaop* key = b->expr( expr->key );
            b->op( delval->sloc, XSSA_DELINKEY, object, key );
        }
    }
}

void xssa_build_stmt::visit( xec_stmt_case* node )
{
    b->switchcase();
    xssaop* value = b->expr( node->value );
    b->switchcase( value );
}

void xssa_build_stmt::visit( xec_stmt_continue* node )
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

void xssa_build_stmt::visit( xec_stmt_break* node )
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

void xssa_build_stmt::visit( xec_stmt_return* node )
{
    // Evaluate return values.
    xssa_valist values;
    b->unpack( &values, node->values, -1 );
    
    // Close scopes we're exiting.
    xec_ast_scope* scope = node->scope;
    while ( true )
    {
        b->close_scope( scope );
        if ( scope->node->kind == XEC_AST_FUNC )
            break;
        scope = scope->outer;
    }
    
    // Pop iterators.
    scope = node->scope;
    while ( true )
    {
        if ( scope->node->kind == XEC_STMT_FOREACH )
            b->op( scope->node->sloc, XSSA_POPITER );
        if ( scope->node->kind == XEC_AST_FUNC )
            break;
        scope = scope->outer;
    }
    
    // Emit return operation.
    xssaop* r = b->op( node->sloc, XSSA_RETURN, values.values.size() );
    for ( size_t i = 0; i < values.values.size(); ++i )
        r->operand[ i ] = values.values.at( i );
    r->multival = values.multival;
    b->funcreturn();
}

void xssa_build_stmt::visit( xec_stmt_throw* node )
{
    assert( ! "not implemented" );
}





