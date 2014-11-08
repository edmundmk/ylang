//
//  yvalue.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yvalue.h"



yvalue yvalue::undefined( VALUE_UNDEFINED );

yobject* yvalue::get() const
{
    if ( x >= MIN_REFERENCE && x <= MAX_REFERENCE )
    {
        return (yobject*)( x & POINTER_MASK );
    }
    else
    {
        return nullptr;
    }
}






