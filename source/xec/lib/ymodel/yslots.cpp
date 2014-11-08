//
//  yslots.cpp
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yslots.h"



ymetatype yslots::metatype = { &mark_slots, "slotlist" };



yslots* yslots::alloc( size_t size )
{
    void* p = malloc( sizeof( yslots ) + sizeof( slot ) * size );
    return new ( p ) yslots( &metatype, size );
}


yslots::yslots( ymetatype* metatype, size_t size )
    :   yobject( metatype )
    ,   ssize( (uint32_t)size )
    ,   smark( 0 )
{
    for ( size_t i = 0; i < ssize; ++i )
    {
        new ( slots + i ) slot();
    }
}


void yslots::mark_slots( yobject* object, yworklist* work, ycolour colour )
{
    yslots* slots = (yslots*)object;
    
    // Mark all slots below watermark.  We are guaranteed a watermark at
    // least as new as the beginning of this collection cycle, and slots
    // must contain either valid references or still remain UNDEFINED
    // (if they have never been stored to).
    
    size_t watermark = slots->smark.load( std::memory_order_relaxed );
    for ( size_t i = 0; i < watermark; ++i )
    {
        // Must use consume in case we are reading a reference to an
        // object allocated in this cycle.
#if Y64BIT

        uint64_t x = slots->slots[ i ].x.load( std::memory_order_consume );
        if ( x <= yvalue::MAX_REFERENCE )
        {
            yobject* p = (yobject*)( x & yvalue::POINTER_MASK );
            p->mark( work, colour );
        }

#else

        uint32_t lo = slots->slots[ i ].lo.load( std::memory_order_consume );
        if ( lo != UNDEFINED )
        {
            yobject* p = (yobject*)lo;
            p->mark( work, colour );
        }

#endif
    }

}




