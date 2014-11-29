//
//  yobject.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yobject.h"



static const size_t COLLECT_THRESHOLD = 64 * ( 1 << 10 );



yobject::yobject( ymetatype* metatype, size_t size )
    :   word( (uintptr_t)metatype | yscope::scope->marked )
    ,   next( nullptr )
{
    // Add ourselves to allocation list.
    yscope* scope = yscope::scope;
    if ( scope->allocs_head )
    {
        scope->allocs_last->next = this;
        scope->allocs_last = this;
    }
    else
    {
        scope->allocs_head = this;
        scope->allocs_last = this;
    }
    
    
    // Increment allocation amount and request collection if it's over
    // the threshold.
    scope->allocs_total += size;
    if ( scope->allocs_total >= COLLECT_THRESHOLD )
    {
        scope->allocs_total = 0;
        ycollect();
    }
    
    
    // Ensure that the initialization of our word (with its colour) is
    // visible before any store of this new object into a slot visible
    // to the collector thread.  This is the reason that the mark thread
    // must use consume memory ordering when reading references.
    std::atomic_thread_fence( std::memory_order_release );
}



