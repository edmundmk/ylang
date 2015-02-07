//
//  yl_token.h
//
//  Created by Edmund Kapusniak on 14/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_TOKEN_H
#define YL_TOKEN_H


#include <utility>
#include <string>
#include <hash.h>
#include <assert.h>


enum yl_token_kind
{
    YL_TOKEN_EOF           = 0,
    
#include "yl_parser_lemon.h"

    YL_KEYWORD_BREAK       = LEMON_BREAK,
    YL_KEYWORD_CASE        = LEMON_CASE,
    YL_KEYWORD_CATCH       = LEMON_CATCH,
    YL_KEYWORD_CONTINUE    = LEMON_CONTINUE,
    YL_KEYWORD_DEF         = LEMON_DEF,
    YL_KEYWORD_DEFAULT     = LEMON_DEFAULT,
    YL_KEYWORD_DELETE      = LEMON_DELETE,
    YL_KEYWORD_DO          = LEMON_DO,
    YL_KEYWORD_ELSE        = LEMON_ELSE,
    YL_KEYWORD_FALSE       = LEMON_FALSE,
    YL_KEYWORD_FINALLY     = LEMON_FINALLY,
    YL_KEYWORD_FOR         = LEMON_FOR,
    YL_KEYWORD_IF          = LEMON_IF,
    YL_KEYWORD_NEW         = LEMON_NEW,
    YL_KEYWORD_NULL        = LEMON_NULL,
    YL_KEYWORD_RETURN      = LEMON_RETURN,
    YL_KEYWORD_SUPEROF     = LEMON_SUPEROF,
    YL_KEYWORD_SWITCH      = LEMON_SWITCH,
    YL_KEYWORD_THROW       = LEMON_THROW,
    YL_KEYWORD_TRUE        = LEMON_TRUE,
    YL_KEYWORD_TRY         = LEMON_TRY,
    YL_KEYWORD_USING       = LEMON_USING,
    YL_KEYWORD_VAR         = LEMON_VAR,
    YL_KEYWORD_WHILE       = LEMON_WHILE,
    YL_KEYWORD_YIELD       = LEMON_YIELD,

    YL_TOKEN_IDENTIFIER    = LEMON_IDENTIFIER,
    YL_TOKEN_NUMBER        = LEMON_NUMBER,
    YL_TOKEN_STRING        = LEMON_STRING,
    
    YL_TOKEN_XMARK         = LEMON_XMARK,
    YL_TOKEN_PERCENT       = LEMON_PERCENT,
    YL_TOKEN_AMPERSAND     = LEMON_AMPERSAND,
    YL_TOKEN_LPN           = LEMON_LPN,
    YL_TOKEN_RPN           = LEMON_RPN,
    YL_TOKEN_ASTERISK      = LEMON_ASTERISK,
    YL_TOKEN_PLUS          = LEMON_PLUS,
    YL_TOKEN_COMMA         = LEMON_COMMA,
    YL_TOKEN_MINUS         = LEMON_MINUS,
    YL_TOKEN_PERIOD        = LEMON_PERIOD,
    YL_TOKEN_SOLIDUS       = LEMON_SOLIDUS,
    YL_TOKEN_COLON         = LEMON_COLON,
    YL_TOKEN_SEMICOLON     = LEMON_SEMICOLON,
    YL_TOKEN_LESS          = LEMON_LESS,
    YL_TOKEN_ASSIGN        = LEMON_ASSIGN,
    YL_TOKEN_GREATER       = LEMON_GREATER,
    YL_TOKEN_QMARK         = LEMON_QMARK,
    YL_TOKEN_LSQ           = LEMON_LSQ,
    YL_TOKEN_RSQ           = LEMON_RSQ,
    YL_TOKEN_CARET         = LEMON_CARET,
    YL_TOKEN_LBR           = LEMON_LBR,
    YL_TOKEN_VBAR          = LEMON_VBAR,
    YL_TOKEN_RBR           = LEMON_RBR,
    YL_TOKEN_TILDE         = LEMON_TILDE,
    
    YL_TOKEN_INCREMENT     = LEMON_INCREMENT,
    YL_TOKEN_DECREMENT     = LEMON_DECREMENT,
    
    YL_TOKEN_CONCATENATE   = LEMON_CONCATENATE,
    
    YL_TOKEN_LSHIFT        = LEMON_LSHIFT,
    YL_TOKEN_LRSHIFT       = LEMON_LRSHIFT,
    YL_TOKEN_ARSHIFT       = LEMON_ARSHIFT,
    
    YL_TOKEN_NOTEQUAL      = LEMON_NOTEQUAL,
    YL_TOKEN_LESSEQUAL     = LEMON_LESSEQUAL,
    YL_TOKEN_EQUAL         = LEMON_EQUAL,
    YL_TOKEN_GREATEREQUAL  = LEMON_GREATEREQUAL,
    YL_TOKEN_IN            = LEMON_IN,
    YL_TOKEN_NOTIN         = LEMON_NOTIN,
    YL_TOKEN_IS            = LEMON_IS,
    YL_TOKEN_NOTIS         = LEMON_NOTIS,
    
    YL_TOKEN_MODASSIGN     = LEMON_MODASSIGN,
    YL_TOKEN_BITANDASSIGN  = LEMON_BITANDASSIGN,
    YL_TOKEN_MULASSIGN     = LEMON_MULASSIGN,
    YL_TOKEN_ADDASSIGN     = LEMON_ADDASSIGN,
    YL_TOKEN_SUBASSIGN     = LEMON_SUBASSIGN,
    YL_TOKEN_DIVASSIGN     = LEMON_DIVASSIGN,
    YL_TOKEN_BITXORASSIGN  = LEMON_BITXORASSIGN,
    YL_TOKEN_BITORASSIGN   = LEMON_BITORASSIGN,
    YL_TOKEN_INTDIVASSIGN  = LEMON_INTDIVASSIGN,
    YL_TOKEN_LSHIFTASSIGN  = LEMON_LSHIFTASSIGN,
    YL_TOKEN_LRSHIFTASSIGN = LEMON_LRSHIFTASSIGN,
    YL_TOKEN_ARSHIFTASSIGN = LEMON_ARSHIFTASSIGN,
    
    YL_TOKEN_LOGICAND      = LEMON_LOGICAND,
    YL_TOKEN_LOGICXOR      = LEMON_LOGICXOR,
    YL_TOKEN_LOGICOR       = LEMON_LOGICOR,
    
    YL_TOKEN_EACHKEY       = LEMON_EACHKEY,
    YL_TOKEN_ELLIPSIS      = LEMON_ELLIPSIS,

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

#undef LEMON_INCREMENT
#undef LEMON_DECREMENT
    
#undef LEMON_LSHIFT
#undef LEMON_RSHIFT
#undef LEMON_URSHIFT
    
#undef LEMON_NOTEQUAL
#undef LEMON_LESSEQUAL
#undef LEMON_EQUAL
#undef LEMON_GREATEREQUAL
#undef LEMON_IN
#undef LEMON_NOTIN
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
};




struct yl_token
{
    yl_token( yl_token_kind kind, int sloc, const char* text, size_t size );

    std::string     get_spelling();
    void            debug_print();

    yl_token_kind  kind;
    int             sloc;
    const char*     text;
    size_t          size;
};



inline yl_token::yl_token(
            yl_token_kind kind, int sloc, const char* text, size_t size )
    :   kind( kind )
    ,   sloc( sloc )
    ,   text( text )
    ,   size( size )
{
}



#endif
