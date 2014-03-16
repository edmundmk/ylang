
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





#line 236 "xec_parser_ragel.rl"




#line 31 "xec_parser_ragel.cpp"
static const int lexer_start = 22;
static const int lexer_first_final = 22;
static const int lexer_error = 0;

static const int lexer_en_main = 22;


#line 240 "xec_parser_ragel.rl"



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

#line 363 "xec_parser_ragel.rl"
    
    
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
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 9u: goto st23;
		case 10u: goto st24;
		case 13u: goto st25;
		case 32u: goto st23;
		case 33u: goto tr55;
		case 34u: goto tr56;
		case 37u: goto tr57;
		case 38u: goto tr58;
		case 40u: goto tr59;
		case 41u: goto tr60;
		case 42u: goto tr61;
		case 43u: goto tr62;
		case 44u: goto tr63;
		case 45u: goto tr64;
		case 46u: goto tr65;
		case 47u: goto tr66;
		case 48u: goto tr67;
		case 58u: goto tr69;
		case 59u: goto tr70;
		case 60u: goto tr71;
		case 61u: goto tr72;
		case 62u: goto tr73;
		case 63u: goto tr74;
		case 91u: goto tr76;
		case 93u: goto tr77;
		case 94u: goto tr78;
		case 95u: goto tr75;
		case 123u: goto tr79;
		case 124u: goto tr80;
		case 125u: goto tr81;
		case 126u: goto tr82;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr68;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr75;
	} else
		goto tr75;
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
#line 234 "xec_parser_ragel.rl"
	{ {goto st22;} }
	goto st0;
tr84:
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
#line 234 "xec_parser_ragel.rl"
	{ {goto st22;} }
	goto st0;
#line 280 "xec_parser_ragel.cpp"
st0:
cs = 0;
	goto _out;
tr85:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st23;
tr116:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st23;
tr147:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st23;
tr178:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st23;
tr209:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st23;
tr240:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st23;
tr271:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st23;
tr302:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st23;
tr333:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st23;
tr364:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st23;
tr395:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st23;
tr426:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st23;
tr457:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st23;
tr488:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st23;
tr519:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st23;
tr550:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st23;
tr579:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st23;
tr613:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st23;
tr645:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st23;
tr676:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st23;
tr707:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st23;
tr738:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st23;
tr769:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st23;
tr800:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st23;
tr831:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st23;
tr862:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st23;
tr893:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st23;
tr924:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st23;
tr953:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st23;
tr984:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st23;
tr1015:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st23;
tr1046:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st23;
tr1077:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st23;
tr1108:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st23;
tr1139:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st23;
tr1170:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st23;
tr1201:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st23;
tr1232:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st23;
tr1263:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st23;
tr1294:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st23;
tr1325:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st23;
tr1356:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st23;
tr1387:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st23;
tr1418:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st23;
tr1449:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st23;
tr1481:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st23;
tr1512:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st23;
tr1543:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st23;
tr1574:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st23;
tr1605:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st23;
tr1636:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st23;
tr1667:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 509 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto st23;
		case 10u: goto st24;
		case 13u: goto st25;
		case 32u: goto st23;
		case 33u: goto tr55;
		case 34u: goto tr56;
		case 37u: goto tr57;
		case 38u: goto tr58;
		case 40u: goto tr59;
		case 41u: goto tr60;
		case 42u: goto tr61;
		case 43u: goto tr62;
		case 44u: goto tr63;
		case 45u: goto tr64;
		case 46u: goto tr65;
		case 47u: goto st21;
		case 48u: goto tr67;
		case 58u: goto tr69;
		case 59u: goto tr70;
		case 60u: goto tr71;
		case 61u: goto tr72;
		case 62u: goto tr73;
		case 63u: goto tr74;
		case 91u: goto tr76;
		case 93u: goto tr77;
		case 94u: goto tr78;
		case 95u: goto tr75;
		case 123u: goto tr79;
		case 124u: goto tr80;
		case 125u: goto tr81;
		case 126u: goto tr82;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr68;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr75;
	} else
		goto tr75;
	goto tr1;
tr86:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st24;
tr117:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st24;
tr148:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st24;
tr179:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st24;
tr210:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st24;
tr241:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st24;
tr272:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st24;
tr303:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st24;
tr334:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st24;
tr365:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st24;
tr396:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st24;
tr427:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st24;
tr458:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st24;
tr489:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st24;
tr520:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st24;
tr551:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st24;
tr580:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st24;
tr614:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st24;
tr646:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st24;
tr677:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st24;
tr708:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st24;
tr739:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st24;
tr770:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st24;
tr801:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st24;
tr832:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st24;
tr863:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st24;
tr894:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st24;
tr925:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st24;
tr954:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st24;
tr985:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st24;
tr1016:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st24;
tr1047:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st24;
tr1078:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st24;
tr1109:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st24;
tr1140:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st24;
tr1171:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st24;
tr1202:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st24;
tr1233:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st24;
tr1264:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st24;
tr1295:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st24;
tr1326:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st24;
tr1357:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st24;
tr1388:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st24;
tr1419:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st24;
tr1450:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st24;
tr1482:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st24;
tr1513:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st24;
tr1544:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st24;
tr1575:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st24;
tr1606:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st24;
tr1637:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st24;
tr1668:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 777 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr85;
		case 10u: goto tr86;
		case 13u: goto tr87;
		case 32u: goto tr85;
		case 33u: goto tr88;
		case 34u: goto tr89;
		case 37u: goto tr90;
		case 38u: goto tr91;
		case 40u: goto tr92;
		case 41u: goto tr93;
		case 42u: goto tr94;
		case 43u: goto tr95;
		case 44u: goto tr96;
		case 45u: goto tr97;
		case 46u: goto tr98;
		case 47u: goto tr99;
		case 48u: goto tr100;
		case 58u: goto tr102;
		case 59u: goto tr103;
		case 60u: goto tr104;
		case 61u: goto tr105;
		case 62u: goto tr106;
		case 63u: goto tr107;
		case 91u: goto tr109;
		case 93u: goto tr110;
		case 94u: goto tr111;
		case 95u: goto tr108;
		case 123u: goto tr112;
		case 124u: goto tr113;
		case 125u: goto tr114;
		case 126u: goto tr115;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr101;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr108;
	} else
		goto tr108;
	goto tr84;
tr87:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st25;
tr118:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st25;
tr149:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st25;
tr180:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st25;
tr211:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st25;
tr242:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st25;
tr273:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st25;
tr304:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st25;
tr335:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st25;
tr366:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st25;
tr397:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st25;
tr428:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st25;
tr459:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st25;
tr490:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st25;
tr521:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st25;
tr552:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st25;
tr581:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st25;
tr615:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st25;
tr647:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st25;
tr678:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st25;
tr709:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st25;
tr740:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st25;
tr771:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st25;
tr802:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st25;
tr833:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st25;
tr864:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st25;
tr895:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st25;
tr926:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st25;
tr955:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st25;
tr986:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st25;
tr1017:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st25;
tr1048:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st25;
tr1079:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st25;
tr1110:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st25;
tr1141:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st25;
tr1172:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st25;
tr1203:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st25;
tr1234:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st25;
tr1265:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st25;
tr1296:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st25;
tr1327:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st25;
tr1358:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st25;
tr1389:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st25;
tr1420:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st25;
tr1451:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st25;
tr1483:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st25;
tr1514:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st25;
tr1545:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st25;
tr1576:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st25;
tr1607:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st25;
tr1638:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st25;
tr1669:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 1045 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr85;
		case 10u: goto st24;
		case 13u: goto tr87;
		case 32u: goto tr85;
		case 33u: goto tr88;
		case 34u: goto tr89;
		case 37u: goto tr90;
		case 38u: goto tr91;
		case 40u: goto tr92;
		case 41u: goto tr93;
		case 42u: goto tr94;
		case 43u: goto tr95;
		case 44u: goto tr96;
		case 45u: goto tr97;
		case 46u: goto tr98;
		case 47u: goto tr99;
		case 48u: goto tr100;
		case 58u: goto tr102;
		case 59u: goto tr103;
		case 60u: goto tr104;
		case 61u: goto tr105;
		case 62u: goto tr106;
		case 63u: goto tr107;
		case 91u: goto tr109;
		case 93u: goto tr110;
		case 94u: goto tr111;
		case 95u: goto tr108;
		case 123u: goto tr112;
		case 124u: goto tr113;
		case 125u: goto tr114;
		case 126u: goto tr115;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr101;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr108;
	} else
		goto tr108;
	goto tr84;
tr55:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr88:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr119:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr150:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr181:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr212:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr243:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr274:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr305:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr336:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr367:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr398:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr429:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr460:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr491:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr522:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr553:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr582:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr616:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr648:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr679:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr710:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr741:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr772:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr803:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr834:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr865:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr896:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr927:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr956:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr987:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1018:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1049:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1080:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1111:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1142:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1173:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1204:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1235:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1266:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1297:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1328:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1359:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1390:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1421:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1452:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1484:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1515:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1546:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1577:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1608:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1639:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1670:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 1421 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr116;
		case 10u: goto tr117;
		case 13u: goto tr118;
		case 32u: goto tr116;
		case 33u: goto tr119;
		case 34u: goto tr120;
		case 37u: goto tr121;
		case 38u: goto tr122;
		case 40u: goto tr123;
		case 41u: goto tr124;
		case 42u: goto tr125;
		case 43u: goto tr126;
		case 44u: goto tr127;
		case 45u: goto tr128;
		case 46u: goto tr129;
		case 47u: goto tr130;
		case 48u: goto tr131;
		case 58u: goto tr133;
		case 59u: goto tr134;
		case 60u: goto tr135;
		case 61u: goto st81;
		case 62u: goto tr137;
		case 63u: goto tr138;
		case 91u: goto tr140;
		case 93u: goto tr141;
		case 94u: goto tr142;
		case 95u: goto tr139;
		case 123u: goto tr143;
		case 124u: goto tr144;
		case 125u: goto tr145;
		case 126u: goto tr146;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr132;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr139;
	} else
		goto tr139;
	goto tr1;
tr0:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st1;
tr21:
#line 78 "xec_parser_ragel.rl"
	{ data.append( '\"' ); }
	goto st1;
tr22:
#line 80 "xec_parser_ragel.rl"
	{ data.append( '/' ); }
	goto st1;
tr23:
#line 79 "xec_parser_ragel.rl"
	{ data.append( '\\' ); }
	goto st1;
tr24:
#line 81 "xec_parser_ragel.rl"
	{ data.append( '\b' ); }
	goto st1;
tr25:
#line 82 "xec_parser_ragel.rl"
	{ data.append( '\f' ); }
	goto st1;
tr26:
#line 83 "xec_parser_ragel.rl"
	{ data.append( '\n' ); }
	goto st1;
tr27:
#line 84 "xec_parser_ragel.rl"
	{ data.append( '\r' ); }
	goto st1;
tr28:
#line 85 "xec_parser_ragel.rl"
	{ data.append( '\t' ); }
	goto st1;
tr46:
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
tr47:
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
tr48:
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
tr52:
#line 91 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - '0' ) ); }
	goto st1;
tr53:
#line 93 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'A' + 0x0A ) ); }
	goto st1;
tr54:
#line 92 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'a' + 0x0A ) ); }
	goto st1;
tr56:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr89:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr120:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr151:
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
tr182:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr213:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr244:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr275:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr306:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr337:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr368:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr399:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr430:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr461:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr492:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr523:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr554:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr583:
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
tr617:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr649:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr680:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr711:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr742:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr773:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr804:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr835:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr866:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr897:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr928:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr957:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr988:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1019:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1050:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1081:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1112:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1143:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1174:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1205:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1236:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1267:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1298:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1329:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1360:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1391:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1422:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1453:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1485:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1516:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1547:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1578:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1609:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1640:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1671:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 1887 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr1;
		case 13u: goto tr1;
		case 34u: goto st27;
		case 92u: goto tr3;
	}
	goto tr0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	switch( (*p) ) {
		case 9u: goto tr147;
		case 10u: goto tr148;
		case 13u: goto tr149;
		case 32u: goto tr147;
		case 33u: goto tr150;
		case 34u: goto tr151;
		case 37u: goto tr152;
		case 38u: goto tr153;
		case 40u: goto tr154;
		case 41u: goto tr155;
		case 42u: goto tr156;
		case 43u: goto tr157;
		case 44u: goto tr158;
		case 45u: goto tr159;
		case 46u: goto tr160;
		case 47u: goto tr161;
		case 48u: goto tr162;
		case 58u: goto tr164;
		case 59u: goto tr165;
		case 60u: goto tr166;
		case 61u: goto tr167;
		case 62u: goto tr168;
		case 63u: goto tr169;
		case 91u: goto tr171;
		case 93u: goto tr172;
		case 94u: goto tr173;
		case 95u: goto tr170;
		case 123u: goto tr174;
		case 124u: goto tr175;
		case 125u: goto tr176;
		case 126u: goto tr177;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr163;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr170;
	} else
		goto tr170;
	goto tr1;
tr57:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr90:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr121:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr152:
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
tr183:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr214:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr245:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr276:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr307:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr338:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr369:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr400:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr431:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr462:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr493:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr524:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr555:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr584:
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
tr618:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr650:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr681:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr712:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr743:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr774:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr805:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr836:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr867:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr898:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr929:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr958:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr989:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1020:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1051:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1082:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1113:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1144:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1175:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1206:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1237:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1268:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1299:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1330:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1361:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1392:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1423:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1454:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1486:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1517:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1548:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1579:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1610:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1641:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1672:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 2274 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr178;
		case 10u: goto tr179;
		case 13u: goto tr180;
		case 32u: goto tr178;
		case 33u: goto tr181;
		case 34u: goto tr182;
		case 37u: goto tr183;
		case 38u: goto tr184;
		case 40u: goto tr185;
		case 41u: goto tr186;
		case 42u: goto tr187;
		case 43u: goto tr188;
		case 44u: goto tr189;
		case 45u: goto tr190;
		case 46u: goto tr191;
		case 47u: goto tr192;
		case 48u: goto tr193;
		case 58u: goto tr195;
		case 59u: goto tr196;
		case 60u: goto tr197;
		case 61u: goto st80;
		case 62u: goto tr199;
		case 63u: goto tr200;
		case 91u: goto tr202;
		case 93u: goto tr203;
		case 94u: goto tr204;
		case 95u: goto tr201;
		case 123u: goto tr205;
		case 124u: goto tr206;
		case 125u: goto tr207;
		case 126u: goto tr208;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr194;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr201;
	} else
		goto tr201;
	goto tr1;
tr58:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr91:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr122:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr153:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr184:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr246:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr277:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr308:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr339:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr370:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr401:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr432:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr463:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr494:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr525:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr556:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr585:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr619:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr651:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr682:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr713:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr744:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr775:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr806:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr837:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr868:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr899:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr930:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr959:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr990:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1021:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1052:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1083:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1114:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1145:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1176:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1207:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1238:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1269:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1300:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1331:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1362:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1393:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1424:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1455:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1487:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1518:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1549:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1580:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1611:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1642:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1673:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 2644 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr209;
		case 10u: goto tr210;
		case 13u: goto tr211;
		case 32u: goto tr209;
		case 33u: goto tr212;
		case 34u: goto tr213;
		case 37u: goto tr214;
		case 38u: goto st30;
		case 40u: goto tr216;
		case 41u: goto tr217;
		case 42u: goto tr218;
		case 43u: goto tr219;
		case 44u: goto tr220;
		case 45u: goto tr221;
		case 46u: goto tr222;
		case 47u: goto tr223;
		case 48u: goto tr224;
		case 58u: goto tr226;
		case 59u: goto tr227;
		case 60u: goto tr228;
		case 61u: goto st79;
		case 62u: goto tr230;
		case 63u: goto tr231;
		case 91u: goto tr233;
		case 93u: goto tr234;
		case 94u: goto tr235;
		case 95u: goto tr232;
		case 123u: goto tr236;
		case 124u: goto tr237;
		case 125u: goto tr238;
		case 126u: goto tr239;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr225;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr232;
	} else
		goto tr232;
	goto tr1;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	switch( (*p) ) {
		case 9u: goto tr240;
		case 10u: goto tr241;
		case 13u: goto tr242;
		case 32u: goto tr240;
		case 33u: goto tr243;
		case 34u: goto tr244;
		case 37u: goto tr245;
		case 38u: goto tr246;
		case 40u: goto tr247;
		case 41u: goto tr248;
		case 42u: goto tr249;
		case 43u: goto tr250;
		case 44u: goto tr251;
		case 45u: goto tr252;
		case 46u: goto tr253;
		case 47u: goto tr254;
		case 48u: goto tr255;
		case 58u: goto tr257;
		case 59u: goto tr258;
		case 60u: goto tr259;
		case 61u: goto tr260;
		case 62u: goto tr261;
		case 63u: goto tr262;
		case 91u: goto tr264;
		case 93u: goto tr265;
		case 94u: goto tr266;
		case 95u: goto tr263;
		case 123u: goto tr267;
		case 124u: goto tr268;
		case 125u: goto tr269;
		case 126u: goto tr270;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr256;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr263;
	} else
		goto tr263;
	goto tr1;
tr59:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr92:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr123:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
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
	goto st31;
tr185:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr216:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr247:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr278:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr309:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr340:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr371:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr402:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr433:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr464:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr495:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr526:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr557:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr586:
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
tr620:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr652:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr683:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr714:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr745:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr776:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr807:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr838:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr869:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr900:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr931:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr960:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr991:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1022:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1053:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1084:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1115:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1146:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1177:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1208:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1239:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1270:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1301:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1332:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1363:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1394:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1425:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1456:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1488:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1519:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1550:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1581:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1612:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1643:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1674:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 3066 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr271;
		case 10u: goto tr272;
		case 13u: goto tr273;
		case 32u: goto tr271;
		case 33u: goto tr274;
		case 34u: goto tr275;
		case 37u: goto tr276;
		case 38u: goto tr277;
		case 40u: goto tr278;
		case 41u: goto tr279;
		case 42u: goto tr280;
		case 43u: goto tr281;
		case 44u: goto tr282;
		case 45u: goto tr283;
		case 46u: goto tr284;
		case 47u: goto tr285;
		case 48u: goto tr286;
		case 58u: goto tr288;
		case 59u: goto tr289;
		case 60u: goto tr290;
		case 61u: goto tr291;
		case 62u: goto tr292;
		case 63u: goto tr293;
		case 91u: goto tr295;
		case 93u: goto tr296;
		case 94u: goto tr297;
		case 95u: goto tr294;
		case 123u: goto tr298;
		case 124u: goto tr299;
		case 125u: goto tr300;
		case 126u: goto tr301;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr287;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr294;
	} else
		goto tr294;
	goto tr1;
tr60:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr93:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr124:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
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
	goto st32;
tr186:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr217:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr248:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr279:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr310:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr341:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr372:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr403:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr434:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr465:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr496:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr527:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr558:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr587:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr621:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr653:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr684:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr715:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr746:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr777:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr808:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr839:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr870:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr901:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr932:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr961:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr992:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1023:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1054:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1085:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1116:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1147:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1178:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1209:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1240:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1271:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1302:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1333:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1364:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1395:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1426:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1457:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1489:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1520:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1551:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1582:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1613:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1644:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1675:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 3442 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr302;
		case 10u: goto tr303;
		case 13u: goto tr304;
		case 32u: goto tr302;
		case 33u: goto tr305;
		case 34u: goto tr306;
		case 37u: goto tr307;
		case 38u: goto tr308;
		case 40u: goto tr309;
		case 41u: goto tr310;
		case 42u: goto tr311;
		case 43u: goto tr312;
		case 44u: goto tr313;
		case 45u: goto tr314;
		case 46u: goto tr315;
		case 47u: goto tr316;
		case 48u: goto tr317;
		case 58u: goto tr319;
		case 59u: goto tr320;
		case 60u: goto tr321;
		case 61u: goto tr322;
		case 62u: goto tr323;
		case 63u: goto tr324;
		case 91u: goto tr326;
		case 93u: goto tr327;
		case 94u: goto tr328;
		case 95u: goto tr325;
		case 123u: goto tr329;
		case 124u: goto tr330;
		case 125u: goto tr331;
		case 126u: goto tr332;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr318;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr325;
	} else
		goto tr325;
	goto tr1;
tr61:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr94:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr125:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
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
	goto st33;
tr187:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr218:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr249:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr280:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr311:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr342:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr373:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr404:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr435:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr466:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr497:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr528:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr588:
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
tr622:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr654:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr685:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr716:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr747:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr778:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr809:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr840:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr871:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr902:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr933:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr962:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr993:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1024:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1055:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1086:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1117:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1148:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1179:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1210:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1241:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1272:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1303:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1334:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1365:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1396:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1427:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1458:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1490:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1521:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1552:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1583:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1614:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1645:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1676:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 3812 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr333;
		case 10u: goto tr334;
		case 13u: goto tr335;
		case 32u: goto tr333;
		case 33u: goto tr336;
		case 34u: goto tr337;
		case 37u: goto tr338;
		case 38u: goto tr339;
		case 40u: goto tr340;
		case 41u: goto tr341;
		case 42u: goto tr342;
		case 43u: goto tr343;
		case 44u: goto tr344;
		case 45u: goto tr345;
		case 46u: goto tr346;
		case 47u: goto tr347;
		case 48u: goto tr348;
		case 58u: goto tr350;
		case 59u: goto tr351;
		case 60u: goto tr352;
		case 61u: goto st78;
		case 62u: goto tr354;
		case 63u: goto tr355;
		case 91u: goto tr357;
		case 93u: goto tr358;
		case 94u: goto tr359;
		case 95u: goto tr356;
		case 123u: goto tr360;
		case 124u: goto tr361;
		case 125u: goto tr362;
		case 126u: goto tr363;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr349;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr356;
	} else
		goto tr356;
	goto tr1;
tr62:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr95:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr126:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
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
	goto st34;
tr188:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr219:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr250:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr281:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr312:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr343:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr405:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr436:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr467:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr498:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr529:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr559:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr589:
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
tr623:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr655:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr686:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr717:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr748:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr779:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr810:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr841:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr872:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr903:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr934:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr963:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr994:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1025:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1056:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1087:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1118:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1149:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1180:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1211:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1242:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1273:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1304:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1335:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1366:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1397:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1428:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1459:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1491:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1522:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1553:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1584:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1615:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1646:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1677:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 4182 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr364;
		case 10u: goto tr365;
		case 13u: goto tr366;
		case 32u: goto tr364;
		case 33u: goto tr367;
		case 34u: goto tr368;
		case 37u: goto tr369;
		case 38u: goto tr370;
		case 40u: goto tr371;
		case 41u: goto tr372;
		case 42u: goto tr373;
		case 43u: goto st35;
		case 44u: goto tr375;
		case 45u: goto tr376;
		case 46u: goto tr377;
		case 47u: goto tr378;
		case 48u: goto tr379;
		case 58u: goto tr381;
		case 59u: goto tr382;
		case 60u: goto tr383;
		case 61u: goto st77;
		case 62u: goto tr385;
		case 63u: goto tr386;
		case 91u: goto tr388;
		case 93u: goto tr389;
		case 94u: goto tr390;
		case 95u: goto tr387;
		case 123u: goto tr391;
		case 124u: goto tr392;
		case 125u: goto tr393;
		case 126u: goto tr394;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr380;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr387;
	} else
		goto tr387;
	goto tr1;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	switch( (*p) ) {
		case 9u: goto tr395;
		case 10u: goto tr396;
		case 13u: goto tr397;
		case 32u: goto tr395;
		case 33u: goto tr398;
		case 34u: goto tr399;
		case 37u: goto tr400;
		case 38u: goto tr401;
		case 40u: goto tr402;
		case 41u: goto tr403;
		case 42u: goto tr404;
		case 43u: goto tr405;
		case 44u: goto tr406;
		case 45u: goto tr407;
		case 46u: goto tr408;
		case 47u: goto tr409;
		case 48u: goto tr410;
		case 58u: goto tr412;
		case 59u: goto tr413;
		case 60u: goto tr414;
		case 61u: goto tr415;
		case 62u: goto tr416;
		case 63u: goto tr417;
		case 91u: goto tr419;
		case 93u: goto tr420;
		case 94u: goto tr421;
		case 95u: goto tr418;
		case 123u: goto tr422;
		case 124u: goto tr423;
		case 125u: goto tr424;
		case 126u: goto tr425;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr411;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr418;
	} else
		goto tr418;
	goto tr1;
tr63:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr96:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr127:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
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
	goto st36;
tr189:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr220:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr251:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr282:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr313:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr344:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr375:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr406:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr437:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr468:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr499:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr530:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr560:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr590:
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
tr624:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr656:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr687:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr718:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr749:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr780:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr811:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr842:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr873:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr904:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr935:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr964:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr995:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1026:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1057:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1088:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1119:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1150:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1181:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1212:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1243:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1274:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1305:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1336:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1367:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1398:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1429:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1460:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1492:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1523:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1554:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1585:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1616:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1647:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1678:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 4604 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr426;
		case 10u: goto tr427;
		case 13u: goto tr428;
		case 32u: goto tr426;
		case 33u: goto tr429;
		case 34u: goto tr430;
		case 37u: goto tr431;
		case 38u: goto tr432;
		case 40u: goto tr433;
		case 41u: goto tr434;
		case 42u: goto tr435;
		case 43u: goto tr436;
		case 44u: goto tr437;
		case 45u: goto tr438;
		case 46u: goto tr439;
		case 47u: goto tr440;
		case 48u: goto tr441;
		case 58u: goto tr443;
		case 59u: goto tr444;
		case 60u: goto tr445;
		case 61u: goto tr446;
		case 62u: goto tr447;
		case 63u: goto tr448;
		case 91u: goto tr450;
		case 93u: goto tr451;
		case 94u: goto tr452;
		case 95u: goto tr449;
		case 123u: goto tr453;
		case 124u: goto tr454;
		case 125u: goto tr455;
		case 126u: goto tr456;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr442;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr449;
	} else
		goto tr449;
	goto tr1;
tr64:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr97:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr128:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
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
	goto st37;
tr190:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr221:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr252:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr283:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr314:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr345:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr376:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr407:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr438:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr500:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr531:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr561:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr591:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr625:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr657:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr688:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr719:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr750:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr781:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr812:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr843:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr874:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr905:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr936:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr965:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr996:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1027:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1058:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1089:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1120:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1151:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1182:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1213:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1244:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1275:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1306:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1337:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1368:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1399:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1430:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1461:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1493:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1524:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1555:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1586:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1617:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1648:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1679:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 4974 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr457;
		case 10u: goto tr458;
		case 13u: goto tr459;
		case 32u: goto tr457;
		case 33u: goto tr460;
		case 34u: goto tr461;
		case 37u: goto tr462;
		case 38u: goto tr463;
		case 40u: goto tr464;
		case 41u: goto tr465;
		case 42u: goto tr466;
		case 43u: goto tr467;
		case 44u: goto tr468;
		case 45u: goto st38;
		case 46u: goto tr470;
		case 47u: goto tr471;
		case 48u: goto tr472;
		case 58u: goto tr474;
		case 59u: goto tr475;
		case 60u: goto tr476;
		case 61u: goto st76;
		case 62u: goto tr478;
		case 63u: goto tr479;
		case 91u: goto tr481;
		case 93u: goto tr482;
		case 94u: goto tr483;
		case 95u: goto tr480;
		case 123u: goto tr484;
		case 124u: goto tr485;
		case 125u: goto tr486;
		case 126u: goto tr487;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr473;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr480;
	} else
		goto tr480;
	goto tr1;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	switch( (*p) ) {
		case 9u: goto tr488;
		case 10u: goto tr489;
		case 13u: goto tr490;
		case 32u: goto tr488;
		case 33u: goto tr491;
		case 34u: goto tr492;
		case 37u: goto tr493;
		case 38u: goto tr494;
		case 40u: goto tr495;
		case 41u: goto tr496;
		case 42u: goto tr497;
		case 43u: goto tr498;
		case 44u: goto tr499;
		case 45u: goto tr500;
		case 46u: goto tr501;
		case 47u: goto tr502;
		case 48u: goto tr503;
		case 58u: goto tr505;
		case 59u: goto tr506;
		case 60u: goto tr507;
		case 61u: goto tr508;
		case 62u: goto tr509;
		case 63u: goto tr510;
		case 91u: goto tr512;
		case 93u: goto tr513;
		case 94u: goto tr514;
		case 95u: goto tr511;
		case 123u: goto tr515;
		case 124u: goto tr516;
		case 125u: goto tr517;
		case 126u: goto tr518;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr504;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr511;
	} else
		goto tr511;
	goto tr1;
tr65:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr98:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr129:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
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
	goto st39;
tr191:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr222:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr253:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr284:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr315:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr346:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr377:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr408:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr439:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr470:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr501:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr532:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr562:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr612:
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
tr626:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr658:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr689:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr720:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr751:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr782:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr813:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr844:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr875:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr906:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr937:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr966:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr997:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1028:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1059:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1090:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1121:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1152:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1183:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1214:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1245:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1276:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1307:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1338:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1369:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1400:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1431:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1462:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1494:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1525:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1556:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1587:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1618:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1649:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1680:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 5396 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr519;
		case 10u: goto tr520;
		case 13u: goto tr521;
		case 32u: goto tr519;
		case 33u: goto tr522;
		case 34u: goto tr523;
		case 37u: goto tr524;
		case 38u: goto tr525;
		case 40u: goto tr526;
		case 41u: goto tr527;
		case 42u: goto tr528;
		case 43u: goto tr529;
		case 44u: goto tr530;
		case 45u: goto tr531;
		case 46u: goto tr532;
		case 47u: goto tr533;
		case 48u: goto tr534;
		case 58u: goto tr536;
		case 59u: goto tr537;
		case 60u: goto tr538;
		case 61u: goto tr539;
		case 62u: goto tr540;
		case 63u: goto tr541;
		case 91u: goto tr543;
		case 93u: goto tr544;
		case 94u: goto tr545;
		case 95u: goto tr542;
		case 123u: goto tr546;
		case 124u: goto tr547;
		case 125u: goto tr548;
		case 126u: goto tr549;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr535;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr542;
	} else
		goto tr542;
	goto tr1;
tr66:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr130:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
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
	goto st40;
tr192:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr223:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr254:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr285:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr316:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr347:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr378:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr409:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr440:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr471:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr502:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr533:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr593:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr627:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr659:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr690:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr721:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr752:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr783:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr814:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr845:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr876:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr907:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr938:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr967:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr998:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1029:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1060:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1091:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1122:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1153:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1184:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1215:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1246:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1277:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1308:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1339:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1370:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1401:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1432:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1463:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1495:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1526:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1557:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1588:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1619:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1650:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1681:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 5760 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr550;
		case 10u: goto tr551;
		case 13u: goto tr552;
		case 32u: goto tr550;
		case 33u: goto tr553;
		case 34u: goto tr554;
		case 37u: goto tr555;
		case 38u: goto tr556;
		case 40u: goto tr557;
		case 41u: goto tr558;
		case 42u: goto st2;
		case 43u: goto tr559;
		case 44u: goto tr560;
		case 45u: goto tr561;
		case 46u: goto tr562;
		case 47u: goto st6;
		case 48u: goto tr563;
		case 58u: goto tr565;
		case 59u: goto tr566;
		case 60u: goto tr567;
		case 61u: goto st75;
		case 62u: goto tr569;
		case 63u: goto tr570;
		case 91u: goto tr572;
		case 93u: goto tr573;
		case 94u: goto tr574;
		case 95u: goto tr571;
		case 123u: goto tr575;
		case 124u: goto tr576;
		case 125u: goto tr577;
		case 126u: goto tr578;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr564;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr571;
	} else
		goto tr571;
	goto tr1;
tr8:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 5811 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st3;
		case 13u: goto st4;
		case 42u: goto st5;
	}
	goto st2;
tr9:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 5826 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr9;
		case 13u: goto tr10;
		case 42u: goto tr11;
	}
	goto tr8;
tr10:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 5841 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st3;
		case 13u: goto tr10;
		case 42u: goto tr11;
	}
	goto tr8;
tr11:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 5856 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st3;
		case 13u: goto st4;
		case 42u: goto st5;
		case 47u: goto st23;
	}
	goto st2;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	switch( (*p) ) {
		case 10u: goto st24;
		case 13u: goto st25;
	}
	goto st6;
tr67:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr100:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr131:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr193:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr224:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr255:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr286:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr317:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr348:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr379:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr410:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr441:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr472:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr503:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr534:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr563:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr594:
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
	goto st41;
tr628:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr660:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr691:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr722:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr753:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr784:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr815:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr846:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr877:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr908:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr968:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr999:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1030:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1061:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1092:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1123:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1154:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1185:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1216:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1247:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1278:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1309:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1340:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1371:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1402:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1433:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1464:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1496:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1527:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1558:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1589:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1620:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1651:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
tr1682:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 6301 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr592;
		case 47u: goto tr593;
		case 48u: goto tr594;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 69u: goto tr603;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 101u: goto tr603;
		case 120u: goto tr607;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr595;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr602;
	} else
		goto tr602;
	goto tr1;
tr592:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 6355 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr16;
	goto tr1;
tr16:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 6367 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr612;
		case 47u: goto tr593;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 69u: goto tr603;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 101u: goto tr603;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr16;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr602;
	} else
		goto tr602;
	goto tr1;
tr69:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr102:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr133:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
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
	goto st43;
tr195:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr226:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr257:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr288:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr319:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr350:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr381:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr412:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr443:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr474:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr505:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr536:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr565:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr596:
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
tr630:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr662:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr693:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr724:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr755:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr786:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr817:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr848:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr879:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr910:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr940:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr970:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1001:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1032:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1063:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1094:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1125:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1156:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1187:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1218:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1249:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1280:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1311:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1342:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1373:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1404:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1435:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1466:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1498:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1529:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1560:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1591:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1622:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1653:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1684:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 6744 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr613;
		case 10u: goto tr614;
		case 13u: goto tr615;
		case 32u: goto tr613;
		case 33u: goto tr616;
		case 34u: goto tr617;
		case 37u: goto tr618;
		case 38u: goto tr619;
		case 40u: goto tr620;
		case 41u: goto tr621;
		case 42u: goto tr622;
		case 43u: goto tr623;
		case 44u: goto tr624;
		case 45u: goto tr625;
		case 46u: goto tr626;
		case 47u: goto tr627;
		case 48u: goto tr628;
		case 58u: goto tr630;
		case 59u: goto tr631;
		case 60u: goto tr632;
		case 61u: goto tr633;
		case 62u: goto tr634;
		case 63u: goto tr635;
		case 91u: goto tr637;
		case 93u: goto tr638;
		case 94u: goto tr639;
		case 95u: goto tr636;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr629;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr636;
	} else
		goto tr636;
	goto tr1;
tr644:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr68:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr101:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr132:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr194:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr225:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr256:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr287:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr318:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr349:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr380:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr411:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr442:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr473:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr504:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr535:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr564:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr595:
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
tr629:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr661:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr692:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr723:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr754:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr785:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr816:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr847:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr878:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr909:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr969:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1000:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1031:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1062:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1093:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1124:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1155:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1186:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1217:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1248:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1279:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1310:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1341:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1372:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1403:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1434:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1465:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1497:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1528:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1559:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1590:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1621:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1652:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
tr1683:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 7219 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr592;
		case 47u: goto tr593;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 69u: goto tr603;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 101u: goto tr603;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr644;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr602;
	} else
		goto tr602;
	goto tr1;
tr70:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr103:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr134:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
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
	goto st45;
tr196:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr227:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr258:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr289:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr320:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr351:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr382:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr413:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr444:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr475:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr506:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr537:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr566:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr597:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr631:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr663:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr694:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr725:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr756:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr787:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr818:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr849:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr880:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr911:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr941:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr971:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1002:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1033:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1064:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1095:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1126:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1157:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1188:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1219:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1250:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1281:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1312:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1343:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1374:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1405:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1436:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1467:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1499:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1530:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1561:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1592:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1623:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1654:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1685:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 7596 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr645;
		case 10u: goto tr646;
		case 13u: goto tr647;
		case 32u: goto tr645;
		case 33u: goto tr648;
		case 34u: goto tr649;
		case 37u: goto tr650;
		case 38u: goto tr651;
		case 40u: goto tr652;
		case 41u: goto tr653;
		case 42u: goto tr654;
		case 43u: goto tr655;
		case 44u: goto tr656;
		case 45u: goto tr657;
		case 46u: goto tr658;
		case 47u: goto tr659;
		case 48u: goto tr660;
		case 58u: goto tr662;
		case 59u: goto tr663;
		case 60u: goto tr664;
		case 61u: goto tr665;
		case 62u: goto tr666;
		case 63u: goto tr667;
		case 91u: goto tr669;
		case 93u: goto tr670;
		case 94u: goto tr671;
		case 95u: goto tr668;
		case 123u: goto tr672;
		case 124u: goto tr673;
		case 125u: goto tr674;
		case 126u: goto tr675;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr661;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr668;
	} else
		goto tr668;
	goto tr1;
tr71:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr104:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr135:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
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
	goto st46;
tr197:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr228:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr259:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr290:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr321:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr352:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr383:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr414:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr445:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr476:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr507:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr538:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr567:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr598:
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
tr632:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr664:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr726:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr757:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr788:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr819:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr850:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr881:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr912:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr942:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr972:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1003:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1034:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1065:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1096:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1127:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1158:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1189:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1220:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1251:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1282:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1313:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1344:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1375:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1406:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1437:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1468:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1500:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1531:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1562:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1593:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1624:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1655:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1686:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 7966 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr676;
		case 10u: goto tr677;
		case 13u: goto tr678;
		case 32u: goto tr676;
		case 33u: goto tr679;
		case 34u: goto tr680;
		case 37u: goto tr681;
		case 38u: goto tr682;
		case 40u: goto tr683;
		case 41u: goto tr684;
		case 42u: goto tr685;
		case 43u: goto tr686;
		case 44u: goto tr687;
		case 45u: goto tr688;
		case 46u: goto tr689;
		case 47u: goto tr690;
		case 48u: goto tr691;
		case 58u: goto tr693;
		case 59u: goto tr694;
		case 60u: goto st47;
		case 61u: goto st71;
		case 62u: goto tr697;
		case 63u: goto tr698;
		case 91u: goto tr700;
		case 93u: goto tr701;
		case 94u: goto tr702;
		case 95u: goto tr699;
		case 123u: goto tr703;
		case 124u: goto tr704;
		case 125u: goto tr705;
		case 126u: goto tr706;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr692;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr699;
	} else
		goto tr699;
	goto tr1;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 9u: goto tr707;
		case 10u: goto tr708;
		case 13u: goto tr709;
		case 32u: goto tr707;
		case 33u: goto tr710;
		case 34u: goto tr711;
		case 37u: goto tr712;
		case 38u: goto tr713;
		case 40u: goto tr714;
		case 41u: goto tr715;
		case 42u: goto tr716;
		case 43u: goto tr717;
		case 44u: goto tr718;
		case 45u: goto tr719;
		case 46u: goto tr720;
		case 47u: goto tr721;
		case 48u: goto tr722;
		case 58u: goto tr724;
		case 59u: goto tr725;
		case 60u: goto tr726;
		case 61u: goto st48;
		case 62u: goto tr728;
		case 63u: goto tr729;
		case 91u: goto tr731;
		case 93u: goto tr732;
		case 94u: goto tr733;
		case 95u: goto tr730;
		case 123u: goto tr734;
		case 124u: goto tr735;
		case 125u: goto tr736;
		case 126u: goto tr737;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr723;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr730;
	} else
		goto tr730;
	goto tr1;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	switch( (*p) ) {
		case 9u: goto tr738;
		case 10u: goto tr739;
		case 13u: goto tr740;
		case 32u: goto tr738;
		case 33u: goto tr741;
		case 34u: goto tr742;
		case 37u: goto tr743;
		case 38u: goto tr744;
		case 40u: goto tr745;
		case 41u: goto tr746;
		case 42u: goto tr747;
		case 43u: goto tr748;
		case 44u: goto tr749;
		case 45u: goto tr750;
		case 46u: goto tr751;
		case 47u: goto tr752;
		case 48u: goto tr753;
		case 58u: goto tr755;
		case 59u: goto tr756;
		case 60u: goto tr757;
		case 61u: goto tr758;
		case 62u: goto tr759;
		case 63u: goto tr760;
		case 91u: goto tr762;
		case 93u: goto tr763;
		case 94u: goto tr764;
		case 95u: goto tr761;
		case 123u: goto tr765;
		case 124u: goto tr766;
		case 125u: goto tr767;
		case 126u: goto tr768;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr754;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr761;
	} else
		goto tr761;
	goto tr1;
tr72:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr105:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
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
	goto st49;
tr260:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr291:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr322:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr415:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr446:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr508:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr539:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr599:
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
tr633:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr665:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr758:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr820:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr882:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr913:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr943:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr973:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1004:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1066:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1097:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1159:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1190:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1252:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1283:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1314:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1376:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1438:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1469:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1501:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1532:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1563:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1594:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1625:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1656:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1687:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
#line 8338 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr769;
		case 10u: goto tr770;
		case 13u: goto tr771;
		case 32u: goto tr769;
		case 33u: goto tr772;
		case 34u: goto tr773;
		case 37u: goto tr774;
		case 38u: goto tr775;
		case 40u: goto tr776;
		case 41u: goto tr777;
		case 42u: goto tr778;
		case 43u: goto tr779;
		case 44u: goto tr780;
		case 45u: goto tr781;
		case 46u: goto tr782;
		case 47u: goto tr783;
		case 48u: goto tr784;
		case 58u: goto tr786;
		case 59u: goto tr787;
		case 60u: goto tr788;
		case 61u: goto st50;
		case 62u: goto tr790;
		case 63u: goto tr791;
		case 91u: goto tr793;
		case 93u: goto tr794;
		case 94u: goto tr795;
		case 95u: goto tr792;
		case 123u: goto tr796;
		case 124u: goto tr797;
		case 125u: goto tr798;
		case 126u: goto tr799;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr785;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr792;
	} else
		goto tr792;
	goto tr1;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9u: goto tr800;
		case 10u: goto tr801;
		case 13u: goto tr802;
		case 32u: goto tr800;
		case 33u: goto tr803;
		case 34u: goto tr804;
		case 37u: goto tr805;
		case 38u: goto tr806;
		case 40u: goto tr807;
		case 41u: goto tr808;
		case 42u: goto tr809;
		case 43u: goto tr810;
		case 44u: goto tr811;
		case 45u: goto tr812;
		case 46u: goto tr813;
		case 47u: goto tr814;
		case 48u: goto tr815;
		case 58u: goto tr817;
		case 59u: goto tr818;
		case 60u: goto tr819;
		case 61u: goto tr820;
		case 62u: goto tr821;
		case 63u: goto tr822;
		case 91u: goto tr824;
		case 93u: goto tr825;
		case 94u: goto tr826;
		case 95u: goto tr823;
		case 123u: goto tr827;
		case 124u: goto tr828;
		case 125u: goto tr829;
		case 126u: goto tr830;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr816;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr823;
	} else
		goto tr823;
	goto tr1;
tr73:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr106:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr137:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
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
	goto st51;
tr199:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr230:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr261:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr292:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr323:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr354:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr385:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr416:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr447:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr478:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr509:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr540:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr569:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr600:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr634:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr666:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr697:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr728:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr759:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr790:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr821:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr883:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr914:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr944:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr974:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1005:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1036:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1067:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1098:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1129:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1160:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1191:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1222:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1253:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1284:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1315:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1377:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1408:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1439:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1470:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1502:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1533:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1564:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1595:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1626:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1657:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1688:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 8748 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr831;
		case 10u: goto tr832;
		case 13u: goto tr833;
		case 32u: goto tr831;
		case 33u: goto tr834;
		case 34u: goto tr835;
		case 37u: goto tr836;
		case 38u: goto tr837;
		case 40u: goto tr838;
		case 41u: goto tr839;
		case 42u: goto tr840;
		case 43u: goto tr841;
		case 44u: goto tr842;
		case 45u: goto tr843;
		case 46u: goto tr844;
		case 47u: goto tr845;
		case 48u: goto tr846;
		case 58u: goto tr848;
		case 59u: goto tr849;
		case 60u: goto tr850;
		case 61u: goto st52;
		case 62u: goto st67;
		case 63u: goto tr853;
		case 91u: goto tr855;
		case 93u: goto tr856;
		case 94u: goto tr857;
		case 95u: goto tr854;
		case 123u: goto tr858;
		case 124u: goto tr859;
		case 125u: goto tr860;
		case 126u: goto tr861;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr847;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr854;
	} else
		goto tr854;
	goto tr1;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	switch( (*p) ) {
		case 9u: goto tr862;
		case 10u: goto tr863;
		case 13u: goto tr864;
		case 32u: goto tr862;
		case 33u: goto tr865;
		case 34u: goto tr866;
		case 37u: goto tr867;
		case 38u: goto tr868;
		case 40u: goto tr869;
		case 41u: goto tr870;
		case 42u: goto tr871;
		case 43u: goto tr872;
		case 44u: goto tr873;
		case 45u: goto tr874;
		case 46u: goto tr875;
		case 47u: goto tr876;
		case 48u: goto tr877;
		case 58u: goto tr879;
		case 59u: goto tr880;
		case 60u: goto tr881;
		case 61u: goto tr882;
		case 62u: goto tr883;
		case 63u: goto tr884;
		case 91u: goto tr886;
		case 93u: goto tr887;
		case 94u: goto tr888;
		case 95u: goto tr885;
		case 123u: goto tr889;
		case 124u: goto tr890;
		case 125u: goto tr891;
		case 126u: goto tr892;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr878;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr885;
	} else
		goto tr885;
	goto tr1;
tr74:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr107:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr138:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
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
	goto st53;
tr200:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr231:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr262:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr293:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr324:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr355:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr386:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr417:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr448:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr479:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr510:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr541:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr570:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr601:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr635:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr667:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr698:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr729:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr760:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr791:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr822:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr853:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr884:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr915:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr945:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr975:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1006:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1037:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1068:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1099:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1130:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1161:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1192:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1223:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1254:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1285:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1316:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1347:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1378:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1409:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1440:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1471:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1503:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1534:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1565:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1596:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1627:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1658:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1689:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 9170 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr893;
		case 10u: goto tr894;
		case 13u: goto tr895;
		case 32u: goto tr893;
		case 33u: goto tr896;
		case 34u: goto tr897;
		case 37u: goto tr898;
		case 38u: goto tr899;
		case 40u: goto tr900;
		case 41u: goto tr901;
		case 42u: goto tr902;
		case 43u: goto tr903;
		case 44u: goto tr904;
		case 45u: goto tr905;
		case 46u: goto tr906;
		case 47u: goto tr907;
		case 48u: goto tr908;
		case 58u: goto tr910;
		case 59u: goto tr911;
		case 60u: goto tr912;
		case 61u: goto tr913;
		case 62u: goto tr914;
		case 63u: goto tr915;
		case 91u: goto tr917;
		case 93u: goto tr918;
		case 94u: goto tr919;
		case 95u: goto tr916;
		case 123u: goto tr920;
		case 124u: goto tr921;
		case 125u: goto tr922;
		case 126u: goto tr923;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr909;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr916;
	} else
		goto tr916;
	goto tr1;
tr939:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr75:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr108:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr139:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr201:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr232:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr263:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr294:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr325:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr356:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr387:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr418:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr449:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr480:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr511:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr542:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr571:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr602:
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
	goto st54;
tr636:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr668:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr699:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr730:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr761:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr792:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr823:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr854:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr885:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr916:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr976:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1007:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1038:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1069:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1100:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1131:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1162:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1193:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1224:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1255:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1286:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1317:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1348:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1379:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1410:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1441:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1472:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1504:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1535:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1566:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1597:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1628:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1659:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
tr1690:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
#line 9645 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr924;
		case 10u: goto tr925;
		case 13u: goto tr926;
		case 32u: goto tr924;
		case 33u: goto tr927;
		case 34u: goto tr928;
		case 37u: goto tr929;
		case 38u: goto tr930;
		case 40u: goto tr931;
		case 41u: goto tr932;
		case 42u: goto tr933;
		case 43u: goto tr934;
		case 44u: goto tr935;
		case 45u: goto tr936;
		case 46u: goto tr937;
		case 47u: goto tr938;
		case 58u: goto tr940;
		case 59u: goto tr941;
		case 60u: goto tr942;
		case 61u: goto tr943;
		case 62u: goto tr944;
		case 63u: goto tr945;
		case 91u: goto tr946;
		case 93u: goto tr947;
		case 94u: goto tr948;
		case 95u: goto tr939;
		case 123u: goto tr949;
		case 124u: goto tr950;
		case 125u: goto tr951;
		case 126u: goto tr952;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr939;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr939;
	} else
		goto tr939;
	goto tr1;
tr76:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr109:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr140:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
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
	goto st55;
tr202:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr233:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr264:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr295:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr326:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr357:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr388:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr419:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr450:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr481:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr512:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr543:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr572:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr604:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr637:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr669:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr700:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr731:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr762:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr793:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr824:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr855:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr886:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr917:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr946:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr977:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1008:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1039:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1070:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1101:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1132:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1163:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1194:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1225:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1256:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1287:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1318:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1349:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1380:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1411:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1442:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1473:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1505:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1536:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1567:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1598:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1629:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1660:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1691:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
#line 10020 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr953;
		case 10u: goto tr954;
		case 13u: goto tr955;
		case 32u: goto tr953;
		case 33u: goto tr956;
		case 34u: goto tr957;
		case 37u: goto tr958;
		case 38u: goto tr959;
		case 40u: goto tr960;
		case 41u: goto tr961;
		case 42u: goto tr962;
		case 43u: goto tr963;
		case 44u: goto tr964;
		case 45u: goto tr965;
		case 46u: goto tr966;
		case 47u: goto tr967;
		case 48u: goto tr968;
		case 58u: goto tr970;
		case 59u: goto tr971;
		case 60u: goto tr972;
		case 61u: goto tr973;
		case 62u: goto tr974;
		case 63u: goto tr975;
		case 91u: goto tr977;
		case 93u: goto tr978;
		case 94u: goto tr979;
		case 95u: goto tr976;
		case 123u: goto tr980;
		case 124u: goto tr981;
		case 125u: goto tr982;
		case 126u: goto tr983;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr969;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr976;
	} else
		goto tr976;
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
	goto st56;
tr203:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr234:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr265:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr296:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr327:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr358:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr389:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr420:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr451:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr482:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr513:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr544:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr573:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr605:
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
tr638:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr670:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr701:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr732:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr763:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr794:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr825:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr856:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr887:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr918:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr947:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr978:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1009:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1040:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1071:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1102:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1133:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1164:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1195:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1226:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1257:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1288:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1319:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1350:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1381:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1412:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1443:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1474:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1506:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1537:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1568:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1599:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1630:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1661:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1692:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
#line 10396 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr984;
		case 10u: goto tr985;
		case 13u: goto tr986;
		case 32u: goto tr984;
		case 33u: goto tr987;
		case 34u: goto tr988;
		case 37u: goto tr989;
		case 38u: goto tr990;
		case 40u: goto tr991;
		case 41u: goto tr992;
		case 42u: goto tr993;
		case 43u: goto tr994;
		case 44u: goto tr995;
		case 45u: goto tr996;
		case 46u: goto tr997;
		case 47u: goto tr998;
		case 48u: goto tr999;
		case 58u: goto tr1001;
		case 59u: goto tr1002;
		case 60u: goto tr1003;
		case 61u: goto tr1004;
		case 62u: goto tr1005;
		case 63u: goto tr1006;
		case 91u: goto tr1008;
		case 93u: goto tr1009;
		case 94u: goto tr1010;
		case 95u: goto tr1007;
		case 123u: goto tr1011;
		case 124u: goto tr1012;
		case 125u: goto tr1013;
		case 126u: goto tr1014;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1000;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1007;
	} else
		goto tr1007;
	goto tr1;
tr78:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr111:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr142:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
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
	goto st57;
tr204:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr235:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr266:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr297:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr328:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr359:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr390:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr421:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr452:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr483:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr514:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr545:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr574:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr606:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr639:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr671:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr702:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr733:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr764:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr795:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr826:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr857:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr888:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr919:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr948:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr979:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1010:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1072:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1103:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1134:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1165:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1196:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1227:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1258:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1289:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1320:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1351:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1382:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1413:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1444:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1475:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1507:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1538:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1569:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1600:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1631:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1662:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1693:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 10766 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1015;
		case 10u: goto tr1016;
		case 13u: goto tr1017;
		case 32u: goto tr1015;
		case 33u: goto tr1018;
		case 34u: goto tr1019;
		case 37u: goto tr1020;
		case 38u: goto tr1021;
		case 40u: goto tr1022;
		case 41u: goto tr1023;
		case 42u: goto tr1024;
		case 43u: goto tr1025;
		case 44u: goto tr1026;
		case 45u: goto tr1027;
		case 46u: goto tr1028;
		case 47u: goto tr1029;
		case 48u: goto tr1030;
		case 58u: goto tr1032;
		case 59u: goto tr1033;
		case 60u: goto tr1034;
		case 61u: goto st58;
		case 62u: goto tr1036;
		case 63u: goto tr1037;
		case 91u: goto tr1039;
		case 93u: goto tr1040;
		case 94u: goto st66;
		case 95u: goto tr1038;
		case 123u: goto tr1042;
		case 124u: goto tr1043;
		case 125u: goto tr1044;
		case 126u: goto tr1045;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1031;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1038;
	} else
		goto tr1038;
	goto tr1;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 9u: goto tr1046;
		case 10u: goto tr1047;
		case 13u: goto tr1048;
		case 32u: goto tr1046;
		case 33u: goto tr1049;
		case 34u: goto tr1050;
		case 37u: goto tr1051;
		case 38u: goto tr1052;
		case 40u: goto tr1053;
		case 41u: goto tr1054;
		case 42u: goto tr1055;
		case 43u: goto tr1056;
		case 44u: goto tr1057;
		case 45u: goto tr1058;
		case 46u: goto tr1059;
		case 47u: goto tr1060;
		case 48u: goto tr1061;
		case 58u: goto tr1063;
		case 59u: goto tr1064;
		case 60u: goto tr1065;
		case 61u: goto tr1066;
		case 62u: goto tr1067;
		case 63u: goto tr1068;
		case 91u: goto tr1070;
		case 93u: goto tr1071;
		case 94u: goto tr1072;
		case 95u: goto tr1069;
		case 123u: goto tr1073;
		case 124u: goto tr1074;
		case 125u: goto tr1075;
		case 126u: goto tr1076;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1062;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1069;
	} else
		goto tr1069;
	goto tr1;
tr79:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr112:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr143:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
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
	goto st59;
tr205:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr236:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr267:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr298:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr329:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr360:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr391:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr422:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr453:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr484:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr515:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr546:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr575:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr608:
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
tr640:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr672:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr703:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr734:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr765:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr796:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr827:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr858:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr889:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr920:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr949:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr980:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1011:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1042:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1073:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1104:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1135:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1166:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1197:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1228:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1259:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1290:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1321:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1352:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1383:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1414:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1445:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1476:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1508:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1539:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1570:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1601:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1632:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1663:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1694:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 11188 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1077;
		case 10u: goto tr1078;
		case 13u: goto tr1079;
		case 32u: goto tr1077;
		case 33u: goto tr1080;
		case 34u: goto tr1081;
		case 37u: goto tr1082;
		case 38u: goto tr1083;
		case 40u: goto tr1084;
		case 41u: goto tr1085;
		case 42u: goto tr1086;
		case 43u: goto tr1087;
		case 44u: goto tr1088;
		case 45u: goto tr1089;
		case 46u: goto tr1090;
		case 47u: goto tr1091;
		case 48u: goto tr1092;
		case 58u: goto tr1094;
		case 59u: goto tr1095;
		case 60u: goto tr1096;
		case 61u: goto tr1097;
		case 62u: goto tr1098;
		case 63u: goto tr1099;
		case 91u: goto tr1101;
		case 93u: goto tr1102;
		case 94u: goto tr1103;
		case 95u: goto tr1100;
		case 123u: goto tr1104;
		case 124u: goto tr1105;
		case 125u: goto tr1106;
		case 126u: goto tr1107;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1093;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1100;
	} else
		goto tr1100;
	goto tr1;
tr80:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr113:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr144:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
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
	goto st60;
tr206:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr237:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr268:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr299:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr330:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr361:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr392:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr423:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr454:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr485:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr516:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr547:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr576:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr609:
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
tr641:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr673:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr704:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr735:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr766:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr797:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr828:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr859:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr890:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr921:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr950:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr981:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1012:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1043:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1074:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1105:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1167:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1198:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1229:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1260:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1291:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1322:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1353:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1384:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1415:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1446:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1477:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1509:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1540:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1571:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1602:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1633:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1664:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1695:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 11558 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1108;
		case 10u: goto tr1109;
		case 13u: goto tr1110;
		case 32u: goto tr1108;
		case 33u: goto tr1111;
		case 34u: goto tr1112;
		case 37u: goto tr1113;
		case 38u: goto tr1114;
		case 40u: goto tr1115;
		case 41u: goto tr1116;
		case 42u: goto tr1117;
		case 43u: goto tr1118;
		case 44u: goto tr1119;
		case 45u: goto tr1120;
		case 46u: goto tr1121;
		case 47u: goto tr1122;
		case 48u: goto tr1123;
		case 58u: goto tr1125;
		case 59u: goto tr1126;
		case 60u: goto tr1127;
		case 61u: goto st61;
		case 62u: goto tr1129;
		case 63u: goto tr1130;
		case 91u: goto tr1132;
		case 93u: goto tr1133;
		case 94u: goto tr1134;
		case 95u: goto tr1131;
		case 123u: goto tr1135;
		case 124u: goto st65;
		case 125u: goto tr1137;
		case 126u: goto tr1138;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1124;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1131;
	} else
		goto tr1131;
	goto tr1;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 9u: goto tr1139;
		case 10u: goto tr1140;
		case 13u: goto tr1141;
		case 32u: goto tr1139;
		case 33u: goto tr1142;
		case 34u: goto tr1143;
		case 37u: goto tr1144;
		case 38u: goto tr1145;
		case 40u: goto tr1146;
		case 41u: goto tr1147;
		case 42u: goto tr1148;
		case 43u: goto tr1149;
		case 44u: goto tr1150;
		case 45u: goto tr1151;
		case 46u: goto tr1152;
		case 47u: goto tr1153;
		case 48u: goto tr1154;
		case 58u: goto tr1156;
		case 59u: goto tr1157;
		case 60u: goto tr1158;
		case 61u: goto tr1159;
		case 62u: goto tr1160;
		case 63u: goto tr1161;
		case 91u: goto tr1163;
		case 93u: goto tr1164;
		case 94u: goto tr1165;
		case 95u: goto tr1162;
		case 123u: goto tr1166;
		case 124u: goto tr1167;
		case 125u: goto tr1168;
		case 126u: goto tr1169;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1155;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1162;
	} else
		goto tr1162;
	goto tr1;
tr81:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr114:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr145:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
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
	goto st62;
tr207:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr238:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr269:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr300:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr331:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr362:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr393:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr424:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr455:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr486:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr517:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr548:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr577:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr610:
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
tr642:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr674:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr705:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr736:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr767:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr798:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr829:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr860:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr891:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr922:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr951:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr982:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1013:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1044:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1075:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1106:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1137:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1168:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1199:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1230:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1261:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1292:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1323:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1354:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1385:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1416:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1447:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1478:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1510:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1541:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1572:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1603:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1634:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1665:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1696:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 11980 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1170;
		case 10u: goto tr1171;
		case 13u: goto tr1172;
		case 32u: goto tr1170;
		case 33u: goto tr1173;
		case 34u: goto tr1174;
		case 37u: goto tr1175;
		case 38u: goto tr1176;
		case 40u: goto tr1177;
		case 41u: goto tr1178;
		case 42u: goto tr1179;
		case 43u: goto tr1180;
		case 44u: goto tr1181;
		case 45u: goto tr1182;
		case 46u: goto tr1183;
		case 47u: goto tr1184;
		case 48u: goto tr1185;
		case 58u: goto tr1187;
		case 59u: goto tr1188;
		case 60u: goto tr1189;
		case 61u: goto tr1190;
		case 62u: goto tr1191;
		case 63u: goto tr1192;
		case 91u: goto tr1194;
		case 93u: goto tr1195;
		case 94u: goto tr1196;
		case 95u: goto tr1193;
		case 123u: goto tr1197;
		case 124u: goto tr1198;
		case 125u: goto tr1199;
		case 126u: goto tr1200;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1186;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1193;
	} else
		goto tr1193;
	goto tr1;
tr82:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr115:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr146:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
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
	goto st63;
tr208:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr239:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr270:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr301:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr332:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr363:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr394:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr425:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr456:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr487:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr518:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr549:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr578:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr611:
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
tr643:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr675:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr706:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr737:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr768:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr799:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr830:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr861:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr892:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr923:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr952:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr983:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1014:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1045:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1076:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1107:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1138:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1169:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1200:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1231:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1262:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1293:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1324:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1355:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1386:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1417:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1448:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1479:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1511:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1542:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1573:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1604:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1635:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1666:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
tr1697:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st63;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
#line 12356 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1201;
		case 10u: goto tr1202;
		case 13u: goto tr1203;
		case 32u: goto tr1201;
		case 33u: goto tr1204;
		case 34u: goto tr1205;
		case 37u: goto tr1206;
		case 38u: goto tr1207;
		case 40u: goto tr1208;
		case 41u: goto tr1209;
		case 42u: goto tr1210;
		case 43u: goto tr1211;
		case 44u: goto tr1212;
		case 45u: goto tr1213;
		case 46u: goto tr1214;
		case 47u: goto tr1215;
		case 48u: goto tr1216;
		case 58u: goto tr1218;
		case 59u: goto tr1219;
		case 60u: goto tr1220;
		case 61u: goto st64;
		case 62u: goto tr1222;
		case 63u: goto tr1223;
		case 91u: goto tr1225;
		case 93u: goto tr1226;
		case 94u: goto tr1227;
		case 95u: goto tr1224;
		case 123u: goto tr1228;
		case 124u: goto tr1229;
		case 125u: goto tr1230;
		case 126u: goto tr1231;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1217;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1224;
	} else
		goto tr1224;
	goto tr1;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 9u: goto tr1232;
		case 10u: goto tr1233;
		case 13u: goto tr1234;
		case 32u: goto tr1232;
		case 33u: goto tr1235;
		case 34u: goto tr1236;
		case 37u: goto tr1237;
		case 38u: goto tr1238;
		case 40u: goto tr1239;
		case 41u: goto tr1240;
		case 42u: goto tr1241;
		case 43u: goto tr1242;
		case 44u: goto tr1243;
		case 45u: goto tr1244;
		case 46u: goto tr1245;
		case 47u: goto tr1246;
		case 48u: goto tr1247;
		case 58u: goto tr1249;
		case 59u: goto tr1250;
		case 60u: goto tr1251;
		case 61u: goto tr1252;
		case 62u: goto tr1253;
		case 63u: goto tr1254;
		case 91u: goto tr1256;
		case 93u: goto tr1257;
		case 94u: goto tr1258;
		case 95u: goto tr1255;
		case 123u: goto tr1259;
		case 124u: goto tr1260;
		case 125u: goto tr1261;
		case 126u: goto tr1262;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1248;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1255;
	} else
		goto tr1255;
	goto tr1;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	switch( (*p) ) {
		case 9u: goto tr1263;
		case 10u: goto tr1264;
		case 13u: goto tr1265;
		case 32u: goto tr1263;
		case 33u: goto tr1266;
		case 34u: goto tr1267;
		case 37u: goto tr1268;
		case 38u: goto tr1269;
		case 40u: goto tr1270;
		case 41u: goto tr1271;
		case 42u: goto tr1272;
		case 43u: goto tr1273;
		case 44u: goto tr1274;
		case 45u: goto tr1275;
		case 46u: goto tr1276;
		case 47u: goto tr1277;
		case 48u: goto tr1278;
		case 58u: goto tr1280;
		case 59u: goto tr1281;
		case 60u: goto tr1282;
		case 61u: goto tr1283;
		case 62u: goto tr1284;
		case 63u: goto tr1285;
		case 91u: goto tr1287;
		case 93u: goto tr1288;
		case 94u: goto tr1289;
		case 95u: goto tr1286;
		case 123u: goto tr1290;
		case 124u: goto tr1291;
		case 125u: goto tr1292;
		case 126u: goto tr1293;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1279;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1286;
	} else
		goto tr1286;
	goto tr1;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 9u: goto tr1294;
		case 10u: goto tr1295;
		case 13u: goto tr1296;
		case 32u: goto tr1294;
		case 33u: goto tr1297;
		case 34u: goto tr1298;
		case 37u: goto tr1299;
		case 38u: goto tr1300;
		case 40u: goto tr1301;
		case 41u: goto tr1302;
		case 42u: goto tr1303;
		case 43u: goto tr1304;
		case 44u: goto tr1305;
		case 45u: goto tr1306;
		case 46u: goto tr1307;
		case 47u: goto tr1308;
		case 48u: goto tr1309;
		case 58u: goto tr1311;
		case 59u: goto tr1312;
		case 60u: goto tr1313;
		case 61u: goto tr1314;
		case 62u: goto tr1315;
		case 63u: goto tr1316;
		case 91u: goto tr1318;
		case 93u: goto tr1319;
		case 94u: goto tr1320;
		case 95u: goto tr1317;
		case 123u: goto tr1321;
		case 124u: goto tr1322;
		case 125u: goto tr1323;
		case 126u: goto tr1324;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1310;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1317;
	} else
		goto tr1317;
	goto tr1;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 9u: goto tr1325;
		case 10u: goto tr1326;
		case 13u: goto tr1327;
		case 32u: goto tr1325;
		case 33u: goto tr1328;
		case 34u: goto tr1329;
		case 37u: goto tr1330;
		case 38u: goto tr1331;
		case 40u: goto tr1332;
		case 41u: goto tr1333;
		case 42u: goto tr1334;
		case 43u: goto tr1335;
		case 44u: goto tr1336;
		case 45u: goto tr1337;
		case 46u: goto tr1338;
		case 47u: goto tr1339;
		case 48u: goto tr1340;
		case 58u: goto tr1342;
		case 59u: goto tr1343;
		case 60u: goto tr1344;
		case 61u: goto st68;
		case 62u: goto st69;
		case 63u: goto tr1347;
		case 91u: goto tr1349;
		case 93u: goto tr1350;
		case 94u: goto tr1351;
		case 95u: goto tr1348;
		case 123u: goto tr1352;
		case 124u: goto tr1353;
		case 125u: goto tr1354;
		case 126u: goto tr1355;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1341;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1348;
	} else
		goto tr1348;
	goto tr1;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 9u: goto tr1356;
		case 10u: goto tr1357;
		case 13u: goto tr1358;
		case 32u: goto tr1356;
		case 33u: goto tr1359;
		case 34u: goto tr1360;
		case 37u: goto tr1361;
		case 38u: goto tr1362;
		case 40u: goto tr1363;
		case 41u: goto tr1364;
		case 42u: goto tr1365;
		case 43u: goto tr1366;
		case 44u: goto tr1367;
		case 45u: goto tr1368;
		case 46u: goto tr1369;
		case 47u: goto tr1370;
		case 48u: goto tr1371;
		case 58u: goto tr1373;
		case 59u: goto tr1374;
		case 60u: goto tr1375;
		case 61u: goto tr1376;
		case 62u: goto tr1377;
		case 63u: goto tr1378;
		case 91u: goto tr1380;
		case 93u: goto tr1381;
		case 94u: goto tr1382;
		case 95u: goto tr1379;
		case 123u: goto tr1383;
		case 124u: goto tr1384;
		case 125u: goto tr1385;
		case 126u: goto tr1386;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1372;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1379;
	} else
		goto tr1379;
	goto tr1;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 9u: goto tr1387;
		case 10u: goto tr1388;
		case 13u: goto tr1389;
		case 32u: goto tr1387;
		case 33u: goto tr1390;
		case 34u: goto tr1391;
		case 37u: goto tr1392;
		case 38u: goto tr1393;
		case 40u: goto tr1394;
		case 41u: goto tr1395;
		case 42u: goto tr1396;
		case 43u: goto tr1397;
		case 44u: goto tr1398;
		case 45u: goto tr1399;
		case 46u: goto tr1400;
		case 47u: goto tr1401;
		case 48u: goto tr1402;
		case 58u: goto tr1404;
		case 59u: goto tr1405;
		case 60u: goto tr1406;
		case 61u: goto st70;
		case 62u: goto tr1408;
		case 63u: goto tr1409;
		case 91u: goto tr1411;
		case 93u: goto tr1412;
		case 94u: goto tr1413;
		case 95u: goto tr1410;
		case 123u: goto tr1414;
		case 124u: goto tr1415;
		case 125u: goto tr1416;
		case 126u: goto tr1417;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1403;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1410;
	} else
		goto tr1410;
	goto tr1;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 9u: goto tr1418;
		case 10u: goto tr1419;
		case 13u: goto tr1420;
		case 32u: goto tr1418;
		case 33u: goto tr1421;
		case 34u: goto tr1422;
		case 37u: goto tr1423;
		case 38u: goto tr1424;
		case 40u: goto tr1425;
		case 41u: goto tr1426;
		case 42u: goto tr1427;
		case 43u: goto tr1428;
		case 44u: goto tr1429;
		case 45u: goto tr1430;
		case 46u: goto tr1431;
		case 47u: goto tr1432;
		case 48u: goto tr1433;
		case 58u: goto tr1435;
		case 59u: goto tr1436;
		case 60u: goto tr1437;
		case 61u: goto tr1438;
		case 62u: goto tr1439;
		case 63u: goto tr1440;
		case 91u: goto tr1442;
		case 93u: goto tr1443;
		case 94u: goto tr1444;
		case 95u: goto tr1441;
		case 123u: goto tr1445;
		case 124u: goto tr1446;
		case 125u: goto tr1447;
		case 126u: goto tr1448;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1434;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1441;
	} else
		goto tr1441;
	goto tr1;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 9u: goto tr1449;
		case 10u: goto tr1450;
		case 13u: goto tr1451;
		case 32u: goto tr1449;
		case 33u: goto tr1452;
		case 34u: goto tr1453;
		case 37u: goto tr1454;
		case 38u: goto tr1455;
		case 40u: goto tr1456;
		case 41u: goto tr1457;
		case 42u: goto tr1458;
		case 43u: goto tr1459;
		case 44u: goto tr1460;
		case 45u: goto tr1461;
		case 46u: goto tr1462;
		case 47u: goto tr1463;
		case 48u: goto tr1464;
		case 58u: goto tr1466;
		case 59u: goto tr1467;
		case 60u: goto tr1468;
		case 61u: goto tr1469;
		case 62u: goto tr1470;
		case 63u: goto tr1471;
		case 91u: goto tr1473;
		case 93u: goto tr1474;
		case 94u: goto tr1475;
		case 95u: goto tr1472;
		case 123u: goto tr1476;
		case 124u: goto tr1477;
		case 125u: goto tr1478;
		case 126u: goto tr1479;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1465;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1472;
	} else
		goto tr1472;
	goto tr1;
tr603:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 12775 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 43u: goto tr17;
		case 45u: goto tr17;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr18;
	goto tr1;
tr17:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 12791 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr18;
	goto tr1;
tr18:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st72;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
#line 12803 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr612;
		case 47u: goto tr593;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr18;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr602;
	} else
		goto tr602;
	goto tr1;
tr607:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 12853 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr19;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr19;
	} else
		goto tr19;
	goto tr1;
tr19:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st73;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
#line 12871 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr1480;
		case 47u: goto tr593;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 80u: goto tr603;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 112u: goto tr603;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
	}
	if ( (*p) < 71u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 70u )
				goto tr19;
		} else if ( (*p) >= 48u )
			goto tr19;
	} else if ( (*p) > 90u ) {
		if ( (*p) > 102u ) {
			if ( 103u <= (*p) && (*p) <= 122u )
				goto tr602;
		} else if ( (*p) >= 97u )
			goto tr19;
	} else
		goto tr602;
	goto tr1;
tr1480:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 12929 "xec_parser_ragel.cpp"
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
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 12947 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr579;
		case 10u: goto tr580;
		case 13u: goto tr581;
		case 32u: goto tr579;
		case 33u: goto tr582;
		case 34u: goto tr583;
		case 37u: goto tr584;
		case 38u: goto tr585;
		case 40u: goto tr586;
		case 41u: goto tr587;
		case 42u: goto tr588;
		case 43u: goto tr589;
		case 44u: goto tr590;
		case 45u: goto tr591;
		case 46u: goto tr612;
		case 47u: goto tr593;
		case 58u: goto tr596;
		case 59u: goto tr597;
		case 60u: goto tr598;
		case 61u: goto tr599;
		case 62u: goto tr600;
		case 63u: goto tr601;
		case 80u: goto tr603;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr602;
		case 112u: goto tr603;
		case 123u: goto tr608;
		case 124u: goto tr609;
		case 125u: goto tr610;
		case 126u: goto tr611;
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
				goto tr602;
		} else if ( (*p) >= 97u )
			goto tr20;
	} else
		goto tr602;
	goto tr1;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	switch( (*p) ) {
		case 9u: goto tr1481;
		case 10u: goto tr1482;
		case 13u: goto tr1483;
		case 32u: goto tr1481;
		case 33u: goto tr1484;
		case 34u: goto tr1485;
		case 37u: goto tr1486;
		case 38u: goto tr1487;
		case 40u: goto tr1488;
		case 41u: goto tr1489;
		case 42u: goto tr1490;
		case 43u: goto tr1491;
		case 44u: goto tr1492;
		case 45u: goto tr1493;
		case 46u: goto tr1494;
		case 47u: goto tr1495;
		case 48u: goto tr1496;
		case 58u: goto tr1498;
		case 59u: goto tr1499;
		case 60u: goto tr1500;
		case 61u: goto tr1501;
		case 62u: goto tr1502;
		case 63u: goto tr1503;
		case 91u: goto tr1505;
		case 93u: goto tr1506;
		case 94u: goto tr1507;
		case 95u: goto tr1504;
		case 123u: goto tr1508;
		case 124u: goto tr1509;
		case 125u: goto tr1510;
		case 126u: goto tr1511;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1497;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1504;
	} else
		goto tr1504;
	goto tr1;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 9u: goto tr1512;
		case 10u: goto tr1513;
		case 13u: goto tr1514;
		case 32u: goto tr1512;
		case 33u: goto tr1515;
		case 34u: goto tr1516;
		case 37u: goto tr1517;
		case 38u: goto tr1518;
		case 40u: goto tr1519;
		case 41u: goto tr1520;
		case 42u: goto tr1521;
		case 43u: goto tr1522;
		case 44u: goto tr1523;
		case 45u: goto tr1524;
		case 46u: goto tr1525;
		case 47u: goto tr1526;
		case 48u: goto tr1527;
		case 58u: goto tr1529;
		case 59u: goto tr1530;
		case 60u: goto tr1531;
		case 61u: goto tr1532;
		case 62u: goto tr1533;
		case 63u: goto tr1534;
		case 91u: goto tr1536;
		case 93u: goto tr1537;
		case 94u: goto tr1538;
		case 95u: goto tr1535;
		case 123u: goto tr1539;
		case 124u: goto tr1540;
		case 125u: goto tr1541;
		case 126u: goto tr1542;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1528;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1535;
	} else
		goto tr1535;
	goto tr1;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	switch( (*p) ) {
		case 9u: goto tr1543;
		case 10u: goto tr1544;
		case 13u: goto tr1545;
		case 32u: goto tr1543;
		case 33u: goto tr1546;
		case 34u: goto tr1547;
		case 37u: goto tr1548;
		case 38u: goto tr1549;
		case 40u: goto tr1550;
		case 41u: goto tr1551;
		case 42u: goto tr1552;
		case 43u: goto tr1553;
		case 44u: goto tr1554;
		case 45u: goto tr1555;
		case 46u: goto tr1556;
		case 47u: goto tr1557;
		case 48u: goto tr1558;
		case 58u: goto tr1560;
		case 59u: goto tr1561;
		case 60u: goto tr1562;
		case 61u: goto tr1563;
		case 62u: goto tr1564;
		case 63u: goto tr1565;
		case 91u: goto tr1567;
		case 93u: goto tr1568;
		case 94u: goto tr1569;
		case 95u: goto tr1566;
		case 123u: goto tr1570;
		case 124u: goto tr1571;
		case 125u: goto tr1572;
		case 126u: goto tr1573;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1559;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1566;
	} else
		goto tr1566;
	goto tr1;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 9u: goto tr1574;
		case 10u: goto tr1575;
		case 13u: goto tr1576;
		case 32u: goto tr1574;
		case 33u: goto tr1577;
		case 34u: goto tr1578;
		case 37u: goto tr1579;
		case 38u: goto tr1580;
		case 40u: goto tr1581;
		case 41u: goto tr1582;
		case 42u: goto tr1583;
		case 43u: goto tr1584;
		case 44u: goto tr1585;
		case 45u: goto tr1586;
		case 46u: goto tr1587;
		case 47u: goto tr1588;
		case 48u: goto tr1589;
		case 58u: goto tr1591;
		case 59u: goto tr1592;
		case 60u: goto tr1593;
		case 61u: goto tr1594;
		case 62u: goto tr1595;
		case 63u: goto tr1596;
		case 91u: goto tr1598;
		case 93u: goto tr1599;
		case 94u: goto tr1600;
		case 95u: goto tr1597;
		case 123u: goto tr1601;
		case 124u: goto tr1602;
		case 125u: goto tr1603;
		case 126u: goto tr1604;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1590;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1597;
	} else
		goto tr1597;
	goto tr1;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 9u: goto tr1605;
		case 10u: goto tr1606;
		case 13u: goto tr1607;
		case 32u: goto tr1605;
		case 33u: goto tr1608;
		case 34u: goto tr1609;
		case 37u: goto tr1610;
		case 38u: goto tr1611;
		case 40u: goto tr1612;
		case 41u: goto tr1613;
		case 42u: goto tr1614;
		case 43u: goto tr1615;
		case 44u: goto tr1616;
		case 45u: goto tr1617;
		case 46u: goto tr1618;
		case 47u: goto tr1619;
		case 48u: goto tr1620;
		case 58u: goto tr1622;
		case 59u: goto tr1623;
		case 60u: goto tr1624;
		case 61u: goto tr1625;
		case 62u: goto tr1626;
		case 63u: goto tr1627;
		case 91u: goto tr1629;
		case 93u: goto tr1630;
		case 94u: goto tr1631;
		case 95u: goto tr1628;
		case 123u: goto tr1632;
		case 124u: goto tr1633;
		case 125u: goto tr1634;
		case 126u: goto tr1635;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1621;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1628;
	} else
		goto tr1628;
	goto tr1;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 9u: goto tr1636;
		case 10u: goto tr1637;
		case 13u: goto tr1638;
		case 32u: goto tr1636;
		case 33u: goto tr1639;
		case 34u: goto tr1640;
		case 37u: goto tr1641;
		case 38u: goto tr1642;
		case 40u: goto tr1643;
		case 41u: goto tr1644;
		case 42u: goto tr1645;
		case 43u: goto tr1646;
		case 44u: goto tr1647;
		case 45u: goto tr1648;
		case 46u: goto tr1649;
		case 47u: goto tr1650;
		case 48u: goto tr1651;
		case 58u: goto tr1653;
		case 59u: goto tr1654;
		case 60u: goto tr1655;
		case 61u: goto tr1656;
		case 62u: goto tr1657;
		case 63u: goto tr1658;
		case 91u: goto tr1660;
		case 93u: goto tr1661;
		case 94u: goto tr1662;
		case 95u: goto tr1659;
		case 123u: goto tr1663;
		case 124u: goto tr1664;
		case 125u: goto tr1665;
		case 126u: goto tr1666;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1652;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1659;
	} else
		goto tr1659;
	goto tr1;
tr3:
#line 97 "xec_parser_ragel.rl"
	{
                    uloc = (int)( offset + ( p - buffer ) );
                }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 13283 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 34u: goto tr21;
		case 47u: goto tr22;
		case 92u: goto tr23;
		case 98u: goto tr24;
		case 102u: goto tr25;
		case 110u: goto tr26;
		case 114u: goto tr27;
		case 116u: goto tr28;
		case 117u: goto st13;
		case 120u: goto st19;
	}
	goto tr1;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr31;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr33;
	} else
		goto tr32;
	goto tr1;
tr31:
#line 100 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 20; }
	goto st14;
tr32:
#line 102 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 20; }
	goto st14;
tr33:
#line 101 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 20; }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 13326 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr34;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr36;
	} else
		goto tr35;
	goto tr1;
tr34:
#line 104 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 16; }
	goto st15;
tr35:
#line 106 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 16; }
	goto st15;
tr36:
#line 105 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 16; }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 13352 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr37;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr39;
	} else
		goto tr38;
	goto tr1;
tr37:
#line 108 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 12; }
	goto st16;
tr38:
#line 110 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 12; }
	goto st16;
tr39:
#line 109 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 12; }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 13378 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr40;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr42;
	} else
		goto tr41;
	goto tr1;
tr40:
#line 112 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 8; }
	goto st17;
tr41:
#line 114 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 8; }
	goto st17;
tr42:
#line 113 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 8; }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 13404 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr43;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr45;
	} else
		goto tr44;
	goto tr1;
tr43:
#line 116 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 4; }
	goto st18;
tr44:
#line 118 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 4; }
	goto st18;
tr45:
#line 117 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 4; }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 13430 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr46;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr48;
	} else
		goto tr47;
	goto tr1;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr49;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr51;
	} else
		goto tr50;
	goto tr1;
tr49:
#line 87 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 4; }
	goto st20;
tr50:
#line 89 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 4; }
	goto st20;
tr51:
#line 88 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 4; }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
#line 13469 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr52;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr54;
	} else
		goto tr53;
	goto tr1;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	switch( (*p) ) {
		case 9u: goto tr1667;
		case 10u: goto tr1668;
		case 13u: goto tr1669;
		case 32u: goto tr1667;
		case 33u: goto tr1670;
		case 34u: goto tr1671;
		case 37u: goto tr1672;
		case 38u: goto tr1673;
		case 40u: goto tr1674;
		case 41u: goto tr1675;
		case 42u: goto tr1676;
		case 43u: goto tr1677;
		case 44u: goto tr1678;
		case 45u: goto tr1679;
		case 46u: goto tr1680;
		case 47u: goto tr1681;
		case 48u: goto tr1682;
		case 58u: goto tr1684;
		case 59u: goto tr1685;
		case 60u: goto tr1686;
		case 61u: goto tr1687;
		case 62u: goto tr1688;
		case 63u: goto tr1689;
		case 91u: goto tr1691;
		case 93u: goto tr1692;
		case 94u: goto tr1693;
		case 95u: goto tr1690;
		case 123u: goto tr1694;
		case 124u: goto tr1695;
		case 125u: goto tr1696;
		case 126u: goto tr1697;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1683;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1690;
	} else
		goto tr1690;
	goto tr1;
tr99:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 13533 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 42u: goto st2;
		case 47u: goto st6;
	}
	goto tr1;
	}
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
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
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
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
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 24: 
	case 25: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	break;
	case 54: 
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	break;
	case 41: 
	case 42: 
	case 44: 
	case 72: 
	case 73: 
	case 74: 
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	break;
	case 27: 
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	break;
	case 26: 
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	break;
	case 28: 
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	break;
	case 29: 
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	break;
	case 31: 
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	break;
	case 32: 
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	break;
	case 33: 
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	break;
	case 34: 
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	break;
	case 36: 
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	break;
	case 37: 
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	break;
	case 39: 
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	break;
	case 40: 
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	break;
	case 43: 
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	break;
	case 45: 
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	break;
	case 46: 
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	break;
	case 49: 
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	break;
	case 51: 
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	break;
	case 53: 
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	break;
	case 55: 
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	break;
	case 56: 
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	break;
	case 57: 
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	break;
	case 59: 
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	break;
	case 60: 
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	break;
	case 62: 
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	break;
	case 63: 
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	break;
	case 35: 
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	break;
	case 38: 
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	break;
	case 47: 
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	break;
	case 67: 
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	break;
	case 69: 
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	break;
	case 81: 
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	break;
	case 71: 
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	break;
	case 50: 
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	break;
	case 52: 
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	break;
	case 80: 
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	break;
	case 79: 
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	break;
	case 78: 
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	break;
	case 77: 
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	break;
	case 76: 
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	break;
	case 75: 
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	break;
	case 58: 
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	break;
	case 61: 
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	break;
	case 64: 
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	break;
	case 48: 
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	break;
	case 68: 
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	break;
	case 70: 
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	break;
	case 30: 
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	break;
	case 66: 
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	break;
	case 65: 
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	break;
	case 1: 
	case 2: 
	case 5: 
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
#line 234 "xec_parser_ragel.rl"
	{ {goto st22;} }
	break;
	case 3: 
	case 4: 
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
#line 234 "xec_parser_ragel.rl"
	{ {goto st22;} }
	break;
#line 13918 "xec_parser_ragel.cpp"
	}
	}

	_out: {}
	}

#line 390 "xec_parser_ragel.rl"
        
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





