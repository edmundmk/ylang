
/* #line 1 "xec_parser_ragel.rl" */
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





/* #line 233 "xec_parser_ragel.rl" */




/* #line 31 "xec_parser_ragel.cpp" */
static const int lexer_start = 22;
static const int lexer_first_final = 22;
static const int lexer_error = 0;

static const int lexer_en_main = 22;


/* #line 237 "xec_parser_ragel.rl" */



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
        assert( key.c_str() == text );
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
    
    
/* #line 163 "xec_parser_ragel.cpp" */
	{
	cs = lexer_start;
	}

/* #line 360 "xec_parser_ragel.rl" */
    
    
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
            diagnostic( offset, "error reading file" );
            goto error;
        }

        const unsigned char* p      = buffer;
        const unsigned char* pe     = buffer + read;
        const unsigned char* eof    = iseof ? pe : NULL;

        
/* #line 195 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr63;
		case 46u: goto tr64;
		case 47u: goto tr65;
		case 48u: goto tr66;
		case 58u: goto tr68;
		case 59u: goto tr69;
		case 60u: goto tr70;
		case 61u: goto tr71;
		case 62u: goto tr72;
		case 63u: goto tr73;
		case 91u: goto tr75;
		case 93u: goto tr76;
		case 94u: goto tr77;
		case 95u: goto tr74;
		case 123u: goto tr78;
		case 124u: goto tr79;
		case 125u: goto tr80;
		case 126u: goto tr81;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr67;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr74;
	} else
		goto tr74;
	goto tr1;
tr1:
/* #line 41 "xec_parser_ragel.rl" */
	{
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
/* #line 231 "xec_parser_ragel.rl" */
	{ {goto st22;} }
	goto st0;
/* #line 260 "xec_parser_ragel.cpp" */
st0:
cs = 0;
	goto _out;
tr83:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st23;
tr113:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st23;
tr143:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st23;
tr173:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st23;
tr203:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st23;
tr233:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st23;
tr263:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st23;
tr293:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st23;
tr323:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st23;
tr353:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st23;
tr383:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st23;
tr413:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st23;
tr443:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st23;
tr473:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st23;
tr503:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st23;
tr531:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st23;
tr564:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st23;
tr595:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st23;
tr625:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st23;
tr655:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st23;
tr685:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st23;
tr715:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st23;
tr745:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st23;
tr775:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st23;
tr805:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st23;
tr835:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st23;
tr865:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st23;
tr893:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st23;
tr923:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st23;
tr953:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st23;
tr983:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st23;
tr1013:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st23;
tr1043:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st23;
tr1073:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st23;
tr1103:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st23;
tr1133:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st23;
tr1163:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st23;
tr1193:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st23;
tr1223:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st23;
tr1253:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st23;
tr1283:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st23;
tr1313:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st23;
tr1343:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st23;
tr1373:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st23;
tr1404:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st23;
tr1434:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st23;
tr1464:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st23;
tr1494:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st23;
tr1524:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st23;
tr1554:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st23;
tr1584:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
/* #line 485 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr63;
		case 46u: goto tr64;
		case 47u: goto st21;
		case 48u: goto tr66;
		case 58u: goto tr68;
		case 59u: goto tr69;
		case 60u: goto tr70;
		case 61u: goto tr71;
		case 62u: goto tr72;
		case 63u: goto tr73;
		case 91u: goto tr75;
		case 93u: goto tr76;
		case 94u: goto tr77;
		case 95u: goto tr74;
		case 123u: goto tr78;
		case 124u: goto tr79;
		case 125u: goto tr80;
		case 126u: goto tr81;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr67;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr74;
	} else
		goto tr74;
	goto tr1;
tr84:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st24;
tr114:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st24;
tr144:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st24;
tr174:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st24;
tr204:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st24;
tr234:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st24;
tr264:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st24;
tr294:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st24;
tr324:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st24;
tr354:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st24;
tr384:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st24;
tr414:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st24;
tr444:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st24;
tr474:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st24;
tr504:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st24;
tr532:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st24;
tr565:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st24;
tr596:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st24;
tr626:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st24;
tr656:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st24;
tr686:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st24;
tr716:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st24;
tr746:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st24;
tr776:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st24;
tr806:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st24;
tr836:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st24;
tr866:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st24;
tr894:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st24;
tr924:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st24;
tr954:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st24;
tr984:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st24;
tr1014:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st24;
tr1044:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st24;
tr1074:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st24;
tr1104:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st24;
tr1134:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st24;
tr1164:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st24;
tr1194:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st24;
tr1224:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st24;
tr1254:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st24;
tr1284:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st24;
tr1314:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st24;
tr1344:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st24;
tr1374:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st24;
tr1405:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st24;
tr1435:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st24;
tr1465:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st24;
tr1495:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st24;
tr1525:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st24;
tr1555:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st24;
tr1585:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
/* #line 748 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr83;
		case 10u: goto tr84;
		case 13u: goto tr85;
		case 32u: goto tr83;
		case 33u: goto tr86;
		case 34u: goto tr87;
		case 37u: goto tr88;
		case 38u: goto tr89;
		case 40u: goto tr90;
		case 41u: goto tr91;
		case 42u: goto tr92;
		case 43u: goto tr93;
		case 45u: goto tr94;
		case 46u: goto tr95;
		case 47u: goto tr96;
		case 48u: goto tr97;
		case 58u: goto tr99;
		case 59u: goto tr100;
		case 60u: goto tr101;
		case 61u: goto tr102;
		case 62u: goto tr103;
		case 63u: goto tr104;
		case 91u: goto tr106;
		case 93u: goto tr107;
		case 94u: goto tr108;
		case 95u: goto tr105;
		case 123u: goto tr109;
		case 124u: goto tr110;
		case 125u: goto tr111;
		case 126u: goto tr112;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr98;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr105;
	} else
		goto tr105;
	goto tr1;
tr85:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st25;
tr115:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	goto st25;
tr145:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	goto st25;
tr175:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	goto st25;
tr205:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	goto st25;
tr235:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	goto st25;
tr265:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	goto st25;
tr295:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	goto st25;
tr325:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	goto st25;
tr355:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	goto st25;
tr385:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	goto st25;
tr415:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	goto st25;
tr445:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	goto st25;
tr475:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	goto st25;
tr505:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	goto st25;
tr533:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	goto st25;
tr566:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	goto st25;
tr597:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	goto st25;
tr627:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	goto st25;
tr657:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	goto st25;
tr687:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	goto st25;
tr717:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	goto st25;
tr747:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	goto st25;
tr777:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	goto st25;
tr807:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	goto st25;
tr837:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	goto st25;
tr867:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	goto st25;
tr895:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	goto st25;
tr925:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	goto st25;
tr955:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	goto st25;
tr985:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	goto st25;
tr1015:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	goto st25;
tr1045:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	goto st25;
tr1075:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	goto st25;
tr1105:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	goto st25;
tr1135:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	goto st25;
tr1165:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	goto st25;
tr1195:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	goto st25;
tr1225:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	goto st25;
tr1255:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	goto st25;
tr1285:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	goto st25;
tr1315:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	goto st25;
tr1345:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	goto st25;
tr1375:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	goto st25;
tr1406:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	goto st25;
tr1436:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	goto st25;
tr1466:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	goto st25;
tr1496:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	goto st25;
tr1526:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	goto st25;
tr1556:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	goto st25;
tr1586:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
/* #line 1011 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr83;
		case 10u: goto st24;
		case 13u: goto tr85;
		case 32u: goto tr83;
		case 33u: goto tr86;
		case 34u: goto tr87;
		case 37u: goto tr88;
		case 38u: goto tr89;
		case 40u: goto tr90;
		case 41u: goto tr91;
		case 42u: goto tr92;
		case 43u: goto tr93;
		case 45u: goto tr94;
		case 46u: goto tr95;
		case 47u: goto tr96;
		case 48u: goto tr97;
		case 58u: goto tr99;
		case 59u: goto tr100;
		case 60u: goto tr101;
		case 61u: goto tr102;
		case 62u: goto tr103;
		case 63u: goto tr104;
		case 91u: goto tr106;
		case 93u: goto tr107;
		case 94u: goto tr108;
		case 95u: goto tr105;
		case 123u: goto tr109;
		case 124u: goto tr110;
		case 125u: goto tr111;
		case 126u: goto tr112;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr98;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr105;
	} else
		goto tr105;
	goto tr1;
tr55:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr86:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr116:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr146:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr176:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr206:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr236:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr266:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr296:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr326:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr356:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr386:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr416:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr446:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr476:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr506:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr534:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr567:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr598:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr628:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr658:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr688:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr718:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr748:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr778:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr808:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr838:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr868:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr896:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr926:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr956:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr986:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1016:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1046:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1076:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1106:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1136:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1166:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1196:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1226:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1256:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1286:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1316:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1346:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1376:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1407:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1437:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1467:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1497:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1527:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1557:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
tr1587:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
/* #line 1380 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr113;
		case 10u: goto tr114;
		case 13u: goto tr115;
		case 32u: goto tr113;
		case 33u: goto tr116;
		case 34u: goto tr117;
		case 37u: goto tr118;
		case 38u: goto tr119;
		case 40u: goto tr120;
		case 41u: goto tr121;
		case 42u: goto tr122;
		case 43u: goto tr123;
		case 45u: goto tr124;
		case 46u: goto tr125;
		case 47u: goto tr126;
		case 48u: goto tr127;
		case 58u: goto tr129;
		case 59u: goto tr130;
		case 60u: goto tr131;
		case 61u: goto st80;
		case 62u: goto tr133;
		case 63u: goto tr134;
		case 91u: goto tr136;
		case 93u: goto tr137;
		case 94u: goto tr138;
		case 95u: goto tr135;
		case 123u: goto tr139;
		case 124u: goto tr140;
		case 125u: goto tr141;
		case 126u: goto tr142;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr128;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr135;
	} else
		goto tr135;
	goto tr1;
tr0:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st1;
tr21:
/* #line 76 "xec_parser_ragel.rl" */
	{ data.append( '\"' ); }
	goto st1;
tr22:
/* #line 78 "xec_parser_ragel.rl" */
	{ data.append( '/' ); }
	goto st1;
tr23:
/* #line 77 "xec_parser_ragel.rl" */
	{ data.append( '\\' ); }
	goto st1;
tr24:
/* #line 79 "xec_parser_ragel.rl" */
	{ data.append( '\b' ); }
	goto st1;
tr25:
/* #line 80 "xec_parser_ragel.rl" */
	{ data.append( '\f' ); }
	goto st1;
tr26:
/* #line 81 "xec_parser_ragel.rl" */
	{ data.append( '\n' ); }
	goto st1;
tr27:
/* #line 82 "xec_parser_ragel.rl" */
	{ data.append( '\r' ); }
	goto st1;
tr28:
/* #line 83 "xec_parser_ragel.rl" */
	{ data.append( '\t' ); }
	goto st1;
tr46:
/* #line 118 "xec_parser_ragel.rl" */
	{ temp |= (*p) - '0'; }
/* #line 123 "xec_parser_ragel.rl" */
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
/* #line 120 "xec_parser_ragel.rl" */
	{ temp |= (*p) - 'A' + 0x0A; }
/* #line 123 "xec_parser_ragel.rl" */
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
/* #line 119 "xec_parser_ragel.rl" */
	{ temp |= (*p) - 'a' + 0x0A; }
/* #line 123 "xec_parser_ragel.rl" */
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
/* #line 89 "xec_parser_ragel.rl" */
	{ data.append( (char)( temp | (*p) - '0' ) ); }
	goto st1;
tr53:
/* #line 91 "xec_parser_ragel.rl" */
	{ data.append( (char)( temp | (*p) - 'A' + 0x0A ) ); }
	goto st1;
tr54:
/* #line 90 "xec_parser_ragel.rl" */
	{ data.append( (char)( temp | (*p) - 'a' + 0x0A ) ); }
	goto st1;
tr56:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr87:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr117:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr147:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr177:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr207:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr237:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr267:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr297:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr327:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr357:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr387:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr417:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr447:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr477:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr507:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr535:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr568:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr599:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr629:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr659:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr689:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr719:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr749:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr779:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr809:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr839:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr869:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr897:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr927:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr957:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr987:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1017:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1047:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1077:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1107:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1137:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1167:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1197:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1227:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1257:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1287:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1317:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1347:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1377:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1408:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1438:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1468:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1498:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1528:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1558:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
tr1588:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
/* #line 1839 "xec_parser_ragel.cpp" */
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
		case 9u: goto tr143;
		case 10u: goto tr144;
		case 13u: goto tr145;
		case 32u: goto tr143;
		case 33u: goto tr146;
		case 34u: goto tr147;
		case 37u: goto tr148;
		case 38u: goto tr149;
		case 40u: goto tr150;
		case 41u: goto tr151;
		case 42u: goto tr152;
		case 43u: goto tr153;
		case 45u: goto tr154;
		case 46u: goto tr155;
		case 47u: goto tr156;
		case 48u: goto tr157;
		case 58u: goto tr159;
		case 59u: goto tr160;
		case 60u: goto tr161;
		case 61u: goto tr162;
		case 62u: goto tr163;
		case 63u: goto tr164;
		case 91u: goto tr166;
		case 93u: goto tr167;
		case 94u: goto tr168;
		case 95u: goto tr165;
		case 123u: goto tr169;
		case 124u: goto tr170;
		case 125u: goto tr171;
		case 126u: goto tr172;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr158;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr165;
	} else
		goto tr165;
	goto tr1;
tr57:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr88:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr118:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr148:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr178:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr208:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr238:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr268:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr298:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr328:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr358:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr388:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr418:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr448:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr478:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr508:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr536:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr569:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr600:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr630:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr660:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr690:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr720:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr750:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr780:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr810:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr840:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr870:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr898:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr928:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr958:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr988:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1018:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1048:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1078:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1108:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1138:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1168:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1198:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1228:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1258:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1288:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1318:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1348:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1378:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1409:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1439:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1469:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1499:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1529:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1559:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
tr1589:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
/* #line 2219 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr173;
		case 10u: goto tr174;
		case 13u: goto tr175;
		case 32u: goto tr173;
		case 33u: goto tr176;
		case 34u: goto tr177;
		case 37u: goto tr178;
		case 38u: goto tr179;
		case 40u: goto tr180;
		case 41u: goto tr181;
		case 42u: goto tr182;
		case 43u: goto tr183;
		case 45u: goto tr184;
		case 46u: goto tr185;
		case 47u: goto tr186;
		case 48u: goto tr187;
		case 58u: goto tr189;
		case 59u: goto tr190;
		case 60u: goto tr191;
		case 61u: goto st79;
		case 62u: goto tr193;
		case 63u: goto tr194;
		case 91u: goto tr196;
		case 93u: goto tr197;
		case 94u: goto tr198;
		case 95u: goto tr195;
		case 123u: goto tr199;
		case 124u: goto tr200;
		case 125u: goto tr201;
		case 126u: goto tr202;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr188;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr195;
	} else
		goto tr195;
	goto tr1;
tr58:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr89:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr119:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr149:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr179:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr239:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr269:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr299:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr329:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr359:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr389:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr419:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr449:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr479:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr509:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr537:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr570:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr601:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr631:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr661:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr691:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr721:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr751:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr781:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr811:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr841:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr871:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr899:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr929:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr959:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr989:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1019:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1049:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1079:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1109:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1139:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1169:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1199:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1229:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1259:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1289:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1319:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1349:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1379:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1410:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1440:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1470:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1500:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1530:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1560:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
tr1590:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
/* #line 2582 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr203;
		case 10u: goto tr204;
		case 13u: goto tr205;
		case 32u: goto tr203;
		case 33u: goto tr206;
		case 34u: goto tr207;
		case 37u: goto tr208;
		case 38u: goto st30;
		case 40u: goto tr210;
		case 41u: goto tr211;
		case 42u: goto tr212;
		case 43u: goto tr213;
		case 45u: goto tr214;
		case 46u: goto tr215;
		case 47u: goto tr216;
		case 48u: goto tr217;
		case 58u: goto tr219;
		case 59u: goto tr220;
		case 60u: goto tr221;
		case 61u: goto st78;
		case 62u: goto tr223;
		case 63u: goto tr224;
		case 91u: goto tr226;
		case 93u: goto tr227;
		case 94u: goto tr228;
		case 95u: goto tr225;
		case 123u: goto tr229;
		case 124u: goto tr230;
		case 125u: goto tr231;
		case 126u: goto tr232;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr218;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr225;
	} else
		goto tr225;
	goto tr1;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	switch( (*p) ) {
		case 9u: goto tr233;
		case 10u: goto tr234;
		case 13u: goto tr235;
		case 32u: goto tr233;
		case 33u: goto tr236;
		case 34u: goto tr237;
		case 37u: goto tr238;
		case 38u: goto tr239;
		case 40u: goto tr240;
		case 41u: goto tr241;
		case 42u: goto tr242;
		case 43u: goto tr243;
		case 45u: goto tr244;
		case 46u: goto tr245;
		case 47u: goto tr246;
		case 48u: goto tr247;
		case 58u: goto tr249;
		case 59u: goto tr250;
		case 60u: goto tr251;
		case 61u: goto tr252;
		case 62u: goto tr253;
		case 63u: goto tr254;
		case 91u: goto tr256;
		case 93u: goto tr257;
		case 94u: goto tr258;
		case 95u: goto tr255;
		case 123u: goto tr259;
		case 124u: goto tr260;
		case 125u: goto tr261;
		case 126u: goto tr262;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr248;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr255;
	} else
		goto tr255;
	goto tr1;
tr59:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr90:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr120:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr150:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr180:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr210:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr240:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr270:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr300:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr330:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr360:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr390:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr420:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr450:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr480:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr510:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr538:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr571:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr602:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr632:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr662:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr692:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr722:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr752:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr782:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr812:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr842:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr872:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr900:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr930:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr960:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr990:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1020:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1050:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1080:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1110:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1140:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1170:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1200:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1230:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1260:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1290:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1320:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1350:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1380:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1411:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1441:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1471:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1501:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1531:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1561:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
tr1591:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
/* #line 2996 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr263;
		case 10u: goto tr264;
		case 13u: goto tr265;
		case 32u: goto tr263;
		case 33u: goto tr266;
		case 34u: goto tr267;
		case 37u: goto tr268;
		case 38u: goto tr269;
		case 40u: goto tr270;
		case 41u: goto tr271;
		case 42u: goto tr272;
		case 43u: goto tr273;
		case 45u: goto tr274;
		case 46u: goto tr275;
		case 47u: goto tr276;
		case 48u: goto tr277;
		case 58u: goto tr279;
		case 59u: goto tr280;
		case 60u: goto tr281;
		case 61u: goto tr282;
		case 62u: goto tr283;
		case 63u: goto tr284;
		case 91u: goto tr286;
		case 93u: goto tr287;
		case 94u: goto tr288;
		case 95u: goto tr285;
		case 123u: goto tr289;
		case 124u: goto tr290;
		case 125u: goto tr291;
		case 126u: goto tr292;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr278;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr285;
	} else
		goto tr285;
	goto tr1;
tr60:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr91:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr121:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr151:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr181:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr211:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr241:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr271:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr301:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr331:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr361:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr391:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr421:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr451:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr481:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr511:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr539:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr572:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr603:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr633:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr663:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr693:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr723:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr753:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr783:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr813:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr843:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr873:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr901:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr931:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr961:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr991:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1021:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1051:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1081:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1111:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1141:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1171:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1201:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1231:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1261:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1291:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1321:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1351:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1381:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1412:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1442:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1472:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1502:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1532:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1562:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
tr1592:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
/* #line 3365 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr293;
		case 10u: goto tr294;
		case 13u: goto tr295;
		case 32u: goto tr293;
		case 33u: goto tr296;
		case 34u: goto tr297;
		case 37u: goto tr298;
		case 38u: goto tr299;
		case 40u: goto tr300;
		case 41u: goto tr301;
		case 42u: goto tr302;
		case 43u: goto tr303;
		case 45u: goto tr304;
		case 46u: goto tr305;
		case 47u: goto tr306;
		case 48u: goto tr307;
		case 58u: goto tr309;
		case 59u: goto tr310;
		case 60u: goto tr311;
		case 61u: goto tr312;
		case 62u: goto tr313;
		case 63u: goto tr314;
		case 91u: goto tr316;
		case 93u: goto tr317;
		case 94u: goto tr318;
		case 95u: goto tr315;
		case 123u: goto tr319;
		case 124u: goto tr320;
		case 125u: goto tr321;
		case 126u: goto tr322;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr308;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr315;
	} else
		goto tr315;
	goto tr1;
tr61:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr92:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr122:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr152:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr182:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr212:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr242:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr272:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr302:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr332:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr362:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr392:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr422:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr452:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr482:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr540:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr573:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr604:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr634:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr664:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr694:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr724:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr754:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr784:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr814:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr844:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr874:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr902:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr932:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr962:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr992:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1022:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1052:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1082:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1112:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1142:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1172:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1202:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1232:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1262:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1292:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1322:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1352:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1382:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1413:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1443:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1473:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1503:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1533:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1563:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
tr1593:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
/* #line 3728 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr323;
		case 10u: goto tr324;
		case 13u: goto tr325;
		case 32u: goto tr323;
		case 33u: goto tr326;
		case 34u: goto tr327;
		case 37u: goto tr328;
		case 38u: goto tr329;
		case 40u: goto tr330;
		case 41u: goto tr331;
		case 42u: goto tr332;
		case 43u: goto tr333;
		case 45u: goto tr334;
		case 46u: goto tr335;
		case 47u: goto tr336;
		case 48u: goto tr337;
		case 58u: goto tr339;
		case 59u: goto tr340;
		case 60u: goto tr341;
		case 61u: goto st77;
		case 62u: goto tr343;
		case 63u: goto tr344;
		case 91u: goto tr346;
		case 93u: goto tr347;
		case 94u: goto tr348;
		case 95u: goto tr345;
		case 123u: goto tr349;
		case 124u: goto tr350;
		case 125u: goto tr351;
		case 126u: goto tr352;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr338;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr345;
	} else
		goto tr345;
	goto tr1;
tr62:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr93:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr123:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr153:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr183:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr213:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr243:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr273:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr303:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr333:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr393:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr423:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr453:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr483:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr512:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr541:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr574:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr605:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr635:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr665:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr695:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr725:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr755:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr785:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr815:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr845:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr875:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr903:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr933:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr963:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr993:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1023:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1053:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1083:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1113:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1143:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1173:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1203:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1233:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1263:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1293:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1323:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1353:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1383:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1414:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1444:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1474:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1504:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1534:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1564:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
tr1594:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
/* #line 4091 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr353;
		case 10u: goto tr354;
		case 13u: goto tr355;
		case 32u: goto tr353;
		case 33u: goto tr356;
		case 34u: goto tr357;
		case 37u: goto tr358;
		case 38u: goto tr359;
		case 40u: goto tr360;
		case 41u: goto tr361;
		case 42u: goto tr362;
		case 43u: goto st35;
		case 45u: goto tr364;
		case 46u: goto tr365;
		case 47u: goto tr366;
		case 48u: goto tr367;
		case 58u: goto tr369;
		case 59u: goto tr370;
		case 60u: goto tr371;
		case 61u: goto st76;
		case 62u: goto tr373;
		case 63u: goto tr374;
		case 91u: goto tr376;
		case 93u: goto tr377;
		case 94u: goto tr378;
		case 95u: goto tr375;
		case 123u: goto tr379;
		case 124u: goto tr380;
		case 125u: goto tr381;
		case 126u: goto tr382;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr368;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr375;
	} else
		goto tr375;
	goto tr1;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	switch( (*p) ) {
		case 9u: goto tr383;
		case 10u: goto tr384;
		case 13u: goto tr385;
		case 32u: goto tr383;
		case 33u: goto tr386;
		case 34u: goto tr387;
		case 37u: goto tr388;
		case 38u: goto tr389;
		case 40u: goto tr390;
		case 41u: goto tr391;
		case 42u: goto tr392;
		case 43u: goto tr393;
		case 45u: goto tr394;
		case 46u: goto tr395;
		case 47u: goto tr396;
		case 48u: goto tr397;
		case 58u: goto tr399;
		case 59u: goto tr400;
		case 60u: goto tr401;
		case 61u: goto tr402;
		case 62u: goto tr403;
		case 63u: goto tr404;
		case 91u: goto tr406;
		case 93u: goto tr407;
		case 94u: goto tr408;
		case 95u: goto tr405;
		case 123u: goto tr409;
		case 124u: goto tr410;
		case 125u: goto tr411;
		case 126u: goto tr412;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr398;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr405;
	} else
		goto tr405;
	goto tr1;
tr63:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr94:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr124:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr154:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr184:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr214:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr244:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr274:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr304:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr334:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr364:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr394:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr454:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr484:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr513:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr542:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr575:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr606:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr636:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr666:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr696:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr726:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr756:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr786:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr816:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr846:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr876:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr904:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr934:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr964:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr994:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1024:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1054:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1084:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1114:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1144:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1174:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1204:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1234:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1264:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1294:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1324:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1354:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1384:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1415:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1445:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1475:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1505:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1535:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1565:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
tr1595:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
/* #line 4499 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr413;
		case 10u: goto tr414;
		case 13u: goto tr415;
		case 32u: goto tr413;
		case 33u: goto tr416;
		case 34u: goto tr417;
		case 37u: goto tr418;
		case 38u: goto tr419;
		case 40u: goto tr420;
		case 41u: goto tr421;
		case 42u: goto tr422;
		case 43u: goto tr423;
		case 45u: goto st37;
		case 46u: goto tr425;
		case 47u: goto tr426;
		case 48u: goto tr427;
		case 58u: goto tr429;
		case 59u: goto tr430;
		case 60u: goto tr431;
		case 61u: goto st75;
		case 62u: goto tr433;
		case 63u: goto tr434;
		case 91u: goto tr436;
		case 93u: goto tr437;
		case 94u: goto tr438;
		case 95u: goto tr435;
		case 123u: goto tr439;
		case 124u: goto tr440;
		case 125u: goto tr441;
		case 126u: goto tr442;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr428;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr435;
	} else
		goto tr435;
	goto tr1;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	switch( (*p) ) {
		case 9u: goto tr443;
		case 10u: goto tr444;
		case 13u: goto tr445;
		case 32u: goto tr443;
		case 33u: goto tr446;
		case 34u: goto tr447;
		case 37u: goto tr448;
		case 38u: goto tr449;
		case 40u: goto tr450;
		case 41u: goto tr451;
		case 42u: goto tr452;
		case 43u: goto tr453;
		case 45u: goto tr454;
		case 46u: goto tr455;
		case 47u: goto tr456;
		case 48u: goto tr457;
		case 58u: goto tr459;
		case 59u: goto tr460;
		case 60u: goto tr461;
		case 61u: goto tr462;
		case 62u: goto tr463;
		case 63u: goto tr464;
		case 91u: goto tr466;
		case 93u: goto tr467;
		case 94u: goto tr468;
		case 95u: goto tr465;
		case 123u: goto tr469;
		case 124u: goto tr470;
		case 125u: goto tr471;
		case 126u: goto tr472;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr458;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr465;
	} else
		goto tr465;
	goto tr1;
tr64:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr95:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr125:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr155:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr185:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr215:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr245:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr275:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr305:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr335:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr365:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr395:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr425:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr455:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr485:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr514:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr563:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr576:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr607:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr637:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr667:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr697:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr727:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr757:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr787:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr817:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr847:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr877:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr905:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr935:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr965:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr995:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1025:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1055:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1085:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1115:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1145:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1175:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1205:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1235:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1265:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1295:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1325:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1355:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1385:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1416:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1446:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1476:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1506:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1536:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1566:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
tr1596:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
/* #line 4913 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr473;
		case 10u: goto tr474;
		case 13u: goto tr475;
		case 32u: goto tr473;
		case 33u: goto tr476;
		case 34u: goto tr477;
		case 37u: goto tr478;
		case 38u: goto tr479;
		case 40u: goto tr480;
		case 41u: goto tr481;
		case 42u: goto tr482;
		case 43u: goto tr483;
		case 45u: goto tr484;
		case 46u: goto tr485;
		case 47u: goto tr486;
		case 48u: goto tr487;
		case 58u: goto tr489;
		case 59u: goto tr490;
		case 60u: goto tr491;
		case 61u: goto tr492;
		case 62u: goto tr493;
		case 63u: goto tr494;
		case 91u: goto tr496;
		case 93u: goto tr497;
		case 94u: goto tr498;
		case 95u: goto tr495;
		case 123u: goto tr499;
		case 124u: goto tr500;
		case 125u: goto tr501;
		case 126u: goto tr502;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr488;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr495;
	} else
		goto tr495;
	goto tr1;
tr65:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr126:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr156:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr186:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr216:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr246:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr276:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr306:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr336:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr366:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr396:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr426:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr456:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr486:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr544:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr577:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr608:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr638:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr668:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr698:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr728:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr758:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr788:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr818:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr848:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr878:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr906:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr936:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr966:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr996:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1026:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1056:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1086:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1116:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1146:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1176:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1206:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1236:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1266:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1296:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1326:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1356:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1386:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1417:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1447:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1477:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1507:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1537:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1567:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
tr1597:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
/* #line 5270 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr503;
		case 10u: goto tr504;
		case 13u: goto tr505;
		case 32u: goto tr503;
		case 33u: goto tr506;
		case 34u: goto tr507;
		case 37u: goto tr508;
		case 38u: goto tr509;
		case 40u: goto tr510;
		case 41u: goto tr511;
		case 42u: goto st2;
		case 43u: goto tr512;
		case 45u: goto tr513;
		case 46u: goto tr514;
		case 47u: goto st6;
		case 48u: goto tr515;
		case 58u: goto tr517;
		case 59u: goto tr518;
		case 60u: goto tr519;
		case 61u: goto st74;
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
tr8:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
/* #line 5320 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 10u: goto st3;
		case 13u: goto st4;
		case 42u: goto st5;
	}
	goto st2;
tr9:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
/* #line 5335 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 10u: goto tr9;
		case 13u: goto tr10;
		case 42u: goto tr11;
	}
	goto tr8;
tr10:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
/* #line 5350 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 10u: goto st3;
		case 13u: goto tr10;
		case 42u: goto tr11;
	}
	goto tr8;
tr11:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
/* #line 5365 "xec_parser_ragel.cpp" */
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
tr66:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr97:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr127:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr157:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr187:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr217:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr247:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr277:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr307:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr337:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr367:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr397:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr427:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr457:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr487:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr515:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr545:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr578:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr609:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr639:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr669:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr699:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr729:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr759:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr789:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr819:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr849:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr907:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr937:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr967:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr997:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1027:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1057:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1087:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1117:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1147:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1177:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1207:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1237:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1267:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1297:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1327:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1357:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1387:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1418:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1448:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1478:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1508:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1538:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1568:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
tr1598:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
/* #line 5802 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr543;
		case 47u: goto tr544;
		case 48u: goto tr545;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 69u: goto tr554;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 101u: goto tr554;
		case 120u: goto tr558;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr546;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr553;
	} else
		goto tr553;
	goto tr1;
tr543:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
/* #line 5855 "xec_parser_ragel.cpp" */
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr16;
	goto tr1;
tr16:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
/* #line 5867 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr563;
		case 47u: goto tr544;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 69u: goto tr554;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 101u: goto tr554;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr16;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr553;
	} else
		goto tr553;
	goto tr1;
tr68:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr99:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr129:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr159:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr189:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr219:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr249:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr279:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr309:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr339:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr369:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr399:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr429:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr459:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr489:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr517:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr547:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr580:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr611:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr641:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr671:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr701:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr731:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr761:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr791:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr821:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr851:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr880:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr909:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr939:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr969:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr999:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1029:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1059:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1089:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1119:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1149:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1179:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1209:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1239:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1269:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1299:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1329:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1359:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1389:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1420:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1450:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1480:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1510:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1540:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1570:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
tr1600:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
/* #line 6237 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr564;
		case 10u: goto tr565;
		case 13u: goto tr566;
		case 32u: goto tr564;
		case 33u: goto tr567;
		case 34u: goto tr568;
		case 37u: goto tr569;
		case 38u: goto tr570;
		case 40u: goto tr571;
		case 41u: goto tr572;
		case 42u: goto tr573;
		case 43u: goto tr574;
		case 45u: goto tr575;
		case 46u: goto tr576;
		case 47u: goto tr577;
		case 48u: goto tr578;
		case 58u: goto tr580;
		case 59u: goto tr581;
		case 60u: goto tr582;
		case 61u: goto tr583;
		case 62u: goto tr584;
		case 63u: goto tr585;
		case 91u: goto tr587;
		case 93u: goto tr588;
		case 94u: goto tr589;
		case 95u: goto tr586;
		case 123u: goto tr590;
		case 124u: goto tr591;
		case 125u: goto tr592;
		case 126u: goto tr593;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr579;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr586;
	} else
		goto tr586;
	goto tr1;
tr594:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr67:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr98:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr128:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr158:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr188:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr218:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr248:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr278:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr308:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr338:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr368:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr398:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr428:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr458:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr488:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr516:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr546:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr579:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr610:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr640:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr670:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr700:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr730:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr760:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr790:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr820:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr850:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr908:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr938:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr968:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr998:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1028:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1058:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1088:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1118:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1148:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1178:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1208:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1238:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1268:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1298:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1328:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1358:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1388:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1419:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1449:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1479:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1509:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1539:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1569:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
tr1599:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
/* #line 6703 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr543;
		case 47u: goto tr544;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 69u: goto tr554;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 101u: goto tr554;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr594;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr553;
	} else
		goto tr553;
	goto tr1;
tr69:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr100:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr130:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr160:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr190:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr220:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr250:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr280:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr310:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr340:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr370:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr400:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr430:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr460:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr490:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr518:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr548:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr581:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr612:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr642:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr672:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr702:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr732:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr762:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr792:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr822:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr852:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr881:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr910:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr940:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr970:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1000:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1030:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1060:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1090:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1120:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1150:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1180:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1210:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1240:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1270:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1300:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1330:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1360:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1390:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1421:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1451:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1481:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1511:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1541:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1571:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
tr1601:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
/* #line 7073 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr595;
		case 10u: goto tr596;
		case 13u: goto tr597;
		case 32u: goto tr595;
		case 33u: goto tr598;
		case 34u: goto tr599;
		case 37u: goto tr600;
		case 38u: goto tr601;
		case 40u: goto tr602;
		case 41u: goto tr603;
		case 42u: goto tr604;
		case 43u: goto tr605;
		case 45u: goto tr606;
		case 46u: goto tr607;
		case 47u: goto tr608;
		case 48u: goto tr609;
		case 58u: goto tr611;
		case 59u: goto tr612;
		case 60u: goto tr613;
		case 61u: goto tr614;
		case 62u: goto tr615;
		case 63u: goto tr616;
		case 91u: goto tr618;
		case 93u: goto tr619;
		case 94u: goto tr620;
		case 95u: goto tr617;
		case 123u: goto tr621;
		case 124u: goto tr622;
		case 125u: goto tr623;
		case 126u: goto tr624;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr610;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr617;
	} else
		goto tr617;
	goto tr1;
tr70:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr101:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr131:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr161:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr191:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr221:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr251:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr281:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr311:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr341:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr371:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr401:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr431:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr461:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr491:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr519:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr549:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr582:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr613:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr673:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr703:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr733:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr763:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr793:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr823:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr853:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr882:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr911:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr941:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr971:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1001:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1031:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1061:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1091:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1121:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1151:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1181:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1211:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1241:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1271:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1301:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1331:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1361:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1391:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1422:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1452:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1482:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1512:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1542:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1572:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
tr1602:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
/* #line 7436 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr625;
		case 10u: goto tr626;
		case 13u: goto tr627;
		case 32u: goto tr625;
		case 33u: goto tr628;
		case 34u: goto tr629;
		case 37u: goto tr630;
		case 38u: goto tr631;
		case 40u: goto tr632;
		case 41u: goto tr633;
		case 42u: goto tr634;
		case 43u: goto tr635;
		case 45u: goto tr636;
		case 46u: goto tr637;
		case 47u: goto tr638;
		case 48u: goto tr639;
		case 58u: goto tr641;
		case 59u: goto tr642;
		case 60u: goto st46;
		case 61u: goto st70;
		case 62u: goto tr645;
		case 63u: goto tr646;
		case 91u: goto tr648;
		case 93u: goto tr649;
		case 94u: goto tr650;
		case 95u: goto tr647;
		case 123u: goto tr651;
		case 124u: goto tr652;
		case 125u: goto tr653;
		case 126u: goto tr654;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr640;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr647;
	} else
		goto tr647;
	goto tr1;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 9u: goto tr655;
		case 10u: goto tr656;
		case 13u: goto tr657;
		case 32u: goto tr655;
		case 33u: goto tr658;
		case 34u: goto tr659;
		case 37u: goto tr660;
		case 38u: goto tr661;
		case 40u: goto tr662;
		case 41u: goto tr663;
		case 42u: goto tr664;
		case 43u: goto tr665;
		case 45u: goto tr666;
		case 46u: goto tr667;
		case 47u: goto tr668;
		case 48u: goto tr669;
		case 58u: goto tr671;
		case 59u: goto tr672;
		case 60u: goto tr673;
		case 61u: goto st47;
		case 62u: goto tr675;
		case 63u: goto tr676;
		case 91u: goto tr678;
		case 93u: goto tr679;
		case 94u: goto tr680;
		case 95u: goto tr677;
		case 123u: goto tr681;
		case 124u: goto tr682;
		case 125u: goto tr683;
		case 126u: goto tr684;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr670;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr677;
	} else
		goto tr677;
	goto tr1;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 9u: goto tr685;
		case 10u: goto tr686;
		case 13u: goto tr687;
		case 32u: goto tr685;
		case 33u: goto tr688;
		case 34u: goto tr689;
		case 37u: goto tr690;
		case 38u: goto tr691;
		case 40u: goto tr692;
		case 41u: goto tr693;
		case 42u: goto tr694;
		case 43u: goto tr695;
		case 45u: goto tr696;
		case 46u: goto tr697;
		case 47u: goto tr698;
		case 48u: goto tr699;
		case 58u: goto tr701;
		case 59u: goto tr702;
		case 60u: goto tr703;
		case 61u: goto tr704;
		case 62u: goto tr705;
		case 63u: goto tr706;
		case 91u: goto tr708;
		case 93u: goto tr709;
		case 94u: goto tr710;
		case 95u: goto tr707;
		case 123u: goto tr711;
		case 124u: goto tr712;
		case 125u: goto tr713;
		case 126u: goto tr714;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr700;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr707;
	} else
		goto tr707;
	goto tr1;
tr71:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr102:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr162:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr252:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr282:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr312:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr402:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr462:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr492:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr550:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr583:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr614:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr704:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr764:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr824:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr854:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr883:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr912:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr942:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1002:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1032:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1092:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1122:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1182:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1212:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1242:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1302:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1362:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1392:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1423:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1453:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1483:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1513:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1543:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1573:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
tr1603:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
/* #line 7799 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr715;
		case 10u: goto tr716;
		case 13u: goto tr717;
		case 32u: goto tr715;
		case 33u: goto tr718;
		case 34u: goto tr719;
		case 37u: goto tr720;
		case 38u: goto tr721;
		case 40u: goto tr722;
		case 41u: goto tr723;
		case 42u: goto tr724;
		case 43u: goto tr725;
		case 45u: goto tr726;
		case 46u: goto tr727;
		case 47u: goto tr728;
		case 48u: goto tr729;
		case 58u: goto tr731;
		case 59u: goto tr732;
		case 60u: goto tr733;
		case 61u: goto st49;
		case 62u: goto tr735;
		case 63u: goto tr736;
		case 91u: goto tr738;
		case 93u: goto tr739;
		case 94u: goto tr740;
		case 95u: goto tr737;
		case 123u: goto tr741;
		case 124u: goto tr742;
		case 125u: goto tr743;
		case 126u: goto tr744;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr730;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr737;
	} else
		goto tr737;
	goto tr1;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	switch( (*p) ) {
		case 9u: goto tr745;
		case 10u: goto tr746;
		case 13u: goto tr747;
		case 32u: goto tr745;
		case 33u: goto tr748;
		case 34u: goto tr749;
		case 37u: goto tr750;
		case 38u: goto tr751;
		case 40u: goto tr752;
		case 41u: goto tr753;
		case 42u: goto tr754;
		case 43u: goto tr755;
		case 45u: goto tr756;
		case 46u: goto tr757;
		case 47u: goto tr758;
		case 48u: goto tr759;
		case 58u: goto tr761;
		case 59u: goto tr762;
		case 60u: goto tr763;
		case 61u: goto tr764;
		case 62u: goto tr765;
		case 63u: goto tr766;
		case 91u: goto tr768;
		case 93u: goto tr769;
		case 94u: goto tr770;
		case 95u: goto tr767;
		case 123u: goto tr771;
		case 124u: goto tr772;
		case 125u: goto tr773;
		case 126u: goto tr774;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr760;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr767;
	} else
		goto tr767;
	goto tr1;
tr72:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr103:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr133:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr163:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr193:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr223:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr253:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr283:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr313:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr343:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr373:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr403:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr433:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr463:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr493:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr521:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr551:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr584:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr615:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr645:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr675:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr705:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr735:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr765:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr825:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr855:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr884:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr913:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr943:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr973:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1003:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1033:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1063:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1093:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1123:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1153:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1183:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1213:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1243:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1303:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1333:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1363:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1393:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1424:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1454:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1484:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1514:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1544:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1574:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
tr1604:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
/* #line 8201 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr775;
		case 10u: goto tr776;
		case 13u: goto tr777;
		case 32u: goto tr775;
		case 33u: goto tr778;
		case 34u: goto tr779;
		case 37u: goto tr780;
		case 38u: goto tr781;
		case 40u: goto tr782;
		case 41u: goto tr783;
		case 42u: goto tr784;
		case 43u: goto tr785;
		case 45u: goto tr786;
		case 46u: goto tr787;
		case 47u: goto tr788;
		case 48u: goto tr789;
		case 58u: goto tr791;
		case 59u: goto tr792;
		case 60u: goto tr793;
		case 61u: goto st51;
		case 62u: goto st66;
		case 63u: goto tr796;
		case 91u: goto tr798;
		case 93u: goto tr799;
		case 94u: goto tr800;
		case 95u: goto tr797;
		case 123u: goto tr801;
		case 124u: goto tr802;
		case 125u: goto tr803;
		case 126u: goto tr804;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr790;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr797;
	} else
		goto tr797;
	goto tr1;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	switch( (*p) ) {
		case 9u: goto tr805;
		case 10u: goto tr806;
		case 13u: goto tr807;
		case 32u: goto tr805;
		case 33u: goto tr808;
		case 34u: goto tr809;
		case 37u: goto tr810;
		case 38u: goto tr811;
		case 40u: goto tr812;
		case 41u: goto tr813;
		case 42u: goto tr814;
		case 43u: goto tr815;
		case 45u: goto tr816;
		case 46u: goto tr817;
		case 47u: goto tr818;
		case 48u: goto tr819;
		case 58u: goto tr821;
		case 59u: goto tr822;
		case 60u: goto tr823;
		case 61u: goto tr824;
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
tr73:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr104:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr134:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr164:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr194:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr224:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr254:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr284:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr314:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr344:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr374:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr404:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr434:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr464:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr494:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr522:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr552:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr585:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr616:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr646:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr676:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr706:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr736:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr766:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr796:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr826:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr856:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr885:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr914:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr944:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr974:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1004:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1034:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1064:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1094:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1124:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1154:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1184:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1214:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1244:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1274:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1304:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1334:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1364:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1394:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1425:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1455:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1485:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1515:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1545:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1575:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
tr1605:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st52;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
/* #line 8615 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr846;
		case 46u: goto tr847;
		case 47u: goto tr848;
		case 48u: goto tr849;
		case 58u: goto tr851;
		case 59u: goto tr852;
		case 60u: goto tr853;
		case 61u: goto tr854;
		case 62u: goto tr855;
		case 63u: goto tr856;
		case 91u: goto tr858;
		case 93u: goto tr859;
		case 94u: goto tr860;
		case 95u: goto tr857;
		case 123u: goto tr861;
		case 124u: goto tr862;
		case 125u: goto tr863;
		case 126u: goto tr864;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr850;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr857;
	} else
		goto tr857;
	goto tr1;
tr879:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr74:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr105:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr135:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr165:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr195:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr225:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr255:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr285:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr315:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr345:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr375:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr405:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr435:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr465:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr495:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr523:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr553:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr586:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr617:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr647:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr677:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr707:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr737:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr767:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr797:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr827:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr857:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr915:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr945:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr975:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1005:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1035:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1065:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1095:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1125:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1155:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1185:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1215:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1245:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1275:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1305:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1335:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1365:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1395:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1426:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1456:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1486:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1516:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1546:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1576:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
tr1606:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
/* #line 9081 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr865;
		case 10u: goto tr866;
		case 13u: goto tr867;
		case 32u: goto tr865;
		case 33u: goto tr868;
		case 34u: goto tr869;
		case 37u: goto tr870;
		case 38u: goto tr871;
		case 40u: goto tr872;
		case 41u: goto tr873;
		case 42u: goto tr874;
		case 43u: goto tr875;
		case 45u: goto tr876;
		case 46u: goto tr877;
		case 47u: goto tr878;
		case 58u: goto tr880;
		case 59u: goto tr881;
		case 60u: goto tr882;
		case 61u: goto tr883;
		case 62u: goto tr884;
		case 63u: goto tr885;
		case 91u: goto tr886;
		case 93u: goto tr887;
		case 94u: goto tr888;
		case 95u: goto tr879;
		case 123u: goto tr889;
		case 124u: goto tr890;
		case 125u: goto tr891;
		case 126u: goto tr892;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr879;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr879;
	} else
		goto tr879;
	goto tr1;
tr75:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr106:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr136:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr166:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr196:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr226:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr256:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr286:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr316:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr346:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr376:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr406:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr436:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr466:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr496:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr524:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr555:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr587:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr618:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr648:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr678:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr708:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr738:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr768:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr798:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr828:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr858:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr886:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr916:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr946:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr976:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1006:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1036:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1066:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1096:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1126:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1156:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1186:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1216:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1246:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1276:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1306:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1336:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1366:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1396:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1427:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1457:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1487:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1517:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1547:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1577:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
tr1607:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
/* #line 9449 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr904;
		case 46u: goto tr905;
		case 47u: goto tr906;
		case 48u: goto tr907;
		case 58u: goto tr909;
		case 59u: goto tr910;
		case 60u: goto tr911;
		case 61u: goto tr912;
		case 62u: goto tr913;
		case 63u: goto tr914;
		case 91u: goto tr916;
		case 93u: goto tr917;
		case 94u: goto tr918;
		case 95u: goto tr915;
		case 123u: goto tr919;
		case 124u: goto tr920;
		case 125u: goto tr921;
		case 126u: goto tr922;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr908;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr915;
	} else
		goto tr915;
	goto tr1;
tr76:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr107:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr137:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr167:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr197:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr227:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr257:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr287:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr317:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr347:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr377:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr407:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr437:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr467:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr497:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr525:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr556:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr588:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr619:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr649:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr679:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr709:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr739:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr769:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr799:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr829:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr859:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr887:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr917:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr947:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr977:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1007:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1037:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1067:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1097:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1127:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1157:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1187:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1217:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1247:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1277:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1307:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1337:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1367:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1397:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1428:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1458:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1488:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1518:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1548:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1578:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
tr1608:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st55;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
/* #line 9818 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr923;
		case 10u: goto tr924;
		case 13u: goto tr925;
		case 32u: goto tr923;
		case 33u: goto tr926;
		case 34u: goto tr927;
		case 37u: goto tr928;
		case 38u: goto tr929;
		case 40u: goto tr930;
		case 41u: goto tr931;
		case 42u: goto tr932;
		case 43u: goto tr933;
		case 45u: goto tr934;
		case 46u: goto tr935;
		case 47u: goto tr936;
		case 48u: goto tr937;
		case 58u: goto tr939;
		case 59u: goto tr940;
		case 60u: goto tr941;
		case 61u: goto tr942;
		case 62u: goto tr943;
		case 63u: goto tr944;
		case 91u: goto tr946;
		case 93u: goto tr947;
		case 94u: goto tr948;
		case 95u: goto tr945;
		case 123u: goto tr949;
		case 124u: goto tr950;
		case 125u: goto tr951;
		case 126u: goto tr952;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr938;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr945;
	} else
		goto tr945;
	goto tr1;
tr77:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr108:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr138:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr168:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr198:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr228:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr258:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr288:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr318:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr348:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr378:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr408:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr438:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr468:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr498:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr526:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr557:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr589:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr620:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr650:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr680:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr710:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr740:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr770:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr800:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr830:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr860:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr888:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr918:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr948:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1008:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1038:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1068:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1098:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1128:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1158:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1188:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1218:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1248:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1278:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1308:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1338:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1368:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1398:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1429:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1459:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1489:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1519:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1549:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1579:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
tr1609:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st56;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
/* #line 10181 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr964;
		case 46u: goto tr965;
		case 47u: goto tr966;
		case 48u: goto tr967;
		case 58u: goto tr969;
		case 59u: goto tr970;
		case 60u: goto tr971;
		case 61u: goto st57;
		case 62u: goto tr973;
		case 63u: goto tr974;
		case 91u: goto tr976;
		case 93u: goto tr977;
		case 94u: goto st65;
		case 95u: goto tr975;
		case 123u: goto tr979;
		case 124u: goto tr980;
		case 125u: goto tr981;
		case 126u: goto tr982;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr968;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr975;
	} else
		goto tr975;
	goto tr1;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	switch( (*p) ) {
		case 9u: goto tr983;
		case 10u: goto tr984;
		case 13u: goto tr985;
		case 32u: goto tr983;
		case 33u: goto tr986;
		case 34u: goto tr987;
		case 37u: goto tr988;
		case 38u: goto tr989;
		case 40u: goto tr990;
		case 41u: goto tr991;
		case 42u: goto tr992;
		case 43u: goto tr993;
		case 45u: goto tr994;
		case 46u: goto tr995;
		case 47u: goto tr996;
		case 48u: goto tr997;
		case 58u: goto tr999;
		case 59u: goto tr1000;
		case 60u: goto tr1001;
		case 61u: goto tr1002;
		case 62u: goto tr1003;
		case 63u: goto tr1004;
		case 91u: goto tr1006;
		case 93u: goto tr1007;
		case 94u: goto tr1008;
		case 95u: goto tr1005;
		case 123u: goto tr1009;
		case 124u: goto tr1010;
		case 125u: goto tr1011;
		case 126u: goto tr1012;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr998;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1005;
	} else
		goto tr1005;
	goto tr1;
tr78:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr109:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr139:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr169:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr199:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr229:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr259:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr289:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr319:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr349:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr379:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr409:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr439:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr469:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr499:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr527:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr559:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr590:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr621:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr651:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr681:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr711:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr741:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr771:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr801:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr831:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr861:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr889:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr919:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr949:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr979:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1009:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1039:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1069:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1099:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1129:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1159:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1189:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1219:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1249:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1279:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1309:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1339:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1369:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1399:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1430:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1460:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1490:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1520:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1550:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1580:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
tr1610:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st58;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
/* #line 10595 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr1013;
		case 10u: goto tr1014;
		case 13u: goto tr1015;
		case 32u: goto tr1013;
		case 33u: goto tr1016;
		case 34u: goto tr1017;
		case 37u: goto tr1018;
		case 38u: goto tr1019;
		case 40u: goto tr1020;
		case 41u: goto tr1021;
		case 42u: goto tr1022;
		case 43u: goto tr1023;
		case 45u: goto tr1024;
		case 46u: goto tr1025;
		case 47u: goto tr1026;
		case 48u: goto tr1027;
		case 58u: goto tr1029;
		case 59u: goto tr1030;
		case 60u: goto tr1031;
		case 61u: goto tr1032;
		case 62u: goto tr1033;
		case 63u: goto tr1034;
		case 91u: goto tr1036;
		case 93u: goto tr1037;
		case 94u: goto tr1038;
		case 95u: goto tr1035;
		case 123u: goto tr1039;
		case 124u: goto tr1040;
		case 125u: goto tr1041;
		case 126u: goto tr1042;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1028;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1035;
	} else
		goto tr1035;
	goto tr1;
tr79:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr110:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr140:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr170:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr200:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr230:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr260:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr290:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr320:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr350:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr380:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr410:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr440:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr470:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr500:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr528:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr560:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr591:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr622:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr652:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr682:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr712:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr742:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr772:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr802:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr832:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr862:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr890:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr920:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr950:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr980:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1010:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1040:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1100:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1130:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1160:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1190:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1220:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1250:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1280:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1310:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1340:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1370:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1400:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1431:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1461:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1491:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1521:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1551:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1581:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
tr1611:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
/* #line 10958 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr1043;
		case 10u: goto tr1044;
		case 13u: goto tr1045;
		case 32u: goto tr1043;
		case 33u: goto tr1046;
		case 34u: goto tr1047;
		case 37u: goto tr1048;
		case 38u: goto tr1049;
		case 40u: goto tr1050;
		case 41u: goto tr1051;
		case 42u: goto tr1052;
		case 43u: goto tr1053;
		case 45u: goto tr1054;
		case 46u: goto tr1055;
		case 47u: goto tr1056;
		case 48u: goto tr1057;
		case 58u: goto tr1059;
		case 59u: goto tr1060;
		case 60u: goto tr1061;
		case 61u: goto st60;
		case 62u: goto tr1063;
		case 63u: goto tr1064;
		case 91u: goto tr1066;
		case 93u: goto tr1067;
		case 94u: goto tr1068;
		case 95u: goto tr1065;
		case 123u: goto tr1069;
		case 124u: goto st64;
		case 125u: goto tr1071;
		case 126u: goto tr1072;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1058;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1065;
	} else
		goto tr1065;
	goto tr1;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	switch( (*p) ) {
		case 9u: goto tr1073;
		case 10u: goto tr1074;
		case 13u: goto tr1075;
		case 32u: goto tr1073;
		case 33u: goto tr1076;
		case 34u: goto tr1077;
		case 37u: goto tr1078;
		case 38u: goto tr1079;
		case 40u: goto tr1080;
		case 41u: goto tr1081;
		case 42u: goto tr1082;
		case 43u: goto tr1083;
		case 45u: goto tr1084;
		case 46u: goto tr1085;
		case 47u: goto tr1086;
		case 48u: goto tr1087;
		case 58u: goto tr1089;
		case 59u: goto tr1090;
		case 60u: goto tr1091;
		case 61u: goto tr1092;
		case 62u: goto tr1093;
		case 63u: goto tr1094;
		case 91u: goto tr1096;
		case 93u: goto tr1097;
		case 94u: goto tr1098;
		case 95u: goto tr1095;
		case 123u: goto tr1099;
		case 124u: goto tr1100;
		case 125u: goto tr1101;
		case 126u: goto tr1102;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1088;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1095;
	} else
		goto tr1095;
	goto tr1;
tr80:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr111:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr141:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr171:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr201:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr231:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr261:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr291:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr321:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr351:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr381:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr411:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr441:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr471:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr501:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr529:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr561:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr592:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr623:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr653:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr683:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr713:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr743:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr773:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr803:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr833:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr863:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr891:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr921:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr951:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr981:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1011:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1041:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1071:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1101:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1131:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1161:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1191:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1221:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1251:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1281:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1311:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1341:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1371:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1401:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1432:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1462:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1492:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1522:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1552:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1582:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
tr1612:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st61;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
/* #line 11372 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr1103;
		case 10u: goto tr1104;
		case 13u: goto tr1105;
		case 32u: goto tr1103;
		case 33u: goto tr1106;
		case 34u: goto tr1107;
		case 37u: goto tr1108;
		case 38u: goto tr1109;
		case 40u: goto tr1110;
		case 41u: goto tr1111;
		case 42u: goto tr1112;
		case 43u: goto tr1113;
		case 45u: goto tr1114;
		case 46u: goto tr1115;
		case 47u: goto tr1116;
		case 48u: goto tr1117;
		case 58u: goto tr1119;
		case 59u: goto tr1120;
		case 60u: goto tr1121;
		case 61u: goto tr1122;
		case 62u: goto tr1123;
		case 63u: goto tr1124;
		case 91u: goto tr1126;
		case 93u: goto tr1127;
		case 94u: goto tr1128;
		case 95u: goto tr1125;
		case 123u: goto tr1129;
		case 124u: goto tr1130;
		case 125u: goto tr1131;
		case 126u: goto tr1132;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1118;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1125;
	} else
		goto tr1125;
	goto tr1;
tr81:
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr112:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr142:
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr172:
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr202:
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr232:
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr262:
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr292:
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr322:
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr352:
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr382:
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr412:
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr442:
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr472:
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr502:
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr530:
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr562:
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr593:
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr624:
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr654:
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr684:
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr714:
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr744:
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr774:
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr804:
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr834:
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr864:
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr892:
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr922:
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr952:
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr982:
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1012:
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1042:
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1072:
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1102:
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1132:
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1162:
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1192:
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1222:
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1252:
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1282:
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1312:
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1342:
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1372:
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1402:
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1433:
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1463:
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1493:
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1523:
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1553:
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1583:
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
tr1613:
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
/* #line 30 "xec_parser_ragel.rl" */
	{ sloc = (int)( offset + ( p - buffer ) ); data.clear(); }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
/* #line 11741 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 9u: goto tr1133;
		case 10u: goto tr1134;
		case 13u: goto tr1135;
		case 32u: goto tr1133;
		case 33u: goto tr1136;
		case 34u: goto tr1137;
		case 37u: goto tr1138;
		case 38u: goto tr1139;
		case 40u: goto tr1140;
		case 41u: goto tr1141;
		case 42u: goto tr1142;
		case 43u: goto tr1143;
		case 45u: goto tr1144;
		case 46u: goto tr1145;
		case 47u: goto tr1146;
		case 48u: goto tr1147;
		case 58u: goto tr1149;
		case 59u: goto tr1150;
		case 60u: goto tr1151;
		case 61u: goto st63;
		case 62u: goto tr1153;
		case 63u: goto tr1154;
		case 91u: goto tr1156;
		case 93u: goto tr1157;
		case 94u: goto tr1158;
		case 95u: goto tr1155;
		case 123u: goto tr1159;
		case 124u: goto tr1160;
		case 125u: goto tr1161;
		case 126u: goto tr1162;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1148;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1155;
	} else
		goto tr1155;
	goto tr1;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 9u: goto tr1163;
		case 10u: goto tr1164;
		case 13u: goto tr1165;
		case 32u: goto tr1163;
		case 33u: goto tr1166;
		case 34u: goto tr1167;
		case 37u: goto tr1168;
		case 38u: goto tr1169;
		case 40u: goto tr1170;
		case 41u: goto tr1171;
		case 42u: goto tr1172;
		case 43u: goto tr1173;
		case 45u: goto tr1174;
		case 46u: goto tr1175;
		case 47u: goto tr1176;
		case 48u: goto tr1177;
		case 58u: goto tr1179;
		case 59u: goto tr1180;
		case 60u: goto tr1181;
		case 61u: goto tr1182;
		case 62u: goto tr1183;
		case 63u: goto tr1184;
		case 91u: goto tr1186;
		case 93u: goto tr1187;
		case 94u: goto tr1188;
		case 95u: goto tr1185;
		case 123u: goto tr1189;
		case 124u: goto tr1190;
		case 125u: goto tr1191;
		case 126u: goto tr1192;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1178;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1185;
	} else
		goto tr1185;
	goto tr1;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 9u: goto tr1193;
		case 10u: goto tr1194;
		case 13u: goto tr1195;
		case 32u: goto tr1193;
		case 33u: goto tr1196;
		case 34u: goto tr1197;
		case 37u: goto tr1198;
		case 38u: goto tr1199;
		case 40u: goto tr1200;
		case 41u: goto tr1201;
		case 42u: goto tr1202;
		case 43u: goto tr1203;
		case 45u: goto tr1204;
		case 46u: goto tr1205;
		case 47u: goto tr1206;
		case 48u: goto tr1207;
		case 58u: goto tr1209;
		case 59u: goto tr1210;
		case 60u: goto tr1211;
		case 61u: goto tr1212;
		case 62u: goto tr1213;
		case 63u: goto tr1214;
		case 91u: goto tr1216;
		case 93u: goto tr1217;
		case 94u: goto tr1218;
		case 95u: goto tr1215;
		case 123u: goto tr1219;
		case 124u: goto tr1220;
		case 125u: goto tr1221;
		case 126u: goto tr1222;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1208;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1215;
	} else
		goto tr1215;
	goto tr1;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	switch( (*p) ) {
		case 9u: goto tr1223;
		case 10u: goto tr1224;
		case 13u: goto tr1225;
		case 32u: goto tr1223;
		case 33u: goto tr1226;
		case 34u: goto tr1227;
		case 37u: goto tr1228;
		case 38u: goto tr1229;
		case 40u: goto tr1230;
		case 41u: goto tr1231;
		case 42u: goto tr1232;
		case 43u: goto tr1233;
		case 45u: goto tr1234;
		case 46u: goto tr1235;
		case 47u: goto tr1236;
		case 48u: goto tr1237;
		case 58u: goto tr1239;
		case 59u: goto tr1240;
		case 60u: goto tr1241;
		case 61u: goto tr1242;
		case 62u: goto tr1243;
		case 63u: goto tr1244;
		case 91u: goto tr1246;
		case 93u: goto tr1247;
		case 94u: goto tr1248;
		case 95u: goto tr1245;
		case 123u: goto tr1249;
		case 124u: goto tr1250;
		case 125u: goto tr1251;
		case 126u: goto tr1252;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1238;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1245;
	} else
		goto tr1245;
	goto tr1;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 9u: goto tr1253;
		case 10u: goto tr1254;
		case 13u: goto tr1255;
		case 32u: goto tr1253;
		case 33u: goto tr1256;
		case 34u: goto tr1257;
		case 37u: goto tr1258;
		case 38u: goto tr1259;
		case 40u: goto tr1260;
		case 41u: goto tr1261;
		case 42u: goto tr1262;
		case 43u: goto tr1263;
		case 45u: goto tr1264;
		case 46u: goto tr1265;
		case 47u: goto tr1266;
		case 48u: goto tr1267;
		case 58u: goto tr1269;
		case 59u: goto tr1270;
		case 60u: goto tr1271;
		case 61u: goto st67;
		case 62u: goto st68;
		case 63u: goto tr1274;
		case 91u: goto tr1276;
		case 93u: goto tr1277;
		case 94u: goto tr1278;
		case 95u: goto tr1275;
		case 123u: goto tr1279;
		case 124u: goto tr1280;
		case 125u: goto tr1281;
		case 126u: goto tr1282;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1268;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1275;
	} else
		goto tr1275;
	goto tr1;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 9u: goto tr1283;
		case 10u: goto tr1284;
		case 13u: goto tr1285;
		case 32u: goto tr1283;
		case 33u: goto tr1286;
		case 34u: goto tr1287;
		case 37u: goto tr1288;
		case 38u: goto tr1289;
		case 40u: goto tr1290;
		case 41u: goto tr1291;
		case 42u: goto tr1292;
		case 43u: goto tr1293;
		case 45u: goto tr1294;
		case 46u: goto tr1295;
		case 47u: goto tr1296;
		case 48u: goto tr1297;
		case 58u: goto tr1299;
		case 59u: goto tr1300;
		case 60u: goto tr1301;
		case 61u: goto tr1302;
		case 62u: goto tr1303;
		case 63u: goto tr1304;
		case 91u: goto tr1306;
		case 93u: goto tr1307;
		case 94u: goto tr1308;
		case 95u: goto tr1305;
		case 123u: goto tr1309;
		case 124u: goto tr1310;
		case 125u: goto tr1311;
		case 126u: goto tr1312;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1298;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1305;
	} else
		goto tr1305;
	goto tr1;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 9u: goto tr1313;
		case 10u: goto tr1314;
		case 13u: goto tr1315;
		case 32u: goto tr1313;
		case 33u: goto tr1316;
		case 34u: goto tr1317;
		case 37u: goto tr1318;
		case 38u: goto tr1319;
		case 40u: goto tr1320;
		case 41u: goto tr1321;
		case 42u: goto tr1322;
		case 43u: goto tr1323;
		case 45u: goto tr1324;
		case 46u: goto tr1325;
		case 47u: goto tr1326;
		case 48u: goto tr1327;
		case 58u: goto tr1329;
		case 59u: goto tr1330;
		case 60u: goto tr1331;
		case 61u: goto st69;
		case 62u: goto tr1333;
		case 63u: goto tr1334;
		case 91u: goto tr1336;
		case 93u: goto tr1337;
		case 94u: goto tr1338;
		case 95u: goto tr1335;
		case 123u: goto tr1339;
		case 124u: goto tr1340;
		case 125u: goto tr1341;
		case 126u: goto tr1342;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1328;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1335;
	} else
		goto tr1335;
	goto tr1;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 9u: goto tr1343;
		case 10u: goto tr1344;
		case 13u: goto tr1345;
		case 32u: goto tr1343;
		case 33u: goto tr1346;
		case 34u: goto tr1347;
		case 37u: goto tr1348;
		case 38u: goto tr1349;
		case 40u: goto tr1350;
		case 41u: goto tr1351;
		case 42u: goto tr1352;
		case 43u: goto tr1353;
		case 45u: goto tr1354;
		case 46u: goto tr1355;
		case 47u: goto tr1356;
		case 48u: goto tr1357;
		case 58u: goto tr1359;
		case 59u: goto tr1360;
		case 60u: goto tr1361;
		case 61u: goto tr1362;
		case 62u: goto tr1363;
		case 63u: goto tr1364;
		case 91u: goto tr1366;
		case 93u: goto tr1367;
		case 94u: goto tr1368;
		case 95u: goto tr1365;
		case 123u: goto tr1369;
		case 124u: goto tr1370;
		case 125u: goto tr1371;
		case 126u: goto tr1372;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1358;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1365;
	} else
		goto tr1365;
	goto tr1;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 9u: goto tr1373;
		case 10u: goto tr1374;
		case 13u: goto tr1375;
		case 32u: goto tr1373;
		case 33u: goto tr1376;
		case 34u: goto tr1377;
		case 37u: goto tr1378;
		case 38u: goto tr1379;
		case 40u: goto tr1380;
		case 41u: goto tr1381;
		case 42u: goto tr1382;
		case 43u: goto tr1383;
		case 45u: goto tr1384;
		case 46u: goto tr1385;
		case 47u: goto tr1386;
		case 48u: goto tr1387;
		case 58u: goto tr1389;
		case 59u: goto tr1390;
		case 60u: goto tr1391;
		case 61u: goto tr1392;
		case 62u: goto tr1393;
		case 63u: goto tr1394;
		case 91u: goto tr1396;
		case 93u: goto tr1397;
		case 94u: goto tr1398;
		case 95u: goto tr1395;
		case 123u: goto tr1399;
		case 124u: goto tr1400;
		case 125u: goto tr1401;
		case 126u: goto tr1402;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1388;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1395;
	} else
		goto tr1395;
	goto tr1;
tr554:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
/* #line 12151 "xec_parser_ragel.cpp" */
	switch( (*p) ) {
		case 43u: goto tr17;
		case 45u: goto tr17;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr18;
	goto tr1;
tr17:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
/* #line 12167 "xec_parser_ragel.cpp" */
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr18;
	goto tr1;
tr18:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st71;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
/* #line 12179 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr563;
		case 47u: goto tr544;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
	}
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr18;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr553;
	} else
		goto tr553;
	goto tr1;
tr558:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
/* #line 12228 "xec_parser_ragel.cpp" */
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
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st72;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
/* #line 12246 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr1403;
		case 47u: goto tr544;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 80u: goto tr554;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 112u: goto tr554;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
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
				goto tr553;
		} else if ( (*p) >= 97u )
			goto tr19;
	} else
		goto tr553;
	goto tr1;
tr1403:
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
/* #line 12303 "xec_parser_ragel.cpp" */
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
/* #line 31 "xec_parser_ragel.rl" */
	{ data.append( (char)(*p) ); }
	goto st73;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
/* #line 12321 "xec_parser_ragel.cpp" */
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
		case 45u: goto tr542;
		case 46u: goto tr563;
		case 47u: goto tr544;
		case 58u: goto tr547;
		case 59u: goto tr548;
		case 60u: goto tr549;
		case 61u: goto tr550;
		case 62u: goto tr551;
		case 63u: goto tr552;
		case 80u: goto tr554;
		case 91u: goto tr555;
		case 93u: goto tr556;
		case 94u: goto tr557;
		case 95u: goto tr553;
		case 112u: goto tr554;
		case 123u: goto tr559;
		case 124u: goto tr560;
		case 125u: goto tr561;
		case 126u: goto tr562;
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
				goto tr553;
		} else if ( (*p) >= 97u )
			goto tr20;
	} else
		goto tr553;
	goto tr1;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	switch( (*p) ) {
		case 9u: goto tr1404;
		case 10u: goto tr1405;
		case 13u: goto tr1406;
		case 32u: goto tr1404;
		case 33u: goto tr1407;
		case 34u: goto tr1408;
		case 37u: goto tr1409;
		case 38u: goto tr1410;
		case 40u: goto tr1411;
		case 41u: goto tr1412;
		case 42u: goto tr1413;
		case 43u: goto tr1414;
		case 45u: goto tr1415;
		case 46u: goto tr1416;
		case 47u: goto tr1417;
		case 48u: goto tr1418;
		case 58u: goto tr1420;
		case 59u: goto tr1421;
		case 60u: goto tr1422;
		case 61u: goto tr1423;
		case 62u: goto tr1424;
		case 63u: goto tr1425;
		case 91u: goto tr1427;
		case 93u: goto tr1428;
		case 94u: goto tr1429;
		case 95u: goto tr1426;
		case 123u: goto tr1430;
		case 124u: goto tr1431;
		case 125u: goto tr1432;
		case 126u: goto tr1433;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1419;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1426;
	} else
		goto tr1426;
	goto tr1;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	switch( (*p) ) {
		case 9u: goto tr1434;
		case 10u: goto tr1435;
		case 13u: goto tr1436;
		case 32u: goto tr1434;
		case 33u: goto tr1437;
		case 34u: goto tr1438;
		case 37u: goto tr1439;
		case 38u: goto tr1440;
		case 40u: goto tr1441;
		case 41u: goto tr1442;
		case 42u: goto tr1443;
		case 43u: goto tr1444;
		case 45u: goto tr1445;
		case 46u: goto tr1446;
		case 47u: goto tr1447;
		case 48u: goto tr1448;
		case 58u: goto tr1450;
		case 59u: goto tr1451;
		case 60u: goto tr1452;
		case 61u: goto tr1453;
		case 62u: goto tr1454;
		case 63u: goto tr1455;
		case 91u: goto tr1457;
		case 93u: goto tr1458;
		case 94u: goto tr1459;
		case 95u: goto tr1456;
		case 123u: goto tr1460;
		case 124u: goto tr1461;
		case 125u: goto tr1462;
		case 126u: goto tr1463;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1449;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1456;
	} else
		goto tr1456;
	goto tr1;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 9u: goto tr1464;
		case 10u: goto tr1465;
		case 13u: goto tr1466;
		case 32u: goto tr1464;
		case 33u: goto tr1467;
		case 34u: goto tr1468;
		case 37u: goto tr1469;
		case 38u: goto tr1470;
		case 40u: goto tr1471;
		case 41u: goto tr1472;
		case 42u: goto tr1473;
		case 43u: goto tr1474;
		case 45u: goto tr1475;
		case 46u: goto tr1476;
		case 47u: goto tr1477;
		case 48u: goto tr1478;
		case 58u: goto tr1480;
		case 59u: goto tr1481;
		case 60u: goto tr1482;
		case 61u: goto tr1483;
		case 62u: goto tr1484;
		case 63u: goto tr1485;
		case 91u: goto tr1487;
		case 93u: goto tr1488;
		case 94u: goto tr1489;
		case 95u: goto tr1486;
		case 123u: goto tr1490;
		case 124u: goto tr1491;
		case 125u: goto tr1492;
		case 126u: goto tr1493;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1479;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1486;
	} else
		goto tr1486;
	goto tr1;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	switch( (*p) ) {
		case 9u: goto tr1494;
		case 10u: goto tr1495;
		case 13u: goto tr1496;
		case 32u: goto tr1494;
		case 33u: goto tr1497;
		case 34u: goto tr1498;
		case 37u: goto tr1499;
		case 38u: goto tr1500;
		case 40u: goto tr1501;
		case 41u: goto tr1502;
		case 42u: goto tr1503;
		case 43u: goto tr1504;
		case 45u: goto tr1505;
		case 46u: goto tr1506;
		case 47u: goto tr1507;
		case 48u: goto tr1508;
		case 58u: goto tr1510;
		case 59u: goto tr1511;
		case 60u: goto tr1512;
		case 61u: goto tr1513;
		case 62u: goto tr1514;
		case 63u: goto tr1515;
		case 91u: goto tr1517;
		case 93u: goto tr1518;
		case 94u: goto tr1519;
		case 95u: goto tr1516;
		case 123u: goto tr1520;
		case 124u: goto tr1521;
		case 125u: goto tr1522;
		case 126u: goto tr1523;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1509;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1516;
	} else
		goto tr1516;
	goto tr1;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 9u: goto tr1524;
		case 10u: goto tr1525;
		case 13u: goto tr1526;
		case 32u: goto tr1524;
		case 33u: goto tr1527;
		case 34u: goto tr1528;
		case 37u: goto tr1529;
		case 38u: goto tr1530;
		case 40u: goto tr1531;
		case 41u: goto tr1532;
		case 42u: goto tr1533;
		case 43u: goto tr1534;
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
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
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
		case 45u: goto tr1565;
		case 46u: goto tr1566;
		case 47u: goto tr1567;
		case 48u: goto tr1568;
		case 58u: goto tr1570;
		case 59u: goto tr1571;
		case 60u: goto tr1572;
		case 61u: goto tr1573;
		case 62u: goto tr1574;
		case 63u: goto tr1575;
		case 91u: goto tr1577;
		case 93u: goto tr1578;
		case 94u: goto tr1579;
		case 95u: goto tr1576;
		case 123u: goto tr1580;
		case 124u: goto tr1581;
		case 125u: goto tr1582;
		case 126u: goto tr1583;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1569;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1576;
	} else
		goto tr1576;
	goto tr1;
tr3:
/* #line 95 "xec_parser_ragel.rl" */
	{
                    uloc = (int)( offset + ( p - buffer ) );
                }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
/* #line 12650 "xec_parser_ragel.cpp" */
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
/* #line 98 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - '0' ) << 20; }
	goto st14;
tr32:
/* #line 100 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - 'A' + 0x0A ) << 20; }
	goto st14;
tr33:
/* #line 99 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - 'a' + 0x0A ) << 20; }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
/* #line 12693 "xec_parser_ragel.cpp" */
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
/* #line 102 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - '0' ) << 16; }
	goto st15;
tr35:
/* #line 104 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'A' + 0x0A ) << 16; }
	goto st15;
tr36:
/* #line 103 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'a' + 0x0A ) << 16; }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
/* #line 12719 "xec_parser_ragel.cpp" */
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
/* #line 106 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - '0' ) << 12; }
	goto st16;
tr38:
/* #line 108 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'A' + 0x0A ) << 12; }
	goto st16;
tr39:
/* #line 107 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'a' + 0x0A ) << 12; }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
/* #line 12745 "xec_parser_ragel.cpp" */
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
/* #line 110 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - '0' ) << 8; }
	goto st17;
tr41:
/* #line 112 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'A' + 0x0A ) << 8; }
	goto st17;
tr42:
/* #line 111 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'a' + 0x0A ) << 8; }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
/* #line 12771 "xec_parser_ragel.cpp" */
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
/* #line 114 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - '0' ) << 4; }
	goto st18;
tr44:
/* #line 116 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'A' + 0x0A ) << 4; }
	goto st18;
tr45:
/* #line 115 "xec_parser_ragel.rl" */
	{ temp |= ( (*p) - 'a' + 0x0A ) << 4; }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
/* #line 12797 "xec_parser_ragel.cpp" */
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
/* #line 85 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - '0' ) << 4; }
	goto st20;
tr50:
/* #line 87 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - 'A' + 0x0A ) << 4; }
	goto st20;
tr51:
/* #line 86 "xec_parser_ragel.rl" */
	{ temp = ( (*p) - 'a' + 0x0A ) << 4; }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
/* #line 12836 "xec_parser_ragel.cpp" */
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr52;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr54;
	} else
		goto tr53;
	goto tr1;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 9u: goto tr1584;
		case 10u: goto tr1585;
		case 13u: goto tr1586;
		case 32u: goto tr1584;
		case 33u: goto tr1587;
		case 34u: goto tr1588;
		case 37u: goto tr1589;
		case 38u: goto tr1590;
		case 40u: goto tr1591;
		case 41u: goto tr1592;
		case 42u: goto tr1593;
		case 43u: goto tr1594;
		case 45u: goto tr1595;
		case 46u: goto tr1596;
		case 47u: goto tr1597;
		case 48u: goto tr1598;
		case 58u: goto tr1600;
		case 59u: goto tr1601;
		case 60u: goto tr1602;
		case 61u: goto tr1603;
		case 62u: goto tr1604;
		case 63u: goto tr1605;
		case 91u: goto tr1607;
		case 93u: goto tr1608;
		case 94u: goto tr1609;
		case 95u: goto tr1606;
		case 123u: goto tr1610;
		case 124u: goto tr1611;
		case 125u: goto tr1612;
		case 126u: goto tr1613;
	}
	if ( (*p) < 65u ) {
		if ( 49u <= (*p) && (*p) <= 57u )
			goto tr1599;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr1606;
	} else
		goto tr1606;
	goto tr1;
tr96:
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
/* #line 12899 "xec_parser_ragel.cpp" */
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
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
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
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 24: 
	case 25: 
/* #line 29 "xec_parser_ragel.rl" */
	{ newlines.push_back( (int)( offset + ( p - buffer ) ) ); }
	break;
	case 53: 
/* #line 151 "xec_parser_ragel.rl" */
	{
                    xec_token_lookup lookup = lookup_identifier( &data );
                    TOKEN( lookup.kind, lookup.hash, lookup.text, lookup.size );
                }
	break;
	case 40: 
	case 41: 
	case 43: 
	case 71: 
	case 72: 
	case 73: 
/* #line 158 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* number = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_NUMBER, sloc, number, length );
                }
	break;
	case 27: 
/* #line 167 "xec_parser_ragel.rl" */
	{
                    size_t length = data.size();
                    data.append( '\0' );
                    const char* string = (const char*)data.tearoff();
                    TOKEN( XEC_TOKEN_STRING, sloc, string, length );
                }
	break;
	case 26: 
/* #line 174 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_XMARK, sloc ); }
	break;
	case 28: 
/* #line 175 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERCENT, sloc ); }
	break;
	case 29: 
/* #line 176 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_AMPERSAND, sloc ); }
	break;
	case 31: 
/* #line 177 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LPN, sloc ); }
	break;
	case 32: 
/* #line 178 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RPN, sloc ); }
	break;
	case 33: 
/* #line 179 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASTERISK, sloc ); }
	break;
	case 34: 
/* #line 180 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PLUS, sloc ); }
	break;
	case 36: 
/* #line 181 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MINUS, sloc ); }
	break;
	case 38: 
/* #line 182 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_PERIOD, sloc ); }
	break;
	case 39: 
/* #line 183 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SOLIDUS, sloc ); }
	break;
	case 42: 
/* #line 184 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_COLON, sloc ); }
	break;
	case 44: 
/* #line 185 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SEMICOLON, sloc ); }
	break;
	case 45: 
/* #line 186 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESS, sloc ); }
	break;
	case 48: 
/* #line 187 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ASSIGN, sloc ); }
	break;
	case 50: 
/* #line 188 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATER, sloc ); }
	break;
	case 52: 
/* #line 189 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_QMARK, sloc ); }
	break;
	case 54: 
/* #line 190 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSQ, sloc ); }
	break;
	case 55: 
/* #line 191 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSQ, sloc ); }
	break;
	case 56: 
/* #line 192 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_CARET, sloc ); }
	break;
	case 58: 
/* #line 193 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LBR, sloc ); }
	break;
	case 59: 
/* #line 194 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_VBAR, sloc ); }
	break;
	case 61: 
/* #line 195 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RBR, sloc ); }
	break;
	case 62: 
/* #line 196 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_TILDE, sloc ); }
	break;
	case 35: 
/* #line 198 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INCREMENT, sloc ); }
	break;
	case 37: 
/* #line 199 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DECREMENT, sloc ); }
	break;
	case 46: 
/* #line 201 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFT, sloc ); }
	break;
	case 66: 
/* #line 202 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFT, sloc ); }
	break;
	case 68: 
/* #line 203 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFT, sloc ); }
	break;
	case 80: 
/* #line 205 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_NOTEQUAL, sloc ); }
	break;
	case 70: 
/* #line 206 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LESSEQUAL, sloc ); }
	break;
	case 49: 
/* #line 207 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_EQUAL, sloc ); }
	break;
	case 51: 
/* #line 208 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_GREATEREQUAL, sloc ); }
	break;
	case 79: 
/* #line 210 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MODASSIGN, sloc ); }
	break;
	case 78: 
/* #line 211 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITANDASSIGN, sloc ); }
	break;
	case 77: 
/* #line 212 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_MULASSIGN, sloc ); }
	break;
	case 76: 
/* #line 213 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_ADDASSIGN, sloc ); }
	break;
	case 75: 
/* #line 214 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_SUBASSIGN, sloc ); }
	break;
	case 74: 
/* #line 215 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_DIVASSIGN, sloc ); }
	break;
	case 57: 
/* #line 216 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITXORASSIGN, sloc ); }
	break;
	case 60: 
/* #line 217 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_BITORASSIGN, sloc ); }
	break;
	case 63: 
/* #line 218 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_INTDIVASSIGN, sloc ); }
	break;
	case 47: 
/* #line 219 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LSHIFTASSIGN, sloc ); }
	break;
	case 67: 
/* #line 220 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_RSHIFTASSIGN, sloc ); }
	break;
	case 69: 
/* #line 221 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_URSHIFTASSIGN, sloc ); }
	break;
	case 30: 
/* #line 223 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICAND, sloc ); }
	break;
	case 65: 
/* #line 224 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICXOR, sloc ); }
	break;
	case 64: 
/* #line 225 "xec_parser_ragel.rl" */
	{ TOKEN( XEC_TOKEN_LOGICOR, sloc ); }
	break;
	case 1: 
	case 2: 
	case 3: 
	case 4: 
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
/* #line 34 "xec_parser_ragel.rl" */
	{
        int sloc = (int)( offset + ( p - buffer ) );
        diagnostic( sloc, "unexpected end of file" );
        {p++; cs = 0; goto _out;}
    }
/* #line 41 "xec_parser_ragel.rl" */
	{
        int sloc = (int)( offset + ( p - buffer ) );
        if ( (*p) >= 0x20 && (*p) <= 0x7E )
            diagnostic( sloc, "unexpected character '%c'", (*p) );
        else
            diagnostic( sloc, "unexpected character '\\x%02X'", (*p) );
        if ( diagnostics.size() >= ERROR_LIMIT )
            goto error;
    }
/* #line 231 "xec_parser_ragel.rl" */
	{ {goto st22;} }
	break;
/* #line 13254 "xec_parser_ragel.cpp" */
	}
	}

	_out: {}
	}

/* #line 386 "xec_parser_ragel.rl" */
        
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





