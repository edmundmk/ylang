//
//  ovalue64.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OVALUE64_H
#define OVALUE64_H



#include <cstddef>
#include <stdint.h>
#include <atomic>
#include <functional>
#include "oheap.h"


class ostring;
class oexpand;


#define OVALUE64 1



/*
    On 64-bit systems ovalues are NaN-boxed values.  We assume that GC
    pointers fit in 48 bits on all platforms.  Strings and expands have
    different tag bits (despite all being GC objects) to speed up type
    checks for string operations and lookups.
 
 
    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

    nan       s111111111111000000000000000000000000000000000000000000000000000
    undefined 1111111111111111111111111111111111111111111111111111111111111111
    null      1111111111111111111111111111111111111111111111111111111111111110
    false     1111111111111111111111111111111111111111111111111111111111111101
    true      1111111111111111111111111111111111111111111111111111111111111100
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppppp
    object    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppppp
    expand    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppppp



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
    
    static const uint64_t POSITIVE_NAN      = UINT64_C( 0x7FF8000000000000 );
    static const uint64_t NEGATIVE_NAN      = UINT64_C( 0xFFF8000000000000 );
    static const uint64_t POSITIVE_ZERO     = UINT64_C( 0x0000000000000000 );
    static const uint64_t NEGATIVE_ZERO     = UINT64_C( 0x8000000000000000 );
    static const uint64_t SIGN_MASK         = UINT64_C( 0x7FFFFFFFFFFFFFFF );

    static const uint64_t VALUE_UNDEFINED   = UINT64_C( 0xFFFFFFFFFFFFFFFF );
    static const uint64_t VALUE_NULL        = UINT64_C( 0xFFFFFFFFFFFFFFFE );
    static const uint64_t VALUE_FALSE       = UINT64_C( 0xFFFFFFFFFFFFFFFD );
    static const uint64_t VALUE_TRUE        = UINT64_C( 0xFFFFFFFFFFFFFFFC );

    static const uint64_t TAG_STRING        = UINT64_C( 0xFFFE000000000000 );
    static const uint64_t TAG_OBJECT        = UINT64_C( 0xFFFD000000000000 );
    static const uint64_t TAG_EXPAND        = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_REFERENCE     = UINT64_C( 0xFFFEFFFFFFFFFFFF );
    static const uint64_t MIN_REFERENCE     = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_FLOAT         = UINT64_C( 0xFFF8000000000000 );

    static const uint64_t TAG_MASK          = UINT64_C( 0xFFFF000000000000 );
    static const uint64_t POINTER_MASK      = UINT64_C( 0x0000FFFFFFFFFFFF );

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




template <>
class owb< ovalue >
{
public:

    owb();
    owb( ovalue q );
    owb& operator = ( ovalue q );
    owb& operator = ( const owb& q );

    operator ovalue () const;


private:

    friend struct omark< ovalue >;
    
    std::atomic< uint64_t > x;
 
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
#include "oexpand.h"
#include "oerror.h"



/*
    ovalue
*/


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
    :   ovalue( object ? TAG_OBJECT | (uintptr_t)object : VALUE_NULL )
{
    assert( ! object || ( ! object->is< ostring >() && ! object->is< oexpand >() ) );
}

inline ovalue::ovalue( ostring* string )
    :   ovalue( string ? TAG_STRING | (uintptr_t)string : VALUE_NULL )
{
}

inline ovalue::ovalue( oexpand* expand )
    :   ovalue( expand ? TAG_EXPAND | (uintptr_t)expand : VALUE_NULL )
{
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
    return x <= MAX_FLOAT;
}

inline bool ovalue::is_object() const
{
    return ( x & TAG_MASK ) == TAG_OBJECT
        || ( x & TAG_MASK ) == TAG_STRING
        || ( x & TAG_MASK ) == TAG_EXPAND;
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
        throw oerror( "expected object" );
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
        if ( ! b.is_string() )
            return false;
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


inline size_t std::hash< ovalue >::operator () ( const ovalue& v ) const
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
    :   x( ovalue::VALUE_UNDEFINED )
{
}

inline owb< ovalue >::owb( ovalue q )
    :   x( q.x )
{
}

inline owb< ovalue >& owb< ovalue >::operator = ( ovalue q )
{
    // Perform write barrier on old value.
    uint64_t x = this->x.load( std::memory_order_relaxed );
    if ( x >= ovalue::MIN_REFERENCE && x <= ovalue::MAX_REFERENCE )
    {
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        p->mark();
    }
    
    // Store new value.
    this->x.store( q.x, std::memory_order_relaxed );
    return *this;
}

inline owb< ovalue >& owb< ovalue >::operator = ( const owb& q )
{
    return this->operator = ( (ovalue)q );
}

inline owb< ovalue >::operator ovalue () const
{
    return ovalue( x.load( std::memory_order_relaxed ) );
}



inline void omark< ovalue >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    uint64_t x = value.x.load( std::memory_order_consume );
    if ( x >= ovalue::MIN_REFERENCE && x <= ovalue::MAX_REFERENCE )
    {
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        p->mark( work, colour );
    }
}




#endif
