//
//  token.cpp
//
//  Created by Edmund Kapusniak on 15/03/2014.
//  Copyright © 2014-2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "token.h"

namespace yl
{

std::string token::get_spelling()
{
    std::string spelling;

    switch ( kind )
    {
    /*
    case TOKEN_STRING:
        spelling += "string";
        break;

    case TOKEN_NUMBER:
        spelling += "number";
        break;

    case TOKEN_IDENTIFIER:
        spelling += "identifier ";
        // [fallthrough]
*/
    default:
        spelling += "'";
        spelling += std::string_view( text, size );
        spelling += "'";
        break;
    }

    return spelling;
}

void token::debug_print()
{
    const char* kind_name = nullptr;
    switch ( kind )
    {
    case TOKEN_EOF:              kind_name = "EOF";              break;
/*
    case KEYWORD_BREAK:          kind_name = "BREAK";            break;
    case KEYWORD_CASE:           kind_name = "CASE";             break;
    case KEYWORD_CATCH:          kind_name = "CATCH";            break;
    case KEYWORD_CONTINUE:       kind_name = "CONTINUE";         break;
    case KEYWORD_DEF:            kind_name = "DEF";              break;
    case KEYWORD_DEFAULT:        kind_name = "DEFAULT";          break;
    case KEYWORD_DELETE:         kind_name = "DELETE";           break;
    case KEYWORD_DO:             kind_name = "DO";               break;
    case KEYWORD_ELSE:           kind_name = "ELSE";             break;
    case KEYWORD_FALSE:          kind_name = "FALSE";            break;
    case KEYWORD_FINALLY:        kind_name = "FINALLY";          break;
    case KEYWORD_FOR:            kind_name = "FOR";              break;
    case KEYWORD_IF:             kind_name = "IF";               break;
    case KEYWORD_NEW:            kind_name = "NEW";              break;
    case KEYWORD_NULL:           kind_name = "NULL";             break;
    case KEYWORD_RETURN:         kind_name = "RETURN";           break;
    case KEYWORD_SUPEROF:        kind_name = "SUPEROF";          break;
    case KEYWORD_SWITCH:         kind_name = "SWITCH";           break;
    case KEYWORD_THROW:          kind_name = "THROW";            break;
    case KEYWORD_TRUE:           kind_name = "TRUE";             break;
    case KEYWORD_TRY:            kind_name = "TRY";              break;
    case KEYWORD_USING:          kind_name = "USING";            break;
    case KEYWORD_VAR:            kind_name = "VAR";              break;
    case KEYWORD_WHILE:          kind_name = "WHILE";            break;
    case KEYWORD_YIELD:          kind_name = "YIELD";            break;

    case TOKEN_IDENTIFIER:       kind_name = "IDENTIFIER";       break;
    case TOKEN_NUMBER:           kind_name = "NUMBER";           break;
    case TOKEN_STRING:           kind_name = "STRING";           break;

    case TOKEN_XMARK:            kind_name = "XMARK";            break;
    case TOKEN_PERCENT:          kind_name = "PERCENT";          break;
    case TOKEN_AMPERSAND:        kind_name = "AMPERSAND";        break;
    case TOKEN_LPN:              kind_name = "LPN";              break;
    case TOKEN_RPN:              kind_name = "RPN";              break;
    case TOKEN_ASTERISK:         kind_name = "ASTERISK";         break;
    case TOKEN_PLUS:             kind_name = "PLUS";             break;
    case TOKEN_COMMA:            kind_name = "COMMA";            break;
    case TOKEN_MINUS:            kind_name = "MINUS";            break;
    case TOKEN_PERIOD:           kind_name = "PERIOD";           break;
    case TOKEN_SOLIDUS:          kind_name = "SOLIDUS";          break;
    case TOKEN_COLON:            kind_name = "COLON";            break;
    case TOKEN_SEMICOLON:        kind_name = "SEMICOLON";        break;
    case TOKEN_LESS:             kind_name = "LESS";             break;
    case TOKEN_ASSIGN:           kind_name = "ASSIGN";           break;
    case TOKEN_GREATER:          kind_name = "GREATER";          break;
    case TOKEN_QMARK:            kind_name = "QMARK";            break;
    case TOKEN_LSQ:              kind_name = "LSQ";              break;
    case TOKEN_RSQ:              kind_name = "RSQ";              break;
    case TOKEN_CARET:            kind_name = "CARET";            break;
    case TOKEN_LBR:              kind_name = "LBR";              break;
    case TOKEN_VBAR:             kind_name = "VBAR";             break;
    case TOKEN_RBR:              kind_name = "RBR";              break;
    case TOKEN_TILDE:            kind_name = "TILDE";            break;

    case TOKEN_INCREMENT:        kind_name = "INCREMENT";        break;
    case TOKEN_DECREMENT:        kind_name = "DECREMENT";        break;

    case TOKEN_CONCATENATE:      kind_name = "CONCATENATE";      break;

    case TOKEN_LSHIFT:           kind_name = "LSHIFT";           break;
    case TOKEN_LRSHIFT:          kind_name = "LRSHIFT";          break;
    case TOKEN_ARSHIFT:          kind_name = "ARSHIFT";          break;

    case TOKEN_NOTEQUAL:         kind_name = "NOTEQUAL";         break;
    case TOKEN_LESSEQUAL:        kind_name = "LESSEQUAL";        break;
    case TOKEN_EQUAL:            kind_name = "EQUAL";            break;
    case TOKEN_GREATEREQUAL:     kind_name = "GREATEREQUAL";     break;
    case TOKEN_IS:               kind_name = "IS";               break;
    case TOKEN_NOTIS:            kind_name = "NOTIS";            break;

    case TOKEN_MODASSIGN:        kind_name = "MODASSIGN";        break;
    case TOKEN_BITANDASSIGN:     kind_name = "BITANDASSIGN";     break;
    case TOKEN_MULASSIGN:        kind_name = "MULASSIGN";        break;
    case TOKEN_ADDASSIGN:        kind_name = "ADDASSIGN";        break;
    case TOKEN_SUBASSIGN:        kind_name = "SUBASSIGN";        break;
    case TOKEN_DIVASSIGN:        kind_name = "DIVASSIGN";        break;
    case TOKEN_BITXORASSIGN:     kind_name = "BITXORASSIGN";     break;
    case TOKEN_BITORASSIGN:      kind_name = "BITORASSIGN";      break;
    case TOKEN_INTDIVASSIGN:     kind_name = "INTDIVASSIGN";     break;
    case TOKEN_LSHIFTASSIGN:     kind_name = "LSHIFTASSIGN";     break;
    case TOKEN_LRSHIFTASSIGN:    kind_name = "LRSHIFTASSIGN";    break;
    case TOKEN_ARSHIFTASSIGN:    kind_name = "ARSHIFTASSIGN";    break;

    case TOKEN_LOGICAND:         kind_name = "LOGICAND";         break;
    case TOKEN_LOGICXOR:         kind_name = "LOGICXOR";         break;
    case TOKEN_LOGICOR:          kind_name = "LOGICOR";          break;

    case TOKEN_EACHKEY:          kind_name = "EACHKEY";          break;
    case TOKEN_HASKEY:           kind_name = "HASKEY";           break;
    case TOKEN_ELLIPSIS:         kind_name = "ELLIPSIS";         break;
*/    }

    printf( "%s '%.*s'\n", kind_name, (int)size, text );
}

}

