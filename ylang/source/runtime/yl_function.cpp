//
//  yl_function.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_function.h"



void yl_function::acquire()
{
    assert( _refcount < 255 );
    _refcount += 1;
    if ( _refcount == 1 )
    {
        // TODO: add to root set.
    }
}

void yl_function::release()
{
    assert( _refcount > 0 );
    _refcount -= 1;
    if ( _refcount == 0 )
    {
        // TODO: remove from root set.
    }
}