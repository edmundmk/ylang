

%name               XecParse
%token_prefix       LEMON_

%include
{
    #include <assert.h>
    #include "xec_parser.h"
    #include "xec_token.h"
}



//
// 'Dangling else' conflict.
//

%nonassoc IF .
%nonassoc ELSE .



//
// Entire script.
//

script          ::= .
script          ::= stmt_list .



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

name_list(x)    ::= name .
name_list(x)    ::= name_list COMMA name .

proto(x)        ::= name expr_paren .

decl_common(x)  ::= name odecl_brace .
decl_common(x)  ::= name COLON expr_simple odecl_brace .
decl_common(x)  ::= proto stmt_brace .
decl_common(x)  ::= proto YIELD stmt_brace .

decl(x)         ::= decl_common .
decl(x)         ::= PERIOD proto stmt_brace .
decl(x)         ::= PERIOD proto YIELD stmt_brace .
decl(x)         ::= VAR name_list SEMICOLON .
decl(x)         ::= VAR name_list ASSIGN expr_list SEMICOLON .

odecl(x)        ::= SEMICOLON .
odecl(x)        ::= decl_common .
odecl(x)        ::= proto SEMICOLON .
odecl(x)        ::= proto YIELD SEMICOLON .
odecl(x)        ::= name_list SEMICOLON .
odecl(x)        ::= name_list ASSIGN expr_list SEMICOLON .

odecl_list(x)   ::= odecl .
odecl_list(x)   ::= odecl_list odecl .






//
// Expressions.
//

expr_call(x)    ::= proto expr_paren .
expr_call(x)    ::= expr_call expr_paren .
expr_call(x)    ::= expr_postfix expr_paren .

expr_postfix(x) ::= LPN expr_assign RPN .
expr_postfix(x) ::= name PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= name LSQ expr_value RSQ .
expr_postfix(x) ::= proto PERIOD IDENTIFIER .
expr_postfix(x) ::= proto PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= proto LSQ expr_value RSQ .
expr_postfix(x) ::= expr_call PERIOD IDENTIFIER .
expr_postfix(x) ::= expr_call PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= expr_call LSQ expr_value RSQ .
expr_postfix(x) ::= expr_postfix PERIOD IDENTIFIER .
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

expr_unary(x)   ::= expr_basic .
expr_unary(x)   ::= PLUS expr_basic .
expr_unary(x)   ::= MINUS expr_basic .
expr_unary(x)   ::= XMARK expr_basic .
expr_unary(x)   ::= TILDE expr_basic .

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
expr_nolbr(x)   ::= QMARK expr_paren stmt_brace .
expr_nolbr(x)   ::= PERIOD QMARK expr_paren stmt_brace .

expr_value(x)   ::= expr_nolbr(x) .
expr_value(x)   ::= LBR RBR .
expr_value(x)   ::= LBR keyval_list RBR .

expr_lbody(x)   ::= expr_value .
expr_lbody(x)   ::= expr_lbody COMMA expr_value .

expr_final(x)   ::= expr_postfix ELLIPSIS .
expr_final(x)   ::= expr_postfix LSQ RSQ ELLIPSIS .
expr_final(x)   ::= ELLIPSIS .

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
value_list(x)   ::= expr_lbody COMMA expr_final COMMA .

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


condition       ::= expr_assign .
condition       ::= VAR name_list ASSIGN expr_list .

stmt            ::= stmt_brace .
stmt            ::= sexpr_assign SEMICOLON .
stmt            ::= IF LPN condition RPN stmt .
stmt            ::= IF LPN condition RPN stmt ELSE stmt .
stmt            ::= SWITCH LPN condition RPN stmt_brace .
stmt            ::= CASE expr_value COLON .
stmt            ::= DEFAULT COLON .
stmt            ::= WHILE LPN condition RPN stmt .
stmt            ::= DO stmt WHILE LPN expr_assign RPN SEMICOLON .
stmt            ::= FOR LPN expr_lbody COLON expr_value RPN stmt .
stmt            ::= FOR LPN expr_lbody EACHKEY expr_value RPN stmt .
stmt            ::= FOR LPN VAR name_list COLON expr_value RPN stmt .
stmt            ::= FOR LPN VAR name_list EACHKEY expr_value RPN stmt .
stmt            ::= FOR LPN condition SEMICOLON
                            expr_assign SEMICOLON expr_assign RPN stmt .
stmt            ::= CONTINUE SEMICOLON .
stmt            ::= BREAK SEMICOLON .
stmt            ::= RETURN SEMICOLON .
stmt            ::= RETURN expr_list SEMICOLON .





stmt_list       ::= stmt .
stmt_list       ::= decl .
stmt_list       ::= SEMICOLON .
stmt_list       ::= stmt_list stmt .
stmt_list       ::= stmt_list decl .
stmt_list       ::= stmt_list SEMICOLON .







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









