//
//  diagnostics.h
//
//  Created by Edmund Kapusniak on 23/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef SRCLOC_H
#define SRCLOC_H

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "yl/ylang.h"
#include "yl/compiler.h"

namespace yl
{

/*
    Source locations are represented as a single character index.  We keep
    track of the start index of each line, allowing mapping a source location
    back to line and column.
*/

typedef uint32_t srcloc;

struct line_column
{
    unsigned line;
    unsigned column;
};

/*
    Tracks diagnostics and source location line information during compilation.
*/

struct diagnostics
{
    diagnostics();
    ~diagnostics();

    void set_filename( std::string_view filename );
    void newline( srcloc srcloc );
    struct line_column line_column( srcloc srcloc ) const;

    void error( srcloc srcloc, const char* message, ... ) PRINTF_FORMAT( 3, 4 );

    std::string _filename;
    std::vector< srcloc > _newlines;
    std::vector< diagnostic > _diagnostics;
};

}

#endif

