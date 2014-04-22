//
//  xec_token.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_token.h"
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <stringf.h>
#include <symbol.h>







std::string xec_token::get_spelling()
{
    switch ( kind )
    {
    case XEC_TOKEN_IDENTIFIER:
        return "string";
        
    case XEC_TOKEN_NUMBER:
        return "number";
        
    case XEC_TOKEN_STRING:
        return stringf( "identifier '%.*s'", (int)size, text );
        
    default:
        return stringf( "'%.*s'", (int)size, text );
    }
}

void xec_token::debug_print()
{
    const char* kind_name = NULL;
    switch ( kind )
    {
    case XEC_TOKEN_EOF:             kind_name = "EOF";              break;
    
    case XEC_KEYWORD_BREAK:         kind_name = "BREAK";            break;
    case XEC_KEYWORD_CASE:          kind_name = "CASE";             break;
    case XEC_KEYWORD_CATCH:         kind_name = "CATCH";            break;
    case XEC_KEYWORD_CONTINUE:      kind_name = "CONTINUE";         break;
    case XEC_KEYWORD_DEFAULT:       kind_name = "DEFAULT";          break;
    case XEC_KEYWORD_DELETE:        kind_name = "DELETE";           break;
    case XEC_KEYWORD_DO:            kind_name = "DO";               break;
    case XEC_KEYWORD_ELSE:          kind_name = "ELSE";             break;
    case XEC_KEYWORD_FALSE:         kind_name = "FALSE";            break;
    case XEC_KEYWORD_FINALLY:       kind_name = "FINALLY";          break;
    case XEC_KEYWORD_FOR:           kind_name = "FOR";              break;
    case XEC_KEYWORD_IF:            kind_name = "IF";               break;
    case XEC_KEYWORD_NEW:           kind_name = "NEW";              break;
    case XEC_KEYWORD_NULL:          kind_name = "NULL";             break;
    case XEC_KEYWORD_RETURN:        kind_name = "RETURN";           break;
    case XEC_KEYWORD_SWITCH:        kind_name = "SWITCH";           break;
    case XEC_KEYWORD_THROW:         kind_name = "THROW";            break;
    case XEC_KEYWORD_TRUE:          kind_name = "TRUE";             break;
    case XEC_KEYWORD_TRY:           kind_name = "TRY";              break;
    case XEC_KEYWORD_USING:         kind_name = "USING";            break;
    case XEC_KEYWORD_VAR:           kind_name = "VAR";              break;
    case XEC_KEYWORD_WHILE:         kind_name = "WHILE";            break;
    case XEC_KEYWORD_YIELD:         kind_name = "YIELD";            break;

    case XEC_TOKEN_IDENTIFIER:      kind_name = "IDENTIFIER";       break;
    case XEC_TOKEN_NUMBER:          kind_name = "NUMBER";           break;
    case XEC_TOKEN_STRING:          kind_name = "STRING";           break;
    
    case XEC_TOKEN_XMARK:           kind_name = "XMARK";            break;
    case XEC_TOKEN_PERCENT:         kind_name = "PERCENT";          break;
    case XEC_TOKEN_AMPERSAND:       kind_name = "AMPERSAND";        break;
    case XEC_TOKEN_LPN:             kind_name = "LPN";              break;
    case XEC_TOKEN_RPN:             kind_name = "RPN";              break;
    case XEC_TOKEN_ASTERISK:        kind_name = "ASTERISK";         break;
    case XEC_TOKEN_PLUS:            kind_name = "PLUS";             break;
    case XEC_TOKEN_COMMA:           kind_name = "COMMA";            break;
    case XEC_TOKEN_MINUS:           kind_name = "MINUS";            break;
    case XEC_TOKEN_PERIOD:          kind_name = "PERIOD";           break;
    case XEC_TOKEN_SOLIDUS:         kind_name = "SOLIDUS";          break;
    case XEC_TOKEN_COLON:           kind_name = "COLON";            break;
    case XEC_TOKEN_SEMICOLON:       kind_name = "SEMICOLON";        break;
    case XEC_TOKEN_LESS:            kind_name = "LESS";             break;
    case XEC_TOKEN_ASSIGN:          kind_name = "ASSIGN";           break;
    case XEC_TOKEN_GREATER:         kind_name = "GREATER";          break;
    case XEC_TOKEN_QMARK:           kind_name = "QMARK";            break;
    case XEC_TOKEN_LSQ:             kind_name = "LSQ";              break;
    case XEC_TOKEN_RSQ:             kind_name = "RSQ";              break;
    case XEC_TOKEN_CARET:           kind_name = "CARET";            break;
    case XEC_TOKEN_LBR:             kind_name = "LBR";              break;
    case XEC_TOKEN_VBAR:            kind_name = "VBAR";             break;
    case XEC_TOKEN_RBR:             kind_name = "RBR";              break;
    case XEC_TOKEN_TILDE:           kind_name = "TILDE";            break;
    
    case XEC_TOKEN_INCREMENT:       kind_name = "INCREMENT";        break;
    case XEC_TOKEN_DECREMENT:       kind_name = "DECREMENT";        break;
    
    case XEC_TOKEN_LSHIFT:          kind_name = "LSHIFT";           break;
    case XEC_TOKEN_RSHIFT:          kind_name = "RSHIFT";           break;
    case XEC_TOKEN_URSHIFT:         kind_name = "URSHIFT";          break;
    
    case XEC_TOKEN_NOTEQUAL:        kind_name = "NOTEQUAL";         break;
    case XEC_TOKEN_LESSEQUAL:       kind_name = "LESSEQUAL";        break;
    case XEC_TOKEN_EQUAL:           kind_name = "EQUAL";            break;
    case XEC_TOKEN_GREATEREQUAL:    kind_name = "GREATEREQUAL";     break;
    case XEC_TOKEN_IN:              kind_name = "IN";               break;
    case XEC_TOKEN_NOTIN:           kind_name = "NOTIN";            break;
    case XEC_TOKEN_IS:              kind_name = "IS";               break;
    case XEC_TOKEN_NOTIS:           kind_name = "NOTIS";            break;
    
    case XEC_TOKEN_MODASSIGN:       kind_name = "MODASSIGN";        break;
    case XEC_TOKEN_BITANDASSIGN:    kind_name = "BITANDASSIGN";     break;
    case XEC_TOKEN_MULASSIGN:       kind_name = "MULASSIGN";        break;
    case XEC_TOKEN_ADDASSIGN:       kind_name = "ADDASSIGN";        break;
    case XEC_TOKEN_SUBASSIGN:       kind_name = "SUBASSIGN";        break;
    case XEC_TOKEN_DIVASSIGN:       kind_name = "DIVASSIGN";        break;
    case XEC_TOKEN_BITXORASSIGN:    kind_name = "BITXORASSIGN";     break;
    case XEC_TOKEN_BITORASSIGN:     kind_name = "BITORASSIGN";      break;
    case XEC_TOKEN_INTDIVASSIGN:    kind_name = "INTDIVASSIGN";     break;
    case XEC_TOKEN_LSHIFTASSIGN:    kind_name = "LSHIFTASSIGN";     break;
    case XEC_TOKEN_RSHIFTASSIGN:    kind_name = "RSHIFTASSIGN";     break;
    case XEC_TOKEN_URSHIFTASSIGN:   kind_name = "URSHIFTASSIGN";    break;
    
    case XEC_TOKEN_LOGICAND:        kind_name = "LOGICAND";         break;
    case XEC_TOKEN_LOGICXOR:        kind_name = "LOGICXOR";         break;
    case XEC_TOKEN_LOGICOR:         kind_name = "LOGICOR";          break;
    
    case XEC_TOKEN_EACHKEY:         kind_name = "EACHKEY";          break;
    case XEC_TOKEN_ELLIPSIS:        kind_name = "ELLIPSIS";         break;
    }
    
    printf( "%s '%.*s'\n", kind_name, (int)size, text );
}


