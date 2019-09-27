//
//  diagnostics.cpp
//
//  Created by Edmund Kapusniak on 23/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "diagnostics.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

namespace yl
{

diagnostics::diagnostics()
{
    _newlines.push_back( 0 );
}

diagnostics::~diagnostics()
{
}

void diagnostics::set_filename( std::string_view filename )
{
    _filename = std::string( filename );
}

void diagnostics::newline( srcloc srcloc )
{
    assert( srcloc >= _newlines.back() );
    _newlines.push_back( srcloc );
}

line_column diagnostics::line_column( srcloc srcloc ) const
{
    auto i = --std::upper_bound( _newlines.begin(), _newlines.end(), srcloc );
    return { (unsigned)( i - _newlines.begin() + 1 ), (unsigned)( srcloc - *i + 1 ) };
}

void diagnostics::error( srcloc srcloc, const char* message, ... )
{
    // Format message.
    va_list vap, ap;
    va_start( vap, message );
    va_copy( ap, vap );
    int size = vsnprintf( nullptr, 0, message, ap );
    va_end( ap );
    std::string text( size + 1, '\0' );
    vsnprintf( text.data(), text.size(), message, vap );
    va_end( vap );
    text.pop_back();

    // Add diagnostic.
    struct line_column lc = line_column( srcloc );
    _diagnostics.push_back( { DIAGNOSTIC_ERROR, lc.line, lc.column, std::move( text ) } );
}

}

