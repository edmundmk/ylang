//
//  seglist.h
//
//  Created by Edmund Kapusniak on 23/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef SEGLIST_H
#define SEGLIST_H


/*
    A write-only container which is used to build a list of elements by
    appending them to the end.
*/


#include <assert.h>
#include <iterator>
#include <limits>
#include <new>


template < typename element_t >
class seglist
{
private:

    static const size_t ELEMENT_COUNT = 250;

    struct segment
    {
        segment*    next;
        segment*    prev;
        element_t   elements[];
    };

    template < typename result_t >
    class basic_iterator
        :   public std::iterator< std::forward_iterator_tag, result_t >
    {
    public:
    
        basic_iterator( const basic_iterator< const element_t >& iterator );
    
        bool operator != ( const basic_iterator& b ) const;

        basic_iterator& operator ++ ();
        basic_iterator operator ++ ( int );
        
        result_t& operator * () const;
        result_t* operator -> () const;
    
    private:
    
        friend class seglist;
        
        basic_iterator( segment* current, size_t index );
    
        segment*    current;
        size_t      index;

    };


public:

    typedef element_t value_type;
    typedef element_t& reference;
    typedef const element_t& const_reference;
    typedef basic_iterator< element_t > iterator;
    typedef basic_iterator< const element_t > const_iterator;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    
    seglist();
    seglist( seglist&& s );
    seglist( const seglist& s );
    seglist& operator = ( seglist&& s );
    seglist& operator = ( const seglist& s );
    ~seglist();

    bool operator == ( const seglist& b ) const;
    bool operator != ( const seglist& b ) const;
    
    size_type       size() const;
    size_type       max_size() const;
    bool            empty() const;

    const_iterator  cbegin() const;
    const_iterator  begin() const;
    const_iterator  cend() const;
    const_iterator  end() const;
    const_reference back() const;

    iterator        begin();
    iterator        end();
    reference       back();

    void            push_back( element_t&& element );
    void            push_back( const element_t& element );
    template < typename ... arguments_t > void emplace_back( arguments_t ... arguments );
    void            pop_back();
    void            clear();

    void            swap( seglist& s );
    

private:

    segment*        new_segment();
    void            pushed_back();

    segment*        first;
    segment*        last;
    size_t          index;

};


template < typename element_t >
void swap( seglist< element_t >& a, seglist< element_t >& b );






template < typename element_t >
template < typename result_t >
seglist< element_t >::basic_iterator< result_t >::basic_iterator(
                segment* current, size_t index )
    :   current( current )
    ,   index( index )
{
}

template < typename element_t >
template < typename result_t >
seglist< element_t >::basic_iterator< result_t >::basic_iterator(
                const basic_iterator< const element_t >& iterator )
    :   current( iterator.current )
    ,   index( iterator.index )
{
}

template < typename element_t >
template < typename result_t >
bool seglist< element_t >::basic_iterator< result_t >::operator != (
                const basic_iterator& b ) const
{
    return current != b.current || index != b.index;
}

template < typename element_t >
template < typename result_t >
seglist< element_t >::basic_iterator< result_t >&
        seglist< element_t >::basic_iterator< result_t >::operator ++ ()
{
    index += 1;
    if ( index >= ELEMENT_COUNT )
    {
        current = current->next;
        index = 0;
    }
}

template < typename element_t >
template < typename result_t >
seglist< element_t >::basic_iterator< result_t >
        seglist< element_t >::basic_iterator< result_t >::operator ++ ( int )
{
    basic_iterator result( *this );
    operator ++ ();
    return result;
}

template < typename element_t >
template < typename result_t >
result_t& seglist< element_t >::basic_iterator< result_t >::operator * () const
{
    return current->elements[ index ];
}

template < typename element_t >
template < typename result_t >
result_t* seglist< element_t >::basic_iterator< result_t >::operator -> () const
{
    return current->elements + index;
}



template < typename element_t >
seglist< element_t >::seglist()
    :   first( nullptr )
    ,   last( nullptr )
    ,   index( 0 )
{
    first = last = new_segment();
}

template < typename element_t >
seglist< element_t >::seglist( seglist&& s ) : seglist()
{
    swap( s );
}

template < typename element_t >
seglist< element_t >::seglist( const seglist& s ) : seglist()
{
    operator = ( s );
}

template < typename element_t >
seglist< element_t >& seglist< element_t >::operator = ( seglist&& s )
{
    swap( s );
}

template < typename element_t >
seglist< element_t >& seglist< element_t >::operator = ( const seglist& s )
{
    clear();
    for ( auto i = s.begin(); i != s.end(); ++i )
    {
        push_back( *i );
    }
}

template < typename element_t >
seglist< element_t >::~seglist()
{
    clear();
    for ( segment* s = first; s != nullptr; )
    {
        segment* seg = s;
        s = s->next;
        free( seg );
    }
}

template < typename element_t >
bool seglist< element_t >::operator == ( const seglist& b ) const
{
    for ( auto i = begin(), j = b.begin(); ; ++i, ++j )
    {
        if ( i == end() )
            return j == b.end();
        if ( j == b.end() )
            return false;
        if ( !( *i == *j ) )
            return false;
    }
}

template < typename element_t >
bool seglist< element_t >::operator != ( const seglist& b ) const
{
    return ! operator == ( b );
}

template < typename element_t >
typename seglist< element_t >::size_type seglist< element_t >::size() const
{
    size_t size = 0;
    for ( segment* s = first; s != last; ++s )
    {
        size += ELEMENT_COUNT;
    }
    size += index;
    return size;
}

template < typename element_t >
typename seglist< element_t >::size_type seglist< element_t >::max_size() const
{
    return std::numeric_limits< size_type >::max();
}

template < typename element_t >
bool seglist< element_t >::empty() const
{
    return first == last && index == 0;
}

template < typename element_t >
typename seglist< element_t >::const_iterator
                seglist< element_t >::cbegin() const
{
    return const_iterator( first, 0 );
}

template < typename element_t >
typename seglist< element_t >::const_iterator
                seglist< element_t >::begin() const
{
    return cbegin();
}

template < typename element_t >
typename seglist< element_t >::const_iterator
                seglist< element_t >::cend() const
{
    return const_iterator( last, index );
}

template < typename element_t >
typename seglist< element_t >::const_iterator
                seglist< element_t >::end() const
{
    return cend();
}

template < typename element_t >
typename seglist< element_t >::const_reference
                seglist< element_t >::back() const
{
    assert( ! empty() );
    if ( index > 0 )
        return last->elements[ index - 1 ];
    else
        return last->prev->elements[ ELEMENT_COUNT - 1 ];
}

template < typename element_t >
typename seglist< element_t >::iterator seglist< element_t >::begin()
{
    return iterator( first, 0 );
}

template < typename element_t >
typename seglist< element_t >::iterator seglist< element_t >::end()
{
    return iterator( last, index );
}

template < typename element_t >
typename seglist< element_t >::reference seglist< element_t >::back()
{
    assert( ! empty() );
    if ( index > 0 )
        return last->elements[ index - 1 ];
    else
        return last->prev->elements[ ELEMENT_COUNT - 1 ];
}

template < typename element_t >
void seglist< element_t >::push_back( element_t&& element )
{
    new ( last->elements + index ) element_t( std::move( element ) );
    pushed_back();
}

template < typename element_t >
void seglist< element_t >::push_back( const element_t& element )
{
    new ( last->elements + index ) element_t( element );
    pushed_back();
}

template < typename element_t >
template < typename ... arguments_t >
void seglist< element_t >::emplace_back( arguments_t ... arguments )
{
    new ( last->elements + index ) element_t(
                    std::forward< arguments_t ... >( arguments ... ) );
    pushed_back();
}

template < typename element_t >
void seglist< element_t >::pop_back()
{
    assert( ! empty() );

    if ( index == 0 )
    {
        last = last->prev;
        index = ELEMENT_COUNT;
    }
    
    index -= 1;
    last->elements[ index ].~element_t();
}

template < typename element_t >
void seglist< element_t >::clear()
{
    if ( ! std::is_trivially_destructible< element_t >::value )
    {
        for ( segment* s = first; s != last; s = s->next )
        {
            for ( size_t i = 0; i < ELEMENT_COUNT; ++i )
            {
                s->elements[ i ].~element_t();
            }
        }
        
        if ( last )
        {
            for ( size_t i = 0; i < index; ++i )
            {
                last->elements[ i ].~element_t();
            }
        }
    }
    
    last = first;
    index = 0;
}

template < typename element_t >
void seglist< element_t >::swap( seglist& s )
{
    std::swap( first, s.first );
    std::swap( last, s.last );
    std::swap( index, s.index );
}

template < typename element_t >
typename seglist< element_t >::segment* seglist< element_t >::new_segment()
{
    size_t segsize = sizeof( segment ) + sizeof( element_t ) * ELEMENT_COUNT;
    segment* s = (segment*)malloc( segsize );
    s->next = nullptr;
    s->prev = nullptr;
    return s;
}

template < typename element_t >
void seglist< element_t >::pushed_back()
{
    if ( index >= ELEMENT_COUNT )
    {
        if ( last->next == nullptr )
        {
            last->next = new_segment();
            last->next->prev = last;
        }
        
        last = last->next;
        index = 0;
    }
}



#endif
