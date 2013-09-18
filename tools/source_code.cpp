//
// source_code.cpp
//
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#include "source_code.h"
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <path.h>
#include <stringf.h>


token::token( int kind, size_t offset, size_t length )
	:	kind( kind )
	,	offset( offset )
	,	length( length )
{
}


source_chunk::source_chunk( size_t offset, size_t length, const char* text )
	:	offset( offset )
	,	length( length )
	,	text( text )
{
}



source_code::source_code( const char* path )
	:	filename( path::get_filename( path ) )
	,	text( NULL )
	,	length( 0 )
	,	offset( 0 )
{
    // Read entire file.
    FILE* f = fopen( path, "r" );
    fseek( f, 0, SEEK_END );
    long fsize = ftell( f );
    fseek( f, 0, SEEK_SET );
    text = (char*)malloc( fsize );
	length = fread( text, 1, fsize, f );
    fclose( f );

	// Line 1 is at offset 0.
    newlines.push_back( 0 );
}

source_code::~source_code()
{
    free( text );
}

const char* source_code::spelling( const token& token )
{
	return text + token.offset;
}

std::string source_code::diagnostic( size_t offset, const char* format, ... )
{
    // Find line and column.
	auto newline = std::lower_bound(
			newlines.begin(), newlines.end(), offset );
	int line   = (int)( newline - newlines.begin() ) + 1;
    int column = (int)( offset - *newline );

	// Format diagnostic.
	va_list arguments;
	va_start( arguments, format );
	std::string message = vstringf( format, arguments );
    va_end( arguments )

	// Return diagnostic with location.
    return stringf( "%s:%s:%s: %s",
			filename.c_str(), line, column, message.c_str() );
}

source_chunk source_code::next_chunk()
{
	source_chunk chunk( offset, length - offset, text + offset );
	offset += chunk.length;
	return chunk;
}

void source_code::parsed_spelling( const token& token )
{
	// Entire file is in memory so no need to preserve token spelling.
}

void source_code::parsed_newline( size_t offset )
{
	assert( offset > newlines.back() );
	newlines.push_back( offset );
}

