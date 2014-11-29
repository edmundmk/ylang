//
//  yfunction.cpp
//
//  Created by Edmund Kapusniak on 09/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yfunction.h"
#include "ymodule.h"
#include "yvm.h"





yframe::yframe()
    :   s( nullptr )
    ,   limit( nullptr )
    ,   count( 0 )
{
    // The stack mark is at the top of the stack.  Start pushing arguments
    // one after this, to leave space for the function.
    ystack* stack = yscope::scope->stack;
    s = stack->stack.data() + stack->mark + 1;
    limit = stack->stack.data() + stack->stack.size();
}


void yframe::grow_stack()
{
    ystack* stack = yscope::scope->stack;
    size_t sp = s - stack->stack.data();
    stack->ensure_stack( sp + 16 );
    s = stack->stack.data() + sp;
    limit = stack->stack.data() + stack->stack.size();
}






ymetatype yupval::metatype = { &mark_upval, "upval" };


yupval* yupval::alloc( yvalue value )
{
    void* p = malloc( sizeof( yupval ) );
    return new ( p ) yupval( &metatype, value );
}


yupval::yupval( ymetatype* metatype, yvalue value )
    :   yobject( metatype, sizeof( yupval ) )
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
    :   yobject( metatype, sizeof( yfunction ) )
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






void yinvoke( yfunction* function, yframe& frame )
{
    ystack* stack = yscope::scope->stack;

    // Stack mark is the stack top, where yframe should have been built.
    size_t fp = stack->mark;
    yvalue* s = stack->stack.data() + fp;
    s[ 0 ] = function;

    // Execute function.
    yexec( stack->mark, (unsigned)( frame.s - s ), Y_MARK );
    s = stack->stack.data() + fp;
    ysafepoint();

    // On exit, frame points to the element one before the first result.
    yvalue* limit = stack->stack.data() + stack->stack.size();
    frame = yframe( s - 1, limit, stack->mark - fp );
    stack->mark = fp;
}



