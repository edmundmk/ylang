//
//  ovalue.h
//
//  Created by Edmund Kapusniak on 25/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OVALUE_H
#define OVALUE_H


#include <cstddef>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <functional>
#include "ostring.h"
#include "oerror.h"


class oexpand;


/*
    An ovalue is a reference to an omodel object.  Some ovalues are stored
    inline.

    Doubles have the following bit patterns:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

    On a system with 48-bit pointers (or less), we NaN-box values into doubles.
    Bit patterns of non-float values are all negative quiet NaNs:

                 F   F   F   x   0   0   0   0   0   0   0   0   0   0   0   0
    nan       s111111111111000000000000000000000000000000000000000000000000000
    null      1111111111111111111111111111111111111111111111111111111111111111
    undefined 1111111111111111111111111111111111111111111111111111111111111110
    false     1111111111111111111111111111111111111111111111111111111111111101
    true      1111111111111111111111111111111111111111111111111111111111111100
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppppp
    expand    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppppp
    native    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppppp

    On systems with a truly 64-bit address space, we should ensure that values
    are allocated from the low 48-bits (or from a 48-bit heap).

*/


class ovalue
{
public:

    static ovalue undefined;

    ovalue();
    ovalue( std::nullptr_t );
    ovalue( bool value );
    ovalue( double value );
    ovalue( const char* string );
    ovalue( ostring* string );
    ovalue( oexpand* expand );
    static ovalue native( void* native );

    explicit operator bool() const;
    const char* c_str() const;

    bool        is_null() const;
    bool        is_undefined() const;
    bool        is_bool() const;
    bool        is_number() const;
    bool        is_string() const;
    bool        is_expand() const;
    bool        is_native() const;

    bool        as_bool() const;
    double      as_number() const;
    ostring*    as_string() const;
    oexpand*    as_expand() const;
    void*       as_native() const;
    
    template < typename object_t > bool is() const;
    template < typename object_t > object_t* as() const;
    
    void*       get() const;


private:

    friend bool operator == ( ovalue a, ovalue b );
    friend struct std::hash< ovalue >;
    
    static const uint64_t POSITIVE_NAN      = INT64_C( 0x7FF8000000000000 );
    static const uint64_t NEGATIVE_NAN      = INT64_C( 0xFFF8000000000000 );
    static const uint64_t POSITIVE_ZERO     = INT64_C( 0x0000000000000000 );
    static const uint64_t NEGATIVE_ZERO     = INT64_C( 0x8000000000000000 );
    static const uint64_t SIGN_MASK         = INT64_C( 0x7FFFFFFFFFFFFFFF );

    static const uint64_t VALUE_NULL        = INT64_C( 0xFFFFFFFFFFFFFFFF );
    static const uint64_t VALUE_UNDEFINED   = INT64_C( 0xFFFFFFFFFFFFFFFE );
    static const uint64_t VALUE_FALSE       = INT64_C( 0xFFFFFFFFFFFFFFFD );
    static const uint64_t VALUE_TRUE        = INT64_C( 0xFFFFFFFFFFFFFFFC );
    static const uint64_t TAG_STRING        = INT64_C( 0xFFFE000000000000 );
    static const uint64_t TAG_EXPAND        = INT64_C( 0xFFFD000000000000 );
    static const uint64_t TAG_NATIVE        = INT64_C( 0xFFFC000000000000 );
    static const uint64_t TAG_MASK          = INT64_C( 0xFFFF000000000000 );
    static const uint64_t POINTER_MASK      = INT64_C( 0x0000FFFFFFFFFFFF );
    static const uint64_t HIGHEST_FLOAT     = INT64_C( 0xFFF8000000000000 );
    
    ovalue( uint64_t x );

    union
    {
        double      n;
        uint64_t    x;
    };

};







inline ovalue::ovalue()
    :   ovalue( VALUE_NULL )
{
}

inline ovalue::ovalue( std::nullptr_t )
    :   ovalue( VALUE_NULL )
{
}

inline ovalue::ovalue( bool value )
    :   ovalue( value ? VALUE_TRUE : VALUE_FALSE )
{
}

inline ovalue::ovalue( double value )
    :   n( ! isnan( value ) ? value : signbit( value ) ? NEGATIVE_NAN : POSITIVE_NAN  )
{
}

inline ovalue::ovalue( const char* string )
    :   ovalue( TAG_STRING | (uintptr_t)( ostring::alloc( string ) ) )
{
}

inline ovalue::ovalue( ostring* string )
    :   ovalue( TAG_STRING | (uintptr_t)string )
{
    assert( string );
}

inline ovalue::ovalue( oexpand* expand )
    :   ovalue( TAG_EXPAND | (uintptr_t)expand )
{
    assert( expand );
}

inline ovalue ovalue::native( void* native )
{
    return ovalue( TAG_NATIVE | (uintptr_t)native );
}

inline ovalue::ovalue( uint64_t x )
    :   x( x )
{
}


inline ovalue::operator bool() const
{
    return ( x >= VALUE_FALSE ) || ( x & SIGN_MASK ) == POSITIVE_ZERO;
}

inline const char* ovalue::c_str() const
{
    return as_string()->c_str();
}


inline bool ovalue::is_null() const
{
    return x == VALUE_NULL;
}

inline bool ovalue::is_undefined() const
{
    return x == VALUE_UNDEFINED;
}

inline bool ovalue::is_bool() const
{
    return x == VALUE_FALSE || x == VALUE_TRUE;
}

inline bool ovalue::is_number() const
{
    return x <= HIGHEST_FLOAT;
}

inline bool ovalue::is_string() const
{
    return ( x & TAG_MASK ) == TAG_STRING;
}

inline bool ovalue::is_expand() const
{
    return ( x & TAG_MASK ) == TAG_EXPAND;
}

inline bool ovalue::is_native() const
{
    return ( x & TAG_MASK ) == TAG_NATIVE;
}


inline bool ovalue::as_bool() const
{
    if ( x == VALUE_FALSE )
        return false;
    else if ( x == VALUE_TRUE )
        return true;
    else
        throw oerror( "expected bool" );
}

inline double ovalue::as_number() const
{
    if ( is_number() )
        return n;
    else
        throw oerror( "expected number" );
}

inline ostring* ovalue::as_string() const
{
    if ( is_string() )
        return (ostring*)( x & POINTER_MASK );
    else
        throw oerror( "expected string" );
}

inline oexpand* ovalue::as_expand() const
{
    if ( is_expand() )
        return (oexpand*)( x & POINTER_MASK );
    else
        throw oerror( "expected object" );
}

inline void* ovalue::as_native() const
{
    if ( is_native() )
        return (void*)( x & POINTER_MASK );
    else
        throw oerror( "expected native pointer" );
}



inline bool operator == ( ovalue a, ovalue b )
{
    if ( ! a.is_string() )
    {
        return ( a.x == b.x ) &&
                !( ( a.x & ovalue::SIGN_MASK ) == ovalue::POSITIVE_NAN );
    }
    else
    {
        ostring* astr = a.as_string();
        ostring* bstr = b.as_string();
        if ( astr == bstr )
            return true;
        if ( astr->size() != bstr->size() )
            return false;
        if ( astr->hash() != bstr->hash() )
            return false;
        return ostring::strcmp( astr, bstr ) == 0;
    }
}

inline bool operator != ( ovalue a, ovalue b )
{
    return !( a == b );
}

inline bool operator <  ( ovalue a, ovalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ostring::strcmp( a.as_string(), b.as_string() ) < 0;
}

inline bool operator <= ( ovalue a, ovalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ostring::strcmp( a.as_string(), b.as_string() ) <= 0;
}

inline bool operator >  ( ovalue a, ovalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ostring::strcmp( a.as_string(), b.as_string() ) > 0;
}

inline bool operator >= ( ovalue a, ovalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ostring::strcmp( a.as_string(), b.as_string() ) >= 0;
}


namespace std
{
template <> struct hash< ovalue >
{
    size_t operator () ( ovalue v )
    {
        if ( v.is_string() )
            return v.as_string()->hash();
        else
            return std::hash< uintptr_t >()( v.x );
    }
};
}






#endif
