//
//  yl_array.cpp
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_array.h"



yl_array* yl_array::alloc( size_t capacity )
{
    return alloc( yl_current->proto_array(), capacity );
}

yl_array* yl_array::alloc( yl_object* prototype, size_t capacity )
{
    void* p = yl_current->malloc( sizeof( yl_array ) );
    return new ( p ) yl_array( yl_current->proto_array(), capacity );
}



yl_array::yl_array( yl_object* prototype, size_t capacity )
    :   yl_object( prototype )
    ,   _elements( capacity ? yl_valarray::alloc( capacity ) : nullptr )
    ,   _size( 0 )
{
}


void yl_array::reserve( size_t capacity )
{
    yl_valarray* elements = _elements.get();
    capacity = std::max( capacity, elements->size() );
    if ( capacity == elements->size() )
    {
        return;
    }
    
    yl_valarray* newelems = yl_valarray::alloc( capacity );
    for ( size_t i = 0; i < elements->size(); ++i )
    {
        newelems->at( i ).set( elements->at( i ).get() );
    }
}
