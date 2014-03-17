

%name               XecParse
%token_prefix       LEMON_

%include
{
    #include <assert.h>
    #include "xec_parser.h"
    #include "xec_token.h"
}


%nonassoc BREAK .
%nonassoc CASE .
%nonassoc CATCH .
%nonassoc CONTINUE .
%nonassoc DEFAULT .
%nonassoc DO .
%nonassoc ELSE .
%nonassoc FALSE .
%nonassoc FINALLY .
%nonassoc FOR .
%nonassoc IF .
%nonassoc NEW .
%nonassoc NULL .
%nonassoc RETURN .
%nonassoc SWITCH .
%nonassoc THROW .
%nonassoc TRUE .
%nonassoc TRY .
%nonassoc USING .
%nonassoc VAR .
%nonassoc WHILE .
%nonassoc YIELD .

%nonassoc IDENTIFIER .
%nonassoc NUMBER .
%nonassoc STRING .
    
%nonassoc XMARK .
%nonassoc PERCENT .
%nonassoc AMPERSAND .
%nonassoc LPN .
%nonassoc RPN .
%nonassoc ASTERISK .
%nonassoc PLUS .
%nonassoc COMMA .
%nonassoc MINUS .
%nonassoc PERIOD .
%nonassoc SOLIDUS .
%nonassoc COLON .
%nonassoc SEMICOLON .
%nonassoc LESS .
%nonassoc ASSIGN .
%nonassoc GREATER .
%nonassoc QMARK .
%nonassoc LSQ .
%nonassoc RSQ .
%nonassoc CARET .
%nonassoc LBR .
%nonassoc VBAR .
%nonassoc RBR .
%nonassoc TILDE .
    
%nonassoc INCREMENT .
%nonassoc DECREMENT .

%nonassoc LSHIFT .
%nonassoc RSHIFT .
%nonassoc URSHIFT .
    
%nonassoc NOTEQUAL .
%nonassoc LESSEQUAL .
%nonassoc EQUAL .
%nonassoc GREATEREQUAL .
    
%nonassoc MODASSIGN .
%nonassoc BITANDASSIGN .
%nonassoc MULASSIGN .
%nonassoc ADDASSIGN .
%nonassoc SUBASSIGN .
%nonassoc DIVASSIGN .
%nonassoc BITXORASSIGN .
%nonassoc BITORASSIGN .
%nonassoc INTDIVASSIGN .
%nonassoc LSHIFTASSIGN .
%nonassoc RSHIFTASSIGN .
%nonassoc URSHIFTASSIGN .
    
%nonassoc LOGICAND .
%nonassoc LOGICXOR .
%nonassoc LOGICOR .

%nonassoc ELLIPSIS .



blah ::= expr_final .
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

expr_postfix(x) ::= LPN expr RPN .
expr_postfix(x) ::= name PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= name LSQ expr_value RSQ .
expr_postfix(x) ::= proto .
expr_postfix(x) ::= expr_postfix PERIOD IDENTIFIER .
expr_postfix(x) ::= expr_postfix PERIOD LSQ expr_value RSQ .
expr_postfix(x) ::= expr_postfix LSQ expr_value RSQ .
expr_postfix(x) ::= expr_postfix LPN arg_list RPN .

expr_basic(x)   ::= name .
expr_basic(x)   ::= expr_postfix .
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


expr_value(x)   ::= expr_unary . /* no it doesn't */

expr_value(x)   ::= NEW expr_value /* LPN arg_list RPN */ .
expr_value(x)   ::= COLON expr_value LBR odecl_list RBR .
expr_value(x)   ::= COLON LBR odecl_list RBR .
expr_value(x)   ::= QMARK LPN arg_list RPN LBR stmt_list RBR .
expr_value(x)   ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR .
expr_value(x)   ::= expr_postfix YIELD .


expr_final(x)   ::= expr_value .
expr_final(x)   ::= expr_postfix ELLIPSIS .
expr_final(x)   ::= expr_postfix LSQ RSQ ELLIPSIS .
expr_final(x)   ::= ELLIPSIS .


expr ::= expr_final . /* no it doesn't */
arg_list ::= COMMA .
odecl_list ::= COMMA .
stmt_list ::= COMMA .











