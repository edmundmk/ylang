//
//  yl_invoke.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_invoke.h"
#include "yl_function.h"



void yl_invoke::acquire()
{
    if ( _function )
    {
        _function->acquire();
    }
}

void yl_invoke::release()
{
    if ( _function )
    {
        _function->release();
    }
    _function = nullptr;
}
