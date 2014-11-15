//
//  yl_script.cpp
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_script.h"
#include <stringf.h>
#include "yl_parser.h"




yl_script::yl_script()
{
}



const char* yl_script::get_filename()
{
    return filename.c_str();
}

int yl_script::get_line( int sloc )
{
    auto i = std::lower_bound( newlines.begin(), newlines.end(), sloc );
    return (int)( i - newlines.begin() ) + 1;
}

int yl_script::get_column( int sloc )
{
    int line_start = 0;
    auto i = std::upper_bound( newlines.begin(), newlines.end(), sloc );
    if ( i != newlines.begin() )
        line_start = *( i - 1 );
    return sloc - line_start;
}



void yl_script::error( int sloc, const char* format, ... )
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

size_t yl_script::error_count()
{
    return errors.size();
}

const char* yl_script::error( size_t index )
{
    return errors.at( index ).c_str();
}



yl_ast* yl_script::get_ast()
{
    return ast.get();
}







