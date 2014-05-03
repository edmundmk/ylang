//
//  xec_expression.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_expression.h"
#include "xec_declaration.h"
#include "xec_statement.h"
#include <assert.h>
#include <stddef.h>
#include "xec_token.h"



xec_expression::~xec_expression()
{
}

xec_expression_list* xec_expression::as_list()
{
    // Most expressions are not lists.
    xec_expression_list* list = new xec_expression_list();
    list->append_expression( this );
    return list;
}

xec_expression* xec_expression::as_mono()
{
    // Most expressions are mono.
    return this;
}

xec_expression_comparison* xec_expression::as_comparison()
{
    // Most expressions are not comparisons.
    xec_expression_comparison* comparison =
                    new xec_expression_comparison( this );
    return comparison;
}



/*
    null
*/

xec_expression_null::xec_expression_null( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_null::visitor_dispatch()
{
    return XEC_EXPRESSION_NULL;
}

int xec_expression_null::get_location()
{
    return token->sloc;
}



/*
    true
    false
*/

xec_expression_bool::xec_expression_bool( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_bool::visitor_dispatch()
{
    return XEC_EXPRESSION_BOOL;
}

int xec_expression_bool::get_location()
{
    return token->sloc;
}


/*
    1337
*/

xec_expression_number::xec_expression_number( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_number::visitor_dispatch()
{
    return XEC_EXPRESSION_NUMBER;
}

int xec_expression_number::get_location()
{
    return token->sloc;
}



/*
    "hello, world!"
*/

xec_expression_string::xec_expression_string( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_string::visitor_dispatch()
{
    return XEC_EXPRESSION_STRING;
}

int xec_expression_string::get_location()
{
    return token->sloc;
}



/*
    identifier
*/

xec_expression_identifier::xec_expression_identifier( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_identifier::visitor_dispatch()
{
    return XEC_EXPRESSION_IDENTIFIER;
}

int xec_expression_identifier::get_location()
{
    return token->sloc;
}



/*
    expr . lookup
*/

xec_expression_lookup::xec_expression_lookup(
                                xec_expression* expr, xec_token* token )
    :   expr( expr )
    ,   token( token )
{
}

xec_expression_dispatch xec_expression_lookup::visitor_dispatch()
{
    return XEC_EXPRESSION_LOOKUP;
}

int xec_expression_lookup::get_location()
{
    return expr->get_location();
}




/*
    expr.[ expr ]
*/

xec_expression_indexkey::xec_expression_indexkey(
                                xec_expression* expr, xec_expression* index )
    :   expr( expr )
    ,   index( index )
{
}

xec_expression_dispatch xec_expression_indexkey::visitor_dispatch()
{
    return XEC_EXPRESSION_INDEXKEY;
}

int xec_expression_indexkey::get_location()
{
    return expr->get_location();
}




/*
    expr[ expr ]
*/

xec_expression_index::xec_expression_index(
                                xec_expression* expr, xec_expression* index )
    :   expr( expr )
    ,   index( index )
{
}

xec_expression_dispatch xec_expression_index::visitor_dispatch()
{
    return XEC_EXPRESSION_INDEX;
}

int xec_expression_index::get_location()
{
    return expr->get_location();
}
    




/*
    yield( expr, expr )
    yield( expr, expr ) ...
*/

xec_expression_yield::xec_expression_yield(
                                xec_token* token, xec_expression_list* args )
    :   token( token )
    ,   args( args )
    ,   unpack( false )
{
}

xec_expression_dispatch xec_expression_yield::visitor_dispatch()
{
    return XEC_EXPRESSION_YIELD;
}

int xec_expression_yield::get_location()
{
    return token->sloc;
}

xec_expression* xec_expression_yield::as_mono()
{
    if ( unpack )
        return new xec_expression_mono( this );
    else
        return this;
}

void xec_expression_yield::set_unpack( bool unpack )
{
    this->unpack = unpack;
}





/*
    expr( expr, expr )
    expr( expr, expr ) yield
    expr( expr, expr ) ...
    expr( expr, expr ) yield ...
*/

xec_expression_call::xec_expression_call(
                        xec_expression* expr, xec_expression_list* args )
    :   expr( expr )
    ,   args( args )
    ,   yieldcall( false )
    ,   unpack( false )
{
}

xec_expression_dispatch xec_expression_call::visitor_dispatch()
{
    return XEC_EXPRESSION_CALL;
}

int xec_expression_call::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_call::as_mono()
{
    if ( unpack )
        return new xec_expression_mono( this );
    else
        return this;
}

void xec_expression_call::set_yieldcall( bool yieldcall )
{
    this->yieldcall = yieldcall;
}

void xec_expression_call::set_unpack( bool unpack )
{
    this->unpack = unpack;
}

xec_declaration_prototype* xec_expression_call::as_prototype()
{
    xec_declaration_prototype* prototype =
            new xec_declaration_prototype( expr.release(), args.release() );
    prototype->set_coroutine( yieldcall );
    assert( ! unpack );
    delete this;
    return prototype;
}

xec_declaration_function* xec_expression_call::as_function()
{
    xec_declaration_function* function =
            new xec_declaration_function( expr.release(), args.release() );
    function->set_coroutine( yieldcall );
    assert( ! unpack );
    delete this;
    return function;
}




/*
    +expr
    -expr
    !expr
    ~expr
    ++expr
    ++expr
    expr++
    expr++
*/

xec_expression_unary::xec_expression_unary(
                    xec_expression* expr, xec_token* token )
    :   expr( expr )
    ,   token( token )
{
}

xec_expression_dispatch xec_expression_unary::visitor_dispatch()
{
    return XEC_EXPRESSION_UNARY;
}

int xec_expression_unary::get_location()
{
    return token->sloc;
}




/*
    expr * expr
    expr / expr
    expr % expr
    expr ~ expr
    expr + expr
    expr - expr
    expr << expr
    expr >> expr
    expr >>> expr
    expr & expr
    expr ^ expr
    expr | expr
*/

xec_expression_binary::xec_expression_binary(
            xec_expression* expr_a, xec_token* token, xec_expression* expr_b )
    :   expr_a( expr_a )
    ,   token( token )
    ,   expr_b( expr_b )
{
}

xec_expression_dispatch xec_expression_binary::visitor_dispatch()
{
    return XEC_EXPRESSION_BINARY;
}

int xec_expression_binary::get_location()
{
    return token->sloc;
}



/*
    expr == expr != expr < expr > expr <=
        expr >= expr in expr !in expr is expr !is expr
*/

xec_expression_comparison::xec_expression_comparison( xec_expression* expr )
    :   expr( expr )
{
}

xec_expression_dispatch xec_expression_comparison::visitor_dispatch()
{
    return XEC_EXPRESSION_COMPARISON;
}

int xec_expression_comparison::get_location()
{
    return expr->get_location();
}

xec_expression_comparison* xec_expression_comparison::as_comparison()
{
    return this;
}
    
void xec_expression_comparison::add_comparison(
                    xec_token* token, xec_expression* expr )
{
    comparison cmp;
    cmp.token = token;
    cmp.expr.reset( expr );
    comparisons.push_back( std::move( cmp ) );
}



/*
    expr && expr
    expr ^^ expr
    expr || expr
*/

xec_expression_logical::xec_expression_logical(
            xec_expression* expr_a, xec_token* token, xec_expression* expr_b )
    :   expr_a( expr_a )
    ,   token( token )
    ,   expr_b( expr_b )
{
}

xec_expression_dispatch xec_expression_logical::visitor_dispatch()
{
    return XEC_EXPRESSION_LOGICAL;
}

int xec_expression_logical::get_location()
{
    return token->sloc;
}



/*
    expr ? expr : expr
*/

xec_expression_conditional::xec_expression_conditional(
            xec_expression* condition, xec_expression* iftrue,
                    xec_expression* iffalse )
    :   condition( condition )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}

xec_expression_dispatch xec_expression_conditional::visitor_dispatch()
{
    return XEC_EXPRESSION_CONDITIONAL;
}

int xec_expression_conditional::get_location()
{
    return condition->get_location();
}



/*
    ...
*/

xec_expression_varargs::xec_expression_varargs( xec_token* token )
    :   token( token )
{
}

xec_expression_dispatch xec_expression_varargs::visitor_dispatch()
{
    return XEC_EXPRESSION_VARARGS;
}

int xec_expression_varargs::get_location()
{
    return token->sloc;
}

xec_expression* xec_expression_varargs::as_mono()
{
    return new xec_expression_mono( this );
}




/*
    expr[] ...
*/

xec_expression_unpack::xec_expression_unpack( xec_expression* expr )
    :   expr( expr )
{
}

xec_expression_dispatch xec_expression_unpack::visitor_dispatch()
{
    return XEC_EXPRESSION_UNPACK;
}

int xec_expression_unpack::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_unpack::as_mono()
{
    return new xec_expression_mono( this );
}



/*
    expr, expr, expr
*/

xec_expression_list::xec_expression_list()
{
}

xec_expression_dispatch xec_expression_list::visitor_dispatch()
{
    return XEC_EXPRESSION_LIST;
}

int xec_expression_list::get_location()
{
    if ( expressions.size() )
        return expressions.at( 0 )->get_location();
    else if ( final )
        return final->get_location();
    else
        return -1;
}

xec_expression* xec_expression_list::as_mono()
{
    if ( expressions.size() == 1 && ! final )
    {
        // One non-final expression, unwrap the list.
        xec_expression* result = expressions.at( 0 ).release();
        delete this;
        return result->as_mono();
    }
    else if ( expressions.empty() && final )
    {
        // One final expression, unwrap the list.
        xec_expression* result = final.release();
        delete this;
        return result->as_mono();
    }
    else
    {
        return new xec_expression_mono( this );
    }
}

xec_expression_list* xec_expression_list::as_list()
{
    return this;
}

void xec_expression_list::append_expression( xec_expression* expression )
{
    expressions.push_back( std::unique_ptr< xec_expression >( expression ) );
}

void xec_expression_list::append_final( xec_expression* final )
{
    this->final.reset( final );
}




/*
    expr = expr
    expr *= expr
    expr /= expr
    expr %= expr
    expr ~= expr
    expr += expr
    expr -= expr
    expr <<= expr
    expr >>= expr
    expr >>>= expr
    expr &= expr
    expr |= expr
    expr ^= expr
*/

xec_expression_assign::xec_expression_assign(
            xec_expression* lvalue, xec_token* op, xec_expression* rvalue )
    :   lvalue( lvalue )
    ,   op( op )
    ,   rvalue( rvalue )
{
}

xec_expression_dispatch xec_expression_assign::visitor_dispatch()
{
    return XEC_EXPRESSION_ASSIGN;
}

int xec_expression_assign::get_location()
{
    return op->sloc;
}

xec_expression* xec_expression_assign::as_mono()
{
    // Assignments can be to more than one value.
    return new xec_expression_mono( this );
}



/*
    ( expr, ... )
*/

xec_expression_mono::xec_expression_mono( xec_expression* expr )
    :   expr( expr )
{
}

xec_expression_dispatch xec_expression_mono::visitor_dispatch()
{
    return XEC_EXPRESSION_MONO;
}

int xec_expression_mono::get_location()
{
    return expr->get_location();
}



/*
    var name, name = expr, expr;
*/

xec_expression_declare::xec_expression_declare( xec_token* token,
            xec_expression_list* name_list, xec_expression_list* expr_list )
    :   token( token )
    ,   name_list( name_list )
    ,   expr_list( expr_list )
{
}

xec_expression_dispatch xec_expression_declare::visitor_dispatch()
{
    return XEC_EXPRESSION_DECLARE;
}

int xec_expression_declare::get_location()
{
    return token->sloc;
}

xec_expression* xec_expression_declare::as_mono()
{
    // Conditions can declare more than one value.
    return new xec_expression_mono( this );
}





