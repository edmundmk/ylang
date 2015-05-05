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

    static yl_object* make_prototype();

    static yl_array* alloc( size_t capacity );
    static yl_array* alloc( yl_object* prototype, size_t capacity );

    size_t      length() const;

    yl_value    get_index( size_t index ) const;
    void        set_index( size_t index, yl_value value );

    void        append( yl_value value );
    void        extend( const yl_value* values, size_t count );

    void        reserve( size_t capacity );
    void        resize( size_t length );


protected:

    yl_array( yl_object* prototype, size_t capacity );


private:

    static yl_array* thunk_this( yl_callframe& xf );
    static void thunk_length( yl_callframe& xf );
    static void thunk_resize( yl_callframe& xf );

    yl_objref< yl_valarray >    _elements;
    size_t                      _length;


};



/*

*/


inline size_t yl_array::length() const
{
    return _length;
}

inline yl_value yl_array::get_index( size_t index ) const
{
    assert( index < _length );
    return _elements.get()->at( index ).get();
}

inline void yl_array::set_index( size_t index, yl_value value )
{
    assert( index < _length );
    _elements.get()->at( index ).set( value );
}

inline void yl_array::append( yl_value value )
{
    yl_valarray* elements = _elements.get();
    if ( ! elements || elements->size() <= _length + 1 )
    {
        reserve( elements && elements->size() ? elements->size() * 2 : 8 );
        elements = _elements.get();
    }
    elements->at( _length ).set( value );
    _length += 1;
}

inline void yl_array::extend( const yl_value* values, size_t count )
{
    yl_valarray* elements = _elements.get();
    if ( ! elements || elements->size() <= _length + count )
    {
        reserve( ceil_pow2( _length + count ) );
        elements = _elements.get();
    }
    for ( size_t i = 0; i < count; ++i )
    {
        elements->at( _length + i ).set( values[ i ] );
    }
    _length += count;
}


#endif






