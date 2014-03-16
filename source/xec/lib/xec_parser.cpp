//
//  xec_parser.cpp
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_parser.h"


void* XecParseAlloc( void* (*alloc)( size_t ) );
void  XecParse( void* lemon, int kind, xec_token* token, xec_parser* parser );
void  XecParseFree( void* lemon, void (*free)( void* ) );



xec_parser::xec_parser()
{
}

xec_parser::xec_parser( const char* filename )
{
    parse( filename );
}

xec_parser::~xec_parser()
{
}



const char* xec_parser::get_filename()
{
    return filename.c_str();
}

int xec_parser::get_line( int sloc )
{
    auto i = std::lower_bound( newlines.begin(), newlines.end(), sloc );
    return (int)( i - newlines.begin() ) + 1;
}

int xec_parser::get_column( int sloc )
{
    int line_start = 0;
    auto i = std::upper_bound( newlines.begin(), newlines.end(), sloc );
    if ( i != newlines.begin() )
        line_start = *( i - 1 );
    return sloc - line_start;
}



void xec_parser::diagnostic( int sloc, const char* format, ... )
{
    va_list arguments;
    va_list vlist;
    va_start( arguments, format );

    int bufsize = snprintf( NULL, 0, "%s:%d:%d: ",
            get_filename(), get_line( sloc ), get_column( sloc ) );
    va_copy( vlist, arguments );
    bufsize += vsnprintf( NULL, 0, format, vlist );
    va_end( vlist );
    
    char* buffer = (char*)alloc.malloc( bufsize + 1 );
    int printsize = snprintf( buffer, bufsize, "%s:%d:%d ",
            get_filename(), get_line( sloc ), get_column( sloc ) );
    va_copy( vlist, arguments );
    vsnprintf( buffer + printsize, bufsize - printsize, format, vlist );
    va_end( vlist );

    diagnostics.push_back( buffer );
}

size_t xec_parser::diagnostic_count()
{
    return diagnostics.size();
}

const char* xec_parser::diagnostic( size_t index )
{
    return diagnostics[ index ];
}





