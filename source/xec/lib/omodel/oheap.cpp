//
//  oheap.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oheap.h"



__thread ocontext* ocontext::context = nullptr;



void oheap::mark_grey( obase* object )
{
    // TODO: Add object to work list.
}


