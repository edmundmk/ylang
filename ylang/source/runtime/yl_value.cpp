//
//  yl_value.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_value.h"


yl_gctype yl_valarray::gctype =
{
    YLOBJ_VALARRAY,
    YL_GCFLAG_NONE,
    "valarray",
    &yl_valarray::destroy,
    &yl_valarray::mark,
    nullptr
};


yl_rootref< yl_valarray > yl_valarray::alloc( size_t size )
{
    size_t sz = sizeof( yl_valarray ) + sizeof( yl_heapval ) * size;
    return yl_gcalloc< yl_valarray >( sz, size );
}


yl_valarray::yl_valarray( size_t size )
    :   _size( size )
{
    for ( size_t i = 0; i < _size; ++i )
    {
        new ( _elements + i ) yl_heapval();
    }
}

yl_valarray::~yl_valarray()
{
    for ( size_t i = 0; i < _size; ++i )
    {
        _elements[ i ].~yl_heapval();
    }
}


void yl_valarray::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_valarray* self = (yl_valarray*)object;
    self->~yl_valarray();
}

void yl_valarray::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_valarray* self = (yl_valarray*)object;
    for ( size_t i = 0; i < self->_size; ++i )
    {
        self->_elements[ i ].mark( heap );
    }
}




