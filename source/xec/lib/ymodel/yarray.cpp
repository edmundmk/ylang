//
//  yarray.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yarray.h"



ymetatype yarray::metatype = { &mark_array, "array" };



yarray* yarray::alloc()
{
    void* p = malloc( sizeof( yarray ) );
    return new ( p ) yarray( &metatype );
}


yarray::yarray( ymetatype* metatype )
    :   yexpand( metatype, nullptr )
{
    // TODO: Use a class that inherits from the actual array prototype.
}


void yarray::mark_array( yobject* object, yworklist* work, ycolour colour )
{
    yarray* array = (yarray*)object;
    mark_expand( array, work, colour );
    ymark( array->values, work, colour );
}



