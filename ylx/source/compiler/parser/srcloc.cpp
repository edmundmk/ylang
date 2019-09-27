//
//  srcloc.cpp
//
//  Created by Edmund Kapusniak on 23/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "srcloc.h"
#include <assert.h>
#include <algorithm>

namespace yl
{

srcloc_table::srcloc_table()
{
    newlines.push_back( 0 );
}

srcloc_table::~srcloc_table()
{
}

void srcloc_table::newline( srcloc srcloc )
{
    assert( srcloc >= newlines.back() );
    newlines.push_back( srcloc );
}

line_column srcloc_table::line_column( srcloc srcloc ) const
{
    auto i = --std::upper_bound( newlines.begin(), newlines.end(), srcloc );
    return { (unsigned)( i - newlines.begin() + 1 ), (unsigned)( srcloc - *i + 1 ) };
}

}

