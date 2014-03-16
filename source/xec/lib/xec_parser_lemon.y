

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
%nonassoc FINALLY .
%nonassoc FOR .
%nonassoc IF .
%nonassoc RETURN .
%nonassoc SWITCH .
%nonassoc THROW .
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



something ::= .