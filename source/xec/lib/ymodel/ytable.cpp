//
//  ytable.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ytable.h"



ymetatype ytable::metatype = { &mark_table, "table" };


ytable* ytable::alloc()
{
    void* p = malloc( sizeof( ytable ) );
    return new ( p ) ytable( &metatype );
}


ytable::ytable( ymetatype* metatype )
    :   yexpand( metatype, nullptr )
{
    // TODO: Use a table class inheriting from the table prototype.
}


void ytable::mark_table( yobject* object, yworklist* work, ycolour colour )
{
    ytable* table = (ytable*)object;
    mark_expand( table, work, colour );
    ymark( table->table, work, colour );
}







