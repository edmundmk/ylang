//
//  yl_context.cpp
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_context.h"
#include <assert.h>
#include "yl_heapobj.h"
#include "yl_table.h"





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
    ,   _unmarked_colour( YL_YELLOW )
    ,   _cothread( nullptr )
    ,   _global( nullptr )
{
    mspace_track_large_chunks( _heap, 1 );
    _global = yl_table::alloc( 0 );
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

void yl_context_impl::add_root( yl_heapobj* root )
{
    _roots.insert( root );
}

void yl_context_impl::remove_root( yl_heapobj* root )
{
    _roots.erase( root );
}



void yl_context_impl::set_cothread( yl_cothread* cothread )
{
    // TODO: interact with GC by 'unlocking' cothread.
    _cothread = cothread;
}



yl_string* yl_context_impl::symbol( yl_string* string )
{
    // TODO.
    return string;
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


