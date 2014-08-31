//
//  xec_source_writer.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_source_writer.h"
#include <stdio.h>


xec_source_writer::xec_source_writer()
    :   mindent( 0 )
{
}


void xec_source_writer::keyword( const char* text )
{
    printf( "%s ", text );
}

void xec_source_writer::identifier( const char* text )
{
    printf( "%s ", text );
}

void xec_source_writer::symbol( const char* text )
{
    printf( "%s ", text );
}

void xec_source_writer::open( const char* brace )
{
    printf( "%s ", brace );
}

void xec_source_writer::close( const char* brace )
{
    printf( "%s ", brace );
}

void xec_source_writer::string( const char* string, size_t length )
{
    printf( "\"" );
    for ( size_t i = 0; i < length; ++i )
    {
        char c = string[ i ];
        if ( c >= 0x20 && c <= 0x7E )
        {
            printf( "%c", c );
        }
        else switch ( c )
        {
        case 0x07:  printf( "\\a" );        break;
        case 0x08:  printf( "\\b" );        break;
        case 0x09:  printf( "\\t" );        break;
        case 0x0A:  printf( "\\n" );        break;
        case 0x0B:  printf( "\\v" );        break;
        case 0x0C:  printf( "\\f" );        break;
        case 0x0D:  printf( "\\r" );        break;
        case '"':   printf( "\\\"" );       break;
        default:    printf( "\\x%02X", c ); break;
        }
    }
    printf( "\" " );
}

void xec_source_writer::newline()
{
    printf( "\n%*s", mindent, "" );
}

void xec_source_writer::indent()
{
    mindent += 4;
    newline();
}

void xec_source_writer::outdent()
{
    mindent -= 4;
    newline();
}


