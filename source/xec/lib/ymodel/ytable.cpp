//
//  ytable.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ytable.h"



ometatype otable::metatype = { &mark_table, "table" };


otable* otable::alloc()
{
    void* p = malloc( sizeof( otable ) );
    return new ( p ) otable( &metatype );
}


otable::otable( ometatype* metatype )
    :   oexpand( metatype, ocontext::context->empty )
{
    // TODO: Use a table class inheriting from the table prototype.
}


void otable::mark_table( oworklist* work, obase* object, ocolour colour )
{
    otable* t = (otable*)object;
    mark_expand( work, object, colour );
    omark< okeytable< ovalue, ovalue > >::mark( t->table, work, colour );
}







