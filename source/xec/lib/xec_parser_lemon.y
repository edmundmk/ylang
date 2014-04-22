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
// Conflicts if the first expression in the yeild statement is in parentheses.
// Resolve in favour of the yield expression (can always add more parentheses).
%nonassoc YIELD .

// using statement: using x; vs using scope: using ( x ) {}
// Conflicts if the expression in the using statement is in parentheses.
// Resolve in favour of the using scope (can always add more parentheses).
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

expr_paren(x)   ::= LPN RPN .
expr_paren(x)   ::= LPN expr_list RPN .

stmt_brace(x)   ::= LBR RBR .
stmt_brace(x)   ::= LBR stmt_list RBR .

odecl_brace(x)  ::= LBR RBR .
odecl_brace(x)  ::= LBR odecl_list RBR .



//
// Names and declarations.
//

name(x)         ::= IDENTIFIER .
name(x)         ::= name PERIOD IDENTIFIER .
name(x)         ::= name PERIOD TILDE IDENTIFIER .

name_list(x)    ::= name .
name_list(x)    ::= name_list COMMA name .

proto(x)        ::= name expr_paren .

decl(x)         ::= name odecl_brace .
decl(x)         ::= name COLON expr_simple odecl_brace .
decl(x)         ::= proto stmt_brace .
decl(x)         ::= proto YIELD stmt_brace .
decl(x)         ::= VAR name_list SEMICOLON .
decl(x)         ::= VAR name_list ASSIGN expr_list SEMICOLON .

odecl(x)        ::= SEMICOLON .
odecl(x)        ::= decl .
odecl(x)        ::= proto SEMICOLON .
odecl(x)        ::= proto YIELD SEMICOLON .
odecl(x)        ::= TILDE proto stmt_brace .
odecl(x)        ::= TILDE proto YIELD stmt_brace .
odecl(x)        ::= TILDE proto SEMICOLON .
odecl(x)        ::= TILDE proto YIELD SEMICOLON .

odecl_list(x)   ::= odecl .
odecl_list(x)   ::= odecl_list odecl .



//
// Expressions.
//

expr_call(x)    ::= YIELD expr_paren .
expr_call(x)    ::= proto expr_paren .
expr_call(x)    ::= expr_call expr_paren .
expr_call(x)    ::= expr_postfix expr_paren .

expr_postfix(x) ::= LPN expr_assign RPN .
expr_postfix(x) ::= name PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= name LSQ expr_value RSQ .
expr_postfix(x) ::= proto PERIOD IDENTIFIER .
expr_postfix(x) ::= proto PERIOD TILDE IDENTIFIER .
expr_postfix(x) ::= proto PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= proto LSQ expr_value RSQ .
expr_postfix(x) ::= expr_call PERIOD IDENTIFIER .
expr_postfix(x) ::= expr_call PERIOD TILDE IDENTIFIER .
expr_postfix(x) ::= expr_call PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= expr_call LSQ expr_value RSQ .
expr_postfix(x) ::= expr_postfix PERIOD IDENTIFIER .
expr_postfix(x) ::= expr_postfix PERIOD TILDE IDENTIFIER .
expr_postfix(x) ::= expr_postfix PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= expr_postfix LSQ expr_value RSQ .

expr_simple(x)  ::= name .
expr_simple(x)  ::= expr_postfix .
expr_simple(x)  ::= proto .
expr_simple(x)  ::= expr_call .
expr_simple(x)  ::= proto YIELD .
expr_simple(x)  ::= expr_call YIELD .

expr_basic(x)   ::= expr_simple .
expr_basic(x)   ::= NUMBER .
expr_basic(x)   ::= STRING .
expr_basic(x)   ::= TRUE .
expr_basic(x)   ::= FALSE .
expr_basic(x)   ::= NULL .

expr_suffix(x)  ::= expr_basic .
expr_suffix(x)  ::= expr_basic INCREMENT .
expr_suffix(x)  ::= expr_basic DECREMENT .

expr_unary(x)   ::= expr_suffix .
expr_unary(x)   ::= PLUS expr_basic .
expr_unary(x)   ::= MINUS expr_basic .
expr_unary(x)   ::= XMARK expr_basic .
expr_unary(x)   ::= TILDE expr_basic .
expr_unary(x)   ::= INCREMENT expr_basic .
expr_unary(x)   ::= DECREMENT expr_basic .

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

expr_nolbr(x)   ::= expr_or .
expr_nolbr(x)   ::= expr_or QMARK expr_value COLON expr_value .
expr_nolbr(x)   ::= NEW name expr_paren .
expr_nolbr(x)   ::= NEW expr_postfix expr_paren .
expr_nolbr(x)   ::= LSQ RSQ .
expr_nolbr(x)   ::= LSQ value_list RSQ .
expr_nolbr(x)   ::= COLON odecl_brace .
expr_nolbr(x)   ::= COLON expr_simple odecl_brace .
expr_nolbr(x)   ::= QMARK expr_paren sexpr_assign SEMICOLON .
expr_nolbr(x)   ::= QMARK expr_paren stmt_brace .
expr_nolbr(x)   ::= PERIOD QMARK expr_paren sexpr_assign SEMICOLON .
expr_nolbr(x)   ::= PERIOD QMARK expr_paren stmt_brace .

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






