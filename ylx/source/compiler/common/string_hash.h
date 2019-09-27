//
//  string_hash.h
//
//  Created by Edmund Kapusniak on 06/08/2017.
//  Copyright © 2017 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <string_view>

/*
    A subclass of string_view that calculates the hash of a string only once.
*/

template < class charT, class traits = std::char_traits< charT > >
class basic_string_hash : public std::basic_string_view< charT, traits >
{
public:

    using typename std::basic_string_view< charT, traits >::const_pointer;
    using typename std::basic_string_view< charT, traits >::size_type;
    using std::basic_string_view< charT, traits >::npos;


    constexpr basic_string_hash() noexcept
        :   std::basic_string_view< charT, traits >()
        ,   _hash( 0 )
    {
    }

    constexpr basic_string_hash( std::basic_string_view< charT, traits > s ) noexcept
        :   std::basic_string_view< charT, traits >( s )
        ,   _hash( std::hash< std::basic_string_view< charT, traits > >()( s ) )
    {
    }

    constexpr basic_string_hash( const_pointer s, size_type count ) noexcept
        :   std::basic_string_view< charT, traits >( s, count )
        ,   _hash( std::hash< std::basic_string_view< charT, traits > >()( *this ) )
    {
    }

    constexpr basic_string_hash( const_pointer s )
        :   std::basic_string_view< charT, traits >( s )
        ,   _hash( std::hash< std::basic_string_view< charT, traits > >()( *this ) )
    {
    }

    constexpr basic_string_hash( size_t hash, std::basic_string_view< charT, traits > s ) noexcept
        :   std::basic_string_view< charT, traits >( s )
        ,   _hash( hash )
    {
    }

    constexpr basic_string_hash( size_t hash, const_pointer s, size_type count ) noexcept
        :   std::basic_string_view< charT, traits >( s, count )
        ,   _hash( hash )
    {
    }

    constexpr basic_string_hash( size_t hash, const_pointer s )
        :   std::basic_string_view< charT, traits >( s )
        ,   _hash( hash )
    {
    }


    constexpr basic_string_hash( const basic_string_hash& ) noexcept = default;
    basic_string_hash& operator = ( const basic_string_hash& ) noexcept = default;


    size_t hash() const { return _hash; }


    basic_string_hash substr( size_type pos = 0, size_type count = npos ) const
    {
        return basic_string_hash( std::basic_string_view< charT, traits >::substr( pos, count ) );
    }


    int compare( basic_string_hash v ) const noexcept
    {
        if ( _hash < v.hash() )
            return -1;
        else if ( _hash > v.hash() )
            return 1;
        else
            return std::basic_string_view< charT, traits >::compare( v );
    }

    int compare( size_type pos1, size_type count1, basic_string_hash v ) const
    {
        return substr( pos1, count1 ).compare( v );
    }

    int compare( size_type pos1, size_type count1, basic_string_hash v, size_type pos2, size_type count2 ) const
    {
        return substr( pos1, count1 ).compare( v.substr( pos2, count2 ) );
    }

    int compare( const charT* s ) const
    {
        return compare( std::basic_string_view< charT, traits >( s ) );
    }

    int compare( size_type pos1, size_type count1, const charT* s ) const
    {
        return substr( pos1, count1 ).compare( std::basic_string_view< charT, traits >( s ) );
    }

    int compare( size_type pos1, size_type count1, const charT* s, size_type count2 ) const
    {
        return substr( pos1, count1 ).compare( std::basic_string_view< charT, traits >( s, count2 ) );
    }

private:

    size_t _hash;

};

template < class charT, class traits >
bool operator == ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() == y.hash() && operator == ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) );
}

template < class charT, class traits >
bool operator != ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() != y.hash() || operator != ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) );
}

template < class charT, class traits >
bool operator <  ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() < y.hash() || ( x.hash() == y.hash() && operator < ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) ) );
}

template < class charT, class traits >
bool operator >  ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() > y.hash() || ( x.hash() == y.hash() && operator > ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) ) );
}

template < class charT, class traits >
bool operator <= ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() <= y.hash() || ( x.hash() == y.hash() && operator <= ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) ) );
}

template < class charT, class traits >
bool operator >= ( basic_string_hash< charT, traits > x, basic_string_hash< charT, traits > y ) noexcept
{
    return x.hash() >= y.hash() || ( x.hash() == y.hash() && operator >= ( std::basic_string_view< charT, traits >( x ), std::basic_string_view< charT, traits >( y ) ) );
}

typedef basic_string_hash< char > string_hash;
typedef basic_string_hash< wchar_t > wstring_hash;
typedef basic_string_hash< char16_t > u16string_hash;
typedef basic_string_hash< char32_t > u32string_hash;

extern template class basic_string_hash< char >;
extern template class basic_string_hash< wchar_t >;
extern template class basic_string_hash< char16_t >;
extern template class basic_string_hash< char32_t >;

template <> struct std::hash< string_hash >    { size_t operator () ( string_hash s ) const { return s.hash(); } };
template <> struct std::hash< wstring_hash >   { size_t operator () ( wstring_hash s ) const { return s.hash(); } };
template <> struct std::hash< u16string_hash > { size_t operator () ( u16string_hash s ) const { return s.hash(); } };
template <> struct std::hash< u32string_hash > { size_t operator () ( u32string_hash s ) const { return s.hash(); } };

#endif

