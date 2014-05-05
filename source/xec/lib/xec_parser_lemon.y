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
    #include <xec/ast/xec_expression.h>
    #include <xec/ast/xec_constructor.h>
    #include <xec/ast/xec_declaration.h>
    #include <xec/ast/xec_statement.h>
}


%extra_argument     { xec_parser* p }
%token_type         { xec_token* }


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
    delete $$;
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

// using statement: using x; vs using scope: using ( x ) {}
// Conflicts if the first expression in the using statement is in parentheses.
// Resolve in favour of the using scope (can always add another variable).
%nonassoc USING .

// The (higher-precedence) token to shift in the previous two cases.
%nonassoc LPN .



//
// Entire script.
//

script          ::= .
                {
                    p->set_root( NULL );
                }
script          ::= stmt_list(stmt_list) .
                {
                    p->set_root( stmt_list );
                }



//
// Braces with optional contents.
//

%type expr_paren    { xec_expression_list* }
%type stmt_brace    { xec_statement_compound* }
%type odecl_brace   { xec_declaration_object* }


expr_paren(x)   ::= LPN RPN .
                {
                    x = new xec_expression_list();
                }
expr_paren(x)   ::= LPN expr_list(expr_list) RPN .
                {
                    x = expr_list->as_list();
                }

stmt_brace(x)   ::= LBR(token) RBR .
                {
                    x = new xec_statement_compound();
                    x->set_token( token );
                }
stmt_brace(x)   ::= LBR(token) stmt_list(stmt_list) RBR .
                {
                    x = stmt_list;
                    x->set_token( token );
                }

odecl_brace(x)  ::= LBR RBR .
                {
                    x = new xec_declaration_object();
                }
odecl_brace(x)  ::= LBR odecl_list(object) RBR .
                {
                    x = object;
                }



//
// Names and declarations.
//

%type name          { xec_expression* }
%type name_list     { xec_expression* }
%type proto         { xec_expression_call* }
%type decl          { xec_declaration* }
%type odecl         { xec_declaration* }
%type odecl_list    { xec_declaration_object* }


// Names as used in declarations.
name(x)         ::= IDENTIFIER(token) .
                {
                    x = new xec_expression_identifier( token );
                }
name(x)         ::= name(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }


// Comma-separated lists of names.
name_list(x)    ::= name(expr) .
                {
                    x = expr;
                }
name_list(x)    ::= name_list(expr_list) COMMA name(expr) .
                {
                    xec_expression_list* list;
                    x = list = expr_list->as_list();
                    list->append_expression( expr );
                }


// Function headers - a single name with a single set of 'call' parentheses.
proto(x)        ::= name(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }


// Declarations valid as statements.
decl(x)         ::= name(name) odecl_brace(object) .
                {
                    object->set_name( name );
                    x = object;
                }
decl(x)         ::= name(name) COLON expr_simple(proto) odecl_brace(object) .
                {
                    // object already constructed by odecl_brace.
                    object->set_name( name );
                    object->set_proto( proto );
                    x = object;
                }
decl(x)         ::= proto(header) stmt_brace(body) .
                {
                    // Convert call expression to function declaration.
                    xec_declaration_function* value;
                    x = value = header->as_function();
                    value->set_body( body );
                }
decl(x)         ::= proto(header) YIELD stmt_brace(body) .
                {
                    // Convert call expression to coroutine declaration.
                    xec_declaration_function* value;
                    x = value = header->as_function();
                    value->set_coroutine( true );
                    value->set_body( body );
                }
decl(x)         ::= VAR(token) name_list(name_list) SEMICOLON .
                {
                    x = new xec_declaration_var(
                        token, name_list->as_list(), NULL );
                }
decl(x)         ::= VAR(token) name_list(name_list)
                            ASSIGN expr_list(expr_list) SEMICOLON .
                {
                    x = new xec_declaration_var(
                        token, name_list->as_list(), expr_list->as_list() );
                }


// Declarations valid only in objects (previous set, plus prototypes).
odecl(x)        ::= decl(decl) .
                {
                    x = decl;
                }
odecl(x)        ::= proto(header) SEMICOLON .
                {
                    // Convert call expression to prototype declaration.
                    x = header->as_prototype();
                }
odecl(x)        ::= proto(header) YIELD SEMICOLON .
                {
                    // Call expression becomes coroutine prototype declaration.
                    xec_declaration_prototype* decl;
                    x = decl = header->as_prototype();
                    decl->set_coroutine( true );
                }


odecl_list(x)   ::= SEMICOLON .
                {
                    x = new xec_declaration_object();
                }
odecl_list(x)   ::= odecl(decl) .
                {
                    x = new xec_declaration_object();
                    decl->set_thiscall( true );
                    x->add_member( decl );
                }
odecl_list(x)   ::= odecl_list(object) SEMICOLON .
                {
                    x = object;
                }
odecl_list(x)   ::= odecl_list(object) odecl(decl) .
                {
                    x = object;
                    decl->set_thiscall( true );
                    x->add_member( decl );
                }



//
// Expressions.
//

%type expr_index    { xec_expression* }
%type expr_yield    { xec_expression* }
%type expr_new      { xec_expression* }
%type expr_call     { xec_expression_call* }
%type expr_postfix  { xec_expression* }
%type expr_simple   { xec_expression* }
%type expr_literal  { xec_expression* }
%type expr_suffix   { xec_expression* }
%type expr_unary    { xec_expression* }
%type expr_mul      { xec_expression* }
%type expr_add      { xec_expression* }
%type expr_shift    { xec_expression* }
%type expr_bitand   { xec_expression* }
%type expr_bitxor   { xec_expression* }
%type expr_bitor    { xec_expression* }
%type expr_concat   { xec_expression* }
%type expr_compare  { xec_expression* }
%type expr_and      { xec_expression* }
%type expr_xor      { xec_expression* }
%type expr_or       { xec_expression* }
%type expr_nolbr    { xec_expression* }
%type expr_value    { xec_expression* }
%type expr_lbody    { xec_expression* }
%type expr_final    { xec_expression* }
%type expr_list     { xec_expression* }
%type expr_assign   { xec_expression* }
%type assign_op     { xec_token* }
%type value_lbody   { xec_constructor_list* }
%type value_list    { xec_constructor_list* }
%type keyval_lbody  { xec_constructor_table* }
%type keyval_list   { xec_constructor_table* }


// All lookups which aren't bare names, up to the first call parenthesis.
expr_index(x)   ::= LPN expr_assign(expr) RPN .
                {
                    // Multiple values in brackets collapse to a single value.
                    x = expr->as_mono();
                }
expr_index(x)   ::= name(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_index(x)   ::= name(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }
expr_index(x)   ::= expr_index(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_index(x)   ::= expr_index(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_index(x)   ::= expr_index(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }

// 'yield' expression - looks like a call but isn't.
expr_yield(x)   ::= YIELD(token) expr_paren(args) .
                {
                    x = new xec_expression_yield( token, args );
                }

// 'new' constructor - looks like a call but isn't.
expr_new(x)     ::= NEW(token) name(proto) expr_paren(args) .
                {
                    x = new xec_constructor_new( token, proto, args );
                }
expr_new(x)     ::= NEW(token) expr_index(proto) expr_paren(args) .
                {
                    x = new xec_constructor_new( token, proto, args );
                }

// All call expressions that aren't bare prototypes.
expr_call(x)    ::= proto(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }
expr_call(x)    ::= expr_index(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }
expr_call(x)    ::= expr_yield(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }
expr_call(x)    ::= expr_new(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }
expr_call(x)    ::= expr_call(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }
expr_call(x)    ::= expr_postfix(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }

// All lookups after the first call parenthesis.
expr_postfix(x) ::= proto(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_postfix(x) ::= proto(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_postfix(x) ::= proto(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }
expr_postfix(x) ::= expr_yield(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_postfix(x) ::= expr_yield(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_postfix(x) ::= expr_yield(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }
expr_postfix(x) ::= expr_new(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_postfix(x) ::= expr_new(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_postfix(x) ::= expr_new(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }
expr_postfix(x) ::= expr_call(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_postfix(x) ::= expr_call(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_postfix(x) ::= expr_call(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                }
expr_postfix(x) ::= expr_postfix(expr) PERIOD LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_indexkey( expr, index );
                }
expr_postfix(x) ::= expr_postfix(expr) LSQ expr_value(index) RSQ .
                {
                    x = new xec_expression_index( expr, index );
                }

// All lookup, call, and call-like expressions, including yield calls.  Used
// to specify prototypes, as it makes little sense to inherit from the result
// of an arithmetic expression, or from a literal.  Yield calls are at this
// precedence level because it is confusing to allow further postfix -
// a() yield.b - or call - a() yield() - expressions after the yield keyword.
expr_simple(x)  ::= name(expr) .
                {
                    x = expr;
                }
expr_simple(x)  ::= proto(expr) .
                {
                    x = expr;
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
                    expr->set_yieldcall( true );
                    x = expr;
                }
expr_simple(x)  ::= expr_call(expr) YIELD .
                {
                    expr->set_yieldcall( true );
                    x = expr;
                }

expr_literal(x) ::= expr_simple(expr) .
                {
                    x = expr;
                }
expr_literal(x) ::= NUMBER(token) .
                {
                    x = new xec_expression_number( token );
                }
expr_literal(x) ::= STRING(token) .
                {
                    x = new xec_expression_string( token );
                }
expr_literal(x) ::= TRUE(token) .
                {
                    x = new xec_expression_bool( token );
                }
expr_literal(x) ::= FALSE(token) .
                {
                    x = new xec_expression_bool( token );
                }
expr_literal(x) ::= NULL(token) .
                {
                    x = new xec_expression_null( token );
                }

expr_suffix(x)  ::= expr_literal(expr) .
                {
                    x = expr;
                }
expr_suffix(x)  ::= expr_suffix(expr) INCREMENT(token) .
                {
                    x = new xec_expression_postop( expr, token );
                }
expr_suffix(x)  ::= expr_suffix(expr) DECREMENT(token) .
                {
                    x = new xec_expression_postop( expr, token );
                }

expr_unary(x)   ::= expr_suffix(expr) .
                {
                    x = expr;
                }
expr_unary(x)   ::= PLUS(token) expr_unary(expr) .
                {
                    x = new xec_expression_unary( expr, token );
                }
expr_unary(x)   ::= MINUS(token) expr_unary(expr) .
                {
                    x = new xec_expression_unary( expr, token );
                }
expr_unary(x)   ::= XMARK(token) expr_unary(expr) .
                {
                    x = new xec_expression_unary( expr, token );
                }
expr_unary(x)   ::= TILDE(token) expr_unary(expr) .
                {
                    x = new xec_expression_unary( expr, token );
                }
expr_unary(x)   ::= INCREMENT(token) expr_unary(expr) .
                {
                    x = new xec_expression_preop( expr, token );
                }
expr_unary(x)   ::= DECREMENT(token) expr_unary(expr) .
                {
                    x = new xec_expression_preop( expr, token );
                }

expr_mul(x)     ::= expr_unary(expr) .
                {
                    x = expr;
                }
expr_mul(x)     ::= expr_mul(expr_a) ASTERISK(token) expr_unary(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_mul(x)     ::= expr_mul(expr_a) SOLIDUS(token) expr_unary(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_mul(x)     ::= expr_mul(expr_a) PERCENT(token) expr_unary(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_mul(x)     ::= expr_mul(expr_a) TILDE(token) expr_unary(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_add(x)     ::= expr_mul(expr) .
                {
                    x = expr;
                }
expr_add(x)     ::= expr_add(expr_a) PLUS(token) expr_mul(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_add(x)     ::= expr_add(expr_a) MINUS(token) expr_mul(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_shift(x)   ::= expr_add(expr) .
                {
                    x = expr;
                }
expr_shift(x)   ::= expr_shift(expr_a) LSHIFT(token) expr_add(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_shift(x)   ::= expr_shift(expr_a) RSHIFT(token) expr_add(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }
expr_shift(x)   ::= expr_shift(expr_a) URSHIFT(token) expr_add(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_bitand(x)  ::= expr_shift(expr) .
                {
                    x = expr;
                }
expr_bitand(x)  ::= expr_bitand(expr_a) AMPERSAND(token) expr_shift(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_bitxor(x)  ::= expr_bitand(expr) .
                {
                    x = expr;
                }
expr_bitxor(x)  ::= expr_bitxor(expr_a) CARET(token) expr_bitand(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_bitor(x)   ::= expr_bitxor(expr) .
                {
                    x = expr;
                }
expr_bitor(x)   ::= expr_bitor(expr_a) VBAR(token) expr_bitxor(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_concat(x)  ::= expr_bitor(expr) .
                {
                    x = expr;
                }
expr_concat(x)  ::= expr_concat(expr_a) CONCATENATE(token) expr_bitor(expr_b) .
                {
                    x = new xec_expression_binary( expr_a, token, expr_b );
                }

expr_compare(x) ::= expr_concat(expr) .
                {
                    x = expr;
                }
expr_compare(x) ::= expr_compare(expr_a) EQUAL(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) NOTEQUAL(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) LESS(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) GREATER(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) LESSEQUAL(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a)
                                GREATEREQUAL(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) IN(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) NOTIN(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) IS(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }
expr_compare(x) ::= expr_compare(expr_a) NOTIS(token) expr_concat(expr_b) .
                {
                    xec_expression_comparison* comparison;
                    x = comparison = expr_a->as_comparison();
                    comparison->add_comparison( token, expr_b );
                }

expr_and(x)     ::= expr_compare(expr) .
                {
                    x = expr;
                }
expr_and(x)     ::= expr_and(expr_a) LOGICAND(token) expr_compare(expr_b) .
                {
                    x = new xec_expression_logical( expr_a, token, expr_b );
                }

expr_xor(x)     ::= expr_and(expr) .
                {
                    x = expr;
                }
expr_xor(x)     ::= expr_xor(expr_a) LOGICXOR(token) expr_and(expr_b) .
                {
                    x = new xec_expression_logical( expr_a, token, expr_b );
                }

expr_or(x)      ::= expr_xor(expr) .
                {
                    x = expr;
                }
expr_or(x)      ::= expr_or(expr_a) LOGICOR(token) expr_xor(expr_b) .
                {
                    x = new xec_expression_logical( expr_a, token, expr_b );
                }

// Must exclude an open brace to avoid conflict with compound statement.
expr_nolbr(x)   ::= expr_or(expr) .
                {
                    x = expr;
                }
expr_nolbr(x)   ::= expr_or(comparison) QMARK
                                expr_value(iftrue) COLON expr_value(iffalse) .
                {
                    x = new xec_expression_conditional(
                                    comparison, iftrue, iffalse );
                }
expr_nolbr(x)   ::= LSQ(token) RSQ .
                {
                    xec_constructor_list* list;
                    x = list = new xec_constructor_list();
                    list->set_token( token );
                }
expr_nolbr(x)   ::= LSQ(token) value_list(expr) RSQ .
                {
                    x = expr;
                    expr->set_token( token );
                }
expr_nolbr(x)   ::= COLON(token) odecl_brace(object) .
                {
                    x = object->as_constructor( token );
                }
expr_nolbr(x)   ::= COLON(token) expr_simple(proto) odecl_brace(object) .
                {
                    object->set_proto( proto );
                    x = object->as_constructor( token );
                }
expr_nolbr(x)   ::= QMARK(token) expr_paren(params) stmt_brace(body) .
                {
                    x = new xec_constructor_function( token, params, body );
                }
expr_nolbr(x)   ::= PERIOD(token) QMARK expr_paren(params) stmt_brace(body) .
                {
                    xec_constructor_function* func;
                    x = func = new xec_constructor_function(
                                    token, params, body );
                    func->set_thiscall( true );
                }
expr_nolbr(x)   ::= QMARK(token) expr_paren(params) YIELD stmt_brace(body) .
                {
                    xec_constructor_function* func;
                    x = func = new xec_constructor_function(
                                    token, params, body );
                    func->set_coroutine( true );
                }
expr_nolbr(x)   ::= PERIOD(token) QMARK
                                expr_paren(params) YIELD stmt_brace(body) .
                {
                    xec_constructor_function* func;
                    x = func = new xec_constructor_function(
                                    token, params, body );
                    func->set_thiscall( true );
                    func->set_coroutine( true );
                }

// All single-value expressions, including those starting with an open brace.
expr_value(x)   ::= expr_nolbr(expr) .
                {
                    x = expr;
                }
expr_value(x)   ::= LBR(token) RBR .
                {
                    xec_constructor_table* table;
                    x = table = new xec_constructor_table();
                    table->set_token( token );
                }
expr_value(x)   ::= LBR(token) keyval_list(table) RBR .
                {
                    x = table;
                    table->set_token( token );
                }

expr_lbody(x)   ::= expr_value(expr) .
                {
                    x = expr;
                }
expr_lbody(x)   ::= expr_lbody(expr_list) COMMA expr_value(expr) .
                {
                    xec_expression_list* list;
                    x = list = expr_list->as_list();
                    list->append_expression( expr );
                }

// Unpack expressions are valid only as the last expression in a list.
expr_final(x)   ::= ELLIPSIS(token) .
                {
                    x = new xec_expression_varargs( token );
                }
expr_final(x)   ::= proto(expr) ELLIPSIS .
                {
                    expr->set_unpack( true );
                    x = expr;
                }
expr_final(x)   ::= expr_call(expr) ELLIPSIS .
                {
                    expr->set_unpack( true );
                    x = expr;
                }
expr_final(x)   ::= expr_yield(expr) ELLIPSIS .
                {
                    x = expr;
                }
expr_final(x)   ::= proto(expr) YIELD ELLIPSIS .
                {
                    expr->set_yieldcall( true );
                    expr->set_unpack( true );
                    x = expr;
                }
expr_final(x)   ::= expr_call(expr) YIELD ELLIPSIS .
                {
                    expr->set_yieldcall( true );
                    expr->set_unpack( true );
                    x = expr;
                }
expr_final(x)   ::= name(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= proto(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= expr_index(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= expr_yield(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= expr_new(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= expr_call(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }
expr_final(x)   ::= expr_postfix(expr) LSQ RSQ ELLIPSIS .
                {
                    x = new xec_expression_unpack( expr );
                }

expr_list(x)    ::= expr_final(expr) .
                {
                    x = expr;
                }
expr_list(x)    ::= expr_lbody(expr) .
                {
                    x = expr;
                }
expr_list(x)    ::= expr_lbody(expr_list) COMMA expr_final(expr) .
                {
                    xec_expression_list* list;
                    x = list = expr_list->as_list();
                    list->append_final( expr );
                }

expr_assign(x)  ::= expr_list(expr) .
                {
                    x = expr;
                }
expr_assign(x)  ::= expr_lbody(lvalue) assign_op(op) expr_assign(rvalue) .
                {
                    x = new xec_expression_assign( lvalue, op, rvalue );
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
                    x = new xec_constructor_list();
                    x->append_value( expr );
                }
value_lbody(x)  ::= value_lbody(list) COMMA expr_value(expr) .
                {
                    x = list;
                    x->append_value( expr );
                }

value_list(x)   ::= expr_final(expr) .
                {
                    x = new xec_constructor_list();
                    x->append_final( expr );
                }
value_list(x)   ::= expr_final(expr) COMMA .
                {
                    x = new xec_constructor_list();
                    x->append_final( expr );
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
                    x->append_final( expr );
                }

// Non-empty key -> value lists for { ... } constructors.
keyval_lbody(x) ::= expr_value(key) COLON expr_value(value) .
                {
                    x = new xec_constructor_table();
                    x->append_keyval( key, value );
                }
keyval_lbody(x) ::= keyval_lbody(table) COMMA
                                expr_value(key) COLON expr_value(value) .
                {
                    x = table;
                    x->append_keyval( key, value );
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
// Expression statements must exclude { in the initial position.
//

%type sexpr_lbody   { xec_expression* }
%type sexpr_list    { xec_expression* }
%type sexpr_assign  { xec_expression* }

sexpr_lbody(x)  ::= expr_nolbr(expr) .
                {
                    x = expr;
                }
sexpr_lbody(x)  ::= sexpr_lbody(expr_list) COMMA expr_value(expr) .
                {
                    xec_expression_list* list;
                    x = list = expr_list->as_list();
                    list->append_expression( expr );
                }

sexpr_list(x)   ::= expr_final(expr) .
                {
                    x = expr;
                }
sexpr_list(x)   ::= sexpr_lbody(expr) .
                {
                    x = expr;
                }
sexpr_list(x)   ::= sexpr_lbody(expr_list) COMMA expr_final(expr) .
                {
                    xec_expression_list* list;
                    x = list = expr_list->as_list();
                    list->append_final( expr );
                }

sexpr_assign(x) ::= sexpr_list(expr) .
                {
                    x = expr;
                }
sexpr_assign(x) ::= sexpr_lbody(lvalue) assign_op(op) expr_assign(rvalue) .
                {
                    x = new xec_expression_assign( lvalue, op, rvalue );
                }



//
// Statements
//

%type condition     { xec_expression* }
%type stmt_yield    { xec_token* }
%type stmt_using    { xec_token* }
%type stmt          { xec_statement* }
%type catch         { xec_statement_catch* }
%type catch_list    { xec_statement_try* }
%type stmt_list     { xec_statement_compound* }


// Conditions are expressions, or var declarations.
condition(x)    ::= expr_assign(expr) .
                {
                    x = expr;
                }
condition(x)    ::= VAR(token) name_list(name_list)
                                ASSIGN expr_list(expr_list) .
                {
                    x = new xec_expression_declare( token,
                                name_list->as_list(), expr_list->as_list() );
                }


// Productions used for conflict resolution.
stmt_yield(x)   ::= YIELD(token) .
                {
                    x = token;
                }

stmt_using(x)   ::= USING(token) .
                {
                    x = token;
                }

// Statements.
stmt(x)         ::= stmt_brace(stmt) .
                {
                    x = stmt;
                }
stmt(x)         ::= sexpr_assign(expr) SEMICOLON .
                {
                    x = new xec_statement_expression( expr );
                }
stmt(x)         ::= DELETE(token) expr_lbody(expr) SEMICOLON .
                {
                    x = new xec_statement_delete( token, expr->as_list() );
                }
stmt(x)         ::= IF(token) LPN condition(expr) RPN stmt(iftrue) .
                {
                    x = new xec_statement_if( token, expr, iftrue, NULL );
                }
stmt(x)         ::= IF(token) LPN condition(expr) RPN stmt(iftrue)
                                ELSE stmt(iffalse) .
                {
                    x = new xec_statement_if( token, expr, iftrue, iffalse );
                }
stmt(x)         ::= SWITCH(token) LPN condition(expr) RPN stmt_brace(body) .
                {
                    x = new xec_statement_switch( token, expr, body );
                }
stmt(x)         ::= CASE(token) expr_value(expr) COLON .
                {
                    x = new xec_statement_case( token, expr );
                }
stmt(x)         ::= DEFAULT(token) COLON .
                {
                    x = new xec_statement_case( token, NULL );
                }
stmt(x)         ::= WHILE(token) LPN condition(expr) RPN stmt(body) .
                {
                    x = new xec_statement_while( token, expr, body );
                }
stmt(x)         ::= DO(token) stmt(body) WHILE
                                LPN expr_assign(expr) RPN SEMICOLON .
                {
                    x = new xec_statement_do( token, body, expr );
                }
stmt(x)         ::= FOR(token) LPN expr_lbody(lvalue)
                                COLON expr_value(expr) RPN stmt(body) .
                {
                    x = new xec_statement_foreach( token,
                                    lvalue->as_list(), expr, body );
                }
stmt(x)         ::= FOR(token) LPN expr_lbody(lvalue)
                                EACHKEY expr_value(expr) RPN stmt(body) .
                {
                    xec_statement_foreach* stmt;
                    x = stmt = new xec_statement_foreach( token,
                                    lvalue->as_list(), expr, body );
                    stmt->set_eachkey( true );
                }
stmt(x)         ::= FOR(token) LPN VAR name_list(name_list)
                                COLON expr_value(expr) RPN stmt(body) .
                {
                    xec_statement_foreach* stmt;
                    x = stmt = new xec_statement_foreach( token,
                                        name_list->as_list(), expr, body );
                    stmt->set_declare( true );
                }
stmt(x)         ::= FOR(token) LPN VAR name_list(name_list)
                                EACHKEY expr_value(expr) RPN stmt(body) .
                {
                    xec_statement_foreach* stmt;
                    x = stmt = new xec_statement_foreach( token,
                                        name_list->as_list(), expr, body );
                    stmt->set_declare( true );
                    stmt->set_eachkey( true );
                }
stmt(x)         ::= FOR(token) LPN condition(init) SEMICOLON expr_assign(expr)
                                SEMICOLON expr_assign(update) RPN stmt(body) .
                {
                    x = new xec_statement_for(
                                        token, init, expr, update, body );
                }
stmt(x)         ::= CONTINUE(token) SEMICOLON .
                {
                    x = new xec_statement_continue( token );
                }
stmt(x)         ::= BREAK(token) SEMICOLON .
                {
                    x = new xec_statement_break( token );
                }
stmt(x)         ::= RETURN(token) SEMICOLON .
                {
                    x = new xec_statement_return( token, NULL );
                }
stmt(x)         ::= RETURN(token) expr_list(expr) SEMICOLON .
                {
                    x = new xec_statement_return( token, expr->as_list() );
                }
stmt(x)         ::= stmt_yield(token) SEMICOLON .
                {
                    x = new xec_statement_yield( token, NULL );
                }
stmt(x)         ::= stmt_yield(token) expr_list(expr) SEMICOLON .
                {
                    x = new xec_statement_yield( token, expr->as_list() );
                }
stmt(x)         ::= USING(token) LPN condition(expr) RPN stmt(body) .
                {
                    x = new xec_statement_usingscope( token, expr, body );
                }
stmt(x)         ::= stmt_using(token) condition(expr) SEMICOLON .
                {
                    x = new xec_statement_using( token, expr );
                }
stmt(x)         ::= TRY(token) stmt(body) catch_list(stmt) .
                {
                    x = stmt;
                    stmt->set_body( token, body );
                }
stmt(x)         ::= TRY(token) stmt(body) FINALLY(ftoken) stmt(fbody) .
                {
                    xec_statement_try* stmt;
                    x = stmt = new xec_statement_try();
                    stmt->set_body( token, body );
                    stmt->set_finally( ftoken, fbody );
                }
stmt(x)         ::= TRY(token) stmt(body) catch_list(stmt)
                                FINALLY(ftoken) stmt(fbody) .
                {
                    x = stmt;
                    stmt->set_body( token, body );
                    stmt->set_finally( ftoken, fbody );
                }
stmt(x)         ::= THROW(token) expr_value(expr) SEMICOLON .
                {
                    x = new xec_statement_throw( token, expr );
                }

// Catch clauses.
catch(x)        ::= CATCH(token) LPN COLON expr_simple(proto) RPN stmt(body) .
                {
                    x = new xec_statement_catch( token, NULL, proto, body );
                }
catch(x)        ::= CATCH(token) LPN expr_value(lvalue)
                                COLON expr_simple(proto) RPN stmt(body) .
                {
                    x = new xec_statement_catch( token, lvalue, proto, body );
                }
catch(x)        ::= CATCH(token) LPN VAR name(lvalue)
                                COLON expr_simple(proto) RPN stmt(body) .
                {
                    x = new xec_statement_catch( token, lvalue, proto, body );
                    x->set_declare( true );
                }

catch_list(x)   ::= catch(cstmt) .
                {
                    x = new xec_statement_try();
                    x->append_catch( cstmt );
                }
catch_list(x)   ::= catch_list(stmt) catch(cstmt) .
                {
                    x = stmt;
                    x->append_catch( cstmt );
                }

// Statement lists.
stmt_list(x)    ::= stmt(stmt) .
                {
                    x = new xec_statement_compound();
                    x->append_statement( stmt );
                }
stmt_list(x)    ::= decl(decl) .
                {
                    x = new xec_statement_compound();
                    xec_statement_declaration* stmt =
                                    new xec_statement_declaration( decl );
                    x->append_statement( stmt );
                }
stmt_list(x)    ::= SEMICOLON .
                {
                    x = new xec_statement_compound();
                }
stmt_list(x)    ::= stmt_list(stmt_list) stmt(stmt) .
                {
                    x = stmt_list;
                    x->append_statement( stmt );
                }
stmt_list(x)    ::= stmt_list(stmt_list) decl(decl) .
                {
                    x = stmt_list;
                    xec_statement_declaration* stmt =
                                    new xec_statement_declaration( decl );
                    x->append_statement( stmt );
                }
stmt_list(x)    ::= stmt_list(stmt_list) SEMICOLON .
                {
                    x = stmt_list;
                }



%syntax_error
{
    p->diagnostic( TOKEN->sloc,
            "unexpected %s", TOKEN->get_spelling().c_str() );
}










