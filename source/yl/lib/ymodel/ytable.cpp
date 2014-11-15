//
//  ytable.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ytable.h"
#include "ythunk.h"



ymetatype ytable::metatype = { &mark_table, "table" };


ytable* ytable::alloc()
{
    void* p = malloc( sizeof( ytable ) );
    return new ( p ) ytable( &metatype );
}


ytable::ytable( ymetatype* metatype )
    :   yexpand( metatype, yscope::scope->model->table_class() )
{
}


static void ytable_length( yframe& frame )
{
    ytable* table = frame[ 0 ].as< ytable >();
    frame.result( (uint32_t)table->length() );
}


yexpand* ytable::make_proto()
{
    yexpand* proto = yexpand::alloc();
    proto->setkey( "length", ytable_length );
    return proto;
}


void ytable::mark_table( yobject* object, yworklist* work, ycolour colour )
{
    ytable* table = (ytable*)object;
    mark_expand( table, work, colour );
    ymark( table->table, work, colour );
}







