//
//  oarray.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oarray.h"



ometatype oarray::metatype = { &mark_array, "array" };



oarray* oarray::alloc()
{
    void* p = malloc( sizeof( oarray ) );
    return new ( p ) oarray( &metatype );
}


oarray::oarray( ometatype* metatype )
    :   oexpand( metatype, ocontext::context->empty )
{
    // TODO: Use a class that inherits from the actual array prototype.
}


void oarray::mark_array( oworklist* work, obase* object, ocolour colour )
{
    oarray* a = (oarray*)object;
    mark_expand( work, object, colour );
    omark< otuple< ovalue >* >::mark( a->values, work, colour );
}



