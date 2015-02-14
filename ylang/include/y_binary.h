//
//  y_binary.h
//
//  Created by Edmund Kapusniak on 02/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_BINARY_H
#define Y_BINARY_H


#include <assert.h>
#include <string.h>
#include "y_object.h"


/*
    A fixed-size chunk of raw binary data.
*/

class y_binary : public y_object
{
public:

    static yauto< y_binary > alloc( size_t size );
    
    size_t          size() const;
    
    uint8_t         at( size_t index ) const;
    uint8_t&        at( size_t index );
    const uint8_t*  data( size_t offset = 0 ) const;
    uint8_t*        data( size_t offset = 0 );

    void            zero();
    void            zero( size_t offset, size_t size );
    void            copy_in( size_t offset, void* buffer, size_t size );
    void            copy_out( void* buffer, size_t offset, size_t size ) const;


private:

    friend class y_object;

    y_binary( size_t size );

    size_t  _size;
    uint8_t _data[];

};




/*
*/



inline size_t y_binary::size() const
{
    return _size;
}

inline uint8_t y_binary::at( size_t index ) const
{
    assert( index < _size );
    return _data[ index ];
}

inline uint8_t& y_binary::at( size_t index )
{
    assert( index < _size );
    return _data[ index ];
}

inline const uint8_t* y_binary::data( size_t offset ) const
{
    assert( offset <= _size );
    return _data + offset;
}

inline uint8_t* y_binary::data( size_t offset )
{
    assert( offset <= _size );
    return _data + offset;
}

inline void y_binary::zero()
{
    zero( 0, _size );
}

inline void y_binary::zero( size_t offset, size_t size )
{
    assert( offset + size <= _size );
    memset( _data + offset, 0, size );
}

inline void y_binary::copy_in( size_t offset, void* buffer, size_t size )
{
    assert( offset + size <= _size );
    memcpy( _data + offset, buffer, size );
}

inline void y_binary::copy_out( void* buffer, size_t offset, size_t size ) const
{
    assert( offset + size <= _size );
    memcpy( buffer, _data + offset, size );
}




#endif



