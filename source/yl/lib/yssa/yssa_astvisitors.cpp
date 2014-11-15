//
//  yssa_astvisitors.cpp
//
//  Created by Edmund Kapusniak on 10/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yssa_astvisitors.h"
#include "yssa_builder.h"
#include <vector>





/*
    Common between visitors.
*/

static yssa_opcode assign_opcode( yl_ast_opkind assignop )
{
    switch ( assignop )
    {
    case YL_ASTOP_MULASSIGN:       return YSSA_MUL;
    case YL_ASTOP_DIVASSIGN:       return YSSA_DIV;
    case YL_ASTOP_MODASSIGN:       return YSSA_MOD;
    case YL_ASTOP_INTDIVASSIGN:    return YSSA_INTDIV;
    case YL_ASTOP_ADDASSIGN:       return YSSA_ADD;
    case YL_ASTOP_SUBASSIGN:       return YSSA_SUB;
    case YL_ASTOP_LSHIFTASSIGN:    return YSSA_LSL;
    case YL_ASTOP_LRSHIFTASSIGN:   return YSSA_LSR;
    case YL_ASTOP_ARSHIFTASSIGN:   return YSSA_ASR;
    case YL_ASTOP_BITANDASSIGN:    return YSSA_AND;
    case YL_ASTOP_BITXORASSIGN:    return YSSA_XOR;
    case YL_ASTOP_BITORASSIGN:     return YSSA_OR;
    default: assert( ! "invalid operator" ); break;
    }

    return YSSA_NOP;
}





/*
    yssa_build_expr
*/

yssa_build_expr::yssa_build_expr( yssa_builder* b )
    :   b( b )
{
}

yssaop* yssa_build_expr::fallback( yl_ast_node* node )
{
    assert( ! "expected expression" );
}

yssaop* yssa_build_expr::visit( yl_ast_func* node )
{
    // Locate (possibly unbuilt) SSA of function.
    yssa_func* func = b->func( node );
    
    // Retrieve upvals.
    yssaop* l = yssaop::op( node->sloc, YSSA_CLOSURE, node->upvals.size() );
    l->func = func;
    for ( size_t i = 0; i < node->upvals.size(); ++i )
    {
        const yl_ast_upval& upval = node->upvals.at( i );
        switch ( upval.kind )
        {
        case YL_UPVAL_LOCAL:
        {
            l->operand[ i ] = b->lookup( node->sloc, upval.local );
            break;
        }
        
        case YL_UPVAL_OBJECT:
        {
            l->operand[ i ] = b->lookup( upval.object );
            break;
        }
        
        case YL_UPVAL_UPVAL:
        {
            l->operand[ i ] = b->op( node->sloc, YSSA_REFUP );
            l->operand[ i ]->immed = upval.upval;
            break;
        }
        }
    }
    
    // Construct closure.
    return b->op( l );
}

yssaop* yssa_build_expr::visit( yl_expr_null* node )
{
    return b->op( node->sloc, YSSA_NULL );
}

yssaop* yssa_build_expr::visit( yl_expr_bool* node )
{
    yssaop* boolean = b->op( node->sloc, YSSA_BOOL );
    boolean->boolean = node->value;
    return boolean;
}

yssaop* yssa_build_expr::visit( yl_expr_number* node )
{
    yssaop* number = b->op( node->sloc, YSSA_NUMBER );
    number->number = node->value;
    return number;
}

yssaop* yssa_build_expr::visit( yl_expr_string* node )
{
    yssaop* s = b->op( node->sloc, YSSA_STRING );
    s->string = yssa_string::s( node->sloc, node->length, node->string );
    return s;
}

yssaop* yssa_build_expr::visit( yl_expr_local* node )
{
    return b->lookup( node->sloc, node->name );
}

yssaop* yssa_build_expr::visit( yl_expr_global* node )
{
    yssaop* global = b->op( node->sloc, YSSA_GLOBAL );
    global->key = b->key( node->name );
    return global;
}

yssaop* yssa_build_expr::visit( yl_expr_upref* node )
{
    yssaop* refup = b->op( node->sloc, YSSA_REFUP );
    refup->immed = node->index;
    return refup;
}

yssaop* yssa_build_expr::visit( yl_expr_objref* node )
{
    return b->lookup( node->object );
}

yssaop* yssa_build_expr::visit( yl_expr_key* node )
{
    yssaop* object = visit( node->object );
    yssaop* key = b->op( node->sloc, YSSA_KEY, object );
    key->key = b->key( node->key );
    return key;
}

yssaop* yssa_build_expr::visit( yl_expr_inkey* node )
{
    yssaop* object = visit( node->object );
    yssaop* key    = visit( node->key );
    return b->op( node->sloc, YSSA_INKEY, object, key );
}

yssaop* yssa_build_expr::visit( yl_expr_index* node )
{
    yssaop* object = visit( node->object );
    yssaop* index  = visit( node->index );
    return b->op( node->sloc, YSSA_INDEX, object, index );
}

yssaop* yssa_build_expr::visit( yl_expr_preop* node )
{
    yssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    yssaop* value   = b->lvalue_value( &lvalue );
    yssaop* literal = b->op( node->sloc, YSSA_NUMBER );
    literal->number = 1.0;
    
    yssa_opcode opcode = YSSA_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_PREINC:   opcode = YSSA_ADD;   break;
    case YL_ASTOP_PREDEC:   opcode = YSSA_SUB;   break;
    default:
        assert( ! "invalid operator" );
        break;
    }
    
    yssaop* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return updated;
}

yssaop* yssa_build_expr::visit( yl_expr_postop* node )
{
    yssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );

    yssaop* value   = b->lvalue_value( &lvalue );
    yssaop* literal = b->op( node->sloc, YSSA_NUMBER );
    literal->number = 1.0;
    
    yssa_opcode opcode = YSSA_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_POSTINC:  opcode = YSSA_ADD;   break;
    case YL_ASTOP_POSTDEC:  opcode = YSSA_SUB;   break;
    default: assert( ! "invalid operator" ); break;
    }
    
    yssaop* updated = b->op( node->sloc, opcode, value, literal );
    b->lvalue_assign( &lvalue, updated );
    return value;
}

yssaop* yssa_build_expr::visit( yl_expr_unary* node )
{
    yssaop* operand = visit( node->operand );
    
    yssa_opcode opcode = YSSA_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_POSITIVE: opcode = YSSA_POS;         break;
    case YL_ASTOP_NEGATIVE: opcode = YSSA_NEG;         break;
    case YL_ASTOP_BITNOT:   opcode = YSSA_NOT;         break;
    case YL_ASTOP_LOGICNOT: opcode = YSSA_LNOT;        break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operand );
}

yssaop* yssa_build_expr::visit( yl_expr_binary* node )
{
    yssaop* operanda = visit( node->lhs );
    yssaop* operandb = visit( node->rhs );
    
    yssa_opcode opcode = YSSA_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_MULTIPLY:     opcode = YSSA_MUL;     break;
    case YL_ASTOP_DIVIDE:       opcode = YSSA_DIV;     break;
    case YL_ASTOP_MODULUS:      opcode = YSSA_MOD;     break;
    case YL_ASTOP_INTDIV:       opcode = YSSA_INTDIV;  break;
    case YL_ASTOP_ADD:          opcode = YSSA_ADD;     break;
    case YL_ASTOP_SUBTRACT:     opcode = YSSA_SUB;     break;
    case YL_ASTOP_LSHIFT:       opcode = YSSA_LSL;     break;
    case YL_ASTOP_LRSHIFT:      opcode = YSSA_LSR;     break;
    case YL_ASTOP_ARSHIFT:      opcode = YSSA_ASR;     break;
    case YL_ASTOP_BITAND:       opcode = YSSA_AND;     break;
    case YL_ASTOP_BITXOR:       opcode = YSSA_XOR;     break;
    case YL_ASTOP_BITOR:        opcode = YSSA_OR;      break;
    case YL_ASTOP_CONCATENATE:  opcode = YSSA_CONCAT;  break;
    default: assert( ! "invalid operator" ); break;
    }
    
    return b->op( node->sloc, opcode, operanda, operandb );
}

yssaop* yssa_build_expr::visit( yl_expr_compare* node )
{
    yssaop* lhs = visit( node->first );
    yssaop* result = nullptr;
    
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
        yl_ast_opkind opkind = node->opkinds.at( i );
        yssaop* rhs = visit( node->terms.at( i ) );
        
        /*
            We have a limited set of comparison opcodes:
                l > r   -> r < l
                l >= r  -> r <= l
                l != r  -> not ( l == r )
                l !in r -> not ( l in r )
                l !is r -> not ( l is r )
        */
        
        yssa_opcode opcode = YSSA_NOP;
        bool inv = false, neg = false;
        switch ( opkind )
        {
        case YL_ASTOP_EQUAL:        opcode = YSSA_EQ;               break;
        case YL_ASTOP_NOTEQUAL:     opcode = YSSA_EQ; neg = true;   break;
        case YL_ASTOP_LESS:         opcode = YSSA_LT;               break;
        case YL_ASTOP_GREATER:      opcode = YSSA_LT; inv = true;   break;
        case YL_ASTOP_LESSEQUAL:    opcode = YSSA_LE;               break;
        case YL_ASTOP_GREATEREQUAL: opcode = YSSA_LE; inv = true;   break;
        case YL_ASTOP_IN:           opcode = YSSA_IN;               break;
        case YL_ASTOP_NOTIN:        opcode = YSSA_IN; neg = true;   break;
        case YL_ASTOP_IS:           opcode = YSSA_IS;               break;
        case YL_ASTOP_NOTIS:        opcode = YSSA_IS; neg = true;   break;
        default: assert( ! "invalid operator" ); break;
        }
        
        if ( ! inv )
            result = b->op( node->sloc, opcode, lhs, rhs );
        else
            result = b->op( node->sloc, opcode, rhs, lhs );
        if ( neg )
            result = b->op( node->sloc, YSSA_LNOT, result );
        
        // Keep the result in a temporary, as if it is false we shortcut
        // evaluation of later terms.
        b->define( node, result );
        
        // The rhs becomes the lhs of the next chained comparison.
        lhs = rhs;
    }
    
    for ( size_t i = 0; i < ifcount; ++i )
    {
        b->ifelse();
        b->ifend();
    }
    
    // Lookup the result, as it is defined in multiple blocks.
    return b->lookup( node );
}

yssaop* yssa_build_expr::visit( yl_expr_logical* node )
{
    switch ( node->opkind )
    {
    case YL_ASTOP_LOGICAND:
    {
        // Only evaluate rhs if lhs is true.
        yssaop* lhs = visit( node->lhs );
        b->define( node, lhs );
        b->ifthen( lhs );
        yssaop* rhs = visit( node->rhs );
        b->define( node, rhs );
        b->ifelse();
        b->ifend();
        return b->lookup( node );
    }
    
    case YL_ASTOP_LOGICXOR:
    {
        // Does not perform shortcut evaluation.
        yssaop* lhs = visit( node->lhs );
        yssaop* rhs = visit( node->rhs );
        return b->op( node->sloc, YSSA_XOR, lhs, rhs );
    }
    
    case YL_ASTOP_LOGICOR:
    {
        // Only evaluate rhs if the lhs is false.
        yssaop* lhs = visit( node->lhs );
        b->define( node, lhs );
        b->ifthen( lhs );
        b->ifelse();
        yssaop* rhs = visit( node->rhs );
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

yssaop* yssa_build_expr::visit( yl_expr_qmark* node )
{
    yssaop* condition = visit( node->condition );
    b->ifthen( condition );
    yssaop* iftrue = visit( node->iftrue );
    b->define( node, iftrue );
    b->ifelse();
    yssaop* iffalse = visit( node->iffalse );
    b->define( node, iffalse );
    b->ifend();
    return b->lookup( node );
}

yssaop* yssa_build_expr::visit( yl_new_new* node )
{
    // Get prototype.
    yssaop* proto = visit( node->proto );

    // Create object inheriting from prototype.
    yssaop* object = b->op( node->sloc, YSSA_OBJECT, proto );
    
    // Lookup 'this' method.
    yssaop* constructor = b->op( node->sloc, YSSA_KEY, object );
    constructor->key = b->key( "this" );

    // Get arguments (unpacked).
    yssa_valist arguments;
    b->unpack( &arguments, node->arguments, -1 );

    // Call constructor.
    yssaop* call = b->op( node->sloc, YSSA_CALL, 2 + arguments.values.size() );
    call->result_count = 0;
    call->operand[ 0 ] = constructor;
    call->operand[ 1 ] = object;
    for ( size_t i = 0; i < arguments.values.size(); ++i )
        call->operand[ 2 + i ] = arguments.values.at( i );
    call->multival = arguments.multival;
    
    // The result is the object.
    return object;
}

yssaop* yssa_build_expr::visit( yl_new_object* node )
{
    // Construct object.
    yssaop* proto = node->proto ? visit( node->proto ) : nullptr;
    yssaop* object = b->op( node->sloc, YSSA_OBJECT, proto );
    
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

yssaop* yssa_build_expr::visit( yl_new_array* node )
{
    // Construct array.
    yssaop* array = b->op( node->sloc, YSSA_ARRAY );
    array->immed = (int)node->values.size();
    
    // Append all values.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        yl_ast_node* elem = node->values.at( i );
        yssaop* value = visit( node->values.at( i ) );
        b->op( elem->sloc, YSSA_APPEND, array, value );
    }
    
    if ( node->final )
    {
        // Unpack final values and extend the array with them.
        yssa_valist values;
        b->unpack( &values, node->final, -1 );
        
        for ( size_t i = 0; i < values.values.size(); ++i )
        {
            yssaop* value = values.values.at( i );
            b->op( node->final->sloc, YSSA_APPEND, array, value );
        }
        
        if ( values.multival )
        {
            yssaop* extend = b->op( node->final->sloc, YSSA_EXTEND, array );
            extend->multival = values.multival;
        }
    }

    return array;
}

yssaop* yssa_build_expr::visit( yl_new_table* node )
{
    // Construct table.
    yssaop* table = b->op( node->sloc, YSSA_TABLE );
    table->immed = (int)node->elements.size();
    
    // Add elements.
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const yl_key_value& e = node->elements.at( i );
        yssaop* key   = visit( e.key );
        yssaop* value = visit( e.value );
        b->op( node->sloc, YSSA_SETINDEX, table, key, value );
    }
    
    return table;
}

yssaop* yssa_build_expr::visit( yl_expr_mono* node )
{
    return visit( node->expr );
}

yssaop* yssa_build_expr::visit( yl_expr_call* node )
{
    yssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

yssaop* yssa_build_expr::visit( yl_expr_yield* node )
{
    yssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

yssaop* yssa_build_expr::visit( yl_expr_vararg* node )
{
    // Fetch the first variable argument.
    yssaop* vararg = b->op( node->sloc, YSSA_VARARG );
    vararg->immed = 0;
    return vararg;
}

yssaop* yssa_build_expr::visit( yl_expr_unpack* node )
{
    yssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

yssaop* yssa_build_expr::visit( yl_expr_list* node )
{
    yssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}

yssaop* yssa_build_expr::visit( yl_expr_assign* node )
{
    yssa_lvalue lvalue;
    b->lvalue( &lvalue, node->lvalue );
    
    if ( node->assignop == YL_ASTOP_DECLARE
            || node->assignop == YL_ASTOP_ASSIGN )
    {
        yssaop* rvalue = visit( node->rvalue );
        b->lvalue_assign( &lvalue, rvalue );
        return rvalue;
    }
    
    yssa_opcode opcode = assign_opcode( node->assignop );
    yssaop* lhs = b->lvalue_value( &lvalue );
    yssaop* rhs = visit( node->rvalue );
    yssaop* rvalue = b->op( node->sloc, opcode, lhs, rhs );
    b->lvalue_assign( &lvalue, rvalue );
    return rvalue;
}

yssaop* yssa_build_expr::visit( yl_expr_assign_list* node )
{
    yssa_valist values;
    b->unpack( &values, node, 1 );
    return values.values.front();
}





yssa_build_unpack::yssa_build_unpack( yssa_builder* b )
    :   b( b )
{
}

void yssa_build_unpack::fallback(
                yl_ast_node* node, yssa_valist* values, int valcount )
{
    // Expression produces only one value.
    values->values.push_back( b->expr( node ) );
    
    // Unpack NULL for remaining values.
    if ( valcount > 1 )
    {
        yssaop* nullval = b->op( node->sloc, YSSA_NULL );
        for ( int i = 1; i < valcount; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}

void yssa_build_unpack::visit(
                yl_expr_null* node, yssa_valist* values, int valcount )
{
    // Special case to unpack NULL.
    yssaop* nullval = b->expr( node );
    values->values.push_back( nullval );
    for ( int i = 1; i < valcount; ++i )
    {
        values->values.push_back( nullval );
    }
}

void yssa_build_unpack::visit(
                yl_expr_call* node, yssa_valist* values, int valcount )
{
    // Only actually return multiple values if this was an unpack call.
    if ( ! node->unpack && valcount != 1 )
    {
        fallback( node, values, valcount );
        return;
    }

    // Get function and this value.
    yssaop* function = nullptr;
    yssaop* thisval  = nullptr;
    switch ( node->function->kind )
    {
    case YL_EXPR_KEY:
    {
        yl_expr_key* key = (yl_expr_key*)node->function;
        thisval = b->expr( key->object );
        function = b->op( key->sloc, YSSA_KEY, thisval );
        function->key = b->key( key->key );
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        yl_expr_inkey* inkey = (yl_expr_inkey*)node->function;
        thisval = b->expr( inkey->object );
        yssaop* key = b->expr( inkey->key );
        function = b->op( inkey->sloc, YSSA_INKEY, thisval, key );
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
    yssa_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }

    // Construct call.
    yssa_opcode opcode = node->yieldcall ? YSSA_YCALL : YSSA_CALL;
    size_t operand_count = 1 + ( thisval ? 1 : 0 ) + arguments.values.size();
    yssaop* call = b->op( node->sloc, opcode, operand_count );
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
            yssaop* sel = b->op( node->sloc, YSSA_SELECT, call );
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

void yssa_build_unpack::visit(
                yl_expr_yield* node, yssa_valist* values, int valcount )
{
    // Only actually return multiple values if this is an unpack yield.
    if ( ! node->unpack && valcount != 1 )
    {
        fallback( node, values, valcount );
        return;
    }

    // Get arguments (unpacked).
    yssa_valist arguments;
    if ( node->arguments )
    {
        b->unpack( &arguments, node->arguments, -1 );
    }
    
    // Construct yield.
    yssaop* yield = b->op( node->sloc, YSSA_YIELD, arguments.values.size() );
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
            yssaop* sel = b->op( node->sloc, YSSA_SELECT, yield );
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

void yssa_build_unpack::visit(
                yl_expr_vararg* node, yssa_valist* values, int valcount )
{
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            yssaop* vararg = b->op( node->sloc, YSSA_VARARG );
            vararg->immed = i;
            values->values.push_back( vararg );
        }
    }
    else
    {
        yssaop* varall = b->op( node->sloc, YSSA_VARALL );
        varall->result_count = -1;
        values->multival = varall;
    }
}

void yssa_build_unpack::visit(
                yl_expr_unpack* node, yssa_valist* values, int valcount )
{
    yssaop* array = b->expr( node->array );
    if ( valcount != -1 )
    {
        for ( int i = 0; i < valcount; ++i )
        {
            yssaop* elem = b->op( node->sloc, YSSA_ELEM, array );
            elem->immed = i;
            values->values.push_back( elem );
        }
    }
    else
    {
        yssaop* unpack = b->op( node->sloc, YSSA_UNPACK, array );
        unpack->result_count = -1;
        values->multival = unpack;
    }
}

void yssa_build_unpack::visit(
                yl_expr_list* node, yssa_valist* values, int valcount )
{
    // Only assign as many values as were requested.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        yssaop* value = b->expr( node->values.at( i ) );
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
        yssa_valist final;
        visit( node->final, &final, required );
        values->values.insert( values->values.end(),
                    final.values.begin(), final.values.end() );
        values->multival = final.multival;
    }
    else if ( required > 0 )
    {
        // Fill in the rest with null.
        yssaop* nullval = b->op( node->sloc, YSSA_NULL );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}

void yssa_build_unpack::visit(
            yl_expr_assign_list* node, yssa_valist* values, int valcount )
{
    // Evaluate lvalues ready for assignment.
    std::deque< yssa_lvalue > lvalues;
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        yssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        lvalues.push_back( lvalue );
    }

    // Evaluate exactly the number of rvalues we require.
    yssa_valist rvalues;
    b->unpack( &rvalues, node->rvalues, (int)lvalues.size() );
    
    // An assignment list only has a maximum number of values.
    for ( int i = 0; i < lvalues.size(); ++i )
    {
        yssa_lvalue lvalue = lvalues.at( i );
        
        yssaop* v = nullptr;
        if ( node->assignop == YL_ASTOP_DECLARE
                || node->assignop == YL_ASTOP_ASSIGN )
        {
            v = rvalues.values.at( i );
            b->lvalue_assign( &lvalue, v );
        }
        else
        {
            yssa_opcode opcode = assign_opcode( node->assignop );
            yssaop* lhs = b->lvalue_value( &lvalue );
            yssaop* rhs = rvalues.values.at( i );
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
        yssaop* nullval = b->op( node->sloc, YSSA_NULL );
        for ( int i = 0; i < required; ++i )
        {
            values->values.push_back( nullval );
        }
    }
}



yssa_build_stmt::yssa_build_stmt( yssa_builder* b )
    :   b( b )
{
}

void yssa_build_stmt::fallback( yl_ast_node* node )
{
    // Could be an expression statement.
    b->expr( node );
}

void yssa_build_stmt::visit( yl_stmt_block* node )
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

void yssa_build_stmt::visit( yl_stmt_if* node )
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

void yssa_build_stmt::visit( yl_stmt_switch* node )
{
    b->switchopen( b->expr( node->value ) );
    visit( node->body );
    b->switchend();
    b->close_scope( node->scope );
}

void yssa_build_stmt::visit( yl_stmt_while* node )
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

void yssa_build_stmt::visit( yl_stmt_do* node )
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

void yssa_build_stmt::visit( yl_stmt_foreach* node )
{
    // Construct iterator.
    yssa_opcode opcode = node->eachkey ? YSSA_ITERKEY : YSSA_ITER;
    yssaop* list = b->expr( node->list );
    b->op( node->sloc, opcode, list );

    // Begin loop.
    b->loopopen( false );
    
    // Get values for this iteration.
    int request = (int)node->lvalues.size();
    yssaop* next = b->op( node->sloc, YSSA_NEXT );
    next->result_count = request;
    for ( int i = 0; i < node->lvalues.size(); ++i )
    {
        yssa_lvalue lvalue;
        b->lvalue( &lvalue, node->lvalues.at( i ) );
        yssaop* rvalue = b->op( node->sloc, YSSA_SELECT, next );
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
    b->op( node->sloc, YSSA_POPITER );
}

void yssa_build_stmt::visit( yl_stmt_for* node )
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

void yssa_build_stmt::visit( yl_stmt_using* node )
{
    assert( ! "not implemented" );
}

void yssa_build_stmt::visit( yl_stmt_try* node )
{
    assert( ! "not implemented" );
}

void yssa_build_stmt::visit( yl_stmt_catch* node )
{
    assert( ! "not implemented" );
}

void yssa_build_stmt::visit( yl_stmt_delete* node )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        yl_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == YL_EXPR_KEY )
        {
            yl_expr_key* expr = (yl_expr_key*)node;
            yssaop* object = b->expr( expr->object );
            yssaop* delkey = b->op( delval->sloc, YSSA_DELKEY, object );
            delkey->key = b->key( expr->key );
            
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            yssaop* object = b->expr( expr->object );
            yssaop* key = b->expr( expr->key );
            b->op( delval->sloc, YSSA_DELINKEY, object, key );
        }
    }
}

void yssa_build_stmt::visit( yl_stmt_case* node )
{
    b->switchcase();
    yssaop* value = b->expr( node->value );
    b->switchcase( value );
}

void yssa_build_stmt::visit( yl_stmt_continue* node )
{
    // If we're in a for loop, update the for before continuing.
    if ( node->target->node->kind == YL_STMT_FOR )
    {
        yl_stmt_for* forstmt = (yl_stmt_for*)node->target;
        if ( forstmt->update )
        {
            visit( forstmt->update );
        }
    }
    
    // Close scopes we're exiting.
    for ( yl_ast_scope* scope = node->scope;
                    scope != node->target; scope = scope->outer )
    {
        b->close_scope( scope );
    }
    
    // Continue with the loop.
    b->loopcontinue();
}

void yssa_build_stmt::visit( yl_stmt_break* node )
{
    // Close scopes we're exiting.
    for ( yl_ast_scope* scope = node->scope;
                    scope != node->target; scope = scope->outer )
    {
        b->close_scope( scope );
    }
    
    // Break out of either for loop or switch block.
    if ( node->target->node->kind == YL_STMT_SWITCH )
    {
        b->switchbreak();
    }
    else
    {
        b->loopbreak();
    }
}

void yssa_build_stmt::visit( yl_stmt_return* node )
{
    // Evaluate return values.
    yssa_valist values;
    b->unpack( &values, node->values, -1 );
    
    // Close scopes we're exiting.
    yl_ast_scope* scope = node->scope;
    while ( true )
    {
        b->close_scope( scope );
        if ( scope->node->kind == YL_AST_FUNC )
            break;
        scope = scope->outer;
    }
    
    // Pop iterators.
    scope = node->scope;
    while ( true )
    {
        if ( scope->node->kind == YL_STMT_FOREACH )
            b->op( scope->node->sloc, YSSA_POPITER );
        if ( scope->node->kind == YL_AST_FUNC )
            break;
        scope = scope->outer;
    }
    
    // Emit return operation.
    yssaop* r = b->op( node->sloc, YSSA_RETURN, values.values.size() );
    for ( size_t i = 0; i < values.values.size(); ++i )
        r->operand[ i ] = values.values.at( i );
    r->multival = values.multival;
    b->funcreturn();
}

void yssa_build_stmt::visit( yl_stmt_throw* node )
{
    assert( ! "not implemented" );
}





