

%name               XecParse
%token_prefix       LEMON_

%include
{
    #include <assert.h>
    #include "xec_parser.h"
    #include "xec_token.h"
}





blah ::= expr SEMICOLON .
blah ::= decl .



//
// Names and declarations.
//

name(x)         ::= IDENTIFIER .
name(x)         ::= name PERIOD IDENTIFIER .

proto(x)        ::= name LPN arg_list RPN .

decl(x)         ::= name LBR odecl_list RBR .
decl(x)         ::= name COLON expr_value LBR odecl_list RBR .
decl(x)         ::= proto LBR stmt_list RBR .
decl(x)         ::= proto YIELD LBR stmt_list RBR .
decl(x)         ::= PERIOD name LPN arg_list RPN LBR stmt_list RBR .
decl(x)         ::= PERIOD name LPN arg_list RPN YIELD LBR stmt_list RBR .




//
// Expressions.
//

expr_call(x)    ::= proto LPN arg_list RPN .
expr_call(x)    ::= expr_call LPN arg_list RPN .
expr_call(x)    ::= expr_postfix LPN arg_list RPN .

expr_postfix(x) ::= LPN expr RPN .
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

expr_basic(x)   ::= name .
expr_basic(x)   ::= expr_postfix .
expr_basic(x)   ::= proto .
expr_basic(x)   ::= expr_call .
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

expr_value(x)   ::= expr_or .
expr_value(x)   ::= expr_or QMARK expr_value COLON expr_value .
expr_value(x)   ::= NEW name LPN arg_list RPN .
expr_value(x)   ::= NEW expr_postfix LPN arg_list RPN .
expr_value(x)   ::= COLON expr_value LBR odecl_list RBR .
expr_value(x)   ::= COLON LBR odecl_list RBR .
expr_value(x)   ::= QMARK LPN arg_list RPN LBR stmt_list RBR .
expr_value(x)   ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR .
expr_value(x)   ::= proto YIELD .
expr_value(x)   ::= expr_call YIELD .

expr_lbody(x)   ::= expr_value .
expr_lbody(x)   ::= expr_lbody COMMA expr_value .

expr_final(x)   ::= expr_postfix ELLIPSIS .
expr_final(x)   ::= expr_postfix LSQ RSQ ELLIPSIS .
expr_final(x)   ::= ELLIPSIS .

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

expr(x)         ::= expr_assign .

arg_list        ::= .
arg_list        ::= expr_list .







odecl_list ::= COMMA .
stmt_list ::= COMMA .



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

 ELLIPSIS .









