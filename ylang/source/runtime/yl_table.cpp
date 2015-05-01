//
//  yl_table.cpp
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_table.h"



yl_table* yl_table::alloc( size_t capacity )
{
    return alloc( yl_current->proto_table(), capacity );
}

yl_table* yl_table::alloc( yl_object* prototype, size_t capacity )
{
    void* p = yl_current->malloc( sizeof( yl_table ) );
    return new ( p ) yl_table( prototype, capacity );
}


yl_table::yl_table( yl_object* prototype, size_t capacity )
    :   yl_object( prototype )
    ,   _occupancy( 0 )
    ,   _buckets( capacity ? bucketlist::alloc( capacity ) : nullptr )
{
}


yl_table::bucket::bucket()
    :   next( nullptr )
{
}

yl_table::bucketlist* yl_table::bucketlist::alloc( size_t size )
{
    size_t s = sizeof( bucketlist ) + sizeof( bucket ) * size;
    void* p = yl_current->malloc( s );
    return new ( p ) bucketlist( size );
}


yl_table::bucketlist::bucketlist( size_t size )
    :   _size( size )
{
    for ( size_t i = 0; i < _size; ++i )
    {
        new ( _elements + i ) bucket();
    }
}

yl_table::bucketlist::~bucketlist()
{
    for ( size_t i = 0; i < _size; ++i )
    {
        _elements[ i ].~bucket();
    }
}





