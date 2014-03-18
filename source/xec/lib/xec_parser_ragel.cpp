
#line 1 "xec_parser_ragel.rl"
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





#line 241 "xec_parser_ragel.rl"




#line 31 "xec_parser_ragel.cpp"
static const int lexer_start = 24;
static const int lexer_first_final = 24;
static const int lexer_error = 0;

static const int lexer_en_main = 24;


#line 245 "xec_parser_ragel.rl"



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
    
    
#line 163 "xec_parser_ragel.cpp"
	{
	cs = lexer_start;
	}

#line 368 "xec_parser_ragel.rl"
    
    
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

        
#line 196 "xec_parser_ragel.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	switch( (*p) ) {
		case 9u: goto st25;
		case 10u: goto st26;
		case 13u: goto st27;
		case 32u: goto st25;
		case 33u: goto tr58;
		case 34u: goto tr59;
		case 37u: goto tr60;
		case 38u: goto tr61;
		case 40u: goto tr62;
		case 41u: goto tr63;
		case 42u: goto tr64;
		case 43u: goto tr65;
		case 44u: goto tr66;
		case 45u: goto tr67;
		case 46u: goto tr68;
		case 47u: goto tr69;
		case 48u: goto tr70;
		case 58u: goto tr72;
		case 59u: goto tr73;
		case 60u: goto tr74;
		case 61u: goto tr75;
		case 62u: goto tr76;
		case 63u: goto tr77;
		case 91u: goto tr79;
		case 93u: goto tr80;
		case 94u: goto tr81;
		case 95u: goto tr78;
		case 123u: goto tr82;
		case 124u: goto tr83;
		case 125u: goto tr84;
		case 126u: goto tr85;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr71;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr78;
	} else
		goto tr78;
	goto tr1;
tr1:
#line 42 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
#line 239 "xec_parser_ragel.rl"
	{ {goto st24;} }
	goto st0;
tr87:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 42 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
#line 239 "xec_parser_ragel.rl"
	{ {goto st24;} }
	goto st0;
#line 280 "xec_parser_ragel.cpp"
st0:
cs = 0;
	goto _out;
tr88:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st25;
tr119:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st25;
tr151:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st25;
tr182:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st25;
tr213:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st25;
tr244:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st25;
tr275:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st25;
tr306:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st25;
tr337:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st25;
tr368:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st25;
tr399:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st25;
tr430:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st25;
tr461:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st25;
tr492:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st25;
tr523:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st25;
tr554:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st25;
tr585:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st25;
tr614:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st25;
tr648:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st25;
tr680:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st25;
tr711:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st25;
tr742:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st25;
tr773:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st25;
tr804:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st25;
tr835:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st25;
tr866:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st25;
tr897:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st25;
tr928:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st25;
tr959:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st25;
tr988:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st25;
tr1019:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st25;
tr1050:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st25;
tr1081:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st25;
tr1112:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st25;
tr1143:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st25;
tr1174:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st25;
tr1205:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st25;
tr1236:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st25;
tr1267:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st25;
tr1298:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st25;
tr1329:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st25;
tr1360:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st25;
tr1391:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st25;
tr1422:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st25;
tr1453:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st25;
tr1484:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st25;
tr1515:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st25;
tr1547:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st25;
tr1578:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st25;
tr1609:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st25;
tr1640:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st25;
tr1671:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st25;
tr1702:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st25;
tr1733:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st25;
tr1764:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st25;
tr1795:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 525 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto st25;
		case 10u: goto st26;
		case 13u: goto st27;
		case 32u: goto st25;
		case 33u: goto tr58;
		case 34u: goto tr59;
		case 37u: goto tr60;
		case 38u: goto tr61;
		case 40u: goto tr62;
		case 41u: goto tr63;
		case 42u: goto tr64;
		case 43u: goto tr65;
		case 44u: goto tr66;
		case 45u: goto tr67;
		case 46u: goto tr68;
		case 47u: goto st23;
		case 48u: goto tr70;
		case 58u: goto tr72;
		case 59u: goto tr73;
		case 60u: goto tr74;
		case 61u: goto tr75;
		case 62u: goto tr76;
		case 63u: goto tr77;
		case 91u: goto tr79;
		case 93u: goto tr80;
		case 94u: goto tr81;
		case 95u: goto tr78;
		case 123u: goto tr82;
		case 124u: goto tr83;
		case 125u: goto tr84;
		case 126u: goto tr85;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr71;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr78;
	} else
		goto tr78;
	goto tr1;
tr89:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st26;
tr120:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st26;
tr152:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st26;
tr183:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st26;
tr214:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st26;
tr245:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st26;
tr276:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st26;
tr307:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st26;
tr338:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st26;
tr369:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st26;
tr400:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st26;
tr431:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st26;
tr462:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st26;
tr493:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st26;
tr524:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st26;
tr555:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st26;
tr586:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st26;
tr615:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st26;
tr649:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st26;
tr681:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st26;
tr712:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st26;
tr743:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st26;
tr774:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st26;
tr805:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st26;
tr836:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st26;
tr867:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st26;
tr898:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st26;
tr929:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st26;
tr960:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st26;
tr989:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st26;
tr1020:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st26;
tr1051:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st26;
tr1082:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st26;
tr1113:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st26;
tr1144:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st26;
tr1175:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st26;
tr1206:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st26;
tr1237:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st26;
tr1268:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st26;
tr1299:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st26;
tr1330:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st26;
tr1361:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st26;
tr1392:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st26;
tr1423:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st26;
tr1454:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st26;
tr1485:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st26;
tr1516:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st26;
tr1548:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st26;
tr1579:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st26;
tr1610:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st26;
tr1641:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st26;
tr1672:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st26;
tr1703:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st26;
tr1734:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st26;
tr1765:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st26;
tr1796:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 809 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr88;
		case 10u: goto tr89;
		case 13u: goto tr90;
		case 32u: goto tr88;
		case 33u: goto tr91;
		case 34u: goto tr92;
		case 37u: goto tr93;
		case 38u: goto tr94;
		case 40u: goto tr95;
		case 41u: goto tr96;
		case 42u: goto tr97;
		case 43u: goto tr98;
		case 44u: goto tr99;
		case 45u: goto tr100;
		case 46u: goto tr101;
		case 47u: goto tr102;
		case 48u: goto tr103;
		case 58u: goto tr105;
		case 59u: goto tr106;
		case 60u: goto tr107;
		case 61u: goto tr108;
		case 62u: goto tr109;
		case 63u: goto tr110;
		case 91u: goto tr112;
		case 93u: goto tr113;
		case 94u: goto tr114;
		case 95u: goto tr111;
		case 123u: goto tr115;
		case 124u: goto tr116;
		case 125u: goto tr117;
		case 126u: goto tr118;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr104;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr111;
	} else
		goto tr111;
	goto tr87;
tr90:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st27;
tr121:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st27;
tr153:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st27;
tr184:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st27;
tr215:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st27;
tr246:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st27;
tr277:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st27;
tr308:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st27;
tr339:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st27;
tr370:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st27;
tr401:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st27;
tr432:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st27;
tr463:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st27;
tr494:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st27;
tr525:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st27;
tr556:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st27;
tr587:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st27;
tr616:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st27;
tr650:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st27;
tr682:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st27;
tr713:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st27;
tr744:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st27;
tr775:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st27;
tr806:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st27;
tr837:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st27;
tr868:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st27;
tr899:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st27;
tr930:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st27;
tr961:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st27;
tr990:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st27;
tr1021:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st27;
tr1052:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st27;
tr1083:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st27;
tr1114:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st27;
tr1145:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st27;
tr1176:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st27;
tr1207:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st27;
tr1238:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st27;
tr1269:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st27;
tr1300:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st27;
tr1331:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st27;
tr1362:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st27;
tr1393:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st27;
tr1424:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st27;
tr1455:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st27;
tr1486:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st27;
tr1517:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st27;
tr1549:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st27;
tr1580:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st27;
tr1611:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st27;
tr1642:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st27;
tr1673:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st27;
tr1704:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st27;
tr1735:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st27;
tr1766:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st27;
tr1797:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 1093 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr88;
		case 10u: goto st26;
		case 13u: goto tr90;
		case 32u: goto tr88;
		case 33u: goto tr91;
		case 34u: goto tr92;
		case 37u: goto tr93;
		case 38u: goto tr94;
		case 40u: goto tr95;
		case 41u: goto tr96;
		case 42u: goto tr97;
		case 43u: goto tr98;
		case 44u: goto tr99;
		case 45u: goto tr100;
		case 46u: goto tr101;
		case 47u: goto tr102;
		case 48u: goto tr103;
		case 58u: goto tr105;
		case 59u: goto tr106;
		case 60u: goto tr107;
		case 61u: goto tr108;
		case 62u: goto tr109;
		case 63u: goto tr110;
		case 91u: goto tr112;
		case 93u: goto tr113;
		case 94u: goto tr114;
		case 95u: goto tr111;
		case 123u: goto tr115;
		case 124u: goto tr116;
		case 125u: goto tr117;
		case 126u: goto tr118;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr104;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr111;
	} else
		goto tr111;
	goto tr87;
tr58:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr91:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr122:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr154:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr185:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr216:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr247:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr278:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr309:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr340:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr371:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr402:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr433:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr464:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr495:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr526:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr557:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr588:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr617:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr651:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr683:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr714:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr745:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr776:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr807:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr838:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr869:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr900:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr931:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr962:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr991:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1022:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1053:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1084:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1115:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1146:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1177:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1208:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1239:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1270:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1301:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1332:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1363:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1394:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1425:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1456:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1487:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1518:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1550:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1581:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1612:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1643:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1674:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1705:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1736:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1767:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1798:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 1493 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr119;
		case 10u: goto tr120;
		case 13u: goto tr121;
		case 32u: goto tr119;
		case 33u: goto tr122;
		case 34u: goto tr123;
		case 37u: goto tr124;
		case 38u: goto tr125;
		case 40u: goto tr126;
		case 41u: goto tr127;
		case 42u: goto tr128;
		case 43u: goto tr129;
		case 44u: goto tr130;
		case 45u: goto tr131;
		case 46u: goto tr132;
		case 47u: goto tr133;
		case 48u: goto tr134;
		case 58u: goto tr136;
		case 59u: goto tr137;
		case 60u: goto tr138;
		case 61u: goto st85;
		case 62u: goto tr140;
		case 63u: goto tr141;
		case 91u: goto tr143;
		case 93u: goto tr144;
		case 94u: goto tr145;
		case 95u: goto tr142;
		case 105u: goto st22;
		case 123u: goto tr147;
		case 124u: goto tr148;
		case 125u: goto tr149;
		case 126u: goto tr150;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr135;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr142;
	} else
		goto tr142;
	goto tr1;
tr0:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st1;
tr22:
#line 78 "xec_parser_ragel.rl"
	{ data.append( '\"' ); }
	goto st1;
tr23:
#line 80 "xec_parser_ragel.rl"
	{ data.append( '/' ); }
	goto st1;
tr24:
#line 79 "xec_parser_ragel.rl"
	{ data.append( '\\' ); }
	goto st1;
tr25:
#line 81 "xec_parser_ragel.rl"
	{ data.append( '\b' ); }
	goto st1;
tr26:
#line 82 "xec_parser_ragel.rl"
	{ data.append( '\f' ); }
	goto st1;
tr27:
#line 83 "xec_parser_ragel.rl"
	{ data.append( '\n' ); }
	goto st1;
tr28:
#line 84 "xec_parser_ragel.rl"
	{ data.append( '\r' ); }
	goto st1;
tr29:
#line 85 "xec_parser_ragel.rl"
	{ data.append( '\t' ); }
	goto st1;
tr47:
#line 120 "xec_parser_ragel.rl"
	{ temp |= (*p) - '0'; }
#line 125 "xec_parser_ragel.rl"
	{
                    if ( ! encode_utf8( &data, temp ) )
                    {
                        diagnostic(
                                sloc, "invalid codepoint U+%04" PRIX32, temp );
                        if ( diagnostics.size() >= ERROR_LIMIT )
                            goto error;
                    }
                }
	goto st1;
tr48:
#line 122 "xec_parser_ragel.rl"
	{ temp |= (*p) - 'A' + 0x0A; }
#line 125 "xec_parser_ragel.rl"
	{
                    if ( ! encode_utf8( &data, temp ) )
                    {
                        diagnostic(
                                sloc, "invalid codepoint U+%04" PRIX32, temp );
                        if ( diagnostics.size() >= ERROR_LIMIT )
                            goto error;
                    }
                }
	goto st1;
tr49:
#line 121 "xec_parser_ragel.rl"
	{ temp |= (*p) - 'a' + 0x0A; }
#line 125 "xec_parser_ragel.rl"
	{
                    if ( ! encode_utf8( &data, temp ) )
                    {
                        diagnostic(
                                sloc, "invalid codepoint U+%04" PRIX32, temp );
                        if ( diagnostics.size() >= ERROR_LIMIT )
                            goto error;
                    }
                }
	goto st1;
tr53:
#line 91 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - '0' ) ); }
	goto st1;
tr54:
#line 93 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'A' + 0x0A ) ); }
	goto st1;
tr55:
#line 92 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'a' + 0x0A ) ); }
	goto st1;
tr59:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr92:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr123:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr155:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr186:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr217:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr248:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr279:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr310:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr341:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr372:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr403:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr434:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr465:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr496:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr527:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr558:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr589:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr618:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr652:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr684:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr715:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr746:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr777:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr808:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr839:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr870:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr901:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr932:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr963:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr992:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1023:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1054:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1085:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1116:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1147:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1178:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1209:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1240:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1271:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1302:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1333:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1364:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1395:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1426:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1457:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1488:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1519:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1551:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1582:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1613:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1644:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1675:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1706:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1737:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1768:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1799:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 1984 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr1;
		case 13u: goto tr1;
		case 34u: goto st29;
		case 92u: goto tr3;
	}
	goto tr0;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	switch( (*p) ) {
		case 9u: goto tr151;
		case 10u: goto tr152;
		case 13u: goto tr153;
		case 32u: goto tr151;
		case 33u: goto tr154;
		case 34u: goto tr155;
		case 37u: goto tr156;
		case 38u: goto tr157;
		case 40u: goto tr158;
		case 41u: goto tr159;
		case 42u: goto tr160;
		case 43u: goto tr161;
		case 44u: goto tr162;
		case 45u: goto tr163;
		case 46u: goto tr164;
		case 47u: goto tr165;
		case 48u: goto tr166;
		case 58u: goto tr168;
		case 59u: goto tr169;
		case 60u: goto tr170;
		case 61u: goto tr171;
		case 62u: goto tr172;
		case 63u: goto tr173;
		case 91u: goto tr175;
		case 93u: goto tr176;
		case 94u: goto tr177;
		case 95u: goto tr174;
		case 123u: goto tr178;
		case 124u: goto tr179;
		case 125u: goto tr180;
		case 126u: goto tr181;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr167;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr174;
	} else
		goto tr174;
	goto tr1;
tr60:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr93:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr124:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr156:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr187:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr218:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr249:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr280:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr311:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr342:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr373:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr404:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr435:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr466:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr497:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr528:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr559:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr590:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr619:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr653:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr685:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr716:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr747:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr778:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr809:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr840:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr871:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr902:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr933:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr964:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr993:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1024:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1055:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1086:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1117:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1148:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1179:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1210:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1241:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1272:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1303:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1334:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1365:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1396:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1427:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1458:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1489:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1520:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1552:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1583:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1614:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1645:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1676:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1707:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1738:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1769:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1800:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 2395 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr182;
		case 10u: goto tr183;
		case 13u: goto tr184;
		case 32u: goto tr182;
		case 33u: goto tr185;
		case 34u: goto tr186;
		case 37u: goto tr187;
		case 38u: goto tr188;
		case 40u: goto tr189;
		case 41u: goto tr190;
		case 42u: goto tr191;
		case 43u: goto tr192;
		case 44u: goto tr193;
		case 45u: goto tr194;
		case 46u: goto tr195;
		case 47u: goto tr196;
		case 48u: goto tr197;
		case 58u: goto tr199;
		case 59u: goto tr200;
		case 60u: goto tr201;
		case 61u: goto st84;
		case 62u: goto tr203;
		case 63u: goto tr204;
		case 91u: goto tr206;
		case 93u: goto tr207;
		case 94u: goto tr208;
		case 95u: goto tr205;
		case 123u: goto tr209;
		case 124u: goto tr210;
		case 125u: goto tr211;
		case 126u: goto tr212;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr198;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr205;
	} else
		goto tr205;
	goto tr1;
tr61:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr94:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr125:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr157:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr188:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr250:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr281:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr312:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr343:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr374:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr405:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr436:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr467:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr498:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr529:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr560:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr591:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr620:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr654:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr686:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr717:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr748:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr779:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr810:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr841:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr872:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr903:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr934:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr965:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr994:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1025:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1056:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1087:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1118:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1149:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1180:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1211:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1242:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1273:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1304:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1335:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1366:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1397:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1428:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1459:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1490:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1521:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1553:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1584:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1615:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1646:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1677:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1708:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1739:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1770:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1801:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 2789 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr213;
		case 10u: goto tr214;
		case 13u: goto tr215;
		case 32u: goto tr213;
		case 33u: goto tr216;
		case 34u: goto tr217;
		case 37u: goto tr218;
		case 38u: goto st32;
		case 40u: goto tr220;
		case 41u: goto tr221;
		case 42u: goto tr222;
		case 43u: goto tr223;
		case 44u: goto tr224;
		case 45u: goto tr225;
		case 46u: goto tr226;
		case 47u: goto tr227;
		case 48u: goto tr228;
		case 58u: goto tr230;
		case 59u: goto tr231;
		case 60u: goto tr232;
		case 61u: goto st83;
		case 62u: goto tr234;
		case 63u: goto tr235;
		case 91u: goto tr237;
		case 93u: goto tr238;
		case 94u: goto tr239;
		case 95u: goto tr236;
		case 123u: goto tr240;
		case 124u: goto tr241;
		case 125u: goto tr242;
		case 126u: goto tr243;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr229;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr236;
	} else
		goto tr236;
	goto tr1;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 9u: goto tr244;
		case 10u: goto tr245;
		case 13u: goto tr246;
		case 32u: goto tr244;
		case 33u: goto tr247;
		case 34u: goto tr248;
		case 37u: goto tr249;
		case 38u: goto tr250;
		case 40u: goto tr251;
		case 41u: goto tr252;
		case 42u: goto tr253;
		case 43u: goto tr254;
		case 44u: goto tr255;
		case 45u: goto tr256;
		case 46u: goto tr257;
		case 47u: goto tr258;
		case 48u: goto tr259;
		case 58u: goto tr261;
		case 59u: goto tr262;
		case 60u: goto tr263;
		case 61u: goto tr264;
		case 62u: goto tr265;
		case 63u: goto tr266;
		case 91u: goto tr268;
		case 93u: goto tr269;
		case 94u: goto tr270;
		case 95u: goto tr267;
		case 123u: goto tr271;
		case 124u: goto tr272;
		case 125u: goto tr273;
		case 126u: goto tr274;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr260;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr267;
	} else
		goto tr267;
	goto tr1;
tr62:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr95:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr126:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr158:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr189:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr220:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr251:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr282:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr313:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr344:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr375:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr406:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr437:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr468:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr499:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr530:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr561:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr592:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr621:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr655:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr687:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr718:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr749:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr780:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr811:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr842:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr873:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr904:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr935:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr966:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr995:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1026:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1057:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1088:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1119:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1150:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1181:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1212:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1243:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1274:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1305:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1336:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1367:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1398:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1429:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1460:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1491:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1522:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1554:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1585:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1616:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1647:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1678:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1709:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1740:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1771:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1802:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 3235 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr275;
		case 10u: goto tr276;
		case 13u: goto tr277;
		case 32u: goto tr275;
		case 33u: goto tr278;
		case 34u: goto tr279;
		case 37u: goto tr280;
		case 38u: goto tr281;
		case 40u: goto tr282;
		case 41u: goto tr283;
		case 42u: goto tr284;
		case 43u: goto tr285;
		case 44u: goto tr286;
		case 45u: goto tr287;
		case 46u: goto tr288;
		case 47u: goto tr289;
		case 48u: goto tr290;
		case 58u: goto tr292;
		case 59u: goto tr293;
		case 60u: goto tr294;
		case 61u: goto tr295;
		case 62u: goto tr296;
		case 63u: goto tr297;
		case 91u: goto tr299;
		case 93u: goto tr300;
		case 94u: goto tr301;
		case 95u: goto tr298;
		case 123u: goto tr302;
		case 124u: goto tr303;
		case 125u: goto tr304;
		case 126u: goto tr305;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr291;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr298;
	} else
		goto tr298;
	goto tr1;
tr63:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr96:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr127:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr159:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr190:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr221:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr252:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr283:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr314:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr345:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr376:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr407:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr438:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr469:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr500:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr531:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr562:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr593:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr622:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr656:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr688:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr719:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr750:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr781:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr812:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr843:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr874:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr905:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr936:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr967:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr996:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1027:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1058:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1089:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1120:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1151:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1182:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1213:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1244:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1275:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1306:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1337:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1368:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1399:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1430:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1461:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1492:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1523:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1555:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1586:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1617:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1648:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1679:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1710:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1741:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1772:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1803:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 3635 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr306;
		case 10u: goto tr307;
		case 13u: goto tr308;
		case 32u: goto tr306;
		case 33u: goto tr309;
		case 34u: goto tr310;
		case 37u: goto tr311;
		case 38u: goto tr312;
		case 40u: goto tr313;
		case 41u: goto tr314;
		case 42u: goto tr315;
		case 43u: goto tr316;
		case 44u: goto tr317;
		case 45u: goto tr318;
		case 46u: goto tr319;
		case 47u: goto tr320;
		case 48u: goto tr321;
		case 58u: goto tr323;
		case 59u: goto tr324;
		case 60u: goto tr325;
		case 61u: goto tr326;
		case 62u: goto tr327;
		case 63u: goto tr328;
		case 91u: goto tr330;
		case 93u: goto tr331;
		case 94u: goto tr332;
		case 95u: goto tr329;
		case 123u: goto tr333;
		case 124u: goto tr334;
		case 125u: goto tr335;
		case 126u: goto tr336;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr322;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr329;
	} else
		goto tr329;
	goto tr1;
tr64:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr97:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr128:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr160:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr191:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr222:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr253:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr284:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr315:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr346:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr377:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr408:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr439:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr470:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr501:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr532:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr563:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr623:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr657:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr689:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr720:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr751:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr782:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr813:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr844:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr875:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr906:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr937:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr968:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr997:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1028:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1059:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1090:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1121:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1152:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1183:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1214:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1245:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1276:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1307:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1338:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1369:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1400:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1431:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1462:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1493:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1524:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1556:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1587:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1618:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1649:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1680:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1711:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1742:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1773:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1804:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 4029 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr337;
		case 10u: goto tr338;
		case 13u: goto tr339;
		case 32u: goto tr337;
		case 33u: goto tr340;
		case 34u: goto tr341;
		case 37u: goto tr342;
		case 38u: goto tr343;
		case 40u: goto tr344;
		case 41u: goto tr345;
		case 42u: goto tr346;
		case 43u: goto tr347;
		case 44u: goto tr348;
		case 45u: goto tr349;
		case 46u: goto tr350;
		case 47u: goto tr351;
		case 48u: goto tr352;
		case 58u: goto tr354;
		case 59u: goto tr355;
		case 60u: goto tr356;
		case 61u: goto st82;
		case 62u: goto tr358;
		case 63u: goto tr359;
		case 91u: goto tr361;
		case 93u: goto tr362;
		case 94u: goto tr363;
		case 95u: goto tr360;
		case 123u: goto tr364;
		case 124u: goto tr365;
		case 125u: goto tr366;
		case 126u: goto tr367;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr353;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr360;
	} else
		goto tr360;
	goto tr1;
tr65:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr98:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr129:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr161:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr192:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr223:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr254:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr285:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr316:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr347:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr409:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr440:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr471:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr502:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr533:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr564:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr594:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr624:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr658:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr690:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr721:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr752:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr783:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr814:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr845:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr876:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr907:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr938:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr969:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr998:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1029:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1060:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1091:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1122:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1153:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1184:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1215:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1246:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1277:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1308:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1339:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1370:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1401:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1432:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1463:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1494:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1525:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1557:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1588:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1619:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1650:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1681:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1712:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1743:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1774:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1805:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 4423 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr368;
		case 10u: goto tr369;
		case 13u: goto tr370;
		case 32u: goto tr368;
		case 33u: goto tr371;
		case 34u: goto tr372;
		case 37u: goto tr373;
		case 38u: goto tr374;
		case 40u: goto tr375;
		case 41u: goto tr376;
		case 42u: goto tr377;
		case 43u: goto st37;
		case 44u: goto tr379;
		case 45u: goto tr380;
		case 46u: goto tr381;
		case 47u: goto tr382;
		case 48u: goto tr383;
		case 58u: goto tr385;
		case 59u: goto tr386;
		case 60u: goto tr387;
		case 61u: goto st81;
		case 62u: goto tr389;
		case 63u: goto tr390;
		case 91u: goto tr392;
		case 93u: goto tr393;
		case 94u: goto tr394;
		case 95u: goto tr391;
		case 123u: goto tr395;
		case 124u: goto tr396;
		case 125u: goto tr397;
		case 126u: goto tr398;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr384;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr391;
	} else
		goto tr391;
	goto tr1;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	switch( (*p) ) {
		case 9u: goto tr399;
		case 10u: goto tr400;
		case 13u: goto tr401;
		case 32u: goto tr399;
		case 33u: goto tr402;
		case 34u: goto tr403;
		case 37u: goto tr404;
		case 38u: goto tr405;
		case 40u: goto tr406;
		case 41u: goto tr407;
		case 42u: goto tr408;
		case 43u: goto tr409;
		case 44u: goto tr410;
		case 45u: goto tr411;
		case 46u: goto tr412;
		case 47u: goto tr413;
		case 48u: goto tr414;
		case 58u: goto tr416;
		case 59u: goto tr417;
		case 60u: goto tr418;
		case 61u: goto tr419;
		case 62u: goto tr420;
		case 63u: goto tr421;
		case 91u: goto tr423;
		case 93u: goto tr424;
		case 94u: goto tr425;
		case 95u: goto tr422;
		case 123u: goto tr426;
		case 124u: goto tr427;
		case 125u: goto tr428;
		case 126u: goto tr429;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr415;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr422;
	} else
		goto tr422;
	goto tr1;
tr66:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr99:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr130:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr162:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr193:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr224:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr255:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr286:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr317:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr348:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr379:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr410:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr441:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr472:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr503:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr534:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr565:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr595:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr625:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr659:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr691:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr722:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr753:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr784:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr815:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr846:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr877:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr908:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr939:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr970:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr999:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1030:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1061:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1092:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1123:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1154:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1185:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1216:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1247:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1278:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1309:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1340:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1371:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1402:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1433:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1464:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1495:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1526:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1558:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1589:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1620:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1651:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1682:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1713:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1744:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1775:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1806:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 4869 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr430;
		case 10u: goto tr431;
		case 13u: goto tr432;
		case 32u: goto tr430;
		case 33u: goto tr433;
		case 34u: goto tr434;
		case 37u: goto tr435;
		case 38u: goto tr436;
		case 40u: goto tr437;
		case 41u: goto tr438;
		case 42u: goto tr439;
		case 43u: goto tr440;
		case 44u: goto tr441;
		case 45u: goto tr442;
		case 46u: goto tr443;
		case 47u: goto tr444;
		case 48u: goto tr445;
		case 58u: goto tr447;
		case 59u: goto tr448;
		case 60u: goto tr449;
		case 61u: goto tr450;
		case 62u: goto tr451;
		case 63u: goto tr452;
		case 91u: goto tr454;
		case 93u: goto tr455;
		case 94u: goto tr456;
		case 95u: goto tr453;
		case 123u: goto tr457;
		case 124u: goto tr458;
		case 125u: goto tr459;
		case 126u: goto tr460;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr446;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr453;
	} else
		goto tr453;
	goto tr1;
tr67:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr100:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr131:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr163:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr194:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr225:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr256:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr287:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr318:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr349:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr380:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr411:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr442:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr504:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr535:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr566:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr596:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr626:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr660:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr692:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr723:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr754:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr785:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr816:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr847:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr878:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr909:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr940:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr971:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1000:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1031:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1062:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1093:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1124:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1155:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1186:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1217:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1248:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1279:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1310:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1341:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1372:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1403:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1434:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1465:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1496:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1527:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1559:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1590:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1621:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1652:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1683:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1714:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1745:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1776:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1807:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 5263 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr461;
		case 10u: goto tr462;
		case 13u: goto tr463;
		case 32u: goto tr461;
		case 33u: goto tr464;
		case 34u: goto tr465;
		case 37u: goto tr466;
		case 38u: goto tr467;
		case 40u: goto tr468;
		case 41u: goto tr469;
		case 42u: goto tr470;
		case 43u: goto tr471;
		case 44u: goto tr472;
		case 45u: goto st40;
		case 46u: goto tr474;
		case 47u: goto tr475;
		case 48u: goto tr476;
		case 58u: goto tr478;
		case 59u: goto tr479;
		case 60u: goto tr480;
		case 61u: goto st80;
		case 62u: goto tr482;
		case 63u: goto tr483;
		case 91u: goto tr485;
		case 93u: goto tr486;
		case 94u: goto tr487;
		case 95u: goto tr484;
		case 123u: goto tr488;
		case 124u: goto tr489;
		case 125u: goto tr490;
		case 126u: goto tr491;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr477;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr484;
	} else
		goto tr484;
	goto tr1;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 9u: goto tr492;
		case 10u: goto tr493;
		case 13u: goto tr494;
		case 32u: goto tr492;
		case 33u: goto tr495;
		case 34u: goto tr496;
		case 37u: goto tr497;
		case 38u: goto tr498;
		case 40u: goto tr499;
		case 41u: goto tr500;
		case 42u: goto tr501;
		case 43u: goto tr502;
		case 44u: goto tr503;
		case 45u: goto tr504;
		case 46u: goto tr505;
		case 47u: goto tr506;
		case 48u: goto tr507;
		case 58u: goto tr509;
		case 59u: goto tr510;
		case 60u: goto tr511;
		case 61u: goto tr512;
		case 62u: goto tr513;
		case 63u: goto tr514;
		case 91u: goto tr516;
		case 93u: goto tr517;
		case 94u: goto tr518;
		case 95u: goto tr515;
		case 123u: goto tr519;
		case 124u: goto tr520;
		case 125u: goto tr521;
		case 126u: goto tr522;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr508;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr515;
	} else
		goto tr515;
	goto tr1;
tr68:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr101:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr132:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr164:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr195:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr226:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr257:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr288:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr319:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr350:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr381:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr412:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr443:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr474:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr505:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr567:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr597:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr647:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr661:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr693:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr724:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr755:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr786:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr817:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr848:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr879:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr910:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr941:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr972:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1001:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1032:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1063:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1094:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1125:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1156:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1187:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1218:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1249:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1280:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1311:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1342:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1373:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1404:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1435:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1466:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1497:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1528:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1560:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1591:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1622:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1653:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1684:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1715:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1746:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1777:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1808:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 5703 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr523;
		case 10u: goto tr524;
		case 13u: goto tr525;
		case 32u: goto tr523;
		case 33u: goto tr526;
		case 34u: goto tr527;
		case 37u: goto tr528;
		case 38u: goto tr529;
		case 40u: goto tr530;
		case 41u: goto tr531;
		case 42u: goto tr532;
		case 43u: goto tr533;
		case 44u: goto tr534;
		case 45u: goto tr535;
		case 46u: goto st2;
		case 47u: goto tr537;
		case 48u: goto tr538;
		case 58u: goto tr540;
		case 59u: goto tr541;
		case 60u: goto tr542;
		case 61u: goto tr543;
		case 62u: goto tr544;
		case 63u: goto tr545;
		case 91u: goto tr547;
		case 93u: goto tr548;
		case 94u: goto tr549;
		case 95u: goto tr546;
		case 123u: goto tr550;
		case 124u: goto tr551;
		case 125u: goto tr552;
		case 126u: goto tr553;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr539;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr546;
	} else
		goto tr546;
	goto tr1;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 46u )
		goto st42;
	goto tr1;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	switch( (*p) ) {
		case 9u: goto tr554;
		case 10u: goto tr555;
		case 13u: goto tr556;
		case 32u: goto tr554;
		case 33u: goto tr557;
		case 34u: goto tr558;
		case 37u: goto tr559;
		case 38u: goto tr560;
		case 40u: goto tr561;
		case 41u: goto tr562;
		case 42u: goto tr563;
		case 43u: goto tr564;
		case 44u: goto tr565;
		case 45u: goto tr566;
		case 46u: goto tr567;
		case 47u: goto tr568;
		case 48u: goto tr569;
		case 58u: goto tr571;
		case 59u: goto tr572;
		case 60u: goto tr573;
		case 61u: goto tr574;
		case 62u: goto tr575;
		case 63u: goto tr576;
		case 91u: goto tr578;
		case 93u: goto tr579;
		case 94u: goto tr580;
		case 95u: goto tr577;
		case 123u: goto tr581;
		case 124u: goto tr582;
		case 125u: goto tr583;
		case 126u: goto tr584;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr570;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr577;
	} else
		goto tr577;
	goto tr1;
tr69:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr133:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr165:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr196:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr227:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr258:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr289:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr320:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr351:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr382:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr413:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr444:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr475:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr506:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr537:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr568:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr628:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr662:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr694:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr725:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr756:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr787:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr818:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr849:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr880:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr911:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr942:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr973:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1002:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1033:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1064:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1095:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1126:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1157:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1188:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1219:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1250:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1281:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1312:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1343:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1374:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1405:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1436:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1467:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1498:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1529:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1561:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1592:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1623:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1654:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1685:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1716:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1747:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1778:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1809:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 6144 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr585;
		case 10u: goto tr586;
		case 13u: goto tr587;
		case 32u: goto tr585;
		case 33u: goto tr588;
		case 34u: goto tr589;
		case 37u: goto tr590;
		case 38u: goto tr591;
		case 40u: goto tr592;
		case 41u: goto tr593;
		case 42u: goto st3;
		case 43u: goto tr594;
		case 44u: goto tr595;
		case 45u: goto tr596;
		case 46u: goto tr597;
		case 47u: goto st7;
		case 48u: goto tr598;
		case 58u: goto tr600;
		case 59u: goto tr601;
		case 60u: goto tr602;
		case 61u: goto st79;
		case 62u: goto tr604;
		case 63u: goto tr605;
		case 91u: goto tr607;
		case 93u: goto tr608;
		case 94u: goto tr609;
		case 95u: goto tr606;
		case 123u: goto tr610;
		case 124u: goto tr611;
		case 125u: goto tr612;
		case 126u: goto tr613;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr599;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr606;
	} else
		goto tr606;
	goto tr1;
tr9:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 6195 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st4;
		case 13u: goto st5;
		case 42u: goto st6;
	}
	goto st3;
tr10:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 6210 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr10;
		case 13u: goto tr11;
		case 42u: goto tr12;
	}
	goto tr9;
tr11:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 6225 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st4;
		case 13u: goto tr11;
		case 42u: goto tr12;
	}
	goto tr9;
tr12:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 6240 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st4;
		case 13u: goto st5;
		case 42u: goto st6;
		case 47u: goto st25;
	}
	goto st3;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 10u: goto st26;
		case 13u: goto st27;
	}
	goto st7;
tr70:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr103:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr134:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr166:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr197:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr228:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr259:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr290:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr321:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr352:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr383:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr414:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr445:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr476:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr507:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr538:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr569:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr598:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr629:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr663:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr695:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr726:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr757:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr788:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr819:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr850:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr881:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr912:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr943:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1003:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1034:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1065:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1096:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1127:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1158:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1189:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1220:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1251:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1282:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1313:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1344:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1375:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1406:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1437:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1468:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1499:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1530:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1562:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1593:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1624:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1655:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1686:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1717:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1748:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1779:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1810:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 6717 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr627;
		case 47u: goto tr628;
		case 48u: goto tr629;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 69u: goto tr638;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 101u: goto tr638;
		case 120u: goto tr642;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr630;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr637;
	} else
		goto tr637;
	goto tr1;
tr627:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 6771 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr17;
	goto tr1;
tr17:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 6783 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr647;
		case 47u: goto tr628;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 69u: goto tr638;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 101u: goto tr638;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr17;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr637;
	} else
		goto tr637;
	goto tr1;
tr72:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr105:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr136:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr168:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr199:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr230:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr261:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr292:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr323:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr354:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr385:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr416:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr447:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr478:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr509:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr540:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr571:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr600:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr631:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr697:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr728:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr759:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr790:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr821:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr852:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr883:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr914:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr945:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr975:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1005:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1036:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1067:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1098:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1129:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1160:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1191:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1222:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1253:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1284:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1315:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1346:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1377:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1408:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1439:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1470:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1501:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1532:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1564:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1595:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1626:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1657:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1688:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1719:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1750:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1781:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1812:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 7178 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr648;
		case 10u: goto tr649;
		case 13u: goto tr650;
		case 32u: goto tr648;
		case 33u: goto tr651;
		case 34u: goto tr652;
		case 37u: goto tr653;
		case 38u: goto tr654;
		case 40u: goto tr655;
		case 41u: goto tr656;
		case 42u: goto tr657;
		case 43u: goto tr658;
		case 44u: goto tr659;
		case 45u: goto tr660;
		case 46u: goto tr661;
		case 47u: goto tr662;
		case 48u: goto tr663;
		case 58u: goto st76;
		case 59u: goto tr666;
		case 60u: goto tr667;
		case 61u: goto tr668;
		case 62u: goto tr669;
		case 63u: goto tr670;
		case 91u: goto tr672;
		case 93u: goto tr673;
		case 94u: goto tr674;
		case 95u: goto tr671;
		case 123u: goto tr675;
		case 124u: goto tr676;
		case 125u: goto tr677;
		case 126u: goto tr678;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr664;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr671;
	} else
		goto tr671;
	goto tr1;
tr679:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr71:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr104:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr135:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr167:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr198:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr229:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr260:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr291:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr322:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr353:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr384:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr415:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr446:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr477:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr508:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr539:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr570:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr599:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr630:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr664:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr696:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr727:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr758:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr789:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr820:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr851:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr882:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr913:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr944:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1004:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1035:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1066:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1097:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1128:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1159:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1190:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1221:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1252:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1283:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1314:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1345:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1376:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1407:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1438:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1469:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1500:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1531:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1563:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1594:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1625:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1656:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1687:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1718:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1749:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1780:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
tr1811:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st47;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
#line 7685 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr627;
		case 47u: goto tr628;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 69u: goto tr638;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 101u: goto tr638;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr679;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr637;
	} else
		goto tr637;
	goto tr1;
tr73:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr106:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr137:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr169:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr200:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr231:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr262:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr293:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr324:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr355:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr386:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr417:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr448:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr479:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr510:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr541:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr572:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr601:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr632:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr666:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr698:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr729:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr760:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr791:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr822:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr853:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr884:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr915:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr946:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr976:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1006:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1037:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1068:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1099:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1130:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1161:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1192:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1223:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1254:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1285:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1316:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1347:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1378:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1409:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1440:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1471:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1502:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1533:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1565:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1596:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1627:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1658:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1689:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1720:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1751:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1782:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1813:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
#line 8086 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr680;
		case 10u: goto tr681;
		case 13u: goto tr682;
		case 32u: goto tr680;
		case 33u: goto tr683;
		case 34u: goto tr684;
		case 37u: goto tr685;
		case 38u: goto tr686;
		case 40u: goto tr687;
		case 41u: goto tr688;
		case 42u: goto tr689;
		case 43u: goto tr690;
		case 44u: goto tr691;
		case 45u: goto tr692;
		case 46u: goto tr693;
		case 47u: goto tr694;
		case 48u: goto tr695;
		case 58u: goto tr697;
		case 59u: goto tr698;
		case 60u: goto tr699;
		case 61u: goto tr700;
		case 62u: goto tr701;
		case 63u: goto tr702;
		case 91u: goto tr704;
		case 93u: goto tr705;
		case 94u: goto tr706;
		case 95u: goto tr703;
		case 123u: goto tr707;
		case 124u: goto tr708;
		case 125u: goto tr709;
		case 126u: goto tr710;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr696;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr703;
	} else
		goto tr703;
	goto tr1;
tr74:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr107:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr138:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr170:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr201:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr232:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr263:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr294:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr325:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr356:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr387:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr418:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr449:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr480:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr511:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr542:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr573:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr602:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr633:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr667:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr699:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr761:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr792:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr823:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr854:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr885:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr916:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr947:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr977:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1007:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1038:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1069:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1100:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1131:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1162:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1193:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1224:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1255:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1286:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1317:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1348:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1379:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1410:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1441:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1472:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1503:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1534:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1566:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1597:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1628:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1659:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1690:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1721:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1752:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1783:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1814:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
#line 8480 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr711;
		case 10u: goto tr712;
		case 13u: goto tr713;
		case 32u: goto tr711;
		case 33u: goto tr714;
		case 34u: goto tr715;
		case 37u: goto tr716;
		case 38u: goto tr717;
		case 40u: goto tr718;
		case 41u: goto tr719;
		case 42u: goto tr720;
		case 43u: goto tr721;
		case 44u: goto tr722;
		case 45u: goto tr723;
		case 46u: goto tr724;
		case 47u: goto tr725;
		case 48u: goto tr726;
		case 58u: goto tr728;
		case 59u: goto tr729;
		case 60u: goto st50;
		case 61u: goto st74;
		case 62u: goto tr732;
		case 63u: goto tr733;
		case 91u: goto tr735;
		case 93u: goto tr736;
		case 94u: goto tr737;
		case 95u: goto tr734;
		case 123u: goto tr738;
		case 124u: goto tr739;
		case 125u: goto tr740;
		case 126u: goto tr741;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr727;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr734;
	} else
		goto tr734;
	goto tr1;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9u: goto tr742;
		case 10u: goto tr743;
		case 13u: goto tr744;
		case 32u: goto tr742;
		case 33u: goto tr745;
		case 34u: goto tr746;
		case 37u: goto tr747;
		case 38u: goto tr748;
		case 40u: goto tr749;
		case 41u: goto tr750;
		case 42u: goto tr751;
		case 43u: goto tr752;
		case 44u: goto tr753;
		case 45u: goto tr754;
		case 46u: goto tr755;
		case 47u: goto tr756;
		case 48u: goto tr757;
		case 58u: goto tr759;
		case 59u: goto tr760;
		case 60u: goto tr761;
		case 61u: goto st51;
		case 62u: goto tr763;
		case 63u: goto tr764;
		case 91u: goto tr766;
		case 93u: goto tr767;
		case 94u: goto tr768;
		case 95u: goto tr765;
		case 123u: goto tr769;
		case 124u: goto tr770;
		case 125u: goto tr771;
		case 126u: goto tr772;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr758;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr765;
	} else
		goto tr765;
	goto tr1;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	switch( (*p) ) {
		case 9u: goto tr773;
		case 10u: goto tr774;
		case 13u: goto tr775;
		case 32u: goto tr773;
		case 33u: goto tr776;
		case 34u: goto tr777;
		case 37u: goto tr778;
		case 38u: goto tr779;
		case 40u: goto tr780;
		case 41u: goto tr781;
		case 42u: goto tr782;
		case 43u: goto tr783;
		case 44u: goto tr784;
		case 45u: goto tr785;
		case 46u: goto tr786;
		case 47u: goto tr787;
		case 48u: goto tr788;
		case 58u: goto tr790;
		case 59u: goto tr791;
		case 60u: goto tr792;
		case 61u: goto tr793;
		case 62u: goto tr794;
		case 63u: goto tr795;
		case 91u: goto tr797;
		case 93u: goto tr798;
		case 94u: goto tr799;
		case 95u: goto tr796;
		case 123u: goto tr800;
		case 124u: goto tr801;
		case 125u: goto tr802;
		case 126u: goto tr803;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr789;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr796;
	} else
		goto tr796;
	goto tr1;
tr75:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr108:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr171:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr264:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr295:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr326:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr419:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr450:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr512:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr543:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr574:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr634:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr668:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr700:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr793:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr855:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr917:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr948:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr978:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1008:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1039:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1101:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1132:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1194:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1225:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1287:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1318:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1349:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1411:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1473:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1504:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1535:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1567:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1598:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1629:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1660:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1691:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1722:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1753:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1784:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1815:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
#line 8876 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr804;
		case 10u: goto tr805;
		case 13u: goto tr806;
		case 32u: goto tr804;
		case 33u: goto tr807;
		case 34u: goto tr808;
		case 37u: goto tr809;
		case 38u: goto tr810;
		case 40u: goto tr811;
		case 41u: goto tr812;
		case 42u: goto tr813;
		case 43u: goto tr814;
		case 44u: goto tr815;
		case 45u: goto tr816;
		case 46u: goto tr817;
		case 47u: goto tr818;
		case 48u: goto tr819;
		case 58u: goto tr821;
		case 59u: goto tr822;
		case 60u: goto tr823;
		case 61u: goto st53;
		case 62u: goto tr825;
		case 63u: goto tr826;
		case 91u: goto tr828;
		case 93u: goto tr829;
		case 94u: goto tr830;
		case 95u: goto tr827;
		case 123u: goto tr831;
		case 124u: goto tr832;
		case 125u: goto tr833;
		case 126u: goto tr834;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr820;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr827;
	} else
		goto tr827;
	goto tr1;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 9u: goto tr835;
		case 10u: goto tr836;
		case 13u: goto tr837;
		case 32u: goto tr835;
		case 33u: goto tr838;
		case 34u: goto tr839;
		case 37u: goto tr840;
		case 38u: goto tr841;
		case 40u: goto tr842;
		case 41u: goto tr843;
		case 42u: goto tr844;
		case 43u: goto tr845;
		case 44u: goto tr846;
		case 45u: goto tr847;
		case 46u: goto tr848;
		case 47u: goto tr849;
		case 48u: goto tr850;
		case 58u: goto tr852;
		case 59u: goto tr853;
		case 60u: goto tr854;
		case 61u: goto tr855;
		case 62u: goto tr856;
		case 63u: goto tr857;
		case 91u: goto tr859;
		case 93u: goto tr860;
		case 94u: goto tr861;
		case 95u: goto tr858;
		case 123u: goto tr862;
		case 124u: goto tr863;
		case 125u: goto tr864;
		case 126u: goto tr865;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr851;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr858;
	} else
		goto tr858;
	goto tr1;
tr76:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr109:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr140:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr172:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr203:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr234:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr265:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr296:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr327:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr358:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr389:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr420:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr451:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr482:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr513:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr544:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr575:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr604:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr635:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr669:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr701:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr732:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr763:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr794:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr825:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr856:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr918:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr949:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr979:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1009:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1040:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1071:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1102:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1133:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1164:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1195:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1226:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1257:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1288:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1319:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1350:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1412:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1443:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1474:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1505:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1536:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1568:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1599:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1630:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1661:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1692:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1723:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1754:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1785:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1816:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
#line 9310 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr866;
		case 10u: goto tr867;
		case 13u: goto tr868;
		case 32u: goto tr866;
		case 33u: goto tr869;
		case 34u: goto tr870;
		case 37u: goto tr871;
		case 38u: goto tr872;
		case 40u: goto tr873;
		case 41u: goto tr874;
		case 42u: goto tr875;
		case 43u: goto tr876;
		case 44u: goto tr877;
		case 45u: goto tr878;
		case 46u: goto tr879;
		case 47u: goto tr880;
		case 48u: goto tr881;
		case 58u: goto tr883;
		case 59u: goto tr884;
		case 60u: goto tr885;
		case 61u: goto st55;
		case 62u: goto st70;
		case 63u: goto tr888;
		case 91u: goto tr890;
		case 93u: goto tr891;
		case 94u: goto tr892;
		case 95u: goto tr889;
		case 123u: goto tr893;
		case 124u: goto tr894;
		case 125u: goto tr895;
		case 126u: goto tr896;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr882;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr889;
	} else
		goto tr889;
	goto tr1;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	switch( (*p) ) {
		case 9u: goto tr897;
		case 10u: goto tr898;
		case 13u: goto tr899;
		case 32u: goto tr897;
		case 33u: goto tr900;
		case 34u: goto tr901;
		case 37u: goto tr902;
		case 38u: goto tr903;
		case 40u: goto tr904;
		case 41u: goto tr905;
		case 42u: goto tr906;
		case 43u: goto tr907;
		case 44u: goto tr908;
		case 45u: goto tr909;
		case 46u: goto tr910;
		case 47u: goto tr911;
		case 48u: goto tr912;
		case 58u: goto tr914;
		case 59u: goto tr915;
		case 60u: goto tr916;
		case 61u: goto tr917;
		case 62u: goto tr918;
		case 63u: goto tr919;
		case 91u: goto tr921;
		case 93u: goto tr922;
		case 94u: goto tr923;
		case 95u: goto tr920;
		case 123u: goto tr924;
		case 124u: goto tr925;
		case 125u: goto tr926;
		case 126u: goto tr927;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr913;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr920;
	} else
		goto tr920;
	goto tr1;
tr77:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr110:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr141:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr173:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr204:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr235:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr266:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr297:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr328:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr359:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr390:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr421:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr452:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr483:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr514:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr545:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr576:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr605:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr636:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr670:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr702:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr733:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr764:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr795:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr826:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr857:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr888:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr919:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr950:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr980:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1010:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1041:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1072:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1103:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1134:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1165:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1196:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1227:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1258:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1289:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1320:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1351:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1382:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1413:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1444:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1475:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1506:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1537:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1569:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1600:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1631:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1662:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1693:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1724:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1755:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1786:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1817:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
#line 9756 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr928;
		case 10u: goto tr929;
		case 13u: goto tr930;
		case 32u: goto tr928;
		case 33u: goto tr931;
		case 34u: goto tr932;
		case 37u: goto tr933;
		case 38u: goto tr934;
		case 40u: goto tr935;
		case 41u: goto tr936;
		case 42u: goto tr937;
		case 43u: goto tr938;
		case 44u: goto tr939;
		case 45u: goto tr940;
		case 46u: goto tr941;
		case 47u: goto tr942;
		case 48u: goto tr943;
		case 58u: goto tr945;
		case 59u: goto tr946;
		case 60u: goto tr947;
		case 61u: goto tr948;
		case 62u: goto tr949;
		case 63u: goto tr950;
		case 91u: goto tr952;
		case 93u: goto tr953;
		case 94u: goto tr954;
		case 95u: goto tr951;
		case 123u: goto tr955;
		case 124u: goto tr956;
		case 125u: goto tr957;
		case 126u: goto tr958;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr944;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr951;
	} else
		goto tr951;
	goto tr1;
tr974:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr78:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr111:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr142:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr174:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr205:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr236:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr267:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr298:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr329:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr360:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr391:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr422:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr453:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr484:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr515:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr546:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr577:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr606:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr637:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr671:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr703:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr734:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr765:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr796:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr827:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr858:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr889:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr920:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr951:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1011:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1042:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1073:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1104:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1135:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1166:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1197:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1228:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1259:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1290:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1321:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1352:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1383:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1414:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1445:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1476:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1507:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1538:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1570:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1601:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1632:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1663:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1694:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1725:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1756:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1787:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
tr1818:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 10263 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr959;
		case 10u: goto tr960;
		case 13u: goto tr961;
		case 32u: goto tr959;
		case 33u: goto tr962;
		case 34u: goto tr963;
		case 37u: goto tr964;
		case 38u: goto tr965;
		case 40u: goto tr966;
		case 41u: goto tr967;
		case 42u: goto tr968;
		case 43u: goto tr969;
		case 44u: goto tr970;
		case 45u: goto tr971;
		case 46u: goto tr972;
		case 47u: goto tr973;
		case 58u: goto tr975;
		case 59u: goto tr976;
		case 60u: goto tr977;
		case 61u: goto tr978;
		case 62u: goto tr979;
		case 63u: goto tr980;
		case 91u: goto tr981;
		case 93u: goto tr982;
		case 94u: goto tr983;
		case 95u: goto tr974;
		case 123u: goto tr984;
		case 124u: goto tr985;
		case 125u: goto tr986;
		case 126u: goto tr987;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr974;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr974;
	} else
		goto tr974;
	goto tr1;
tr79:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr112:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr143:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr175:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr206:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr237:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr268:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr299:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr330:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr361:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr392:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr423:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr454:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr485:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr516:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr547:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr578:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr607:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr639:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr672:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr704:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr735:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr766:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr797:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr828:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr859:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr890:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr921:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr952:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr981:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1012:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1043:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1074:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1105:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1136:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1167:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1198:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1229:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1260:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1291:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1322:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1353:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1384:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1415:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1446:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1477:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1508:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1539:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1571:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1602:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1633:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1664:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1695:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1726:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1757:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1788:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1819:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
#line 10662 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr988;
		case 10u: goto tr989;
		case 13u: goto tr990;
		case 32u: goto tr988;
		case 33u: goto tr991;
		case 34u: goto tr992;
		case 37u: goto tr993;
		case 38u: goto tr994;
		case 40u: goto tr995;
		case 41u: goto tr996;
		case 42u: goto tr997;
		case 43u: goto tr998;
		case 44u: goto tr999;
		case 45u: goto tr1000;
		case 46u: goto tr1001;
		case 47u: goto tr1002;
		case 48u: goto tr1003;
		case 58u: goto tr1005;
		case 59u: goto tr1006;
		case 60u: goto tr1007;
		case 61u: goto tr1008;
		case 62u: goto tr1009;
		case 63u: goto tr1010;
		case 91u: goto tr1012;
		case 93u: goto tr1013;
		case 94u: goto tr1014;
		case 95u: goto tr1011;
		case 123u: goto tr1015;
		case 124u: goto tr1016;
		case 125u: goto tr1017;
		case 126u: goto tr1018;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1004;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1011;
	} else
		goto tr1011;
	goto tr1;
tr80:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr113:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr144:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr176:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr207:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr238:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr269:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr300:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr331:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr362:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr393:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr424:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr455:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr486:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr517:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr548:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr579:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr608:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr640:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr673:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr705:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr736:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr767:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr798:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr829:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr860:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr891:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr922:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr953:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr982:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1013:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1044:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1075:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1106:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1137:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1168:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1199:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1230:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1261:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1292:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1323:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1354:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1385:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1416:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1447:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1478:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1509:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1540:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1572:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1603:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1634:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1665:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1696:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1727:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1758:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1789:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1820:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 11062 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1019;
		case 10u: goto tr1020;
		case 13u: goto tr1021;
		case 32u: goto tr1019;
		case 33u: goto tr1022;
		case 34u: goto tr1023;
		case 37u: goto tr1024;
		case 38u: goto tr1025;
		case 40u: goto tr1026;
		case 41u: goto tr1027;
		case 42u: goto tr1028;
		case 43u: goto tr1029;
		case 44u: goto tr1030;
		case 45u: goto tr1031;
		case 46u: goto tr1032;
		case 47u: goto tr1033;
		case 48u: goto tr1034;
		case 58u: goto tr1036;
		case 59u: goto tr1037;
		case 60u: goto tr1038;
		case 61u: goto tr1039;
		case 62u: goto tr1040;
		case 63u: goto tr1041;
		case 91u: goto tr1043;
		case 93u: goto tr1044;
		case 94u: goto tr1045;
		case 95u: goto tr1042;
		case 123u: goto tr1046;
		case 124u: goto tr1047;
		case 125u: goto tr1048;
		case 126u: goto tr1049;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1035;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1042;
	} else
		goto tr1042;
	goto tr1;
tr81:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr114:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr145:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr177:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr208:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr239:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr270:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr301:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr332:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr363:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr394:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr425:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr456:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr487:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr518:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr549:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr580:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr609:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr641:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr674:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr706:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr737:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr768:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr799:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr830:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr861:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr892:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr923:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr954:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr983:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1014:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1045:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1107:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1138:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1169:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1200:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1231:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1262:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1293:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1324:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1355:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1386:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1417:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1448:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1479:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1510:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1541:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1573:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1604:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1635:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1666:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1697:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1728:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1759:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1790:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1821:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 11456 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1050;
		case 10u: goto tr1051;
		case 13u: goto tr1052;
		case 32u: goto tr1050;
		case 33u: goto tr1053;
		case 34u: goto tr1054;
		case 37u: goto tr1055;
		case 38u: goto tr1056;
		case 40u: goto tr1057;
		case 41u: goto tr1058;
		case 42u: goto tr1059;
		case 43u: goto tr1060;
		case 44u: goto tr1061;
		case 45u: goto tr1062;
		case 46u: goto tr1063;
		case 47u: goto tr1064;
		case 48u: goto tr1065;
		case 58u: goto tr1067;
		case 59u: goto tr1068;
		case 60u: goto tr1069;
		case 61u: goto st61;
		case 62u: goto tr1071;
		case 63u: goto tr1072;
		case 91u: goto tr1074;
		case 93u: goto tr1075;
		case 94u: goto st69;
		case 95u: goto tr1073;
		case 123u: goto tr1077;
		case 124u: goto tr1078;
		case 125u: goto tr1079;
		case 126u: goto tr1080;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1066;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1073;
	} else
		goto tr1073;
	goto tr1;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 9u: goto tr1081;
		case 10u: goto tr1082;
		case 13u: goto tr1083;
		case 32u: goto tr1081;
		case 33u: goto tr1084;
		case 34u: goto tr1085;
		case 37u: goto tr1086;
		case 38u: goto tr1087;
		case 40u: goto tr1088;
		case 41u: goto tr1089;
		case 42u: goto tr1090;
		case 43u: goto tr1091;
		case 44u: goto tr1092;
		case 45u: goto tr1093;
		case 46u: goto tr1094;
		case 47u: goto tr1095;
		case 48u: goto tr1096;
		case 58u: goto tr1098;
		case 59u: goto tr1099;
		case 60u: goto tr1100;
		case 61u: goto tr1101;
		case 62u: goto tr1102;
		case 63u: goto tr1103;
		case 91u: goto tr1105;
		case 93u: goto tr1106;
		case 94u: goto tr1107;
		case 95u: goto tr1104;
		case 123u: goto tr1108;
		case 124u: goto tr1109;
		case 125u: goto tr1110;
		case 126u: goto tr1111;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1097;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1104;
	} else
		goto tr1104;
	goto tr1;
tr82:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr115:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr147:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr178:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr209:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr240:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr271:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr302:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr333:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr364:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr395:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr426:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr457:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr488:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr519:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr550:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr581:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr610:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr643:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr675:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr707:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr738:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr769:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr800:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr831:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr862:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr893:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr924:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr955:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr984:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1015:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1046:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1077:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1108:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1139:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1170:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1201:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1232:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1263:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1294:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1325:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1356:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1387:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1418:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1449:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1480:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1511:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1542:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1574:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1605:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1636:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1667:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1698:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1729:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1760:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1791:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1822:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 11902 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1112;
		case 10u: goto tr1113;
		case 13u: goto tr1114;
		case 32u: goto tr1112;
		case 33u: goto tr1115;
		case 34u: goto tr1116;
		case 37u: goto tr1117;
		case 38u: goto tr1118;
		case 40u: goto tr1119;
		case 41u: goto tr1120;
		case 42u: goto tr1121;
		case 43u: goto tr1122;
		case 44u: goto tr1123;
		case 45u: goto tr1124;
		case 46u: goto tr1125;
		case 47u: goto tr1126;
		case 48u: goto tr1127;
		case 58u: goto tr1129;
		case 59u: goto tr1130;
		case 60u: goto tr1131;
		case 61u: goto tr1132;
		case 62u: goto tr1133;
		case 63u: goto tr1134;
		case 91u: goto tr1136;
		case 93u: goto tr1137;
		case 94u: goto tr1138;
		case 95u: goto tr1135;
		case 123u: goto tr1139;
		case 124u: goto tr1140;
		case 125u: goto tr1141;
		case 126u: goto tr1142;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1128;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1135;
	} else
		goto tr1135;
	goto tr1;
tr83:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr116:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr148:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr179:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr210:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr241:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr272:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr303:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr334:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr365:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr396:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr427:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr458:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr489:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr520:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr551:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr582:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr611:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr644:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr676:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr708:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr739:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr770:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr801:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr832:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr863:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr894:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr925:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr956:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr985:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1016:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1047:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1078:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1109:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1140:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1202:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1233:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1264:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1295:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1326:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1357:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1388:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1419:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1450:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1481:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1512:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1543:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1575:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1606:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1637:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1668:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1699:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1730:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1761:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1792:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1823:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
#line 12296 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1143;
		case 10u: goto tr1144;
		case 13u: goto tr1145;
		case 32u: goto tr1143;
		case 33u: goto tr1146;
		case 34u: goto tr1147;
		case 37u: goto tr1148;
		case 38u: goto tr1149;
		case 40u: goto tr1150;
		case 41u: goto tr1151;
		case 42u: goto tr1152;
		case 43u: goto tr1153;
		case 44u: goto tr1154;
		case 45u: goto tr1155;
		case 46u: goto tr1156;
		case 47u: goto tr1157;
		case 48u: goto tr1158;
		case 58u: goto tr1160;
		case 59u: goto tr1161;
		case 60u: goto tr1162;
		case 61u: goto st64;
		case 62u: goto tr1164;
		case 63u: goto tr1165;
		case 91u: goto tr1167;
		case 93u: goto tr1168;
		case 94u: goto tr1169;
		case 95u: goto tr1166;
		case 123u: goto tr1170;
		case 124u: goto st68;
		case 125u: goto tr1172;
		case 126u: goto tr1173;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1159;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1166;
	} else
		goto tr1166;
	goto tr1;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 9u: goto tr1174;
		case 10u: goto tr1175;
		case 13u: goto tr1176;
		case 32u: goto tr1174;
		case 33u: goto tr1177;
		case 34u: goto tr1178;
		case 37u: goto tr1179;
		case 38u: goto tr1180;
		case 40u: goto tr1181;
		case 41u: goto tr1182;
		case 42u: goto tr1183;
		case 43u: goto tr1184;
		case 44u: goto tr1185;
		case 45u: goto tr1186;
		case 46u: goto tr1187;
		case 47u: goto tr1188;
		case 48u: goto tr1189;
		case 58u: goto tr1191;
		case 59u: goto tr1192;
		case 60u: goto tr1193;
		case 61u: goto tr1194;
		case 62u: goto tr1195;
		case 63u: goto tr1196;
		case 91u: goto tr1198;
		case 93u: goto tr1199;
		case 94u: goto tr1200;
		case 95u: goto tr1197;
		case 123u: goto tr1201;
		case 124u: goto tr1202;
		case 125u: goto tr1203;
		case 126u: goto tr1204;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1190;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1197;
	} else
		goto tr1197;
	goto tr1;
tr84:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr117:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr149:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr180:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr211:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr242:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr273:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr304:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr335:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr366:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr397:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr428:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr459:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr490:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr521:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr552:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr583:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr612:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr645:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr677:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr709:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr740:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr771:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr802:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr833:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr864:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr895:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr926:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr957:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr986:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1017:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1048:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1079:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1110:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1141:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1172:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1203:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1234:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1265:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1296:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1327:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1358:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1389:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1420:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1451:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1482:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1513:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1544:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1576:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1607:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1638:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1669:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1700:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1731:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1762:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1793:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1824:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 12742 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1205;
		case 10u: goto tr1206;
		case 13u: goto tr1207;
		case 32u: goto tr1205;
		case 33u: goto tr1208;
		case 34u: goto tr1209;
		case 37u: goto tr1210;
		case 38u: goto tr1211;
		case 40u: goto tr1212;
		case 41u: goto tr1213;
		case 42u: goto tr1214;
		case 43u: goto tr1215;
		case 44u: goto tr1216;
		case 45u: goto tr1217;
		case 46u: goto tr1218;
		case 47u: goto tr1219;
		case 48u: goto tr1220;
		case 58u: goto tr1222;
		case 59u: goto tr1223;
		case 60u: goto tr1224;
		case 61u: goto tr1225;
		case 62u: goto tr1226;
		case 63u: goto tr1227;
		case 91u: goto tr1229;
		case 93u: goto tr1230;
		case 94u: goto tr1231;
		case 95u: goto tr1228;
		case 123u: goto tr1232;
		case 124u: goto tr1233;
		case 125u: goto tr1234;
		case 126u: goto tr1235;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1221;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1228;
	} else
		goto tr1228;
	goto tr1;
tr85:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr118:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr150:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr181:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr212:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr243:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr274:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr305:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr336:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr367:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr398:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr429:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr460:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr491:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr522:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr553:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr584:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr613:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr646:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr678:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr710:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr741:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr772:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr803:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr834:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr865:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr896:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr927:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr958:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr987:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1018:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1049:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1080:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1111:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1142:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1173:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1204:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1235:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1266:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1297:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1328:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1359:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1390:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1421:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1452:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1483:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1514:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1545:
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1577:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1608:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1639:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1670:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1701:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1732:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1763:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1794:
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1825:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 13142 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1236;
		case 10u: goto tr1237;
		case 13u: goto tr1238;
		case 32u: goto tr1236;
		case 33u: goto tr1239;
		case 34u: goto tr1240;
		case 37u: goto tr1241;
		case 38u: goto tr1242;
		case 40u: goto tr1243;
		case 41u: goto tr1244;
		case 42u: goto tr1245;
		case 43u: goto tr1246;
		case 44u: goto tr1247;
		case 45u: goto tr1248;
		case 46u: goto tr1249;
		case 47u: goto tr1250;
		case 48u: goto tr1251;
		case 58u: goto tr1253;
		case 59u: goto tr1254;
		case 60u: goto tr1255;
		case 61u: goto st67;
		case 62u: goto tr1257;
		case 63u: goto tr1258;
		case 91u: goto tr1260;
		case 93u: goto tr1261;
		case 94u: goto tr1262;
		case 95u: goto tr1259;
		case 123u: goto tr1263;
		case 124u: goto tr1264;
		case 125u: goto tr1265;
		case 126u: goto tr1266;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1252;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1259;
	} else
		goto tr1259;
	goto tr1;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 9u: goto tr1267;
		case 10u: goto tr1268;
		case 13u: goto tr1269;
		case 32u: goto tr1267;
		case 33u: goto tr1270;
		case 34u: goto tr1271;
		case 37u: goto tr1272;
		case 38u: goto tr1273;
		case 40u: goto tr1274;
		case 41u: goto tr1275;
		case 42u: goto tr1276;
		case 43u: goto tr1277;
		case 44u: goto tr1278;
		case 45u: goto tr1279;
		case 46u: goto tr1280;
		case 47u: goto tr1281;
		case 48u: goto tr1282;
		case 58u: goto tr1284;
		case 59u: goto tr1285;
		case 60u: goto tr1286;
		case 61u: goto tr1287;
		case 62u: goto tr1288;
		case 63u: goto tr1289;
		case 91u: goto tr1291;
		case 93u: goto tr1292;
		case 94u: goto tr1293;
		case 95u: goto tr1290;
		case 123u: goto tr1294;
		case 124u: goto tr1295;
		case 125u: goto tr1296;
		case 126u: goto tr1297;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1283;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1290;
	} else
		goto tr1290;
	goto tr1;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 9u: goto tr1298;
		case 10u: goto tr1299;
		case 13u: goto tr1300;
		case 32u: goto tr1298;
		case 33u: goto tr1301;
		case 34u: goto tr1302;
		case 37u: goto tr1303;
		case 38u: goto tr1304;
		case 40u: goto tr1305;
		case 41u: goto tr1306;
		case 42u: goto tr1307;
		case 43u: goto tr1308;
		case 44u: goto tr1309;
		case 45u: goto tr1310;
		case 46u: goto tr1311;
		case 47u: goto tr1312;
		case 48u: goto tr1313;
		case 58u: goto tr1315;
		case 59u: goto tr1316;
		case 60u: goto tr1317;
		case 61u: goto tr1318;
		case 62u: goto tr1319;
		case 63u: goto tr1320;
		case 91u: goto tr1322;
		case 93u: goto tr1323;
		case 94u: goto tr1324;
		case 95u: goto tr1321;
		case 123u: goto tr1325;
		case 124u: goto tr1326;
		case 125u: goto tr1327;
		case 126u: goto tr1328;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1314;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1321;
	} else
		goto tr1321;
	goto tr1;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 9u: goto tr1329;
		case 10u: goto tr1330;
		case 13u: goto tr1331;
		case 32u: goto tr1329;
		case 33u: goto tr1332;
		case 34u: goto tr1333;
		case 37u: goto tr1334;
		case 38u: goto tr1335;
		case 40u: goto tr1336;
		case 41u: goto tr1337;
		case 42u: goto tr1338;
		case 43u: goto tr1339;
		case 44u: goto tr1340;
		case 45u: goto tr1341;
		case 46u: goto tr1342;
		case 47u: goto tr1343;
		case 48u: goto tr1344;
		case 58u: goto tr1346;
		case 59u: goto tr1347;
		case 60u: goto tr1348;
		case 61u: goto tr1349;
		case 62u: goto tr1350;
		case 63u: goto tr1351;
		case 91u: goto tr1353;
		case 93u: goto tr1354;
		case 94u: goto tr1355;
		case 95u: goto tr1352;
		case 123u: goto tr1356;
		case 124u: goto tr1357;
		case 125u: goto tr1358;
		case 126u: goto tr1359;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1345;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1352;
	} else
		goto tr1352;
	goto tr1;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 9u: goto tr1360;
		case 10u: goto tr1361;
		case 13u: goto tr1362;
		case 32u: goto tr1360;
		case 33u: goto tr1363;
		case 34u: goto tr1364;
		case 37u: goto tr1365;
		case 38u: goto tr1366;
		case 40u: goto tr1367;
		case 41u: goto tr1368;
		case 42u: goto tr1369;
		case 43u: goto tr1370;
		case 44u: goto tr1371;
		case 45u: goto tr1372;
		case 46u: goto tr1373;
		case 47u: goto tr1374;
		case 48u: goto tr1375;
		case 58u: goto tr1377;
		case 59u: goto tr1378;
		case 60u: goto tr1379;
		case 61u: goto st71;
		case 62u: goto st72;
		case 63u: goto tr1382;
		case 91u: goto tr1384;
		case 93u: goto tr1385;
		case 94u: goto tr1386;
		case 95u: goto tr1383;
		case 123u: goto tr1387;
		case 124u: goto tr1388;
		case 125u: goto tr1389;
		case 126u: goto tr1390;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1376;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1383;
	} else
		goto tr1383;
	goto tr1;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 9u: goto tr1391;
		case 10u: goto tr1392;
		case 13u: goto tr1393;
		case 32u: goto tr1391;
		case 33u: goto tr1394;
		case 34u: goto tr1395;
		case 37u: goto tr1396;
		case 38u: goto tr1397;
		case 40u: goto tr1398;
		case 41u: goto tr1399;
		case 42u: goto tr1400;
		case 43u: goto tr1401;
		case 44u: goto tr1402;
		case 45u: goto tr1403;
		case 46u: goto tr1404;
		case 47u: goto tr1405;
		case 48u: goto tr1406;
		case 58u: goto tr1408;
		case 59u: goto tr1409;
		case 60u: goto tr1410;
		case 61u: goto tr1411;
		case 62u: goto tr1412;
		case 63u: goto tr1413;
		case 91u: goto tr1415;
		case 93u: goto tr1416;
		case 94u: goto tr1417;
		case 95u: goto tr1414;
		case 123u: goto tr1418;
		case 124u: goto tr1419;
		case 125u: goto tr1420;
		case 126u: goto tr1421;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1407;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1414;
	} else
		goto tr1414;
	goto tr1;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 9u: goto tr1422;
		case 10u: goto tr1423;
		case 13u: goto tr1424;
		case 32u: goto tr1422;
		case 33u: goto tr1425;
		case 34u: goto tr1426;
		case 37u: goto tr1427;
		case 38u: goto tr1428;
		case 40u: goto tr1429;
		case 41u: goto tr1430;
		case 42u: goto tr1431;
		case 43u: goto tr1432;
		case 44u: goto tr1433;
		case 45u: goto tr1434;
		case 46u: goto tr1435;
		case 47u: goto tr1436;
		case 48u: goto tr1437;
		case 58u: goto tr1439;
		case 59u: goto tr1440;
		case 60u: goto tr1441;
		case 61u: goto st73;
		case 62u: goto tr1443;
		case 63u: goto tr1444;
		case 91u: goto tr1446;
		case 93u: goto tr1447;
		case 94u: goto tr1448;
		case 95u: goto tr1445;
		case 123u: goto tr1449;
		case 124u: goto tr1450;
		case 125u: goto tr1451;
		case 126u: goto tr1452;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1438;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1445;
	} else
		goto tr1445;
	goto tr1;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	switch( (*p) ) {
		case 9u: goto tr1453;
		case 10u: goto tr1454;
		case 13u: goto tr1455;
		case 32u: goto tr1453;
		case 33u: goto tr1456;
		case 34u: goto tr1457;
		case 37u: goto tr1458;
		case 38u: goto tr1459;
		case 40u: goto tr1460;
		case 41u: goto tr1461;
		case 42u: goto tr1462;
		case 43u: goto tr1463;
		case 44u: goto tr1464;
		case 45u: goto tr1465;
		case 46u: goto tr1466;
		case 47u: goto tr1467;
		case 48u: goto tr1468;
		case 58u: goto tr1470;
		case 59u: goto tr1471;
		case 60u: goto tr1472;
		case 61u: goto tr1473;
		case 62u: goto tr1474;
		case 63u: goto tr1475;
		case 91u: goto tr1477;
		case 93u: goto tr1478;
		case 94u: goto tr1479;
		case 95u: goto tr1476;
		case 123u: goto tr1480;
		case 124u: goto tr1481;
		case 125u: goto tr1482;
		case 126u: goto tr1483;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1469;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1476;
	} else
		goto tr1476;
	goto tr1;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	switch( (*p) ) {
		case 9u: goto tr1484;
		case 10u: goto tr1485;
		case 13u: goto tr1486;
		case 32u: goto tr1484;
		case 33u: goto tr1487;
		case 34u: goto tr1488;
		case 37u: goto tr1489;
		case 38u: goto tr1490;
		case 40u: goto tr1491;
		case 41u: goto tr1492;
		case 42u: goto tr1493;
		case 43u: goto tr1494;
		case 44u: goto tr1495;
		case 45u: goto tr1496;
		case 46u: goto tr1497;
		case 47u: goto tr1498;
		case 48u: goto tr1499;
		case 58u: goto tr1501;
		case 59u: goto tr1502;
		case 60u: goto tr1503;
		case 61u: goto tr1504;
		case 62u: goto tr1505;
		case 63u: goto tr1506;
		case 91u: goto tr1508;
		case 93u: goto tr1509;
		case 94u: goto tr1510;
		case 95u: goto tr1507;
		case 123u: goto tr1511;
		case 124u: goto tr1512;
		case 125u: goto tr1513;
		case 126u: goto tr1514;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1500;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1507;
	} else
		goto tr1507;
	goto tr1;
tr638:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 13561 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 43u: goto tr18;
		case 45u: goto tr18;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr19;
	goto tr1;
tr18:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 13577 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr19;
	goto tr1;
tr19:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st75;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
#line 13589 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr647;
		case 47u: goto tr628;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr19;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr637;
	} else
		goto tr637;
	goto tr1;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 9u: goto tr1515;
		case 10u: goto tr1516;
		case 13u: goto tr1517;
		case 32u: goto tr1515;
		case 33u: goto tr1518;
		case 34u: goto tr1519;
		case 37u: goto tr1520;
		case 38u: goto tr1521;
		case 40u: goto tr1522;
		case 41u: goto tr1523;
		case 42u: goto tr1524;
		case 43u: goto tr1525;
		case 44u: goto tr1526;
		case 45u: goto tr1527;
		case 46u: goto tr1528;
		case 47u: goto tr1529;
		case 48u: goto tr1530;
		case 58u: goto tr1532;
		case 59u: goto tr1533;
		case 60u: goto tr1534;
		case 61u: goto tr1535;
		case 62u: goto tr1536;
		case 63u: goto tr1537;
		case 91u: goto tr1539;
		case 93u: goto tr1540;
		case 94u: goto tr1541;
		case 95u: goto tr1538;
		case 123u: goto tr1542;
		case 124u: goto tr1543;
		case 125u: goto tr1544;
		case 126u: goto tr1545;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1531;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1538;
	} else
		goto tr1538;
	goto tr1;
tr642:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 13685 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr20;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr20;
	} else
		goto tr20;
	goto tr1;
tr20:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st77;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
#line 13703 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr1546;
		case 47u: goto tr628;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 80u: goto tr638;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 112u: goto tr638;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 71u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 70u )
				goto tr20;
		} else if ( (*p) >= 48u )
			goto tr20;
	} else if ( (*p) > 90u ) {
		if ( (*p) > 102u ) {
			if ( 103u <= (*p) && (*p) <= 122u )
				goto tr637;
		} else if ( (*p) >= 97u )
			goto tr20;
	} else
		goto tr637;
	goto tr1;
tr1546:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 13761 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr21;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr21;
	} else
		goto tr21;
	goto tr1;
tr21:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st78;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
#line 13779 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr614;
		case 10u: goto tr615;
		case 13u: goto tr616;
		case 32u: goto tr614;
		case 33u: goto tr617;
		case 34u: goto tr618;
		case 37u: goto tr619;
		case 38u: goto tr620;
		case 40u: goto tr621;
		case 41u: goto tr622;
		case 42u: goto tr623;
		case 43u: goto tr624;
		case 44u: goto tr625;
		case 45u: goto tr626;
		case 46u: goto tr647;
		case 47u: goto tr628;
		case 58u: goto tr631;
		case 59u: goto tr632;
		case 60u: goto tr633;
		case 61u: goto tr634;
		case 62u: goto tr635;
		case 63u: goto tr636;
		case 80u: goto tr638;
		case 91u: goto tr639;
		case 93u: goto tr640;
		case 94u: goto tr641;
		case 95u: goto tr637;
		case 112u: goto tr638;
		case 123u: goto tr643;
		case 124u: goto tr644;
		case 125u: goto tr645;
		case 126u: goto tr646;
	}
	if ( (*p) < 71u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 70u )
				goto tr21;
		} else if ( (*p) >= 48u )
			goto tr21;
	} else if ( (*p) > 90u ) {
		if ( (*p) > 102u ) {
			if ( 103u <= (*p) && (*p) <= 122u )
				goto tr637;
		} else if ( (*p) >= 97u )
			goto tr21;
	} else
		goto tr637;
	goto tr1;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 9u: goto tr1547;
		case 10u: goto tr1548;
		case 13u: goto tr1549;
		case 32u: goto tr1547;
		case 33u: goto tr1550;
		case 34u: goto tr1551;
		case 37u: goto tr1552;
		case 38u: goto tr1553;
		case 40u: goto tr1554;
		case 41u: goto tr1555;
		case 42u: goto tr1556;
		case 43u: goto tr1557;
		case 44u: goto tr1558;
		case 45u: goto tr1559;
		case 46u: goto tr1560;
		case 47u: goto tr1561;
		case 48u: goto tr1562;
		case 58u: goto tr1564;
		case 59u: goto tr1565;
		case 60u: goto tr1566;
		case 61u: goto tr1567;
		case 62u: goto tr1568;
		case 63u: goto tr1569;
		case 91u: goto tr1571;
		case 93u: goto tr1572;
		case 94u: goto tr1573;
		case 95u: goto tr1570;
		case 123u: goto tr1574;
		case 124u: goto tr1575;
		case 125u: goto tr1576;
		case 126u: goto tr1577;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1563;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1570;
	} else
		goto tr1570;
	goto tr1;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 9u: goto tr1578;
		case 10u: goto tr1579;
		case 13u: goto tr1580;
		case 32u: goto tr1578;
		case 33u: goto tr1581;
		case 34u: goto tr1582;
		case 37u: goto tr1583;
		case 38u: goto tr1584;
		case 40u: goto tr1585;
		case 41u: goto tr1586;
		case 42u: goto tr1587;
		case 43u: goto tr1588;
		case 44u: goto tr1589;
		case 45u: goto tr1590;
		case 46u: goto tr1591;
		case 47u: goto tr1592;
		case 48u: goto tr1593;
		case 58u: goto tr1595;
		case 59u: goto tr1596;
		case 60u: goto tr1597;
		case 61u: goto tr1598;
		case 62u: goto tr1599;
		case 63u: goto tr1600;
		case 91u: goto tr1602;
		case 93u: goto tr1603;
		case 94u: goto tr1604;
		case 95u: goto tr1601;
		case 123u: goto tr1605;
		case 124u: goto tr1606;
		case 125u: goto tr1607;
		case 126u: goto tr1608;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1594;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1601;
	} else
		goto tr1601;
	goto tr1;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	switch( (*p) ) {
		case 9u: goto tr1609;
		case 10u: goto tr1610;
		case 13u: goto tr1611;
		case 32u: goto tr1609;
		case 33u: goto tr1612;
		case 34u: goto tr1613;
		case 37u: goto tr1614;
		case 38u: goto tr1615;
		case 40u: goto tr1616;
		case 41u: goto tr1617;
		case 42u: goto tr1618;
		case 43u: goto tr1619;
		case 44u: goto tr1620;
		case 45u: goto tr1621;
		case 46u: goto tr1622;
		case 47u: goto tr1623;
		case 48u: goto tr1624;
		case 58u: goto tr1626;
		case 59u: goto tr1627;
		case 60u: goto tr1628;
		case 61u: goto tr1629;
		case 62u: goto tr1630;
		case 63u: goto tr1631;
		case 91u: goto tr1633;
		case 93u: goto tr1634;
		case 94u: goto tr1635;
		case 95u: goto tr1632;
		case 123u: goto tr1636;
		case 124u: goto tr1637;
		case 125u: goto tr1638;
		case 126u: goto tr1639;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1625;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1632;
	} else
		goto tr1632;
	goto tr1;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	switch( (*p) ) {
		case 9u: goto tr1640;
		case 10u: goto tr1641;
		case 13u: goto tr1642;
		case 32u: goto tr1640;
		case 33u: goto tr1643;
		case 34u: goto tr1644;
		case 37u: goto tr1645;
		case 38u: goto tr1646;
		case 40u: goto tr1647;
		case 41u: goto tr1648;
		case 42u: goto tr1649;
		case 43u: goto tr1650;
		case 44u: goto tr1651;
		case 45u: goto tr1652;
		case 46u: goto tr1653;
		case 47u: goto tr1654;
		case 48u: goto tr1655;
		case 58u: goto tr1657;
		case 59u: goto tr1658;
		case 60u: goto tr1659;
		case 61u: goto tr1660;
		case 62u: goto tr1661;
		case 63u: goto tr1662;
		case 91u: goto tr1664;
		case 93u: goto tr1665;
		case 94u: goto tr1666;
		case 95u: goto tr1663;
		case 123u: goto tr1667;
		case 124u: goto tr1668;
		case 125u: goto tr1669;
		case 126u: goto tr1670;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1656;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1663;
	} else
		goto tr1663;
	goto tr1;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	switch( (*p) ) {
		case 9u: goto tr1671;
		case 10u: goto tr1672;
		case 13u: goto tr1673;
		case 32u: goto tr1671;
		case 33u: goto tr1674;
		case 34u: goto tr1675;
		case 37u: goto tr1676;
		case 38u: goto tr1677;
		case 40u: goto tr1678;
		case 41u: goto tr1679;
		case 42u: goto tr1680;
		case 43u: goto tr1681;
		case 44u: goto tr1682;
		case 45u: goto tr1683;
		case 46u: goto tr1684;
		case 47u: goto tr1685;
		case 48u: goto tr1686;
		case 58u: goto tr1688;
		case 59u: goto tr1689;
		case 60u: goto tr1690;
		case 61u: goto tr1691;
		case 62u: goto tr1692;
		case 63u: goto tr1693;
		case 91u: goto tr1695;
		case 93u: goto tr1696;
		case 94u: goto tr1697;
		case 95u: goto tr1694;
		case 123u: goto tr1698;
		case 124u: goto tr1699;
		case 125u: goto tr1700;
		case 126u: goto tr1701;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1687;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1694;
	} else
		goto tr1694;
	goto tr1;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	switch( (*p) ) {
		case 9u: goto tr1702;
		case 10u: goto tr1703;
		case 13u: goto tr1704;
		case 32u: goto tr1702;
		case 33u: goto tr1705;
		case 34u: goto tr1706;
		case 37u: goto tr1707;
		case 38u: goto tr1708;
		case 40u: goto tr1709;
		case 41u: goto tr1710;
		case 42u: goto tr1711;
		case 43u: goto tr1712;
		case 44u: goto tr1713;
		case 45u: goto tr1714;
		case 46u: goto tr1715;
		case 47u: goto tr1716;
		case 48u: goto tr1717;
		case 58u: goto tr1719;
		case 59u: goto tr1720;
		case 60u: goto tr1721;
		case 61u: goto tr1722;
		case 62u: goto tr1723;
		case 63u: goto tr1724;
		case 91u: goto tr1726;
		case 93u: goto tr1727;
		case 94u: goto tr1728;
		case 95u: goto tr1725;
		case 123u: goto tr1729;
		case 124u: goto tr1730;
		case 125u: goto tr1731;
		case 126u: goto tr1732;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1718;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1725;
	} else
		goto tr1725;
	goto tr1;
tr3:
#line 97 "xec_parser_ragel.rl"
	{
                    uloc = (int)( offset + ( p - buffer ) );
                }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 14115 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 34u: goto tr22;
		case 47u: goto tr23;
		case 92u: goto tr24;
		case 98u: goto tr25;
		case 102u: goto tr26;
		case 110u: goto tr27;
		case 114u: goto tr28;
		case 116u: goto tr29;
		case 117u: goto st14;
		case 120u: goto st20;
	}
	goto tr1;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr32;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr34;
	} else
		goto tr33;
	goto tr1;
tr32:
#line 100 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 20; }
	goto st15;
tr33:
#line 102 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 20; }
	goto st15;
tr34:
#line 101 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 20; }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 14158 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr35;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr37;
	} else
		goto tr36;
	goto tr1;
tr35:
#line 104 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 16; }
	goto st16;
tr36:
#line 106 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 16; }
	goto st16;
tr37:
#line 105 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 16; }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 14184 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr38;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr40;
	} else
		goto tr39;
	goto tr1;
tr38:
#line 108 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 12; }
	goto st17;
tr39:
#line 110 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 12; }
	goto st17;
tr40:
#line 109 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 12; }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 14210 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr41;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr43;
	} else
		goto tr42;
	goto tr1;
tr41:
#line 112 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 8; }
	goto st18;
tr42:
#line 114 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 8; }
	goto st18;
tr43:
#line 113 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 8; }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 14236 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr44;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr46;
	} else
		goto tr45;
	goto tr1;
tr44:
#line 116 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 4; }
	goto st19;
tr45:
#line 118 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 4; }
	goto st19;
tr46:
#line 117 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 4; }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 14262 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr47;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr49;
	} else
		goto tr48;
	goto tr1;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr50;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr52;
	} else
		goto tr51;
	goto tr1;
tr50:
#line 87 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 4; }
	goto st21;
tr51:
#line 89 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 4; }
	goto st21;
tr52:
#line 88 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 4; }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 14301 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr55;
	} else
		goto tr54;
	goto tr1;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	switch( (*p) ) {
		case 9u: goto tr1733;
		case 10u: goto tr1734;
		case 13u: goto tr1735;
		case 32u: goto tr1733;
		case 33u: goto tr1736;
		case 34u: goto tr1737;
		case 37u: goto tr1738;
		case 38u: goto tr1739;
		case 40u: goto tr1740;
		case 41u: goto tr1741;
		case 42u: goto tr1742;
		case 43u: goto tr1743;
		case 44u: goto tr1744;
		case 45u: goto tr1745;
		case 46u: goto tr1746;
		case 47u: goto tr1747;
		case 48u: goto tr1748;
		case 58u: goto tr1750;
		case 59u: goto tr1751;
		case 60u: goto tr1752;
		case 61u: goto tr1753;
		case 62u: goto tr1754;
		case 63u: goto tr1755;
		case 91u: goto tr1757;
		case 93u: goto tr1758;
		case 94u: goto tr1759;
		case 95u: goto tr1756;
		case 123u: goto tr1760;
		case 124u: goto tr1761;
		case 125u: goto tr1762;
		case 126u: goto tr1763;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1749;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1756;
	} else
		goto tr1756;
	goto tr1;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 110u: goto st86;
		case 115u: goto st87;
	}
	goto tr1;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	switch( (*p) ) {
		case 9u: goto tr1764;
		case 10u: goto tr1765;
		case 13u: goto tr1766;
		case 32u: goto tr1764;
		case 33u: goto tr1767;
		case 34u: goto tr1768;
		case 37u: goto tr1769;
		case 38u: goto tr1770;
		case 40u: goto tr1771;
		case 41u: goto tr1772;
		case 42u: goto tr1773;
		case 43u: goto tr1774;
		case 44u: goto tr1775;
		case 45u: goto tr1776;
		case 46u: goto tr1777;
		case 47u: goto tr1778;
		case 48u: goto tr1779;
		case 58u: goto tr1781;
		case 59u: goto tr1782;
		case 60u: goto tr1783;
		case 61u: goto tr1784;
		case 62u: goto tr1785;
		case 63u: goto tr1786;
		case 91u: goto tr1788;
		case 93u: goto tr1789;
		case 94u: goto tr1790;
		case 95u: goto tr1787;
		case 123u: goto tr1791;
		case 124u: goto tr1792;
		case 125u: goto tr1793;
		case 126u: goto tr1794;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1780;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1787;
	} else
		goto tr1787;
	goto tr1;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	switch( (*p) ) {
		case 9u: goto tr1795;
		case 10u: goto tr1796;
		case 13u: goto tr1797;
		case 32u: goto tr1795;
		case 33u: goto tr1798;
		case 34u: goto tr1799;
		case 37u: goto tr1800;
		case 38u: goto tr1801;
		case 40u: goto tr1802;
		case 41u: goto tr1803;
		case 42u: goto tr1804;
		case 43u: goto tr1805;
		case 44u: goto tr1806;
		case 45u: goto tr1807;
		case 46u: goto tr1808;
		case 47u: goto tr1809;
		case 48u: goto tr1810;
		case 58u: goto tr1812;
		case 59u: goto tr1813;
		case 60u: goto tr1814;
		case 61u: goto tr1815;
		case 62u: goto tr1816;
		case 63u: goto tr1817;
		case 91u: goto tr1819;
		case 93u: goto tr1820;
		case 94u: goto tr1821;
		case 95u: goto tr1818;
		case 123u: goto tr1822;
		case 124u: goto tr1823;
		case 125u: goto tr1824;
		case 126u: goto tr1825;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1811;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1818;
	} else
		goto tr1818;
	goto tr1;
tr102:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 14466 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 42u: goto st3;
		case 47u: goto st7;
	}
	goto tr1;
	}
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 26: 
	case 27: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	break;
	case 57: 
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	break;
	case 44: 
	case 45: 
	case 47: 
	case 75: 
	case 77: 
	case 78: 
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	break;
	case 29: 
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	break;
	case 28: 
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	break;
	case 30: 
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	break;
	case 31: 
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	break;
	case 33: 
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	break;
	case 34: 
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	break;
	case 35: 
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	break;
	case 36: 
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	break;
	case 38: 
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	break;
	case 39: 
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	break;
	case 41: 
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	break;
	case 43: 
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	break;
	case 46: 
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	break;
	case 48: 
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	break;
	case 49: 
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	break;
	case 52: 
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	break;
	case 54: 
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	break;
	case 56: 
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	break;
	case 58: 
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	break;
	case 59: 
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	break;
	case 60: 
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	break;
	case 62: 
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	break;
	case 63: 
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	break;
	case 65: 
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	break;
	case 66: 
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	break;
	case 37: 
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	break;
	case 40: 
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	break;
	case 50: 
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	break;
	case 70: 
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	break;
	case 72: 
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	break;
	case 85: 
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	break;
	case 74: 
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	break;
	case 53: 
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	break;
	case 55: 
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	break;
	case 86: 
#line 212 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	break;
	case 87: 
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	break;
	case 84: 
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	break;
	case 83: 
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	break;
	case 82: 
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	break;
	case 81: 
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	break;
	case 80: 
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	break;
	case 79: 
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	break;
	case 61: 
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	break;
	case 64: 
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	break;
	case 67: 
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	break;
	case 51: 
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	break;
	case 71: 
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	break;
	case 73: 
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	break;
	case 32: 
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	break;
	case 69: 
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	break;
	case 68: 
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	break;
	case 76: 
#line 232 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	break;
	case 42: 
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	break;
	case 1: 
	case 2: 
	case 3: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
	case 12: 
	case 13: 
	case 14: 
	case 15: 
	case 16: 
	case 17: 
	case 18: 
	case 19: 
	case 20: 
	case 21: 
	case 22: 
	case 23: 
#line 34 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        diagnostic( sloc, "unexpected end of file" );
        {p++; cs = 0; goto _out;}
    }
#line 42 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
#line 239 "xec_parser_ragel.rl"
	{ {goto st24;} }
	break;
	case 4: 
	case 5: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 34 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        diagnostic( sloc, "unexpected end of file" );
        {p++; cs = 0; goto _out;}
    }
#line 42 "xec_parser_ragel.rl"
	{
        data.shrink();
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
#line 239 "xec_parser_ragel.rl"
	{ {goto st24;} }
	break;
#line 14875 "xec_parser_ragel.cpp"
	}
	}

	_out: {}
	}

#line 395 "xec_parser_ragel.rl"
        
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





