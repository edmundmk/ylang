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

    const_reference operator [] ( size_t i ) const;
    reference       operator [] ( size_t i );

    const_reference at( size_t i ) const;
    reference       at( size_t i );

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
    template < typename ... arguments_t > void emplace_back( alloc_region* alloc, arguments_t ... arguments );
    void            clear();

private:

    list_chunk* _first;
    list_chunk* _chunk;
    size_t _index;

};

#endif

