//
//  oslotlist.cpp
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oslotlist.h"



ometatype oslotlist::metatype = { &mark_slotlist, "slotlist" };



oslotlist* oslotlist::alloc( size_t size )
{
    void* p = malloc( sizeof( oslotlist ) + sizeof( slot ) * size );
    return new ( p ) oslotlist( &metatype, size );
}


oslotlist::oslotlist( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   ssize( (uint32_t)size )
    ,   smark( 0 )
{
    for ( size_t i = 0; i < ssize; ++i )
    {
        new ( slots + i ) slot();
    }
}


void oslotlist::mark_slotlist( oworklist* work, obase* object, ocolour colour )
{
    oslotlist* sl = (oslotlist*)object;
    
    // Mark all slots below watermark.  We are guaranteed a watermark at
    // least as new as the beginning of this collection cycle, and slots
    // must contain either valid references or still remain UNDEFINED
    // (if they have never been stored to).
    
    size_t watermark = sl->smark.load( std::memory_order_relaxed );
    for ( size_t i = 0; i < watermark; ++i )
    {
        // Must use consume in case we are reading a reference to an
        // object allocated in this cycle.
        uint3_t lo = sl->slots[ i ].lo.load( std::memory_order_consume );
        if ( lo != UNDEFINED )
        {
            obase* p = (obase*)lo;
            p->mark( work, colour );
        }
    }

}