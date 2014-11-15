//
//  yvalue.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yvalue.h"


yvalue yvalue::yundefined   = yvalue::value( VALUE_UNDEFINED );
yvalue yvalue::ynull        = yvalue::value( VALUE_NULL );
yvalue yvalue::yfalse       = yvalue::value( VALUE_FALSE );
yvalue yvalue::ytrue        = yvalue::value( VALUE_TRUE );


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






