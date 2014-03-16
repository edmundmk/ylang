//
//  xec_token.h
//
//  Created by Edmund Kapusniak on 14/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_TOKEN_H
#define XEC_TOKEN_H


#include <utility>
#include <string>
#include <hash.h>
#include <assert.h>


enum xec_token_kind
{
    XEC_TOKEN_EOF           = 0,
    
#include "xec_parser_lemon.h"

    XEC_KEYWORD_BREAK       = LEMON_BREAK,
    XEC_KEYWORD_CASE        = LEMON_CASE,
    XEC_KEYWORD_CATCH       = LEMON_CATCH,
    XEC_KEYWORD_CONTINUE    = LEMON_CONTINUE,
    XEC_KEYWORD_DEFAULT     = LEMON_DEFAULT,
    XEC_KEYWORD_DO          = LEMON_DO,
    XEC_KEYWORD_ELSE        = LEMON_ELSE,
    XEC_KEYWORD_FINALLY     = LEMON_FINALLY,
    XEC_KEYWORD_FOR         = LEMON_FOR,
    XEC_KEYWORD_IF          = LEMON_IF,
    XEC_KEYWORD_RETURN      = LEMON_RETURN,
    XEC_KEYWORD_SWITCH      = LEMON_SWITCH,
    XEC_KEYWORD_THROW       = LEMON_THROW,
    XEC_KEYWORD_TRY         = LEMON_TRY,
    XEC_KEYWORD_USING       = LEMON_USING,
    XEC_KEYWORD_VAR         = LEMON_VAR,
    XEC_KEYWORD_WHILE       = LEMON_WHILE,
    XEC_KEYWORD_YIELD       = LEMON_YIELD,

    XEC_TOKEN_IDENTIFIER    = LEMON_IDENTIFIER,
    XEC_TOKEN_NUMBER        = LEMON_NUMBER,
    XEC_TOKEN_STRING        = LEMON_STRING,
    
    XEC_TOKEN_XMARK         = LEMON_XMARK,
    XEC_TOKEN_PERCENT       = LEMON_PERCENT,
    XEC_TOKEN_AMPERSAND     = LEMON_AMPERSAND,
    XEC_TOKEN_LPN           = LEMON_LPN,
    XEC_TOKEN_RPN           = LEMON_RPN,
    XEC_TOKEN_ASTERISK      = LEMON_ASTERISK,
    XEC_TOKEN_PLUS          = LEMON_PLUS,
    XEC_TOKEN_MINUS         = LEMON_MINUS,
    XEC_TOKEN_PERIOD        = LEMON_PERIOD,
    XEC_TOKEN_SOLIDUS       = LEMON_SOLIDUS,
    XEC_TOKEN_COLON         = LEMON_COLON,
    XEC_TOKEN_SEMICOLON     = LEMON_SEMICOLON,
    XEC_TOKEN_LESS          = LEMON_LESS,
    XEC_TOKEN_ASSIGN        = LEMON_ASSIGN,
    XEC_TOKEN_GREATER       = LEMON_GREATER,
    XEC_TOKEN_QMARK         = LEMON_QMARK,
    XEC_TOKEN_LSQ           = LEMON_LSQ,
    XEC_TOKEN_RSQ           = LEMON_RSQ,
    XEC_TOKEN_CARET         = LEMON_CARET,
    XEC_TOKEN_LBR           = LEMON_LBR,
    XEC_TOKEN_VBAR          = LEMON_VBAR,
    XEC_TOKEN_RBR           = LEMON_RBR,
    XEC_TOKEN_TILDE         = LEMON_TILDE,
    
    XEC_TOKEN_INCREMENT     = LEMON_INCREMENT,
    XEC_TOKEN_DECREMENT     = LEMON_DECREMENT,
    
    XEC_TOKEN_LSHIFT        = LEMON_LSHIFT,
    XEC_TOKEN_RSHIFT        = LEMON_RSHIFT,
    XEC_TOKEN_URSHIFT       = LEMON_URSHIFT,
    
    XEC_TOKEN_NOTEQUAL      = LEMON_NOTEQUAL,
    XEC_TOKEN_LESSEQUAL     = LEMON_LESSEQUAL,
    XEC_TOKEN_EQUAL         = LEMON_EQUAL,
    XEC_TOKEN_GREATEREQUAL  = LEMON_GREATEREQUAL,
    
    XEC_TOKEN_MODASSIGN     = LEMON_MODASSIGN,
    XEC_TOKEN_BITANDASSIGN  = LEMON_BITANDASSIGN,
    XEC_TOKEN_MULASSIGN     = LEMON_MULASSIGN,
    XEC_TOKEN_ADDASSIGN     = LEMON_ADDASSIGN,
    XEC_TOKEN_SUBASSIGN     = LEMON_SUBASSIGN,
    XEC_TOKEN_DIVASSIGN     = LEMON_DIVASSIGN,
    XEC_TOKEN_BITXORASSIGN  = LEMON_BITXORASSIGN,
    XEC_TOKEN_BITORASSIGN   = LEMON_BITORASSIGN,
    XEC_TOKEN_INTDIVASSIGN  = LEMON_INTDIVASSIGN,
    XEC_TOKEN_LSHIFTASSIGN  = LEMON_LSHIFTASSIGN,
    XEC_TOKEN_RSHIFTASSIGN  = LEMON_RSHIFTASSIGN,
    XEC_TOKEN_URSHIFTASSIGN = LEMON_URSHIFTASSIGN,
    
    XEC_TOKEN_LOGICAND      = LEMON_LOGICAND,
    XEC_TOKEN_LOGICXOR      = LEMON_LOGICXOR,
    XEC_TOKEN_LOGICOR       = LEMON_LOGICOR,

#undef LEMON_BREAK
#undef LEMON_CASE
#undef LEMON_CATCH
#undef LEMON_CONTINUE
#undef LEMON_DEFAULT
#undef LEMON_DO
#undef LEMON_ELSE
#undef LEMON_FINALLY
#undef LEMON_FOR
#undef LEMON_IF
#undef LEMON_RETURN
#undef LEMON_SWITCH
#undef LEMON_THROW
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
    
#undef LEMON_INCREMENT
#undef LEMON_DECREMENT
    
#undef LEMON_LSHIFT
#undef LEMON_RSHIFT
#undef LEMON_URSHIFT
    
#undef LEMON_NOTEQUAL
#undef LEMON_LESSEQUAL
#undef LEMON_EQUAL
#undef LEMON_GREATEREQUAL
    
#undef LEMON_MODASSIGN
#undef LEMON_BITANDASSIGN
#undef LEMON_MULASSIGN
#undef LEMON_ADDASSIGN
#undef LEMON_SUBASSIGN
#undef LEMON_DIVASSIGN
#undef LEMON_BITXORASSIGN
#undef LEMON_ORASSIGN
#undef LEMON_INTDIVASSIGN
#undef LEMON_LSHIFTASSIGN
#undef LEMON_RSHIFTASSIGN
#undef LEMON_URSHIFTASSIGN
    
#undef LEMON_LOGICAND
#undef LEMON_LOGICXOR
#undef LEMON_LOGICOR
};




struct xec_token
{
    xec_token();
    xec_token( xec_token_kind kind, int sloc );
    xec_token( xec_token_kind kind, int sloc, const char* text, size_t size );
    xec_token( xec_token_kind kind, int sloc,
                    hash32_t hash, const char* text, size_t size );

    std::string     get_spelling();
    void            debug_print();

    xec_token_kind  kind;
    int             sloc;
    hash32_t        hash;
    const char*     text;
    size_t          size;
};



struct xec_token_lookup
{
    xec_token_lookup();
    xec_token_lookup(
        xec_token_kind kind, hash32_t hash, const char* text, size_t size );

    xec_token_kind  kind;
    hash32_t        hash;
    const char*     text;
    size_t          size;
};


bool xec_lookup_operator( xec_token_kind kind, xec_token_lookup* out_lookup );
bool xec_lookup_keyword( hash32_t hash,
            const char* text, size_t size, xec_token_lookup* out_lookup );




inline xec_token::xec_token()
    :   kind( XEC_TOKEN_EOF )
    ,   sloc( 0 )
    ,   hash( 0 )
    ,   text( NULL )
    ,   size( 0 )
{
}

inline xec_token::xec_token( xec_token_kind kind, int sloc )
    :   kind( kind )
    ,   sloc( sloc )
    ,   hash( 0 )
    ,   text( NULL )
    ,   size( 0 )
{
    xec_token_lookup op;
    if ( xec_lookup_operator( kind, &op ) )
    {
        hash = op.hash;
        text = op.text;
        size = op.size;
    }
}


inline xec_token::xec_token(
            xec_token_kind kind, int sloc, const char* text, size_t size )
    :   kind( kind )
    ,   sloc( sloc )
    ,   hash( 0 )
    ,   text( text )
    ,   size( size )
{
}

inline xec_token::xec_token( xec_token_kind kind,
                int sloc, hash32_t hash, const char* text, size_t size )
    :   kind( kind )
    ,   sloc( sloc )
    ,   hash( hash )
    ,   text( text )
    ,   size( size )
{
}



inline xec_token_lookup::xec_token_lookup()
    :   kind( XEC_TOKEN_EOF )
    ,   hash( 0 )
    ,   text( NULL )
    ,   size( 0 )
{
}

inline xec_token_lookup::xec_token_lookup(
            xec_token_kind kind, hash32_t hash, const char* text, size_t size )
    :   kind( kind )
    ,   hash( hash )
    ,   text( text )
    ,   size( size )
{
}






#endif
