//
//  xec_compile.cpp
//
//  Created by Edmund Kapusniak on 18/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//

#if 1

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
    void        flow_break( xec_looseop ijump );
    void        flow_continue( xec_looseop ijump );
    void        flow_endloop( xec_looseop ijump );
    
    void        flow_if( xec_looseop ijump );
    void        flow_else( xec_looseop ijump );
    void        flow_endif();

    void        flow_return( xec_looseop ireturn );


    // Instructions.
    
    xec_looseop emit_unary( xec_opcode opcode, xec_looseop a );
    xec_looseop emit_binary( xec_opcode opcode, xec_looseop a, xec_looseop b );
    xec_looseop emit_key( xec_opcode opcode, xec_looseop a, const char* key );
    xec_looseop emit_upval( xec_opcode opcode, xec_looseop a, xec_upindex u );
    xec_looseop emit_unpack( xec_opcode opcode, size_t n, xec_unpack_kind r );
    void        emit_argument( xec_looseop v );
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
    
    size_t          count();
    void            emit_arguments();
    xec_looseop     emit_select( int n );

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
    xec_looseop v =
            c->e->emit_unpack( XEC_YIELD, arguments.count(), unpack_kind );
    arguments.emit_arguments();
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
    xec_looseop v =
            c->e->emit_unpack( XEC_CALL, arguments.count(), unpack_kind );
    arguments.emit_arguments();
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
    xec_looseop v =
            e->emit_unpack( XEC_YIELD, arguments.count(), XEC_UNPACK_MONO );
    arguments.emit_arguments();
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
    xec_looseop v =
            e->emit_unpack( XEC_CALL, arguments.count(), XEC_UNPACK_MONO );
    arguments.emit_arguments();
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

    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_logical* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_conditional* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_varargs* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_unpack* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_list* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_assign* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_mono* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_expression_declare* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_constructor_new* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_constructor_list* expr )
{
    return INVALIDOP;
}

xec_looseop xec_compiler::visit( xec_constructor_table* expr )
{
    return INVALIDOP;
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





 
    
    
 
    
 
    


#if 0
 

/*

    Compilation proceeds in two stages.  First, the AST is visited to
    generate loosecode.  loosecode is a set of vm opcodes but with
    SSA-form-esque operand references rather than register numbers.  Trying
    to pin results down to register numbers during the tree walk was driving
    me mad.
    
    Additional loosecode opcodes load and store local variables - unlike real
    SSA there are no phi functions.  Values used across branches must be stored
    in a local and reloaded after the branch.  Also call and yield instructions
    require additional instructions to identify their arguments.  Often
    when unpack expressions are used we must select a particular result.
    
    These additional opcodes should fall out in the register allocation phase,
    when locals, temporaries, arguments, and return values are all pinned down
    to specific virtual machine registers.
    
    loosecode also allows us to perform some additional optimization passes,
    such as constant folding and common subexpression elimination, taking
    advantage of its SSA-like properties.
    
    At least, that is the idea.
*/


enum
{
    XEC_GETLOCAL    = 255,
    XEC_SETLOCAL    = 254,
    XEC_ARGUMENT    = 253,
    XEC_SELECT      = 252,
};


struct xec_loosecode
{
    unsigned    opcode  : 8;
    unsigned    a       : 24;
    unsigned    b       : 24;
    unsigned    c       : 16;
};


class xec_loosebuilder
{
public:

    xec_constructor_function* get_function();

    int     literal( const char* s, size_t length );
    int     literal( double n );
    int     literal( int n );
    int     literal( bool b );
    int     key( const char* s );

    int     label();
    int     label( int jump );
    int     jump( unsigned opcode, int a );
    int     jump( unsigned opcode, int a, int label );
    int     emit( unsigned opcode, int a, int b );
    int     emit( unsigned opcode, int c );

    int     local( xec_name* name );
    int     upval( xec_name* name );
    int     local_object( xec_scope* object_scope );
    int     upval_object( xec_scope* object_scope );

};
















#endif






#if 0

#include "xec_compile.h"
#include <deque>
#include "xec_astvisitor.h"
#include "xec_parser.h"
#include "xec_opcodes.h"
#include "xec_scope.h"


class xec_emitcode;
class xec_lrvalue;
class xec_compiler;




/*
    Emits xec code instructions during compilation.
*/


class xec_emitcode
{
public:

    xec_constructor_function* get_function();

    int     literal( const char* s );
    int     literal( double n );
    int     literal( int n );
    int     literal( bool b );
    int     key( const char* s );
    
    int     upval( xec_name* name );
    int     local( xec_name* name );
    
    int     label();
    void    label( int jump );
    void    emit( xec_opcode opcode, int r, int a, int b );
    void    emit( xec_opcode opcode, int r, int c );
    int     jump( xec_opcode opcode, int r );
    void    jump( xec_opcode opcode, int r, int label );

};




int xec_emitcode::literal( const char* s )
{
    return 0;
}

int xec_emitcode::literal( double n )
{
    return 0;
}

int xec_emitcode::literal( int n )
{
    return 0;
}

int xec_emitcode::literal( bool b )
{
    return 0;
}

int xec_emitcode::key( const char* s )
{
    return 0;
}

void xec_emitcode::emit( xec_opcode opcode, int r, int a, int b )
{
}

void xec_emitcode::emit( xec_opcode opcode, int r, int c )
{
}

int xec_emitcode::label()
{
    return 0;
}

void xec_emitcode::label( int jump )
{
}

int xec_emitcode::jump( xec_opcode opcode, int r)
{
    return 0;
}

void xec_emitcode::jump( xec_opcode opcode, int r, int label )
{
}






/*
    An lrvalue represents requests for values from an AST node, or the values
    resulting from evaluation of an AST node:
    
    Requests:
      -  A request for an lvalue.
      -  A request for value(s), with no temporary(ies) allocated yet.
      -  A set of lvalues into which results should be assigned.  Each is:
            -  A new declaration (not allocated until assigned).
            -  An object lookup, index, or indexkey (with temporaries).
            -  A local.
            -  An upvalue.
            -  A temporary register (not allocated until assigned).
 
    Values:
      -  An lvalue.
      -  An rvalue or list of rvalues in temporary or local registers.
      -  The chain of jumps and result value for a boolean expression.

*/


/*
    An lrvalue represents values requested and generated from an AST node:
  
        -  Generate lvalues to be assigned to later.
        -  Evaluate without generating a value.
        -  Generate an operand (a single value in a register).
        -  Generate n values in adjacent registers (-1 meaning all).
        -  Generate either [ f, this ] or [ f ] depending on expression.
        -  Assign values into the lvalues provided.
 
*/

enum xec_lrkind
{
    XEC_LVALUE,
    XEC_EVALUATE,
    XEC_OPERAND,
    XEC_MULTIVAL,
    XEC_THISCALL,
    XEC_ASSIGN,
};


class xec_lrvalue
{
public:

    xec_lrvalue( xec_emitcode* e, xec_lrkind kind );
    xec_lrvalue( xec_emitcode* e, xec_lrkind kind, int count );

    xec_lrvalue thiscall(); // multival get thiscall at start (munge count if this)
    xec_lrvalue append();   // multival append.

    void        mono(); // restrict request to 1 value
    void        allocate( xec_lrvalue* v ); // allocate storage reusing storage from v
    
    void        local( int local );
    void        global( int key );
    void        upval( int upval );
    void        lookup( xec_lrvalue* o, int key );
    void        indexkey( xec_lrvalue* o, xec_lrvalue* k );
    void        index( xec_lrvalue* o, xec_lrvalue* i );

    bool        open(); // if this is an operand, allocate temporary, if it's an lvalue, possibly allocate temporary.
    bool        update( int r ); // evaluate lvalue, also open (possibly reuse r as temp)
    void        close(); // assign lvalue and turn into rvalue, do not allocate storage.

    int         count();
    int         r();
    

};






#define xec_looseop void
#define ARGT xec_lrvalue*
#define ARG xec_lrvalue* v



class xec_compiler : public xec_astvisitor< xec_compiler, xec_looseop, ARGT >
{
public:

    explicit xec_compiler( xec_emitcode* b );
    
    using xec_astvisitor< xec_compiler, xec_looseop, ARGT >::fallback;
    using xec_astvisitor< xec_compiler, xec_looseop, ARGT >::visit;

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

    xec_opcode comparisonop( xec_operator_kind op, bool* inverted );

    xec_emitcode* e;

};




xec_compiler::xec_compiler( xec_emitcode* e )
    :   e( e )
{
}


xec_looseop xec_compiler::visit( xec_declaration_var* decl )
{
}

xec_looseop xec_compiler::visit( xec_declaration_object* decl )
{
}

xec_looseop xec_compiler::visit( xec_declaration_prototype* decl )
{
}

xec_looseop xec_compiler::visit( xec_declaration_function* decl )
{
}



xec_looseop xec_compiler::visit( xec_expression_null* expr )
{
    if ( v->open() )
    {
        e->emit( XEC_WIPE, v->r(), 1 );
        v->close();
    }
}

xec_looseop xec_compiler::visit( xec_expression_bool* expr )
{
    if ( v->open() )
    {
        e->emit( XEC_LITERAL, v->r(), e->literal( expr->get_value() ) );
        v->close();
    }
}

xec_looseop xec_compiler::visit( xec_expression_number* expr )
{
    if ( v->open() )
    {
        e->emit( XEC_LITERAL, v->r(), e->literal( expr->get_value() ) );
        v->close();
    }
}

xec_looseop xec_compiler::visit( xec_expression_string* expr )
{
    if ( v->open() )
    {
        e->emit( XEC_LITERAL, v->r(), e->literal( expr->get_string() ) );
        v->close();
    }
}

xec_looseop xec_compiler::visit( xec_expression_identifier* expr )
{
    xec_name* name = expr->get_name();
    
    // TODO: identifiers scoped to this (or to outer thises) and methods,
    // which should be thiscall.
    
    if ( name->get_kind() == XEC_NAME_GLOBAL )
    {
        v->global( e->key( name->get_name() ) );
    }
    else if ( name->get_scope()->get_function() != e->get_function() )
    {
        if ( name->get_kind() != XEC_NAME_BASE )
        {
            v->upval( e->upval( name ) );
        }
        else
        {
            xec_name* thisname = name->get_scope()->lookup_name( "this" );
            xec_lrvalue thisval( e, XEC_OPERAND );
            e->emit( XEC_GETUPVAL, thisval.r(), e->upval( thisname ) );
            v->lookup( &thisval, e->key( "base" ) );
        }
    }
    else
    {
        // local
        if ( name->get_kind() != XEC_NAME_BASE )
        {
            v->local( e->local( name ) );
        }
        else
        {
            xec_name* thisname = name->get_scope()->lookup_name( "this" );
            xec_lrvalue thisval( e, XEC_OPERAND );
            thisval.local( e->local( thisname ) );
            v->lookup( &thisval, e->key( "base" ) );
        }
    }
}

xec_looseop xec_compiler::visit( xec_expression_lookup* expr )
{
    xec_lrvalue o( e, XEC_OPERAND );
    visit( expr->get_expr(), &o );
    v->lookup( &o, e->key( expr->get_identifier() ) );
}

xec_looseop xec_compiler::visit( xec_expression_indexkey* expr )
{
    xec_lrvalue o( e, XEC_OPERAND );
    xec_lrvalue k( e, XEC_OPERAND );
    visit( expr->get_expr(), &o );
    visit( expr->get_index(), &k );
    v->indexkey( &o, &k );
}

xec_looseop xec_compiler::visit( xec_expression_index* expr )
{
    xec_lrvalue o( e, XEC_OPERAND );
    xec_lrvalue i( e, XEC_OPERAND );
    visit( expr->get_expr(), &o );
    visit( expr->get_index(), &i );
    v->index( &o, &i );
}

xec_looseop xec_compiler::visit( xec_expression_yield* expr )
{
    /*
        A yield instruction is similar to a call.
        
            yield r, #m, #n
     
        Where:
            -  r is the register containing the first parameter.
            -  m is the number of arguments, or -1 to use the stack top.
            -  n is the number of results desired, or -1 to set the stack top.
            
    */

    if ( ! expr->get_unpack() )
        v->mono();

    xec_lrvalue alloc( e, XEC_MULTIVAL );
    visit( expr->get_arguments(), &alloc );
    v->allocate( &alloc );
    e->emit( XEC_YIELD, alloc.r(), alloc.count(), v->count() );

}


xec_looseop xec_compiler::visit( xec_expression_call* expr )
{
    /*
        A call instruction has the following form:
        
            call r, #m, #n
            
        Where:
            -  r is the register containing the function to call.
            -  m is the number of parameters after r, or -1 to use stack top.
            -  n is the number of results desired, or -1 to set the stack top.

    */

    if ( ! expr->get_unpack() )
        v->mono();

    xec_lrvalue alloc( e, XEC_MULTIVAL );
    xec_lrvalue f = alloc.thiscall();
    visit( expr->get_expr(), &f );
    visit( expr->get_arguments(), &alloc );
    v->allocate( &alloc );
    e->emit( XEC_CALL, f.r(), alloc.count(), v->count() );
}

xec_looseop xec_compiler::visit( xec_expression_preop* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_INCREMENT:
    {
        if ( v->open() )
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );

            if ( l.update( v->r() ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_ADD, l.r(), l.r(), a.r() );
                e->emit( XEC_MOVE, v->r(), l.r(), 0 );
                l.close();
            }
            
            v->close();
        }
        else
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );

            if ( l.update( -1 ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_ADD, l.r(), l.r(), a.r() );
                l.close();
            }
        }
    }
    
    case XEC_OPERATOR_DECREMENT:
    {
        if ( v->open() )
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );
        
            if ( l.update( v->r() ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_SUB, l.r(), l.r(), a.r() );
                e->emit( XEC_MOVE, v->r(), l.r(), 0 );
                l.close();
            }
            
            v->close();
        }
        else
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );

            if ( l.update( -1 ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_SUB, l.r(), l.r(), a.r() );
                l.close();
            }
        }
        
        break;
    }

    default:
        assert( ! "invalid prefix operator" );
        break;
    }
    
}

xec_looseop xec_compiler::visit( xec_expression_postop* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_INCREMENT:
    {
        if ( v->open() )
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );
        
            if ( l.update( -1 ) )
            {
                e->emit( XEC_MOVE, v->r(), l.r(), 0 );
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_ADD, l.r(), l.r(), a.r() );
                l.close();
            }
            
            v->close();
        }
        else
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );
        
            if ( l.update( -1 ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_ADD, l.r(), l.r(), a.r() );
                l.close();
            }
        }

        break;
    }
    
    case XEC_OPERATOR_DECREMENT:
    {
        if ( v->open() )
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );
        
            if ( l.update( -1 ) )
            {
                e->emit( XEC_MOVE, v->r(), l.r(), 0 );
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_SUB, l.r(), l.r(), a.r() );
                l.close();
            }
            
            v->close();
        }
        else
        {
            xec_lrvalue l( e, XEC_LVALUE );
            visit( expr->get_expr(), &l );
        
            if ( l.update( -1 ) )
            {
                xec_lrvalue a( e, XEC_OPERAND );
                e->emit( XEC_LITERAL, a.r(), e->literal( 1.0 ) );
                e->emit( XEC_SUB, l.r(), l.r(), a.r() );
                l.close();
            }
        }

        break;
    }
    
    default:
        assert( ! "invalid postfix operator" );
        break;
    }
}

xec_looseop xec_compiler::visit( xec_expression_unary* expr )
{
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_POSITIVE:
    {
        visit( expr->get_expr(), v );
    }
        
    case XEC_OPERATOR_NEGATIVE:
    {
        xec_lrvalue o( e, XEC_OPERAND );
        visit( expr->get_expr(), &o );
        if ( v->open() )
        {
            e->emit( XEC_NEG, v->r(), o.r(), 0 );
            e->close();
        }
    }
    
    case XEC_OPERATOR_LOGICNOT:
    {
        // Logical not operation will occur when operand is realized.  This
        // allows potential elision of not operations in logical expressions.
        return invertnot( visit( expr->get_expr(), DARG ) );
    }
    
    case XEC_OPERATOR_BITNOT:
    {
        xec_alloc a = realize( visit( expr->get_expr(), DARG ) );
        r = allocate( r, a );
        e->emit( XEC_NOT, r, a, 0 );
        return r;
    }
    
    default:
        assert( ! "invalid postfix operator" );
        return r;
    }
}

xec_looseop xec_compiler::visit( xec_expression_binary* expr )
{
    xec_opcode opcode;
    switch ( expr->get_operator() )
    {
    case XEC_OPERATOR_MULTIPLY:     opcode = XEC_MUL;       break;
    case XEC_OPERATOR_DIVIDE:       opcode = XEC_DIV;       break;
    case XEC_OPERATOR_MODULUS:      opcode = XEC_MOD;       break;
    case XEC_OPERATOR_INTDIV:       opcode = XEC_IDIV;      break;
    case XEC_OPERATOR_ADD:          opcode = XEC_ADD;       break;
    case XEC_OPERATOR_SUBTRACT:     opcode = XEC_SUB;       break;
    case XEC_OPERATOR_LSHIFT:       opcode = XEC_LSL;       break;
    case XEC_OPERATOR_RSHIFT:       opcode = XEC_ASR;       break;
    case XEC_OPERATOR_URSHIFT:      opcode = XEC_LSR;       break;
    case XEC_OPERATOR_BITAND:       opcode = XEC_AND;       break;
    case XEC_OPERATOR_BITXOR:       opcode = XEC_XOR;       break;
    case XEC_OPERATOR_BITOR:        opcode = XEC_OR;        break;
    case XEC_OPERATOR_CONCATENATE:  opcode = XEC_CONCAT;    break;
    default:
        assert( ! "invalid binary operator" );
        return r;
    }
    
    xec_alloc a = realize( visit( expr->get_lhs(), DARG ) );
    xec_alloc b = realize( visit( expr->get_rhs(), DARG ) );
    r = allocate( r, a, b );
    e->emit( opcode, r, a, b );
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_comparison* expr )
{
    // Chained comparison operators follow Python - a < b < c is the same
    // as ( a < b ) && ( b < c ), with b evaluated at most once.  Shortcut
    // evaluation is present, so c is only evaluated if a < b is true.
    
    xec_alloc a = realize( visit( expr->get_first_expr(), DARG ) );
    
    for ( size_t i = 0; i < expr->get_rest_count() - 1; ++i )
    {
        xec_alloc b = realize( visit( expr->get_rest_expr( i ), DARG ) );
        r = shuffle( r, a, b );

        bool inverted;
        xec_opcode opcode = comparisonop( expr->get_operator( i ), &inverted );
        e->emit( opcode, r, a, b );

        if ( inverted )
            r = invertnot( r );
        
        r = shortand( r );
        
        a = b;
    }
    
    size_t i = expr->get_rest_count() - 1;
    xec_alloc b = realize( visit( expr->get_rest_expr( i ), DARG ) );

    r = allocate( r, a, b );

    bool inverted;
    xec_opcode opcode = comparisonop( expr->get_operator( i ), &inverted );
    e->emit( opcode, r, a, b );

    if ( inverted )
        r = invertnot( r );
    
    return r;
}


xec_opcode xec_compiler::comparisonop( xec_operator_kind op, bool* inverted )
{
    switch ( op )
    {
    case XEC_OPERATOR_EQUAL:    *inverted = false; return XEC_EQ;
    case XEC_OPERATOR_NOTEQUAL: *inverted = true;  return XEC_EQ;
    case XEC_OPERATOR_LESS:     *inverted = false; return XEC_LT;
    case XEC_OPERATOR_GREATER:  *inverted = true;  return XEC_LE;
    case XEC_OPERATOR_LEQUAL:   *inverted = false; return XEC_LE;
    case XEC_OPERATOR_GEQUAL:   *inverted = true;  return XEC_LT;
    case XEC_OPERATOR_IN:       *inverted = false; return XEC_IN;
    case XEC_OPERATOR_NOTIN:    *inverted = true;  return XEC_IN;
    case XEC_OPERATOR_IS:       *inverted = false; return XEC_IS;
    case XEC_OPERATOR_NOTIS:    *inverted = true;  return XEC_IS;
    default:
        assert( ! "invalid comparison operator" );
        *inverted = false; return XEC_EQ;
    }
}


xec_looseop xec_compiler::visit( xec_expression_logical* expr )
{
    switch( expr->get_operator() )
    {
    case XEC_OPERATOR_LOGICAND:
    {
        r = visit( expr->get_lhs(), DARG );
        r = shortand( r );
        r = visit( expr->get_rhs(), r );
        return r;
    }
    
    case XEC_OPERATOR_LOGICXOR:
    {
        bool ainv, binv;
        xec_alloc a = realizeinv( visit( expr->get_lhs(), DARG ), &ainv );
        xec_alloc b = realizeinv( visit( expr->get_rhs(), DARG ), &binv );
        
        r = allocate( r, a, b );
        e->emit( XEC_LXOR, r, a, b );
        
        // If both operands were normal or both were inverted, then the
        // xor operation gives the correct result.  If only one operand
        // was inverted, then the result is always wrong and requires
        // inversion itself.
        if ( ainv != binv )
            r = invertnot( r );
        
        return r;
    }
    
    case XEC_OPERATOR_LOGICOR:
    {
        r = visit( expr->get_lhs(), DARG );
        r = shortor( r );
        r = visit( expr->get_rhs(), r );
        return r;
    }
    
    default:
        assert( ! "invalid logical operator" );
        return r;
    }
}

xec_looseop xec_compiler::visit( xec_expression_conditional* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_varargs* expr )
{
    r = allocate( r );
    e->emit( XEC_VARARGS, r, 0, r.count() );
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_unpack* expr )
{
    xec_alloc a = realize( visit( expr->get_expr(), DARG ) );
    r = allocate( r, a );
    e->emit( XEC_UNPACK, r, a, r.count() );
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_list* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_assign* expr )
{
    // Right this is the complicated one.
    
    
    expr->get_operator();
    expr->get_rhs();
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_mono* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_expression_declare* expr )
{
    return r;
}



xec_looseop xec_compiler::visit( xec_constructor_new* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_list* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_table* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_object* expr )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_constructor_function* expr )
{
    return r;
}



xec_looseop xec_compiler::visit( xec_statement_declaration* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_expression* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_compound* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_delete* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_if* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_switch* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_case* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_while* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_do* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_foreach* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_for* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_continue* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_break* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_return* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_yield* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_using* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_usingscope* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_try* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_catch* stmt )
{
    return r;
}

xec_looseop xec_compiler::visit( xec_statement_throw* stmt )
{
    return r;
}



xec_alloc xec_compiler::local( int r )
{
    // Allocation represents register r, the location of a local.
    return xec_alloc();
}

xec_alloc xec_compiler::realize( xec_alloc r )
{
    // 'Realize' an allocation so that it can be used as an operand.

    bool inverted;
    r = realizeinv( r, &inverted );

    if ( inverted )
        e->emit( XEC_NOT, r, r, 0 );

    return r;
}

xec_alloc xec_compiler::realizeinv( xec_alloc r, bool* inverted )
{
    // 'Realize' an allocation so that it can be used as an operand,
    // but allow a resulting boolean value to potentially be inverted.
    return r;
}

xec_alloc xec_compiler::allocate()
{
    return xec_alloc();
}

xec_alloc xec_compiler::allocate( xec_alloc r )
{
    // 'Allocate' an allocation to provide a result location.
    return r;
}

xec_alloc xec_compiler::allocate( xec_alloc r, xec_alloc a )
{
    // Provide a result location for an operation which consumes operand a.
    return r;
}

xec_alloc xec_compiler::allocate( xec_alloc r, xec_alloc a, xec_alloc b )
{
    // Provide a result location for an operation consuming operands a and b.
    return r;
}

xec_alloc xec_compiler::shuffle( xec_alloc r, xec_alloc a, xec_alloc b )
{
    // Provide a result location for an operation consuming operand a, but
    // operand b must remain valid.  There may be a chain of shuffles.
    return r;
}

xec_alloc xec_compiler::invertnot( xec_alloc r )
{
    // r represents a conditional expression which requires inversion when
    // it is realized or when it is used as a condition.
    return r;
}

xec_alloc xec_compiler::shortand( xec_alloc r )
{
    // r represents a conditional expression.  Set up shortcut evaluation
    // of a logical and operation - if the expression is false at this point
    // then the final result is false.  Note that the value may be inverted.
    return r;
}

xec_alloc xec_compiler::shortor( xec_alloc r )
{
    // r represents a conditional expression.  Set up shortcut evaluation
    // of a logical or operation - if the expression is true at this point
    // then the final result is true.  Note that the value may be inverted.
    return r;
}

void xec_compiler::release( xec_alloc r )
{
    // Release an allocation.
}






void xec_compile( xec_parser* parser, xec_code* code )
{
    xec_emitcode emitter;
    xec_compiler compiler( &emitter );
    compiler.visit( parser->get_script(), DARG );
}



#endif

#endif 



