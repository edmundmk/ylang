
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





#line 238 "xec_parser_ragel.rl"




#line 31 "xec_parser_ragel.cpp"
static const int lexer_start = 23;
static const int lexer_first_final = 23;
static const int lexer_error = 0;

static const int lexer_en_main = 23;


#line 242 "xec_parser_ragel.rl"



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

#line 365 "xec_parser_ragel.rl"
    
    
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
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 9u: goto st24;
		case 10u: goto st25;
		case 13u: goto st26;
		case 32u: goto st24;
		case 33u: goto tr56;
		case 34u: goto tr57;
		case 37u: goto tr58;
		case 38u: goto tr59;
		case 40u: goto tr60;
		case 41u: goto tr61;
		case 42u: goto tr62;
		case 43u: goto tr63;
		case 44u: goto tr64;
		case 45u: goto tr65;
		case 46u: goto tr66;
		case 47u: goto tr67;
		case 48u: goto tr68;
		case 58u: goto tr70;
		case 59u: goto tr71;
		case 60u: goto tr72;
		case 61u: goto tr73;
		case 62u: goto tr74;
		case 63u: goto tr75;
		case 91u: goto tr77;
		case 93u: goto tr78;
		case 94u: goto tr79;
		case 95u: goto tr76;
		case 123u: goto tr80;
		case 124u: goto tr81;
		case 125u: goto tr82;
		case 126u: goto tr83;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr69;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr76;
	} else
		goto tr76;
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
#line 236 "xec_parser_ragel.rl"
	{ {goto st23;} }
	goto st0;
tr85:
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
#line 236 "xec_parser_ragel.rl"
	{ {goto st23;} }
	goto st0;
#line 280 "xec_parser_ragel.cpp"
st0:
cs = 0;
	goto _out;
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
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st24;
tr582:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st24;
tr611:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st24;
tr645:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st24;
tr677:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st24;
tr708:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st24;
tr739:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st24;
tr770:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st24;
tr801:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st24;
tr832:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st24;
tr863:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st24;
tr894:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st24;
tr925:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st24;
tr956:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st24;
tr985:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st24;
tr1016:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st24;
tr1047:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st24;
tr1078:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st24;
tr1109:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st24;
tr1140:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st24;
tr1171:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st24;
tr1202:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st24;
tr1233:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st24;
tr1264:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st24;
tr1295:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st24;
tr1326:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st24;
tr1357:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st24;
tr1388:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st24;
tr1419:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st24;
tr1450:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st24;
tr1481:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st24;
tr1513:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st24;
tr1544:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st24;
tr1575:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st24;
tr1606:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st24;
tr1637:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st24;
tr1668:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st24;
tr1699:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 513 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto st24;
		case 10u: goto st25;
		case 13u: goto st26;
		case 32u: goto st24;
		case 33u: goto tr56;
		case 34u: goto tr57;
		case 37u: goto tr58;
		case 38u: goto tr59;
		case 40u: goto tr60;
		case 41u: goto tr61;
		case 42u: goto tr62;
		case 43u: goto tr63;
		case 44u: goto tr64;
		case 45u: goto tr65;
		case 46u: goto tr66;
		case 47u: goto st22;
		case 48u: goto tr68;
		case 58u: goto tr70;
		case 59u: goto tr71;
		case 60u: goto tr72;
		case 61u: goto tr73;
		case 62u: goto tr74;
		case 63u: goto tr75;
		case 91u: goto tr77;
		case 93u: goto tr78;
		case 94u: goto tr79;
		case 95u: goto tr76;
		case 123u: goto tr80;
		case 124u: goto tr81;
		case 125u: goto tr82;
		case 126u: goto tr83;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr69;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr76;
	} else
		goto tr76;
	goto tr1;
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
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st25;
tr583:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st25;
tr612:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st25;
tr646:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st25;
tr678:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st25;
tr709:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st25;
tr740:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st25;
tr771:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st25;
tr802:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st25;
tr833:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st25;
tr864:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st25;
tr895:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st25;
tr926:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st25;
tr957:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st25;
tr986:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st25;
tr1017:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st25;
tr1048:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st25;
tr1079:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st25;
tr1110:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st25;
tr1141:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st25;
tr1172:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st25;
tr1203:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st25;
tr1234:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st25;
tr1265:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st25;
tr1296:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st25;
tr1327:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st25;
tr1358:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st25;
tr1389:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st25;
tr1420:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st25;
tr1451:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st25;
tr1482:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st25;
tr1514:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st25;
tr1545:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st25;
tr1576:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st25;
tr1607:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st25;
tr1638:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st25;
tr1669:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st25;
tr1700:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 785 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr86;
		case 10u: goto tr87;
		case 13u: goto tr88;
		case 32u: goto tr86;
		case 33u: goto tr89;
		case 34u: goto tr90;
		case 37u: goto tr91;
		case 38u: goto tr92;
		case 40u: goto tr93;
		case 41u: goto tr94;
		case 42u: goto tr95;
		case 43u: goto tr96;
		case 44u: goto tr97;
		case 45u: goto tr98;
		case 46u: goto tr99;
		case 47u: goto tr100;
		case 48u: goto tr101;
		case 58u: goto tr103;
		case 59u: goto tr104;
		case 60u: goto tr105;
		case 61u: goto tr106;
		case 62u: goto tr107;
		case 63u: goto tr108;
		case 91u: goto tr110;
		case 93u: goto tr111;
		case 94u: goto tr112;
		case 95u: goto tr109;
		case 123u: goto tr113;
		case 124u: goto tr114;
		case 125u: goto tr115;
		case 126u: goto tr116;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr102;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr109;
	} else
		goto tr109;
	goto tr85;
tr88:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st26;
tr119:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st26;
tr150:
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st26;
tr181:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st26;
tr212:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st26;
tr243:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st26;
tr274:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st26;
tr305:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st26;
tr336:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st26;
tr367:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st26;
tr398:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st26;
tr429:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	goto st26;
tr460:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st26;
tr491:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st26;
tr522:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st26;
tr553:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
	goto st26;
tr584:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st26;
tr613:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st26;
tr647:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st26;
tr679:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st26;
tr710:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st26;
tr741:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st26;
tr772:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st26;
tr803:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st26;
tr834:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st26;
tr865:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st26;
tr896:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st26;
tr927:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st26;
tr958:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st26;
tr987:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st26;
tr1018:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st26;
tr1049:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st26;
tr1080:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st26;
tr1111:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st26;
tr1142:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st26;
tr1173:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st26;
tr1204:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st26;
tr1235:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st26;
tr1266:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st26;
tr1297:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st26;
tr1328:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st26;
tr1359:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st26;
tr1390:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st26;
tr1421:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st26;
tr1452:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st26;
tr1483:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st26;
tr1515:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st26;
tr1546:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st26;
tr1577:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st26;
tr1608:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st26;
tr1639:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st26;
tr1670:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st26;
tr1701:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 1057 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr86;
		case 10u: goto st25;
		case 13u: goto tr88;
		case 32u: goto tr86;
		case 33u: goto tr89;
		case 34u: goto tr90;
		case 37u: goto tr91;
		case 38u: goto tr92;
		case 40u: goto tr93;
		case 41u: goto tr94;
		case 42u: goto tr95;
		case 43u: goto tr96;
		case 44u: goto tr97;
		case 45u: goto tr98;
		case 46u: goto tr99;
		case 47u: goto tr100;
		case 48u: goto tr101;
		case 58u: goto tr103;
		case 59u: goto tr104;
		case 60u: goto tr105;
		case 61u: goto tr106;
		case 62u: goto tr107;
		case 63u: goto tr108;
		case 91u: goto tr110;
		case 93u: goto tr111;
		case 94u: goto tr112;
		case 95u: goto tr109;
		case 123u: goto tr113;
		case 124u: goto tr114;
		case 125u: goto tr115;
		case 126u: goto tr116;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr102;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr109;
	} else
		goto tr109;
	goto tr85;
tr56:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr89:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr120:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
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
	goto st27;
tr182:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr213:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr244:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr275:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr306:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr337:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr368:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr399:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr430:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr461:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr492:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr523:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr554:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr585:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr614:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr648:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr680:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr711:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr742:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr773:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr804:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr835:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr866:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr897:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr928:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr959:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr988:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1019:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1050:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1081:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1112:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1143:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1174:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1205:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1236:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1267:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1298:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1329:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1360:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1391:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1422:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1453:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1484:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1516:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1547:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1578:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1609:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1640:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1671:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
tr1702:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 1439 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr117;
		case 10u: goto tr118;
		case 13u: goto tr119;
		case 32u: goto tr117;
		case 33u: goto tr120;
		case 34u: goto tr121;
		case 37u: goto tr122;
		case 38u: goto tr123;
		case 40u: goto tr124;
		case 41u: goto tr125;
		case 42u: goto tr126;
		case 43u: goto tr127;
		case 44u: goto tr128;
		case 45u: goto tr129;
		case 46u: goto tr130;
		case 47u: goto tr131;
		case 48u: goto tr132;
		case 58u: goto tr134;
		case 59u: goto tr135;
		case 60u: goto tr136;
		case 61u: goto st83;
		case 62u: goto tr138;
		case 63u: goto tr139;
		case 91u: goto tr141;
		case 93u: goto tr142;
		case 94u: goto tr143;
		case 95u: goto tr140;
		case 123u: goto tr144;
		case 124u: goto tr145;
		case 125u: goto tr146;
		case 126u: goto tr147;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr133;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr140;
	} else
		goto tr140;
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
tr57:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr90:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr121:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
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
	goto st1;
tr183:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr214:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr245:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr276:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr307:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr338:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr369:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr400:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr431:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr462:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr493:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr524:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr555:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr586:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr615:
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
tr649:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr681:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr712:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr743:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr774:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr805:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr836:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr867:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr898:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr929:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr960:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr989:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1020:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1051:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1082:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1113:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1144:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1175:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1206:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1237:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1268:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1299:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1330:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1361:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1392:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1423:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1454:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1485:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1517:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1548:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1579:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1610:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1641:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1672:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1703:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 1911 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto tr1;
		case 13u: goto tr1;
		case 34u: goto st28;
		case 92u: goto tr3;
	}
	goto tr0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	switch( (*p) ) {
		case 9u: goto tr148;
		case 10u: goto tr149;
		case 13u: goto tr150;
		case 32u: goto tr148;
		case 33u: goto tr151;
		case 34u: goto tr152;
		case 37u: goto tr153;
		case 38u: goto tr154;
		case 40u: goto tr155;
		case 41u: goto tr156;
		case 42u: goto tr157;
		case 43u: goto tr158;
		case 44u: goto tr159;
		case 45u: goto tr160;
		case 46u: goto tr161;
		case 47u: goto tr162;
		case 48u: goto tr163;
		case 58u: goto tr165;
		case 59u: goto tr166;
		case 60u: goto tr167;
		case 61u: goto tr168;
		case 62u: goto tr169;
		case 63u: goto tr170;
		case 91u: goto tr172;
		case 93u: goto tr173;
		case 94u: goto tr174;
		case 95u: goto tr171;
		case 123u: goto tr175;
		case 124u: goto tr176;
		case 125u: goto tr177;
		case 126u: goto tr178;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr164;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr171;
	} else
		goto tr171;
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
tr215:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
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
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr587:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr616:
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
tr650:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr682:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr713:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr744:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr775:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr806:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr837:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr868:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr899:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr930:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr961:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr990:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1021:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1052:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1083:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1114:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1145:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1176:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1207:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1238:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1269:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1300:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1331:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1362:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1393:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1424:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1455:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1486:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1518:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1549:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1580:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1611:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1642:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1673:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1704:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 2304 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr179;
		case 10u: goto tr180;
		case 13u: goto tr181;
		case 32u: goto tr179;
		case 33u: goto tr182;
		case 34u: goto tr183;
		case 37u: goto tr184;
		case 38u: goto tr185;
		case 40u: goto tr186;
		case 41u: goto tr187;
		case 42u: goto tr188;
		case 43u: goto tr189;
		case 44u: goto tr190;
		case 45u: goto tr191;
		case 46u: goto tr192;
		case 47u: goto tr193;
		case 48u: goto tr194;
		case 58u: goto tr196;
		case 59u: goto tr197;
		case 60u: goto tr198;
		case 61u: goto st82;
		case 62u: goto tr200;
		case 63u: goto tr201;
		case 91u: goto tr203;
		case 93u: goto tr204;
		case 94u: goto tr205;
		case 95u: goto tr202;
		case 123u: goto tr206;
		case 124u: goto tr207;
		case 125u: goto tr208;
		case 126u: goto tr209;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr195;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr202;
	} else
		goto tr202;
	goto tr1;
tr59:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr92:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr123:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
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
	goto st30;
tr185:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr247:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr278:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr309:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr340:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr371:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr402:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr433:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr464:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr495:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr526:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr557:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr588:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
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
	goto st30;
tr651:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr683:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr714:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr745:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr776:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr807:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr838:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr869:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr900:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr931:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr962:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr991:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1022:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1053:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1084:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1115:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1146:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1177:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1208:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1239:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1270:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1301:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1332:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1363:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1394:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1425:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1456:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1487:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1519:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1550:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1581:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1612:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1643:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1674:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
tr1705:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 2680 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr210;
		case 10u: goto tr211;
		case 13u: goto tr212;
		case 32u: goto tr210;
		case 33u: goto tr213;
		case 34u: goto tr214;
		case 37u: goto tr215;
		case 38u: goto st31;
		case 40u: goto tr217;
		case 41u: goto tr218;
		case 42u: goto tr219;
		case 43u: goto tr220;
		case 44u: goto tr221;
		case 45u: goto tr222;
		case 46u: goto tr223;
		case 47u: goto tr224;
		case 48u: goto tr225;
		case 58u: goto tr227;
		case 59u: goto tr228;
		case 60u: goto tr229;
		case 61u: goto st81;
		case 62u: goto tr231;
		case 63u: goto tr232;
		case 91u: goto tr234;
		case 93u: goto tr235;
		case 94u: goto tr236;
		case 95u: goto tr233;
		case 123u: goto tr237;
		case 124u: goto tr238;
		case 125u: goto tr239;
		case 126u: goto tr240;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr226;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr233;
	} else
		goto tr233;
	goto tr1;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	switch( (*p) ) {
		case 9u: goto tr241;
		case 10u: goto tr242;
		case 13u: goto tr243;
		case 32u: goto tr241;
		case 33u: goto tr244;
		case 34u: goto tr245;
		case 37u: goto tr246;
		case 38u: goto tr247;
		case 40u: goto tr248;
		case 41u: goto tr249;
		case 42u: goto tr250;
		case 43u: goto tr251;
		case 44u: goto tr252;
		case 45u: goto tr253;
		case 46u: goto tr254;
		case 47u: goto tr255;
		case 48u: goto tr256;
		case 58u: goto tr258;
		case 59u: goto tr259;
		case 60u: goto tr260;
		case 61u: goto tr261;
		case 62u: goto tr262;
		case 63u: goto tr263;
		case 91u: goto tr265;
		case 93u: goto tr266;
		case 94u: goto tr267;
		case 95u: goto tr264;
		case 123u: goto tr268;
		case 124u: goto tr269;
		case 125u: goto tr270;
		case 126u: goto tr271;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr257;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr264;
	} else
		goto tr264;
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
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr589:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
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
	goto st32;
tr652:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr684:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr715:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr746:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr777:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr808:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr839:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr870:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr901:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr932:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr963:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr992:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1023:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1054:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1085:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1116:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1147:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1178:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1209:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1240:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1271:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1302:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1333:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1364:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1395:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1426:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1457:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1488:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1520:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1551:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1582:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1613:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1644:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1675:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1706:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 3108 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr272;
		case 10u: goto tr273;
		case 13u: goto tr274;
		case 32u: goto tr272;
		case 33u: goto tr275;
		case 34u: goto tr276;
		case 37u: goto tr277;
		case 38u: goto tr278;
		case 40u: goto tr279;
		case 41u: goto tr280;
		case 42u: goto tr281;
		case 43u: goto tr282;
		case 44u: goto tr283;
		case 45u: goto tr284;
		case 46u: goto tr285;
		case 47u: goto tr286;
		case 48u: goto tr287;
		case 58u: goto tr289;
		case 59u: goto tr290;
		case 60u: goto tr291;
		case 61u: goto tr292;
		case 62u: goto tr293;
		case 63u: goto tr294;
		case 91u: goto tr296;
		case 93u: goto tr297;
		case 94u: goto tr298;
		case 95u: goto tr295;
		case 123u: goto tr299;
		case 124u: goto tr300;
		case 125u: goto tr301;
		case 126u: goto tr302;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr288;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr295;
	} else
		goto tr295;
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
tr559:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr590:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
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
	goto st33;
tr653:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr685:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr716:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr747:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr778:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr809:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr840:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr871:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr902:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr933:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr964:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr993:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1024:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1055:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1086:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1117:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1148:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1179:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1210:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1241:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1272:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1303:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1334:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1365:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1396:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1427:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1458:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1489:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1521:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1552:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1583:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1614:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1645:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1676:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1707:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 3490 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr303;
		case 10u: goto tr304;
		case 13u: goto tr305;
		case 32u: goto tr303;
		case 33u: goto tr306;
		case 34u: goto tr307;
		case 37u: goto tr308;
		case 38u: goto tr309;
		case 40u: goto tr310;
		case 41u: goto tr311;
		case 42u: goto tr312;
		case 43u: goto tr313;
		case 44u: goto tr314;
		case 45u: goto tr315;
		case 46u: goto tr316;
		case 47u: goto tr317;
		case 48u: goto tr318;
		case 58u: goto tr320;
		case 59u: goto tr321;
		case 60u: goto tr322;
		case 61u: goto tr323;
		case 62u: goto tr324;
		case 63u: goto tr325;
		case 91u: goto tr327;
		case 93u: goto tr328;
		case 94u: goto tr329;
		case 95u: goto tr326;
		case 123u: goto tr330;
		case 124u: goto tr331;
		case 125u: goto tr332;
		case 126u: goto tr333;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr319;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr326;
	} else
		goto tr326;
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
tr374:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
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
tr560:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
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
	goto st34;
tr654:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr686:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr717:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr748:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr779:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr810:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr841:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr872:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr903:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr934:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr965:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr994:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1025:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1056:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1087:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1118:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1149:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1180:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1211:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1242:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1273:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1304:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1335:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1366:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1397:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1428:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1459:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1490:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1522:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1553:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1584:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1615:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1646:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1677:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1708:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 3866 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr334;
		case 10u: goto tr335;
		case 13u: goto tr336;
		case 32u: goto tr334;
		case 33u: goto tr337;
		case 34u: goto tr338;
		case 37u: goto tr339;
		case 38u: goto tr340;
		case 40u: goto tr341;
		case 41u: goto tr342;
		case 42u: goto tr343;
		case 43u: goto tr344;
		case 44u: goto tr345;
		case 45u: goto tr346;
		case 46u: goto tr347;
		case 47u: goto tr348;
		case 48u: goto tr349;
		case 58u: goto tr351;
		case 59u: goto tr352;
		case 60u: goto tr353;
		case 61u: goto st80;
		case 62u: goto tr355;
		case 63u: goto tr356;
		case 91u: goto tr358;
		case 93u: goto tr359;
		case 94u: goto tr360;
		case 95u: goto tr357;
		case 123u: goto tr361;
		case 124u: goto tr362;
		case 125u: goto tr363;
		case 126u: goto tr364;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr350;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr357;
	} else
		goto tr357;
	goto tr1;
tr63:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr96:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr127:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
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
	goto st35;
tr189:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr220:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr251:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr282:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr313:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr344:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr406:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr437:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr468:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr499:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr530:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr561:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr591:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
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
	goto st35;
tr655:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr687:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr718:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr749:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr780:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr811:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr842:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr873:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr904:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr935:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr966:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr995:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1026:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1057:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1088:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1119:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1150:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1181:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1212:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1243:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1274:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1305:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1336:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1367:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1398:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1429:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1460:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1491:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1523:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1554:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1585:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1616:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1647:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1678:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
tr1709:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 4242 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr365;
		case 10u: goto tr366;
		case 13u: goto tr367;
		case 32u: goto tr365;
		case 33u: goto tr368;
		case 34u: goto tr369;
		case 37u: goto tr370;
		case 38u: goto tr371;
		case 40u: goto tr372;
		case 41u: goto tr373;
		case 42u: goto tr374;
		case 43u: goto st36;
		case 44u: goto tr376;
		case 45u: goto tr377;
		case 46u: goto tr378;
		case 47u: goto tr379;
		case 48u: goto tr380;
		case 58u: goto tr382;
		case 59u: goto tr383;
		case 60u: goto tr384;
		case 61u: goto st79;
		case 62u: goto tr386;
		case 63u: goto tr387;
		case 91u: goto tr389;
		case 93u: goto tr390;
		case 94u: goto tr391;
		case 95u: goto tr388;
		case 123u: goto tr392;
		case 124u: goto tr393;
		case 125u: goto tr394;
		case 126u: goto tr395;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr381;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr388;
	} else
		goto tr388;
	goto tr1;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	switch( (*p) ) {
		case 9u: goto tr396;
		case 10u: goto tr397;
		case 13u: goto tr398;
		case 32u: goto tr396;
		case 33u: goto tr399;
		case 34u: goto tr400;
		case 37u: goto tr401;
		case 38u: goto tr402;
		case 40u: goto tr403;
		case 41u: goto tr404;
		case 42u: goto tr405;
		case 43u: goto tr406;
		case 44u: goto tr407;
		case 45u: goto tr408;
		case 46u: goto tr409;
		case 47u: goto tr410;
		case 48u: goto tr411;
		case 58u: goto tr413;
		case 59u: goto tr414;
		case 60u: goto tr415;
		case 61u: goto tr416;
		case 62u: goto tr417;
		case 63u: goto tr418;
		case 91u: goto tr420;
		case 93u: goto tr421;
		case 94u: goto tr422;
		case 95u: goto tr419;
		case 123u: goto tr423;
		case 124u: goto tr424;
		case 125u: goto tr425;
		case 126u: goto tr426;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr412;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr419;
	} else
		goto tr419;
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
tr469:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
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
tr562:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr592:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
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
	goto st37;
tr656:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr688:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr719:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr750:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr781:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr812:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr843:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr874:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr905:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr936:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr967:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr996:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1027:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1058:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1089:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1120:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1151:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1182:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1213:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1244:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1275:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1306:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1337:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1368:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1399:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1430:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1461:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1492:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1524:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1555:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1586:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1617:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1648:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1679:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
tr1710:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 4670 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr427;
		case 10u: goto tr428;
		case 13u: goto tr429;
		case 32u: goto tr427;
		case 33u: goto tr430;
		case 34u: goto tr431;
		case 37u: goto tr432;
		case 38u: goto tr433;
		case 40u: goto tr434;
		case 41u: goto tr435;
		case 42u: goto tr436;
		case 43u: goto tr437;
		case 44u: goto tr438;
		case 45u: goto tr439;
		case 46u: goto tr440;
		case 47u: goto tr441;
		case 48u: goto tr442;
		case 58u: goto tr444;
		case 59u: goto tr445;
		case 60u: goto tr446;
		case 61u: goto tr447;
		case 62u: goto tr448;
		case 63u: goto tr449;
		case 91u: goto tr451;
		case 93u: goto tr452;
		case 94u: goto tr453;
		case 95u: goto tr450;
		case 123u: goto tr454;
		case 124u: goto tr455;
		case 125u: goto tr456;
		case 126u: goto tr457;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr443;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr450;
	} else
		goto tr450;
	goto tr1;
tr65:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr98:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr129:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
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
	goto st38;
tr191:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr222:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr253:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr284:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr315:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr346:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr377:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr408:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr439:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr501:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr532:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr563:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr593:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
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
	goto st38;
tr657:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr689:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr720:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr751:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr782:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr813:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr844:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr875:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr906:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr937:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr968:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr997:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1028:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1059:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1090:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1121:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1152:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1183:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1214:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1245:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1276:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1307:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1338:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1369:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1400:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1431:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1462:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1493:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1525:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1556:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1587:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1618:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1649:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1680:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1711:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 5046 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr458;
		case 10u: goto tr459;
		case 13u: goto tr460;
		case 32u: goto tr458;
		case 33u: goto tr461;
		case 34u: goto tr462;
		case 37u: goto tr463;
		case 38u: goto tr464;
		case 40u: goto tr465;
		case 41u: goto tr466;
		case 42u: goto tr467;
		case 43u: goto tr468;
		case 44u: goto tr469;
		case 45u: goto st39;
		case 46u: goto tr471;
		case 47u: goto tr472;
		case 48u: goto tr473;
		case 58u: goto tr475;
		case 59u: goto tr476;
		case 60u: goto tr477;
		case 61u: goto st78;
		case 62u: goto tr479;
		case 63u: goto tr480;
		case 91u: goto tr482;
		case 93u: goto tr483;
		case 94u: goto tr484;
		case 95u: goto tr481;
		case 123u: goto tr485;
		case 124u: goto tr486;
		case 125u: goto tr487;
		case 126u: goto tr488;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr474;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr481;
	} else
		goto tr481;
	goto tr1;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	switch( (*p) ) {
		case 9u: goto tr489;
		case 10u: goto tr490;
		case 13u: goto tr491;
		case 32u: goto tr489;
		case 33u: goto tr492;
		case 34u: goto tr493;
		case 37u: goto tr494;
		case 38u: goto tr495;
		case 40u: goto tr496;
		case 41u: goto tr497;
		case 42u: goto tr498;
		case 43u: goto tr499;
		case 44u: goto tr500;
		case 45u: goto tr501;
		case 46u: goto tr502;
		case 47u: goto tr503;
		case 48u: goto tr504;
		case 58u: goto tr506;
		case 59u: goto tr507;
		case 60u: goto tr508;
		case 61u: goto tr509;
		case 62u: goto tr510;
		case 63u: goto tr511;
		case 91u: goto tr513;
		case 93u: goto tr514;
		case 94u: goto tr515;
		case 95u: goto tr512;
		case 123u: goto tr516;
		case 124u: goto tr517;
		case 125u: goto tr518;
		case 126u: goto tr519;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr505;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr512;
	} else
		goto tr512;
	goto tr1;
tr66:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr99:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
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
tr564:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr594:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
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
	goto st40;
tr658:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr690:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr721:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr752:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr783:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr814:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr845:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr876:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr907:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr938:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr969:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr998:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1029:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1060:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1091:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1122:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1153:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1184:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1215:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1246:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1277:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1308:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1339:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1370:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1401:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1432:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1463:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1494:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1526:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1557:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1588:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1619:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1650:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1681:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
tr1712:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 5468 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr520;
		case 10u: goto tr521;
		case 13u: goto tr522;
		case 32u: goto tr520;
		case 33u: goto tr523;
		case 34u: goto tr524;
		case 37u: goto tr525;
		case 38u: goto tr526;
		case 40u: goto tr527;
		case 41u: goto tr528;
		case 42u: goto tr529;
		case 43u: goto tr530;
		case 44u: goto tr531;
		case 45u: goto tr532;
		case 46u: goto st2;
		case 47u: goto tr534;
		case 48u: goto tr535;
		case 58u: goto tr537;
		case 59u: goto tr538;
		case 60u: goto tr539;
		case 61u: goto tr540;
		case 62u: goto tr541;
		case 63u: goto tr542;
		case 91u: goto tr544;
		case 93u: goto tr545;
		case 94u: goto tr546;
		case 95u: goto tr543;
		case 123u: goto tr547;
		case 124u: goto tr548;
		case 125u: goto tr549;
		case 126u: goto tr550;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr536;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr543;
	} else
		goto tr543;
	goto tr1;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 46u )
		goto st41;
	goto tr1;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	switch( (*p) ) {
		case 9u: goto tr551;
		case 10u: goto tr552;
		case 13u: goto tr553;
		case 32u: goto tr551;
		case 33u: goto tr554;
		case 34u: goto tr555;
		case 37u: goto tr556;
		case 38u: goto tr557;
		case 40u: goto tr558;
		case 41u: goto tr559;
		case 42u: goto tr560;
		case 43u: goto tr561;
		case 44u: goto tr562;
		case 45u: goto tr563;
		case 46u: goto tr564;
		case 47u: goto tr565;
		case 48u: goto tr566;
		case 58u: goto tr568;
		case 59u: goto tr569;
		case 60u: goto tr570;
		case 61u: goto tr571;
		case 62u: goto tr572;
		case 63u: goto tr573;
		case 91u: goto tr575;
		case 93u: goto tr576;
		case 94u: goto tr577;
		case 95u: goto tr574;
		case 123u: goto tr578;
		case 124u: goto tr579;
		case 125u: goto tr580;
		case 126u: goto tr581;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr567;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr574;
	} else
		goto tr574;
	goto tr1;
tr67:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr131:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
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
	goto st42;
tr193:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr224:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr255:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr286:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr317:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr348:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr379:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr410:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr441:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr472:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr503:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr534:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr565:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
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
	goto st42;
tr659:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr691:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr722:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr753:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr784:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr815:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr846:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr877:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr908:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr939:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr970:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr999:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1030:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1061:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1092:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1123:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1154:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1185:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1216:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1247:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1278:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1309:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1340:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1371:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1402:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1433:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1464:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1495:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1527:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1558:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1589:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1620:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1651:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1682:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1713:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 5891 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr582;
		case 10u: goto tr583;
		case 13u: goto tr584;
		case 32u: goto tr582;
		case 33u: goto tr585;
		case 34u: goto tr586;
		case 37u: goto tr587;
		case 38u: goto tr588;
		case 40u: goto tr589;
		case 41u: goto tr590;
		case 42u: goto st3;
		case 43u: goto tr591;
		case 44u: goto tr592;
		case 45u: goto tr593;
		case 46u: goto tr594;
		case 47u: goto st7;
		case 48u: goto tr595;
		case 58u: goto tr597;
		case 59u: goto tr598;
		case 60u: goto tr599;
		case 61u: goto st77;
		case 62u: goto tr601;
		case 63u: goto tr602;
		case 91u: goto tr604;
		case 93u: goto tr605;
		case 94u: goto tr606;
		case 95u: goto tr603;
		case 123u: goto tr607;
		case 124u: goto tr608;
		case 125u: goto tr609;
		case 126u: goto tr610;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr596;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr603;
	} else
		goto tr603;
	goto tr1;
tr9:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 5942 "xec_parser_ragel.cpp"
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
#line 5957 "xec_parser_ragel.cpp"
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
#line 5972 "xec_parser_ragel.cpp"
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
#line 5987 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 10u: goto st4;
		case 13u: goto st5;
		case 42u: goto st6;
		case 47u: goto st24;
	}
	goto st3;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 10u: goto st25;
		case 13u: goto st26;
	}
	goto st7;
tr68:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr101:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr132:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
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
	goto st43;
tr194:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr225:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr256:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr287:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr318:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr349:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr380:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr411:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr442:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr473:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr504:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr535:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr566:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr595:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr660:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr692:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr723:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr754:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr785:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr816:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr847:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr878:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr909:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr940:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1000:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1031:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1062:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1093:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1124:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1155:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1186:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1217:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1248:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1279:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1310:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1341:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1372:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1403:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1434:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1465:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1496:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1528:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1559:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1590:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1621:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1652:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1683:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
tr1714:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 6440 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr624;
		case 47u: goto tr625;
		case 48u: goto tr626;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 69u: goto tr635;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 101u: goto tr635;
		case 120u: goto tr639;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr627;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr634;
	} else
		goto tr634;
	goto tr1;
tr624:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 6494 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr17;
	goto tr1;
tr17:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 6506 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr644;
		case 47u: goto tr625;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 69u: goto tr635;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 101u: goto tr635;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr17;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr634;
	} else
		goto tr634;
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
tr568:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr597:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
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
	goto st45;
tr662:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr694:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr725:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr756:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr787:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr818:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr849:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr880:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr911:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr942:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr972:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1002:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1033:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1064:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1095:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1126:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1157:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1188:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1219:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1250:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1281:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1312:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1343:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1374:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1405:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1436:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1467:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1498:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1530:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1561:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1592:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1623:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1654:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1685:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1716:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 6889 "xec_parser_ragel.cpp"
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
tr676:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr69:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr102:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr133:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr195:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr226:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr257:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr288:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr319:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr350:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr381:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr412:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr443:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr474:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr505:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr536:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr567:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr596:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr627:
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
	goto st46;
tr661:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr693:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr724:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr755:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr786:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr817:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr848:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr879:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr910:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr941:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1001:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1032:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1063:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1094:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1125:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1156:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1187:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1218:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1249:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1280:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1311:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1342:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1373:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1404:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1435:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1466:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1497:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1529:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1560:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1591:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1622:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1653:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1684:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
tr1715:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 7372 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr624;
		case 47u: goto tr625;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 69u: goto tr635;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 101u: goto tr635;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr676;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr634;
	} else
		goto tr634;
	goto tr1;
tr71:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr104:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr135:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
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
	goto st47;
tr197:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr228:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr259:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr290:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr321:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr352:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr383:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr414:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr445:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr476:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr507:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr538:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr569:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr598:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
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
	goto st47;
tr663:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr695:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr726:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr757:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr788:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr819:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr850:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr881:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr912:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr943:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr973:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1003:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1034:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1065:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1096:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1127:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1158:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1189:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1220:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1251:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1282:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1313:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1344:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1375:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1406:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1437:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1468:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1499:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1531:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1562:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1593:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1624:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1655:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1686:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
tr1717:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st47;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
#line 7755 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr677;
		case 10u: goto tr678;
		case 13u: goto tr679;
		case 32u: goto tr677;
		case 33u: goto tr680;
		case 34u: goto tr681;
		case 37u: goto tr682;
		case 38u: goto tr683;
		case 40u: goto tr684;
		case 41u: goto tr685;
		case 42u: goto tr686;
		case 43u: goto tr687;
		case 44u: goto tr688;
		case 45u: goto tr689;
		case 46u: goto tr690;
		case 47u: goto tr691;
		case 48u: goto tr692;
		case 58u: goto tr694;
		case 59u: goto tr695;
		case 60u: goto tr696;
		case 61u: goto tr697;
		case 62u: goto tr698;
		case 63u: goto tr699;
		case 91u: goto tr701;
		case 93u: goto tr702;
		case 94u: goto tr703;
		case 95u: goto tr700;
		case 123u: goto tr704;
		case 124u: goto tr705;
		case 125u: goto tr706;
		case 126u: goto tr707;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr693;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr700;
	} else
		goto tr700;
	goto tr1;
tr72:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr105:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr136:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
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
	goto st48;
tr198:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr229:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr260:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr291:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr322:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr353:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr384:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr415:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr446:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr477:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr508:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr539:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr570:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr599:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
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
	goto st48;
tr664:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr696:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr758:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr789:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr820:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr851:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr882:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr913:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr944:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr974:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1004:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1035:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1066:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1097:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1128:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1159:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1190:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1221:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1252:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1283:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1314:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1345:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1376:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1407:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1438:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1469:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1500:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1532:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1563:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1594:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1625:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1656:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1687:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1718:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
#line 8131 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr708;
		case 10u: goto tr709;
		case 13u: goto tr710;
		case 32u: goto tr708;
		case 33u: goto tr711;
		case 34u: goto tr712;
		case 37u: goto tr713;
		case 38u: goto tr714;
		case 40u: goto tr715;
		case 41u: goto tr716;
		case 42u: goto tr717;
		case 43u: goto tr718;
		case 44u: goto tr719;
		case 45u: goto tr720;
		case 46u: goto tr721;
		case 47u: goto tr722;
		case 48u: goto tr723;
		case 58u: goto tr725;
		case 59u: goto tr726;
		case 60u: goto st49;
		case 61u: goto st73;
		case 62u: goto tr729;
		case 63u: goto tr730;
		case 91u: goto tr732;
		case 93u: goto tr733;
		case 94u: goto tr734;
		case 95u: goto tr731;
		case 123u: goto tr735;
		case 124u: goto tr736;
		case 125u: goto tr737;
		case 126u: goto tr738;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr724;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr731;
	} else
		goto tr731;
	goto tr1;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	switch( (*p) ) {
		case 9u: goto tr739;
		case 10u: goto tr740;
		case 13u: goto tr741;
		case 32u: goto tr739;
		case 33u: goto tr742;
		case 34u: goto tr743;
		case 37u: goto tr744;
		case 38u: goto tr745;
		case 40u: goto tr746;
		case 41u: goto tr747;
		case 42u: goto tr748;
		case 43u: goto tr749;
		case 44u: goto tr750;
		case 45u: goto tr751;
		case 46u: goto tr752;
		case 47u: goto tr753;
		case 48u: goto tr754;
		case 58u: goto tr756;
		case 59u: goto tr757;
		case 60u: goto tr758;
		case 61u: goto st50;
		case 62u: goto tr760;
		case 63u: goto tr761;
		case 91u: goto tr763;
		case 93u: goto tr764;
		case 94u: goto tr765;
		case 95u: goto tr762;
		case 123u: goto tr766;
		case 124u: goto tr767;
		case 125u: goto tr768;
		case 126u: goto tr769;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr755;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr762;
	} else
		goto tr762;
	goto tr1;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9u: goto tr770;
		case 10u: goto tr771;
		case 13u: goto tr772;
		case 32u: goto tr770;
		case 33u: goto tr773;
		case 34u: goto tr774;
		case 37u: goto tr775;
		case 38u: goto tr776;
		case 40u: goto tr777;
		case 41u: goto tr778;
		case 42u: goto tr779;
		case 43u: goto tr780;
		case 44u: goto tr781;
		case 45u: goto tr782;
		case 46u: goto tr783;
		case 47u: goto tr784;
		case 48u: goto tr785;
		case 58u: goto tr787;
		case 59u: goto tr788;
		case 60u: goto tr789;
		case 61u: goto tr790;
		case 62u: goto tr791;
		case 63u: goto tr792;
		case 91u: goto tr794;
		case 93u: goto tr795;
		case 94u: goto tr796;
		case 95u: goto tr793;
		case 123u: goto tr797;
		case 124u: goto tr798;
		case 125u: goto tr799;
		case 126u: goto tr800;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr786;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr793;
	} else
		goto tr793;
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
tr571:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
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
	goto st51;
tr665:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr697:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr790:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr852:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr914:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr945:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr975:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1005:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1036:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1098:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1129:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1191:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1222:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1284:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1315:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1346:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1408:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1470:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1501:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1533:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1564:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1595:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1626:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1657:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1688:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
tr1719:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 8509 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr801;
		case 10u: goto tr802;
		case 13u: goto tr803;
		case 32u: goto tr801;
		case 33u: goto tr804;
		case 34u: goto tr805;
		case 37u: goto tr806;
		case 38u: goto tr807;
		case 40u: goto tr808;
		case 41u: goto tr809;
		case 42u: goto tr810;
		case 43u: goto tr811;
		case 44u: goto tr812;
		case 45u: goto tr813;
		case 46u: goto tr814;
		case 47u: goto tr815;
		case 48u: goto tr816;
		case 58u: goto tr818;
		case 59u: goto tr819;
		case 60u: goto tr820;
		case 61u: goto st52;
		case 62u: goto tr822;
		case 63u: goto tr823;
		case 91u: goto tr825;
		case 93u: goto tr826;
		case 94u: goto tr827;
		case 95u: goto tr824;
		case 123u: goto tr828;
		case 124u: goto tr829;
		case 125u: goto tr830;
		case 126u: goto tr831;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr817;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr824;
	} else
		goto tr824;
	goto tr1;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	switch( (*p) ) {
		case 9u: goto tr832;
		case 10u: goto tr833;
		case 13u: goto tr834;
		case 32u: goto tr832;
		case 33u: goto tr835;
		case 34u: goto tr836;
		case 37u: goto tr837;
		case 38u: goto tr838;
		case 40u: goto tr839;
		case 41u: goto tr840;
		case 42u: goto tr841;
		case 43u: goto tr842;
		case 44u: goto tr843;
		case 45u: goto tr844;
		case 46u: goto tr845;
		case 47u: goto tr846;
		case 48u: goto tr847;
		case 58u: goto tr849;
		case 59u: goto tr850;
		case 60u: goto tr851;
		case 61u: goto tr852;
		case 62u: goto tr853;
		case 63u: goto tr854;
		case 91u: goto tr856;
		case 93u: goto tr857;
		case 94u: goto tr858;
		case 95u: goto tr855;
		case 123u: goto tr859;
		case 124u: goto tr860;
		case 125u: goto tr861;
		case 126u: goto tr862;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr848;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr855;
	} else
		goto tr855;
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
tr572:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr601:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
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
	goto st53;
tr666:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr698:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr729:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr760:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr791:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr822:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr853:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr915:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr946:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr976:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1006:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1037:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1068:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1099:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1130:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1161:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1192:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1223:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1254:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1285:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1316:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1347:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1409:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1440:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1471:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1502:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1534:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1565:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1596:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1627:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1658:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1689:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
tr1720:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 8925 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr863;
		case 10u: goto tr864;
		case 13u: goto tr865;
		case 32u: goto tr863;
		case 33u: goto tr866;
		case 34u: goto tr867;
		case 37u: goto tr868;
		case 38u: goto tr869;
		case 40u: goto tr870;
		case 41u: goto tr871;
		case 42u: goto tr872;
		case 43u: goto tr873;
		case 44u: goto tr874;
		case 45u: goto tr875;
		case 46u: goto tr876;
		case 47u: goto tr877;
		case 48u: goto tr878;
		case 58u: goto tr880;
		case 59u: goto tr881;
		case 60u: goto tr882;
		case 61u: goto st54;
		case 62u: goto st69;
		case 63u: goto tr885;
		case 91u: goto tr887;
		case 93u: goto tr888;
		case 94u: goto tr889;
		case 95u: goto tr886;
		case 123u: goto tr890;
		case 124u: goto tr891;
		case 125u: goto tr892;
		case 126u: goto tr893;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr879;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr886;
	} else
		goto tr886;
	goto tr1;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 9u: goto tr894;
		case 10u: goto tr895;
		case 13u: goto tr896;
		case 32u: goto tr894;
		case 33u: goto tr897;
		case 34u: goto tr898;
		case 37u: goto tr899;
		case 38u: goto tr900;
		case 40u: goto tr901;
		case 41u: goto tr902;
		case 42u: goto tr903;
		case 43u: goto tr904;
		case 44u: goto tr905;
		case 45u: goto tr906;
		case 46u: goto tr907;
		case 47u: goto tr908;
		case 48u: goto tr909;
		case 58u: goto tr911;
		case 59u: goto tr912;
		case 60u: goto tr913;
		case 61u: goto tr914;
		case 62u: goto tr915;
		case 63u: goto tr916;
		case 91u: goto tr918;
		case 93u: goto tr919;
		case 94u: goto tr920;
		case 95u: goto tr917;
		case 123u: goto tr921;
		case 124u: goto tr922;
		case 125u: goto tr923;
		case 126u: goto tr924;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr910;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr917;
	} else
		goto tr917;
	goto tr1;
tr75:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr108:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr139:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
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
	goto st55;
tr201:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr232:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr263:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr294:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr325:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr356:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr387:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr418:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr449:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr480:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr511:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr542:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr573:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr602:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
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
	goto st55;
tr667:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr699:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr730:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr761:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr792:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr823:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr854:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr885:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr916:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr947:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr977:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1007:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1038:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1069:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1100:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1131:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1162:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1193:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1224:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1255:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1286:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1317:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1348:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1379:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1410:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1441:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1472:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1503:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1535:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1566:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1597:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1628:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1659:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1690:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1721:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
#line 9353 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr925;
		case 10u: goto tr926;
		case 13u: goto tr927;
		case 32u: goto tr925;
		case 33u: goto tr928;
		case 34u: goto tr929;
		case 37u: goto tr930;
		case 38u: goto tr931;
		case 40u: goto tr932;
		case 41u: goto tr933;
		case 42u: goto tr934;
		case 43u: goto tr935;
		case 44u: goto tr936;
		case 45u: goto tr937;
		case 46u: goto tr938;
		case 47u: goto tr939;
		case 48u: goto tr940;
		case 58u: goto tr942;
		case 59u: goto tr943;
		case 60u: goto tr944;
		case 61u: goto tr945;
		case 62u: goto tr946;
		case 63u: goto tr947;
		case 91u: goto tr949;
		case 93u: goto tr950;
		case 94u: goto tr951;
		case 95u: goto tr948;
		case 123u: goto tr952;
		case 124u: goto tr953;
		case 125u: goto tr954;
		case 126u: goto tr955;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr941;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr948;
	} else
		goto tr948;
	goto tr1;
tr971:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr76:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr109:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr140:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr202:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr233:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr264:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr295:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr326:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr357:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr388:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr419:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr450:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr481:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr512:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr543:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr574:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr603:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
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
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr668:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr700:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr731:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr762:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr793:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr824:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr855:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr886:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr917:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr948:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1008:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1039:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1070:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1101:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1132:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1163:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1194:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1225:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1256:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1287:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1318:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1349:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1380:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1411:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1442:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1473:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1504:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1536:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1567:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1598:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1629:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1660:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1691:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
tr1722:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st56;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
#line 9836 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr956;
		case 10u: goto tr957;
		case 13u: goto tr958;
		case 32u: goto tr956;
		case 33u: goto tr959;
		case 34u: goto tr960;
		case 37u: goto tr961;
		case 38u: goto tr962;
		case 40u: goto tr963;
		case 41u: goto tr964;
		case 42u: goto tr965;
		case 43u: goto tr966;
		case 44u: goto tr967;
		case 45u: goto tr968;
		case 46u: goto tr969;
		case 47u: goto tr970;
		case 58u: goto tr972;
		case 59u: goto tr973;
		case 60u: goto tr974;
		case 61u: goto tr975;
		case 62u: goto tr976;
		case 63u: goto tr977;
		case 91u: goto tr978;
		case 93u: goto tr979;
		case 94u: goto tr980;
		case 95u: goto tr971;
		case 123u: goto tr981;
		case 124u: goto tr982;
		case 125u: goto tr983;
		case 126u: goto tr984;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr971;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr971;
	} else
		goto tr971;
	goto tr1;
tr77:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr110:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr141:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
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
	goto st57;
tr203:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr234:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr265:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr296:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr327:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr358:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr389:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr420:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr451:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr482:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr513:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr544:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr575:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr604:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
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
	goto st57;
tr669:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr701:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr732:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr763:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr794:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr825:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr856:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr887:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr918:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr949:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr978:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1009:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1040:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1071:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1102:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1133:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1164:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1195:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1226:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1257:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1288:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1319:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1350:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1381:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1412:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1443:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1474:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1505:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1537:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1568:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1599:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1630:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1661:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1692:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
tr1723:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 10217 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr985;
		case 10u: goto tr986;
		case 13u: goto tr987;
		case 32u: goto tr985;
		case 33u: goto tr988;
		case 34u: goto tr989;
		case 37u: goto tr990;
		case 38u: goto tr991;
		case 40u: goto tr992;
		case 41u: goto tr993;
		case 42u: goto tr994;
		case 43u: goto tr995;
		case 44u: goto tr996;
		case 45u: goto tr997;
		case 46u: goto tr998;
		case 47u: goto tr999;
		case 48u: goto tr1000;
		case 58u: goto tr1002;
		case 59u: goto tr1003;
		case 60u: goto tr1004;
		case 61u: goto tr1005;
		case 62u: goto tr1006;
		case 63u: goto tr1007;
		case 91u: goto tr1009;
		case 93u: goto tr1010;
		case 94u: goto tr1011;
		case 95u: goto tr1008;
		case 123u: goto tr1012;
		case 124u: goto tr1013;
		case 125u: goto tr1014;
		case 126u: goto tr1015;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1001;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1008;
	} else
		goto tr1008;
	goto tr1;
tr78:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr111:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr142:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
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
	goto st58;
tr204:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr235:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr266:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr297:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr328:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr359:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr390:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr421:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr452:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr483:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr514:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr545:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr576:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr605:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
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
	goto st58;
tr670:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr702:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr733:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr764:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr795:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr826:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr857:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr888:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr919:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr950:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr979:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1010:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1041:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1072:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1103:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1134:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1165:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1196:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1227:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1258:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1289:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1320:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1351:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1382:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1413:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1444:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1475:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1506:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1538:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1569:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1600:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1631:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1662:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1693:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1724:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
#line 10599 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1016;
		case 10u: goto tr1017;
		case 13u: goto tr1018;
		case 32u: goto tr1016;
		case 33u: goto tr1019;
		case 34u: goto tr1020;
		case 37u: goto tr1021;
		case 38u: goto tr1022;
		case 40u: goto tr1023;
		case 41u: goto tr1024;
		case 42u: goto tr1025;
		case 43u: goto tr1026;
		case 44u: goto tr1027;
		case 45u: goto tr1028;
		case 46u: goto tr1029;
		case 47u: goto tr1030;
		case 48u: goto tr1031;
		case 58u: goto tr1033;
		case 59u: goto tr1034;
		case 60u: goto tr1035;
		case 61u: goto tr1036;
		case 62u: goto tr1037;
		case 63u: goto tr1038;
		case 91u: goto tr1040;
		case 93u: goto tr1041;
		case 94u: goto tr1042;
		case 95u: goto tr1039;
		case 123u: goto tr1043;
		case 124u: goto tr1044;
		case 125u: goto tr1045;
		case 126u: goto tr1046;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1032;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1039;
	} else
		goto tr1039;
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
tr577:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr606:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr638:
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
tr671:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr703:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr734:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr765:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr796:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr827:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr858:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr889:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr920:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr951:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr980:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1011:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1042:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1104:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1135:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1166:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1197:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1228:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1259:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1290:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1321:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1352:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1383:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1414:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1445:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1476:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1507:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1539:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1570:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1601:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1632:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1663:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1694:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1725:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 10975 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1047;
		case 10u: goto tr1048;
		case 13u: goto tr1049;
		case 32u: goto tr1047;
		case 33u: goto tr1050;
		case 34u: goto tr1051;
		case 37u: goto tr1052;
		case 38u: goto tr1053;
		case 40u: goto tr1054;
		case 41u: goto tr1055;
		case 42u: goto tr1056;
		case 43u: goto tr1057;
		case 44u: goto tr1058;
		case 45u: goto tr1059;
		case 46u: goto tr1060;
		case 47u: goto tr1061;
		case 48u: goto tr1062;
		case 58u: goto tr1064;
		case 59u: goto tr1065;
		case 60u: goto tr1066;
		case 61u: goto st60;
		case 62u: goto tr1068;
		case 63u: goto tr1069;
		case 91u: goto tr1071;
		case 93u: goto tr1072;
		case 94u: goto st68;
		case 95u: goto tr1070;
		case 123u: goto tr1074;
		case 124u: goto tr1075;
		case 125u: goto tr1076;
		case 126u: goto tr1077;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1063;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1070;
	} else
		goto tr1070;
	goto tr1;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	switch( (*p) ) {
		case 9u: goto tr1078;
		case 10u: goto tr1079;
		case 13u: goto tr1080;
		case 32u: goto tr1078;
		case 33u: goto tr1081;
		case 34u: goto tr1082;
		case 37u: goto tr1083;
		case 38u: goto tr1084;
		case 40u: goto tr1085;
		case 41u: goto tr1086;
		case 42u: goto tr1087;
		case 43u: goto tr1088;
		case 44u: goto tr1089;
		case 45u: goto tr1090;
		case 46u: goto tr1091;
		case 47u: goto tr1092;
		case 48u: goto tr1093;
		case 58u: goto tr1095;
		case 59u: goto tr1096;
		case 60u: goto tr1097;
		case 61u: goto tr1098;
		case 62u: goto tr1099;
		case 63u: goto tr1100;
		case 91u: goto tr1102;
		case 93u: goto tr1103;
		case 94u: goto tr1104;
		case 95u: goto tr1101;
		case 123u: goto tr1105;
		case 124u: goto tr1106;
		case 125u: goto tr1107;
		case 126u: goto tr1108;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1094;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1101;
	} else
		goto tr1101;
	goto tr1;
tr80:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr113:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr144:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
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
	goto st61;
tr206:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr237:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr268:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr299:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr330:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr361:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr392:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr423:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr454:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr485:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr516:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr547:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr578:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr607:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
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
	goto st61;
tr672:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr704:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr735:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr766:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr797:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr828:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr859:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr890:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr921:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr952:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr981:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1012:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1043:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1074:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1105:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1136:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1167:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1198:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1229:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1260:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1291:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1322:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1353:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1384:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1415:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1446:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1477:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1508:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1540:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1571:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1602:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1633:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1664:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1695:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1726:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
#line 11403 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1109;
		case 10u: goto tr1110;
		case 13u: goto tr1111;
		case 32u: goto tr1109;
		case 33u: goto tr1112;
		case 34u: goto tr1113;
		case 37u: goto tr1114;
		case 38u: goto tr1115;
		case 40u: goto tr1116;
		case 41u: goto tr1117;
		case 42u: goto tr1118;
		case 43u: goto tr1119;
		case 44u: goto tr1120;
		case 45u: goto tr1121;
		case 46u: goto tr1122;
		case 47u: goto tr1123;
		case 48u: goto tr1124;
		case 58u: goto tr1126;
		case 59u: goto tr1127;
		case 60u: goto tr1128;
		case 61u: goto tr1129;
		case 62u: goto tr1130;
		case 63u: goto tr1131;
		case 91u: goto tr1133;
		case 93u: goto tr1134;
		case 94u: goto tr1135;
		case 95u: goto tr1132;
		case 123u: goto tr1136;
		case 124u: goto tr1137;
		case 125u: goto tr1138;
		case 126u: goto tr1139;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1125;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1132;
	} else
		goto tr1132;
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
tr579:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr608:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
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
	goto st62;
tr673:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr705:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr736:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr767:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr798:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr829:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr860:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr891:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr922:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr953:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr982:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1013:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1044:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1075:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1106:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1137:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1199:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1230:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1261:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1292:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1323:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1354:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1385:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1416:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1447:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1478:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1509:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1541:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1572:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1603:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1634:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1665:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1696:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1727:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 11779 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1140;
		case 10u: goto tr1141;
		case 13u: goto tr1142;
		case 32u: goto tr1140;
		case 33u: goto tr1143;
		case 34u: goto tr1144;
		case 37u: goto tr1145;
		case 38u: goto tr1146;
		case 40u: goto tr1147;
		case 41u: goto tr1148;
		case 42u: goto tr1149;
		case 43u: goto tr1150;
		case 44u: goto tr1151;
		case 45u: goto tr1152;
		case 46u: goto tr1153;
		case 47u: goto tr1154;
		case 48u: goto tr1155;
		case 58u: goto tr1157;
		case 59u: goto tr1158;
		case 60u: goto tr1159;
		case 61u: goto st63;
		case 62u: goto tr1161;
		case 63u: goto tr1162;
		case 91u: goto tr1164;
		case 93u: goto tr1165;
		case 94u: goto tr1166;
		case 95u: goto tr1163;
		case 123u: goto tr1167;
		case 124u: goto st67;
		case 125u: goto tr1169;
		case 126u: goto tr1170;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1156;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1163;
	} else
		goto tr1163;
	goto tr1;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 9u: goto tr1171;
		case 10u: goto tr1172;
		case 13u: goto tr1173;
		case 32u: goto tr1171;
		case 33u: goto tr1174;
		case 34u: goto tr1175;
		case 37u: goto tr1176;
		case 38u: goto tr1177;
		case 40u: goto tr1178;
		case 41u: goto tr1179;
		case 42u: goto tr1180;
		case 43u: goto tr1181;
		case 44u: goto tr1182;
		case 45u: goto tr1183;
		case 46u: goto tr1184;
		case 47u: goto tr1185;
		case 48u: goto tr1186;
		case 58u: goto tr1188;
		case 59u: goto tr1189;
		case 60u: goto tr1190;
		case 61u: goto tr1191;
		case 62u: goto tr1192;
		case 63u: goto tr1193;
		case 91u: goto tr1195;
		case 93u: goto tr1196;
		case 94u: goto tr1197;
		case 95u: goto tr1194;
		case 123u: goto tr1198;
		case 124u: goto tr1199;
		case 125u: goto tr1200;
		case 126u: goto tr1201;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1187;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1194;
	} else
		goto tr1194;
	goto tr1;
tr82:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr115:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr146:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
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
	goto st64;
tr208:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr239:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr270:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr301:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr332:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr363:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr394:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr425:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr456:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr487:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr518:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr549:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr580:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr609:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr642:
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr674:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr706:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr737:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr768:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr799:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr830:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr861:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr892:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr923:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr954:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr983:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1014:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1045:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1076:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1107:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1138:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1169:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1200:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1231:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1262:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1293:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1324:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1355:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1386:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1417:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1448:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1479:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1510:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1542:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1573:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1604:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1635:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1666:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1697:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
tr1728:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st64;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
#line 12207 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1202;
		case 10u: goto tr1203;
		case 13u: goto tr1204;
		case 32u: goto tr1202;
		case 33u: goto tr1205;
		case 34u: goto tr1206;
		case 37u: goto tr1207;
		case 38u: goto tr1208;
		case 40u: goto tr1209;
		case 41u: goto tr1210;
		case 42u: goto tr1211;
		case 43u: goto tr1212;
		case 44u: goto tr1213;
		case 45u: goto tr1214;
		case 46u: goto tr1215;
		case 47u: goto tr1216;
		case 48u: goto tr1217;
		case 58u: goto tr1219;
		case 59u: goto tr1220;
		case 60u: goto tr1221;
		case 61u: goto tr1222;
		case 62u: goto tr1223;
		case 63u: goto tr1224;
		case 91u: goto tr1226;
		case 93u: goto tr1227;
		case 94u: goto tr1228;
		case 95u: goto tr1225;
		case 123u: goto tr1229;
		case 124u: goto tr1230;
		case 125u: goto tr1231;
		case 126u: goto tr1232;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1218;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1225;
	} else
		goto tr1225;
	goto tr1;
tr83:
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr116:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr147:
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
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
	goto st65;
tr209:
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr240:
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr271:
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr302:
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr333:
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr364:
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr395:
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr426:
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr457:
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr488:
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr519:
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr550:
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr581:
#line 230 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ELLIPSIS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr610:
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
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
	goto st65;
tr675:
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr707:
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr738:
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr769:
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr800:
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr831:
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr862:
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr893:
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr924:
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr955:
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr984:
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1015:
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1046:
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1077:
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1108:
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1139:
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1170:
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1201:
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1232:
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1263:
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1294:
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1325:
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1356:
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1387:
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1418:
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1449:
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1480:
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1511:
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1543:
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1574:
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1605:
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1636:
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1667:
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1698:
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
tr1729:
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
#line 30 "xec_parser_ragel.rl"
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 12589 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr1233;
		case 10u: goto tr1234;
		case 13u: goto tr1235;
		case 32u: goto tr1233;
		case 33u: goto tr1236;
		case 34u: goto tr1237;
		case 37u: goto tr1238;
		case 38u: goto tr1239;
		case 40u: goto tr1240;
		case 41u: goto tr1241;
		case 42u: goto tr1242;
		case 43u: goto tr1243;
		case 44u: goto tr1244;
		case 45u: goto tr1245;
		case 46u: goto tr1246;
		case 47u: goto tr1247;
		case 48u: goto tr1248;
		case 58u: goto tr1250;
		case 59u: goto tr1251;
		case 60u: goto tr1252;
		case 61u: goto st66;
		case 62u: goto tr1254;
		case 63u: goto tr1255;
		case 91u: goto tr1257;
		case 93u: goto tr1258;
		case 94u: goto tr1259;
		case 95u: goto tr1256;
		case 123u: goto tr1260;
		case 124u: goto tr1261;
		case 125u: goto tr1262;
		case 126u: goto tr1263;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1249;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1256;
	} else
		goto tr1256;
	goto tr1;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 9u: goto tr1264;
		case 10u: goto tr1265;
		case 13u: goto tr1266;
		case 32u: goto tr1264;
		case 33u: goto tr1267;
		case 34u: goto tr1268;
		case 37u: goto tr1269;
		case 38u: goto tr1270;
		case 40u: goto tr1271;
		case 41u: goto tr1272;
		case 42u: goto tr1273;
		case 43u: goto tr1274;
		case 44u: goto tr1275;
		case 45u: goto tr1276;
		case 46u: goto tr1277;
		case 47u: goto tr1278;
		case 48u: goto tr1279;
		case 58u: goto tr1281;
		case 59u: goto tr1282;
		case 60u: goto tr1283;
		case 61u: goto tr1284;
		case 62u: goto tr1285;
		case 63u: goto tr1286;
		case 91u: goto tr1288;
		case 93u: goto tr1289;
		case 94u: goto tr1290;
		case 95u: goto tr1287;
		case 123u: goto tr1291;
		case 124u: goto tr1292;
		case 125u: goto tr1293;
		case 126u: goto tr1294;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1280;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1287;
	} else
		goto tr1287;
	goto tr1;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 9u: goto tr1295;
		case 10u: goto tr1296;
		case 13u: goto tr1297;
		case 32u: goto tr1295;
		case 33u: goto tr1298;
		case 34u: goto tr1299;
		case 37u: goto tr1300;
		case 38u: goto tr1301;
		case 40u: goto tr1302;
		case 41u: goto tr1303;
		case 42u: goto tr1304;
		case 43u: goto tr1305;
		case 44u: goto tr1306;
		case 45u: goto tr1307;
		case 46u: goto tr1308;
		case 47u: goto tr1309;
		case 48u: goto tr1310;
		case 58u: goto tr1312;
		case 59u: goto tr1313;
		case 60u: goto tr1314;
		case 61u: goto tr1315;
		case 62u: goto tr1316;
		case 63u: goto tr1317;
		case 91u: goto tr1319;
		case 93u: goto tr1320;
		case 94u: goto tr1321;
		case 95u: goto tr1318;
		case 123u: goto tr1322;
		case 124u: goto tr1323;
		case 125u: goto tr1324;
		case 126u: goto tr1325;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1311;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1318;
	} else
		goto tr1318;
	goto tr1;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 9u: goto tr1326;
		case 10u: goto tr1327;
		case 13u: goto tr1328;
		case 32u: goto tr1326;
		case 33u: goto tr1329;
		case 34u: goto tr1330;
		case 37u: goto tr1331;
		case 38u: goto tr1332;
		case 40u: goto tr1333;
		case 41u: goto tr1334;
		case 42u: goto tr1335;
		case 43u: goto tr1336;
		case 44u: goto tr1337;
		case 45u: goto tr1338;
		case 46u: goto tr1339;
		case 47u: goto tr1340;
		case 48u: goto tr1341;
		case 58u: goto tr1343;
		case 59u: goto tr1344;
		case 60u: goto tr1345;
		case 61u: goto tr1346;
		case 62u: goto tr1347;
		case 63u: goto tr1348;
		case 91u: goto tr1350;
		case 93u: goto tr1351;
		case 94u: goto tr1352;
		case 95u: goto tr1349;
		case 123u: goto tr1353;
		case 124u: goto tr1354;
		case 125u: goto tr1355;
		case 126u: goto tr1356;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1342;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1349;
	} else
		goto tr1349;
	goto tr1;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 9u: goto tr1357;
		case 10u: goto tr1358;
		case 13u: goto tr1359;
		case 32u: goto tr1357;
		case 33u: goto tr1360;
		case 34u: goto tr1361;
		case 37u: goto tr1362;
		case 38u: goto tr1363;
		case 40u: goto tr1364;
		case 41u: goto tr1365;
		case 42u: goto tr1366;
		case 43u: goto tr1367;
		case 44u: goto tr1368;
		case 45u: goto tr1369;
		case 46u: goto tr1370;
		case 47u: goto tr1371;
		case 48u: goto tr1372;
		case 58u: goto tr1374;
		case 59u: goto tr1375;
		case 60u: goto tr1376;
		case 61u: goto st70;
		case 62u: goto st71;
		case 63u: goto tr1379;
		case 91u: goto tr1381;
		case 93u: goto tr1382;
		case 94u: goto tr1383;
		case 95u: goto tr1380;
		case 123u: goto tr1384;
		case 124u: goto tr1385;
		case 125u: goto tr1386;
		case 126u: goto tr1387;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1373;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1380;
	} else
		goto tr1380;
	goto tr1;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 9u: goto tr1388;
		case 10u: goto tr1389;
		case 13u: goto tr1390;
		case 32u: goto tr1388;
		case 33u: goto tr1391;
		case 34u: goto tr1392;
		case 37u: goto tr1393;
		case 38u: goto tr1394;
		case 40u: goto tr1395;
		case 41u: goto tr1396;
		case 42u: goto tr1397;
		case 43u: goto tr1398;
		case 44u: goto tr1399;
		case 45u: goto tr1400;
		case 46u: goto tr1401;
		case 47u: goto tr1402;
		case 48u: goto tr1403;
		case 58u: goto tr1405;
		case 59u: goto tr1406;
		case 60u: goto tr1407;
		case 61u: goto tr1408;
		case 62u: goto tr1409;
		case 63u: goto tr1410;
		case 91u: goto tr1412;
		case 93u: goto tr1413;
		case 94u: goto tr1414;
		case 95u: goto tr1411;
		case 123u: goto tr1415;
		case 124u: goto tr1416;
		case 125u: goto tr1417;
		case 126u: goto tr1418;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1404;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1411;
	} else
		goto tr1411;
	goto tr1;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 9u: goto tr1419;
		case 10u: goto tr1420;
		case 13u: goto tr1421;
		case 32u: goto tr1419;
		case 33u: goto tr1422;
		case 34u: goto tr1423;
		case 37u: goto tr1424;
		case 38u: goto tr1425;
		case 40u: goto tr1426;
		case 41u: goto tr1427;
		case 42u: goto tr1428;
		case 43u: goto tr1429;
		case 44u: goto tr1430;
		case 45u: goto tr1431;
		case 46u: goto tr1432;
		case 47u: goto tr1433;
		case 48u: goto tr1434;
		case 58u: goto tr1436;
		case 59u: goto tr1437;
		case 60u: goto tr1438;
		case 61u: goto st72;
		case 62u: goto tr1440;
		case 63u: goto tr1441;
		case 91u: goto tr1443;
		case 93u: goto tr1444;
		case 94u: goto tr1445;
		case 95u: goto tr1442;
		case 123u: goto tr1446;
		case 124u: goto tr1447;
		case 125u: goto tr1448;
		case 126u: goto tr1449;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1435;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1442;
	} else
		goto tr1442;
	goto tr1;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 9u: goto tr1450;
		case 10u: goto tr1451;
		case 13u: goto tr1452;
		case 32u: goto tr1450;
		case 33u: goto tr1453;
		case 34u: goto tr1454;
		case 37u: goto tr1455;
		case 38u: goto tr1456;
		case 40u: goto tr1457;
		case 41u: goto tr1458;
		case 42u: goto tr1459;
		case 43u: goto tr1460;
		case 44u: goto tr1461;
		case 45u: goto tr1462;
		case 46u: goto tr1463;
		case 47u: goto tr1464;
		case 48u: goto tr1465;
		case 58u: goto tr1467;
		case 59u: goto tr1468;
		case 60u: goto tr1469;
		case 61u: goto tr1470;
		case 62u: goto tr1471;
		case 63u: goto tr1472;
		case 91u: goto tr1474;
		case 93u: goto tr1475;
		case 94u: goto tr1476;
		case 95u: goto tr1473;
		case 123u: goto tr1477;
		case 124u: goto tr1478;
		case 125u: goto tr1479;
		case 126u: goto tr1480;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1466;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1473;
	} else
		goto tr1473;
	goto tr1;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
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
tr635:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 13008 "xec_parser_ragel.cpp"
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
#line 13024 "xec_parser_ragel.cpp"
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr19;
	goto tr1;
tr19:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 13036 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr644;
		case 47u: goto tr625;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr19;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr634;
	} else
		goto tr634;
	goto tr1;
tr639:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 13086 "xec_parser_ragel.cpp"
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
	goto st75;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
#line 13104 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr1512;
		case 47u: goto tr625;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 80u: goto tr635;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 112u: goto tr635;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
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
				goto tr634;
		} else if ( (*p) >= 97u )
			goto tr20;
	} else
		goto tr634;
	goto tr1;
tr1512:
#line 31 "xec_parser_ragel.rl"
	{ data.append( (char)(*p) ); }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 13162 "xec_parser_ragel.cpp"
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
	goto st76;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
#line 13180 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 9u: goto tr611;
		case 10u: goto tr612;
		case 13u: goto tr613;
		case 32u: goto tr611;
		case 33u: goto tr614;
		case 34u: goto tr615;
		case 37u: goto tr616;
		case 38u: goto tr617;
		case 40u: goto tr618;
		case 41u: goto tr619;
		case 42u: goto tr620;
		case 43u: goto tr621;
		case 44u: goto tr622;
		case 45u: goto tr623;
		case 46u: goto tr644;
		case 47u: goto tr625;
		case 58u: goto tr628;
		case 59u: goto tr629;
		case 60u: goto tr630;
		case 61u: goto tr631;
		case 62u: goto tr632;
		case 63u: goto tr633;
		case 80u: goto tr635;
		case 91u: goto tr636;
		case 93u: goto tr637;
		case 94u: goto tr638;
		case 95u: goto tr634;
		case 112u: goto tr635;
		case 123u: goto tr640;
		case 124u: goto tr641;
		case 125u: goto tr642;
		case 126u: goto tr643;
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
				goto tr634;
		} else if ( (*p) >= 97u )
			goto tr21;
	} else
		goto tr634;
	goto tr1;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	switch( (*p) ) {
		case 9u: goto tr1513;
		case 10u: goto tr1514;
		case 13u: goto tr1515;
		case 32u: goto tr1513;
		case 33u: goto tr1516;
		case 34u: goto tr1517;
		case 37u: goto tr1518;
		case 38u: goto tr1519;
		case 40u: goto tr1520;
		case 41u: goto tr1521;
		case 42u: goto tr1522;
		case 43u: goto tr1523;
		case 44u: goto tr1524;
		case 45u: goto tr1525;
		case 46u: goto tr1526;
		case 47u: goto tr1527;
		case 48u: goto tr1528;
		case 58u: goto tr1530;
		case 59u: goto tr1531;
		case 60u: goto tr1532;
		case 61u: goto tr1533;
		case 62u: goto tr1534;
		case 63u: goto tr1535;
		case 91u: goto tr1537;
		case 93u: goto tr1538;
		case 94u: goto tr1539;
		case 95u: goto tr1536;
		case 123u: goto tr1540;
		case 124u: goto tr1541;
		case 125u: goto tr1542;
		case 126u: goto tr1543;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1529;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1536;
	} else
		goto tr1536;
	goto tr1;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 9u: goto tr1544;
		case 10u: goto tr1545;
		case 13u: goto tr1546;
		case 32u: goto tr1544;
		case 33u: goto tr1547;
		case 34u: goto tr1548;
		case 37u: goto tr1549;
		case 38u: goto tr1550;
		case 40u: goto tr1551;
		case 41u: goto tr1552;
		case 42u: goto tr1553;
		case 43u: goto tr1554;
		case 44u: goto tr1555;
		case 45u: goto tr1556;
		case 46u: goto tr1557;
		case 47u: goto tr1558;
		case 48u: goto tr1559;
		case 58u: goto tr1561;
		case 59u: goto tr1562;
		case 60u: goto tr1563;
		case 61u: goto tr1564;
		case 62u: goto tr1565;
		case 63u: goto tr1566;
		case 91u: goto tr1568;
		case 93u: goto tr1569;
		case 94u: goto tr1570;
		case 95u: goto tr1567;
		case 123u: goto tr1571;
		case 124u: goto tr1572;
		case 125u: goto tr1573;
		case 126u: goto tr1574;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1560;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1567;
	} else
		goto tr1567;
	goto tr1;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 9u: goto tr1575;
		case 10u: goto tr1576;
		case 13u: goto tr1577;
		case 32u: goto tr1575;
		case 33u: goto tr1578;
		case 34u: goto tr1579;
		case 37u: goto tr1580;
		case 38u: goto tr1581;
		case 40u: goto tr1582;
		case 41u: goto tr1583;
		case 42u: goto tr1584;
		case 43u: goto tr1585;
		case 44u: goto tr1586;
		case 45u: goto tr1587;
		case 46u: goto tr1588;
		case 47u: goto tr1589;
		case 48u: goto tr1590;
		case 58u: goto tr1592;
		case 59u: goto tr1593;
		case 60u: goto tr1594;
		case 61u: goto tr1595;
		case 62u: goto tr1596;
		case 63u: goto tr1597;
		case 91u: goto tr1599;
		case 93u: goto tr1600;
		case 94u: goto tr1601;
		case 95u: goto tr1598;
		case 123u: goto tr1602;
		case 124u: goto tr1603;
		case 125u: goto tr1604;
		case 126u: goto tr1605;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1591;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1598;
	} else
		goto tr1598;
	goto tr1;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 9u: goto tr1606;
		case 10u: goto tr1607;
		case 13u: goto tr1608;
		case 32u: goto tr1606;
		case 33u: goto tr1609;
		case 34u: goto tr1610;
		case 37u: goto tr1611;
		case 38u: goto tr1612;
		case 40u: goto tr1613;
		case 41u: goto tr1614;
		case 42u: goto tr1615;
		case 43u: goto tr1616;
		case 44u: goto tr1617;
		case 45u: goto tr1618;
		case 46u: goto tr1619;
		case 47u: goto tr1620;
		case 48u: goto tr1621;
		case 58u: goto tr1623;
		case 59u: goto tr1624;
		case 60u: goto tr1625;
		case 61u: goto tr1626;
		case 62u: goto tr1627;
		case 63u: goto tr1628;
		case 91u: goto tr1630;
		case 93u: goto tr1631;
		case 94u: goto tr1632;
		case 95u: goto tr1629;
		case 123u: goto tr1633;
		case 124u: goto tr1634;
		case 125u: goto tr1635;
		case 126u: goto tr1636;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1622;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1629;
	} else
		goto tr1629;
	goto tr1;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	switch( (*p) ) {
		case 9u: goto tr1637;
		case 10u: goto tr1638;
		case 13u: goto tr1639;
		case 32u: goto tr1637;
		case 33u: goto tr1640;
		case 34u: goto tr1641;
		case 37u: goto tr1642;
		case 38u: goto tr1643;
		case 40u: goto tr1644;
		case 41u: goto tr1645;
		case 42u: goto tr1646;
		case 43u: goto tr1647;
		case 44u: goto tr1648;
		case 45u: goto tr1649;
		case 46u: goto tr1650;
		case 47u: goto tr1651;
		case 48u: goto tr1652;
		case 58u: goto tr1654;
		case 59u: goto tr1655;
		case 60u: goto tr1656;
		case 61u: goto tr1657;
		case 62u: goto tr1658;
		case 63u: goto tr1659;
		case 91u: goto tr1661;
		case 93u: goto tr1662;
		case 94u: goto tr1663;
		case 95u: goto tr1660;
		case 123u: goto tr1664;
		case 124u: goto tr1665;
		case 125u: goto tr1666;
		case 126u: goto tr1667;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1653;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1660;
	} else
		goto tr1660;
	goto tr1;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	switch( (*p) ) {
		case 9u: goto tr1668;
		case 10u: goto tr1669;
		case 13u: goto tr1670;
		case 32u: goto tr1668;
		case 33u: goto tr1671;
		case 34u: goto tr1672;
		case 37u: goto tr1673;
		case 38u: goto tr1674;
		case 40u: goto tr1675;
		case 41u: goto tr1676;
		case 42u: goto tr1677;
		case 43u: goto tr1678;
		case 44u: goto tr1679;
		case 45u: goto tr1680;
		case 46u: goto tr1681;
		case 47u: goto tr1682;
		case 48u: goto tr1683;
		case 58u: goto tr1685;
		case 59u: goto tr1686;
		case 60u: goto tr1687;
		case 61u: goto tr1688;
		case 62u: goto tr1689;
		case 63u: goto tr1690;
		case 91u: goto tr1692;
		case 93u: goto tr1693;
		case 94u: goto tr1694;
		case 95u: goto tr1691;
		case 123u: goto tr1695;
		case 124u: goto tr1696;
		case 125u: goto tr1697;
		case 126u: goto tr1698;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1684;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1691;
	} else
		goto tr1691;
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
#line 13516 "xec_parser_ragel.cpp"
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
#line 13559 "xec_parser_ragel.cpp"
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
#line 13585 "xec_parser_ragel.cpp"
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
#line 13611 "xec_parser_ragel.cpp"
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
#line 13637 "xec_parser_ragel.cpp"
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
#line 13663 "xec_parser_ragel.cpp"
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
#line 13702 "xec_parser_ragel.cpp"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr55;
	} else
		goto tr54;
	goto tr1;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	switch( (*p) ) {
		case 9u: goto tr1699;
		case 10u: goto tr1700;
		case 13u: goto tr1701;
		case 32u: goto tr1699;
		case 33u: goto tr1702;
		case 34u: goto tr1703;
		case 37u: goto tr1704;
		case 38u: goto tr1705;
		case 40u: goto tr1706;
		case 41u: goto tr1707;
		case 42u: goto tr1708;
		case 43u: goto tr1709;
		case 44u: goto tr1710;
		case 45u: goto tr1711;
		case 46u: goto tr1712;
		case 47u: goto tr1713;
		case 48u: goto tr1714;
		case 58u: goto tr1716;
		case 59u: goto tr1717;
		case 60u: goto tr1718;
		case 61u: goto tr1719;
		case 62u: goto tr1720;
		case 63u: goto tr1721;
		case 91u: goto tr1723;
		case 93u: goto tr1724;
		case 94u: goto tr1725;
		case 95u: goto tr1722;
		case 123u: goto tr1726;
		case 124u: goto tr1727;
		case 125u: goto tr1728;
		case 126u: goto tr1729;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1715;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1722;
	} else
		goto tr1722;
	goto tr1;
tr100:
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 13766 "xec_parser_ragel.cpp"
	switch( (*p) ) {
		case 42u: goto st3;
		case 47u: goto st7;
	}
	goto tr1;
	}
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
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
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
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
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 25: 
	case 26: 
#line 29 "xec_parser_ragel.rl"
	{ newlines.push_back( (int)( offset + ( p - buffer ) - 1 ) ); }
	break;
	case 56: 
#line 153 "xec_parser_ragel.rl"
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	break;
	case 43: 
	case 44: 
	case 46: 
	case 74: 
	case 75: 
	case 76: 
#line 160 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	break;
	case 28: 
#line 169 "xec_parser_ragel.rl"
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	break;
	case 27: 
#line 176 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	break;
	case 29: 
#line 177 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	break;
	case 30: 
#line 178 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	break;
	case 32: 
#line 179 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	break;
	case 33: 
#line 180 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	break;
	case 34: 
#line 181 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	break;
	case 35: 
#line 182 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	break;
	case 37: 
#line 183 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COMMA, sloc ); }
	break;
	case 38: 
#line 184 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	break;
	case 40: 
#line 185 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	break;
	case 42: 
#line 186 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	break;
	case 45: 
#line 187 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	break;
	case 47: 
#line 188 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	break;
	case 48: 
#line 189 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	break;
	case 51: 
#line 190 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	break;
	case 53: 
#line 191 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	break;
	case 55: 
#line 192 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	break;
	case 57: 
#line 193 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	break;
	case 58: 
#line 194 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	break;
	case 59: 
#line 195 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	break;
	case 61: 
#line 196 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	break;
	case 62: 
#line 197 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	break;
	case 64: 
#line 198 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	break;
	case 65: 
#line 199 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	break;
	case 36: 
#line 201 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	break;
	case 39: 
#line 202 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	break;
	case 49: 
#line 204 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	break;
	case 69: 
#line 205 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	break;
	case 71: 
#line 206 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	break;
	case 83: 
#line 208 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	break;
	case 73: 
#line 209 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	break;
	case 52: 
#line 210 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	break;
	case 54: 
#line 211 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	break;
	case 82: 
#line 213 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	break;
	case 81: 
#line 214 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	break;
	case 80: 
#line 215 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	break;
	case 79: 
#line 216 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	break;
	case 78: 
#line 217 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	break;
	case 77: 
#line 218 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	break;
	case 60: 
#line 219 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	break;
	case 63: 
#line 220 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	break;
	case 66: 
#line 221 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	break;
	case 50: 
#line 222 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	break;
	case 70: 
#line 223 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	break;
	case 72: 
#line 224 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	break;
	case 31: 
#line 226 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	break;
	case 68: 
#line 227 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	break;
	case 67: 
#line 228 "xec_parser_ragel.rl"
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	break;
	case 41: 
#line 230 "xec_parser_ragel.rl"
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
#line 236 "xec_parser_ragel.rl"
	{ {goto st23;} }
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
#line 236 "xec_parser_ragel.rl"
	{ {goto st23;} }
	break;
#line 14158 "xec_parser_ragel.cpp"
	}
	}

	_out: {}
	}

#line 392 "xec_parser_ragel.rl"
        
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





