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


class xec_operand;
class xec_unpack;
class xec_lvalues;

class xec_compiler;
class xec_compiler_operand;
class xec_compiler_thiscall;




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



/*
    A single register, either a local variable, an operand allocation, or
    a temporary.
    
        -   Local variables are fixed on the stack and are allocated their own
            stack slot until the block is closed.
    
        -   Operand allocations reserve a stack slot until the operand
            structure is disposed (or destroyed).
    
        -   Temporaries pick a stack location but do not necessarily reserve
            the slot.  Therefore allocating new operands may clobber the
            temporary, and assigning to the temporary may clobber any operands
            created after the temporary.  Temporaries may be temporarily
            reserved if the value must persist during operand evaluation.
            
    It might be tempting to reuse the 'target' register as an ad-hoc
    temporary, but this is only valid if there is no possibility that a
    later expression depends on the original value of any local variables.
    
    There is no disadvantage to using temporaries as other allocations will
    freely reuse the slot.
*/

class xec_operand
{
public:

    operator int ();
    
    void reserve();
    void release();
    void dispose();
    
    void invert();
    bool inverted();

};



/*
    A contiguous set of registers, used when dealing with call-like opcodes.
*/

class xec_unpack
{
public:

    // Register range.
    int base();
    int count();
    
};



/*
    A set of 'lvalues' that can be assigned to.
*/

class xec_lvalues
{
public:

    int count();
    xec_operand value( int i );
    void assign( int i, int v );
    void assign( int i, xec_expression* expr );

};



/*
    The target of jump instructions.
*/

class xec_label
{
};




/*
    Evaluates expressions but returns no values.
*/

class xec_compiler_evaluate
{
};



/*
    Evaulates expressions returning a single value.  If target is -1 then
    the resulting operand will be a temporary allocation, otherwise assigns
    the expression into the target register.
*/

class xec_compiler_operand
    :   public xec_astvisitor< xec_compiler_operand, xec_operand, int >
{
public:

    explicit xec_compiler_operand( xec_compiler* c );

    xec_operand visit( xec_expression_null* expr, int target );
    xec_operand visit( xec_expression_bool* expr, int target );
    xec_operand visit( xec_expression_number* expr, int target );
    xec_operand visit( xec_expression_string* expr, int target );
    xec_operand visit( xec_expression_identifier* expr, int target );
    xec_operand visit( xec_expression_lookup* expr, int target );
    xec_operand visit( xec_expression_indexkey* expr, int target );
    xec_operand visit( xec_expression_index* expr, int target );
    xec_operand visit( xec_expression_yield* expr, int target );
    xec_operand visit( xec_expression_call* expr, int target );
    xec_operand visit( xec_expression_preop* expr, int target );
    xec_operand visit( xec_expression_postop* expr, int target );
    xec_operand visit( xec_expression_unary* expr, int target );
    xec_operand visit( xec_expression_binary* expr, int target );
    xec_operand visit( xec_expression_comparison* expr, int target );
    xec_operand visit( xec_expression_logical* expr, int target );
    xec_operand visit( xec_expression_conditional* expr, int target );
    xec_operand visit( xec_expression_varargs* expr, int target );
    xec_operand visit( xec_expression_unpack* expr, int target );
    xec_operand visit( xec_expression_list* expr, int target );
    xec_operand visit( xec_expression_assign* expr, int target );
    xec_operand visit( xec_expression_mono* expr, int target );
    xec_operand visit( xec_expression_declare* expr, int target );
    xec_operand visit( xec_constructor_new* expr, int target );
    xec_operand visit( xec_constructor_list* expr, int target );
    xec_operand visit( xec_constructor_table* expr, int target );
    xec_operand visit( xec_constructor_object* expr, int target );
    xec_operand visit( xec_constructor_function* expr, int target );

private:

    xec_compiler* c;

};



/*
    Appends a function and possibly
*/

class xec_compiler_thiscall
{
};



/*
    Appends n results from an unpack expression to a unpack.
*/

class xec_compiler_unpack
{
};



/*
    Appends lvalues to an lvalue list.
*/

class xec_compiler_lvalue
{
};



/*
    Branches to a label if an expression is true, or to a different one
    if the expression is false.
*/

class xec_compiler_test
{
};



/*
    The compiler object, which builds functions' opcode lists and tracks
    stack slot allocations.  Dispatches to the other compilers.
*/

class xec_compiler
{
public:

    xec_name_qual name_qual( xec_name* name );
    
    int literal( double n );
    int literal( const char* string, size_t length );

    int key( const char* s );

    int local_this( xec_name* name );
    int local( xec_name* name );
    int local( xec_scope* object );
    
    int upval_this( xec_name* name );
    int upval( xec_name* name );
    int upval( xec_scope* object );

    xec_operand temporary( int n );
    xec_operand promote( xec_operand temporary, int target );
    xec_operand result( int target );
    xec_operand result( int target, xec_operand a );
    xec_operand result( int target, xec_operand a, xec_operand b );
    xec_operand asmono( int target, xec_unpack* manyval );

    void evaluate( xec_expression* expr );
    void assign( int target, xec_expression* expr );
    xec_operand operand( xec_expression* expr );
    xec_operand operinv( xec_expression* expr );
    void thiscall( xec_unpack* unpack, xec_expression* expr );
    void unpack( xec_unpack* unpack, xec_expression* expr );
    void lvalues( xec_lvalues* lvalues, xec_expression* expr );
    void test( xec_expression* expr, xec_label* iftrue, xec_label* iffalse );

    void move( int a, int b );
    void emit( xec_opcode o, int a );
    void emit( xec_opcode o, int a, int b );
    void emit( xec_opcode o, int a, int b, int c );
    void comp( xec_operator_kind o, int r, int a, int b );

    void label( xec_label* label );
    void jump( xec_opcode o, xec_label* label );
    void jump( xec_opcode o, int a, xec_label* label );
    

private:

    xec_compiler_operand compiler_operand;

};




xec_compiler_operand::xec_compiler_operand( xec_compiler* c )
    :   c( c )
{
}

xec_operand xec_compiler_operand::visit( xec_expression_null* expr, int target )
{
    xec_operand r = c->result( target );
    c->emit( XEC_LITNULL, r );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_bool* expr, int target )
{
    xec_operand r = c->result( target );
    c->emit( expr->get_value() ? XEC_LITTRUE : XEC_LITFALSE, r );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_number* expr, int target )
{
    xec_operand r = c->result( target );
    double n = expr->get_value();
    if ( n >= XEC_LITINT_MIN && n <= XEC_LITINT_MAX && rint( n ) == n )
        c->emit( XEC_LITINT, r, (int)n );
    else
        c->emit( XEC_LITERAL, r, c->literal( n ) );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_string* expr, int target )
{
    xec_operand r = c->result( target );
    int literal = c->literal( expr->get_string(), expr->get_length() );
    c->emit( XEC_LITERAL, r, literal );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_identifier* expr, int target )
{
    xec_operand r = c->result( target );
    xec_name* name = expr->get_name();
    switch ( c->name_qual( name ) )
    {
    case XEC_QUAL_GLOBAL:
        c->emit( XEC_GETGLOBAL, r, c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_LOCAL:
        c->move( r, c->local( name ) );
        break;
    
    case XEC_QUAL_THIS:
        c->emit( XEC_GETKEY, r,
                c->local_this( name ), c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_OBJECT:
        c->emit( XEC_GETKEY, r,
                c->local( name->get_scope() ), c->key( name->get_name() ) );
        break;
    
    case XEC_QUAL_UPVAL:
        c->emit( XEC_GETUPVAL, r, c->upval( name ) );
        break;
    
    case XEC_QUAL_UPVAL_THIS:
        c->emit( XEC_GETUPVAL, r, c->upval_this( name ) );
        c->emit( XEC_GETKEY, r, r, c->key( name->get_name() ) );
        break;
        
    case XEC_QUAL_UPVAL_OBJECT:
        c->emit( XEC_GETUPVAL, r, c->upval( name->get_scope() ) );
        c->emit( XEC_GETKEY, r, r, c->key( name->get_name() ) );
        break;
    }
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_lookup* expr, int target )
{
    xec_operand object = c->operand( expr->get_expr() );
    xec_operand r = c->result( target, object );
    c->emit( XEC_GETKEY, r, object, c->key( expr->get_identifier() ) );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_indexkey* expr, int target )
{
    xec_operand object = c->operand( expr->get_expr() );
    xec_operand index = c->operand( expr->get_index() );
    xec_operand r = c->result( target, object, index );
    c->emit( XEC_GETINDEXKEY, r, object, index );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_index* expr, int target )
{
    xec_operand object = c->operand( expr->get_expr() );
    xec_operand index = c->operand( expr->get_index() );
    xec_operand r = c->result( target, object, index );
    c->emit( XEC_GETINDEX, r, object, index );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_yield* expr, int target )
{
    xec_unpack unpack;
    c->unpack( &unpack, expr->get_arguments() );
    c->emit( XEC_YIELD, unpack.base(), unpack.count(), 1 );
    return c->asmono( target, &unpack );
}

xec_operand xec_compiler_operand::visit( xec_expression_call* expr, int target )
{
    xec_unpack unpack;
    c->thiscall( &unpack, expr->get_expr() );
    c->unpack( &unpack, expr->get_arguments() );
    if ( expr->get_yieldcall() )
        c->emit( XEC_YIELDCALL, unpack.base(), unpack.count(), 1 );
    else
        c->emit( XEC_CALL, unpack.base(), unpack.count(), 1 );
    return c->asmono( target, &unpack );
}

xec_operand xec_compiler_operand::visit( xec_expression_preop* expr, int target )
{
    xec_operand r = c->result( target );
    
    xec_lvalues lvalues;
    c->lvalues( &lvalues, expr->get_expr() );
    
    for ( int i = 0; i < lvalues.count(); ++i )
    {
        xec_operand v = lvalues.value( i );
        
        if ( i == 0 )
        {
            if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
                c->emit( XEC_INC, r, v );
            else
                c->emit( XEC_DEC, r, v );
            lvalues.assign( i, r );
        }
        else
        {
            if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
                c->emit( XEC_INC, v, v );
            else
                c->emit( XEC_DEC, v, v );
            lvalues.assign( i, v );
        }
    }
    
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_postop* expr, int target )
{
    xec_operand r = c->result( target );
    
    xec_lvalues lvalues;
    c->lvalues( &lvalues, expr->get_expr() );
    
    for ( int i = 0; i < lvalues.count(); ++i )
    {
        xec_operand v = lvalues.value( i );
        
        if ( i == 0 )
            c->move( r, v );
        
        if ( expr->get_operator() == XEC_OPERATOR_INCREMENT )
            c->emit( XEC_INC, v, v );
        else
            c->emit( XEC_DEC, v, v );
        lvalues.assign( i, v );
    }
    
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_unary* expr, int target )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_POSITIVE:
    {
        // Doesn't do anything but is only allowed on numbers.
        xec_operand v = c->operand( expr->get_expr() );
        c->emit( XEC_ASSERTNUM, v );
        return v;
    }
        
    case XEC_OPERATOR_NEGATIVE:
    {
        xec_operand v = c->operand( expr->get_expr() );
        xec_operand r = c->result( target, v );
        c->emit( XEC_NEG, r, v );
        return r;
    }
    
    case XEC_OPERATOR_LOGICNOT:
    {
        // Lazily evaluate not expressions.
        xec_operand v = c->operand( expr->get_expr() );
        v.invert();
        return v;
    }
    
    case XEC_OPERATOR_BITNOT:
    {
        xec_operand v = c->operand( expr->get_expr() );
        xec_operand r = c->result( target, v );
        c->emit( XEC_NOT, r, v );
        return r;
    }
    
    default:
        assert( ! "invalid unary operator" );
        return c->operand( expr->get_expr() );
    }
}

xec_operand xec_compiler_operand::visit( xec_expression_binary* expr, int target )
{
    xec_operand a = c->operand( expr->get_lhs() );
    xec_operand b = c->operand( expr->get_rhs() );
    xec_operand r = c->result( target, a, b );

    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_MULTIPLY:
        c->emit( XEC_MUL, r, a, b );
        return r;
    
    case XEC_OPERATOR_DIVIDE:
        c->emit( XEC_DIV, r, a, b );
        return r;
        
    case XEC_OPERATOR_MODULUS:
        c->emit( XEC_MOD, r, a, b );
        return r;
        
    case XEC_OPERATOR_INTDIV:
        c->emit( XEC_IDIV, r, a, b );
        return r;
        
    case XEC_OPERATOR_ADD:
        c->emit( XEC_ADD, r, a, b );
        return r;
        
    case XEC_OPERATOR_SUBTRACT:
        c->emit( XEC_SUB, r, a, b );
        return r;
        
    case XEC_OPERATOR_LSHIFT:
        c->emit( XEC_LSL, r, a, b );
        return r;
        
    case XEC_OPERATOR_RSHIFT:
        c->emit( XEC_ASR, r, a, b );
        return r;
        
    case XEC_OPERATOR_URSHIFT:
        c->emit( XEC_LSR, r, a, b );
        return r;
        
    case XEC_OPERATOR_BITAND:
        c->emit( XEC_AND, r, a, b );
        return r;
        
    case XEC_OPERATOR_BITXOR:
        c->emit( XEC_XOR, r, a, b );
        return r;
        
    case XEC_OPERATOR_BITOR:
        c->emit( XEC_OR, r, a, b );
        return r;
        
    case XEC_OPERATOR_CONCATENATE:
        c->emit( XEC_CONCAT, r, a, b );
        return r;
    
    default:
        assert( ! "invalid binary operator" );
        c->emit( XEC_LITNULL, r );
        return r;
    }
}

xec_operand xec_compiler_operand::visit( xec_expression_comparison* expr, int target )
{
    std::pair< xec_opcode, bool > op;

    if ( expr->get_rest_count() == 1 )
    {
        // A single comparison with no shortcut evaluation.
        xec_operand a = c->operand( expr->get_first_expr() );
        xec_operand b = c->operand( expr->get_rest_expr( 0 ) );
        xec_operand r = c->result( target, a, b );
        c->comp( expr->get_operator( 0 ), r, a, b  );
        return r;
    }
    else
    {
        // A list of comparisons each sharing a term with the one before, and
        // requiring shortcut evaluation.
        
        // Allocate temporaries.
        xec_operand q = c->temporary( 0 );  // result
        xec_operand s = c->temporary( 1 );  // middle term (odd terms)
        xec_operand t = c->temporary( 2 );  // middle term (even terms)
        
        // Perform first comparison.
        xec_operand a = c->operand( expr->get_first_expr() );
        assert( (int)a != (int)s );
        c->assign( s, expr->get_rest_expr( 0 ) );
        c->comp( expr->get_operator( 0 ), q, a, s );
        a.dispose();
        
        // To avoid moves we change where we store the shared term.
        xec_operand* prev = &s;
        xec_operand* next = &t;

        // Middle comparisons.  Middle terms must be stored in temporaries
        // even if they are locals since the next term might assign to them.
        xec_label label;
        for ( size_t i = 1; i < expr->get_rest_count() - 1; ++i )
        {
            // Shortcut evaluation.
            c->jump( XEC_IFFALSE, q, &label );
            
            // Perform comparison.
            prev->reserve();
            c->assign( *next, expr->get_rest_expr( i ) );
            c->comp( expr->get_operator( i ), q, *prev, *next );
            prev->release();
            
            // Next is now the term we want to preserve.
            std::swap( prev, next );
        }
        
        // Shortcut evaluation.
        c->jump( XEC_IFFALSE, q, &label );
        
        // Last comparison.
        prev->reserve();
        size_t last = expr->get_rest_count() - 1;
        xec_operand b = c->operand( expr->get_rest_expr( last ) );
        c->comp( expr->get_operator( last ), q, *prev, b );
        prev->release();
        b.dispose();

        // At this point q contains the first false result or the last result
        // condition if all of them were true.
        c->label( &label );
        
        // Ensure result ends up in target location.
        return c->promote( q, target );
    }
}

xec_operand xec_compiler_operand::visit( xec_expression_logical* expr, int target )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_LOGICAND:
    {
        // If lhs is false, return it.  Otherwise return the rhs.
        xec_label label;
        xec_operand q = c->temporary( 0 );
        c->assign( q, expr->get_lhs() );
        c->jump( XEC_IFFALSE, &label );
        c->assign( q, expr->get_rhs() );
        c->label( &label );
        return c->promote( q, target );
    }
    
    case XEC_OPERATOR_LOGICXOR:
    {
        // We can elide nots in some cases.
        xec_operand a = c->operinv( expr->get_lhs() );
        xec_operand b = c->operinv( expr->get_rhs() );
        xec_operand r = c->result( target, a, b );
        c->emit( XEC_LXOR, r, a, b );
        if ( a.inverted() != b.inverted() )
            r.invert();
        return r;
    }
    
    case XEC_OPERATOR_LOGICOR:
    {
        // If lhs is true, return it.  Otherwise return the rhs.
        xec_label label;
        xec_operand q = c->temporary( 0 );
        c->assign( q, expr->get_lhs() );
        c->jump( XEC_IFTRUE, &label );
        c->assign( q, expr->get_rhs() );
        c->label( &label );
        return c->promote( q, target );
    }
    
    default:
        assert( ! "invalid logical operator" );
        return c->result( target );
    }
}

xec_operand xec_compiler_operand::visit( xec_expression_conditional* expr, int target )
{
    xec_operand q = c->temporary( 0 );
    xec_label iftrue, iffalse, ifend;
    c->test( expr->get_condition(), &iftrue, &iffalse );
    c->label( &iftrue );
    c->assign( q, expr->get_iftrue() );
    c->jump( XEC_JUMP, &ifend );
    c->label( &iffalse );
    c->assign( q, expr->get_iffalse() );
    c->label( &ifend );
    return c->promote( q, target );
}

xec_operand xec_compiler_operand::visit( xec_expression_varargs* expr, int target )
{
    xec_operand r = c->result( target );
    c->emit( XEC_VARARGS, r, 1 );
    return r;
}

xec_operand xec_compiler_operand::visit( xec_expression_unpack* expr, int target )
{
    xec_operand r = c->result( target );
    c->emit( XEC_UNPACK, r, 1 );
    return r;
}

void visit( xec_expression_list* expr );
void visit( xec_expression_assign* expr );
void visit( xec_expression_mono* expr );
void visit( xec_expression_declare* expr );
void visit( xec_constructor_new* expr );
void visit( xec_constructor_list* expr );
void visit( xec_constructor_table* expr );
void visit( xec_constructor_object* expr );
void visit( xec_constructor_function* expr );














#if 0

#include "xec_compile.h"
#include <assert.h>
#include "xec_opcodes.h"
#include "xec_scope.h"
#include "xec_astvisitor.h"


class xec_emitloose;
class xec_compiler;
class xec_compiler_lvalue;
class xec_compiler_unpack;
class xec_compiler_thiscall;
class xec_lvalue_list;
class xec_unpack_list;



/*
    Walk the AST to produce loosecode.  Loosecode is an SSA form.

*/


enum xec_unpack_kind
{
    XEC_UNPACK_MONO,
    XEC_UNPACK_FINITE,
    XEC_UNPACK_ALL,
};



typedef int xec_looseop;
typedef int xec_upindex;
typedef int xec_varslot;
typedef std::pair< xec_looseop, xec_looseop > xec_looseop_pair;


static const xec_looseop INVALIDOP = -1;



class xec_emitloose
{
public:

    // Function that's being compiled.

    xec_constructor_function* get_function();


    // Literals and keys.
    
    xec_looseop null();
    xec_looseop literal( const char* string, size_t length );
    xec_looseop literal( double number );
    xec_looseop literal( bool boolean );

    
    // Locals.

    xec_varslot temp();
    xec_varslot local( xec_name* name );
    xec_varslot object( xec_scope* object_scope );
    
    xec_looseop set( xec_varslot value, xec_looseop iassign );
    xec_looseop get( xec_varslot value );


    // Upvals.
    
    xec_upindex upval( xec_name* name );
    xec_upindex upval_object( xec_scope* object_scope );


    // Control flow.

    xec_looseop flow_loop();
    void        flow_break();
    void        flow_continue();
    void        flow_endloop();
    
    void        flow_iftrue( xec_looseop v );
    void        flow_iffalse( xec_looseop v );
    void        flow_else();
    void        flow_endif();

    void        flow_return();


    // Instructions.
    
    xec_looseop emit_immed( xec_opcode opcode, int immediate );
    xec_looseop emit_unary( xec_opcode opcode, xec_looseop a );
    xec_looseop emit_binary( xec_opcode opcode, xec_looseop a, xec_looseop b );
    xec_looseop emit_trinary( xec_opcode opcode,
                        xec_looseop a, xec_looseop b, xec_looseop c );
    xec_looseop emit_key( xec_opcode opcode, xec_looseop a, const char* key );
    xec_looseop emit_upval( xec_opcode opcode, xec_looseop a, xec_upindex u );
    xec_looseop emit_unpack( xec_opcode opcode,
                        xec_unpack_kind r, xec_unpack_list* unpack );
    xec_looseop emit_select( xec_looseop unpack, size_t n );

};




class xec_lvalue_list
{
public:

    explicit xec_lvalue_list( xec_emitloose* e );

    size_t          count();
    xec_looseop     value( size_t n );
    xec_looseop     assign( size_t n, xec_looseop v );
    
};






class xec_unpack_list
{
public:

    explicit xec_unpack_list( xec_emitloose* e );
    
    void            append( xec_looseop v );
    void            append_unpack( xec_unpack_kind unpack_kind, xec_looseop v );
    
    xec_looseop     select( size_t i );

};








class xec_compiler_lvalue : public xec_astvisitor<
                xec_compiler_lvalue, void, xec_lvalue_list* >
{
public:

    explicit xec_compiler_lvalue( xec_compiler* c );

    using xec_astvisitor< xec_compiler_lvalue,
                    void, xec_lvalue_list* >::fallback;
    using xec_astvisitor< xec_compiler_lvalue,
                    void, xec_lvalue_list* >::visit;

    void fallback( xec_declaration* decl, xec_lvalue_list* lvlist );
    void fallback( xec_expression* expr, xec_lvalue_list* lvlist );
    void fallback( xec_statement* stmt, xec_lvalue_list* lvlist );

    void visit( xec_expression_identifier* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_lookup* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_indexkey* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_index* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_conditional* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_list* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_mono* expr, xec_lvalue_list* lvlist );
    
};



class xec_compiler_declare : public xec_astvisitor<
                xec_compiler_declare, void, xec_lvalue_list* >
{
public:

    explicit xec_compiler_declare( xec_compiler* c );

    using xec_astvisitor< xec_compiler_declare,
                    void, xec_lvalue_list* >::fallback;
    using xec_astvisitor< xec_compiler_declare,
                    void, xec_lvalue_list* >::visit;
    

    void fallback( xec_declaration* decl, xec_lvalue_list* lvlist );
    void fallback( xec_expression* expr, xec_lvalue_list* lvlist );
    void fallback( xec_statement* stmt, xec_lvalue_list* lvlist );

    void visit( xec_expression_identifier* expr, xec_lvalue_list* lvlist );
    void visit( xec_expression_list* expr, xec_lvalue_list* lvlist );

};



class xec_compiler_unpack : public xec_astvisitor<
                xec_compiler_unpack, void, xec_unpack_kind, xec_unpack_list* >
{
public:

    explicit xec_compiler_unpack( xec_compiler* c );

    using xec_astvisitor< xec_compiler_unpack,
                    void, xec_unpack_kind, xec_unpack_list* >::fallback;
    using xec_astvisitor< xec_compiler_unpack,
                    void, xec_unpack_kind, xec_unpack_list* >::visit;
    
    void fallback( xec_declaration* decl,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void fallback( xec_expression* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void fallback( xec_statement* stmt,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );

    void visit( xec_expression_yield* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void visit( xec_expression_call* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void visit( xec_expression_varargs* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void visit( xec_expression_unpack* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void visit( xec_expression_list* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );
    void visit( xec_expression_declare* expr,
            xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list );

private:

    xec_compiler* c;

};








class xec_compiler_thiscall
    :   public xec_astvisitor< xec_compiler_thiscall, xec_looseop_pair >
{
public:

    explicit xec_compiler_thiscall( xec_compiler* c );

    using xec_astvisitor< xec_compiler_thiscall, xec_looseop_pair >::fallback;
    using xec_astvisitor< xec_compiler_thiscall, xec_looseop_pair >::visit;

    xec_looseop_pair fallback( xec_declaration* decl );
    xec_looseop_pair fallback( xec_expression* expr );
    xec_looseop_pair fallback( xec_statement* stmt );

    xec_looseop_pair visit( xec_expression_lookup* expr );
    xec_looseop_pair visit( xec_expression_indexkey* expr );


private:

    xec_compiler* c;


};








class xec_compiler : public xec_astvisitor< xec_compiler, xec_looseop >
{
public:

    explicit xec_compiler();
    
    using xec_astvisitor< xec_compiler, xec_looseop >::fallback;
    using xec_astvisitor< xec_compiler, xec_looseop >::visit;

    xec_looseop visit( xec_declaration_var* decl );
    xec_looseop visit( xec_declaration_object* decl );
    xec_looseop visit( xec_declaration_prototype* decl );
    xec_looseop visit( xec_declaration_function* decl );

    xec_looseop visit( xec_expression_null* expr );
    xec_looseop visit( xec_expression_bool* expr );
    xec_looseop visit( xec_expression_number* expr );
    xec_looseop visit( xec_expression_string* expr );
    xec_looseop visit( xec_expression_identifier* expr );
    xec_looseop visit( xec_expression_lookup* expr );
    xec_looseop visit( xec_expression_indexkey* expr );
    xec_looseop visit( xec_expression_index* expr );
    xec_looseop visit( xec_expression_yield* expr );
    xec_looseop visit( xec_expression_call* expr );
    xec_looseop visit( xec_expression_preop* expr );
    xec_looseop visit( xec_expression_postop* expr );
    xec_looseop visit( xec_expression_unary* expr );
    xec_looseop visit( xec_expression_binary* expr );
    xec_looseop visit( xec_expression_comparison* expr );
    xec_looseop visit( xec_expression_logical* expr );
    xec_looseop visit( xec_expression_conditional* expr );
    xec_looseop visit( xec_expression_varargs* expr );
    xec_looseop visit( xec_expression_unpack* expr );
    xec_looseop visit( xec_expression_list* expr );
    xec_looseop visit( xec_expression_assign* expr );
    xec_looseop visit( xec_expression_mono* expr );
    xec_looseop visit( xec_expression_declare* expr );
    xec_looseop visit( xec_constructor_new* expr );
    xec_looseop visit( xec_constructor_list* expr );
    xec_looseop visit( xec_constructor_table* expr );
    xec_looseop visit( xec_constructor_object* expr );
    xec_looseop visit( xec_constructor_function* expr );
    
    xec_looseop visit( xec_statement_declaration* stmt );
    xec_looseop visit( xec_statement_expression* stmt );
    xec_looseop visit( xec_statement_compound* stmt );
    xec_looseop visit( xec_statement_delete* stmt );
    xec_looseop visit( xec_statement_if* stmt );
    xec_looseop visit( xec_statement_switch* stmt );
    xec_looseop visit( xec_statement_case* stmt );
    xec_looseop visit( xec_statement_while* stmt );
    xec_looseop visit( xec_statement_do* stmt );
    xec_looseop visit( xec_statement_foreach* stmt );
    xec_looseop visit( xec_statement_for* stmt );
    xec_looseop visit( xec_statement_continue* stmt );
    xec_looseop visit( xec_statement_break* stmt );
    xec_looseop visit( xec_statement_return* stmt );
    xec_looseop visit( xec_statement_yield* stmt );
    xec_looseop visit( xec_statement_using* stmt );
    xec_looseop visit( xec_statement_usingscope* stmt );
    xec_looseop visit( xec_statement_try* stmt );
    xec_looseop visit( xec_statement_catch* stmt );
    xec_looseop visit( xec_statement_throw* stmt );


private:

    friend class xec_compiler_lvalue;
    friend class xec_compiler_unpack;
    friend class xec_compiler_thiscall;
    

    enum xec_name_qual
    {
        XEC_QUAL_GLOBAL,
        XEC_QUAL_LOCAL,
        XEC_QUAL_THIS,
        XEC_QUAL_OBJECT,
        XEC_QUAL_UPVAL,
        XEC_QUAL_UPVAL_THIS,
        XEC_QUAL_UPVAL_OBJECT,
    };

    xec_name_qual   name_qual( xec_name* name );
    xec_scope*      function_scope( xec_scope* scope );
    xec_opcode      comparison_op( xec_operator_kind op, bool* inverted );

    xec_emitloose* e;
    xec_compiler_lvalue compiler_lvalue;
    xec_compiler_declare compiler_declare;
    xec_compiler_unpack compiler_unpack;
    xec_compiler_thiscall compiler_thiscall;

};




xec_compiler_unpack::xec_compiler_unpack( xec_compiler* c )
    :   c( c )
{
}


void xec_compiler_unpack::fallback( xec_declaration* decl,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
    unpack_list->append( c->visit( decl ) );
}

void xec_compiler_unpack::fallback( xec_expression* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
    unpack_list->append( c->visit( expr ) );
}

void xec_compiler_unpack::fallback( xec_statement* stmt,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
    unpack_list->append( c->visit( stmt ) );
}


void xec_compiler_unpack::visit( xec_expression_yield* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
    if ( ! expr->get_unpack() )
    {
        unpack_list->append( c->visit( expr ) );
        return;
    }
    
    // Evaluate arguments.
    xec_unpack_list arguments( c->e );
    visit( expr->get_arguments(), XEC_UNPACK_ALL, &arguments );

    // Yield instruction, returning multiple values.
    xec_looseop v = c->e->emit_unpack( XEC_YIELD, unpack_kind, &arguments );
    unpack_list->append_unpack( unpack_kind, v );
}

void xec_compiler_unpack::visit( xec_expression_call* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
    if ( expr->get_unpack() )
    {
        unpack_list->append( c->visit( expr ) );
        return;
    }
    
    // Arguments to call instruction include the function and this pointer.
    xec_unpack_list arguments( c->e );
    
    // Find both method object and this (if it's a thiscall).
    xec_looseop_pair method_and_this =
            c->compiler_thiscall.visit( expr->get_expr() );
    arguments.append( method_and_this.first );
    if ( method_and_this.second != INVALIDOP )
        arguments.append( method_and_this.second );
    
    // Evaluate arguments.
    visit( expr->get_arguments(), XEC_UNPACK_ALL, &arguments );
    
    // Call instruction, returning multiple values.
    xec_opcode opcode = expr->get_yieldcall() ? XEC_YIELDCALL : XEC_CALL;
    xec_looseop v = c->e->emit_unpack( opcode, unpack_kind, &arguments );
    unpack_list->append_unpack( unpack_kind, v );
}

void xec_compiler_unpack::visit( xec_expression_varargs* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
}

void xec_compiler_unpack::visit( xec_expression_unpack* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
}

void xec_compiler_unpack::visit( xec_expression_list* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
}

void xec_compiler_unpack::visit( xec_expression_declare* expr,
        xec_unpack_kind unpack_kind, xec_unpack_list* unpack_list )
{
}




xec_compiler_thiscall::xec_compiler_thiscall( xec_compiler* c )
    :   c( c )
{
}

xec_looseop_pair xec_compiler_thiscall::fallback( xec_declaration* decl )
{
    return std::make_pair( c->visit( decl ), INVALIDOP );
}

xec_looseop_pair xec_compiler_thiscall::fallback( xec_expression* expr )
{
    return std::make_pair( c->visit( expr ), INVALIDOP );
}

xec_looseop_pair xec_compiler_thiscall::fallback( xec_statement* stmt )
{
    return std::make_pair( c->visit( stmt ), INVALIDOP );
}

xec_looseop_pair xec_compiler_thiscall::visit( xec_expression_lookup* expr )
{
    xec_looseop o = c->visit( expr->get_expr() );
    xec_looseop r = c->e->emit_key( XEC_GETKEY, o, expr->get_identifier() );
    return std::make_pair( r, o );
}

xec_looseop_pair xec_compiler_thiscall::visit( xec_expression_indexkey* expr )
{
    xec_looseop o = c->visit( expr->get_expr() );
    xec_looseop k = c->visit( expr->get_index() );
    xec_looseop r = c->e->emit_binary( XEC_GETXKEY, o, k );
    return std::make_pair( r, o );
}














xec_looseop xec_compiler::visit( xec_declaration_var* decl )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_declaration_object* decl )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_declaration_prototype* decl )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_declaration_function* decl )
{
    return INVALIDOP;
}


xec_looseop xec_compiler::visit( xec_expression_null* expr )
{
    return e->null();
}

xec_looseop xec_compiler::visit( xec_expression_bool* expr )
{
    return e->literal( expr->get_value() );
}

xec_looseop xec_compiler::visit( xec_expression_number* expr )
{
    return e->literal( expr->get_value() );
}

xec_looseop xec_compiler::visit( xec_expression_string* expr )
{
    return e->literal( expr->get_string(), expr->get_length() );
}

xec_looseop xec_compiler::visit( xec_expression_identifier* expr )
{
    xec_name* name = expr->get_name();

    /*
        Depending on the parse tree there are a few possibilities.
        
        object
        {
            var name;
            
            object
            {
                name -> name
                
                method()
                {
                    name -> ( upval object ).name
                }
            }
            
            method()
            {
                name -> this.name
                
                object
                {
                    name -> this.name
                    
                    method()
                    {
                        name -> ( upval this ).name
                    }
                }
     
                closure()
                {
                    name -> ( upval this ).name
                }
            }
        }
    */


    xec_name_qual qual = name_qual( expr->get_name() );
    switch ( qual )
    {
    case XEC_QUAL_GLOBAL:
    {
        return e->emit_key( XEC_GETGLOBAL, INVALIDOP, expr->get_identifier() );
    }

    case XEC_QUAL_LOCAL:
    {
        return e->get( e->local( name ) );
    }
    
    case XEC_QUAL_THIS:
    {
        xec_scope* scope = function_scope( name->get_scope() );
        xec_name* thisname = scope->lookup_name( "this" );
        xec_looseop thisval = e->get( e->local( thisname ) );
        return e->emit_key( XEC_GETKEY, thisval, name->get_name() );
    }
    
    case XEC_QUAL_OBJECT:
    {
        xec_looseop objval = e->get( e->object( name->get_scope() ) );
        return e->emit_key( XEC_GETKEY, objval, name->get_name() );
    }
    
    case XEC_QUAL_UPVAL:
    {
        return e->get( e->upval( name ) );
    }
    
    case XEC_QUAL_UPVAL_THIS:
    {
        xec_scope* scope = function_scope( name->get_scope() );
        xec_name* thisname = scope->lookup_name( "this" );
        xec_looseop thisval = e->get( e->upval( thisname ) );
        return e->emit_key( XEC_GETKEY, thisval, name->get_name() );
    }
    
    case XEC_QUAL_UPVAL_OBJECT:
    {
        xec_looseop objval = e->get( e->upval_object( name->get_scope() ) );
        return e->emit_key( XEC_GETKEY, objval, name->get_name() );
    }
    }
    
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_lookup* expr )
{
    xec_looseop o = visit( expr->get_expr() );
    return e->emit_key( XEC_GETKEY, o, expr->get_identifier() );
}

xec_looseop xec_compiler::visit( xec_expression_indexkey* expr )
{
    xec_looseop o = visit( expr->get_expr() );
    xec_looseop k = visit( expr->get_index() );
    return e->emit_binary( XEC_GETXKEY, o, k );
}

xec_looseop xec_compiler::visit( xec_expression_index* expr )
{
    xec_looseop o = visit( expr->get_expr() );
    xec_looseop i = visit( expr->get_index() );
    return e->emit_binary( XEC_GETINDEX, o, i );
}

xec_looseop xec_compiler::visit( xec_expression_yield* expr )
{
    // Evaluate arguments.
    xec_unpack_list arguments( e );
    compiler_unpack.visit( expr->get_arguments(), XEC_UNPACK_ALL, &arguments );
    
    // Yield and return a single result.
    xec_looseop v = e->emit_unpack( XEC_YIELD, XEC_UNPACK_MONO, &arguments );
    return v;
}

xec_looseop xec_compiler::visit( xec_expression_call* expr )
{
    // Arguments to call instruction include the function and this pointer.
    xec_unpack_list arguments( e );

    // Find both method object and this (if it's a thiscall).
    xec_looseop_pair method_and_this =
            compiler_thiscall.visit( expr->get_expr() );
    arguments.append( method_and_this.first );
    if ( method_and_this.second != INVALIDOP )
        arguments.append( method_and_this.second );
    
    // Evaluate arguments.
    compiler_unpack.visit( expr->get_arguments(), XEC_UNPACK_ALL, &arguments );
    
    // Call and return a single result.
    xec_opcode opcode = expr->get_yieldcall() ? XEC_YIELDCALL : XEC_CALL;
    xec_looseop v = e->emit_unpack( opcode, XEC_UNPACK_MONO, &arguments );
    return v;
}

xec_looseop xec_compiler::visit( xec_expression_preop* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_INCREMENT:
    {
        xec_lvalue_list lvalues( e );
        compiler_lvalue.visit( expr->get_expr(), &lvalues );
        
        xec_looseop b = e->literal( 1.0 );

        xec_looseop result = INVALIDOP;
        for ( size_t i = 0; i < lvalues.count(); ++i )
        {
            xec_looseop a = lvalues.value( i );
            xec_looseop r = e->emit_binary( XEC_ADD, a, b );
            lvalues.assign( i, r );
            if ( i == 0 )
                result = r;
        }
        
        return result;
    }
    
    case XEC_OPERATOR_DECREMENT:
    {
        xec_lvalue_list lvalues( e );
        compiler_lvalue.visit( expr->get_expr(), &lvalues );
        
        xec_looseop b = e->literal( 1.0 );
        
        xec_looseop result = INVALIDOP;
        for ( size_t i = 0; i < lvalues.count(); ++i )
        {
            xec_looseop a = lvalues.value( i );
            xec_looseop r = e->emit_binary( XEC_SUB, a, b );
            lvalues.assign( i, r );
            if ( i == 0 )
                result = r;
        }
        
        return result;
    }
    
    default:
        assert( ! "invalid prefix operator" );
        return INVALIDOP;
    }
}

xec_looseop xec_compiler::visit( xec_expression_postop* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_INCREMENT:
    {
        xec_lvalue_list lvalues( e );
        compiler_lvalue.visit( expr->get_expr(), &lvalues );
        
        xec_looseop b = e->literal( 1.0 );

        xec_looseop result = INVALIDOP;
        for ( size_t i = 0; i < lvalues.count(); ++i )
        {
            xec_looseop a = lvalues.value( i );
            xec_looseop r = e->emit_binary( XEC_ADD, a, b );
            lvalues.assign( i, r );
            if ( i == 0 )
                result = a;
        }
        
        return result;
    }
    
    case XEC_OPERATOR_DECREMENT:
    {
        xec_lvalue_list lvalues( e );
        compiler_lvalue.visit( expr->get_expr(), &lvalues );
        
        xec_looseop b = e->literal( 1.0 );

        xec_looseop result = INVALIDOP;
        for ( size_t i = 0; i < lvalues.count(); ++i )
        {
            xec_looseop a = lvalues.value( i );
            xec_looseop r = e->emit_binary( XEC_SUB, a, b );
            lvalues.assign( i, r );
            if ( i == 0 )
                result = a;
        }
        
        return result;
    }
    
    default:
        assert( ! "invalid postfix operator" );
        return INVALIDOP;
    }
}

xec_looseop xec_compiler::visit( xec_expression_unary* expr )
{
    xec_looseop o = visit( expr->get_expr() );

    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_POSITIVE:     return o;
    case XEC_OPERATOR_NEGATIVE:     return e->emit_unary( XEC_NEG, o );
    case XEC_OPERATOR_LOGICNOT:     return e->emit_unary( XEC_LNOT, o );
    case XEC_OPERATOR_BITNOT:       return e->emit_unary( XEC_NOT, o );
    
    default:
        assert( ! "invalid unary operator" );
        return INVALIDOP;
    }
}

xec_looseop xec_compiler::visit( xec_expression_binary* expr )
{
    xec_looseop a = visit( expr->get_lhs() );
    xec_looseop b = visit( expr->get_rhs() );
    
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_MULTIPLY:     return e->emit_binary( XEC_MUL, a, b );
    case XEC_OPERATOR_DIVIDE:       return e->emit_binary( XEC_DIV, a, b );
    case XEC_OPERATOR_MODULUS:      return e->emit_binary( XEC_MOD, a, b );
    case XEC_OPERATOR_INTDIV:       return e->emit_binary( XEC_IDIV, a, b );
    case XEC_OPERATOR_ADD:          return e->emit_binary( XEC_ADD, a, b );
    case XEC_OPERATOR_SUBTRACT:     return e->emit_binary( XEC_SUB, a, b );
    case XEC_OPERATOR_LSHIFT:       return e->emit_binary( XEC_LSL, a, b );
    case XEC_OPERATOR_RSHIFT:       return e->emit_binary( XEC_ASR, a, b );
    case XEC_OPERATOR_URSHIFT:      return e->emit_binary( XEC_LSR, a, b );
    case XEC_OPERATOR_BITAND:       return e->emit_binary( XEC_AND, a, b );
    case XEC_OPERATOR_BITXOR:       return e->emit_binary( XEC_XOR, a, b );
    case XEC_OPERATOR_BITOR:        return e->emit_binary( XEC_OR, a, b );
    case XEC_OPERATOR_CONCATENATE:  return e->emit_binary( XEC_CONCAT, a, b );
    
    default:
        assert( ! "invalid binary operator" );
        return INVALIDOP;
    }
}

xec_looseop xec_compiler::visit( xec_expression_comparison* expr )
{
    // Evaluate first expression.
    xec_looseop a = visit( expr->get_first_expr() );
    xec_looseop b = visit( expr->get_rest_expr( 0 ) );
    
    bool inverted = false;
    xec_opcode opcode = comparison_op( expr->get_operator( 0 ), &inverted );
    xec_looseop r = e->emit_binary( opcode, a, b );
    if ( inverted )
        r = e->emit_unary( XEC_NOT, r );
    
    if ( expr->get_rest_count() > 1 )
    {
        // The final result must survive through the shortcut evaluations.
        xec_varslot rslot = e->temp();
        e->set( rslot, r );
        
        for ( size_t i = 1; i < expr->get_rest_count(); ++i )
        {
            // Only evaluate later conditions if the previous ones are true.
            e->flow_iftrue( r );
            r = e->get( rslot );
            
            // Evaluate.
            a = b;
            b = visit( expr->get_rest_expr( i ) );
            opcode = comparison_op( expr->get_operator( 0 ), &inverted );
            r = e->emit_binary( opcode, a, b );
            if ( inverted )
                r = e->emit_unary( XEC_NOT, r );
        
            // Store it.
            e->set( rslot, r );
        }
        
        // Close shortcuts.
        for ( size_t i = 1; i < expr->get_rest_count(); ++i )
        {
            e->flow_endif();
        }
        
        return e->get( rslot );
    }
    else
    {
        // No shortcut evaulation required.
        return r;
    }
}

xec_looseop xec_compiler::visit( xec_expression_logical* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_LOGICAND:
    {
        xec_varslot rslot = e->temp();
        xec_looseop r = visit( expr->get_lhs() );
        e->set( rslot, r );
        e->flow_iftrue( r );
        r = visit( expr->get_rhs() );
        e->set( rslot, r );
        e->flow_endif();
        return e->get( rslot );
    }
    
    case XEC_OPERATOR_LOGICXOR:
    {
        xec_looseop a = visit( expr->get_lhs() );
        xec_looseop b = visit( expr->get_rhs() );
        return e->emit_binary( XEC_LXOR, a, b );
    }
    
    case XEC_OPERATOR_LOGICOR:
    {
        xec_varslot rslot = e->temp();
        xec_looseop r = visit( expr->get_lhs() );
        e->set( rslot, r );
        e->flow_iffalse( r );
        r = visit( expr->get_rhs() );
        e->set( rslot, r );
        e->flow_endif();
        return e->get( rslot );
    }
    
    default:
        assert( ! "invalid logical operator" );
        return INVALIDOP;
    }
}

xec_looseop xec_compiler::visit( xec_expression_conditional* expr )
{
    xec_varslot rslot = e->temp();
    xec_looseop q = visit( expr->get_condition() );
    e->flow_iftrue( q );
    e->set( rslot, visit( expr->get_iftrue() ) );
    e->flow_else();
    e->set( rslot, visit( expr->get_iffalse() ) );
    e->flow_endif();
    return e->get( rslot );
}

xec_looseop xec_compiler::visit( xec_expression_varargs* expr )
{
    xec_unpack_list empty( e );
    return e->emit_unpack( XEC_VARARGS, XEC_UNPACK_MONO, &empty );
}

xec_looseop xec_compiler::visit( xec_expression_unpack* expr )
{
    xec_unpack_list list( e );
    list.append( visit( expr->get_expr() ) );
    xec_looseop r = e->emit_unpack( XEC_UNPACK, XEC_UNPACK_MONO, &list );
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_list* expr )
{
    xec_looseop r = INVALIDOP;
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        xec_looseop l = visit( expr->get_expr( i ) );
        if ( r == INVALIDOP )
            r = l;
    }
    if ( expr->get_final() )
    {
        xec_looseop l = visit( expr->get_final() );
        if ( r == INVALIDOP )
            r = l;
    }
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_assign* expr )
{
    xec_lvalue_list lvalues( e );
    compiler_lvalue.visit( expr->get_lhs(), &lvalues );
    
    xec_unpack_list rvalues( e );
    compiler_unpack.visit( expr->get_rhs(), XEC_UNPACK_FINITE, &rvalues );
    
    xec_looseop r = INVALIDOP;
    for ( size_t i = 0; i < lvalues.count(); ++i )
    {
        xec_looseop l = lvalues.assign( i, rvalues.select( i ) );
        if ( r == INVALIDOP )
            r = l;
    }
    
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_mono* expr )
{
    return visit( expr->get_expr() );
}

xec_looseop xec_compiler::visit( xec_expression_declare* expr )
{
    xec_lvalue_list lvalues( e );
    compiler_declare.visit( expr->get_name_list(), &lvalues );

    xec_unpack_list rvalues( e );
    compiler_unpack.visit( expr->get_expr_list(), XEC_UNPACK_FINITE, &rvalues );
    
    xec_looseop r = INVALIDOP;
    for ( size_t i = 0; i < lvalues.count(); ++i )
    {
        xec_looseop l = lvalues.assign( i, rvalues.select( i ) );
        if ( r == INVALIDOP )
            r = l;
    }
    
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_new* expr )
{
    xec_unpack_list arguments( e );
    arguments.append( visit( expr->get_proto() ) );
    compiler_unpack.visit( expr->get_arguments(), XEC_UNPACK_ALL, &arguments );
    xec_looseop r = e->emit_unpack( XEC_NEW, XEC_UNPACK_MONO, &arguments );
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_list* expr )
{
    xec_looseop l = e->emit_immed( XEC_LIST, (int)expr->get_count() );
    
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        xec_looseop v = visit( expr->get_expr( i ) );
        e->emit_trinary( XEC_PUTINDEX, l, e->literal( (double)i ), v );
    }
    
    if ( expr->get_final() )
    {
        xec_unpack_list final( e );
        final.append( l );
        compiler_unpack.visit( expr->get_final(), XEC_UNPACK_ALL, &final );
        e->emit_unpack( XEC_APPEND, XEC_UNPACK_MONO, &final );
    }
    
    return l;
}

xec_looseop xec_compiler::visit( xec_constructor_table* expr )
{
    xec_looseop t = e->emit_immed( XEC_TABLE, (int)expr->get_count() );
    
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        xec_looseop x = visit( expr->get_key( i ) );
        xec_looseop v = visit( expr->get_value( i ) );
        e->emit_trinary( XEC_PUTINDEX, t, x, v );
    }
    
    return t;
}

xec_looseop xec_compiler::visit( xec_constructor_object* expr )
{


    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_constructor_function* expr )
{
    return INVALIDOP;
}


xec_looseop xec_compiler::visit( xec_statement_declaration* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_expression* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_compound* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_delete* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_if* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_switch* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_case* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_while* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_do* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_foreach* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_for* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_continue* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_break* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_return* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_yield* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_using* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_usingscope* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_try* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_catch* stmt )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_statement_throw* stmt )
{
    return INVALIDOP;
}





 
    
    
 
    

#endif