//
//  ybase.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yobject.h"



obase::obase( ometatype* metatype )
    :   word( (uintptr_t)metatype | ocontext::context->mark_colour )
    ,   next( ocontext::context->allocs )
{
    ocontext::context->allocs = this;
    
    // Ensure that the initialization of our word (with its colour) is
    // visible before any store of this new object into a slot visible
    // to the collector thread.  This is the reason that the mark thread
    // must use consume memory ordering when reading references.
    std::atomic_thread_fence( std::memory_order_release );
}



