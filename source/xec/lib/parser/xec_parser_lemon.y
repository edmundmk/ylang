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
    xec_token*          token;
    xec_ast_scope*      scope;
};

inline xec_token_scope make_token_scope(
                xec_token* token, xec_ast_scope* scope )
{
    xec_token_scope result;
    result.token = token;
    result.scope = scope;
    return result;
}


struct xec_token_op
{
    xec_token*          token;
    xec_operator_kind   op;
};

inline xec_token_op make_token_op(
                xec_token* token, xec_operator_kind op )
{
    xec_token_op result;
    result.token = token;
    result.op = op;
    return result;
}


}


%extra_argument     { xec_parser* p }
%token_type         { xec_token* }
%default_type       { xec_ast_node* }

%type name          { xec_name_name* }

%type param_list    { xec_name_list* }
%type arg_list      { xec_expr_list* }

%type object_scope  { xec_new_object* }
%type newobj_scope  { xec_new_object* }

%type func_scope    { xec_ast_func* }
%type newf_scope    { xec_ast_func* }

%type compare_op    { xec_token_op }
%type assign_op     { xec_token_op }
%type value_lbody   { xec_new_array* }
%type value_list    { xec_new_array* }
%type keyval_lbody  { xec_new_table* }
%type keyval_list   { xec_new_table* }

%type stmt_block    { xec_stmt_block* }
%type stmt_reblock  { xec_stmt_block* }
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





//
// Script.
//

script          ::= stmt_list .
                {
                }




//
// Names.
//

name(x)         ::= IDENTIFIER(token) .
                {
                    x = p->alloc< xec_name_name >( token->sloc, token->text );
                    p->destroy( token );
                }
name(x)         ::= FUNCTION(token) .
                {
                    x = p->alloc< xec_name_name >( token->sloc, token->text );
                    p->destroy( token );
                }
name(x)         ::= OBJECT(token) .
                {
                    x = p->alloc< xec_name_name >( token->sloc, token->text );
                    p->destroy( token );
                }


name_list(x)    ::= name(name) .
                {
                    x = name;
                }
name_list(x)    ::= name_list(list) COMMA name(name) .
                {
                    xec_name_list* l;
                    x = l = p->name_list( list );
                    l->names.push_back( name );
                }


param_list(x)   ::= .
                {
                    x = nullptr;
                }
param_list(x)   ::= name_list(list) .
                {
                    x = p->name_list( list );
                }
param_list(x)   ::= ELLIPSIS(token) .
                {
                    xec_name_list* l;
                    x = l = p->alloc< xec_name_list >( token->sloc );
                    l->varargs = true;
                }
param_list(x)   ::= name_list(list) COMMA ELLIPSIS .
                {
                    xec_name_list* l;
                    x = l = p->name_list( list );
                    l->varargs = true;
                }


qual_name(x)    ::= name(name) .
                {
                    x = name;
                }
qual_name(x)    ::= qual_name(name) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_name_qual >(
                                    token->sloc, name, token->text );
                    p->destroy( token );
                }





//
// Declarations.
//


decl_object     ::= object_scope(object) LBR object_decls RBR .
                {
                    p->close_scope( object->scope );
                }

object_scope(x) ::= OBJECT qual_name(name) .
                {
                    x = p->object( name->sloc, name, nullptr );
                }
object_scope(x) ::= OBJECT qual_name(name) COLON expr_simple(proto) .
                {
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
object_decl     ::= decl_proto .
                {
                }
object_decl     ::= decl_func .
                {
                }
object_decl     ::= decl_var .
                {
                }


decl_proto      ::= FUNCTION(token) qual_name(name)
                                LPN param_list(params) RPN SEMICOLON .
                {
                    p->prototype( token->sloc, name, params, false );
                    p->destroy( token );
                }
decl_proto      ::= FUNCTION(token) qual_name(name)
                                LPN param_list(params) RPN YIELD SEMICOLON .
                {
                    p->prototype( token->sloc, name, params, true );
                    p->destroy( token );
                }


decl_func       ::= func_scope(function) LBR stmt_list RBR .
                {
                    p->close_scope( function->scope );
                }

func_scope(x)   ::= FUNCTION(token) qual_name(name)
                                LPN param_list(params) RPN .
                {
                    x = p->function( token->sloc, name, params, false, false );
                    p->destroy( token );
                }
func_scope(x)   ::= FUNCTION(token) qual_name(name)
                                LPN param_list(params) RPN YIELD .
                {
                    x = p->function( token->sloc, name, params, true, false );
                    p->destroy( token );
                }


decl_var        ::= VAR(token) name_list(names) SEMICOLON .
                {
                    p->var( token->sloc, names, nullptr );
                    p->destroy( token );
                }
decl_var        ::= VAR(token) name_list(names)
                                ASSIGN expr_list(rvals) SEMICOLON .
                {
                    p->var( token->sloc, names, rvals );
                    p->destroy( token );
                }








//
// expressions
//

expr_index(x)   ::= LPN expr_assign(expr) RPN .
                {
                    x = p->mono( expr );
                }
expr_index(x)   ::= IDENTIFIER(token) .
                {
                    x = p->lookup( token->sloc, token->text, true );
                    p->destroy( token );
                }
expr_index(x)   ::= FUNCTION(token) .
                {
                    x = p->lookup( token->sloc, token->text, true );
                    p->destroy( token );
                }
expr_index(x)   ::= OBJECT(token) .
                {
                    x = p->lookup( token->sloc, token->text, true );
                    p->destroy( token );
                }
expr_index(x)   ::= expr_index(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >( expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_index(x)   ::= expr_index(expr) PERIOD LSQ expr_value(key) RSQ .
                {
                    x = p->alloc< xec_expr_inkey >( expr->sloc, expr, key );
                }
expr_index(x)   ::= expr_index(expr) LSQ expr_value(index) RSQ .
                {
                    x = p->alloc< xec_expr_index >( expr->sloc, expr, index );
                }

expr_yield(x)   ::= YIELD(token) LPN /* conflict */ arg_list(args) RPN .
                {
                    p->check_yield( token->sloc );
                    x = p->alloc< xec_expr_yield >( token->sloc, args );
                    p->destroy( token );
                }

expr_new(x)     ::= NEW(token) expr_index(proto) LPN arg_list(args) RPN .
                {
                    x = p->alloc< xec_new_new >( token->sloc, proto, args );
                    p->destroy( token );
                }

// With at least one call or call-like expression
expr_postfix(x) ::= expr_yield(expr) .
                {
                    x = expr;
                }
expr_postfix(x) ::= expr_new(expr) .
                {
                    x = expr;
                }
expr_postfix(x) ::= expr_index(expr) LPN arg_list(args) RPN .
                {
                    x = p->alloc< xec_expr_call >(
                                expr->sloc, expr, args, false );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD IDENTIFIER(token) .
                {
                    x = p->alloc< xec_expr_key >(
                                expr->sloc, expr, token->text );
                    p->destroy( token );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = p->alloc< xec_expr_inkey >(
                                expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) LSQ expr_value(index) RSQ .
                {
                    x = p->alloc< xec_expr_index >(
                                expr->sloc, expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) LPN arg_list(args) RPN .
                {
                    x = p->alloc< xec_expr_call >(
                                expr->sloc, expr, args, false );
                }


// All index, call, and call-like expressions, including yield calls.  Used
// to specify prototypes, as it makes little sense to inherit from the result
// of an arithmetic expression, or from a literal.  Yield calls are at this
// precedence level because it is confusing to allow further postfix -
// a() yield.b - or call - a() yield() - expressions after the yield keyword.
expr_simple(x)  ::= expr_index(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_postfix(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= expr_index(expr) LPN arg_list(args) RPN YIELD .
                {
                    p->check_yield( expr->sloc );
                    x = p->alloc< xec_expr_call >(
                                expr->sloc, expr, args, true );
                }
expr_simple(x)  ::= expr_postfix(expr) LPN arg_list(args) RPN YIELD .
                {
                    p->check_yield( expr->sloc );
                    x = p->alloc< xec_expr_call >(
                                expr->sloc, expr, args, true );
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
                    x = p->alloc< xec_expr_postop >(
                             expr->sloc, XEC_OPERATOR_POSTINC, expr );
                    p->destroy( token );
                }
expr_suffix(x)  ::= expr_suffix(expr) DECREMENT(token) .
                {
                    x = p->alloc< xec_expr_postop >(
                             expr->sloc, XEC_OPERATOR_POSTDEC, expr );
                    p->destroy( token );
                }

expr_unary(x)   ::= expr_suffix(expr) .
                {
                    x = expr;
                }
expr_unary(x)   ::= PLUS(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, XEC_OPERATOR_POSITIVE, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= MINUS(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, XEC_OPERATOR_NEGATIVE, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= XMARK(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, XEC_OPERATOR_LOGICNOT, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= TILDE(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_unary >(
                             token->sloc, XEC_OPERATOR_BITNOT, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= INCREMENT(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_preop >(
                             token->sloc, XEC_OPERATOR_PREINC, expr );
                    p->destroy( token );
                }
expr_unary(x)   ::= DECREMENT(token) expr_unary(expr) .
                {
                    x = p->alloc< xec_expr_preop >(
                             token->sloc, XEC_OPERATOR_PREDEC, expr );
                    p->destroy( token );
                }

expr_mul(x)     ::= expr_unary(expr) .
                {
                    x = expr;
                }
expr_mul(x)     ::= expr_mul(lhs) ASTERISK(token) expr_unary(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_MULTIPLY, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) SOLIDUS(token) expr_unary(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_DIVIDE, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) PERCENT(token) expr_unary(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_MODULUS, lhs, rhs );
                    p->destroy( token );
                }
expr_mul(x)     ::= expr_mul(lhs) TILDE(token) expr_unary(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_INTDIV, lhs, rhs );
                    p->destroy( token );
                }

expr_add(x)     ::= expr_mul(expr) .
                {
                    x = expr;
                }
expr_add(x)     ::= expr_add(lhs) PLUS(token) expr_mul(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_ADD, lhs, rhs );
                    p->destroy( token );
                }
expr_add(x)     ::= expr_add(lhs) MINUS(token) expr_mul(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_SUBTRACT, lhs, rhs );
                    p->destroy( token );
                }

expr_shift(x)   ::= expr_add(expr) .
                {
                    x = expr;
                }
expr_shift(x)   ::= expr_shift(lhs) LSHIFT(token) expr_add(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_LSHIFT, lhs, rhs );
                    p->destroy( token );
                }
expr_shift(x)   ::= expr_shift(lhs) LRSHIFT(token) expr_add(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_LRSHIFT, lhs, rhs );
                    p->destroy( token );
                }
expr_shift(x)   ::= expr_shift(lhs) ARSHIFT(token) expr_add(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_ARSHIFT, lhs, rhs );
                    p->destroy( token );
                }

expr_bitand(x)  ::= expr_shift(expr) .
                {
                    x = expr;
                }
expr_bitand(x)  ::= expr_bitand(lhs) AMPERSAND(token) expr_shift(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_BITAND, lhs, rhs );
                    p->destroy( token );
                }

expr_bitxor(x)  ::= expr_bitand(expr) .
                {
                    x = expr;
                }
expr_bitxor(x)  ::= expr_bitxor(lhs) CARET(token) expr_bitand(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_BITXOR, lhs, rhs );
                    p->destroy( token );
                }

expr_bitor(x)   ::= expr_bitxor(expr) .
                {
                    x = expr;
                }
expr_bitor(x)   ::= expr_bitor(lhs) VBAR(token) expr_bitxor(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_BITOR, lhs, rhs );
                    p->destroy( token );
                }

expr_concat(x)  ::= expr_bitor(expr) .
                {
                    x = expr;
                }
expr_concat(x)  ::= expr_concat(lhs) CONCATENATE(token) expr_bitor(rhs) .
                {
                    x = p->alloc< xec_expr_binary >( lhs->sloc,
                                    XEC_OPERATOR_CONCATENATE, lhs, rhs );
                    p->destroy( token );
                }

expr_compare(x) ::= expr_concat(expr) .
                {
                    x = expr;
                }
expr_compare(x) ::= expr_compare(lhs) compare_op(tokenop) expr_concat(rhs) .
                {
                    x = p->compare( tokenop.token->sloc, tokenop.op, lhs, rhs );
                    p->destroy( tokenop.token );
                }

compare_op(x)   ::= EQUAL(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_EQUAL );
                }
compare_op(x)   ::= NOTEQUAL(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_NOTEQUAL );
                }
compare_op(x)   ::= LESS(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_LESS );
                }
compare_op(x)   ::= GREATER(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_GREATER );
                }
compare_op(x)   ::= LESSEQUAL(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_LESSEQUAL );
                }
compare_op(x)   ::= GREATEREQUAL(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_GREATEREQUAL );
                }
compare_op(x)   ::= IN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_IN );
                }
compare_op(x)   ::= NOTIN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_NOTIN );
                }
compare_op(x)   ::= IS(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_IS );
                }
compare_op(x)   ::= NOTIS(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_NOTIS );
                }


expr_and(x)     ::= expr_compare(expr) .
                {
                    x = expr;
                }
expr_and(x)     ::= expr_and(lhs) LOGICAND(token) expr_compare(rhs) .
                {
                    x = p->alloc< xec_expr_logical >( lhs->sloc,
                                    XEC_OPERATOR_LOGICAND, lhs, rhs );
                    p->destroy( token );
                }

expr_xor(x)     ::= expr_and(expr) .
                {
                    x = expr;
                }
expr_xor(x)     ::= expr_xor(lhs) LOGICXOR(token) expr_and(rhs) .
                {
                    x = p->alloc< xec_expr_logical >( lhs->sloc,
                                    XEC_OPERATOR_LOGICXOR, lhs, rhs );
                    p->destroy( token );
                }

expr_or(x)      ::= expr_xor(expr) .
                {
                    x = expr;
                }
expr_or(x)      ::= expr_or(lhs) LOGICOR(token) expr_xor(rhs) .
                {
                    x = p->alloc< xec_expr_logical >( lhs->sloc,
                                    XEC_OPERATOR_LOGICOR, lhs, rhs );
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
                    x = p->alloc< xec_expr_qmark >(
                            condition->sloc, condition, iftrue, iffalse );
                }
expr_nolbr(x)   ::= LSQ(token) RSQ .
                {
                    x = p->alloc< xec_new_array >( token->sloc );
                    p->destroy( token );
                }
expr_nolbr(x)   ::= LSQ(token) value_list(expr) RSQ .
                {
                    x = expr;
                    x->sloc = token->sloc;
                    p->destroy( token );
                }
expr_nolbr(x)   ::= newobj_scope(object) LBR object_decls RBR .
                {
                    x = object;
                    p->close_scope( object->scope );
                }
expr_nolbr(x)   ::= newf_scope(function) LBR stmt_list RBR .
                {
                    x = function;
                    p->close_scope( function->scope );
                }

newobj_scope(x) ::= COLON(token) .
                {
                    x = p->object( token->sloc, nullptr, nullptr );
                    p->destroy( token );
                }
newobj_scope(x) ::= COLON(token) COLON expr_simple(proto) .
                {
                    x = p->object( token->sloc, nullptr, proto );
                    p->destroy( token );
                }

newf_scope(x)   ::= QMARK(token) LPN param_list(params) RPN .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, false, false );
                    p->destroy( token );
                }
newf_scope(x)   ::= PERIOD(token) QMARK LPN param_list(params) RPN .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, false, true );
                    p->destroy( token );
                }
newf_scope(x)   ::= QMARK(token) LPN param_list(params) RPN YIELD .
                {
                    x = p->function( token->sloc,
                                    nullptr, params, true, false );
                    p->destroy( token );
                }
newf_scope(x)   ::= PERIOD(token) QMARK LPN param_list(params) RPN YIELD .
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
                    xec_expr_list* l;
                    x = l = p->expr_list( list );
                    l->values.push_back( expr );
                }

// Unpack expressions are valid only as the last expression in a list.
expr_final(x)   ::= ELLIPSIS(token) .
                {
                    p->check_vararg( token->sloc );
                    x = p->alloc< xec_expr_vararg >( token->sloc );
                    p->destroy( token );
                }
expr_final(x)   ::= expr_postfix(expr) ELLIPSIS .
                {
                    x = p->unpack( expr );
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
                    xec_expr_list* l;
                    x = l = p->expr_list( list );
                    l->final = expr;
                }

arg_list(x)     ::= .
                {
                    x = nullptr;
                }
arg_list(x)     ::= expr_list(list) .
                {
                    x = p->expr_list( list );
                }

expr_assign(x)  ::= expr_list(expr) .
                {
                    x = expr;
                }
expr_assign(x)  ::= expr_lbody(lv) assign_op(tokenop) expr_assign(rv) .
                {
                    x = p->assign( tokenop.token->sloc, tokenop.op, lv, rv );
                    p->destroy( tokenop.token );
                }

// Assignment operators.
assign_op(x)    ::= ASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_ASSIGN );
                }
assign_op(x)    ::= MULASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_MULASSIGN );
                }
assign_op(x)    ::= DIVASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_DIVASSIGN );
                }
assign_op(x)    ::= MODASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_MODASSIGN );
                }
assign_op(x)    ::= INTDIVASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_INTDIVASSIGN );
                }
assign_op(x)    ::= ADDASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_ADDASSIGN );
                }
assign_op(x)    ::= SUBASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_SUBASSIGN );
                }
assign_op(x)    ::= LSHIFTASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_LSHIFTASSIGN );
                }
assign_op(x)    ::= LRSHIFTASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_LRSHIFTASSIGN );
                }
assign_op(x)    ::= ARSHIFTASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_ARSHIFTASSIGN );
                }
assign_op(x)    ::= BITANDASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_BITANDASSIGN );
                }
assign_op(x)    ::= BITXORASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_BITXORASSIGN );
                }
assign_op(x)    ::= BITORASSIGN(token) .
                {
                    x = make_token_op( token, XEC_OPERATOR_BITORASSIGN );
                }

// Non-empty lists for list [ ... ] constructors.
value_lbody(x)  ::= expr_value(expr) .
                {
                    x = p->alloc< xec_new_array >( -1 );
                    x->values.push_back( expr );
                }
value_lbody(x)  ::= value_lbody(list) COMMA expr_value(expr) .
                {
                    x = list;
                    x->values.push_back( expr );
                }

value_list(x)   ::= expr_final(expr) .
                {
                    x = p->alloc< xec_new_array >( -1 );
                    x->final = expr;
                }
value_list(x)   ::= expr_final(expr) COMMA .
                {
                    x = p->alloc< xec_new_array >( -1 );
                    x->final = expr;
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
                    x->final = expr;
                }

// Non-empty key -> value lists for { ... } constructors.
keyval_lbody(x) ::= expr_value(key) COLON expr_value(value) .
                {
                    x = new xec_new_table( -1 );
                    x->elements.emplace_back( key, value );
                }
keyval_lbody(x) ::= keyval_lbody(table) COMMA
                                expr_value(key) COLON expr_value(value) .
                {
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
                    x = expr;
                }
sexpr_lbody(x)  ::= sexpr_lbody(list) COMMA expr_value(expr) .
                {
                    xec_expr_list* l;
                    x = l = p->expr_list( list );
                    l->values.push_back( expr );
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
                    xec_expr_list* l;
                    x = l = p->expr_list( list );
                    l->final = expr;
                }

sexpr_assign(x) ::= sexpr_list(expr) .
                {
                    x = expr;
                }
sexpr_assign(x) ::= sexpr_lbody(lv) assign_op(tokenop) expr_assign(rv) .
                {
                    x = p->assign( tokenop.token->sloc, tokenop.op, lv, rv );
                    p->destroy( tokenop.token );
                }




//
// statements
//



// Conditions.
condition(x)    ::= expr_assign(expr) .
                {
                    x = expr;
                }
condition(x)    ::= VAR(token) name_list(names) ASSIGN expr_list(rvals) .
                {
                    x = p->varstmt( token->sloc, names, rvals );
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
stmt_list       ::= stmt_list decl_var .
                {
                }


// Normally block statements open a new scope scopes.
stmt(x)         ::= stmt_block(stmt) stmt_list RBR .
                {
                    x = stmt;
                    p->close_scope( stmt->scope );
                }
stmt(x)         ::= stmt_common(stmt) .
                {
                    x = stmt;
                }

stmt_block(x)   ::= LBR(token) .
                {
                    x = p->alloc< xec_stmt_block >( token->sloc );
                    x->scope = p->block_scope( x );
                    x->scope->block = x;
                    p->destroy( token );
                }


// Compound statements which are loop or switch bodies reuse the scope.
stmt_reuse(x)   ::= stmt_reblock(stmt) stmt_list RBR .
                {
                    x = stmt;
                }
stmt_reuse(x)   ::= stmt_common(stmt) .
                {
                    x = stmt;
                }

stmt_reblock(x) ::= LBR(token) .
                {
                    x = p->alloc< xec_stmt_block >( token->sloc );
                    p->get_scope()->block = x;
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
                                stmt_reblock(block) stmt_list RPN .
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
                    x = p->delstmt( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= CASE(token) expr_value(expr) COLON .
                {
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
                    x = p->alloc< xec_stmt_return >( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= token_yield(token) SEMICOLON .
                {
                    p->check_yield( token->sloc );
                    x = p->alloc< xec_expr_yield >( token->sloc, nullptr );
                    p->destroy( token );
                }
stmt_common(x)  ::= token_yield(token) expr_list(expr) SEMICOLON .
                {
                    x = p->alloc< xec_expr_yield >( token->sloc, expr );
                    p->destroy( token );
                }
stmt_common(x)  ::= THROW(token) expr_value(expr) SEMICOLON .
                {
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
                        expr_lbody(lvals) COLON expr_value(expr) RPN .
                {
                    x = p->foreachstmt( forscope.token->sloc,
                            forscope.scope, lvals, expr, false, false );
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN
                        expr_lbody(lvals) EACHKEY expr_value(expr) RPN .
                {
                    x = p->foreachstmt( forscope.token->sloc,
                            forscope.scope, lvals, expr, false, true );
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN
                        VAR name_list(names) COLON expr_value(expr) RPN .
                {
                    x = p->foreachstmt( forscope.token->sloc,
                            forscope.scope, names, expr, true, false );
                    p->destroy( forscope.token );
                }
stmt_foreach(x) ::= scope_for(forscope) LPN VAR name_list(names)
                                EACHKEY expr_value(expr) RPN .
                {
                    x = p->foreachstmt( forscope.token->sloc,
                            forscope.scope, names, expr, true, true );
                    p->destroy( forscope.token );
                }

stmt_for(x)     ::= scope_for(forscope) LPN
                        for_cond(einit) SEMICOLON
                            for_expr(expr) SEMICOLON
                                for_expr(eupdate) RPN .
                {
                    x = p->alloc< xec_stmt_for >( forscope.token->sloc );
                    forscope.scope->node = x;
                    x->scope        = forscope.scope;
                    x->init         = einit;
                    x->condition    = expr;
                    x->update       = eupdate;
                    p->destroy( forscope.token );
                }

for_cond(x)     ::= .
                {
                    x = nullptr;
                }
for_cond(x)     ::= condition(cond) .
                {
                    x = cond;
                }

for_expr(x)     ::= .
                {
                    x = nullptr;
                }
for_expr(x)     ::= expr_assign(expr) .
                {
                    x = expr;
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
                    x = p->catchstmt( cscope.token->sloc,
                            cscope.scope, nullptr, cproto, false );
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN
                        expr_value(lval) COLON expr_simple(cproto) RPN .
                {
                    x = p->catchstmt( cscope.token->sloc,
                            cscope.scope, lval, cproto, false );
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN
                        VAR name(name) COLON expr_simple(cproto) RPN .
                {
                    x = p->catchstmt( cscope.token->sloc,
                            cscope.scope, name, cproto, true );
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN expr_value(lval) RPN .
                {
                    x = p->catchstmt( cscope.token->sloc,
                            cscope.scope, lval, nullptr, false );
                    p->destroy( cscope.token );
                }
stmt_catch(x)   ::= scope_catch(cscope) LPN VAR name(name) RPN .
                {
                    x = p->catchstmt( cscope.token->sloc,
                            cscope.scope, name, nullptr, true );
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
    p->get_root()->script->error( TOKEN->sloc,
            "unexpected %s", TOKEN->get_spelling().c_str() );
}










