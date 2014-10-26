//
//  ovalue.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ovalue.h"



ovalue ovalue::undefined( ovalue::VALUE_UNDEFINED );


void* ovalue::get() const
{
    return (void*)( x & POINTER_MASK );
}


