//
//  xec_expression.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_expression.h"
#include "xec_constructor.h"
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

xec_expression_list* xec_expression::is_list()
{
    // Not a list!
    return NULL;
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

xec_expression_kind xec_expression_null::get_kind()
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

xec_expression_kind xec_expression_bool::get_kind()
{
    return XEC_EXPRESSION_BOOL;
}

int xec_expression_bool::get_location()
{
    return token->sloc;
}

bool xec_expression_bool::get_value()
{
    return token->kind == XEC_KEYWORD_TRUE;
}


/*
    1337
*/

xec_expression_number::xec_expression_number( xec_token* token )
    :   token( token )
{
}

xec_expression_kind xec_expression_number::get_kind()
{
    return XEC_EXPRESSION_NUMBER;
}

int xec_expression_number::get_location()
{
    return token->sloc;
}

double xec_expression_number::get_value()
{
    return strtod( token->text, NULL );
}



/*
    "hello, world!"
*/

xec_expression_string::xec_expression_string( xec_token* token )
    :   token( token )
{
}

xec_expression_kind xec_expression_string::get_kind()
{
    return XEC_EXPRESSION_STRING;
}

int xec_expression_string::get_location()
{
    return token->sloc;
}

size_t xec_expression_string::get_length()
{
    return token->size;
}

const char* xec_expression_string::get_string()
{
    return token->text;
}



/*
    identifier
*/

xec_expression_identifier::xec_expression_identifier( xec_token* token )
    :   token( token )
    ,   name( NULL )
{
}

xec_expression_kind xec_expression_identifier::get_kind()
{
    return XEC_EXPRESSION_IDENTIFIER;
}

int xec_expression_identifier::get_location()
{
    return token->sloc;
}

void xec_expression_identifier::set_name( xec_name* name )
{
    this->name = name;
}

const char* xec_expression_identifier::get_identifier()
{
    return token->text;
}

xec_name* xec_expression_identifier::get_name()
{
    return name;
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

xec_expression_kind xec_expression_lookup::get_kind()
{
    return XEC_EXPRESSION_LOOKUP;
}

int xec_expression_lookup::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_lookup::as_mono()
{
    // ( x.y )() suppresses thiscall.
    return new xec_expression_mono( this );
}

xec_expression* xec_expression_lookup::get_expr()
{
    return expr.get();
}

const char* xec_expression_lookup::get_identifier()
{
    return token->text;
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

xec_expression_kind xec_expression_indexkey::get_kind()
{
    return XEC_EXPRESSION_INDEXKEY;
}

int xec_expression_indexkey::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_indexkey::as_mono()
{
    // ( x.[ y ] )() suppresses thiscall.
    return new xec_expression_mono( this );
}

xec_expression* xec_expression_indexkey::get_expr()
{
    return expr.get();
}

xec_expression* xec_expression_indexkey::get_index()
{
    return index.get();
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

xec_expression_kind xec_expression_index::get_kind()
{
    return XEC_EXPRESSION_INDEX;
}

int xec_expression_index::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_index::get_expr()
{
    return expr.get();
}

xec_expression* xec_expression_index::get_index()
{
    return index.get();
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

xec_expression_kind xec_expression_yield::get_kind()
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

bool xec_expression_yield::get_unpack()
{
    return unpack;
}

xec_expression_list* xec_expression_yield::get_arguments()
{
    return args.get();
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

xec_expression_kind xec_expression_call::get_kind()
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

xec_declaration_function* xec_expression_call::as_function(
                                    xec_statement_compound* body )
{
    xec_constructor_function* function =
            new xec_constructor_function( args.release() );
    function->set_coroutine( yieldcall );
    assert( ! unpack );
    function->set_body( body );
    xec_declaration_function* decl =
            new xec_declaration_function( expr.release(), function );
    delete this;
    return decl;
}

bool xec_expression_call::get_yieldcall()
{
    return yieldcall;
}

bool xec_expression_call::get_unpack()
{
    return unpack;
}

xec_expression* xec_expression_call::get_expr()
{
    return expr.get();
}

xec_expression_list* xec_expression_call::get_arguments()
{
    return args.get();
}



/*
    expr++
    expr--
*/

xec_expression_postop::xec_expression_postop(
                xec_expression* expr, xec_token* token )
    :   expr( expr )
    ,   token( token )
{
}

xec_expression_kind xec_expression_postop::get_kind()
{
    return XEC_EXPRESSION_POSTOP;
}

int xec_expression_postop::get_location()
{
    return token->sloc;
}

xec_operator_kind xec_expression_postop::get_operator()
{
    switch ( token->kind )
    {
    case XEC_TOKEN_INCREMENT:   return XEC_OPERATOR_INCREMENT;
    case XEC_TOKEN_DECREMENT:   return XEC_OPERATOR_DECREMENT;
    default:                    return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_postop::get_expr()
{
    return expr.get();
}



/*
    ++expr
    --expr
*/

xec_expression_preop::xec_expression_preop(
                xec_expression* expr, xec_token* token )
    :   expr( expr )
    ,   token( token )
{
}

xec_expression_kind xec_expression_preop::get_kind()
{
    return XEC_EXPRESSION_PREOP;
}

int xec_expression_preop::get_location()
{
    return token->sloc;
}

xec_operator_kind xec_expression_preop::get_operator()
{
    switch ( token->kind )
    {
    case XEC_TOKEN_INCREMENT:   return XEC_OPERATOR_INCREMENT;
    case XEC_TOKEN_DECREMENT:   return XEC_OPERATOR_DECREMENT;
    default:                    return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_preop::get_expr()
{
    return expr.get();
}


/*
    +expr
    -expr
    !expr
    ~expr
*/

xec_expression_unary::xec_expression_unary(
                    xec_expression* expr, xec_token* token )
    :   expr( expr )
    ,   token( token )
{
}

xec_expression_kind xec_expression_unary::get_kind()
{
    return XEC_EXPRESSION_UNARY;
}

int xec_expression_unary::get_location()
{
    return token->sloc;
}

xec_operator_kind xec_expression_unary::get_operator()
{
    switch ( token->kind )
    {
    case XEC_TOKEN_PLUS:    return XEC_OPERATOR_POSITIVE;
    case XEC_TOKEN_MINUS:   return XEC_OPERATOR_NEGATIVE;
    case XEC_TOKEN_XMARK:   return XEC_OPERATOR_LOGICNOT;
    case XEC_TOKEN_TILDE:   return XEC_OPERATOR_BITNOT;
    default:                return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_unary::get_expr()
{
    return expr.get();
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
    expr .. expr
*/

xec_expression_binary::xec_expression_binary(
            xec_expression* expr_a, xec_token* token, xec_expression* expr_b )
    :   expr_a( expr_a )
    ,   token( token )
    ,   expr_b( expr_b )
{
}

xec_expression_kind xec_expression_binary::get_kind()
{
    return XEC_EXPRESSION_BINARY;
}

int xec_expression_binary::get_location()
{
    return token->sloc;
}

xec_operator_kind xec_expression_binary::get_operator()
{
    switch ( token->kind )
    {
    case XEC_TOKEN_ASTERISK:    return XEC_OPERATOR_MULTIPLY;
    case XEC_TOKEN_SOLIDUS:     return XEC_OPERATOR_DIVIDE;
    case XEC_TOKEN_PERCENT:     return XEC_OPERATOR_MODULUS;
    case XEC_TOKEN_TILDE:       return XEC_OPERATOR_INTDIV;
    case XEC_TOKEN_PLUS:        return XEC_OPERATOR_ADD;
    case XEC_TOKEN_MINUS:       return XEC_OPERATOR_SUBTRACT;
    case XEC_TOKEN_LSHIFT:      return XEC_OPERATOR_LSHIFT;
    case XEC_TOKEN_RSHIFT:      return XEC_OPERATOR_RSHIFT;
    case XEC_TOKEN_URSHIFT:     return XEC_OPERATOR_URSHIFT;
    case XEC_TOKEN_AMPERSAND:   return XEC_OPERATOR_BITAND;
    case XEC_TOKEN_CARET:       return XEC_OPERATOR_BITXOR;
    case XEC_TOKEN_VBAR:        return XEC_OPERATOR_BITOR;
    case XEC_TOKEN_CONCATENATE: return XEC_OPERATOR_CONCATENATE;
    default:                    return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_binary::get_lhs()
{
    return expr_a.get();
}

xec_expression* xec_expression_binary::get_rhs()
{
    return expr_b.get();
}



/*
    expr == expr != expr < expr > expr <=
        expr >= expr in expr !in expr is expr !is expr
*/

xec_expression_comparison::xec_expression_comparison( xec_expression* expr )
    :   expr( expr )
{
}

xec_expression_kind xec_expression_comparison::get_kind()
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

size_t xec_expression_comparison::get_rest_count()
{
    return comparisons.size();
}

xec_expression* xec_expression_comparison::get_first_expr()
{
    return expr.get();
}

xec_operator_kind xec_expression_comparison::get_operator( size_t index )
{
    switch ( comparisons.at( index ).token->kind )
    {
    case XEC_TOKEN_EQUAL:           return XEC_OPERATOR_EQUAL;
    case XEC_TOKEN_NOTEQUAL:        return XEC_OPERATOR_NOTEQUAL;
    case XEC_TOKEN_LESS:            return XEC_OPERATOR_LESS;
    case XEC_TOKEN_GREATER:         return XEC_OPERATOR_GREATER;
    case XEC_TOKEN_LESSEQUAL:       return XEC_OPERATOR_LEQUAL;
    case XEC_TOKEN_GREATEREQUAL:    return XEC_OPERATOR_GEQUAL;
    case XEC_TOKEN_IN:              return XEC_OPERATOR_IN;
    case XEC_TOKEN_NOTIN:           return XEC_OPERATOR_NOTIN;
    case XEC_TOKEN_IS:              return XEC_OPERATOR_IS;
    case XEC_TOKEN_NOTIS:           return XEC_OPERATOR_NOTIS;
    default:                        return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_comparison::get_rest_expr( size_t index )
{
    return comparisons.at( index ).expr.get();
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

xec_expression_kind xec_expression_logical::get_kind()
{
    return XEC_EXPRESSION_LOGICAL;
}

int xec_expression_logical::get_location()
{
    return token->sloc;
}

xec_operator_kind xec_expression_logical::get_operator()
{
    switch ( token->kind )
    {
    case XEC_TOKEN_LOGICAND:    return XEC_OPERATOR_LOGICAND;
    case XEC_TOKEN_LOGICXOR:    return XEC_OPERATOR_LOGICXOR;
    case XEC_TOKEN_LOGICOR:     return XEC_OPERATOR_LOGICOR;
    default:                    return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_logical::get_lhs()
{
    return expr_a.get();
}

xec_expression* xec_expression_logical::get_rhs()
{
    return expr_b.get();
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

xec_expression_kind xec_expression_conditional::get_kind()
{
    return XEC_EXPRESSION_CONDITIONAL;
}

int xec_expression_conditional::get_location()
{
    return condition->get_location();
}

xec_expression* xec_expression_conditional::get_condition()
{
    return condition.get();
}

xec_expression* xec_expression_conditional::get_iftrue()
{
    return iftrue.get();
}

xec_expression* xec_expression_conditional::get_iffalse()
{
    return iffalse.get();
}



/*
    ...
*/

xec_expression_varargs::xec_expression_varargs( xec_token* token )
    :   token( token )
{
}

xec_expression_kind xec_expression_varargs::get_kind()
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

xec_expression_kind xec_expression_unpack::get_kind()
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

xec_expression* xec_expression_unpack::get_expr()
{
    return expr.get();
}



/*
    expr, expr, expr
*/

xec_expression_list::xec_expression_list()
{
}

xec_expression_kind xec_expression_list::get_kind()
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

xec_expression_list* xec_expression_list::is_list()
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

size_t xec_expression_list::get_count()
{
    return expressions.size();
}

xec_expression* xec_expression_list::get_expr( size_t index )
{
    return expressions.at( index ).get();
}

xec_expression* xec_expression_list::get_final()
{
    return final.get();
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

xec_expression_kind xec_expression_assign::get_kind()
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

xec_operator_kind xec_expression_assign::get_operator()
{
    switch ( op->kind )
    {
    case XEC_TOKEN_ASSIGN:          return XEC_OPERATOR_NONE;
    case XEC_TOKEN_MULASSIGN:       return XEC_OPERATOR_MULTIPLY;
    case XEC_TOKEN_DIVASSIGN:       return XEC_OPERATOR_DIVIDE;
    case XEC_TOKEN_MODASSIGN:       return XEC_OPERATOR_MODULUS;
    case XEC_TOKEN_INTDIVASSIGN:    return XEC_OPERATOR_INTDIV;
    case XEC_TOKEN_ADDASSIGN:       return XEC_OPERATOR_ADD;
    case XEC_TOKEN_SUBASSIGN:       return XEC_OPERATOR_SUBTRACT;
    case XEC_TOKEN_LSHIFTASSIGN:    return XEC_OPERATOR_LSHIFT;
    case XEC_TOKEN_RSHIFTASSIGN:    return XEC_OPERATOR_RSHIFT;
    case XEC_TOKEN_URSHIFTASSIGN:   return XEC_OPERATOR_URSHIFT;
    case XEC_TOKEN_BITANDASSIGN:    return XEC_OPERATOR_BITAND;
    case XEC_TOKEN_BITXORASSIGN:    return XEC_OPERATOR_BITXOR;
    case XEC_TOKEN_BITORASSIGN:     return XEC_OPERATOR_BITOR;
    default:                        return XEC_OPERATOR_NONE;
    }
}

xec_expression* xec_expression_assign::get_lhs()
{
    return lvalue.get();
}

xec_expression* xec_expression_assign::get_rhs()
{
    return rvalue.get();
}




/*
    ( expr, ... )
*/

xec_expression_mono::xec_expression_mono( xec_expression* expr )
    :   expr( expr )
{
}

xec_expression_kind xec_expression_mono::get_kind()
{
    return XEC_EXPRESSION_MONO;
}

int xec_expression_mono::get_location()
{
    return expr->get_location();
}

xec_expression* xec_expression_mono::get_expr()
{
    return expr.get();
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

xec_expression_kind xec_expression_declare::get_kind()
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

xec_expression_list* xec_expression_declare::get_name_list()
{
    return name_list.get();
}

xec_expression_list* xec_expression_declare::get_expr_list()
{
    return expr_list.get();
}





