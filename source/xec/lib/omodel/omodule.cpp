//
//  omodule.cpp
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "omodule.h"



ometatype omodule::metatype = { &mark_module, "module" };


omodule* omodule::alloc()
{
    void* p = malloc( sizeof( omodule ) );
    return new ( p ) omodule( &metatype );
}


omodule::omodule( ometatype* metatype )
    :   obase( metatype )
{
}


void omodule::mark_module( oworklist* work, obase* object, ocolour colour )
{
    omodule* module = (omodule*)object;
    omarkwb( module->mname, work, colour );
    omarkwb( module->symbols, work, colour );
    omarkwb( module->values, work, colour );
    omarkwb( module->functions, work, colour );
}




ometatype ofunction::metatype = { &mark_function, "function" };

ofunction* ofunction::alloc( size_t size )
{
    void* p = malloc( sizeof( ofunction ) + sizeof( oinstruction ) * size );
    return new ( p ) ofunction( &metatype, size );
}


ofunction::ofunction( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   fparamcount( 0 )
    ,   fupvalcount( 0 )
    ,   fnewupcount( 0 )
    ,   fvarargs( false )
    ,   fcoroutine( false )
    ,   fsize( size )
{
    for ( size_t i = 0; i < size; ++i )
    {
        new ( fcode + i ) oinstruction();
    }
}


void ofunction::mark_function( oworklist* work, obase* object, ocolour colour )
{
    ofunction* f = (ofunction*)object;
    omarkwb( f->fmodule, work, colour );
    omarkwb( f->fname, work, colour );
}






void omodule::print( omodule* module )
{
}


