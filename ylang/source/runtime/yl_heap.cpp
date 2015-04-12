//
//  yl_heap.cpp
//
//  Created by Edmund Kapusniak on 06/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_heap.h"



__thread yl_heap* yl_heap_current = nullptr;


yl_heap::yl_heap()
    :   _mspace( create_mspace( 0, 0 ) )
{
    mspace_track_large_chunks( _mspace, 1 );
}

yl_heap::~yl_heap()
{
    destroy_mspace( _mspace );
}


void* yl_heap::malloc( size_t size )
{
    return mspace_malloc( _mspace, size );
}

void yl_heap::write_barrier( yl_heapobj* obj )
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


yl_heapobj::yl_heapobj( yl_objkind kind )
    :   _kind( kind )
    ,   _colour( /* TODO */ YL_GREY )
{
}


