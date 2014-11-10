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
    ,   count( 0 )
{
    // TODO: Use a class that inherits from the actual array prototype.
}



void yarray::append( yvalue value )
{
    if ( ! values || count >= values->size() )
    {
        size_t newsize = values ? values->size() * 2 : 8;
        ytuple< yvalue >* newvalues = ytuple< yvalue >::alloc( newsize );
        for ( size_t i = 0; i < count; ++i )
        {
            newvalues->set( i, values->get( i ) );
        }
        values = newvalues;
    }
    values->set( count, value );
    count += 1;
}




void yarray::mark_array( yobject* object, yworklist* work, ycolour colour )
{
    yarray* array = (yarray*)object;
    mark_expand( array, work, colour );
    ymark( array->values, work, colour );
}



