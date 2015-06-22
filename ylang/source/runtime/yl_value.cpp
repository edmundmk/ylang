//
//  yl_value.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_value.h"



yl_stackref< yl_valarray > yl_valarray::alloc( size_t size )
{
    size_t s = sizeof( yl_valarray ) + sizeof( yl_valref ) * size;
    void* p = yl_current->allocate( s );
    return new ( p ) yl_valarray( size );
}


yl_valarray::yl_valarray( size_t size )
    :   yl_gcobject( YLOBJ_VALARRAY )
    ,   _size( size )
{
    for ( size_t i = 0; i < _size; ++i )
    {
        new ( _elements + i ) yl_valref();
    }
}

yl_valarray::~yl_valarray()
{
    for ( size_t i = 0; i < _size; ++i )
    {
        _elements[ i ].~yl_valref();
    }
}





