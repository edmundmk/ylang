//
//  srcloc.h
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

namespace yl
{

typedef uint32_t srcloc;

struct line_column
{
    unsigned line;
    unsigned column;
};

class srcloc_table
{
public:

    srcloc_table();
    ~srcloc_table();

    void newline( srcloc srcloc );
    line_column line_column( srcloc srcloc ) const;

private:

    std::vector< srcloc > newlines;

};

}

#endif

