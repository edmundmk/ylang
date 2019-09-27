//
//  token.h
//
//  Created by Edmund Kapusniak on 14/03/2014.
//  Copyright © 2014 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "diagnostics.h"

namespace yl
{

enum token_kind
{
    TOKEN_EOF            = 0,

/*
#include "yl_parser_lemon.h"

    KEYWORD_BREAK        = LEMON_BREAK,
    KEYWORD_CASE         = LEMON_CASE,
    KEYWORD_CATCH        = LEMON_CATCH,
    KEYWORD_CONTINUE     = LEMON_CONTINUE,
    KEYWORD_DEF          = LEMON_DEF,
    KEYWORD_DEFAULT      = LEMON_DEFAULT,
    KEYWORD_DELETE       = LEMON_DELETE,
    KEYWORD_DO           = LEMON_DO,
    KEYWORD_ELSE         = LEMON_ELSE,
    KEYWORD_FALSE        = LEMON_FALSE,
    KEYWORD_FINALLY      = LEMON_FINALLY,
    KEYWORD_FOR          = LEMON_FOR,
    KEYWORD_IF           = LEMON_IF,
    KEYWORD_NEW          = LEMON_NEW,
    KEYWORD_NULL         = LEMON_NULL,
    KEYWORD_RETURN       = LEMON_RETURN,
    KEYWORD_SUPEROF      = LEMON_SUPEROF,
    KEYWORD_SWITCH       = LEMON_SWITCH,
    KEYWORD_THROW        = LEMON_THROW,
    KEYWORD_TRUE         = LEMON_TRUE,
    KEYWORD_TRY          = LEMON_TRY,
    KEYWORD_USING        = LEMON_USING,
    KEYWORD_VAR          = LEMON_VAR,
    KEYWORD_WHILE        = LEMON_WHILE,
    KEYWORD_YIELD        = LEMON_YIELD,

    TOKEN_IDENTIFIER     = LEMON_IDENTIFIER,
    TOKEN_NUMBER         = LEMON_NUMBER,
    TOKEN_STRING         = LEMON_STRING,

    TOKEN_XMARK          = LEMON_XMARK,
    TOKEN_PERCENT        = LEMON_PERCENT,
    TOKEN_AMPERSAND      = LEMON_AMPERSAND,
    TOKEN_LPN            = LEMON_LPN,
    TOKEN_RPN            = LEMON_RPN,
    TOKEN_ASTERISK       = LEMON_ASTERISK,
    TOKEN_PLUS           = LEMON_PLUS,
    TOKEN_COMMA          = LEMON_COMMA,
    TOKEN_MINUS          = LEMON_MINUS,
    TOKEN_PERIOD         = LEMON_PERIOD,
    TOKEN_SOLIDUS        = LEMON_SOLIDUS,
    TOKEN_COLON          = LEMON_COLON,
    TOKEN_SEMICOLON      = LEMON_SEMICOLON,
    TOKEN_LESS           = LEMON_LESS,
    TOKEN_ASSIGN         = LEMON_ASSIGN,
    TOKEN_GREATER        = LEMON_GREATER,
    TOKEN_QMARK          = LEMON_QMARK,
    TOKEN_LSQ            = LEMON_LSQ,
    TOKEN_RSQ            = LEMON_RSQ,
    TOKEN_CARET          = LEMON_CARET,
    TOKEN_LBR            = LEMON_LBR,
    TOKEN_VBAR           = LEMON_VBAR,
    TOKEN_RBR            = LEMON_RBR,
    TOKEN_TILDE          = LEMON_TILDE,

    TOKEN_INCREMENT      = LEMON_INCREMENT,
    TOKEN_DECREMENT      = LEMON_DECREMENT,

    TOKEN_CONCATENATE    = LEMON_CONCATENATE,

    TOKEN_LSHIFT         = LEMON_LSHIFT,
    TOKEN_LRSHIFT        = LEMON_LRSHIFT,
    TOKEN_ARSHIFT        = LEMON_ARSHIFT,

    TOKEN_NOTEQUAL       = LEMON_NOTEQUAL,
    TOKEN_LESSEQUAL      = LEMON_LESSEQUAL,
    TOKEN_EQUAL          = LEMON_EQUAL,
    TOKEN_GREATEREQUAL   = LEMON_GREATEREQUAL,
    TOKEN_IS             = LEMON_IS,
    TOKEN_NOTIS          = LEMON_NOTIS,

    TOKEN_MODASSIGN      = LEMON_MODASSIGN,
    TOKEN_BITANDASSIGN   = LEMON_BITANDASSIGN,
    TOKEN_MULASSIGN      = LEMON_MULASSIGN,
    TOKEN_ADDASSIGN      = LEMON_ADDASSIGN,
    TOKEN_SUBASSIGN      = LEMON_SUBASSIGN,
    TOKEN_DIVASSIGN      = LEMON_DIVASSIGN,
    TOKEN_BITXORASSIGN   = LEMON_BITXORASSIGN,
    TOKEN_BITORASSIGN    = LEMON_BITORASSIGN,
    TOKEN_INTDIVASSIGN   = LEMON_INTDIVASSIGN,
    TOKEN_LSHIFTASSIGN   = LEMON_LSHIFTASSIGN,
    TOKEN_LRSHIFTASSIGN  = LEMON_LRSHIFTASSIGN,
    TOKEN_ARSHIFTASSIGN  = LEMON_ARSHIFTASSIGN,

    TOKEN_LOGICAND       = LEMON_LOGICAND,
    TOKEN_LOGICXOR       = LEMON_LOGICXOR,
    TOKEN_LOGICOR        = LEMON_LOGICOR,

    TOKEN_EACHKEY        = LEMON_EACHKEY,
    TOKEN_HASKEY         = LEMON_HASKEY,
    TOKEN_ELLIPSIS       = LEMON_ELLIPSIS,

#undef LEMON_BREAK
#undef LEMON_CASE
#undef LEMON_CATCH
#undef LEMON_CONTINUE
#undef LEMON_DEFAULT
#undef LEMON_DELETE
#undef LEMON_DO
#undef LEMON_ELSE
#undef LEMON_FALSE
#undef LEMON_FINALLY
#undef LEMON_FOR
#undef LEMON_IF
#undef LEMON_NEW
#undef LEMON_nullptr
#undef LEMON_RETURN
#undef LEMON_SWITCH
#undef LEMON_THROW
#undef LEMON_TRUE
#undef LEMON_TRY
#undef LEMON_USING
#undef LEMON_VAR
#undef LEMON_WHILE
#undef LEMON_YIELD

#undef LEMON_IDENTIFIER
#undef LEMON_NUMBER
#undef LEMON_STRING

#undef LEMON_XMARK
#undef LEMON_PERCENT
#undef LEMON_AMPERSAND
#undef LEMON_LPN
#undef LEMON_RPN
#undef LEMON_ASTERISK
#undef LEMON_PLUS
#undef LEMON_COMMA
#undef LEMON_MINUS
#undef LEMON_PERIOD
#undef LEMON_SOLIDUS
#undef LEMON_COLON
#undef LEMON_SEMICOLON
#undef LEMON_LESS
#undef LEMON_ASSIGN
#undef LEMON_GREATER
#undef LEMON_QMARK
#undef LEMON_LSQ
#undef LEMON_RSQ
#undef LEMON_CARET
#undef LEMON_LBR
#undef LEMON_VBAR
#undef LEMON_RBR
#undef LEMON_TILDE

#undef LEMON_CONCATENATE

#undef LEMON_RESPONDS

#undef LEMON_INCREMENT
#undef LEMON_DECREMENT

#undef LEMON_LSHIFT
#undef LEMON_RSHIFT
#undef LEMON_URSHIFT

#undef LEMON_NOTEQUAL
#undef LEMON_LESSEQUAL
#undef LEMON_EQUAL
#undef LEMON_GREATEREQUAL
#undef LEMON_IS
#undef LEMON_NOTIS

#undef LEMON_MODASSIGN
#undef LEMON_BITANDASSIGN
#undef LEMON_MULASSIGN
#undef LEMON_ADDASSIGN
#undef LEMON_SUBASSIGN
#undef LEMON_DIVASSIGN
#undef LEMON_BITXORASSIGN
#undef LEMON_BITORASSIGN
#undef LEMON_INTDIVASSIGN
#undef LEMON_LSHIFTASSIGN
#undef LEMON_RSHIFTASSIGN
#undef LEMON_URSHIFTASSIGN

#undef LEMON_LOGICAND
#undef LEMON_LOGICXOR
#undef LEMON_LOGICOR

#undef LEMON_EACHKEY
#undef LEMON_ELLIPSIS
*/
};

struct token
{
    token( token_kind kind, srcloc sloc, const char* text, size_t size );

    std::string     get_spelling();
    void            debug_print();

    token_kind      kind;
    int             sloc;
    const char*     text;
    size_t          size;
};

inline token::token( token_kind kind, srcloc sloc, const char* text, size_t size )
    :   kind( kind )
    ,   sloc( sloc )
    ,   text( text )
    ,   size( size )
{
}

}

#endif

