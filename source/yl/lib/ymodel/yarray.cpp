//
//  yarray.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yarray.h"
#include "yfunction.h"



ymetatype yarray::metatype = { &mark_array, "array" };



yarray* yarray::alloc()
{
    return alloc( yscope::scope->model->array_proto() );
}

yarray* yarray::alloc( yexpand* prototype )
{
    void* p = malloc( sizeof( yarray ) );
    return new ( p ) yarray( &metatype, prototype->empty_class() );
}


yarray::yarray( ymetatype* metatype, yclass* klass )
    :   yexpand( metatype, klass )
    ,   count( 0 )
{
}


static void yarray_length( yframe& frame )
{
    yarray* array = frame[ 1 ].as< yarray >();
    frame.reset();
    frame.push( (uint32_t)array->length() );
}

static void yarray_resize( yframe& frame )
{
    yarray* array = frame[ 1 ].as< yarray >();
    array->resize( (size_t)frame[ 2 ].as_number() );
    frame.reset();
}


yexpand* yarray::make_proto()
{
    yexpand* proto = yexpand::alloc();
    proto->setkey( "length", yarray_length );
    proto->setkey( "resize", yarray_resize );
    return proto;
}



void yarray::append( yvalue value )
{
    ensure( count + 1 );
    values->set( count, value );
    count += 1;
}


void yarray::resize( size_t newlength )
{
    ensure( newlength );
    for ( size_t i = newlength; i < count; ++i )
    {
        values->set( i, yvalue::yundefined );
    }
    for ( size_t i = count; i < newlength; ++i )
    {
        values->set( i, yvalue::ynull );
    }
    count = newlength;
}


void yarray::ensure( size_t capacity )
{
    if ( ! values || capacity > values->size() )
    {
        size_t newsize = std::max( values ? values->size() * 2 : 8, capacity );
        ytuple< yvalue >* newvalues = ytuple< yvalue >::alloc( newsize );
        for ( size_t i = 0; i < count; ++i )
        {
            newvalues->set( i, values->get( i ) );
        }
        values = newvalues;
    }
}



void yarray::mark_array( yobject* object, yworklist* work, ycolour colour )
{
    yarray* array = (yarray*)object;
    mark_expand( array, work, colour );
    ymark( array->values, work, colour );
}



