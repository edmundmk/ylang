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
#include <atomic>
#include <functional>
#include "oheap.h"


class obase;
class ostring;
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
    Bit patterns of non-float values are all negative quiet NaNs.  This has
    the desirable property that 'falsey' values are clustered together as the
    maximum possible values:

                 F   F   F   x   0   0   0   0   0   0   0   0   0   0   0   0
    nan       s111111111111000000000000000000000000000000000000000000000000000
    null      1111111111111111111111111111111111111111111111111111111111111111
    undefined 1111111111111111111111111111111111111111111111111111111111111110
    false     1111111111111111111111111111111111111111111111111111111111111101
    true      1111111111111111111111111111111111111111111111111111111111111100
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppppp
    expand    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppppp
    object    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppppp

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
    ovalue( obase* object );
    ovalue( ostring* string );
    ovalue( oexpand* expand );

    explicit operator bool() const;
    const char* c_str() const;

    bool        is_null() const;
    bool        is_undefined() const;
    bool        is_bool() const;
    bool        is_number() const;
    bool        is_object() const;
    bool        is_string() const;
    bool        is_expand() const;

    bool        as_bool() const;
    double      as_number() const;
    obase*      as_object() const;
    ostring*    as_string() const;
    oexpand*    as_expand() const;
    
    obase*      get() const;
    

private:

    friend class owb< ovalue >;
    friend struct omark< ovalue >;
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
    static const uint64_t TAG_OBJECT        = INT64_C( 0xFFFC000000000000 );
    static const uint64_t TAG_MASK          = INT64_C( 0xFFFF000000000000 );
    static const uint64_t POINTER_MASK      = INT64_C( 0x0000FFFFFFFFFFFF );
    static const uint64_t HIGHEST_FLOAT     = INT64_C( 0xFFF8000000000000 );
    
    explicit ovalue( uint64_t x );
    
    union
    {
        double      n;
        uint64_t    x;
    };

};


bool operator == ( ovalue a, ovalue b );
bool operator != ( ovalue a, ovalue b );
bool operator <  ( ovalue a, ovalue b );
bool operator <= ( ovalue a, ovalue b );
bool operator >  ( ovalue a, ovalue b );
bool operator >= ( ovalue a, ovalue b );


namespace std
{
template <> struct hash< ovalue >
{
    size_t operator () ( const ovalue& v ) const;
};
}




/*
    As ovalues can be references they need an atomic, write-barriered type.
*/

template <>
class owb< ovalue >
{
public:

    owb();
    owb& operator = ( ovalue q );
    owb& operator = ( const owb& q );

    operator ovalue () const;

    explicit operator bool() const      { return value().operator bool(); }
    const char* c_str() const           { return value().c_str(); }

    bool        is_null() const         { return value().is_null(); }
    bool        is_undefined() const    { return value().is_undefined(); }
    bool        is_bool() const         { return value().is_bool(); }
    bool        is_number() const       { return value().is_number(); }
    bool        is_object() const       { return value().is_object(); }
    bool        is_string() const       { return value().is_string(); }
    bool        is_expand() const       { return value().is_expand(); }

    bool        as_bool() const         { return value().as_bool(); }
    double      as_number() const       { return value().as_number(); }
    obase*      as_object() const       { return value().as_object(); }
    ostring*    as_string() const       { return value().as_string(); }
    oexpand*    as_expand() const       { return value().as_expand(); }

    obase*      get() const;


private:

    friend struct omark< ovalue >;

    ovalue value() const;

    std::atomic< uint64_t > v;
    
};


template <>
struct omark< ovalue >
{
    typedef owb< ovalue > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};





/*

*/


#include <math.h>
#include "ostring.h"
#include "oerror.h"


inline ovalue::ovalue( uint64_t x )
    :   x( x )
{
}

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

inline ovalue::ovalue( obase* object )
    :   ovalue( TAG_OBJECT | (uintptr_t)object )
{
    assert( object );
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


inline ovalue::operator bool() const
{
    return ( x < VALUE_FALSE ) && ( x & SIGN_MASK ) != POSITIVE_ZERO;
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

inline bool ovalue::is_object() const
{
    return ( x & TAG_MASK ) == TAG_OBJECT;
}

inline bool ovalue::is_string() const
{
    return ( x & TAG_MASK ) == TAG_STRING;
}

inline bool ovalue::is_expand() const
{
    return ( x & TAG_MASK ) == TAG_EXPAND;
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

inline obase* ovalue::as_object() const
{
    if ( is_object() )
        return (obase*)( x & POINTER_MASK );
    else
        throw oerror( "expected native pointer" );
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


size_t std::hash< ovalue >::operator () ( const ovalue& v ) const
{
    if ( v.is_string() )
        return v.as_string()->hash();
    else
        return std::hash< uint64_t >()( v.x );
}




/*
    owb< ovalue >
*/


inline owb< ovalue >::owb()
    :   v( ovalue::VALUE_UNDEFINED )
{
}

inline owb< ovalue >& owb< ovalue >::operator = ( ovalue q )
{
    // Check current value and mark it if necessary.
    uint64_t x = v.load( std::memory_order_relaxed );
    uint64_t tag = x & ovalue::TAG_MASK;
    if ( tag == ovalue::TAG_OBJECT || tag == ovalue::TAG_EXPAND )
    {
        // Mark expands or other objects.
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        if ( p->mark( ocontext::context->mark_colour, O_GREY ) )
        {
            ocontext::context->heap->marked_grey( p );
        }
    }
    else if ( tag == ovalue::TAG_STRING )
    {
        // Just mark strings directly without marking them grey first.
        ostring * s = (ostring*)( x & ovalue::POINTER_MASK );
        ocolour mark_colour = ocontext::context->mark_colour;
        s->mark( mark_colour, mark_colour );
    }
    
    // Update slot.
    v.store( q.x, std::memory_order_relaxed );
    return *this;
}

inline owb< ovalue >& owb< ovalue >::operator = ( const owb& q )
{
    return this->operator = ( (ovalue)q );
}

inline owb< ovalue >::operator ovalue() const
{
    return value();
}

inline ovalue owb< ovalue >::value() const
{
    return ovalue( v.load( std::memory_order_relaxed ) );
}


inline void omark< ovalue >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // Read reference from mark thread, must be a consume operation.
    uint64_t x = value.v.load( std::memory_order_consume );

    // Mark reference appropriately (if it is a reference).
    uint64_t tag = x & ovalue::TAG_MASK;
    if ( tag == ovalue::TAG_OBJECT || tag == ovalue::TAG_EXPAND )
    {
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        if ( p->mark( colour, O_GREY ) )
        {
            work->push_back( p );
        }
    }
    else if ( tag == ovalue::TAG_STRING )
    {
        ostring * s = (ostring*)( x & ovalue::POINTER_MASK );
        s->mark( colour, colour );
    }
}




#endif
