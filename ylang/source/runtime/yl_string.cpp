//
//  yl_string.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_string.h"



yl_string::yl_string( size_t size )
    :   yl_heapobj( YL_STRING )
    ,   _is_symbol( false )
    ,   _has_hash( false )
    ,   _hash( 0 )
    ,   _size( (uint32_t)size )
{
}



