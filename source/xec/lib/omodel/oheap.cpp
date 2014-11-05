//
//  oheap.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oheap.h"



__thread ocontext* ocontext::context = nullptr;


ostring* oheap::make_symbol( const char* s )
{
    return NULL;
}


ostring* oheap::make_symbol( ostring* s )
{
    return NULL;
}



void oheap::mark_grey( obase* object )
{
    // TODO: Add object to work list.
}


