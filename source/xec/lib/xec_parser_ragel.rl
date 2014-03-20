//
//  xec_parser_ragel.rl
//
//  Created by Edmund Kapusniak on 14/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


/*
    ragel xec_parser_ragel.rl -o xec_parser_ragel.cpp -G2
*/


#include "xec_parser.h"
#include <stdint.h>
#include <intformat.h>
#include "xec_token.h"



%%{

    machine lexer;
    
    alphtype unsigned char;
    
    
    action nl { newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
    action ts { sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
    action dc { data.append( (char)fc ); }
    
    action neof
    {
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        diagnostic( sloc, "unexpected end of file" );
        fbreak;
    }
    
    action nchar
    {
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( fc >= 0x20 && fc <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", fc );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", fc );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }


    
    # Newlines (handling CR LF properly), comments, and whitespace.

    newline         = ( '\n' | '\r' | '\r\n' ) $1 %0 %nl %!nl ;
    block_comment   = '/*' ( any | newline )* :>> '*/' ;
    line_comment    = '//' any* :>> newline ;
    whitespace      = ( [ \t] | newline | block_comment | line_comment )+ ;


    identifier =
    (
            [a-zA-Z_][a-zA-Z0-9_]*
    ) $dc;

    
    number =
    (
            ( '0' | [1-9][0-9]* ) ( '.' [0-9]+ )? ( [eE] ( '+' | '-' )? [0-9]+ )?
        |   '0x' [0-9A-Fa-f]+ ( '.' [0-9A-Fa-f]+ )? ( [pP] ( '+' | '-' )? [0-9]+ )?
    ) $dc;


    string_escape =
    (
            '\\"'   @ { data.append( '\"' ); }
        |   '\\\\'  @ { data.append( '\\' ); }
        |   '\\/'   @ { data.append( '/' ); }
        |   '\\b'   @ { data.append( '\b' ); }
        |   '\\f'   @ { data.append( '\f' ); }
        |   '\\n'   @ { data.append( '\n' ); }
        |   '\\r'   @ { data.append( '\r' ); }
        |   '\\t'   @ { data.append( '\t' ); }
        |   '\\x'
            ( [0-9] @ { temp = ( fc - '0' ) << 4; }
            | [a-f] @ { temp = ( fc - 'a' + 0x0A ) << 4; }
            | [A-F] @ { temp = ( fc - 'A' + 0x0A ) << 4; }
            )
            ( [0-9] @ { data.append( (char)( temp | fc - '0' ) ); }
            | [a-f] @ { data.append( (char)( temp | fc - 'a' + 0x0A ) ); }
            | [A-F] @ { data.append( (char)( temp | fc - 'A' + 0x0A ) ); }
            )
        |   '\\u'
                >
                {
                    uloc = (int)( offset + ( p - buffer ) );
                }
            ( [0-9] @ { temp = ( fc - '0' ) << 20; }
            | [a-f] @ { temp = ( fc - 'a' + 0x0A ) << 20; }
            | [A-F] @ { temp = ( fc - 'A' + 0x0A ) << 20; }
            )
            ( [0-9] @ { temp |= ( fc - '0' ) << 16; }
            | [a-f] @ { temp |= ( fc - 'a' + 0x0A ) << 16; }
            | [A-F] @ { temp |= ( fc - 'A' + 0x0A ) << 16; }
            )
            ( [0-9] @ { temp |= ( fc - '0' ) << 12; }
            | [a-f] @ { temp |= ( fc - 'a' + 0x0A ) << 12; }
            | [A-F] @ { temp |= ( fc - 'A' + 0x0A ) << 12; }
            )
            ( [0-9] @ { temp |= ( fc - '0' ) << 8; }
            | [a-f] @ { temp |= ( fc - 'a' + 0x0A ) << 8; }
            | [A-F] @ { temp |= ( fc - 'A' + 0x0A ) << 8; }
            )
            ( [0-9] @ { temp |= ( fc - '0' ) << 4; }
            | [a-f] @ { temp |= ( fc - 'a' + 0x0A ) << 4; }
            | [A-F] @ { temp |= ( fc - 'A' + 0x0A ) << 4; }
            )
            ( [0-9] @ { temp |= fc - '0'; }
            | [a-f] @ { temp |= fc - 'a' + 0x0A; }
            | [A-F] @ { temp |= fc - 'A' + 0x0A; }
            )
                @
                {
                    if ( ! encode_utf8( &data, temp ) )
                    {
                        diagnostic(
                                sloc, "invalid codepoint U+%04" PRIX32, temp );
                        if ( diagnostics.size() >= ERROR_LIMIT )
                            goto error;
                    }
                }
    );

    string_char =
    (
            any - [\n\r\"\\]    
    ) $dc;

    string_quoted =
    (
            '"' string_char* ( string_escape string_char* )* '"'
    );

    string =
    (
            ( string_quoted whitespace? )+
    );


    main :=
    (
            whitespace

        |   identifier >ts
                %
                {
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }

        |   number >ts
                %
                {
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }

        |   string >ts
                %
                {
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }

        |   '!'     >ts %{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
        |   '%'     >ts %{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
        |   '&'     >ts %{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
        |   '('     >ts %{ TOKEN( XEC_TOKEN_LPN, sloc ); }
        |   ')'     >ts %{ TOKEN( XEC_TOKEN_RPN, sloc ); }
        |   '*'     >ts %{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
        |   '+'     >ts %{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
        |   ','     >ts %{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
        |   '-'     >ts %{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
        |   '.'     >ts %{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
        |   '/'     >ts %{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
        |   ':'     >ts %{ TOKEN( XEC_TOKEN_COLON, sloc ); }
        |   ';'     >ts %{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
        |   '<'     >ts %{ TOKEN( XEC_TOKEN_LESS, sloc ); }
        |   '='     >ts %{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
        |   '>'     >ts %{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
        |   '?'     >ts %{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
        |   '['     >ts %{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
        |   ']'     >ts %{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
        |   '^'     >ts %{ TOKEN( XEC_TOKEN_CARET, sloc ); }
        |   '{'     >ts %{ TOKEN( XEC_TOKEN_LBR, sloc ); }
        |   '|'     >ts %{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
        |   '}'     >ts %{ TOKEN( XEC_TOKEN_RBR, sloc ); }
        |   '~'     >ts %{ TOKEN( XEC_TOKEN_TILDE, sloc ); }

        |   '++'    >ts %{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
        |   '--'    >ts %{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }

        |   '<<'    >ts %{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
        |   '>>'    >ts %{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
        |   '>>>'   >ts %{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }

        |   '!='    >ts %{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
        |   '<='    >ts %{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
        |   '=='    >ts %{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
        |   '>='    >ts %{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
        |   '!in'   >ts %{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
        |   '!is'   >ts %{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }

        |   '%='    >ts %{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
        |   '&='    >ts %{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
        |   '*='    >ts %{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
        |   '+='    >ts %{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
        |   '-='    >ts %{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
        |   '/='    >ts %{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
        |   '^='    >ts %{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
        |   '|='    >ts %{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
        |   '~='    >ts %{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
        |   '<<='   >ts %{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
        |   '>>='   >ts %{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
        |   '>>>='  >ts %{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }

        |   '&&'    >ts %{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
        |   '^^'    >ts %{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
        |   '||'    >ts %{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
        
        |   '::'    >ts %{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
        |   '...'   >ts %{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }

    )**

    @/neof
    
    $!nchar $!{ fgoto main; };

}%%


%% write data;



template < typename ... arguments_t >
xec_token* xec_parser::make_token( arguments_t ... arguments )
{
    if ( recycle_tokens.size() )
    {
        void* p = recycle_tokens.back();
        recycle_tokens.pop_back();
        return new ( p ) xec_token( arguments ... );
    }
    else
    {
        return new ( alloc ) xec_token( arguments ... );
    }
}


xec_token_lookup xec_parser::lookup_identifier( region_buffer* data )
{
    xec_token_lookup lookup;

    size_t size = data->size();
    data->append( '\0' );
    const char* text = (const char*)data->get();

    symkey key( text, size );
    if ( xec_lookup_keyword( key.hash(), key.c_str(), key.size(), &lookup ) )
    {
        return lookup;
    }

    auto i = identifiers.find( key );
    if ( i != identifiers.end() )
    {
        key = *i;
    }
    else
    {
        text = (const char*)data->tearoff();
        key = symkey( key.hash(), text, size );
        identifiers.insert( key );
    }
    
    lookup = xec_token_lookup
    (
        XEC_TOKEN_IDENTIFIER,
        key.hash(),
        key.c_str(),
        key.size()
    );
    return lookup;
}


bool xec_parser::encode_utf8( region_buffer* data, uint32_t cp )
{
    if ( cp <= 0x7F )
    {
        data->append( (uint8_t)cp );
        return true;
    }
    else if ( cp <= 0x7FF )
    {
        data->append( (uint8_t)( 0xC0 | ( cp >> 6 & 0x1F ) ) );
        data->append( (uint8_t)( 0x80 | ( cp & 0x3F ) ) );
        return true;
    }
    else if ( cp <= 0xFFFF )
    {
        data->append( (uint8_t)( 0xE0 | ( cp >> 12 & 0x0F ) ) );
        data->append( (uint8_t)( 0x80 | ( cp >> 6 & 0x3F ) ) );
        data->append( (uint8_t)( 0x80 | ( cp & 0x3F ) ) );
        return true;
    }
    else if ( cp <= 0x10FFFF )
    {
        data->append( (uint8_t)( 0xF0 | ( cp >> 18 & 0x07 ) ) );
        data->append( (uint8_t)( 0x80 | ( cp >> 12 & 0x3F ) ) );
        data->append( (uint8_t)( 0x80 | ( cp >> 6 & 0x3F ) ) );
        data->append( (uint8_t)( 0x80 | ( cp & 0x3F ) ) );
        return true;
    }
    else
    {
        return false;
    }
}



bool xec_parser::parse( const char* path )
{
    region_scope rscope( &alloc );
    
    
    // Open file.
    this->filename = path;
    FILE* file = fopen( path, "r" );
    if ( ! file )
    {
        diagnostic( 0, "unable to open file" );
        return false;
    }
    
    
    // File state.
    int offset = 0;
    unsigned char* buffer = (unsigned char*)malloc( BUFFER_SIZE );
    
    
    // Token state.
    int sloc = -1;
    int uloc = -1;
    region_buffer data;
    uint32_t temp;


    // Initialize state machine.
    int cs;
    
    %% write init;
    
    
    // Perform lexing.
#define TOKEN( ... ) \
    { \
        xec_token* token = make_token( __VA_ARGS__ ); \
        token->debug_print(); \
    }

    bool iseof = false;
    while ( ! iseof )
    {
        size_t read = fread( buffer, 1, BUFFER_SIZE, file );
        iseof = read < BUFFER_SIZE;
        
        if ( iseof && ferror( file ) )
        {
            data.shrink();
            diagnostic( offset, "error reading file" );
            goto error;
        }

        const unsigned char* p      = buffer;
        const unsigned char* pe     = buffer + read;
        const unsigned char* eof    = iseof ? pe : NULL;

        %% write exec;
        
        offset += read;
    }

    if ( ! ferror( file ) )
    {
        TOKEN( XEC_TOKEN_EOF, offset );
    }

#undef TOKEN

error:

    // Close file.
    free( buffer );
    fclose( file );


    return diagnostics.size() == 0;
}



#pragma unused( lexer_first_final )
#pragma unused( lexer_error )
#pragma unused( lexer_en_main )






