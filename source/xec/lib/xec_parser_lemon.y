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
// Conflicts if the expression in the using statement is in parentheses.
// Resolve in favour of the using scope (can always add another variable).
%nonassoc USING .

// The (higher-precedence) token to shift in the previous two cases.
%nonassoc LPN .



//
// Entire script.
//

script(x)       ::= .
script(x)       ::= stmt_list .



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

stmt_brace(x)   ::= LBR RBR .
                {
                    x = new xec_statement_compound();
                }
stmt_brace(x)   ::= LBR stmt_list(stmt_list) RBR .
                {
                    x = stmt_list;
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


name(x)         ::= IDENTIFIER(token) .
                {
                    x = new xec_expression_identifier( token );
                    p->destroy( token );
                }
name(x)         ::= name(expr) PERIOD IDENTIFIER(token) .
                {
                    x = new xec_expression_lookup( expr, token );
                    p->destroy( token );
                }


name_list(x)    ::= name(expr) .
                {
                    x = expr;
                }
name_list(x)    ::= name_list(expr_list) COMMA name(expr) .
                {
                    x = expr_list;
                    x->as_list()->append_expression( expr );
                }


proto(x)        ::= name(expr) expr_paren(args) .
                {
                    x = new xec_expression_call( expr, args );
                }


decl(x)         ::= name(name) odecl_brace(object) .
                {
                    object->set_name( name );
                    x = object;
                }
decl(x)         ::= name(name) COLON expr_simple(proto) odecl_brace(object) .
                {
                    object->set_name( name );
                    object->set_prototype( proto );
                    x = object;
                }
decl(x)         ::= proto(header) stmt_brace(body) .
                {
                    xec_declaration_function* value;
                    x = value = header->as_function();
                    value->set_body( body );
                    delete header;
                }
decl(x)         ::= proto(header) YIELD stmt_brace(body) .
                {
                    xec_declaration_function* value;
                    x = value = header->as_function();
                    value->set_coroutine( true );
                    value->set_body( body );
                    delete header;
                }
decl(x)         ::= VAR name_list(name_list) SEMICOLON .
                {
                    x = new xec_declaration_var(
                                name_list->as_list(), NULL );
                }
decl(x)         ::= VAR name_list(name_list)
                            ASSIGN expr_list(expr_list) SEMICOLON .
                {
                    x = new xec_declaration_var(
                                name_list->as_list(), expr_list->as_list() );
                }


odecl(x)        ::= decl(decl) .
                {
                    x = decl;
                }
odecl(x)        ::= proto(header) SEMICOLON .
                {
                    x = header->as_prototype();
                    delete header;
                }
odecl(x)        ::= proto(header) YIELD SEMICOLON .
                {
                    xec_declaration_prototype* decl;
                    x = decl = header->as_prototype();
                    decl->set_coroutine( true );
                    delete header;
                }


odecl_list(x)   ::= SEMICOLON .
                {
                    x = new xec_declaration_object();
                }
odecl_list(x)   ::= odecl(decl) .
                {
                    x = new xec_declaration_object();
                    x->add_declaration( decl );
                }
odecl_list(x)   ::= odecl_list(object) SEMICOLON .
                {
                    x = object;
                }
odecl_list(x)   ::= odecl_list(object) odecl(decl) .
                {
                    x = object;
                    x->add_declaration( decl );
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
%type expr_compare  { xec_expression* }
%type expr_and      { xec_expression* }
%type expr_xor      { xec_expression* }
%type expr_or       { xec_expression* }
%type expr_nolbr    { xec_expression* }
%type expr_value    { xec_expression* }
%type expr_lbody    { xec_expression_list* }
%type expr_final    { xec_expression* }
%type expr_list     { xec_expression* }
%type expr_assign   { xec_expression* }
%type assign_op     { xec_token* }
%type value_list    { xec_expression_list* }


// All lookups which aren't bare names, up to the first call parenthesis.
expr_index(x)   ::= LPN expr_assign(expr) RPN .
expr_index(x)   ::= name(expr) PERIOD LSQ expr_value(value) RSQ .
expr_index(x)   ::= name(expr) LSQ expr_value(index) RSQ .
expr_index(x)   ::= expr_index(expr) PERIOD IDENTIFIER .
expr_index(x)   ::= expr_index(expr) PERIOD LSQ expr_value(value) RSQ .
expr_index(x)   ::= expr_index(expr) LSQ expr_value(index) RSQ .

// 'yield' expression - looks like a call but isn't.
expr_yield(x)   ::= YIELD expr_paren(args) .

// 'new' constructor - looks like a call but isn't.
expr_new(x)     ::= NEW name(type) expr_paren(args) .
expr_new(x)     ::= NEW expr_index(type) expr_paren(args) .

// All call expressions that aren't bare prototypes.
expr_call(x)    ::= proto(expr) expr_paren(args) .
expr_call(x)    ::= expr_index(expr) expr_paren(args) .
expr_call(x)    ::= expr_yield(expr) expr_paren(args) .
expr_call(x)    ::= expr_new(expr) expr_paren(args) .
expr_call(x)    ::= expr_call(expr) expr_paren(args) .
expr_call(x)    ::= expr_postfix(expr) expr_paren(args) .

// All lookups after the first call parenthesis.
expr_postfix(x) ::= proto(expr) PERIOD IDENTIFIER(token) .
expr_postfix(x) ::= proto(expr) PERIOD LSQ expr_value(value) RSQ .
expr_postfix(x) ::= proto(expr) LSQ expr_value(index) RSQ .
expr_postfix(x) ::= expr_yield(expr) PERIOD IDENTIFIER(token) .
expr_postfix(x) ::= expr_yield(expr) PERIOD LSQ expr_value(value) RSQ .
expr_postfix(x) ::= expr_yield(expr) LSQ expr_value(index) RSQ .
expr_postfix(x) ::= expr_new(expr) PERIOD IDENTIFIER(token) .
expr_postfix(x) ::= expr_new(expr) PERIOD LSQ expr_value(value) RSQ .
expr_postfix(x) ::= expr_new(expr) LSQ expr_value(index) RSQ .
expr_postfix(x) ::= expr_call(expr) PERIOD IDENTIFIER(token) .
expr_postfix(x) ::= expr_call(expr) PERIOD LSQ expr_value(value) RSQ .
expr_postfix(x) ::= expr_call(expr) LSQ expr_value(index) RSQ .
expr_postfix(x) ::= expr_postfix(expr) PERIOD IDENTIFIER(token) .
expr_postfix(x) ::= expr_postfix(expr) PERIOD LSQ expr_value(value) RSQ .
expr_postfix(x) ::= expr_postfix(expr) LSQ expr_value(index) RSQ .

// All lookup, call, and call-like expressions, including yield calls.
expr_simple(x)  ::= name(expr) .
expr_simple(x)  ::= proto(expr) .
expr_simple(x)  ::= expr_index(expr) .
expr_simple(x)  ::= expr_yield(expr) .
expr_simple(x)  ::= expr_new(x) .
expr_simple(x)  ::= expr_call(expr) .
expr_simple(x)  ::= expr_postfix(expr) .
expr_simple(x)  ::= proto(expr) YIELD .
expr_simple(x)  ::= expr_call(expr) YIELD .

expr_literal(x) ::= expr_simple .
expr_literal(x) ::= NUMBER .
expr_literal(x) ::= STRING .
expr_literal(x) ::= TRUE .
expr_literal(x) ::= FALSE .
expr_literal(x) ::= NULL .

expr_suffix(x)  ::= expr_literal .
expr_suffix(x)  ::= expr_suffix INCREMENT .
expr_suffix(x)  ::= expr_suffix DECREMENT .

expr_unary(x)   ::= expr_suffix .
expr_unary(x)   ::= PLUS expr_unary .
expr_unary(x)   ::= MINUS expr_unary .
expr_unary(x)   ::= XMARK expr_unary .
expr_unary(x)   ::= TILDE expr_unary .
expr_unary(x)   ::= INCREMENT expr_unary .
expr_unary(x)   ::= DECREMENT expr_unary .

expr_mul(x)     ::= expr_unary .
expr_mul(x)     ::= expr_mul ASTERISK expr_unary .
expr_mul(x)     ::= expr_mul SOLIDUS expr_unary .
expr_mul(x)     ::= expr_mul PERCENT expr_unary .
expr_mul(x)     ::= expr_mul TILDE expr_unary .

expr_add(x)     ::= expr_mul .
expr_add(x)     ::= expr_add PLUS expr_mul .
expr_add(x)     ::= expr_add MINUS expr_mul .

expr_shift(x)   ::= expr_add .
expr_shift(x)   ::= expr_shift LSHIFT expr_add .
expr_shift(x)   ::= expr_shift RSHIFT expr_add .
expr_shift(x)   ::= expr_shift URSHIFT expr_add .

expr_bitand(x)  ::= expr_shift .
expr_bitand(x)  ::= expr_bitand AMPERSAND expr_shift .

expr_bitxor(x)  ::= expr_bitand .
expr_bitxor(x)  ::= expr_bitxor CARET expr_bitand .

expr_bitor(x)   ::= expr_bitxor .
expr_bitor(x)   ::= expr_bitor VBAR expr_bitxor .

expr_compare(x) ::= expr_bitor .
expr_compare(x) ::= expr_compare EQUAL expr_bitor .
expr_compare(x) ::= expr_compare NOTEQUAL expr_bitor .
expr_compare(x) ::= expr_compare LESS expr_bitor .
expr_compare(x) ::= expr_compare GREATER expr_bitor .
expr_compare(x) ::= expr_compare LESSEQUAL expr_bitor .
expr_compare(x) ::= expr_compare GREATEREQUAL expr_bitor .
expr_compare(x) ::= expr_compare IN expr_bitor .
expr_compare(x) ::= expr_compare NOTIN expr_bitor .
expr_compare(x) ::= expr_compare IS expr_bitor .
expr_compare(x) ::= expr_compare NOTIS expr_bitor .

expr_and(x)     ::= expr_compare .
expr_and(x)     ::= expr_and LOGICAND expr_compare .

expr_xor(x)     ::= expr_and .
expr_xor(x)     ::= expr_xor LOGICXOR expr_and .

expr_or(x)      ::= expr_xor .
expr_or(x)      ::= expr_or LOGICOR expr_xor .

// Must exclude an open brace to avoid conflict with compound statement.
expr_nolbr(x)   ::= expr_or .
expr_nolbr(x)   ::= expr_or QMARK expr_value COLON expr_value .
expr_nolbr(x)   ::= LSQ RSQ .
expr_nolbr(x)   ::= LSQ value_list RSQ .
expr_nolbr(x)   ::= COLON odecl_brace .
expr_nolbr(x)   ::= COLON expr_simple odecl_brace .
expr_nolbr(x)   ::= QMARK expr_paren sexpr_assign SEMICOLON .
expr_nolbr(x)   ::= QMARK expr_paren stmt_brace .
expr_nolbr(x)   ::= PERIOD QMARK expr_paren sexpr_assign SEMICOLON .
expr_nolbr(x)   ::= PERIOD QMARK expr_paren stmt_brace .

// All single-valued expressions, including those starting with an open brace.
expr_value(x)   ::= expr_nolbr(x) .
expr_value(x)   ::= LBR RBR .
expr_value(x)   ::= LBR keyval_list RBR .

expr_lbody(x)   ::= expr_value .
expr_lbody(x)   ::= expr_lbody COMMA expr_value .

expr_final(x)   ::= ELLIPSIS .
expr_final(x)   ::= proto ELLIPSIS .
expr_final(x)   ::= expr_call ELLIPSIS .
expr_final(x)   ::= proto YIELD ELLIPSIS .
expr_final(x)   ::= expr_call YIELD ELLIPSIS .
expr_final(x)   ::= name LSQ RSQ ELLIPSIS .
expr_final(x)   ::= proto LSQ RSQ ELLIPSIS .
expr_final(x)   ::= expr_call LSQ RSQ ELLIPSIS .
expr_final(x)   ::= expr_postfix LSQ RSQ ELLIPSIS .

expr_list(x)    ::= expr_final .
expr_list(x)    ::= expr_lbody .
expr_list(x)    ::= expr_lbody COMMA expr_final .

expr_assign(x)  ::= expr_list .
expr_assign(x)  ::= expr_lbody assign_op expr_list .

assign_op(x)    ::= ASSIGN .
assign_op(x)    ::= MULASSIGN .
assign_op(x)    ::= DIVASSIGN .
assign_op(x)    ::= MODASSIGN .
assign_op(x)    ::= INTDIVASSIGN .
assign_op(x)    ::= ADDASSIGN .
assign_op(x)    ::= SUBASSIGN .
assign_op(x)    ::= LSHIFTASSIGN .
assign_op(x)    ::= RSHIFTASSIGN .
assign_op(x)    ::= URSHIFTASSIGN .
assign_op(x)    ::= BITANDASSIGN .
assign_op(x)    ::= BITXORASSIGN .
assign_op(x)    ::= BITORASSIGN .

value_list(x)   ::= expr_final .
value_list(x)   ::= expr_final COMMA .
value_list(x)   ::= expr_lbody .
value_list(x)   ::= expr_lbody COMMA .
value_list(x)   ::= expr_lbody COMMA expr_final .

keyval_lbody(x) ::= expr_value COLON expr_value .
keyval_lbody(x) ::= keyval_lbody COMMA expr_value COLON expr_value .

keyval_list(x)  ::= keyval_lbody .
keyval_list(x)  ::= keyval_lbody COMMA .



//
// Expression statements must exclude { in the initial position.
//

sexpr_lbody(x)  ::= expr_nolbr .
sexpr_lbody(x)  ::= sexpr_lbody COMMA expr_value .

sexpr_list(x)   ::= expr_final .
sexpr_list(x)   ::= sexpr_lbody .
sexpr_list(x)   ::= sexpr_lbody COMMA expr_final .

sexpr_assign(x) ::= sexpr_list .
sexpr_assign(x) ::= sexpr_lbody assign_op expr_list .



//
// Statements
//


condition(x)    ::= expr_assign .
condition(x)    ::= VAR name_list ASSIGN expr_list .

stmt_yield(x)   ::= YIELD .

stmt_using(x)   ::= USING .

stmt(x)         ::= stmt_brace .
stmt(x)         ::= sexpr_assign SEMICOLON .
stmt(x)         ::= DELETE expr_lbody SEMICOLON .
stmt(x)         ::= IF LPN condition RPN stmt .
stmt(x)         ::= IF LPN condition RPN stmt ELSE stmt .
stmt(x)         ::= SWITCH LPN condition RPN stmt_brace .
stmt(x)         ::= CASE expr_value COLON .
stmt(x)         ::= DEFAULT COLON .
stmt(x)         ::= WHILE LPN condition RPN stmt .
stmt(x)         ::= DO stmt WHILE LPN expr_assign RPN SEMICOLON .
stmt(x)         ::= FOR LPN expr_lbody COLON expr_value RPN stmt .
stmt(x)         ::= FOR LPN expr_lbody EACHKEY expr_value RPN stmt .
stmt(x)         ::= FOR LPN VAR name_list COLON expr_value RPN stmt .
stmt(x)         ::= FOR LPN VAR name_list EACHKEY expr_value RPN stmt .
stmt(x)         ::= FOR LPN condition SEMICOLON
                            expr_assign SEMICOLON expr_assign RPN stmt .
stmt(x)         ::= CONTINUE SEMICOLON .
stmt(x)         ::= BREAK SEMICOLON .
stmt(x)         ::= RETURN SEMICOLON .
stmt(x)         ::= RETURN expr_list SEMICOLON .
stmt(x)         ::= stmt_yield SEMICOLON .
stmt(x)         ::= stmt_yield expr_list SEMICOLON .
stmt(x)         ::= USING LPN condition RPN stmt .
stmt(x)         ::= stmt_using condition SEMICOLON .
stmt(x)         ::= TRY stmt catch_list .
stmt(x)         ::= TRY stmt FINALLY stmt .
stmt(x)         ::= TRY stmt catch_list FINALLY stmt .
stmt(x)         ::= THROW expr_value SEMICOLON .

catch(x)        ::= CATCH LPN COLON expr_simple RPN stmt .
catch(x)        ::= CATCH LPN expr_value COLON expr_simple RPN stmt .
catch(x)        ::= CATCH LPN VAR name COLON expr_simple RPN stmt .

catch_list(x)   ::= catch .
catch_list(x)   ::= catch_list catch .

stmt_list(x)    ::= stmt .
stmt_list(x)    ::= decl .
stmt_list(x)    ::= SEMICOLON .
stmt_list(x)    ::= stmt_list stmt .
stmt_list(x)    ::= stmt_list decl .
stmt_list(x)    ::= stmt_list SEMICOLON .








/* you can only yield where we expect an expr_list, this is:


    each script is a function.  we don't know what the parameters to the function
    are when we compile it.
    
    outside the function is the global scope.  The function can reference the
    global scope with the identifier global.
    
    

    m()
    {
    }
    
    
    declares in current scope
    
    
    a.m()
    
    looks up a.  if a does not exist, this is an error.
    reopening scopes?
    
    



    this()
    {
    }
 
    ~this()
    {
    }



green_sword : weapon
{
    var x;
    var y;

    this()
    {
        x = 4;
        y = 23;
    }
    
    dispose()
    {
    }


    damage()
    {
 
    }
    
    damage() yield
    {
 
    }

}



    implicit this:
    
        functions declared inside an object scope.
        functions declared inside an object scope (out-of-line).
        functions declared with .name()
        function literals with .?()
        
    otherwise no implicit this and this can be an upval.
 


    swords
    {
    
        glitchy
        {
        }
        
        
        glitchy.blah()
        {
        }
        
        glitchy.blah()
 
        glitchy.new()
        {
        }
 
        glitchy.delete()
        {
 
        }
 
    }







        yield a, b;
        a, b = yield a, b;
        x = [ yield a, b ];
        f( yield a, b );
    
        yield;
        yield( a, b );
        return a, b;
        a, b = yield( a, b ) ...;
        x = [ yield( a, b ) ... ];
        f( yield( a, b ) ... );
 
 
        select( 1, yield( a, b ) ... );
        x, y = yield( a, b ) ...;
        yield;
        yield new blah( x );
        
        yield( a, b );
 
        yield( ( new x() ).blah )
        yield new x();
        yield new x(), y() ...;
 
 
 
        f( a, b, yield a, b )

        hmmm.
        
        yield;
        yield a, b;
        x, y = yield( a, b );
        
        yield;
        
        x, y = 2, yield a, b, yield f, m;
 
        x, y = yield() ...
        x, y = yield( a, b ) ...
 
        yield( a, b )
        yield a, b;
 
 
 

        yield        yield a, b;
( a, b ) ... is nicer in some ways.



tokens ::=
 BREAK
 CASE
 CATCH
 CONTINUE
 DEFAULT
 DO
 ELSE
 FALSE
 FINALLY
 FOR
 IF
 NEW
 NULL
 RETURN
 SWITCH
 THROW
 TRUE
 TRY
 USING
 VAR
 WHILE
 YIELD

 IDENTIFIER
 NUMBER
 STRING
    
 XMARK
 PERCENT
 AMPERSAND
 LPN
 RPN
 ASTERISK
 PLUS
 COMMA
 MINUS
 PERIOD
 SOLIDUS
 COLON
 SEMICOLON
 LESS
 ASSIGN
 GREATER
 QMARK
 LSQ
 RSQ
 CARET
 LBR
 VBAR
 RBR
 TILDE
    
 INCREMENT
 DECREMENT

 LSHIFT
 RSHIFT
 URSHIFT
    
 NOTEQUAL
 LESSEQUAL
 EQUAL
 GREATEREQUAL
    
 MODASSIGN
 BITANDASSIGN
 MULASSIGN
 ADDASSIGN
 SUBASSIGN
 DIVASSIGN
 BITXORASSIGN
 BITORASSIGN
 INTDIVASSIGN
 LSHIFTASSIGN
 RSHIFTASSIGN
 URSHIFTASSIGN
    
 LOGICAND
 LOGICXOR
 LOGICOR

 EACHKEY
 ELLIPSIS .


*/






