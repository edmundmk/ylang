//
//  xec_compile.cpp
//
//  Created by Edmund Kapusniak on 18/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_compile.h"
#include <assert.h>
#include <math.h>
#include "xec_astvisitor.h"
#include "xec_opcodes.h"
#include "xec_scope.h"


class xec_compiler;




/*
    A bare identifier represents a name in a number of contexts.  Each name
    is 'qualified' by the context it should be looked up in.
*/

enum xec_name_qual
{
    XEC_QUAL_GLOBAL,        // name is a global
    XEC_QUAL_LOCAL,         // name is a local
    XEC_QUAL_THIS,          // name -> this.name
    XEC_QUAL_OBJECT,        // name -> [object].name
    XEC_QUAL_UPVAL,         // name is upval
    XEC_QUAL_UPVAL_THIS,    // name -> [upval this].name
    XEC_QUAL_UPVAL_OBJECT,  // name -> [upval object].name
};





class xec_alloc
{
public:

    xec_alloc();
    xec_alloc( xec_compiler* c, int r );

    operator int ();
    int count();

    void allocate();
    void release();

};


class xec_lvals
{
public:

    bool        single();
    xec_alloc   target();

    int         count();
    xec_alloc   values();
    void        assign( int i, xec_alloc alloc, int n );
    void        release();

};


class xec_label
{
};




class xec_compiler_visitexpr
    :   public xec_astvisitor< xec_compiler_visitexpr, void, xec_alloc >
{
public:

    explicit xec_compiler_visitexpr( xec_compiler* c );

    using xec_astvisitor< xec_compiler_visitexpr, void, xec_alloc >::visit;

    void visit( xec_expression_null* expr, xec_alloc target );
    void visit( xec_expression_bool* expr, xec_alloc target );
    void visit( xec_expression_number* expr, xec_alloc target );
    void visit( xec_expression_string* expr, xec_alloc target );
    void visit( xec_expression_identifier* expr, xec_alloc target );
    void visit( xec_expression_lookup* expr, xec_alloc target );
    void visit( xec_expression_indexkey* expr, xec_alloc target );
    void visit( xec_expression_index* expr, xec_alloc target );
    void visit( xec_expression_yield* expr, xec_alloc target );
    void visit( xec_expression_call* expr, xec_alloc target );
    void visit( xec_expression_preop* expr, xec_alloc target );
    void visit( xec_expression_postop* expr, xec_alloc target );
    void visit( xec_expression_unary* expr, xec_alloc target );
    void visit( xec_expression_binary* expr, xec_alloc target );
    void visit( xec_expression_comparison* expr, xec_alloc target );
    void visit( xec_expression_logical* expr, xec_alloc target );
    void visit( xec_expression_conditional* expr, xec_alloc target );
    void visit( xec_expression_varargs* expr, xec_alloc target );
    void visit( xec_expression_unpack* expr, xec_alloc target );
    void visit( xec_expression_list* expr, xec_alloc target );
    void visit( xec_expression_assign* expr, xec_alloc target );
    void visit( xec_expression_mono* expr, xec_alloc target );
    void visit( xec_expression_declare* expr, xec_alloc target );
    void visit( xec_constructor_new* expr, xec_alloc target );
    void visit( xec_constructor_list* expr, xec_alloc target );
    void visit( xec_constructor_table* expr, xec_alloc target );
    void visit( xec_constructor_object* expr, xec_alloc target );
    void visit( xec_constructor_function* expr, xec_alloc target );

private:

    xec_compiler* c;

};








class xec_compiler
{
public:

    int             literal( double n );
    int             literal( const char* string, size_t length );
    int             key( const char* s );

    void            declare( xec_name* name, xec_alloc alloc, int n );
    void            declare( xec_scope* scope, xec_alloc alloc );
    void            retire( xec_name* name );
    void            retire( xec_scope* scope );

    xec_name_qual   name_qual( xec_name* name );
    xec_alloc       local_this( xec_name* name );
    xec_alloc       local( xec_name* name );
    xec_alloc       local( xec_scope* object );
    int             upval_this( xec_name* name );
    int             upval( xec_name* name );
    int             upval( xec_scope* object );

    xec_lvals       lvalues( xec_expression* expr );

    xec_alloc       push();
    xec_alloc       push( xec_expression* expr );
    xec_alloc       push_thiscall( xec_expression* expr );
    xec_alloc       push( xec_expression* expr, int count );
    xec_alloc       operand( xec_expression* expr );
    std::pair< xec_alloc, bool > operinv( xec_expression* expr );
    void            assign( xec_expression* expr, xec_alloc target );
    void            evaluate( xec_expression* expr );
    void            iffalse( xec_expression* expr, xec_label* label );
    void            iftrue( xec_expression* expr, xec_label* label );
    
    void            move( int a, int b );
    void            emit( xec_opcode o, int a );
    void            emit( xec_opcode o, int a, int b );
    void            emit( xec_opcode o, int a, int b, int c );
    void            compare( xec_operator_kind o, int r, int a, int b );

    void            label( xec_label* label );
    void            jump( xec_opcode o, xec_label* label );
    void            jump( xec_opcode o, int a, xec_label* label );

};



xec_compiler_visitexpr::xec_compiler_visitexpr( xec_compiler* c )
    :   c( c )
{
}

void xec_compiler_visitexpr::visit( xec_expression_null* expr, xec_alloc target )
{
    c->emit( XEC_LITNULL, target );
}

void xec_compiler_visitexpr::visit( xec_expression_bool* expr, xec_alloc target )
{
    if ( expr->get_value() )
        c->emit( XEC_LITTRUE, target );
    else
        c->emit( XEC_LITFALSE, target );
}

void xec_compiler_visitexpr::visit( xec_expression_number* expr, xec_alloc target )
{
    double n = expr->get_value();
    if ( n >= XEC_LITINT_MIN && n <= XEC_LITINT_MAX && rint( n ) == n )
        c->emit( XEC_LITINT, target, (int)n );
    else
        c->emit( XEC_LITERAL, target, c->literal( n ) );
}

void xec_compiler_visitexpr::visit( xec_expression_string* expr, xec_alloc target )
{
    int literal = c->literal( expr->get_string(), expr->get_length() );
    c->emit( XEC_LITERAL, target, literal );
}

void xec_compiler_visitexpr::visit( xec_expression_identifier* expr, xec_alloc target )
{
    xec_name* name = expr->get_name();
    switch ( c->name_qual( name ) )
    {
    case XEC_QUAL_GLOBAL:
        c->emit( XEC_GETGLOBAL, target, c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_LOCAL:
        c->move( target, c->local( name ) );
        break;
    
    case XEC_QUAL_THIS:
        c->emit( XEC_GETKEY, target,
                c->local_this( name ), c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_OBJECT:
        c->emit( XEC_GETKEY, target,
                c->local( name->get_scope() ), c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_UPVAL:
        c->emit( XEC_GETUPVAL, target, c->upval( name ) );
        break;
    
    case XEC_QUAL_UPVAL_THIS:
        c->emit( XEC_GETUPVAL, target, c->upval_this( name ) );
        c->emit( XEC_GETKEY, target, target, c->key( name->get_name() ) );
        break;
        
    case XEC_QUAL_UPVAL_OBJECT:
        c->emit( XEC_GETUPVAL, target, c->upval( name->get_scope() ) );
        c->emit( XEC_GETKEY, target, target, c->key( name->get_name() ) );
        break;
    }
}

void xec_compiler_visitexpr::visit( xec_expression_lookup* expr, xec_alloc target )
{
    xec_alloc object = c->operand( expr->get_expr() );
    c->emit( XEC_GETKEY, target, object, c->key( expr->get_identifier() ) );
    object.release();
}

void xec_compiler_visitexpr::visit( xec_expression_indexkey* expr, xec_alloc target )
{
    xec_alloc object = c->operand( expr->get_expr() );
    xec_alloc key = c->operand( expr->get_index() );
    c->emit( XEC_GETINDEXKEY, target, object, key );
    key.release();
    object.release();
}

void xec_compiler_visitexpr::visit( xec_expression_index* expr, xec_alloc target )
{
    xec_alloc object = c->operand( expr->get_expr() );
    xec_alloc index = c->operand( expr->get_index() );
    c->emit( XEC_GETINDEX, target, object, index );
    index.release();
    object.release();
}

void xec_compiler_visitexpr::visit( xec_expression_yield* expr, xec_alloc target )
{
    xec_alloc args = c->push( expr->get_arguments(), -1 );
    int argcount = args.count();
    c->emit( XEC_YIELD, args, argcount, 1 );
    c->move( target, args );
    args.release();
}

void xec_compiler_visitexpr::visit( xec_expression_call* expr, xec_alloc target )
{
    xec_alloc thiscall = c->push_thiscall( expr->get_expr() );
    xec_alloc args = c->push( expr->get_arguments(), -1 );
    assert( thiscall + thiscall.count() == args );
    int argcount = args.count() != -1 ? thiscall.count() + args.count() : -1;
    if ( expr->get_yieldcall() )
        c->emit( XEC_YIELDCALL, thiscall, argcount, 1 );
    else
        c->emit( XEC_CALL, thiscall, argcount, 1 );
    c->move( target, thiscall );
    args.release();
    thiscall.release();
}

void xec_compiler_visitexpr::visit( xec_expression_preop* expr, xec_alloc target )
{
    xec_lvals lvalues = c->lvalues( expr->get_expr() );
    
    if ( lvalues.single() )
    {
        xec_alloc ltarget = lvalues.target();
        if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
            c->emit( XEC_INC, ltarget, ltarget );
        else
            c->emit( XEC_DEC, ltarget, ltarget );
        c->move( target, ltarget );
        ltarget.release();
    }
    else
    {
        xec_alloc values = lvalues.values();
        for ( int i = 0; i < lvalues.count(); ++i )
        {
            if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
                c->emit( XEC_INC, values + i, values + i );
            else
                c->emit( XEC_DEC, values + i, values + i );
        }
        c->move( target, values );
        values.release();
    }
    
    lvalues.release();
}

void xec_compiler_visitexpr::visit( xec_expression_postop* expr, xec_alloc target )
{
    xec_lvals lvalues = c->lvalues( expr->get_expr() );
    
    if ( lvalues.single() )
    {
        xec_alloc ltarget = lvalues.target();
        c->move( target, ltarget );
        if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
            c->emit( XEC_INC, ltarget, ltarget );
        else
            c->emit( XEC_DEC, ltarget, ltarget );
        ltarget.release();
    }
    else
    {
        xec_alloc result = c->push();
        xec_alloc values = lvalues.values();
        c->move( result, values );
        for ( int i = 0; i < lvalues.count(); ++i )
        {
            if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
                c->emit( XEC_INC, values + i, values + i );
            else
                c->emit( XEC_DEC, values + i, values + i );
        }
        values.release();
        c->move( target, result );
        result.release();
    }
    
    lvalues.release();
}

void xec_compiler_visitexpr::visit( xec_expression_unary* expr, xec_alloc target )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_POSITIVE:
    {
        c->assign( expr, target );
        c->emit( XEC_ASSERTNUM, target );
    }
        
    case XEC_OPERATOR_NEGATIVE:
    {
        xec_alloc value = c->operand( expr->get_expr() );
        c->emit( XEC_NEG, target, value );
        value.release();
    }
    
    case XEC_OPERATOR_LOGICNOT:
    {
        xec_alloc value = c->operand( expr->get_expr() );
        c->emit( XEC_LNOT, target, value );
        value.release();
    }
    
    case XEC_OPERATOR_BITNOT:
    {
        xec_alloc value = c->operand( expr->get_expr() );
        c->emit( XEC_NOT, target, value );
        value.release();
    }
    
    default:
        assert( ! "invalid unary operator" );
        break;
    }
}

void xec_compiler_visitexpr::visit( xec_expression_binary* expr, xec_alloc target )
{
    xec_alloc lhs = c->operand( expr->get_lhs() );
    xec_alloc rhs = c->operand( expr->get_rhs() );
    
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_MULTIPLY:
        c->emit( XEC_MUL, target, lhs, rhs );
        break;
    
    case XEC_OPERATOR_DIVIDE:
        c->emit( XEC_DIV, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_MODULUS:
        c->emit( XEC_MOD, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_INTDIV:
        c->emit( XEC_IDIV, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_ADD:
        c->emit( XEC_ADD, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_SUBTRACT:
        c->emit( XEC_SUB, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_LSHIFT:
        c->emit( XEC_LSL, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_RSHIFT:
        c->emit( XEC_ASR, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_URSHIFT:
        c->emit( XEC_LSR, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_BITAND:
        c->emit( XEC_AND, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_BITXOR:
        c->emit( XEC_XOR, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_BITOR:
        c->emit( XEC_OR, target, lhs, rhs );
        break;
        
    case XEC_OPERATOR_CONCATENATE:
        c->emit( XEC_CONCAT, target, lhs, rhs );
        break;
    
    default:
        assert( ! "invalid binary operator" );
        break;
    }
    
    rhs.release();
    lhs.release();
}

void xec_compiler_visitexpr::visit( xec_expression_comparison* expr, xec_alloc target )
{
    if ( expr->get_rest_count() == 1 )
    {
        xec_alloc lhs = c->operand( expr->get_first_expr() );
        xec_alloc rhs = c->operand( expr->get_rest_expr( 0 ) );
        c->compare( expr->get_operator( 0 ), target, lhs, rhs );
        rhs.release();
        lhs.release();
    }
    else
    {
        xec_label label;
        
        xec_alloc result = c->push();
        xec_alloc a = c->push( expr->get_rest_expr( 0 ) );
        xec_alloc b = c->push();
        
        xec_alloc* p = &a;
        xec_alloc* q = &b;
        
        size_t last = expr->get_rest_count() - 1;
        for ( size_t i = 0; i < expr->get_rest_count(); ++i )
        {
            c->assign( expr->get_rest_expr( i ), *q );
            c->compare( expr->get_operator( i ), result, *p, *q );
            if ( i != last )
            {
                c->jump( XEC_IFFALSE, result, &label );
            }
            std::swap( p, q );
        }
        
        c->label( &label );
        
        b.release();
        a.release();
        
        c->move( target, result );
        result.release();
    }
}

void xec_compiler_visitexpr::visit( xec_expression_logical* expr, xec_alloc target )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_LOGICAND:
    {
        // If lhs is false, return it.  Otherwise return the rhs.
        xec_label label;
        xec_alloc result = c->push();
        c->assign( expr->get_lhs(), result );
        c->jump( XEC_IFFALSE, result, &label );
        c->assign( expr->get_rhs(), result );
        c->label( &label );
        c->move( target, result );
        result.release();
    }
    
    case XEC_OPERATOR_LOGICXOR:
    {
        std::pair< xec_alloc, bool > lhs = c->operinv( expr->get_lhs() );
        std::pair< xec_alloc, bool > rhs = c->operinv( expr->get_rhs() );
        c->emit( XEC_LXOR, target, lhs.first, rhs.first );
        if ( lhs.second != rhs.second )
            c->emit( XEC_NOT, target, target );
        rhs.first.release();
        lhs.first.release();
    }
    
    case XEC_OPERATOR_LOGICOR:
    {
        // If lhs is true, return it.  Otherwise return the rhs.
        xec_label label;
        xec_alloc result = c->push();
        c->assign( expr->get_lhs(), result );
        c->jump( XEC_IFTRUE, result, &label );
        c->assign( expr->get_rhs(), result );
        c->label( &label );
        c->move( target, result );
        result.release();
    }
    
    default:
        assert( ! "invalid logical operator" );
        break;
    }
}

void xec_compiler_visitexpr::visit( xec_expression_conditional* expr, xec_alloc target )
{
    xec_label label;
    xec_alloc result = c->push();
    c->iffalse( expr->get_condition(), &label );
    c->assign( expr->get_iftrue(), result );
    c->label( &label );
    c->assign( expr->get_iffalse(), result );
    c->move( target, result );
    result.release();
}

void xec_compiler_visitexpr::visit( xec_expression_varargs* expr, xec_alloc target )
{
    c->emit( XEC_VARARGS, target, 0, 1 );
}

void xec_compiler_visitexpr::visit( xec_expression_unpack* expr, xec_alloc target )
{
    c->assign( expr->get_expr(), target );
    c->emit( XEC_UNPACK, target, 1, 1 );
}

void xec_compiler_visitexpr::visit( xec_expression_list* expr, xec_alloc target )
{
    if ( expr->get_count() == 1 && ! expr->get_final() )
    {
        c->assign( expr->get_expr( 0 ), target );
    }
    else if ( expr->get_count() == 0 && expr->get_final() )
    {
        c->assign( expr->get_final(), target );
    }
    else
    {
        // Remember the result in case assigning it clobbers something the
        // later expressions in the lest rely on.
        xec_alloc result = c->push();
        c->assign( expr->get_expr( 0 ), result );
        
        // Evaluate remaining expressions.
        for ( size_t i = 1; i < expr->get_count(); ++i )
            c->evaluate( expr->get_expr( i ) );
        if ( expr->get_final() )
            c->evaluate( expr->get_final() );
        
        // Put it in the correct place.
        c->move( target, result );
        result.release();
    }
}

void xec_compiler_visitexpr::visit( xec_expression_assign* expr, xec_alloc target )
{
    xec_lvals lvalues = c->lvalues( expr->get_lhs() );

    if ( lvalues.single() )
    {
        xec_alloc ltarget = lvalues.target();
        c->assign( expr->get_rhs(), ltarget );
        c->move( target, ltarget );
        ltarget.release();
    }
    else
    {
        xec_alloc values = c->push( expr->get_rhs(), lvalues.count() );
        for ( int i = 0; i < lvalues.count(); ++i )
        {
            lvalues.assign( i, values, i );
        }
        c->move( target, values );
        values.release();
    }
    
    lvalues.release();
}

void xec_compiler_visitexpr::visit( xec_expression_mono* expr, xec_alloc target )
{
    c->assign( expr, target );
}

void xec_compiler_visitexpr::visit( xec_expression_declare* expr, xec_alloc target )
{
    xec_expression_list* names = expr->get_name_list();
    xec_alloc values = c->push( expr->get_expr_list(), (int)names->get_count() );
    for ( size_t i = 0; i < names->get_count(); ++i )
    {
        xec_expression* name = names->get_expr( i );
        if ( name->get_kind() == XEC_EXPRESSION_IDENTIFIER )
        {
            xec_expression_identifier* ident =
                    (xec_expression_identifier*)names->get_expr( i );
            c->declare( ident->get_name(), values, (int)i );
        }
        else
        {
            assert( ! "invalid name for declaration" );
        }
    }
    values.release();
}

void xec_compiler_visitexpr::visit( xec_constructor_new* expr, xec_alloc target )
{
    xec_alloc proto = c->push( expr->get_proto() );
    xec_alloc args = c->push( expr->get_arguments(), -1 );
    assert( proto + proto.count() == args );
    int argcount = args.count() != -1 ? proto.count() + args.count() : -1;
    c->emit( XEC_NEW, proto, argcount, 1 );
    args.release();
    proto.release();
}

void xec_compiler_visitexpr::visit( xec_constructor_list* expr, xec_alloc target )
{
    xec_alloc result = c->push();
    c->emit( XEC_LIST, result, (int)expr->get_count() );
    
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        xec_alloc element = c->operand( expr->get_expr( i ) );
        if ( (int)i <= XEC_ELEMENT_MAX )
        {
            c->emit( XEC_ELEMENT, result, (int)i, element );
        }
        else if ( (int)i <= XEC_LITINT_MAX )
        {
            xec_alloc index = c->push();
            c->emit( XEC_LITINT, index, (int)i );
            c->emit( XEC_SETINDEX, result, index, element );
            index.release();
        }
        else
        {
            xec_alloc index = c->push();
            c->emit( XEC_LITERAL, index, c->literal( (double)i ) );
            c->emit( XEC_SETINDEX, result, index, element );
            index.release();
        }
        element.release();
    }
    
    if ( expr->get_final() )
    {
        xec_alloc append = c->push( expr->get_final(), -1 );
        c->emit( XEC_APPEND, result, append.count(), 0 );
        append.release();
    }
    
    c->move( target, result );
    result.release();
}

void xec_compiler_visitexpr::visit( xec_constructor_table* expr, xec_alloc target )
{
    xec_alloc result = c->push();
    c->emit( XEC_TABLE, result, (int)expr->get_count() );
    
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        xec_alloc key = c->operand( expr->get_key( i ) );
        xec_alloc value = c->operand( expr->get_value( i ) );
        c->emit( XEC_SETINDEX, result, key, value );
        value.release();
        key.release();
    }
    
    c->move( target, result );
    result.release();
}

void xec_compiler_visitexpr::visit( xec_constructor_object* expr, xec_alloc target )
{
    xec_alloc proto = c->operand( expr->get_proto() );
    proto.release();
    xec_alloc result = c->push();
    c->emit( XEC_OBJECT, result, proto );
    c->declare( expr->get_scope(), result );
    
    for ( size_t i = 0; i < expr->get_member_count(); ++i )
    {
        // HMMM.
    }
    
    c->move( target, result );
    result.release();
}

void xec_compiler_visitexpr::visit( xec_constructor_function* expr, xec_alloc target )
{
}






