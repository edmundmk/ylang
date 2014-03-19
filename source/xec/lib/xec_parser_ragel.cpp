
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





#line 246 "xec_parser_ragel.rl"




#line 31 "xec_parser_ragel.cpp"
static const int lexer_start = 30;
static const int lexer_first_final = 30;
static const int lexer_error = 0;

static const int lexer_en_main = 30;


#line 250 "xec_parser_ragel.rl"



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

#line 373 "xec_parser_ragel.rl"
    
    
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
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	switch( (*p) ) {
		case 9u: goto st31;
		case 10u: goto st32;
		case 13u: goto st33;
		case 32u: goto st31;
		case 33u: goto tr69;
		case 34u: goto tr70;
		case 37u: goto tr71;
		case 38u: goto tr72;
		case 40u: goto tr73;
		case 41u: goto tr74;
		case 42u: goto tr75;
		case 43u: goto tr76;
		case 44u: goto tr77;
		case 45u: goto tr78;
		case 46u: goto tr79;
		case 47u: goto tr80;
		case 48u: goto tr81;
		case 58u: goto tr83;
		case 59u: goto tr84;
		case 60u: goto tr85;
		case 61u: goto tr86;
		case 62u: goto tr87;
		case 63u: goto tr88;
		case 91u: goto tr90;
		case 93u: goto tr91;
		case 94u: goto tr92;
		case 95u: goto tr89;
		case 123u: goto tr93;
		case 124u: goto tr94;
		case 125u: goto tr95;
		case 126u: goto tr96;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr82;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr89;
	} else
		goto tr89;
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
#line 244 "xec_parser_ragel.rl"
	{ {goto st30;} }
	goto st0;
tr98:
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
#line 244 "xec_parser_ragel.rl"
	{ {goto st30;} }
	goto st0;
#line 280 "xec_parser_ragel.cpp"
st0:
cs = 0;
	goto _out;
tr99:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st31;
tr130:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st31;
tr221:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st31;
tr252:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st31;
tr283:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st31;
tr314:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st31;
tr345:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st31;
tr376:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st31;
tr407:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st31;
tr438:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st31;
tr469:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st31;
tr500:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st31;
tr531:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st31;
tr562:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st31;
tr593:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st31;
tr624:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st31;
tr653:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st31;
tr687:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st31;
tr719:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st31;
tr750:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st31;
tr781:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st31;
tr812:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st31;
tr843:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st31;
tr874:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st31;
tr905:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st31;
tr936:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st31;
tr967:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st31;
tr998:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st31;
tr1027:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st31;
tr1058:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st31;
tr1089:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st31;
tr1120:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st31;
tr1151:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st31;
tr1182:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st31;
tr1213:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st31;
tr1244:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st31;
tr1275:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st31;
tr1306:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st31;
tr1337:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st31;
tr1368:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st31;
tr1399:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st31;
tr1430:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st31;
tr1461:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st31;
tr1492:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st31;
tr1523:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st31;
tr1554:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st31;
tr1586:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st31;
tr1617:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st31;
tr1648:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st31;
tr1679:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st31;
tr1710:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st31;
tr1741:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st31;
tr1772:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st31;
tr1803:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st31;
tr1834:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 516 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto st31;
		case 10u: goto st32;
		case 13u: goto st33;
		case 32u: goto st31;
		case 33u: goto tr69;
		case 34u: goto tr70;
		case 37u: goto tr71;
		case 38u: goto tr72;
		case 40u: goto tr73;
		case 41u: goto tr74;
		case 42u: goto tr75;
		case 43u: goto tr76;
		case 44u: goto tr77;
		case 45u: goto tr78;
		case 46u: goto tr79;
		case 47u: goto st29;
		case 48u: goto tr81;
		case 58u: goto tr83;
		case 59u: goto tr84;
		case 60u: goto tr85;
		case 61u: goto tr86;
		case 62u: goto tr87;
		case 63u: goto tr88;
		case 91u: goto tr90;
		case 93u: goto tr91;
		case 94u: goto tr92;
		case 95u: goto tr89;
		case 123u: goto tr93;
		case 124u: goto tr94;
		case 125u: goto tr95;
		case 126u: goto tr96;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr82;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr89;
	} else
		goto tr89;
	goto tr1;
tr100:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st32;
tr131:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st32;
tr222:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st32;
tr253:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st32;
tr284:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st32;
tr315:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st32;
tr346:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st32;
tr377:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st32;
tr408:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st32;
tr439:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st32;
tr470:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st32;
tr501:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st32;
tr532:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st32;
tr563:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st32;
tr594:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st32;
tr625:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st32;
tr654:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st32;
tr688:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st32;
tr720:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st32;
tr751:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st32;
tr782:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st32;
tr813:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st32;
tr844:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st32;
tr875:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st32;
tr906:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st32;
tr937:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st32;
tr968:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st32;
tr999:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st32;
tr1028:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st32;
tr1059:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st32;
tr1090:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st32;
tr1121:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st32;
tr1152:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st32;
tr1183:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st32;
tr1214:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st32;
tr1245:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st32;
tr1276:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st32;
tr1307:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st32;
tr1338:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st32;
tr1369:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st32;
tr1400:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st32;
tr1431:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st32;
tr1462:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st32;
tr1493:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st32;
tr1524:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st32;
tr1555:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st32;
tr1587:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st32;
tr1618:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st32;
tr1649:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st32;
tr1680:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st32;
tr1711:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st32;
tr1742:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st32;
tr1773:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st32;
tr1804:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st32;
tr1835:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 791 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr99;
		case 10u: goto tr100;
		case 13u: goto tr101;
		case 32u: goto tr99;
		case 33u: goto tr102;
		case 34u: goto tr103;
		case 37u: goto tr104;
		case 38u: goto tr105;
		case 40u: goto tr106;
		case 41u: goto tr107;
		case 42u: goto tr108;
		case 43u: goto tr109;
		case 44u: goto tr110;
		case 45u: goto tr111;
		case 46u: goto tr112;
		case 47u: goto tr113;
		case 48u: goto tr114;
		case 58u: goto tr116;
		case 59u: goto tr117;
		case 60u: goto tr118;
		case 61u: goto tr119;
		case 62u: goto tr120;
		case 63u: goto tr121;
		case 91u: goto tr123;
		case 93u: goto tr124;
		case 94u: goto tr125;
		case 95u: goto tr122;
		case 123u: goto tr126;
		case 124u: goto tr127;
		case 125u: goto tr128;
		case 126u: goto tr129;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr115;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr98;
tr101:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st33;
tr132:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st33;
tr223:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st33;
tr254:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st33;
tr285:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st33;
tr316:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st33;
tr347:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st33;
tr378:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st33;
tr409:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st33;
tr440:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st33;
tr471:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st33;
tr502:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st33;
tr533:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st33;
tr564:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st33;
tr595:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st33;
tr626:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st33;
tr655:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st33;
tr689:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st33;
tr721:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st33;
tr752:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st33;
tr783:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st33;
tr814:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st33;
tr845:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st33;
tr876:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st33;
tr907:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st33;
tr938:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st33;
tr969:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st33;
tr1000:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st33;
tr1029:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st33;
tr1060:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st33;
tr1091:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st33;
tr1122:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st33;
tr1153:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st33;
tr1184:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st33;
tr1215:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st33;
tr1246:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st33;
tr1277:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st33;
tr1308:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st33;
tr1339:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st33;
tr1370:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st33;
tr1401:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st33;
tr1432:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st33;
tr1463:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st33;
tr1494:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st33;
tr1525:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st33;
tr1556:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	goto st33;
tr1588:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st33;
tr1619:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st33;
tr1650:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st33;
tr1681:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st33;
tr1712:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st33;
tr1743:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st33;
tr1774:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st33;
tr1805:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	goto st33;
tr1836:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 1066 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr99;
		case 10u: goto st32;
		case 13u: goto tr101;
		case 32u: goto tr99;
		case 33u: goto tr102;
		case 34u: goto tr103;
		case 37u: goto tr104;
		case 38u: goto tr105;
		case 40u: goto tr106;
		case 41u: goto tr107;
		case 42u: goto tr108;
		case 43u: goto tr109;
		case 44u: goto tr110;
		case 45u: goto tr111;
		case 46u: goto tr112;
		case 47u: goto tr113;
		case 48u: goto tr114;
		case 58u: goto tr116;
		case 59u: goto tr117;
		case 60u: goto tr118;
		case 61u: goto tr119;
		case 62u: goto tr120;
		case 63u: goto tr121;
		case 91u: goto tr123;
		case 93u: goto tr124;
		case 94u: goto tr125;
		case 95u: goto tr122;
		case 123u: goto tr126;
		case 124u: goto tr127;
		case 125u: goto tr128;
		case 126u: goto tr129;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr115;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr98;
tr69:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr102:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr133:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr162:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr193:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr224:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr255:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr286:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr317:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr348:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr379:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr410:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr441:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr472:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr503:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr534:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr565:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr596:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr627:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr656:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr690:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr722:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr753:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr784:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr815:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr846:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr877:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr908:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr939:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr970:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1001:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1030:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1061:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1092:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1123:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1154:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1185:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1216:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1247:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1278:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1309:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1340:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1371:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1402:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1433:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1464:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1495:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1526:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1557:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1589:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1620:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1651:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1682:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1713:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1744:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1775:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1806:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1837:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 1479 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr130;
		case 10u: goto tr131;
		case 13u: goto tr132;
		case 32u: goto tr130;
		case 33u: goto tr133;
		case 34u: goto tr134;
		case 37u: goto tr135;
		case 38u: goto tr136;
		case 40u: goto tr137;
		case 41u: goto tr138;
		case 42u: goto tr139;
		case 43u: goto tr140;
		case 44u: goto tr141;
		case 45u: goto tr142;
		case 46u: goto tr143;
		case 47u: goto tr144;
		case 48u: goto tr145;
		case 58u: goto tr147;
		case 59u: goto tr148;
		case 60u: goto tr149;
		case 61u: goto st93;
		case 62u: goto tr151;
		case 63u: goto tr152;
		case 91u: goto tr154;
		case 93u: goto tr155;
		case 94u: goto tr156;
		case 95u: goto tr153;
		case 105u: goto st28;
		case 123u: goto tr158;
		case 124u: goto tr159;
		case 125u: goto tr160;
		case 126u: goto tr161;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr146;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr153;
	} else
		goto tr153;
	goto tr1;
tr0:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st1;
tr194:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st1;
tr33:
#line 78 "xec_parser_ragel.rl"
	{ data.append( '\"' ); }
	goto st1;
tr34:
#line 80 "xec_parser_ragel.rl"
	{ data.append( '/' ); }
	goto st1;
tr35:
#line 79 "xec_parser_ragel.rl"
	{ data.append( '\\' ); }
	goto st1;
tr36:
#line 81 "xec_parser_ragel.rl"
	{ data.append( '\b' ); }
	goto st1;
tr37:
#line 82 "xec_parser_ragel.rl"
	{ data.append( '\f' ); }
	goto st1;
tr38:
#line 83 "xec_parser_ragel.rl"
	{ data.append( '\n' ); }
	goto st1;
tr39:
#line 84 "xec_parser_ragel.rl"
	{ data.append( '\r' ); }
	goto st1;
tr40:
#line 85 "xec_parser_ragel.rl"
	{ data.append( '\t' ); }
	goto st1;
tr58:
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
tr59:
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
tr60:
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
tr64:
#line 91 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - '0' ) ); }
	goto st1;
tr65:
#line 93 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'A' + 0x0A ) ); }
	goto st1;
tr66:
#line 92 "xec_parser_ragel.rl"
	{ data.append( (char)( temp | (*p) - 'a' + 0x0A ) ); }
	goto st1;
tr70:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr103:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr134:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr225:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr256:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr287:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr318:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr349:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr380:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr411:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr442:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr473:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr504:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr535:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr566:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr597:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr628:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr657:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr691:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr723:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr754:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr785:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr816:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr847:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr878:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr909:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr940:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr971:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1002:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1031:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1062:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1093:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1124:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1155:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1186:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1217:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1248:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1279:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1310:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1341:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1372:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1403:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1434:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1465:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1496:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1527:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1558:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1590:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1621:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1652:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1683:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1714:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1745:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1776:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1807:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1838:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 1963 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr1;
		case 13u: goto tr1;
		case 34u: goto st35;
		case 92u: goto tr3;
	}
	goto tr0;
tr190:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 1979 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto st35;
		case 10u: goto st36;
		case 13u: goto st37;
		case 32u: goto st35;
		case 33u: goto tr162;
		case 34u: goto st1;
		case 37u: goto tr164;
		case 38u: goto tr165;
		case 40u: goto tr166;
		case 41u: goto tr167;
		case 42u: goto tr168;
		case 43u: goto tr169;
		case 44u: goto tr170;
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 47u: goto st13;
		case 48u: goto tr174;
		case 58u: goto tr176;
		case 59u: goto tr177;
		case 60u: goto tr178;
		case 61u: goto tr179;
		case 62u: goto tr180;
		case 63u: goto tr181;
		case 91u: goto tr183;
		case 93u: goto tr184;
		case 94u: goto tr185;
		case 95u: goto tr182;
		case 123u: goto tr186;
		case 124u: goto tr187;
		case 125u: goto tr188;
		case 126u: goto tr189;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr175;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr182;
	} else
		goto tr182;
	goto tr1;
tr191:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 2030 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr190;
		case 10u: goto tr191;
		case 13u: goto tr192;
		case 32u: goto tr190;
		case 33u: goto tr193;
		case 34u: goto tr194;
		case 37u: goto tr195;
		case 38u: goto tr196;
		case 40u: goto tr197;
		case 41u: goto tr198;
		case 42u: goto tr199;
		case 43u: goto tr200;
		case 44u: goto tr201;
		case 45u: goto tr202;
		case 46u: goto tr203;
		case 47u: goto tr204;
		case 48u: goto tr205;
		case 58u: goto tr207;
		case 59u: goto tr208;
		case 60u: goto tr209;
		case 61u: goto tr210;
		case 62u: goto tr211;
		case 63u: goto tr212;
		case 91u: goto tr214;
		case 93u: goto tr215;
		case 94u: goto tr216;
		case 95u: goto tr213;
		case 123u: goto tr217;
		case 124u: goto tr218;
		case 125u: goto tr219;
		case 126u: goto tr220;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr206;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr213;
	} else
		goto tr213;
	goto tr98;
tr192:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 2081 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr190;
		case 10u: goto st36;
		case 13u: goto tr192;
		case 32u: goto tr190;
		case 33u: goto tr193;
		case 34u: goto tr194;
		case 37u: goto tr195;
		case 38u: goto tr196;
		case 40u: goto tr197;
		case 41u: goto tr198;
		case 42u: goto tr199;
		case 43u: goto tr200;
		case 44u: goto tr201;
		case 45u: goto tr202;
		case 46u: goto tr203;
		case 47u: goto tr204;
		case 48u: goto tr205;
		case 58u: goto tr207;
		case 59u: goto tr208;
		case 60u: goto tr209;
		case 61u: goto tr210;
		case 62u: goto tr211;
		case 63u: goto tr212;
		case 91u: goto tr214;
		case 93u: goto tr215;
		case 94u: goto tr216;
		case 95u: goto tr213;
		case 123u: goto tr217;
		case 124u: goto tr218;
		case 125u: goto tr219;
		case 126u: goto tr220;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr206;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr213;
	} else
		goto tr213;
	goto tr98;
tr71:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr104:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr135:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr164:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr195:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr226:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr257:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr288:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr319:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr350:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr381:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr412:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr443:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr474:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr505:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr536:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr567:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr598:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr629:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr658:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr692:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr724:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr755:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr786:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr817:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr848:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr879:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr910:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr941:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr972:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1003:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1032:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1063:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1094:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1125:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1156:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1187:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1218:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1249:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1280:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1311:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1342:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1373:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1404:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1435:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1466:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1497:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1528:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1559:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1591:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1622:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1653:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1684:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1715:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1746:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1777:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1808:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1839:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 2494 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr221;
		case 10u: goto tr222;
		case 13u: goto tr223;
		case 32u: goto tr221;
		case 33u: goto tr224;
		case 34u: goto tr225;
		case 37u: goto tr226;
		case 38u: goto tr227;
		case 40u: goto tr228;
		case 41u: goto tr229;
		case 42u: goto tr230;
		case 43u: goto tr231;
		case 44u: goto tr232;
		case 45u: goto tr233;
		case 46u: goto tr234;
		case 47u: goto tr235;
		case 48u: goto tr236;
		case 58u: goto tr238;
		case 59u: goto tr239;
		case 60u: goto tr240;
		case 61u: goto st92;
		case 62u: goto tr242;
		case 63u: goto tr243;
		case 91u: goto tr245;
		case 93u: goto tr246;
		case 94u: goto tr247;
		case 95u: goto tr244;
		case 123u: goto tr248;
		case 124u: goto tr249;
		case 125u: goto tr250;
		case 126u: goto tr251;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr237;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr244;
	} else
		goto tr244;
	goto tr1;
tr72:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr105:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr136:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr165:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr196:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr227:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr289:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr320:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr351:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr382:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr413:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr444:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr475:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr506:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr537:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr568:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr599:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr630:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr659:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr693:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr725:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr756:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr787:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr818:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr849:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr880:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr911:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr942:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr973:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1004:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1033:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1064:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1095:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1126:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1157:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1188:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1219:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1250:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1281:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1312:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1343:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1374:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1405:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1436:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1467:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1498:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1529:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1560:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1592:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1623:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1654:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1685:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1716:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1747:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1778:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1809:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1840:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 2901 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr252;
		case 10u: goto tr253;
		case 13u: goto tr254;
		case 32u: goto tr252;
		case 33u: goto tr255;
		case 34u: goto tr256;
		case 37u: goto tr257;
		case 38u: goto st40;
		case 40u: goto tr259;
		case 41u: goto tr260;
		case 42u: goto tr261;
		case 43u: goto tr262;
		case 44u: goto tr263;
		case 45u: goto tr264;
		case 46u: goto tr265;
		case 47u: goto tr266;
		case 48u: goto tr267;
		case 58u: goto tr269;
		case 59u: goto tr270;
		case 60u: goto tr271;
		case 61u: goto st91;
		case 62u: goto tr273;
		case 63u: goto tr274;
		case 91u: goto tr276;
		case 93u: goto tr277;
		case 94u: goto tr278;
		case 95u: goto tr275;
		case 123u: goto tr279;
		case 124u: goto tr280;
		case 125u: goto tr281;
		case 126u: goto tr282;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr268;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr275;
	} else
		goto tr275;
	goto tr1;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 9u: goto tr283;
		case 10u: goto tr284;
		case 13u: goto tr285;
		case 32u: goto tr283;
		case 33u: goto tr286;
		case 34u: goto tr287;
		case 37u: goto tr288;
		case 38u: goto tr289;
		case 40u: goto tr290;
		case 41u: goto tr291;
		case 42u: goto tr292;
		case 43u: goto tr293;
		case 44u: goto tr294;
		case 45u: goto tr295;
		case 46u: goto tr296;
		case 47u: goto tr297;
		case 48u: goto tr298;
		case 58u: goto tr300;
		case 59u: goto tr301;
		case 60u: goto tr302;
		case 61u: goto tr303;
		case 62u: goto tr304;
		case 63u: goto tr305;
		case 91u: goto tr307;
		case 93u: goto tr308;
		case 94u: goto tr309;
		case 95u: goto tr306;
		case 123u: goto tr310;
		case 124u: goto tr311;
		case 125u: goto tr312;
		case 126u: goto tr313;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr299;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr306;
	} else
		goto tr306;
	goto tr1;
tr73:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr106:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr137:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr166:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr197:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr228:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr259:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr290:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr321:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr352:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr383:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr414:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr445:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr476:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr507:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr538:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr569:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr600:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr631:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr660:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr694:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr726:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr757:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr788:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr819:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr850:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr881:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr912:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr943:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr974:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1005:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1034:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1065:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1096:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1127:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1158:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1189:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1220:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1251:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1282:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1313:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1344:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1375:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1406:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1437:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1468:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1499:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1530:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1561:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1593:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1624:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1655:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1686:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1717:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1748:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1779:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1810:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
tr1841:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 3360 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr314;
		case 10u: goto tr315;
		case 13u: goto tr316;
		case 32u: goto tr314;
		case 33u: goto tr317;
		case 34u: goto tr318;
		case 37u: goto tr319;
		case 38u: goto tr320;
		case 40u: goto tr321;
		case 41u: goto tr322;
		case 42u: goto tr323;
		case 43u: goto tr324;
		case 44u: goto tr325;
		case 45u: goto tr326;
		case 46u: goto tr327;
		case 47u: goto tr328;
		case 48u: goto tr329;
		case 58u: goto tr331;
		case 59u: goto tr332;
		case 60u: goto tr333;
		case 61u: goto tr334;
		case 62u: goto tr335;
		case 63u: goto tr336;
		case 91u: goto tr338;
		case 93u: goto tr339;
		case 94u: goto tr340;
		case 95u: goto tr337;
		case 123u: goto tr341;
		case 124u: goto tr342;
		case 125u: goto tr343;
		case 126u: goto tr344;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr330;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr337;
	} else
		goto tr337;
	goto tr1;
tr74:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr107:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr138:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr167:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr198:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr229:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr260:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr291:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr322:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr353:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr384:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr415:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr446:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr477:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr508:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr539:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr570:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr601:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr632:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr661:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr695:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr727:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr758:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr789:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr820:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr851:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr882:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr913:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr944:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr975:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1006:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1035:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1066:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1097:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1128:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1159:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1190:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1221:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1252:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1283:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1314:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1345:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1376:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1407:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1438:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1469:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1500:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1531:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1562:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1594:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1625:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1656:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1687:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1718:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1749:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1780:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1811:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1842:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 3773 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr345;
		case 10u: goto tr346;
		case 13u: goto tr347;
		case 32u: goto tr345;
		case 33u: goto tr348;
		case 34u: goto tr349;
		case 37u: goto tr350;
		case 38u: goto tr351;
		case 40u: goto tr352;
		case 41u: goto tr353;
		case 42u: goto tr354;
		case 43u: goto tr355;
		case 44u: goto tr356;
		case 45u: goto tr357;
		case 46u: goto tr358;
		case 47u: goto tr359;
		case 48u: goto tr360;
		case 58u: goto tr362;
		case 59u: goto tr363;
		case 60u: goto tr364;
		case 61u: goto tr365;
		case 62u: goto tr366;
		case 63u: goto tr367;
		case 91u: goto tr369;
		case 93u: goto tr370;
		case 94u: goto tr371;
		case 95u: goto tr368;
		case 123u: goto tr372;
		case 124u: goto tr373;
		case 125u: goto tr374;
		case 126u: goto tr375;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr361;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr368;
	} else
		goto tr368;
	goto tr1;
tr75:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr108:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr139:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr168:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr199:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr230:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr261:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr292:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr323:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr354:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr385:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr416:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr447:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr478:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr509:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr540:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr571:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr602:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr662:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr696:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr728:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr759:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr790:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr821:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr852:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr883:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr914:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr945:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr976:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1007:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1036:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1067:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1098:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1129:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1160:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1191:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1222:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1253:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1284:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1315:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1346:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1377:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1408:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1439:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1470:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1501:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1532:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1563:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1595:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1626:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1657:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1688:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1719:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1750:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1781:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1812:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
tr1843:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 4180 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr376;
		case 10u: goto tr377;
		case 13u: goto tr378;
		case 32u: goto tr376;
		case 33u: goto tr379;
		case 34u: goto tr380;
		case 37u: goto tr381;
		case 38u: goto tr382;
		case 40u: goto tr383;
		case 41u: goto tr384;
		case 42u: goto tr385;
		case 43u: goto tr386;
		case 44u: goto tr387;
		case 45u: goto tr388;
		case 46u: goto tr389;
		case 47u: goto tr390;
		case 48u: goto tr391;
		case 58u: goto tr393;
		case 59u: goto tr394;
		case 60u: goto tr395;
		case 61u: goto st90;
		case 62u: goto tr397;
		case 63u: goto tr398;
		case 91u: goto tr400;
		case 93u: goto tr401;
		case 94u: goto tr402;
		case 95u: goto tr399;
		case 123u: goto tr403;
		case 124u: goto tr404;
		case 125u: goto tr405;
		case 126u: goto tr406;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr392;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr399;
	} else
		goto tr399;
	goto tr1;
tr76:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr109:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr140:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr169:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr200:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr231:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr262:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr293:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr324:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr355:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr386:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr448:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr479:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr510:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr541:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr572:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr603:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr633:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr663:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr697:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr729:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr760:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr791:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr822:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr853:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr884:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr915:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr946:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr977:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1008:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1037:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1068:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1099:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1130:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1161:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1192:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1223:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1254:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1285:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1316:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1347:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1378:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1409:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1440:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1471:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1502:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1533:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1564:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1596:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1627:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1658:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1689:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1720:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1751:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1782:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1813:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1844:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 4587 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr407;
		case 10u: goto tr408;
		case 13u: goto tr409;
		case 32u: goto tr407;
		case 33u: goto tr410;
		case 34u: goto tr411;
		case 37u: goto tr412;
		case 38u: goto tr413;
		case 40u: goto tr414;
		case 41u: goto tr415;
		case 42u: goto tr416;
		case 43u: goto st45;
		case 44u: goto tr418;
		case 45u: goto tr419;
		case 46u: goto tr420;
		case 47u: goto tr421;
		case 48u: goto tr422;
		case 58u: goto tr424;
		case 59u: goto tr425;
		case 60u: goto tr426;
		case 61u: goto st89;
		case 62u: goto tr428;
		case 63u: goto tr429;
		case 91u: goto tr431;
		case 93u: goto tr432;
		case 94u: goto tr433;
		case 95u: goto tr430;
		case 123u: goto tr434;
		case 124u: goto tr435;
		case 125u: goto tr436;
		case 126u: goto tr437;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr423;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr430;
	} else
		goto tr430;
	goto tr1;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 9u: goto tr438;
		case 10u: goto tr439;
		case 13u: goto tr440;
		case 32u: goto tr438;
		case 33u: goto tr441;
		case 34u: goto tr442;
		case 37u: goto tr443;
		case 38u: goto tr444;
		case 40u: goto tr445;
		case 41u: goto tr446;
		case 42u: goto tr447;
		case 43u: goto tr448;
		case 44u: goto tr449;
		case 45u: goto tr450;
		case 46u: goto tr451;
		case 47u: goto tr452;
		case 48u: goto tr453;
		case 58u: goto tr455;
		case 59u: goto tr456;
		case 60u: goto tr457;
		case 61u: goto tr458;
		case 62u: goto tr459;
		case 63u: goto tr460;
		case 91u: goto tr462;
		case 93u: goto tr463;
		case 94u: goto tr464;
		case 95u: goto tr461;
		case 123u: goto tr465;
		case 124u: goto tr466;
		case 125u: goto tr467;
		case 126u: goto tr468;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr454;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr461;
	} else
		goto tr461;
	goto tr1;
tr77:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr110:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr141:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr170:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr201:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr232:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr263:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr294:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr325:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr356:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr387:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr418:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr449:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr480:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr511:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr542:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr573:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr604:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr634:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr664:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr698:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr730:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr761:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr792:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr823:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr854:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr885:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr916:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr947:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr978:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1009:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1038:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1069:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1100:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1131:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1162:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1193:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1224:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1255:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1286:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1317:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1348:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1379:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1410:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1441:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1472:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1503:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1534:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1565:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1597:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1628:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1659:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1690:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1721:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1752:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1783:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1814:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
tr1845:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 5046 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr469;
		case 10u: goto tr470;
		case 13u: goto tr471;
		case 32u: goto tr469;
		case 33u: goto tr472;
		case 34u: goto tr473;
		case 37u: goto tr474;
		case 38u: goto tr475;
		case 40u: goto tr476;
		case 41u: goto tr477;
		case 42u: goto tr478;
		case 43u: goto tr479;
		case 44u: goto tr480;
		case 45u: goto tr481;
		case 46u: goto tr482;
		case 47u: goto tr483;
		case 48u: goto tr484;
		case 58u: goto tr486;
		case 59u: goto tr487;
		case 60u: goto tr488;
		case 61u: goto tr489;
		case 62u: goto tr490;
		case 63u: goto tr491;
		case 91u: goto tr493;
		case 93u: goto tr494;
		case 94u: goto tr495;
		case 95u: goto tr492;
		case 123u: goto tr496;
		case 124u: goto tr497;
		case 125u: goto tr498;
		case 126u: goto tr499;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr485;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr492;
	} else
		goto tr492;
	goto tr1;
tr78:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr111:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr142:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr171:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr202:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr233:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr264:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr295:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr326:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr357:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr388:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr419:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr450:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr481:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr543:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr574:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr605:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr635:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr665:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr699:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr731:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr762:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr793:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr824:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr855:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr886:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr917:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr948:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr979:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1010:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1039:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1070:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1101:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1132:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1163:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1194:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1225:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1256:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1287:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1318:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1349:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1380:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1411:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1442:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1473:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1504:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1535:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1566:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1598:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1629:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1660:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1691:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1722:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1753:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1784:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1815:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1846:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
#line 5453 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr500;
		case 10u: goto tr501;
		case 13u: goto tr502;
		case 32u: goto tr500;
		case 33u: goto tr503;
		case 34u: goto tr504;
		case 37u: goto tr505;
		case 38u: goto tr506;
		case 40u: goto tr507;
		case 41u: goto tr508;
		case 42u: goto tr509;
		case 43u: goto tr510;
		case 44u: goto tr511;
		case 45u: goto st48;
		case 46u: goto tr513;
		case 47u: goto tr514;
		case 48u: goto tr515;
		case 58u: goto tr517;
		case 59u: goto tr518;
		case 60u: goto tr519;
		case 61u: goto st88;
		case 62u: goto tr521;
		case 63u: goto tr522;
		case 91u: goto tr524;
		case 93u: goto tr525;
		case 94u: goto tr526;
		case 95u: goto tr523;
		case 123u: goto tr527;
		case 124u: goto tr528;
		case 125u: goto tr529;
		case 126u: goto tr530;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr516;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr523;
	} else
		goto tr523;
	goto tr1;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	switch( (*p) ) {
		case 9u: goto tr531;
		case 10u: goto tr532;
		case 13u: goto tr533;
		case 32u: goto tr531;
		case 33u: goto tr534;
		case 34u: goto tr535;
		case 37u: goto tr536;
		case 38u: goto tr537;
		case 40u: goto tr538;
		case 41u: goto tr539;
		case 42u: goto tr540;
		case 43u: goto tr541;
		case 44u: goto tr542;
		case 45u: goto tr543;
		case 46u: goto tr544;
		case 47u: goto tr545;
		case 48u: goto tr546;
		case 58u: goto tr548;
		case 59u: goto tr549;
		case 60u: goto tr550;
		case 61u: goto tr551;
		case 62u: goto tr552;
		case 63u: goto tr553;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr554;
		case 123u: goto tr558;
		case 124u: goto tr559;
		case 125u: goto tr560;
		case 126u: goto tr561;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr547;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr554;
	} else
		goto tr554;
	goto tr1;
tr79:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr112:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr143:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr172:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr203:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr234:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr265:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr296:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr327:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr358:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr389:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr420:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr451:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr482:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr513:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr544:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr606:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr636:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr686:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr700:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr732:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr763:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr794:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr825:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr856:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr887:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr918:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr949:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr980:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1011:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1040:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1071:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1102:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1133:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1164:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1195:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1226:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1257:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1288:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1319:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1350:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1381:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1412:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1443:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1474:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1505:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1536:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1567:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1599:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1630:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1661:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1692:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1723:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1754:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1785:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1816:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
tr1847:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st49;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
#line 5906 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr562;
		case 10u: goto tr563;
		case 13u: goto tr564;
		case 32u: goto tr562;
		case 33u: goto tr565;
		case 34u: goto tr566;
		case 37u: goto tr567;
		case 38u: goto tr568;
		case 40u: goto tr569;
		case 41u: goto tr570;
		case 42u: goto tr571;
		case 43u: goto tr572;
		case 44u: goto tr573;
		case 45u: goto tr574;
		case 46u: goto st2;
		case 47u: goto tr576;
		case 48u: goto tr577;
		case 58u: goto tr579;
		case 59u: goto tr580;
		case 60u: goto tr581;
		case 61u: goto tr582;
		case 62u: goto tr583;
		case 63u: goto tr584;
		case 91u: goto tr586;
		case 93u: goto tr587;
		case 94u: goto tr588;
		case 95u: goto tr585;
		case 123u: goto tr589;
		case 124u: goto tr590;
		case 125u: goto tr591;
		case 126u: goto tr592;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr578;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr585;
	} else
		goto tr585;
	goto tr1;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 46u )
		goto st50;
	goto tr1;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9u: goto tr593;
		case 10u: goto tr594;
		case 13u: goto tr595;
		case 32u: goto tr593;
		case 33u: goto tr596;
		case 34u: goto tr597;
		case 37u: goto tr598;
		case 38u: goto tr599;
		case 40u: goto tr600;
		case 41u: goto tr601;
		case 42u: goto tr602;
		case 43u: goto tr603;
		case 44u: goto tr604;
		case 45u: goto tr605;
		case 46u: goto tr606;
		case 47u: goto tr607;
		case 48u: goto tr608;
		case 58u: goto tr610;
		case 59u: goto tr611;
		case 60u: goto tr612;
		case 61u: goto tr613;
		case 62u: goto tr614;
		case 63u: goto tr615;
		case 91u: goto tr617;
		case 93u: goto tr618;
		case 94u: goto tr619;
		case 95u: goto tr616;
		case 123u: goto tr620;
		case 124u: goto tr621;
		case 125u: goto tr622;
		case 126u: goto tr623;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr609;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr616;
	} else
		goto tr616;
	goto tr1;
tr80:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr144:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr235:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr266:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr297:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr328:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr359:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr390:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr421:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr452:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr483:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr514:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr545:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr576:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr607:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr667:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr701:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr733:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr764:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr795:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr826:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr857:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr888:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr919:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr950:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr981:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1012:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1041:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1072:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1103:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1134:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1165:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1196:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1227:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1258:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1289:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1320:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1351:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1382:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1413:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1444:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1475:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1506:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1537:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1568:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1600:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1631:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1662:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1693:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1724:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1755:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1786:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1817:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1848:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 6336 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr624;
		case 10u: goto tr625;
		case 13u: goto tr626;
		case 32u: goto tr624;
		case 33u: goto tr627;
		case 34u: goto tr628;
		case 37u: goto tr629;
		case 38u: goto tr630;
		case 40u: goto tr631;
		case 41u: goto tr632;
		case 42u: goto st3;
		case 43u: goto tr633;
		case 44u: goto tr634;
		case 45u: goto tr635;
		case 46u: goto tr636;
		case 47u: goto st7;
		case 48u: goto tr637;
		case 58u: goto tr639;
		case 59u: goto tr640;
		case 60u: goto tr641;
		case 61u: goto st87;
		case 62u: goto tr643;
		case 63u: goto tr644;
		case 91u: goto tr646;
		case 93u: goto tr647;
		case 94u: goto tr648;
		case 95u: goto tr645;
		case 123u: goto tr649;
		case 124u: goto tr650;
		case 125u: goto tr651;
		case 126u: goto tr652;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr638;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr645;
	} else
		goto tr645;
	goto tr1;
tr9:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 6387 "xec_parser_ragel.cpp"
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
#line 6402 "xec_parser_ragel.cpp"
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
#line 6417 "xec_parser_ragel.cpp"
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
#line 6432 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st4;
		case 13u: goto st5;
		case 42u: goto st6;
		case 47u: goto st31;
	}
	goto st3;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 10u: goto st32;
		case 13u: goto st33;
	}
	goto st7;
tr81:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr114:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr145:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr174:
#line 174 "xec_parser_ragel.rl"
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
	goto st52;
tr205:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
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
	goto st52;
tr236:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr267:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr298:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr329:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr360:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr391:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr422:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr453:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr484:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr515:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr546:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr577:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr608:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr637:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr668:
#line 165 "xec_parser_ragel.rl"
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
	goto st52;
tr702:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr734:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr765:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr796:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr827:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr858:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr889:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr920:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr951:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr982:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1042:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1073:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1104:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1135:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1166:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1197:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1228:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1259:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1290:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1321:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1352:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1383:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1414:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1445:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1476:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1507:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1538:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1569:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1601:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1632:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1663:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1694:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1725:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1756:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1787:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1818:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
tr1849:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st52;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
#line 6924 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr666;
		case 47u: goto tr667;
		case 48u: goto tr668;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 69u: goto tr677;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 101u: goto tr677;
		case 120u: goto tr681;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr669;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr676;
	} else
		goto tr676;
	goto tr1;
tr666:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 6978 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr17;
	goto tr1;
tr17:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 6990 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr686;
		case 47u: goto tr667;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 69u: goto tr677;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 101u: goto tr677;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr17;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr676;
	} else
		goto tr676;
	goto tr1;
tr83:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr116:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr147:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr176:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr207:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr238:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr269:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr300:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr331:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr362:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr393:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr424:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr455:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr486:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr517:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr548:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr579:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr610:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr639:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr670:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr736:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr767:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr798:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr829:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr860:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr891:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr922:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr953:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr984:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1014:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1044:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1075:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1106:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1137:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1168:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1199:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1230:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1261:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1292:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1323:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1354:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1385:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1416:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1447:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1478:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1509:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1540:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1571:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1603:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1634:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1665:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1696:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1727:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1758:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1789:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1820:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1851:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
#line 7398 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr687;
		case 10u: goto tr688;
		case 13u: goto tr689;
		case 32u: goto tr687;
		case 33u: goto tr690;
		case 34u: goto tr691;
		case 37u: goto tr692;
		case 38u: goto tr693;
		case 40u: goto tr694;
		case 41u: goto tr695;
		case 42u: goto tr696;
		case 43u: goto tr697;
		case 44u: goto tr698;
		case 45u: goto tr699;
		case 46u: goto tr700;
		case 47u: goto tr701;
		case 48u: goto tr702;
		case 58u: goto st84;
		case 59u: goto tr705;
		case 60u: goto tr706;
		case 61u: goto tr707;
		case 62u: goto tr708;
		case 63u: goto tr709;
		case 91u: goto tr711;
		case 93u: goto tr712;
		case 94u: goto tr713;
		case 95u: goto tr710;
		case 123u: goto tr714;
		case 124u: goto tr715;
		case 125u: goto tr716;
		case 126u: goto tr717;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr703;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr710;
	} else
		goto tr710;
	goto tr1;
tr718:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr82:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr115:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr146:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr175:
#line 174 "xec_parser_ragel.rl"
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
	goto st55;
tr206:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
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
	goto st55;
tr237:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr268:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr299:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr330:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr361:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr392:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr423:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr454:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr485:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr516:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr547:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr578:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr609:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr638:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr669:
#line 165 "xec_parser_ragel.rl"
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
	goto st55;
tr703:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr735:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr766:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr797:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr828:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr859:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr890:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr921:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr952:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr983:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1043:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1074:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1105:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1136:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1167:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1198:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1229:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1260:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1291:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1322:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1353:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1384:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1415:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1446:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1477:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1508:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1539:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1570:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1602:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1633:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1664:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1695:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1726:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1757:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1788:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1819:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
tr1850:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st55;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
#line 7920 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr666;
		case 47u: goto tr667;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 69u: goto tr677;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 101u: goto tr677;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr718;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr676;
	} else
		goto tr676;
	goto tr1;
tr84:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr117:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr148:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr177:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr208:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr239:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr270:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr301:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr332:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr363:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr394:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr425:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr456:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr487:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr518:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr549:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr580:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr611:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr640:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr671:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr705:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr737:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr768:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr799:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr830:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr861:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr892:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr923:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr954:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr985:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1015:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1045:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1076:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1107:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1138:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1169:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1200:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1231:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1262:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1293:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1324:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1355:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1386:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1417:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1448:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1479:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1510:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1541:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1572:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1604:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1635:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1666:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1697:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1728:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1759:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1790:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1821:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1852:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
#line 8334 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr719;
		case 10u: goto tr720;
		case 13u: goto tr721;
		case 32u: goto tr719;
		case 33u: goto tr722;
		case 34u: goto tr723;
		case 37u: goto tr724;
		case 38u: goto tr725;
		case 40u: goto tr726;
		case 41u: goto tr727;
		case 42u: goto tr728;
		case 43u: goto tr729;
		case 44u: goto tr730;
		case 45u: goto tr731;
		case 46u: goto tr732;
		case 47u: goto tr733;
		case 48u: goto tr734;
		case 58u: goto tr736;
		case 59u: goto tr737;
		case 60u: goto tr738;
		case 61u: goto tr739;
		case 62u: goto tr740;
		case 63u: goto tr741;
		case 91u: goto tr743;
		case 93u: goto tr744;
		case 94u: goto tr745;
		case 95u: goto tr742;
		case 123u: goto tr746;
		case 124u: goto tr747;
		case 125u: goto tr748;
		case 126u: goto tr749;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr735;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr742;
	} else
		goto tr742;
	goto tr1;
tr85:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr118:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr149:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr178:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr209:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr240:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr271:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr302:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr333:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr364:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr395:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr426:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr457:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr488:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr519:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr550:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr581:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr612:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr641:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr672:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr706:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr738:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr800:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr831:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr862:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr893:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr924:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr955:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr986:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1016:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1046:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1077:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1108:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1139:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1170:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1201:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1232:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1263:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1294:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1325:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1356:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1387:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1418:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1449:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1480:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1511:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1542:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1573:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1605:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1636:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1667:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1698:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1729:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1760:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1791:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1822:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1853:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 8741 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr750;
		case 10u: goto tr751;
		case 13u: goto tr752;
		case 32u: goto tr750;
		case 33u: goto tr753;
		case 34u: goto tr754;
		case 37u: goto tr755;
		case 38u: goto tr756;
		case 40u: goto tr757;
		case 41u: goto tr758;
		case 42u: goto tr759;
		case 43u: goto tr760;
		case 44u: goto tr761;
		case 45u: goto tr762;
		case 46u: goto tr763;
		case 47u: goto tr764;
		case 48u: goto tr765;
		case 58u: goto tr767;
		case 59u: goto tr768;
		case 60u: goto st58;
		case 61u: goto st82;
		case 62u: goto tr771;
		case 63u: goto tr772;
		case 91u: goto tr774;
		case 93u: goto tr775;
		case 94u: goto tr776;
		case 95u: goto tr773;
		case 123u: goto tr777;
		case 124u: goto tr778;
		case 125u: goto tr779;
		case 126u: goto tr780;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr766;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr773;
	} else
		goto tr773;
	goto tr1;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 9u: goto tr781;
		case 10u: goto tr782;
		case 13u: goto tr783;
		case 32u: goto tr781;
		case 33u: goto tr784;
		case 34u: goto tr785;
		case 37u: goto tr786;
		case 38u: goto tr787;
		case 40u: goto tr788;
		case 41u: goto tr789;
		case 42u: goto tr790;
		case 43u: goto tr791;
		case 44u: goto tr792;
		case 45u: goto tr793;
		case 46u: goto tr794;
		case 47u: goto tr795;
		case 48u: goto tr796;
		case 58u: goto tr798;
		case 59u: goto tr799;
		case 60u: goto tr800;
		case 61u: goto st59;
		case 62u: goto tr802;
		case 63u: goto tr803;
		case 91u: goto tr805;
		case 93u: goto tr806;
		case 94u: goto tr807;
		case 95u: goto tr804;
		case 123u: goto tr808;
		case 124u: goto tr809;
		case 125u: goto tr810;
		case 126u: goto tr811;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr797;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr804;
	} else
		goto tr804;
	goto tr1;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	switch( (*p) ) {
		case 9u: goto tr812;
		case 10u: goto tr813;
		case 13u: goto tr814;
		case 32u: goto tr812;
		case 33u: goto tr815;
		case 34u: goto tr816;
		case 37u: goto tr817;
		case 38u: goto tr818;
		case 40u: goto tr819;
		case 41u: goto tr820;
		case 42u: goto tr821;
		case 43u: goto tr822;
		case 44u: goto tr823;
		case 45u: goto tr824;
		case 46u: goto tr825;
		case 47u: goto tr826;
		case 48u: goto tr827;
		case 58u: goto tr829;
		case 59u: goto tr830;
		case 60u: goto tr831;
		case 61u: goto tr832;
		case 62u: goto tr833;
		case 63u: goto tr834;
		case 91u: goto tr836;
		case 93u: goto tr837;
		case 94u: goto tr838;
		case 95u: goto tr835;
		case 123u: goto tr839;
		case 124u: goto tr840;
		case 125u: goto tr841;
		case 126u: goto tr842;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr828;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr835;
	} else
		goto tr835;
	goto tr1;
tr86:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr119:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr179:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr210:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr303:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr334:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr365:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr458:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr489:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr551:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr582:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr613:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr673:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr707:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr739:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr832:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr894:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr956:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr987:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1017:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1047:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1078:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1140:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1171:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1233:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1264:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1326:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1357:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1388:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1450:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1512:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1543:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1574:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1606:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1637:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1668:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1699:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1730:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1761:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1792:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1823:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
tr1854:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 9150 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr843;
		case 10u: goto tr844;
		case 13u: goto tr845;
		case 32u: goto tr843;
		case 33u: goto tr846;
		case 34u: goto tr847;
		case 37u: goto tr848;
		case 38u: goto tr849;
		case 40u: goto tr850;
		case 41u: goto tr851;
		case 42u: goto tr852;
		case 43u: goto tr853;
		case 44u: goto tr854;
		case 45u: goto tr855;
		case 46u: goto tr856;
		case 47u: goto tr857;
		case 48u: goto tr858;
		case 58u: goto tr860;
		case 59u: goto tr861;
		case 60u: goto tr862;
		case 61u: goto st61;
		case 62u: goto tr864;
		case 63u: goto tr865;
		case 91u: goto tr867;
		case 93u: goto tr868;
		case 94u: goto tr869;
		case 95u: goto tr866;
		case 123u: goto tr870;
		case 124u: goto tr871;
		case 125u: goto tr872;
		case 126u: goto tr873;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr859;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr866;
	} else
		goto tr866;
	goto tr1;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 9u: goto tr874;
		case 10u: goto tr875;
		case 13u: goto tr876;
		case 32u: goto tr874;
		case 33u: goto tr877;
		case 34u: goto tr878;
		case 37u: goto tr879;
		case 38u: goto tr880;
		case 40u: goto tr881;
		case 41u: goto tr882;
		case 42u: goto tr883;
		case 43u: goto tr884;
		case 44u: goto tr885;
		case 45u: goto tr886;
		case 46u: goto tr887;
		case 47u: goto tr888;
		case 48u: goto tr889;
		case 58u: goto tr891;
		case 59u: goto tr892;
		case 60u: goto tr893;
		case 61u: goto tr894;
		case 62u: goto tr895;
		case 63u: goto tr896;
		case 91u: goto tr898;
		case 93u: goto tr899;
		case 94u: goto tr900;
		case 95u: goto tr897;
		case 123u: goto tr901;
		case 124u: goto tr902;
		case 125u: goto tr903;
		case 126u: goto tr904;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr890;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr897;
	} else
		goto tr897;
	goto tr1;
tr87:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr120:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr151:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr180:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr211:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr242:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr273:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr304:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr335:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr366:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr397:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr428:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr459:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr490:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr521:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr552:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr583:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr614:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr643:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr674:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr708:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr740:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr771:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr802:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr833:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr864:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr895:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr957:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr988:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1018:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1048:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1079:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1110:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1141:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1172:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1203:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1234:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1265:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1296:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1327:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1358:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1389:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1451:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1482:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1513:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1544:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1575:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1607:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1638:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1669:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1700:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1731:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1762:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1793:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1824:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1855:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 9597 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr905;
		case 10u: goto tr906;
		case 13u: goto tr907;
		case 32u: goto tr905;
		case 33u: goto tr908;
		case 34u: goto tr909;
		case 37u: goto tr910;
		case 38u: goto tr911;
		case 40u: goto tr912;
		case 41u: goto tr913;
		case 42u: goto tr914;
		case 43u: goto tr915;
		case 44u: goto tr916;
		case 45u: goto tr917;
		case 46u: goto tr918;
		case 47u: goto tr919;
		case 48u: goto tr920;
		case 58u: goto tr922;
		case 59u: goto tr923;
		case 60u: goto tr924;
		case 61u: goto st63;
		case 62u: goto st78;
		case 63u: goto tr927;
		case 91u: goto tr929;
		case 93u: goto tr930;
		case 94u: goto tr931;
		case 95u: goto tr928;
		case 123u: goto tr932;
		case 124u: goto tr933;
		case 125u: goto tr934;
		case 126u: goto tr935;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr921;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr928;
	} else
		goto tr928;
	goto tr1;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 9u: goto tr936;
		case 10u: goto tr937;
		case 13u: goto tr938;
		case 32u: goto tr936;
		case 33u: goto tr939;
		case 34u: goto tr940;
		case 37u: goto tr941;
		case 38u: goto tr942;
		case 40u: goto tr943;
		case 41u: goto tr944;
		case 42u: goto tr945;
		case 43u: goto tr946;
		case 44u: goto tr947;
		case 45u: goto tr948;
		case 46u: goto tr949;
		case 47u: goto tr950;
		case 48u: goto tr951;
		case 58u: goto tr953;
		case 59u: goto tr954;
		case 60u: goto tr955;
		case 61u: goto tr956;
		case 62u: goto tr957;
		case 63u: goto tr958;
		case 91u: goto tr960;
		case 93u: goto tr961;
		case 94u: goto tr962;
		case 95u: goto tr959;
		case 123u: goto tr963;
		case 124u: goto tr964;
		case 125u: goto tr965;
		case 126u: goto tr966;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr952;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr959;
	} else
		goto tr959;
	goto tr1;
tr88:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr121:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr152:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr181:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr212:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr243:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr274:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr305:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr336:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr367:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr398:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr429:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr460:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr491:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr522:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr553:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr584:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr615:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr644:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr675:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr709:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr741:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr772:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr803:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr834:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr865:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr896:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr927:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr958:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr989:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1019:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1049:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1080:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1111:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1142:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1173:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1204:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1235:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1266:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1297:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1328:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1359:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1390:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1421:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1452:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1483:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1514:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1545:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1576:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1608:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1639:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1670:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1701:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1732:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1763:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1794:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1825:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1856:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
#line 10056 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr967;
		case 10u: goto tr968;
		case 13u: goto tr969;
		case 32u: goto tr967;
		case 33u: goto tr970;
		case 34u: goto tr971;
		case 37u: goto tr972;
		case 38u: goto tr973;
		case 40u: goto tr974;
		case 41u: goto tr975;
		case 42u: goto tr976;
		case 43u: goto tr977;
		case 44u: goto tr978;
		case 45u: goto tr979;
		case 46u: goto tr980;
		case 47u: goto tr981;
		case 48u: goto tr982;
		case 58u: goto tr984;
		case 59u: goto tr985;
		case 60u: goto tr986;
		case 61u: goto tr987;
		case 62u: goto tr988;
		case 63u: goto tr989;
		case 91u: goto tr991;
		case 93u: goto tr992;
		case 94u: goto tr993;
		case 95u: goto tr990;
		case 123u: goto tr994;
		case 124u: goto tr995;
		case 125u: goto tr996;
		case 126u: goto tr997;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr983;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr990;
	} else
		goto tr990;
	goto tr1;
tr1013:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr89:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr122:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr153:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr182:
#line 174 "xec_parser_ragel.rl"
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
	goto st65;
tr213:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
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
	goto st65;
tr244:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr275:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr306:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr337:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr368:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr399:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr430:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr461:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr492:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr523:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr554:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr585:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr616:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr645:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr676:
#line 165 "xec_parser_ragel.rl"
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
	goto st65;
tr710:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr742:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr773:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr804:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr835:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr866:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr897:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr928:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr959:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr990:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1050:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1081:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1112:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1143:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1174:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1205:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1236:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1267:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1298:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1329:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1360:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1391:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1422:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1453:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1484:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1515:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1546:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1577:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1609:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1640:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1671:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1702:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1733:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1764:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1795:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1826:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
tr1857:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 10578 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr998;
		case 10u: goto tr999;
		case 13u: goto tr1000;
		case 32u: goto tr998;
		case 33u: goto tr1001;
		case 34u: goto tr1002;
		case 37u: goto tr1003;
		case 38u: goto tr1004;
		case 40u: goto tr1005;
		case 41u: goto tr1006;
		case 42u: goto tr1007;
		case 43u: goto tr1008;
		case 44u: goto tr1009;
		case 45u: goto tr1010;
		case 46u: goto tr1011;
		case 47u: goto tr1012;
		case 58u: goto tr1014;
		case 59u: goto tr1015;
		case 60u: goto tr1016;
		case 61u: goto tr1017;
		case 62u: goto tr1018;
		case 63u: goto tr1019;
		case 91u: goto tr1020;
		case 93u: goto tr1021;
		case 94u: goto tr1022;
		case 95u: goto tr1013;
		case 123u: goto tr1023;
		case 124u: goto tr1024;
		case 125u: goto tr1025;
		case 126u: goto tr1026;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr1013;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1013;
	} else
		goto tr1013;
	goto tr1;
tr90:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr123:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr154:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr183:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr214:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr245:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr276:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr307:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr338:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr369:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr400:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr431:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr462:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr493:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr524:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr555:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr586:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr617:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr646:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr678:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr711:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr743:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr774:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr805:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr836:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr867:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr898:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr929:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr960:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr991:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1020:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1051:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1082:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1113:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1144:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1175:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1206:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1237:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1268:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1299:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1330:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1361:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1392:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1423:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1454:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1485:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1516:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1547:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1578:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1610:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1641:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1672:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1703:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1734:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1765:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1796:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1827:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
tr1858:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 10990 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1027;
		case 10u: goto tr1028;
		case 13u: goto tr1029;
		case 32u: goto tr1027;
		case 33u: goto tr1030;
		case 34u: goto tr1031;
		case 37u: goto tr1032;
		case 38u: goto tr1033;
		case 40u: goto tr1034;
		case 41u: goto tr1035;
		case 42u: goto tr1036;
		case 43u: goto tr1037;
		case 44u: goto tr1038;
		case 45u: goto tr1039;
		case 46u: goto tr1040;
		case 47u: goto tr1041;
		case 48u: goto tr1042;
		case 58u: goto tr1044;
		case 59u: goto tr1045;
		case 60u: goto tr1046;
		case 61u: goto tr1047;
		case 62u: goto tr1048;
		case 63u: goto tr1049;
		case 91u: goto tr1051;
		case 93u: goto tr1052;
		case 94u: goto tr1053;
		case 95u: goto tr1050;
		case 123u: goto tr1054;
		case 124u: goto tr1055;
		case 125u: goto tr1056;
		case 126u: goto tr1057;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1043;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1050;
	} else
		goto tr1050;
	goto tr1;
tr91:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr124:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr155:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr184:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr215:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr246:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr277:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr308:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr339:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr370:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr401:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr432:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr463:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr494:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr525:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr556:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr587:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr618:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr647:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr679:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr712:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr744:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr775:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr806:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr837:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr868:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr899:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr930:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr961:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr992:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1021:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1052:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1083:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1114:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1145:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1176:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1207:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1238:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1269:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1300:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1331:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1362:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1393:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1424:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1455:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1486:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1517:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1548:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1579:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1611:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1642:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1673:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1704:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1735:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1766:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1797:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1828:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
tr1859:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st67;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
#line 11403 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1058;
		case 10u: goto tr1059;
		case 13u: goto tr1060;
		case 32u: goto tr1058;
		case 33u: goto tr1061;
		case 34u: goto tr1062;
		case 37u: goto tr1063;
		case 38u: goto tr1064;
		case 40u: goto tr1065;
		case 41u: goto tr1066;
		case 42u: goto tr1067;
		case 43u: goto tr1068;
		case 44u: goto tr1069;
		case 45u: goto tr1070;
		case 46u: goto tr1071;
		case 47u: goto tr1072;
		case 48u: goto tr1073;
		case 58u: goto tr1075;
		case 59u: goto tr1076;
		case 60u: goto tr1077;
		case 61u: goto tr1078;
		case 62u: goto tr1079;
		case 63u: goto tr1080;
		case 91u: goto tr1082;
		case 93u: goto tr1083;
		case 94u: goto tr1084;
		case 95u: goto tr1081;
		case 123u: goto tr1085;
		case 124u: goto tr1086;
		case 125u: goto tr1087;
		case 126u: goto tr1088;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1074;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1081;
	} else
		goto tr1081;
	goto tr1;
tr92:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr125:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr156:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr185:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr216:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr247:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr278:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr309:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr340:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr371:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr402:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr433:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr464:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr495:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr526:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr557:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr588:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr619:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr648:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr680:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr713:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr745:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr776:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr807:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr838:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr869:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr900:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr931:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr962:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr993:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1022:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1053:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1084:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1146:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1177:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1208:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1239:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1270:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1301:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1332:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1363:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1394:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1425:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1456:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1487:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1518:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1549:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1580:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1612:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1643:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1674:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1705:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1736:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1767:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1798:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1829:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
tr1860:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st68;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
#line 11810 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1089;
		case 10u: goto tr1090;
		case 13u: goto tr1091;
		case 32u: goto tr1089;
		case 33u: goto tr1092;
		case 34u: goto tr1093;
		case 37u: goto tr1094;
		case 38u: goto tr1095;
		case 40u: goto tr1096;
		case 41u: goto tr1097;
		case 42u: goto tr1098;
		case 43u: goto tr1099;
		case 44u: goto tr1100;
		case 45u: goto tr1101;
		case 46u: goto tr1102;
		case 47u: goto tr1103;
		case 48u: goto tr1104;
		case 58u: goto tr1106;
		case 59u: goto tr1107;
		case 60u: goto tr1108;
		case 61u: goto st69;
		case 62u: goto tr1110;
		case 63u: goto tr1111;
		case 91u: goto tr1113;
		case 93u: goto tr1114;
		case 94u: goto st77;
		case 95u: goto tr1112;
		case 123u: goto tr1116;
		case 124u: goto tr1117;
		case 125u: goto tr1118;
		case 126u: goto tr1119;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1105;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1112;
	} else
		goto tr1112;
	goto tr1;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 9u: goto tr1120;
		case 10u: goto tr1121;
		case 13u: goto tr1122;
		case 32u: goto tr1120;
		case 33u: goto tr1123;
		case 34u: goto tr1124;
		case 37u: goto tr1125;
		case 38u: goto tr1126;
		case 40u: goto tr1127;
		case 41u: goto tr1128;
		case 42u: goto tr1129;
		case 43u: goto tr1130;
		case 44u: goto tr1131;
		case 45u: goto tr1132;
		case 46u: goto tr1133;
		case 47u: goto tr1134;
		case 48u: goto tr1135;
		case 58u: goto tr1137;
		case 59u: goto tr1138;
		case 60u: goto tr1139;
		case 61u: goto tr1140;
		case 62u: goto tr1141;
		case 63u: goto tr1142;
		case 91u: goto tr1144;
		case 93u: goto tr1145;
		case 94u: goto tr1146;
		case 95u: goto tr1143;
		case 123u: goto tr1147;
		case 124u: goto tr1148;
		case 125u: goto tr1149;
		case 126u: goto tr1150;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1136;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1143;
	} else
		goto tr1143;
	goto tr1;
tr93:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr126:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr158:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr186:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr217:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr248:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr279:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr310:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr341:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr372:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr403:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr434:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr465:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr496:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr527:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr558:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr589:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr620:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr649:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr682:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr714:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr746:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr777:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr808:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr839:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr870:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr901:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr932:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr963:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr994:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1023:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1054:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1085:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1116:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1147:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1178:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1209:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1240:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1271:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1302:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1333:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1364:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1395:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1426:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1457:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1488:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1519:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1550:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1581:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1613:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1644:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1675:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1706:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1737:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1768:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1799:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1830:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
tr1861:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st70;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
#line 12269 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1151;
		case 10u: goto tr1152;
		case 13u: goto tr1153;
		case 32u: goto tr1151;
		case 33u: goto tr1154;
		case 34u: goto tr1155;
		case 37u: goto tr1156;
		case 38u: goto tr1157;
		case 40u: goto tr1158;
		case 41u: goto tr1159;
		case 42u: goto tr1160;
		case 43u: goto tr1161;
		case 44u: goto tr1162;
		case 45u: goto tr1163;
		case 46u: goto tr1164;
		case 47u: goto tr1165;
		case 48u: goto tr1166;
		case 58u: goto tr1168;
		case 59u: goto tr1169;
		case 60u: goto tr1170;
		case 61u: goto tr1171;
		case 62u: goto tr1172;
		case 63u: goto tr1173;
		case 91u: goto tr1175;
		case 93u: goto tr1176;
		case 94u: goto tr1177;
		case 95u: goto tr1174;
		case 123u: goto tr1178;
		case 124u: goto tr1179;
		case 125u: goto tr1180;
		case 126u: goto tr1181;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1167;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1174;
	} else
		goto tr1174;
	goto tr1;
tr94:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr127:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr159:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr187:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr218:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr249:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr280:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr311:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr342:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr373:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr404:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr435:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr466:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr497:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr528:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr559:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr590:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr621:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr650:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr683:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr715:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr747:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr778:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr809:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr840:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr871:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr902:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr933:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr964:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr995:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1024:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1055:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1086:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1117:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1148:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1179:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1241:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1272:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1303:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1334:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1365:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1396:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1427:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1458:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1489:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1520:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1551:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1582:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1614:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1645:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1676:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1707:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1738:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1769:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1800:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1831:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
tr1862:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st71;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
#line 12676 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1182;
		case 10u: goto tr1183;
		case 13u: goto tr1184;
		case 32u: goto tr1182;
		case 33u: goto tr1185;
		case 34u: goto tr1186;
		case 37u: goto tr1187;
		case 38u: goto tr1188;
		case 40u: goto tr1189;
		case 41u: goto tr1190;
		case 42u: goto tr1191;
		case 43u: goto tr1192;
		case 44u: goto tr1193;
		case 45u: goto tr1194;
		case 46u: goto tr1195;
		case 47u: goto tr1196;
		case 48u: goto tr1197;
		case 58u: goto tr1199;
		case 59u: goto tr1200;
		case 60u: goto tr1201;
		case 61u: goto st72;
		case 62u: goto tr1203;
		case 63u: goto tr1204;
		case 91u: goto tr1206;
		case 93u: goto tr1207;
		case 94u: goto tr1208;
		case 95u: goto tr1205;
		case 123u: goto tr1209;
		case 124u: goto st76;
		case 125u: goto tr1211;
		case 126u: goto tr1212;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1198;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1205;
	} else
		goto tr1205;
	goto tr1;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 9u: goto tr1213;
		case 10u: goto tr1214;
		case 13u: goto tr1215;
		case 32u: goto tr1213;
		case 33u: goto tr1216;
		case 34u: goto tr1217;
		case 37u: goto tr1218;
		case 38u: goto tr1219;
		case 40u: goto tr1220;
		case 41u: goto tr1221;
		case 42u: goto tr1222;
		case 43u: goto tr1223;
		case 44u: goto tr1224;
		case 45u: goto tr1225;
		case 46u: goto tr1226;
		case 47u: goto tr1227;
		case 48u: goto tr1228;
		case 58u: goto tr1230;
		case 59u: goto tr1231;
		case 60u: goto tr1232;
		case 61u: goto tr1233;
		case 62u: goto tr1234;
		case 63u: goto tr1235;
		case 91u: goto tr1237;
		case 93u: goto tr1238;
		case 94u: goto tr1239;
		case 95u: goto tr1236;
		case 123u: goto tr1240;
		case 124u: goto tr1241;
		case 125u: goto tr1242;
		case 126u: goto tr1243;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1229;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1236;
	} else
		goto tr1236;
	goto tr1;
tr95:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr128:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr160:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr188:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr219:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr250:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr281:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr312:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr343:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr374:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr405:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr436:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr467:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr498:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr529:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr560:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr591:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr622:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr651:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr684:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr716:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr748:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr779:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr810:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr841:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr872:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr903:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr934:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr965:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr996:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1025:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1056:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1087:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1118:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1149:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1180:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1211:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1242:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1273:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1304:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1335:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1366:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1397:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1428:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1459:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1490:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1521:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1552:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1583:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1615:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1646:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1677:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1708:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1739:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1770:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1801:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1832:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
tr1863:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st73;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
#line 13135 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1244;
		case 10u: goto tr1245;
		case 13u: goto tr1246;
		case 32u: goto tr1244;
		case 33u: goto tr1247;
		case 34u: goto tr1248;
		case 37u: goto tr1249;
		case 38u: goto tr1250;
		case 40u: goto tr1251;
		case 41u: goto tr1252;
		case 42u: goto tr1253;
		case 43u: goto tr1254;
		case 44u: goto tr1255;
		case 45u: goto tr1256;
		case 46u: goto tr1257;
		case 47u: goto tr1258;
		case 48u: goto tr1259;
		case 58u: goto tr1261;
		case 59u: goto tr1262;
		case 60u: goto tr1263;
		case 61u: goto tr1264;
		case 62u: goto tr1265;
		case 63u: goto tr1266;
		case 91u: goto tr1268;
		case 93u: goto tr1269;
		case 94u: goto tr1270;
		case 95u: goto tr1267;
		case 123u: goto tr1271;
		case 124u: goto tr1272;
		case 125u: goto tr1273;
		case 126u: goto tr1274;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1260;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1267;
	} else
		goto tr1267;
	goto tr1;
tr96:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr129:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr161:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr189:
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr220:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr251:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr282:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr313:
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr344:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr375:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr406:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr437:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr468:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr499:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr530:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr561:
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr592:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr623:
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr652:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr685:
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr717:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr749:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr780:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr811:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr842:
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr873:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr904:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr935:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr966:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr997:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1026:
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1057:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1088:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1119:
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1150:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1181:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1212:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1243:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1274:
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1305:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1336:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1367:
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1398:
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1429:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1460:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1491:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1522:
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1553:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1584:
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1616:
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1647:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1678:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1709:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1740:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1771:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1802:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1833:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
tr1864:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 13548 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1275;
		case 10u: goto tr1276;
		case 13u: goto tr1277;
		case 32u: goto tr1275;
		case 33u: goto tr1278;
		case 34u: goto tr1279;
		case 37u: goto tr1280;
		case 38u: goto tr1281;
		case 40u: goto tr1282;
		case 41u: goto tr1283;
		case 42u: goto tr1284;
		case 43u: goto tr1285;
		case 44u: goto tr1286;
		case 45u: goto tr1287;
		case 46u: goto tr1288;
		case 47u: goto tr1289;
		case 48u: goto tr1290;
		case 58u: goto tr1292;
		case 59u: goto tr1293;
		case 60u: goto tr1294;
		case 61u: goto st75;
		case 62u: goto tr1296;
		case 63u: goto tr1297;
		case 91u: goto tr1299;
		case 93u: goto tr1300;
		case 94u: goto tr1301;
		case 95u: goto tr1298;
		case 123u: goto tr1302;
		case 124u: goto tr1303;
		case 125u: goto tr1304;
		case 126u: goto tr1305;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1291;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1298;
	} else
		goto tr1298;
	goto tr1;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	switch( (*p) ) {
		case 9u: goto tr1306;
		case 10u: goto tr1307;
		case 13u: goto tr1308;
		case 32u: goto tr1306;
		case 33u: goto tr1309;
		case 34u: goto tr1310;
		case 37u: goto tr1311;
		case 38u: goto tr1312;
		case 40u: goto tr1313;
		case 41u: goto tr1314;
		case 42u: goto tr1315;
		case 43u: goto tr1316;
		case 44u: goto tr1317;
		case 45u: goto tr1318;
		case 46u: goto tr1319;
		case 47u: goto tr1320;
		case 48u: goto tr1321;
		case 58u: goto tr1323;
		case 59u: goto tr1324;
		case 60u: goto tr1325;
		case 61u: goto tr1326;
		case 62u: goto tr1327;
		case 63u: goto tr1328;
		case 91u: goto tr1330;
		case 93u: goto tr1331;
		case 94u: goto tr1332;
		case 95u: goto tr1329;
		case 123u: goto tr1333;
		case 124u: goto tr1334;
		case 125u: goto tr1335;
		case 126u: goto tr1336;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1322;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1329;
	} else
		goto tr1329;
	goto tr1;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 9u: goto tr1337;
		case 10u: goto tr1338;
		case 13u: goto tr1339;
		case 32u: goto tr1337;
		case 33u: goto tr1340;
		case 34u: goto tr1341;
		case 37u: goto tr1342;
		case 38u: goto tr1343;
		case 40u: goto tr1344;
		case 41u: goto tr1345;
		case 42u: goto tr1346;
		case 43u: goto tr1347;
		case 44u: goto tr1348;
		case 45u: goto tr1349;
		case 46u: goto tr1350;
		case 47u: goto tr1351;
		case 48u: goto tr1352;
		case 58u: goto tr1354;
		case 59u: goto tr1355;
		case 60u: goto tr1356;
		case 61u: goto tr1357;
		case 62u: goto tr1358;
		case 63u: goto tr1359;
		case 91u: goto tr1361;
		case 93u: goto tr1362;
		case 94u: goto tr1363;
		case 95u: goto tr1360;
		case 123u: goto tr1364;
		case 124u: goto tr1365;
		case 125u: goto tr1366;
		case 126u: goto tr1367;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1353;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1360;
	} else
		goto tr1360;
	goto tr1;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	switch( (*p) ) {
		case 9u: goto tr1368;
		case 10u: goto tr1369;
		case 13u: goto tr1370;
		case 32u: goto tr1368;
		case 33u: goto tr1371;
		case 34u: goto tr1372;
		case 37u: goto tr1373;
		case 38u: goto tr1374;
		case 40u: goto tr1375;
		case 41u: goto tr1376;
		case 42u: goto tr1377;
		case 43u: goto tr1378;
		case 44u: goto tr1379;
		case 45u: goto tr1380;
		case 46u: goto tr1381;
		case 47u: goto tr1382;
		case 48u: goto tr1383;
		case 58u: goto tr1385;
		case 59u: goto tr1386;
		case 60u: goto tr1387;
		case 61u: goto tr1388;
		case 62u: goto tr1389;
		case 63u: goto tr1390;
		case 91u: goto tr1392;
		case 93u: goto tr1393;
		case 94u: goto tr1394;
		case 95u: goto tr1391;
		case 123u: goto tr1395;
		case 124u: goto tr1396;
		case 125u: goto tr1397;
		case 126u: goto tr1398;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1384;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1391;
	} else
		goto tr1391;
	goto tr1;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 9u: goto tr1399;
		case 10u: goto tr1400;
		case 13u: goto tr1401;
		case 32u: goto tr1399;
		case 33u: goto tr1402;
		case 34u: goto tr1403;
		case 37u: goto tr1404;
		case 38u: goto tr1405;
		case 40u: goto tr1406;
		case 41u: goto tr1407;
		case 42u: goto tr1408;
		case 43u: goto tr1409;
		case 44u: goto tr1410;
		case 45u: goto tr1411;
		case 46u: goto tr1412;
		case 47u: goto tr1413;
		case 48u: goto tr1414;
		case 58u: goto tr1416;
		case 59u: goto tr1417;
		case 60u: goto tr1418;
		case 61u: goto st79;
		case 62u: goto st80;
		case 63u: goto tr1421;
		case 91u: goto tr1423;
		case 93u: goto tr1424;
		case 94u: goto tr1425;
		case 95u: goto tr1422;
		case 123u: goto tr1426;
		case 124u: goto tr1427;
		case 125u: goto tr1428;
		case 126u: goto tr1429;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1415;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1422;
	} else
		goto tr1422;
	goto tr1;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 9u: goto tr1430;
		case 10u: goto tr1431;
		case 13u: goto tr1432;
		case 32u: goto tr1430;
		case 33u: goto tr1433;
		case 34u: goto tr1434;
		case 37u: goto tr1435;
		case 38u: goto tr1436;
		case 40u: goto tr1437;
		case 41u: goto tr1438;
		case 42u: goto tr1439;
		case 43u: goto tr1440;
		case 44u: goto tr1441;
		case 45u: goto tr1442;
		case 46u: goto tr1443;
		case 47u: goto tr1444;
		case 48u: goto tr1445;
		case 58u: goto tr1447;
		case 59u: goto tr1448;
		case 60u: goto tr1449;
		case 61u: goto tr1450;
		case 62u: goto tr1451;
		case 63u: goto tr1452;
		case 91u: goto tr1454;
		case 93u: goto tr1455;
		case 94u: goto tr1456;
		case 95u: goto tr1453;
		case 123u: goto tr1457;
		case 124u: goto tr1458;
		case 125u: goto tr1459;
		case 126u: goto tr1460;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1446;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1453;
	} else
		goto tr1453;
	goto tr1;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 9u: goto tr1461;
		case 10u: goto tr1462;
		case 13u: goto tr1463;
		case 32u: goto tr1461;
		case 33u: goto tr1464;
		case 34u: goto tr1465;
		case 37u: goto tr1466;
		case 38u: goto tr1467;
		case 40u: goto tr1468;
		case 41u: goto tr1469;
		case 42u: goto tr1470;
		case 43u: goto tr1471;
		case 44u: goto tr1472;
		case 45u: goto tr1473;
		case 46u: goto tr1474;
		case 47u: goto tr1475;
		case 48u: goto tr1476;
		case 58u: goto tr1478;
		case 59u: goto tr1479;
		case 60u: goto tr1480;
		case 61u: goto st81;
		case 62u: goto tr1482;
		case 63u: goto tr1483;
		case 91u: goto tr1485;
		case 93u: goto tr1486;
		case 94u: goto tr1487;
		case 95u: goto tr1484;
		case 123u: goto tr1488;
		case 124u: goto tr1489;
		case 125u: goto tr1490;
		case 126u: goto tr1491;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1477;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1484;
	} else
		goto tr1484;
	goto tr1;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	switch( (*p) ) {
		case 9u: goto tr1492;
		case 10u: goto tr1493;
		case 13u: goto tr1494;
		case 32u: goto tr1492;
		case 33u: goto tr1495;
		case 34u: goto tr1496;
		case 37u: goto tr1497;
		case 38u: goto tr1498;
		case 40u: goto tr1499;
		case 41u: goto tr1500;
		case 42u: goto tr1501;
		case 43u: goto tr1502;
		case 44u: goto tr1503;
		case 45u: goto tr1504;
		case 46u: goto tr1505;
		case 47u: goto tr1506;
		case 48u: goto tr1507;
		case 58u: goto tr1509;
		case 59u: goto tr1510;
		case 60u: goto tr1511;
		case 61u: goto tr1512;
		case 62u: goto tr1513;
		case 63u: goto tr1514;
		case 91u: goto tr1516;
		case 93u: goto tr1517;
		case 94u: goto tr1518;
		case 95u: goto tr1515;
		case 123u: goto tr1519;
		case 124u: goto tr1520;
		case 125u: goto tr1521;
		case 126u: goto tr1522;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1508;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1515;
	} else
		goto tr1515;
	goto tr1;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	switch( (*p) ) {
		case 9u: goto tr1523;
		case 10u: goto tr1524;
		case 13u: goto tr1525;
		case 32u: goto tr1523;
		case 33u: goto tr1526;
		case 34u: goto tr1527;
		case 37u: goto tr1528;
		case 38u: goto tr1529;
		case 40u: goto tr1530;
		case 41u: goto tr1531;
		case 42u: goto tr1532;
		case 43u: goto tr1533;
		case 44u: goto tr1534;
		case 45u: goto tr1535;
		case 46u: goto tr1536;
		case 47u: goto tr1537;
		case 48u: goto tr1538;
		case 58u: goto tr1540;
		case 59u: goto tr1541;
		case 60u: goto tr1542;
		case 61u: goto tr1543;
		case 62u: goto tr1544;
		case 63u: goto tr1545;
		case 91u: goto tr1547;
		case 93u: goto tr1548;
		case 94u: goto tr1549;
		case 95u: goto tr1546;
		case 123u: goto tr1550;
		case 124u: goto tr1551;
		case 125u: goto tr1552;
		case 126u: goto tr1553;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1539;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1546;
	} else
		goto tr1546;
	goto tr1;
tr677:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 13967 "xec_parser_ragel.cpp"
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
#line 13983 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr19;
	goto tr1;
tr19:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
#line 13995 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr686;
		case 47u: goto tr667;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr19;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr676;
	} else
		goto tr676;
	goto tr1;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	switch( (*p) ) {
		case 9u: goto tr1554;
		case 10u: goto tr1555;
		case 13u: goto tr1556;
		case 32u: goto tr1554;
		case 33u: goto tr1557;
		case 34u: goto tr1558;
		case 37u: goto tr1559;
		case 38u: goto tr1560;
		case 40u: goto tr1561;
		case 41u: goto tr1562;
		case 42u: goto tr1563;
		case 43u: goto tr1564;
		case 44u: goto tr1565;
		case 45u: goto tr1566;
		case 46u: goto tr1567;
		case 47u: goto tr1568;
		case 48u: goto tr1569;
		case 58u: goto tr1571;
		case 59u: goto tr1572;
		case 60u: goto tr1573;
		case 61u: goto tr1574;
		case 62u: goto tr1575;
		case 63u: goto tr1576;
		case 91u: goto tr1578;
		case 93u: goto tr1579;
		case 94u: goto tr1580;
		case 95u: goto tr1577;
		case 123u: goto tr1581;
		case 124u: goto tr1582;
		case 125u: goto tr1583;
		case 126u: goto tr1584;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1570;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1577;
	} else
		goto tr1577;
	goto tr1;
tr681:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 14091 "xec_parser_ragel.cpp"
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
	goto st85;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
#line 14109 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr1585;
		case 47u: goto tr667;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 80u: goto tr677;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 112u: goto tr677;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
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
				goto tr676;
		} else if ( (*p) >= 97u )
			goto tr20;
	} else
		goto tr676;
	goto tr1;
tr1585:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 14167 "xec_parser_ragel.cpp"
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
	goto st86;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 14185 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr653;
		case 10u: goto tr654;
		case 13u: goto tr655;
		case 32u: goto tr653;
		case 33u: goto tr656;
		case 34u: goto tr657;
		case 37u: goto tr658;
		case 38u: goto tr659;
		case 40u: goto tr660;
		case 41u: goto tr661;
		case 42u: goto tr662;
		case 43u: goto tr663;
		case 44u: goto tr664;
		case 45u: goto tr665;
		case 46u: goto tr686;
		case 47u: goto tr667;
		case 58u: goto tr670;
		case 59u: goto tr671;
		case 60u: goto tr672;
		case 61u: goto tr673;
		case 62u: goto tr674;
		case 63u: goto tr675;
		case 80u: goto tr677;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr676;
		case 112u: goto tr677;
		case 123u: goto tr682;
		case 124u: goto tr683;
		case 125u: goto tr684;
		case 126u: goto tr685;
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
				goto tr676;
		} else if ( (*p) >= 97u )
			goto tr21;
	} else
		goto tr676;
	goto tr1;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	switch( (*p) ) {
		case 9u: goto tr1586;
		case 10u: goto tr1587;
		case 13u: goto tr1588;
		case 32u: goto tr1586;
		case 33u: goto tr1589;
		case 34u: goto tr1590;
		case 37u: goto tr1591;
		case 38u: goto tr1592;
		case 40u: goto tr1593;
		case 41u: goto tr1594;
		case 42u: goto tr1595;
		case 43u: goto tr1596;
		case 44u: goto tr1597;
		case 45u: goto tr1598;
		case 46u: goto tr1599;
		case 47u: goto tr1600;
		case 48u: goto tr1601;
		case 58u: goto tr1603;
		case 59u: goto tr1604;
		case 60u: goto tr1605;
		case 61u: goto tr1606;
		case 62u: goto tr1607;
		case 63u: goto tr1608;
		case 91u: goto tr1610;
		case 93u: goto tr1611;
		case 94u: goto tr1612;
		case 95u: goto tr1609;
		case 123u: goto tr1613;
		case 124u: goto tr1614;
		case 125u: goto tr1615;
		case 126u: goto tr1616;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1602;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1609;
	} else
		goto tr1609;
	goto tr1;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	switch( (*p) ) {
		case 9u: goto tr1617;
		case 10u: goto tr1618;
		case 13u: goto tr1619;
		case 32u: goto tr1617;
		case 33u: goto tr1620;
		case 34u: goto tr1621;
		case 37u: goto tr1622;
		case 38u: goto tr1623;
		case 40u: goto tr1624;
		case 41u: goto tr1625;
		case 42u: goto tr1626;
		case 43u: goto tr1627;
		case 44u: goto tr1628;
		case 45u: goto tr1629;
		case 46u: goto tr1630;
		case 47u: goto tr1631;
		case 48u: goto tr1632;
		case 58u: goto tr1634;
		case 59u: goto tr1635;
		case 60u: goto tr1636;
		case 61u: goto tr1637;
		case 62u: goto tr1638;
		case 63u: goto tr1639;
		case 91u: goto tr1641;
		case 93u: goto tr1642;
		case 94u: goto tr1643;
		case 95u: goto tr1640;
		case 123u: goto tr1644;
		case 124u: goto tr1645;
		case 125u: goto tr1646;
		case 126u: goto tr1647;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1633;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1640;
	} else
		goto tr1640;
	goto tr1;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	switch( (*p) ) {
		case 9u: goto tr1648;
		case 10u: goto tr1649;
		case 13u: goto tr1650;
		case 32u: goto tr1648;
		case 33u: goto tr1651;
		case 34u: goto tr1652;
		case 37u: goto tr1653;
		case 38u: goto tr1654;
		case 40u: goto tr1655;
		case 41u: goto tr1656;
		case 42u: goto tr1657;
		case 43u: goto tr1658;
		case 44u: goto tr1659;
		case 45u: goto tr1660;
		case 46u: goto tr1661;
		case 47u: goto tr1662;
		case 48u: goto tr1663;
		case 58u: goto tr1665;
		case 59u: goto tr1666;
		case 60u: goto tr1667;
		case 61u: goto tr1668;
		case 62u: goto tr1669;
		case 63u: goto tr1670;
		case 91u: goto tr1672;
		case 93u: goto tr1673;
		case 94u: goto tr1674;
		case 95u: goto tr1671;
		case 123u: goto tr1675;
		case 124u: goto tr1676;
		case 125u: goto tr1677;
		case 126u: goto tr1678;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1664;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1671;
	} else
		goto tr1671;
	goto tr1;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	switch( (*p) ) {
		case 9u: goto tr1679;
		case 10u: goto tr1680;
		case 13u: goto tr1681;
		case 32u: goto tr1679;
		case 33u: goto tr1682;
		case 34u: goto tr1683;
		case 37u: goto tr1684;
		case 38u: goto tr1685;
		case 40u: goto tr1686;
		case 41u: goto tr1687;
		case 42u: goto tr1688;
		case 43u: goto tr1689;
		case 44u: goto tr1690;
		case 45u: goto tr1691;
		case 46u: goto tr1692;
		case 47u: goto tr1693;
		case 48u: goto tr1694;
		case 58u: goto tr1696;
		case 59u: goto tr1697;
		case 60u: goto tr1698;
		case 61u: goto tr1699;
		case 62u: goto tr1700;
		case 63u: goto tr1701;
		case 91u: goto tr1703;
		case 93u: goto tr1704;
		case 94u: goto tr1705;
		case 95u: goto tr1702;
		case 123u: goto tr1706;
		case 124u: goto tr1707;
		case 125u: goto tr1708;
		case 126u: goto tr1709;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1695;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1702;
	} else
		goto tr1702;
	goto tr1;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	switch( (*p) ) {
		case 9u: goto tr1710;
		case 10u: goto tr1711;
		case 13u: goto tr1712;
		case 32u: goto tr1710;
		case 33u: goto tr1713;
		case 34u: goto tr1714;
		case 37u: goto tr1715;
		case 38u: goto tr1716;
		case 40u: goto tr1717;
		case 41u: goto tr1718;
		case 42u: goto tr1719;
		case 43u: goto tr1720;
		case 44u: goto tr1721;
		case 45u: goto tr1722;
		case 46u: goto tr1723;
		case 47u: goto tr1724;
		case 48u: goto tr1725;
		case 58u: goto tr1727;
		case 59u: goto tr1728;
		case 60u: goto tr1729;
		case 61u: goto tr1730;
		case 62u: goto tr1731;
		case 63u: goto tr1732;
		case 91u: goto tr1734;
		case 93u: goto tr1735;
		case 94u: goto tr1736;
		case 95u: goto tr1733;
		case 123u: goto tr1737;
		case 124u: goto tr1738;
		case 125u: goto tr1739;
		case 126u: goto tr1740;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1726;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1733;
	} else
		goto tr1733;
	goto tr1;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	switch( (*p) ) {
		case 9u: goto tr1741;
		case 10u: goto tr1742;
		case 13u: goto tr1743;
		case 32u: goto tr1741;
		case 33u: goto tr1744;
		case 34u: goto tr1745;
		case 37u: goto tr1746;
		case 38u: goto tr1747;
		case 40u: goto tr1748;
		case 41u: goto tr1749;
		case 42u: goto tr1750;
		case 43u: goto tr1751;
		case 44u: goto tr1752;
		case 45u: goto tr1753;
		case 46u: goto tr1754;
		case 47u: goto tr1755;
		case 48u: goto tr1756;
		case 58u: goto tr1758;
		case 59u: goto tr1759;
		case 60u: goto tr1760;
		case 61u: goto tr1761;
		case 62u: goto tr1762;
		case 63u: goto tr1763;
		case 91u: goto tr1765;
		case 93u: goto tr1766;
		case 94u: goto tr1767;
		case 95u: goto tr1764;
		case 123u: goto tr1768;
		case 124u: goto tr1769;
		case 125u: goto tr1770;
		case 126u: goto tr1771;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1757;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1764;
	} else
		goto tr1764;
	goto tr1;
tr204:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 14519 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 42u: goto st14;
		case 47u: goto st18;
	}
	goto tr1;
tr27:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 14533 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st15;
		case 13u: goto st16;
		case 42u: goto st17;
	}
	goto st14;
tr28:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 14548 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr28;
		case 13u: goto tr29;
		case 42u: goto tr30;
	}
	goto tr27;
tr29:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 14563 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st15;
		case 13u: goto tr29;
		case 42u: goto tr30;
	}
	goto tr27;
tr30:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 14578 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st15;
		case 13u: goto st16;
		case 42u: goto st17;
		case 47u: goto st35;
	}
	goto st14;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	switch( (*p) ) {
		case 10u: goto st36;
		case 13u: goto st37;
	}
	goto st18;
tr3:
#line 97 "xec_parser_ragel.rl"
	{
                    uloc = (int)( offset + ( p - buffer ) );
                }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 14605 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 34u: goto tr33;
		case 47u: goto tr34;
		case 92u: goto tr35;
		case 98u: goto tr36;
		case 102u: goto tr37;
		case 110u: goto tr38;
		case 114u: goto tr39;
		case 116u: goto tr40;
		case 117u: goto st20;
		case 120u: goto st26;
	}
	goto tr1;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
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
#line 100 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 20; }
	goto st21;
tr44:
#line 102 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 20; }
	goto st21;
tr45:
#line 101 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 20; }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 14648 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr46;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr48;
	} else
		goto tr47;
	goto tr1;
tr46:
#line 104 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 16; }
	goto st22;
tr47:
#line 106 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 16; }
	goto st22;
tr48:
#line 105 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 16; }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 14674 "xec_parser_ragel.cpp"
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
#line 108 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 12; }
	goto st23;
tr50:
#line 110 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 12; }
	goto st23;
tr51:
#line 109 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 12; }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 14700 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr52;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr54;
	} else
		goto tr53;
	goto tr1;
tr52:
#line 112 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 8; }
	goto st24;
tr53:
#line 114 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 8; }
	goto st24;
tr54:
#line 113 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 8; }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 14726 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr55;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr57;
	} else
		goto tr56;
	goto tr1;
tr55:
#line 116 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - '0' ) << 4; }
	goto st25;
tr56:
#line 118 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'A' + 0x0A ) << 4; }
	goto st25;
tr57:
#line 117 "xec_parser_ragel.rl"
	{ temp |= ( (*p) - 'a' + 0x0A ) << 4; }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 14752 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr58;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr60;
	} else
		goto tr59;
	goto tr1;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr61;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr63;
	} else
		goto tr62;
	goto tr1;
tr61:
#line 87 "xec_parser_ragel.rl"
	{ temp = ( (*p) - '0' ) << 4; }
	goto st27;
tr62:
#line 89 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'A' + 0x0A ) << 4; }
	goto st27;
tr63:
#line 88 "xec_parser_ragel.rl"
	{ temp = ( (*p) - 'a' + 0x0A ) << 4; }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 14791 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr64;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr66;
	} else
		goto tr65;
	goto tr1;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	switch( (*p) ) {
		case 9u: goto tr1772;
		case 10u: goto tr1773;
		case 13u: goto tr1774;
		case 32u: goto tr1772;
		case 33u: goto tr1775;
		case 34u: goto tr1776;
		case 37u: goto tr1777;
		case 38u: goto tr1778;
		case 40u: goto tr1779;
		case 41u: goto tr1780;
		case 42u: goto tr1781;
		case 43u: goto tr1782;
		case 44u: goto tr1783;
		case 45u: goto tr1784;
		case 46u: goto tr1785;
		case 47u: goto tr1786;
		case 48u: goto tr1787;
		case 58u: goto tr1789;
		case 59u: goto tr1790;
		case 60u: goto tr1791;
		case 61u: goto tr1792;
		case 62u: goto tr1793;
		case 63u: goto tr1794;
		case 91u: goto tr1796;
		case 93u: goto tr1797;
		case 94u: goto tr1798;
		case 95u: goto tr1795;
		case 123u: goto tr1799;
		case 124u: goto tr1800;
		case 125u: goto tr1801;
		case 126u: goto tr1802;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1788;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1795;
	} else
		goto tr1795;
	goto tr1;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	switch( (*p) ) {
		case 110u: goto st94;
		case 115u: goto st95;
	}
	goto tr1;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	switch( (*p) ) {
		case 9u: goto tr1803;
		case 10u: goto tr1804;
		case 13u: goto tr1805;
		case 32u: goto tr1803;
		case 33u: goto tr1806;
		case 34u: goto tr1807;
		case 37u: goto tr1808;
		case 38u: goto tr1809;
		case 40u: goto tr1810;
		case 41u: goto tr1811;
		case 42u: goto tr1812;
		case 43u: goto tr1813;
		case 44u: goto tr1814;
		case 45u: goto tr1815;
		case 46u: goto tr1816;
		case 47u: goto tr1817;
		case 48u: goto tr1818;
		case 58u: goto tr1820;
		case 59u: goto tr1821;
		case 60u: goto tr1822;
		case 61u: goto tr1823;
		case 62u: goto tr1824;
		case 63u: goto tr1825;
		case 91u: goto tr1827;
		case 93u: goto tr1828;
		case 94u: goto tr1829;
		case 95u: goto tr1826;
		case 123u: goto tr1830;
		case 124u: goto tr1831;
		case 125u: goto tr1832;
		case 126u: goto tr1833;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1819;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1826;
	} else
		goto tr1826;
	goto tr1;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	switch( (*p) ) {
		case 9u: goto tr1834;
		case 10u: goto tr1835;
		case 13u: goto tr1836;
		case 32u: goto tr1834;
		case 33u: goto tr1837;
		case 34u: goto tr1838;
		case 37u: goto tr1839;
		case 38u: goto tr1840;
		case 40u: goto tr1841;
		case 41u: goto tr1842;
		case 42u: goto tr1843;
		case 43u: goto tr1844;
		case 44u: goto tr1845;
		case 45u: goto tr1846;
		case 46u: goto tr1847;
		case 47u: goto tr1848;
		case 48u: goto tr1849;
		case 58u: goto tr1851;
		case 59u: goto tr1852;
		case 60u: goto tr1853;
		case 61u: goto tr1854;
		case 62u: goto tr1855;
		case 63u: goto tr1856;
		case 91u: goto tr1858;
		case 93u: goto tr1859;
		case 94u: goto tr1860;
		case 95u: goto tr1857;
		case 123u: goto tr1861;
		case 124u: goto tr1862;
		case 125u: goto tr1863;
		case 126u: goto tr1864;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1850;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1857;
	} else
		goto tr1857;
	goto tr1;
tr113:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 14956 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 42u: goto st3;
		case 47u: goto st7;
	}
	goto tr1;
	}
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
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
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof88: cs = 88; goto _test_eof; 
	_test_eof89: cs = 89; goto _test_eof; 
	_test_eof90: cs = 90; goto _test_eof; 
	_test_eof91: cs = 91; goto _test_eof; 
	_test_eof92: cs = 92; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof93: cs = 93; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof94: cs = 94; goto _test_eof; 
	_test_eof95: cs = 95; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 32: 
	case 33: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	break;
	case 65: 
#line 158 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	break;
	case 52: 
	case 53: 
	case 55: 
	case 83: 
	case 85: 
	case 86: 
#line 165 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	break;
	case 35: 
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	break;
	case 34: 
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	break;
	case 38: 
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	break;
	case 39: 
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	break;
	case 41: 
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	break;
	case 42: 
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	break;
	case 43: 
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	break;
	case 44: 
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	break;
	case 46: 
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	break;
	case 47: 
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	break;
	case 49: 
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	break;
	case 51: 
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	break;
	case 54: 
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	break;
	case 56: 
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	break;
	case 57: 
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	break;
	case 60: 
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	break;
	case 62: 
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	break;
	case 64: 
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	break;
	case 66: 
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	break;
	case 67: 
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	break;
	case 68: 
#line 200 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	break;
	case 70: 
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	break;
	case 71: 
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	break;
	case 73: 
#line 203 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	break;
	case 74: 
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	break;
	case 45: 
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	break;
	case 48: 
#line 207 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	break;
	case 58: 
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	break;
	case 78: 
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	break;
	case 80: 
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	break;
	case 93: 
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	break;
	case 82: 
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	break;
	case 61: 
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	break;
	case 63: 
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	break;
	case 94: 
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIN, sloc ); }
	break;
	case 95: 
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTIS, sloc ); }
	break;
	case 92: 
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	break;
	case 91: 
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	break;
	case 90: 
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	break;
	case 89: 
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	break;
	case 88: 
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	break;
	case 87: 
#line 225 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	break;
	case 69: 
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	break;
	case 72: 
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	break;
	case 75: 
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	break;
	case 59: 
#line 229 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	break;
	case 79: 
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	break;
	case 81: 
#line 231 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	break;
	case 40: 
#line 233 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	break;
	case 77: 
#line 234 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	break;
	case 76: 
#line 235 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	break;
	case 84: 
#line 237 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EACHKEY, sloc ); }
	break;
	case 50: 
#line 238 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	break;
	case 36: 
	case 37: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 174 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
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
	case 17: 
	case 18: 
	case 19: 
	case 20: 
	case 21: 
	case 22: 
	case 23: 
	case 24: 
	case 25: 
	case 26: 
	case 27: 
	case 28: 
	case 29: 
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
#line 244 "xec_parser_ragel.rl"
	{ {goto st30;} }
	break;
	case 4: 
	case 5: 
	case 15: 
	case 16: 
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
#line 244 "xec_parser_ragel.rl"
	{ {goto st30;} }
	break;
#line 15391 "xec_parser_ragel.cpp"
	}
	}

	_out: {}
	}

#line 400 "xec_parser_ragel.rl"
        
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





