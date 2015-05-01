//
//  yl_array.h
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_ARRAY_H
#define YL_ARRAY_H


#include <assert.h>
#include <intmath.h>
#include "yl_object.h"
#include "yl_value.h"



class yl_array : public yl_object
{
public:

    static yl_array* alloc( size_t capacity );
    static yl_array* alloc( yl_object* prototype, size_t capacity );

    size_t      size() const;

    yl_value   get( size_t index ) const;
    void        set( size_t index, const yl_value& value );

    void        append( const yl_value& value );
    void        extend( const yl_value* values, size_t count );

    void        reserve( size_t capacity );


protected:

    yl_array( yl_object* prototype, size_t capacity );



private:

    yl_objref< yl_valarray >   _elements;
    size_t                      _size;


};



/*

*/


inline size_t yl_array::size() const
{
    return _size;
}

inline yl_value yl_array::get( size_t index ) const
{
    if ( index >= _size )
    {
        throw yl_exception( "invalid index" );
    }
    return _elements.get()->at( index ).get();
}

inline void yl_array::set( size_t index, const yl_value& value )
{
    if ( index >= _size )
    {
        throw yl_exception( "invalid index" );
    }
    _elements.get()->at( index ).set( value );
}

inline void yl_array::append( const yl_value& value )
{
    yl_valarray* elements = _elements.get();
    if ( elements->size() <= _size + 1 )
    {
        reserve( elements->size() ? elements->size() * 2 : 8 );
        elements = _elements.get();
    }
    elements->at( _size ).set( value );
    _size += 1;
}

inline void yl_array::extend( const yl_value* values, size_t count )
{
    yl_valarray* elements = _elements.get();
    if ( elements->size() <= _size + count )
    {
        reserve( ceil_pow2( _size + count ) );
        elements = _elements.get();
    }
    for ( size_t i = 0; i < count; ++i )
    {
        elements->at( _size + i ).set( values[ i ] );
    }
    _size += count;
}


#endif






