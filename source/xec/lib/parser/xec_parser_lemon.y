//
//  xec_parser.lemon.y
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


/*
    lemon xec_parser.lemon.y
*/



%name               XecParse
%token_prefix       LEMON_

%include
{

#include <assert.h>
#include "xec_parser.h"
#include "xec_token.h"
#include "xec_ast.h"


struct xec_token_scope
{
    xec_token*      token;
    xec_ast_scope*  scope;
};


inline xec_token_scope make_token_scope(
                xec_token* token, xec_ast_scope* scope )
{
    xec_token_scope result;
    result.token = token;
    result.scope = scope;
    return result;
}


}


%extra_argument     { xec_parser* p }
%token_type         { xec_token* }
%default_type       { xec_ast_node* }

%type proto         { xec_unqual_proto* }
%type expr_call     { xec_expr_call* }
%type expr_yield    { xec_expr_yield* }

%type object_lbr    { xec_new_object* }
%type newobj_lbr    { xec_new_object* }

%type func_lbr      { xec_ast_func* }
%type newfunc_lbr   { xec_ast_func* }

%type varname       { xec_unqual_name* }
%type varname_list  { xec_unqual_list* }

%type compare_op    { xec_token* }
%type assign_op     { xec_token* }
%type value_lbody   { xec_new_list* }
%type value_list    { xec_new_list* }
%type keyval_lbody  { xec_new_table* }
%type keyval_list   { xec_new_table* }

%type stmt_lbr      { xec_stmt_block* }
%type stmt_lbr_ru   { xec_stmt_block* }
%type stmt_if       { xec_stmt_if* }
%type stmt_switch   { xec_stmt_switch* }
%type stmt_while    { xec_stmt_while* }
%type stmt_do       { xec_stmt_do* }
%type scope_for     { xec_token_scope }
%type stmt_foreach  { xec_stmt_foreach* }
%type stmt_for      { xec_stmt_for* }
%type stmt_using    { xec_stmt_using* }
%type try_block     { xec_stmt_try* }
%type try_catch     { xec_stmt_try* }
%type scope_catch   { xec_token_scope }
%type stmt_catch    { xec_stmt_catch* }

%type token_yield   { xec_token* }


%include
{


void xec_parser::destroy( xec_token* token )
{
    token->~xec_token();
    recycle_tokens.push_back( token );
}


}


%default_destructor
{
}

%token_destructor
{
    p->destroy( $$ );
}





//
// Conflicts.
//

// Dangling 'else'.
%nonassoc IF .
%nonassoc ELSE .

// Dangling 'catch'/'finally'.
%nonassoc TRY .
%nonassoc CATCH .
%nonassoc FINALLY .

// yield expression: yield( a, b ) vs yield statement: yield a, b
// Conflicts if the first expression in the yield statement is in parentheses.
// Resolve in favour of the yield expression (can always add more parentheses).
%nonassoc YIELD .

// The (higher-precedence) token to shift in the previous two cases.
%nonassoc LPN .




script          ::= stmt_list .
                {
                }




//
//  declaration name or expression.
//


name(x)         ::= IDENTIFIER(token) .
                {
                    x = p->alloc< xec_unqual_name >(
                                    token->sloc, token->text );
                }
name(x)         ::= name(name) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_unqual_qual >(
                                    token->sloc, name, token->text );
                }

proto(x)        ::= name(name) expr_paren(params) .
                {
                    x = p->alloc< xec_unqual_proto >(
                                    name->sloc, name, p->list( params ) );
                }

expr_paren(x)   ::= LPN RPN .
                {
                    x = nullptr;
                }
expr_paren(x)   ::= LPN expr_list(list) RPN .
                {
                    x = list;
                }




//
// declarations
//


// objects
decl_object     ::= object_lbr(object) object_decls RBR .
                {
                    p->close_scope( object->scope );
                }

object_lbr(x)   ::= name(name) LBR .
                {
                    x = p->object( name->sloc, name, nullptr );
                }
object_lbr(x)   ::= name(name) COLON expr_simple(proto) LBR .
                {
                    proto = p->resolve( proto );
                    x = p->object( name->sloc, name, proto );
                }

object_decls    ::= .
                {
                }
object_decls    ::= object_decls object_decl .
                {
                }

object_decl     ::= SEMICOLON .
                {
                }
object_decl     ::= decl_object .
                {
                }
object_decl     ::= decl_func .
                {
                }
object_decl     ::= decl_proto SEMICOLON .
                {
                }
object_decl     ::= decl_var SEMICOLON .
                {
                }
object_decl     ::= decl_noinit SEMICOLON .
                {
                }



// prototypes
decl_proto      ::= proto(proto) .
                {
                    p->prototype( proto->sloc, proto );
                }
decl_proto      ::= proto(proto) YIELD .
                {
                    proto->coroutine = true;
                    p->prototype( proto->sloc, proto );
                }


// functions (with implementation)
decl_func       ::= func_lbr(func) stmt_list RBR .
                {
                    p->close_scope( func->scope );
                }

func_lbr(x)     ::= proto(proto) LBR .
                {
                    x = p->function( proto->sloc, proto->name,
                                    proto->params, proto->coroutine, false );
                }
func_lbr(x)     ::= proto(proto) YIELD LBR .
                {
                    proto->coroutine = true;
                    x = p->function( proto->sloc, proto->name,
                                    proto->params, proto->coroutine, false );
                }


// variable declarations.
decl_var        ::= VAR(token) varname(varname) ASSIGN expr_list(rval) .
                {
                    p->var( token->sloc, varname, p->resolve( rval ) );
                    p->destroy( token );
                }
decl_var        ::= VAR(token) varname_list(varnames) ASSIGN expr_list(rvals) .
                {
                    p->var_list( token->sloc, varnames, p->resolve( rvals ) );
                    p->destroy( token );
                }

decl_noinit     ::= VAR(token) varname(varname) .
                {
                    p->var( token->sloc, varname, nullptr );
                    p->destroy( token );
                }
decl_noinit     ::= VAR(token) varname_list(varnames) .
                {
                    p->var_list( token->sloc, varnames, nullptr );
                    p->destroy( token );
                }


// varnames and varname lists.
varname(x)      ::= IDENTIFIER(token) .
                {
                    x = p->alloc< xec_unqual_name >( token->sloc, token->text );
                    p->destroy( token );
                }

varname_list(x) ::= varname(a) COMMA varname(b).
                {
                    x = p->alloc< xec_unqual_list >( a->sloc );
                    x->names.push_back( a );
                    x->names.push_back( b );
                }
varname_list(x) ::= varname_list(list) COMMA varname(name) .
                {
                    x = list;
                    x->names.push_back( name );
                }





//
// expressions
//


// All lookups which aren't bare names, up to the first call parenthesis.
expr_index(x)   ::= LPN expr_assign(expr) RPN .
                {
                    expr = p->resolve( expr );
                    switch ( expr->kind )
                    {
                    case XEC_EXPR_KEY:
                    case XEC_EXPR_INKEY:
                    case XEC_EXPR_CALL:
                    case XEC_EXPR_CALLKEY:
                    case XEC_EXPR_CALLINKEY:
                    case XEC_EXPR_YIELD:
                    case XEC_EXPR_VARARG:
                    case XEC_EXPR_UNPACK:
                    case XEC_EXPR_LIST:
                    case XEC_EXPR_ASSIGN_LIST:
                        x = p->alloc< xec_expr_mono >( expr->sloc, expr );
                        break;
                        
                    default:
                        x = expr;
                        break;
                    }
                }
expr_index(x)   ::= name(expr) PERIOD LSQ expr_value(key) RSQ .
                {
                    expr  = p->resolve( expr );
                    key   = p->resolve( key );
                    x = p->alloc< xec_expr_inkey >( expr->sloc, expr, key );
                }
expr_index(x)   ::= name(expr) LSQ expr_value(index) RSQ .
                {
                    expr  = p->resolve( expr );
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >( expr->sloc, expr, index );
                }
expr_index(x)   ::= expr_index(expr) PERIOD IDENTIFIER(token) .
                {
                    expr  = p->resolve( expr );
                    x = p->alloc< xec_expr_key >( expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_index(x)   ::= expr_index(expr) PERIOD LSQ expr_value(key) RSQ .
                {
                    expr  = p->resolve( expr );
                    key   = p->resolve( key );
                    x = p->alloc< xec_expr_inkey >( expr->sloc, expr, key );
                }
expr_index(x)   ::= expr_index(expr) LSQ expr_value(index) RSQ .
                {
                    expr  = p->resolve( expr );
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >( expr->sloc, expr, index );
                }

// 'yield' expression - looks like a call but isn't.
expr_yield(x)   ::= YIELD(token) LPN /* shift in preference to token_yield */
                                    RPN .
                {
                    x = p->alloc< xec_expr_yield >( token->sloc, nullptr );
                    p->destroy( token );
                }
expr_yield(x)   ::= YIELD(token) LPN /* shift in preference to token_yield */
                                expr_list(args) RPN .
                {
                    x = p->alloc< xec_expr_yield >(
                            token->sloc, p->list( p->resolve( args ) ) );
                    p->destroy( token );
                }

// 'new' constructor - looks like a call but isn't.
expr_new(x)     ::= NEW(token) name(proto) expr_paren(args) .
                {
                    proto = p->resolve( proto );
                    x = p->alloc< xec_new_new >(
                            token->sloc, proto, p->list( p->resolve( args ) ) );
                    p->destroy( token );
                }
expr_new(x)     ::= NEW(token) expr_index(proto) expr_paren(args) .
                {
                    x = p->alloc< xec_new_new >(
                            token->sloc, proto, p->list( p->resolve( args ) ) );
                    p->destroy( token );
                }

// All call expressions that aren't bare prototypes.
expr_call(x)    ::= proto(expr) expr_paren(args) .
                {
                    xec_expr_call* call = p->resolve( expr );
                    x = p->alloc< xec_expr_call >(
                            call->sloc, call, p->list( p->resolve( args ) ) );
                }
expr_call(x)    ::= expr_index(expr) expr_paren(args) .
                {
                    x = p->alloc< xec_expr_call >(
                            expr->sloc, expr, p->list( p->resolve( args ) ) );
                }
expr_call(x)    ::= expr_yield(expr) expr_paren(args) .
                {
                    x = p->alloc< xec_expr_call >(
                            expr->sloc, expr, p->list( p->resolve( args ) ) );
                }
expr_call(x)    ::= expr_new(expr) expr_paren(args) .
                {
                    x = p->alloc< xec_expr_call >(
                            expr->sloc, expr, p->list( p->resolve( args ) ) );
                }
expr_call(x)    ::= expr_call(expr) expr_paren(args) .
                {
                    x = p->alloc< xec_expr_call >(
                            expr->sloc, expr, p->list( p->resolve( args ) ) );
                }
expr_call(x)    ::= expr_postfix(expr) expr_paren(args) .
                {
                    x = p->alloc< xec_expr_call >(
                            expr->sloc, expr, p->list( p->resolve( args ) ) );
                }

// All lookups after the first call parenthesis.
expr_postfix(x) ::= proto(expr) PERIOD IDENTIFIER(token) .
                {
                    xec_expr_call* call = p->resolve( expr );
                    x = p->alloc< xec_expr_key >(
                             call->sloc, call, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= proto(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    xec_expr_call* call = p->resolve( expr );
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_inkey >(
                             call->sloc, call, index );
                }
expr_postfix(x) ::= proto(expr) LSQ expr_value(index) RSQ .
                {
                    xec_expr_call* call = p->resolve( expr );
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >(
                             call->sloc, call, index );
                }
expr_postfix(x) ::= expr_yield(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >(
                             expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= expr_yield(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_inkey >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_yield(expr) LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_new(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >(
                             expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= expr_new(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_inkey >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_new(expr) LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_call(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >(
                             expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= expr_call(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_inkey >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_call(expr) LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >(
                             expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_inkey >(
                             expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) LSQ expr_value(index) RSQ .
                {
                    index = p->resolve( index );
                    x = p->alloc< xec_expr_index >(
                             expr->sloc, expr, index );
                }

// All lookup, call, and call-like expressions, including yield calls.  Used
// to specify prototypes, as it makes little sense to inherit from the result
// of an arithmetic expression, or from a literal.  Yield calls are at this
// precedence level because it is confusing to allow further postfix -
// a() yield.b - or call - a() yield() - expressions after the yield keyword.
expr_simple(x)  ::= name(expr) .
                {
                    // Don't resolve the name here, as it may be a parameter
                    // declaration for a prototype or function.
                    x = expr;
                }
expr_simple(x)  ::= proto(expr) .
                {
                    x = p->resolve( expr );
                }
expr_simple(x)  ::= expr_index(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_yield(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_new(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_call(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_postfix(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= proto(expr) YIELD .
                {
                    xec_expr_call* call = p->resolve( expr );
                    call->yieldcall = true;
                    x = call;
                }
expr_simple(x)  ::= expr_call(expr) YIELD .
                {
                    expr->yieldcall = true;
                    x = expr;
                }

expr_literal(x) ::= expr_simple(expr) .
                {
                    x = expr;
                }
expr_literal(x) ::= NULL(token) .
                {
                    x = p->alloc< xec_expr_null >( token->sloc );
                    p->destroy( token );
                }
expr_literal(x) ::= TRUE(token) .
                {
                    x = p->alloc< xec_expr_bool >( token->sloc, true );
                    p->destroy( token );
                }
expr_literal(x) ::= FALSE(token) .
                {
                    x = p->alloc< xec_expr_bool >( token->sloc, false );
                    p->destroy( token );
                }
expr_literal(x) ::= NUMBER(token) .
                {
                    double number = p->parse_number( token );
                    x = p->alloc< xec_expr_number >( token->sloc, number );
                    p->destroy( token );
                }
expr_literal(x) ::= STRING(token) .
                {
                    x = p->alloc< xec_expr_string >(
                            token->sloc, token->text, token->size );
                    p->destroy( token );
                }

expr_suffix(x)  ::= expr_literal(expr) .
                {
                    x = expr;
                }
expr_suffix(x)  ::= expr_suffix(expr) INCREMENT(token) .
                {
                    expr = p->resolve( expr );
                    expr = p->lvalue( expr );
                    x = p->alloc< xec_expr_postop >(
                             expr->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_suffix(x)  ::= expr_suffix(expr) DECREMENT(token) .
                {
                    expr = p->resolve( expr );
                    expr = p->lvalue( expr );
                    x = p->alloc< xec_expr_postop >(
                             expr->sloc, token->kind, expr );
                    p->destroy( token );
                }

expr_unary(x)   ::= expr_suffix(expr) .
                {
                    x = expr;
                }
expr_unary(x)   ::= PLUS(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= MINUS(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= XMARK(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= TILDE(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= INCREMENT(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    expr = p->lvalue( expr );
                    x = p->alloc< xec_expr_preop >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= DECREMENT(token) expr_unary(expr) .
                {
                    expr = p->resolve( expr );
                    expr = p->lvalue( expr );
                    x = p->alloc< xec_expr_preop >(
                             token->sloc, token->kind, expr );
                    p->destroy( token );
                }

expr_mul(x)     ::= expr_unary(expr) .
                {
                    x = expr;
                }
expr_mul(x)     ::= expr_mul(lhs) ASTERISK(token) expr_unary(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) SOLIDUS(token) expr_unary(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) PERCENT(token) expr_unary(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) TILDE(token) expr_unary(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_add(x)     ::= expr_mul(expr) .
                {
                    x = expr;
                }
expr_add(x)     ::= expr_add(lhs) PLUS(token) expr_mul(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_add(x)     ::= expr_add(lhs) MINUS(token) expr_mul(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_shift(x)   ::= expr_add(expr) .
                {
                    x = expr;
                }
expr_shift(x)   ::= expr_shift(lhs) LSHIFT(token) expr_add(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_shift(x)   ::= expr_shift(lhs) RSHIFT(token) expr_add(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }
expr_shift(x)   ::= expr_shift(lhs) URSHIFT(token) expr_add(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_bitand(x)  ::= expr_shift(expr) .
                {
                    x = expr;
                }
expr_bitand(x)  ::= expr_bitand(lhs) AMPERSAND(token) expr_shift(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_bitxor(x)  ::= expr_bitand(expr) .
                {
                    x = expr;
                }
expr_bitxor(x)  ::= expr_bitxor(lhs) CARET(token) expr_bitand(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_bitor(x)   ::= expr_bitxor(expr) .
                {
                    x = expr;
                }
expr_bitor(x)   ::= expr_bitor(lhs) VBAR(token) expr_bitxor(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_concat(x)  ::= expr_bitor(expr) .
                {
                    x = expr;
                }
expr_concat(x)  ::= expr_concat(lhs) CONCATENATE(token) expr_bitor(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_binary >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_compare(x) ::= expr_concat(expr) .
                {
                    x = expr;
                }
expr_compare(x) ::= expr_compare(lhs) compare_op(token) expr_concat(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->compare( token, lhs, rhs );
                    p->destroy( token );
                }

compare_op(x)   ::= EQUAL(token) .
                {
                    x = token;
                }
compare_op(x)   ::= NOTEQUAL(token) .
                {
                    x = token;
                }
compare_op(x)   ::= LESS(token) .
                {
                    x = token;
                }
compare_op(x)   ::= GREATER(token) .
                {
                    x = token;
                }
compare_op(x)   ::= LESSEQUAL(token) .
                {
                    x = token;
                }
compare_op(x)   ::= GREATEREQUAL(token) .
                {
                    x = token;
                }
compare_op(x)   ::= IN(token) .
                {
                    x = token;
                }
compare_op(x)   ::= NOTIN(token) .
                {
                    x = token;
                }
compare_op(x)   ::= IS(token) .
                {
                    x = token;
                }
compare_op(x)   ::= NOTIS(token) .
                {
                    x = token;
                }


expr_and(x)     ::= expr_compare(expr) .
                {
                    x = expr;
                }
expr_and(x)     ::= expr_and(lhs) LOGICAND(token) expr_compare(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_logical >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_xor(x)     ::= expr_and(expr) .
                {
                    x = expr;
                }
expr_xor(x)     ::= expr_xor(lhs) LOGICXOR(token) expr_and(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_logical >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

expr_or(x)      ::= expr_xor(expr) .
                {
                    x = expr;
                }
expr_or(x)      ::= expr_or(lhs) LOGICOR(token) expr_xor(rhs) .
                {
                    lhs = p->resolve( lhs );
                    rhs = p->resolve( rhs );
                    x = p->alloc< xec_expr_logical >(
                                    lhs->sloc, token->kind, lhs, rhs );
                    p->destroy( token );
                }

// Must exclude an open brace to avoid conflict with compound statement.
expr_nolbr(x)   ::= expr_or(expr) .
                {
                    x = expr;
                }
expr_nolbr(x)   ::= expr_or(condition) QMARK
                                expr_value(iftrue) COLON expr_value(iffalse) .
                {
                    condition = p->resolve( condition );
                    iftrue    = p->resolve( iftrue );
                    iffalse   = p->resolve( iffalse );
                    x = p->alloc< xec_expr_qmark >(
                            condition->sloc, condition, iftrue, iffalse );
                }
expr_nolbr(x)   ::= LSQ(token) RSQ .
                {
                    x = p->alloc< xec_new_list >( token->sloc );
                    p->destroy( token );
                }
expr_nolbr(x)   ::= LSQ(token) value_list(expr) RSQ .
                {
                    x = expr;
                    x->sloc = token->sloc;
                    p->destroy( token );
                }
expr_nolbr(x)   ::= newobj_lbr(object) object_decls RBR .
                {
                    x = object;
                    p->close_scope( object->scope );
                }
expr_nolbr(x)   ::= newfunc_lbr(func) stmt_list RBR .
                {
                    x = func;
                    p->close_scope( func->scope );
                }

newobj_lbr(x)   ::= COLON(token) LBR .
                {
                    x = p->object( token->sloc, nullptr, nullptr );
                    p->destroy( token );
                }
newobj_lbr(x)   ::= COLON(token) expr_simple(proto) LBR .
                {
                    proto = p->resolve( proto );
                    x = p->object( token->sloc, nullptr, proto );
                    p->destroy( token );
                }

newfunc_lbr(x)  ::= QMARK(token) expr_paren(params) LBR .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, false, false );
                    p->destroy( token );
                }
newfunc_lbr(x)  ::= PERIOD(token) QMARK expr_paren(params) LBR .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, false, true );
                    p->destroy( token );
                }
newfunc_lbr(x)  ::= QMARK(token) expr_paren(params) YIELD LBR .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, true, false );
                    p->destroy( token );
                }
newfunc_lbr(x)  ::= PERIOD(token) QMARK expr_paren(params) YIELD LBR .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, true, true );
                    p->destroy( token );
                }


// All single-value expressions, including those starting with an open brace.
expr_value(x)   ::= expr_nolbr(expr) .
                {
                    x = expr;
                }
expr_value(x)   ::= LBR(token) RBR .
                {
                    x = p->alloc< xec_new_table >( token->sloc );
                    p->destroy( token );
                }
expr_value(x)   ::= LBR(token) keyval_list(table) RBR .
                {
                    x = table;
                    x->sloc = token->sloc;
                    p->destroy( token );
                }

expr_lbody(x)   ::= expr_value(expr) .
                {
                    x = expr;
                }
expr_lbody(x)   ::= expr_lbody(list) COMMA expr_value(expr) .
                {
                    // Do not resolve.
                    x = p->append( list, expr );
                }

// Unpack expressions are valid only as the last expression in a list.
expr_final(x)   ::= ELLIPSIS(token) .
                {
                    x = p->alloc< xec_expr_vararg >( token->sloc );
                    p->destroy( token );
                }
expr_final(x)   ::= proto(expr) ELLIPSIS .
                {
                    xec_expr_call* call = p->resolve( expr );
                    call->unpack = true;
                    x = call;
                }
expr_final(x)   ::= expr_call(expr) ELLIPSIS .
                {
                    expr->unpack = true;
                    x = expr;
                }
expr_final(x)   ::= expr_yield(expr) ELLIPSIS .
                {
                    expr->unpack = true;
                    x = expr;
                }
expr_final(x)   ::= proto(expr) YIELD ELLIPSIS .
                {
                    xec_expr_call* call = p->resolve( expr );
                    call->yieldcall = true;
                    call->unpack = true;
                    x = call;
                }
expr_final(x)   ::= expr_call(expr) YIELD ELLIPSIS .
                {
                    expr->yieldcall = true;
                    expr->unpack = true;
                    x = expr;
                }
expr_final(x)   ::= name(expr) LSQ RSQ ELLIPSIS .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }
expr_final(x)   ::= proto(expr) LSQ RSQ ELLIPSIS .
                {
                    xec_expr_call* call = p->resolve( expr );
                    x = p->alloc< xec_expr_unpack >( call->sloc, call );
                }
expr_final(x)   ::= expr_index(expr) LSQ RSQ ELLIPSIS .
                {
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }
expr_final(x)   ::= expr_yield(expr) LSQ RSQ ELLIPSIS .
                {
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }
expr_final(x)   ::= expr_new(expr) LSQ RSQ ELLIPSIS .
                {
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }
expr_final(x)   ::= expr_call(expr) LSQ RSQ ELLIPSIS .
                {
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }
expr_final(x)   ::= expr_postfix(expr) LSQ RSQ ELLIPSIS .
                {
                    x = p->alloc< xec_expr_unpack >( expr->sloc, expr );
                }

expr_list(x)    ::= expr_final(expr) .
                {
                    x = expr;
                }
expr_list(x)    ::= expr_lbody(expr) .
                {
                    x = expr;
                }
expr_list(x)    ::= expr_lbody(list) COMMA expr_final(expr) .
                {
                    x = p->final( list, expr );
                }

expr_assign(x)  ::= expr_list(expr) .
                {
                    expr = p->resolve( expr );
                    x = expr;
                }
expr_assign(x)  ::= expr_lbody(lv) assign_op(op) expr_assign(rv) .
                {
                    lv = p->resolve( lv );
                    rv = p->resolve( rv );
                    x = p->assign( op, lv, rv );
                    p->destroy( op );
                }

// Assignment operators.
assign_op(x)    ::= ASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= MULASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= DIVASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= MODASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= INTDIVASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= ADDASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= SUBASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= LSHIFTASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= RSHIFTASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= URSHIFTASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= BITANDASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= BITXORASSIGN(token) .
                {
                    x = token;
                }
assign_op(x)    ::= BITORASSIGN(token) .
                {
                    x = token;
                }

// Non-empty lists for list [ ... ] constructors.
value_lbody(x)  ::= expr_value(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_new_list >( -1 );
                    x->values.push_back( expr );
                }
value_lbody(x)  ::= value_lbody(list) COMMA expr_value(expr) .
                {
                    expr = p->resolve( expr );
                    x = list;
                    x->values.push_back( expr );
                }

value_list(x)   ::= expr_final(expr) .
                {
                    x = p->alloc< xec_new_list >( -1 );
                    x->unpack = expr;
                }
value_list(x)   ::= expr_final(expr) COMMA .
                {
                    x = p->alloc< xec_new_list >( -1 );
                    x->unpack = expr;
                }
value_list(x)   ::= value_lbody(list) .
                {
                    x = list;
                }
value_list(x)   ::= value_lbody(list) COMMA .
                {
                    x = list;
                }
value_list(x)   ::= value_lbody(list) COMMA expr_final(expr) .
                {
                    x = list;
                    x->unpack = expr;
                }

// Non-empty key -> value lists for { ... } constructors.
keyval_lbody(x) ::= expr_value(key) COLON expr_value(value) .
                {
                    key   = p->resolve( key );
                    value = p->resolve( value );
                    x = new xec_new_table( -1 );
                    x->elements.emplace_back( key, value );
                }
keyval_lbody(x) ::= keyval_lbody(table) COMMA
                                expr_value(key) COLON expr_value(value) .
                {
                    key   = p->resolve( key );
                    value = p->resolve( value );
                    x = table;
                    x->elements.emplace_back( key, value );
                }

keyval_list(x)  ::= keyval_lbody(table) .
                {
                    x = table;
                }
keyval_list(x)  ::= keyval_lbody(table) COMMA .
                {
                    x = table;
                }



//
// expressions excluding { in the initial position
//


sexpr_lbody(x)  ::= expr_nolbr(expr) .
                {
                    expr = p->resolve( expr );
                    x = expr;
                }
sexpr_lbody(x)  ::= sexpr_lbody(list) COMMA expr_value(expr) .
                {
                    expr = p->resolve( expr );
                    x = p->append( list, expr );
                }

sexpr_list(x)   ::= expr_final(expr) .
                {
                    x = expr;
                }
sexpr_list(x)   ::= sexpr_lbody(expr) .
                {
                    x = expr;
                }
sexpr_list(x)   ::= sexpr_lbody(list) COMMA expr_final(expr) .
                {
                    x = p->final( list, expr );
                }

sexpr_assign(x) ::= sexpr_list(expr) .
                {
                    x = expr;
                }
sexpr_assign(x) ::= sexpr_lbody(lv) assign_op(op) expr_assign(rv) .
                {
                    rv = p->resolve( rv );
                    x = p->assign( op, lv, rv );
                    p->destroy( op );
                }




//
// statements
//



// Conditions.
condition(x)    ::= expr_assign(expr) .
                {
                    x = expr;
                }
condition(x)    ::= VAR(token) varname(varname) ASSIGN expr_list(rval) .
                {
                    rval = p->resolve( rval );
                    xec_expr_assign* assign = p->alloc< xec_expr_assign >(
                                    token->sloc, XEC_TOKEN_ASSIGN );
                    assign->lvalue = p->declare_local( varname );
                    assign->rvalue = rval;
                    x = assign;
                    p->destroy( token );
                }
condition(x)    ::= VAR(token) varname_list(varnames) ASSIGN expr_list(rvals) .
                {
                    rvals = p->resolve( rvals );
                    xec_expr_assign_list* assign =
                            p->alloc< xec_expr_assign_list >(
                                    token->sloc, XEC_TOKEN_ASSIGN );
                    p->declare_local_list( varnames, &assign->lvalues );
                    assign->rvalues = rvals;
                    x = assign;
                    p->destroy( token );
                }




// Statement lists.
stmt_list       ::= .
                {
                }
stmt_list       ::= stmt_list SEMICOLON .
                {
                }
stmt_list       ::= stmt_list stmt(stmt) .
                {
                    p->statement( stmt );
                }
stmt_list       ::= stmt_list decl_object .
                {
                }
stmt_list       ::= stmt_list decl_func .
                {
                }
stmt_list       ::= stmt_list decl_var SEMICOLON .
                {
                }
stmt_list       ::= stmt_list decl_noinit SEMICOLON .
                {
                }


// Normally block statements open a new scope scopes.
stmt(x)         ::= stmt_lbr(stmt) stmt_list RBR .
                {
                    x = stmt;
                    p->close_scope( stmt->scope );
                }
stmt(x)         ::= stmt_common(stmt) .
                {
                    x = stmt;
                }

stmt_lbr(x)     ::= LBR(token) .
                {
                    x = p->alloc< xec_stmt_block >( token->sloc );
                    x->scope = p->block_scope( x );
                    x->scope->block = x;
                    p->destroy( token );
                }


// Compound statements which are loop or switch bodies reuse the scope.
stmt_reuse(x)   ::= stmt_lbr_ru(stmt) stmt_list RBR .
                {
                    x = stmt;
                }
stmt_reuse(x)   ::= stmt_common(stmt) .
                {
                    x = stmt;
                }

stmt_lbr_ru(x)  ::= LBR(token) .
                {
                    x = p->alloc< xec_stmt_block >( token->sloc );
                    x->scope = p->get_scope();
                    x->scope->block = x;
                    p->destroy( token );
                }



// Statements.
stmt_common(x)  ::= sexpr_assign(expr) SEMICOLON .
                {
                    x = expr;
                }
stmt_common(x)  ::= stmt_if(stmt) LPN condition(expr) RPN stmt(block) .
                {
                    stmt->condition = expr;
                    stmt->iftrue    = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_if(stmt) LPN condition(expr) RPN stmt(block)
                                ELSE stmt(orblock) .
                {
                    stmt->condition = expr;
                    stmt->iftrue    = block;
                    stmt->iffalse   = orblock;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_switch(stmt) LPN condition(expr) RPN
                                stmt_lbr_ru(block) stmt_list RPN .
                {
                    stmt->value     = expr;
                    stmt->body      = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_while(stmt) LPN condition(expr) RPN
                                stmt_reuse(block) .
                {
                    stmt->condition = expr;
                    stmt->body      = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_do(stmt) stmt_reuse(block) WHILE
                                LPN expr_assign(expr) RPN SEMICOLON .
                {
                    stmt->body      = block;
                    stmt->condition = expr;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_foreach(stmt) stmt_reuse(block) .
                {
                    stmt->body      = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_for(stmt) stmt_reuse(block) .
                {
                    stmt->body      = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_using(stmt) LPN condition(expr) RPN
                                stmt_reuse(block) .
                {
                    stmt->uvalue    = expr;
                    stmt->body      = block;
                    p->close_scope( stmt->scope );
                    x = stmt;
                }
stmt_common(x)  ::= stmt_try(stmt) .
                {
                    x = stmt;
                }
stmt_common(x)  ::= DELETE(token) expr_lbody(expr) SEMICOLON .
                {
                    xec_stmt_delete* s;
                    x = s = p->alloc< xec_stmt_delete >( token->sloc );
                    p->delval_list( expr, &s->delvals );
                    p->destroy( token );
                }
stmt_common(x)  ::= CASE(token) expr_value(expr) COLON .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_case >( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= DEFAULT(token) COLON .
                {
                    x = p->alloc< xec_stmt_case >( token->sloc, nullptr );
                    p->destroy( token );
                }
stmt_common(x)  ::= CONTINUE(token) SEMICOLON .
                {
                    xec_ast_node* target = p->continue_target( token->sloc );
                    x = p->alloc< xec_stmt_continue >( token->sloc, target );
                    p->destroy( token );
                }
stmt_common(x)  ::= BREAK(token) SEMICOLON .
                {
                    xec_ast_node* target = p->break_target( token->sloc );
                    x = p->alloc< xec_stmt_break >( token->sloc, target );
                    p->destroy( token );
                }
stmt_common(x)  ::= RETURN(token) SEMICOLON .
                {
                    x = p->alloc< xec_stmt_return >( token->sloc, nullptr );
                    p->destroy( token );
                }
stmt_common(x)  ::= RETURN(token) expr_list(expr) SEMICOLON .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_return >( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= token_yield(token) SEMICOLON .
                {
                    x = p->alloc< xec_expr_yield >( token->sloc, nullptr );
                    p->destroy( token );
                }
stmt_common(x)  ::= token_yield(token) expr_list(expr) SEMICOLON .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_expr_yield >( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= THROW(token) expr_value(expr) SEMICOLON .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_throw >( token->sloc, expr );
                    p->destroy( token );
                }



// Statements with scopes.
stmt_if(x)      ::= IF(token) .
                {
                    x = p->alloc< xec_stmt_if >( token->sloc );
                    x->scope = p->block_scope( x );
                    p->destroy( token );
                }

stmt_switch(x)  ::= SWITCH(token) .
                {
                    x = p->alloc< xec_stmt_switch >( token->sloc );
                    x->scope = p->block_scope( x );
                    p->destroy( token );
                }

stmt_while(x)   ::= WHILE(token) .
                {
                    x = p->alloc< xec_stmt_while >( token->sloc );
                    x->scope = p->block_scope( x );
                    p->destroy( token );
                }

stmt_do(x)      ::= DO(token) .
                {
                    x = p->alloc< xec_stmt_do >( token->sloc );
                    x->scope = p->block_scope( x );
                    p->destroy( token );
                }

scope_for(x)    ::= FOR(token) .
                {
                    // open scope before we know the kind of statement.
                    x = make_token_scope( token, p->block_scope( nullptr ) );
                }

stmt_foreach(x) ::= scope_for(forscope) LPN
                        expr_lbody(lval) COLON expr_value(expr) RPN .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_foreach >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope    = forscope.scope;
                    p->lvalue_list( lval, &x->lvalues );
                    x->list     = expr;
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN
                        expr_lbody(lval) EACHKEY expr_value(expr) RPN .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_foreach >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope    = forscope.scope;
                    p->lvalue_list( lval, &x->lvalues );
                    x->list     = expr;
                    x->eachkey  = true;
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN
                        VAR varname_list(varnames) COLON expr_value(expr) RPN .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_foreach >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope    = forscope.scope;
                    p->declare_local_list( varnames, &x->lvalues );
                    x->list     = expr;
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN VAR varname_list(varnames)
                                EACHKEY expr_value(expr) RPN .
                {
                    expr = p->resolve( expr );
                    x = p->alloc< xec_stmt_foreach >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope    = forscope.scope;
                    p->declare_local_list( varnames, &x->lvalues );
                    x->list     = expr;
                    x->eachkey  = true;
                    p->destroy( forscope.token );
                }

stmt_for(x)     ::= scope_for(forscope) LPN
                        condition(einit) SEMICOLON
                            expr_assign(expr) SEMICOLON
                                expr_assign(eupdate) RPN .
                {
                    x = p->alloc< xec_stmt_for >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope        = forscope.scope;
                    x->init         = einit;
                    x->condition    = expr;
                    x->update       = eupdate;
                    p->destroy( forscope.token );
                }

stmt_using(x)   ::= USING(token) .
                {
                    x = p->alloc< xec_stmt_using >( token->sloc );
                    x->scope = p->block_scope( x );
                    p->destroy( token );
                }



// Try statement.
stmt_try(x)     ::= try_catch(stmt) . [TRY]
                {
                    x = stmt;
                }
stmt_try(x)     ::= try_catch(stmt) FINALLY stmt(block) . [TRY]
                {
                    stmt->fstmt = block;
                    x = stmt;
                }
stmt_try(x)     ::= try_block(stmt) FINALLY stmt(block) . [TRY]
                {
                    stmt->fstmt = block;
                    x = stmt;
                }

try_block(x)    ::= TRY(token) stmt(tstmt) .
                {
                    x = p->alloc< xec_stmt_try >( token->sloc, tstmt );
                    p->destroy( token );
                }

try_catch(x)    ::= try_block(stmt) stmt_catch(cstmt) stmt_reuse(block) .
                {
                    cstmt->body = block;
                    p->close_scope( cstmt->scope );
                    stmt->clist.push_back( cstmt );
                    x = stmt;
                }
try_catch(x)    ::= try_catch(stmt) stmt_catch(cstmt) stmt_reuse(block) .
                {
                    cstmt->body = block;
                    p->close_scope( cstmt->scope );
                    stmt->clist.push_back( cstmt );
                    x = stmt;
                }

scope_catch(x)  ::= CATCH(token) .
                {
                    // open scope here ready for declarations.
                    x = make_token_scope( token, p->block_scope( nullptr ) );
                }

stmt_catch(x)   ::= scope_catch(cscope) LPN COLON expr_simple(cproto) RPN .
                {
                    cproto = p->resolve( cproto );
                    x = p->alloc< xec_stmt_catch >( cscope.token->sloc );
                    cscope.scope->node = x;
                    x->proto    = cproto;
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN
                        expr_value(lval) COLON expr_simple(cproto) RPN .
                {
                    lval = p->resolve( lval );
                    cproto = p->resolve( cproto );
                    x = p->alloc< xec_stmt_catch >( cscope.token->sloc );
                    cscope.scope->node = x;
                    x->lvalue   = p->lvalue( lval );
                    x->proto    = cproto;
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN
                        VAR varname(varname) COLON expr_simple(cproto) RPN .
                {
                    cproto = p->resolve( cproto );
                    x = p->alloc< xec_stmt_catch >( cscope.token->sloc );
                    cscope.scope->node = x;
                    x->lvalue   = p->declare_local( varname );
                    x->proto    = cproto;
                    p->destroy( cscope.token );
                }






//
// deal with grammar conflicts
//

token_yield(x)  ::= YIELD(token) .
                {
                    x = token;
                }








%syntax_error
{
    p->get_script()->diagnostic( TOKEN->sloc,
            "unexpected %s", TOKEN->get_spelling().c_str() );
}










