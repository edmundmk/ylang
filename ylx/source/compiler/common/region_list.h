//
//  region_list.h
//
//  Created by Edmund Kapusniak on 27/09/2019.
//  Copyright © Edmund Kapusniak 2019.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef TRIVIAL_LIST_H
#define TRIVIAL_LIST_H

#include <type_traits>
#include <iterator>
#include <new>
#include "alloc_region.h"

/*
    A list containing objects that don't require destruction, that allocates
    in chunks from an allocation region.
*/

template < typename T >
class region_list
{
private:

    static const size_t CHUNK_SIZE = 15;

    struct list_chunk
    {
        list_chunk* next;
        T elements[ CHUNK_SIZE ];
    };

    template < typename constT >
    class basic_iterator
    {
    public:

        typedef std::forward_iterator_tag iterator_category;
        typedef std::remove_cv_t< constT > value_type;
        typedef ptrdiff_t difference_type;
        typedef constT* pointer;
        typedef constT& reference;

        basic_iterator( const basic_iterator< T >& iterator );

        bool operator == ( const basic_iterator& b ) const;
        bool operator != ( const basic_iterator& b ) const;

        basic_iterator& operator ++ ();
        basic_iterator operator ++ ( int );

        constT& operator * () const;
        constT* operator -> () const;

    private:

        friend class region_list;
        basic_iterator( list_chunk* chunk, size_t index );
        list_chunk* chunk;
        size_t index;

    };

public:

    static_assert( std::is_trivially_copyable< T >::value );

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef basic_iterator< T > iterator;
    typedef basic_iterator< const T > const_iterator;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    region_list();
    region_list( region_list&& ) = default;
    region_list( const region_list& ) = delete;
    region_list& operator = ( region_list&& ) = default;
    region_list& operator = ( const region_list& ) = delete;

    size_type       size() const;
    size_type       max_size() const;
    bool            empty() const;

    const_iterator  cbegin() const;
    const_iterator  begin() const;
    const_iterator  cend() const;
    const_iterator  end() const;

    iterator        begin();
    iterator        end();

    void            push_back( alloc_region* alloc, T&& element );
    void            push_back( alloc_region* alloc, const T& element );
    template < typename ... A > void emplace_back( alloc_region* alloc, A ... arguments );
    void            clear();

private:

    void* allocate( alloc_region* alloc );

    list_chunk* _first;
    list_chunk* _chunk;
    size_t _index;

};

/*

*/

template < typename T >
template < typename constT >
region_list< T >::basic_iterator< constT >::basic_iterator( list_chunk* chunk, size_t index )
    :   chunk( chunk )
    ,   index( index )
{
}

template < typename T >
template < typename constT >
region_list< T >::basic_iterator< constT >::basic_iterator( const basic_iterator< T >& iterator )
    :   chunk( iterator.chunk )
    ,   index( iterator.index )
{
}

template < typename T >
template < typename constT >
bool region_list< T >::basic_iterator< constT >::operator != ( const basic_iterator& b ) const
{
    return chunk != b.chunk || index != b.index;
}

template < typename T >
template < typename constT >
typename region_list< T >::template basic_iterator< constT >& region_list< T >::basic_iterator< constT >::operator ++ ()
{
    index += 1;
    if ( index >= CHUNK_SIZE )
    {
        chunk = chunk->next;
        index = 0;
    }
    return *this;
}

template < typename T >
template < typename constT >
typename region_list< T >::template basic_iterator< constT > region_list< T >::basic_iterator< constT >::operator ++ ( int )
{
    basic_iterator result( *this );
    operator ++ ();
    return result;
}

template < typename T >
template < typename constT >
constT& region_list< T >::basic_iterator< constT >::operator * () const
{
    return chunk->elements[ index ];
}

template < typename T >
template < typename constT >
constT* region_list< T >::basic_iterator< constT >::operator -> () const
{
    return chunk->elements + index;
}


template < typename T >
region_list< T >::region_list()
    :   _first( nullptr )
    ,   _chunk( nullptr )
    ,   _index( 0 )
{
}

template < typename T >
typename region_list< T >::size_type region_list< T >::size() const
{
    size_t size = 0;
    for ( list_chunk* c = _first; c != _chunk; c = c->next )
    {
        size += CHUNK_SIZE;
    }
    size += _index;
    return size;
}

template < typename T >
typename region_list< T >::size_type region_list< T >::max_size() const
{
    return std::numeric_limits< size_type >::max();
}

template < typename T >
bool region_list< T >::empty() const
{
    return _first == _chunk && _index == 0;
}

template < typename T >
typename region_list< T >::const_iterator region_list< T >::cbegin() const
{
    return const_iterator( _first, 0 );
}

template < typename T >
typename region_list< T >::const_iterator region_list< T >::begin() const
{
    return cbegin();
}

template < typename T >
typename region_list< T >::const_iterator region_list< T >::cend() const
{
    return const_iterator( _chunk, _index );
}

template < typename T >
typename region_list< T >::const_iterator region_list< T >::end() const
{
    return cend();
}

template < typename T >
typename region_list< T >::iterator region_list< T >::begin()
{
    return iterator( _chunk, 0 );
}

template < typename T >
typename region_list< T >::iterator region_list< T >::end()
{
    return iterator( _chunk, _index );
}

template < typename T >
void region_list< T >::push_back( alloc_region* alloc, T&& element )
{
    new ( allocate( alloc ) ) T( std::move( element ) );
}

template < typename T >
void region_list< T >::push_back( alloc_region* alloc, const T& element )
{
    new ( allocate( alloc ) ) T( element );
}

template < typename T >
template < typename ... A >
void region_list< T >::emplace_back( alloc_region* alloc, A ... arguments )
{
    new ( allocate( alloc ) ) T( std::forward< A ... >( arguments ... ) );
}

template < typename T >
void region_list< T >::clear()
{
    _chunk = _first;
    _index = 0;
}

template < typename T >
void* region_list< T >::allocate( alloc_region* alloc )
{
    if ( ! _chunk || _index >= CHUNK_SIZE )
    {
        if ( _chunk && _chunk->next )
        {
            _chunk = _chunk->next;
        }
        else
        {
            list_chunk* new_chunk = (list_chunk*)alloc->malloc( sizeof( list_chunk ) );
            new_chunk->next = nullptr;
            _chunk->next = new_chunk;
        }
        _index = 0;
    }

    return &_chunk->elements[ _index++ ];
}

#endif

