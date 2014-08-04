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
    xec_ssa_build_expr
*/

xec_ssa_build_expr::xec_ssa_build_expr( xec_ssa_builder* b )
    :   b( b )
{
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
    return b->node( b->packed( node->sloc, XEC_SSA_GLOBAL, NULL, node->name ) );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_upref* node )
{
    return b->node( b->packed( node->sloc, XEC_SSA_UPREF, NULL, node->index ) );
                
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
    b->lvalue_assign( &lvalue, updated );
    return updated;
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
    return NULL;
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
    
    // !! TODO: close upvals in scope.
    
    return object;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_array* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_new_table* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_mono* node )
{
    return visit( node->expr );
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_call* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_yield* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_vararg* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_unpack* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_list* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_assign* node )
{
    return NULL;
}

xec_ssa_node* xec_ssa_build_expr::visit( xec_expr_assign_list* node )
{
    return NULL;
}




#if 0

#include "xec_buildssa.h"
#include <deque>



/*
    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/




struct xec_buildssa_block
{
    std::deque< xec_buildssa_block* > predecessors;
    std::unordered_map< xec_ssaname, xec_ssavalue > definitions;
    std::unordered_map< xec_ssaname, xec_ssavalue > proxy_definitions;
    bool sealed;
};





/*

xec_buildssa::xec_buildssa()
    :   next_name( 0 )
    ,   block( NULL )
{
}


xec_ssaname xec_buildssa::declare( const char* text )
{
    xec_ssaname name = next_name;
    next_name += 1;
    names.emplace( name, text );
    active_names.insert( name );
    return name;
}

void xec_buildssa::retire( xec_ssaname name )
{
    active_names.erase( name );
}




void xec_buildssa::write( xec_ssaname name, xec_ssavalue value )
{
}

xec_ssavalue xec_buildssa::read( xec_ssaname name )
{
}





void xec_buildssa::loop_begin()
{
    
}

void xec_buildssa::loop_break()
{
}

void xec_buildssa::loop_continue()
{
}

void xec_buildssa::loop_end()
{
}

void xec_buildssa::if_true( xec_ssavalue condition )
{
}

void xec_buildssa::if_false( xec_ssavalue condition )
{
}

void xec_buildssa::if_else()
{
}

void xec_buildssa::if_end()
{
}




xec_ssavalue xec_buildssa::null()
{
}

xec_ssavalue xec_buildssa::literal( bool boolean )
{
}

xec_ssavalue xec_buildssa::literal( double number )
{
}

xec_ssavalue xec_buildssa::literal( const char* string, size_t length )
{
}

xec_ssavalue xec_buildssa::emit( int opcode, xec_ssavalue a )
{
}

xec_ssavalue xec_buildssa::emit( int opcode, xec_ssavalue a, xec_ssavalue b )
{
}

void xec_buildssa::parameter( xec_ssavalue a )
{
}

xec_ssavalue xec_buildssa::multiple( int opcode, int count )
{
}

xec_ssavalue xec_buildssa::result( xec_ssavalue v, int n )
{
}


*/

#endif






