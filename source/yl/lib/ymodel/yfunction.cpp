//
//  yfunction.cpp
//
//  Created by Edmund Kapusniak on 09/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yfunction.h"
#include "ymodule.h"
#include "yvm.h"



ymetatype yupval::metatype = { &mark_upval, "upval" };


yupval* yupval::alloc( yvalue value )
{
    void* p = malloc( sizeof( yupval ) );
    return new ( p ) yupval( &metatype, value );
}


yupval::yupval( ymetatype* metatype, yvalue value )
    :   yobject( metatype )
    ,   value( value )
{
}



void yupval::mark_upval( yobject* object, yworklist* work, ycolour colour )
{
    yupval* upval = (yupval*)object;
    ymark( upval->value, work, colour );
}





ymetatype yfunction::metatype = { &mark_function, "function" };


yfunction* yfunction::alloc( yexpand* global, yrecipe* recipe )
{
    void* p = malloc( sizeof( yfunction ) );
    return new ( p ) yfunction( &metatype, global, recipe );
}


yfunction::yfunction( ymetatype* metatype, yexpand* global, yrecipe* recipe )
    :   yexpand( metatype, yscope::scope->model->function_proto()->empty_class() )
    ,   fglobal( global )
    ,   frecipe( recipe )
    ,   fupvals( ytuple< yupval* >::alloc( recipe->upval_count() ) )
{
}


void yfunction::mark_function( yobject* object, yworklist* work, ycolour colour )
{
    yfunction* function = (yfunction*)object;
    ymark( function->fglobal, work, colour );
    ymark( function->frecipe, work, colour );
    ymark( function->fupvals, work, colour );
}




yexpand* yfunction::make_proto()
{
    return yexpand::alloc();
}











void yinvoke( yfunction* function )
{
    ystack* stack = yscope::scope->stack;
    stack->ensure_stack( 16 );
    stack->stack[ 0 ] = function;
    yexec( 0, 0, 0 );
}



