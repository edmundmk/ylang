//
//  yl_parser_ragel.rl
//
//  Created by Edmund Kapusniak on 14/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


/*
    ragel yl_parser_ragel.rl -o yl_parser_ragel.cpp -G2
*/


#include "yl_parser.h"
#include <stdint.h>
#include <intformat.h>
#include <unordered_map>
#include <region.h>
#include "yl_token.h"


void* XecParseAlloc( void* (*malloc)( size_t ) );
void  XecParse( void* yyp, int yymajor, yl_token* yyminor, yl_parser* p );
void  XecParseFree( void* p, void (*free)( void* ) );



%%{

    machine lexer;
    
    alphtype unsigned char;
    
    
    action nl { newline( (int)( offset + ( p - buffer ) - 1 ) ); }
    action ts { sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
    action dc { data.append( (char)fc ); }
    
    action neof
    {
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        root->script->error( sloc, "unexpected end of file" );
        fbreak;
    }
    
    action nchar
    {
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( fc >= 0x20 && fc <= 0x7E )
            root->script->error( sloc, "unexpected character '%c'", fc );
        else
            root->script->error( sloc, "unexpected character '\\x%02X'", fc );
        if ( root->script->error_count() >= ERROR_LIMIT )
            goto error;
    }


    
    # Newlines (handling CR LF properly), comments, and whitespace.

    newline         = ( '\n' | '\r' | '\r\n' ) $1 %0 %nl %!nl ;
    block_comment   = '/*' ( any | newline )* :>> '*/' ;
    line_comment    = '//' any* :>> newline ;
    whitespace      = [ \t] | newline | block_comment | line_comment ;


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
                        root->script->error(
                                sloc, "invalid codepoint U+%04" PRIX32, temp );
                        if ( root->script->error_count() >= ERROR_LIMIT )
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
            ( string_quoted whitespace* )+
    );


    main :=
    (
            whitespace

        |   identifier >ts
                %
                {
                    yl_token* token = make_identifier( sloc, &data );
                    TOKEN( token );
                }

        |   number >ts
                %
                {
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    MTOKEN( YL_TOKEN_NUMBER, sloc, number, length );
                }

        |   string >ts
                %
                {
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    MTOKEN( YL_TOKEN_STRING, sloc, string, length );
                }
        
        |   '!'    >ts %{ MTOKEN( YL_TOKEN_XMARK, sloc, "!", 1 ); }
        |   '%'    >ts %{ MTOKEN( YL_TOKEN_PERCENT, sloc, "%", 1 ); }
        |   '&'    >ts %{ MTOKEN( YL_TOKEN_AMPERSAND, sloc, "&", 1 ); }
        |   '('    >ts %{ MTOKEN( YL_TOKEN_LPN, sloc, "(", 1 ); }
        |   ')'    >ts %{ MTOKEN( YL_TOKEN_RPN, sloc, ")", 1 ); }
        |   '*'    >ts %{ MTOKEN( YL_TOKEN_ASTERISK, sloc, "*", 1 ); }
        |   '+'    >ts %{ MTOKEN( YL_TOKEN_PLUS, sloc, "+", 1 ); }
        |   ','    >ts %{ MTOKEN( YL_TOKEN_COMMA, sloc, ",", 1 ); }
        |   '-'    >ts %{ MTOKEN( YL_TOKEN_MINUS, sloc, "-", 1 ); }
        |   '.'    >ts %{ MTOKEN( YL_TOKEN_PERIOD, sloc, ".", 1 ); }
        |   '/'    >ts %{ MTOKEN( YL_TOKEN_SOLIDUS, sloc, "/", 1 ); }
        |   ':'    >ts %{ MTOKEN( YL_TOKEN_COLON, sloc, ":", 1 ); }
        |   ';'    >ts %{ MTOKEN( YL_TOKEN_SEMICOLON, sloc, ";", 1 ); }
        |   '<'    >ts %{ MTOKEN( YL_TOKEN_LESS, sloc, "<", 1 ); }
        |   '='    >ts %{ MTOKEN( YL_TOKEN_ASSIGN, sloc, "=", 1 ); }
        |   '>'    >ts %{ MTOKEN( YL_TOKEN_GREATER, sloc, ">", 1 ); }
        |   '?'    >ts %{ MTOKEN( YL_TOKEN_QMARK, sloc, "?", 1 ); }
        |   '['    >ts %{ MTOKEN( YL_TOKEN_LSQ, sloc, "[", 1 ); }
        |   ']'    >ts %{ MTOKEN( YL_TOKEN_RSQ, sloc, "]", 1 ); }
        |   '^'    >ts %{ MTOKEN( YL_TOKEN_CARET, sloc, "^", 1 ); }
        |   '{'    >ts %{ MTOKEN( YL_TOKEN_LBR, sloc, "{", 1 ); }
        |   '|'    >ts %{ MTOKEN( YL_TOKEN_VBAR, sloc, "|", 1 ); }
        |   '}'    >ts %{ MTOKEN( YL_TOKEN_RBR, sloc, "}", 1 ); }
        |   '~'    >ts %{ MTOKEN( YL_TOKEN_TILDE, sloc, "~", 1 ); }

        |   '++'   >ts %{ MTOKEN( YL_TOKEN_INCREMENT, sloc, "++", 2 ); }
        |   '--'   >ts %{ MTOKEN( YL_TOKEN_DECREMENT, sloc, "--", 2 ); }

        |   '..'   >ts %{ MTOKEN( YL_TOKEN_CONCATENATE, sloc, "..", 2 ); }

        |   '<<'   >ts %{ MTOKEN( YL_TOKEN_LSHIFT, sloc, "<<", 2 ); }
        |   '>>'   >ts %{ MTOKEN( YL_TOKEN_LRSHIFT, sloc, ">>", 2 ); }
        |   '~>>'  >ts %{ MTOKEN( YL_TOKEN_ARSHIFT, sloc, "~>>", 3 ); }

        |   '!='   >ts %{ MTOKEN( YL_TOKEN_NOTEQUAL, sloc, "!=", 2 ); }
        |   '<='   >ts %{ MTOKEN( YL_TOKEN_LESSEQUAL, sloc, "<=", 2 ); }
        |   '=='   >ts %{ MTOKEN( YL_TOKEN_EQUAL, sloc, "==", 2 ); }
        |   '>='   >ts %{ MTOKEN( YL_TOKEN_GREATEREQUAL, sloc, ">=", 2 ); }
        |   '!in'  >ts %{ MTOKEN( YL_TOKEN_NOTIN, sloc, "!in", 3 ); }
        |   '!is'  >ts %{ MTOKEN( YL_TOKEN_NOTIS, sloc, "!is", 3 ); }

        |   '%='   >ts %{ MTOKEN( YL_TOKEN_MODASSIGN, sloc, "%=", 2 ); }
        |   '&='   >ts %{ MTOKEN( YL_TOKEN_BITANDASSIGN, sloc, "&=", 2 ); }
        |   '*='   >ts %{ MTOKEN( YL_TOKEN_MULASSIGN, sloc, "*=", 2 ); }
        |   '+='   >ts %{ MTOKEN( YL_TOKEN_ADDASSIGN, sloc, "+=", 2 ); }
        |   '-='   >ts %{ MTOKEN( YL_TOKEN_SUBASSIGN, sloc, "-=", 2 ); }
        |   '/='   >ts %{ MTOKEN( YL_TOKEN_DIVASSIGN, sloc, "/=", 2 ); }
        |   '^='   >ts %{ MTOKEN( YL_TOKEN_BITXORASSIGN, sloc, "^=", 2 ); }
        |   '|='   >ts %{ MTOKEN( YL_TOKEN_BITORASSIGN, sloc, "|=", 2 ); }
        |   '~='   >ts %{ MTOKEN( YL_TOKEN_INTDIVASSIGN, sloc, "~=", 2 ); }
        |   '<<='  >ts %{ MTOKEN( YL_TOKEN_LSHIFTASSIGN, sloc, "<<=", 3 ); }
        |   '>>='  >ts %{ MTOKEN( YL_TOKEN_LRSHIFTASSIGN, sloc, ">>=", 3 ); }
        |   '~>>=' >ts %{ MTOKEN( YL_TOKEN_ARSHIFTASSIGN, sloc, "~>>=", 4 ); }

        |   '&&'   >ts %{ MTOKEN( YL_TOKEN_LOGICAND, sloc, "&&", 2 ); }
        |   '^^'   >ts %{ MTOKEN( YL_TOKEN_LOGICXOR, sloc, "^^", 2 ); }
        |   '||'   >ts %{ MTOKEN( YL_TOKEN_LOGICOR, sloc, "||", 2 ); }
        
        |   '::'   >ts %{ MTOKEN( YL_TOKEN_EACHKEY, sloc, "::", 2 ); }
        |   '...'  >ts %{ MTOKEN( YL_TOKEN_ELLIPSIS, sloc, "...", 3 ); }

    )**

    @/neof
    
    $!nchar $!{ fgoto main; };

}%%


%% write data;



void yl_parser::newline( int sloc )
{
    root->script->newlines.push_back( sloc );
}



template < typename ... arguments_t >
yl_token* yl_parser::make_token( arguments_t ... arguments )
{
    void* p = NULL;
    if ( recycle_tokens.size() )
    {
        p = recycle_tokens.back();
        recycle_tokens.pop_back();
    }
    else
    {
        p = malloc( sizeof( yl_token ) );
    }
    return new ( p ) yl_token( arguments ... );
}



struct yl_keywords
    :   public std::unordered_map< symkey, yl_token_kind >
{
    
    yl_keywords()
    {
        emplace( symkey( "break" ),     YL_KEYWORD_BREAK );
        emplace( symkey( "case" ),      YL_KEYWORD_CASE );
        emplace( symkey( "catch" ),     YL_KEYWORD_CATCH );
        emplace( symkey( "continue" ),  YL_KEYWORD_CONTINUE );
        emplace( symkey( "def" ),       YL_KEYWORD_DEF );
        emplace( symkey( "default" ),   YL_KEYWORD_DEFAULT );
        emplace( symkey( "delete" ),    YL_KEYWORD_DELETE );
        emplace( symkey( "do" ),        YL_KEYWORD_DO );
        emplace( symkey( "else" ),      YL_KEYWORD_ELSE );
        emplace( symkey( "false" ),     YL_KEYWORD_FALSE );
        emplace( symkey( "finally" ),   YL_KEYWORD_FINALLY );
        emplace( symkey( "for" ),       YL_KEYWORD_FOR );
        emplace( symkey( "if" ),        YL_KEYWORD_IF );
        emplace( symkey( "new" ),       YL_KEYWORD_NEW );
        emplace( symkey( "null" ),      YL_KEYWORD_NULL );
        emplace( symkey( "return" ),    YL_KEYWORD_RETURN );
        emplace( symkey( "superof" ),   YL_KEYWORD_SUPEROF );
        emplace( symkey( "switch" ),    YL_KEYWORD_SWITCH );
        emplace( symkey( "throw" ),     YL_KEYWORD_THROW );
        emplace( symkey( "true" ),      YL_KEYWORD_TRUE );
        emplace( symkey( "try" ),       YL_KEYWORD_TRY );
        emplace( symkey( "using" ),     YL_KEYWORD_USING );
        emplace( symkey( "var" ),       YL_KEYWORD_VAR );
        emplace( symkey( "while" ),     YL_KEYWORD_WHILE );
        emplace( symkey( "yield" ),     YL_KEYWORD_YIELD );
        emplace( symkey( "in" ),        YL_TOKEN_IN );
        emplace( symkey( "is" ),        YL_TOKEN_IS );
    }
    
};



yl_token* yl_parser::make_identifier( int sloc, region_buffer* data )
{
    static yl_keywords keywords;

    size_t length = data->size();
    const char* identifier = (const char*)data->get();
    symkey key( identifier, length );
    
    // Check for keyword.
    auto i = keywords.find( key );
    if ( i != keywords.end() )
    {
        return make_token( i->second, sloc, i->first.c_str(), i->first.size() );
    }
    
    // Check for existing identifier.
    auto j = root->script->identifiers.find( key );
    if ( j != root->script->identifiers.end() )
    {
        return make_token( YL_TOKEN_IDENTIFIER, sloc, j->c_str(), j->size() );
    }

    // New identifier.
    data->append( '\0' );
    identifier = (const char*)data->tearoff();
    root->script->identifiers.emplace( key.hash(), identifier, length );
    return make_token( YL_TOKEN_IDENTIFIER, sloc, identifier, length );

}



static bool encode_utf8( region_buffer* data, uint32_t cp )
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



bool yl_parser::parse( const char* path )
{
    // Implicit allocations (for std::deque) from AST region.
    region_scope rscope( root->alloc );

    // Allocate strings and identifiers in the script's region.
    region_buffer data( root->script->alloc );



    // Push function scope.
    scopes.push_back( root->function->scope );

    
    // Open file.
    root->script->filename = path;
    FILE* file = fopen( path, "r" );
    if ( ! file )
    {
        root->script->error( 0, "unable to open file" );
        return false;
    }
    
    
    // File state.
    int offset = 0;
    unsigned char* buffer = (unsigned char*)malloc( BUFFER_SIZE );
    
    
    // Token state.
    int sloc = -1;
    uint32_t temp = 0;


    // Parser state.
    void* parser = XecParseAlloc( malloc );


    // Initialize state machine.
    int cs;
    
    %% write init;
    
    
    // Perform lexing.
#define TOKEN( token ) \
    { \
        XecParse( parser, token->kind, token, this ); \
        if ( root->script->error_count() >= ERROR_LIMIT ) \
            goto error; \
    }
#define MTOKEN( ... ) \
    { \
        yl_token* token = make_token( __VA_ARGS__ ); \
        XecParse( parser, token->kind, token, this ); \
        if ( root->script->error_count() >= ERROR_LIMIT ) \
            goto error; \
    }

    bool iseof = false;
    while ( ! iseof )
    {
        size_t read = fread( buffer, 1, BUFFER_SIZE, file );
        iseof = read < BUFFER_SIZE;
        
        if ( iseof && ferror( file ) )
        {
            data.shrink();
            root->script->error( offset, "error reading file" );
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
        MTOKEN( YL_TOKEN_EOF, offset, "", 0 );
    }

#undef MTOKEN
#undef TOKEN

error:

    // Free parser.
    XecParseFree( parser, free );

        
    // Close file.
    free( buffer );
    fclose( file );


    // Check if there were errors.
    return root->script->error_count() == 0;
}



#pragma unused( lexer_first_final )
#pragma unused( lexer_error )
#pragma unused( lexer_en_main )






