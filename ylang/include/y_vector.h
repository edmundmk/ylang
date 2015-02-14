//
//  y_vector.h
//
//  Created by Edmund Kapusniak on 02/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_VECTOR_H
#define Y_VECTOR_H


#include <assert.h>
#include <intmath.h>
#include <algorithm>
#include "y_object.h"


/*
    y_vector is a resizable vector of elements.
*/

template < typename element_t >
class y_vector : public y_object
{
public:

    typedef typename yslot_wrap< element_t >::type element_type;

    y_vector();

    const element_type& at( size_t index ) const;
    element_type&       at( size_t index );
    const element_type& front() const;
    element_type&       front();
    const element_type& back() const;
    element_type&       back();
    const element_type* data() const;
    element_type*       data();
    
    size_t              capacity() const;
    size_t              size() const;
    bool                empty() const;
    
    void                reserve( size_t new_capacity );
    void                shrink_to_fit();
    void                clear();

    void                push_back( element_type&& value );
    void                push_back( const element_type& value );
    void                pop_back();
    
    element_type&       insert( size_t index, element_type&& value );
    element_type&       insert( size_t index, const element_type& value );
    
    void                remove( size_t index, size_t count = 1 );


private:

    size_t                          _size;
    yslot< y_array< element_t > >   _elements;

};



/*
*/


template < typename element_t >
y_vector< element_t >::y_vector()
    :   _size( 0 )
    ,   _elements( nullptr )
{
}

template < typename element_t >
const typename y_vector< element_t >::element_type&
                y_vector< element_t >::at( size_t index ) const
{
    return _elements->at( index );
}

template < typename element_t >
typename y_vector< element_t >::element_type&
                y_vector< element_t >::at( size_t index )
{
    return _elements->at( index );
}

template < typename element_t >
const typename y_vector< element_t >::element_type&
                y_vector< element_t >::front() const
{
    return _elements->at( 0 );
}

template < typename element_t >
typename y_vector< element_t >::element_type&
                y_vector< element_t >::front()
{
    return _elements->at( 0 );
}

template < typename element_t >
const typename y_vector< element_t >::element_type&
                y_vector< element_t >::back() const
{
    return _elements->at( _size - 1 );
}

template < typename element_t >
typename y_vector< element_t >::element_type&
                y_vector< element_t >::back()
{
    return _elements->at( _size - 1 );
}

template < typename element_t >
const typename y_vector< element_t >::element_type*
                y_vector< element_t >::data() const
{
    return _size ? &_elements->at( 0 ) : nullptr;
}

template < typename element_t >
typename y_vector< element_t >::element_type*
                y_vector< element_t >::data()
{
    return _size ? &_elements->at( 0 ) : nullptr;
}

template < typename element_t >
size_t y_vector< element_t >::capacity() const
{
    return _elements ? _elements->size() : 0;
}

template < typename element_t >
size_t y_vector< element_t >::size() const
{
    return _size;
}

template < typename element_t >
bool y_vector< element_t >::empty() const
{
    return _size == 0;
}

template < typename element_t >
void y_vector< element_t >::reserve( size_t new_capacity )
{
    new_capacity = std::max( new_capacity, _size );
    if ( new_capacity == capacity() )
        return;
    
    if ( new_capacity )
    {
        yauto< y_array< element_t > > new_elements =
                        y_array< element_t >::alloc( new_capacity );
    
        for ( size_t i = 0; i < _size; ++i )
        {
            new_elements->at( i ) = std::move( _elements->at( i ) );
        }
        
        _elements = new_elements;
    }
    else
    {
        _elements = nullptr;
    }
}

template < typename element_t >
void y_vector< element_t >::shrink_to_fit()
{
    reserve( _size );
}

template < typename element_t >
void y_vector< element_t >::clear()
{
    remove( 0, _size );
}

template < typename element_t >
void y_vector< element_t >::push_back( element_type&& value )
{
    insert( _size, std::forward( value ) );
}

template < typename element_t >
void y_vector< element_t >::push_back( const element_type& value )
{
    insert( _size, value );
}

template < typename element_t >
void y_vector< element_t >::pop_back()
{
    assert( _size > 0 );
    remove( _size - 1 );
}

template < typename element_t >
typename y_vector< element_t >::element_type&
        y_vector< element_t >::insert( size_t index, element_type&& value )
{
    assert( index <= _size );
    
    size_t new_size = _size + 1;
    if ( capacity() < new_size )
    {
        reserve( ceil_pow2( new_size ) );
    }
    
    for ( size_t i = _size; i > index; --i )
    {
        _elements->at( i ) = std::move( _elements->at( i - 1 ) );
    }
    _elements->at( index ) = std::forward( value );

    _size = new_size;
    return _elements->at( index );
}

template < typename element_t >
typename y_vector< element_t >::element_type&
        y_vector< element_t >::insert( size_t index, const element_type& value )
{
    assert( index <= _size );
    
    size_t new_size = _size + 1;
    if ( capacity() < new_size )
    {
        reserve( ceil_pow2( new_size ) );
    }
    
    for ( size_t i = _size; i > index; --i )
    {
        _elements->at( i ) = std::move( _elements->at( i - 1 ) );
    }
    _elements->at( index ) = value;

    _size = new_size;
    return _elements->at( index );
}

template < typename element_t >
void y_vector< element_t >::remove( size_t index, size_t count )
{
    assert( index + count <= _size );

    size_t new_size = _size - count;

    for ( size_t i = index; i < new_size; ++i )
    {
        _elements->at( i ) = std::move( _elements->at( i + count ) );
    }
    
    for ( size_t i = new_size; i < _size; ++i )
    {
        _elements->at( i ) = element_type();
    }
    
    _size = new_size;
}



#endif


