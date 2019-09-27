//
//  region_buffer.h
//
//  Created by Edmund Kapusniak on 17/09/2013.
//  Copyright © Edmund Kapusniak 2013-2019.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef REGION_BUFFER_H
#define REGION_BUFFER_H

#include "alloc_region.h"

class region_buffer
{
public:

    explicit region_buffer( alloc_region& region );
    ~region_buffer();

    void    clear();
    void    append( char c );
    void    append( uint8_t c );
    void    append( const void* data, size_t size );
    size_t  size();
    void*   get();

    void    shrink();
    void*   tearoff();

private:

    void _expand( size_t new_capacity );

    alloc_region* _region;
    char* _buffer;
    size_t _capacity;
    size_t _index;

};

/*

*/

inline void region_buffer::clear()
{
    _index = 0;
}

inline void region_buffer::append( char c )
{
    if ( _index >= _capacity )
    {
        _expand( _index + 1 );
    }

    _buffer[ _index++ ] = c;
}

inline void region_buffer::append( uint8_t c )
{
    if ( _index >= _capacity )
    {
        _expand( _index + 1 );
    }

    _buffer[ _index++ ] = c;
}

inline void region_buffer::append( const void* data, size_t size )
{
    if ( _index + _size > _capacity )
    {
        _expand( _index + _size );
    }

    memcpy( _buffer + _index, data, size );
    _index += _size;
}

inline size_t region_buffer::size()
{
    return _index;
}

inline void* region_buffer::get()
{
    return _buffer;
}

#endif

