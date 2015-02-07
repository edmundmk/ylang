//
//  yl_diagnostics.cpp
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_diagnostics.h"
#include <stringf.h>




void yl_diagnostics::set_filename( const char* filename )
{
    this->filename = filename;
}

void yl_diagnostics::newline( int sloc )
{
    newlines.push_back( sloc );
}



const char* yl_diagnostics::get_filename()
{
    return filename.c_str();
}

int yl_diagnostics::get_line( int sloc )
{
    auto i = std::lower_bound( newlines.begin(), newlines.end(), sloc );
    return (int)( i - newlines.begin() ) + 1;
}

int yl_diagnostics::get_column( int sloc )
{
    int line_start = 0;
    auto i = std::upper_bound( newlines.begin(), newlines.end(), sloc );
    if ( i != newlines.begin() )
        line_start = *( i - 1 );
    return sloc - line_start;
}



void yl_diagnostics::error( int sloc, const char* format, ... )
{
    va_list arguments;
    va_list vlist;
    va_start( arguments, format );

    std::string error;
    error += stringf( "%s:%d:%d: ",
                    get_filename(), get_line( sloc ), get_column( sloc ) );
    error += vstringf( format, arguments );
    errors.push_back( error );

    va_end( vlist );
}

size_t yl_diagnostics::error_count()
{
    return errors.size();
}

const char* yl_diagnostics::error( size_t index )
{
    return errors.at( index ).c_str();
}






