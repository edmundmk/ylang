//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_heap.h"




yl_context::yl_context()
    :   _heap( new yl_heap() )
{
}

yl_context::~yl_context()
{
}



yl_scope::yl_scope( yl_context* context )
    :   _heap( context->_heap.get() )
    ,   _prev( yl_heap_current )
{
    yl_heap_current = _heap;
}

yl_scope::~yl_scope()
{
    assert( yl_heap_current == _heap );
    yl_heap_current = _prev;
}


