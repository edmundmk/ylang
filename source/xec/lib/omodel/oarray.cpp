//
//  oarray.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oarray.h"



ometatype oarray::metatype = { &mark_array, "array" };


void oarray::mark_array( oworklist* work, obase* object, ocolour colour )
{
    oarray* a = (oarray*)object;
    mark_expand( work, object, colour );
    omark< otuple< ovalue >* >::mark( a->values, work, colour );
}



