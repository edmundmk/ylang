//
//  y_array.h
//
//  Created by Edmund Kapusniak on 03/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_ARRAY_H
#define Y_ARRAY_H


#include <assert.h>
#include "y_object.h"



/*
    A fixed-length array.
*/

template < typename element_t >
class y_array : public y_object
{
public:

    typedef typename yslot_wrap< element_t >::type element_type;

    static yauto< y_array< element_t > > alloc( size_t size );

    virtual ~y_array();
    
    size_t              size() const;
    const element_type& at( size_t index ) const;
    element_type&       at( size_t index );
    

private:

    y_array( size_t size );

    size_t          _size;
    element_type    _elements[];

};




/*
*/


template < typename element_t >
yauto< y_array< element_t > > y_array< element_t >::alloc( size_t size )
{
    return y_object::alloc< y_array >( sizeof( element_type ) * size, size );
}

template < typename element_t >
y_array< element_t >::y_array( size_t size )
    :   _size( size )
{
    for ( size_t i = 0; i < _size; ++i )
    {
        new ( _elements + i ) element_type();
    }
}

template < typename element_t >
y_array< element_t >::~y_array()
{
    for ( size_t i = 0; i < _size; ++i )
    {
        _elements[ i ].~element_type();
    }
}

template < typename element_t >
size_t y_array< element_t >::size() const
{
    return _size;
}

template < typename element_t >
const typename y_array< element_t >::element_type&
                y_array< element_t >::at( size_t index ) const
{
    assert( index < _size );
    return _elements[ index ];
}

template < typename element_t >
typename y_array< element_t >::element_type&
                y_array< element_t >::at( size_t index )
{
    assert( index < _size );
    return _elements[ index ];
}



#endif


