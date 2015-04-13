//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_heapobj.h"




yl_context::yl_context()
    :   _impl( new yl_context_impl() )
{
}

yl_context::~yl_context()
{
}




__thread yl_context_impl* yl_current = nullptr;


yl_context_impl::yl_context_impl()
    :   _heap( create_mspace( 0, 0 ) )
{
    mspace_track_large_chunks( _heap, 1 );
}

yl_context_impl::~yl_context_impl()
{
    destroy_mspace( _heap );
}


void* yl_context_impl::malloc( size_t size )
{
    return mspace_malloc( _heap, size );
}

void yl_context_impl::write_barrier( yl_heapobj* object )
{
    // TODO.
}



yl_alloc_scope::yl_alloc_scope()
{
    // TODO.
}

yl_alloc_scope::~yl_alloc_scope()
{
    // TODO.
}





yl_scope::yl_scope( yl_context* context )
    :   _context( context->_impl.get() )
    ,   _previous( yl_current )
{
    yl_current = _context;
}

yl_scope::~yl_scope()
{
    assert( yl_current == _context );
    yl_current = _previous;
}


