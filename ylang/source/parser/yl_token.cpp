//
//  yl_token.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_token.h"
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <stringf.h>
#include <symkey.h>







std::string yl_token::get_spelling()
{
    switch ( kind )
    {
    case YL_TOKEN_STRING:
        return "string";
        
    case YL_TOKEN_NUMBER:
        return "number";
        
    case YL_TOKEN_IDENTIFIER:
        return stringf( "identifier '%.*s'", (int)size, text );
        
    default:
        return stringf( "'%.*s'", (int)size, text );
    }
}

void yl_token::debug_print()
{
    const char* kind_name = nullptr;
    switch ( kind )
    {
    case YL_TOKEN_EOF:              kind_name = "EOF";              break;
    
    case YL_KEYWORD_BREAK:          kind_name = "BREAK";            break;
    case YL_KEYWORD_CASE:           kind_name = "CASE";             break;
    case YL_KEYWORD_CATCH:          kind_name = "CATCH";            break;
    case YL_KEYWORD_CONTINUE:       kind_name = "CONTINUE";         break;
    case YL_KEYWORD_DEF:            kind_name = "DEF";              break;
    case YL_KEYWORD_DEFAULT:        kind_name = "DEFAULT";          break;
    case YL_KEYWORD_DELETE:         kind_name = "DELETE";           break;
    case YL_KEYWORD_DO:             kind_name = "DO";               break;
    case YL_KEYWORD_ELSE:           kind_name = "ELSE";             break;
    case YL_KEYWORD_FALSE:          kind_name = "FALSE";            break;
    case YL_KEYWORD_FINALLY:        kind_name = "FINALLY";          break;
    case YL_KEYWORD_FOR:            kind_name = "FOR";              break;
    case YL_KEYWORD_IF:             kind_name = "IF";               break;
    case YL_KEYWORD_NEW:            kind_name = "NEW";              break;
    case YL_KEYWORD_NULL:           kind_name = "NULL";             break;
    case YL_KEYWORD_RETURN:         kind_name = "RETURN";           break;
    case YL_KEYWORD_SUPEROF:        kind_name = "SUPEROF";          break;
    case YL_KEYWORD_SWITCH:         kind_name = "SWITCH";           break;
    case YL_KEYWORD_THROW:          kind_name = "THROW";            break;
    case YL_KEYWORD_TRUE:           kind_name = "TRUE";             break;
    case YL_KEYWORD_TRY:            kind_name = "TRY";              break;
    case YL_KEYWORD_USING:          kind_name = "USING";            break;
    case YL_KEYWORD_VAR:            kind_name = "VAR";              break;
    case YL_KEYWORD_WHILE:          kind_name = "WHILE";            break;
    case YL_KEYWORD_YIELD:          kind_name = "YIELD";            break;

    case YL_TOKEN_IDENTIFIER:       kind_name = "IDENTIFIER";       break;
    case YL_TOKEN_NUMBER:           kind_name = "NUMBER";           break;
    case YL_TOKEN_STRING:           kind_name = "STRING";           break;
    
    case YL_TOKEN_XMARK:            kind_name = "XMARK";            break;
    case YL_TOKEN_PERCENT:          kind_name = "PERCENT";          break;
    case YL_TOKEN_AMPERSAND:        kind_name = "AMPERSAND";        break;
    case YL_TOKEN_LPN:              kind_name = "LPN";              break;
    case YL_TOKEN_RPN:              kind_name = "RPN";              break;
    case YL_TOKEN_ASTERISK:         kind_name = "ASTERISK";         break;
    case YL_TOKEN_PLUS:             kind_name = "PLUS";             break;
    case YL_TOKEN_COMMA:            kind_name = "COMMA";            break;
    case YL_TOKEN_MINUS:            kind_name = "MINUS";            break;
    case YL_TOKEN_PERIOD:           kind_name = "PERIOD";           break;
    case YL_TOKEN_SOLIDUS:          kind_name = "SOLIDUS";          break;
    case YL_TOKEN_COLON:            kind_name = "COLON";            break;
    case YL_TOKEN_SEMICOLON:        kind_name = "SEMICOLON";        break;
    case YL_TOKEN_LESS:             kind_name = "LESS";             break;
    case YL_TOKEN_ASSIGN:           kind_name = "ASSIGN";           break;
    case YL_TOKEN_GREATER:          kind_name = "GREATER";          break;
    case YL_TOKEN_QMARK:            kind_name = "QMARK";            break;
    case YL_TOKEN_LSQ:              kind_name = "LSQ";              break;
    case YL_TOKEN_RSQ:              kind_name = "RSQ";              break;
    case YL_TOKEN_CARET:            kind_name = "CARET";            break;
    case YL_TOKEN_LBR:              kind_name = "LBR";              break;
    case YL_TOKEN_VBAR:             kind_name = "VBAR";             break;
    case YL_TOKEN_RBR:              kind_name = "RBR";              break;
    case YL_TOKEN_TILDE:            kind_name = "TILDE";            break;
    
    case YL_TOKEN_INCREMENT:        kind_name = "INCREMENT";        break;
    case YL_TOKEN_DECREMENT:        kind_name = "DECREMENT";        break;
    
    case YL_TOKEN_CONCATENATE:      kind_name = "CONCATENATE";      break;

    case YL_TOKEN_LSHIFT:           kind_name = "LSHIFT";           break;
    case YL_TOKEN_LRSHIFT:          kind_name = "LRSHIFT";          break;
    case YL_TOKEN_ARSHIFT:          kind_name = "ARSHIFT";          break;
    
    case YL_TOKEN_NOTEQUAL:         kind_name = "NOTEQUAL";         break;
    case YL_TOKEN_LESSEQUAL:        kind_name = "LESSEQUAL";        break;
    case YL_TOKEN_EQUAL:            kind_name = "EQUAL";            break;
    case YL_TOKEN_GREATEREQUAL:     kind_name = "GREATEREQUAL";     break;
    case YL_TOKEN_IS:               kind_name = "IS";               break;
    case YL_TOKEN_NOTIS:            kind_name = "NOTIS";            break;
    
    case YL_TOKEN_MODASSIGN:        kind_name = "MODASSIGN";        break;
    case YL_TOKEN_BITANDASSIGN:     kind_name = "BITANDASSIGN";     break;
    case YL_TOKEN_MULASSIGN:        kind_name = "MULASSIGN";        break;
    case YL_TOKEN_ADDASSIGN:        kind_name = "ADDASSIGN";        break;
    case YL_TOKEN_SUBASSIGN:        kind_name = "SUBASSIGN";        break;
    case YL_TOKEN_DIVASSIGN:        kind_name = "DIVASSIGN";        break;
    case YL_TOKEN_BITXORASSIGN:     kind_name = "BITXORASSIGN";     break;
    case YL_TOKEN_BITORASSIGN:      kind_name = "BITORASSIGN";      break;
    case YL_TOKEN_INTDIVASSIGN:     kind_name = "INTDIVASSIGN";     break;
    case YL_TOKEN_LSHIFTASSIGN:     kind_name = "LSHIFTASSIGN";     break;
    case YL_TOKEN_LRSHIFTASSIGN:    kind_name = "LRSHIFTASSIGN";    break;
    case YL_TOKEN_ARSHIFTASSIGN:    kind_name = "ARSHIFTASSIGN";    break;
    
    case YL_TOKEN_LOGICAND:         kind_name = "LOGICAND";         break;
    case YL_TOKEN_LOGICXOR:         kind_name = "LOGICXOR";         break;
    case YL_TOKEN_LOGICOR:          kind_name = "LOGICOR";          break;
    
    case YL_TOKEN_EACHKEY:          kind_name = "EACHKEY";          break;
    case YL_TOKEN_HASKEY:           kind_name = "HASKEY";           break;
    case YL_TOKEN_ELLIPSIS:         kind_name = "ELLIPSIS";         break;
    }
    
    printf( "%s '%.*s'\n", kind_name, (int)size, text );
}


