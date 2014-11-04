//
//  ovalue.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OVALUE_H
#define OVALUE_H



#include <cstddef>
#include <stdint.h>
#include <atomic>
#include <functional>
#include "oheap.h"


class ostring;
class oexpand;
class oslotlist;



/*
    ovalues are NaN-boxed values.  We assume that GC pointers fit in 48 bits
    on all platforms.  Strings and expands have different tag bits (despite
    all being GC objects) to speed up type checks for string operations and
    lookups.
 
 
    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    
    nan       s111111111111000000000000000000000000000000000000000000000000000
    undefined 1111111111111111111111111111111111111111111111111111111111111111
    null      1111111111111111111111111111111011111111111111111111111111111111
    false     1111111111111111111111111111110111111111111111111111111111111111
    true      1111111111111111111111111111110011111111111111111111111111111111
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppppp
    object    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppppp
    expand    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppppp

*/


#if defined( __x86_64__ ) || defined( __arm64__ )
#define OVALUE64 1
#else
#define OVALUE32 1
#endif


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

    friend class oslotlist;

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
    static const uint64_t VALUE_NULL        = UINT64_C( 0xFFFFFFFEFFFFFFFF );
    static const uint64_t VALUE_FALSE       = UINT64_C( 0xFFFFFFFDFFFFFFFF );
    static const uint64_t VALUE_TRUE        = UINT64_C( 0xFFFFFFFCFFFFFFFF );

    static const uint64_t TAG_STRING        = UINT64_C( 0xFFFE000000000000 );
    static const uint64_t TAG_OBJECT        = UINT64_C( 0xFFFD000000000000 );
    static const uint64_t TAG_EXPAND        = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_REFERENCE     = UINT64_C( 0xFFFEFFFFFFFFFFFF );
    static const uint64_t MIN_REFERENCE     = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_FLOAT         = UINT64_C( 0xFFF8000000000000 );

    static const uint64_t TAG_MASK          = UINT64_C( 0xFFFF000000000000 );
    static const uint64_t POINTER_MASK      = UINT64_C( 0x0000FFFFFFFFFFFF );

    explicit ovalue( uint64_t x );
#if OVALUE32
    explicit ovalue( uint32_t lo, uint32_t hi );
#endif
    
    union
    {
        double   n;
        uint64_t x;
#if OVALUE32
        struct
        {
            uint32_t lo;
            uint32_t hi;
        };
#endif
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




#if OVALUE64


/*
    ovalues should only be write-barriered directly on platforms with fast
    64-bit atomic accesses.
*/



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


#else


/*
    Prevent write-barrers for ovalues.
*/

template <> class owb< ovalue >;
template <> struct omark< ovalue >;


#endif





/*

*/





#include <math.h>
#include "ostring.h"
#include "oexpand.h"
#include "oerror.h"


#if OVALUE32
#define HI( value ) (uint32_t)( value >> 32 )
#endif


/*
    ovalue
*/


inline ovalue::ovalue( uint64_t x )
    :   x( x )
{
}

#if OVALUE32

inline ovalue::ovalue( uint32_t lo, uint32_t hi )
    :   lo( lo )
    ,   hi( hi )
{
}

#endif

inline ovalue::ovalue()
    :   x( VALUE_NULL )
{
}

inline ovalue::ovalue( std::nullptr_t )
    :   x( VALUE_NULL )
{
}

inline ovalue::ovalue( bool value )
    :   x( value ? VALUE_TRUE : VALUE_FALSE )
{
}

inline ovalue::ovalue( double value )
    :   n( ! isnan( value ) ? value : signbit( value ) ? NEGATIVE_NAN : POSITIVE_NAN  )
{
}

inline ovalue::ovalue( const char* string )
    :   x( TAG_STRING | (uintptr_t)( ostring::alloc( string ) ) )
{
}

inline ovalue::ovalue( obase* object )
    :   x( object ? TAG_OBJECT | (uintptr_t)object : VALUE_NULL )
{
    assert( ! object || ( ! object->is< ostring >() && ! object->is< oexpand >() ) );
}

inline ovalue::ovalue( ostring* string )
    :   x( string ? TAG_STRING | (uintptr_t)string : VALUE_NULL )
{
}

inline ovalue::ovalue( oexpand* expand )
    :   x( expand ? TAG_EXPAND | (uintptr_t)expand : VALUE_NULL )
{
}


inline ovalue::operator bool() const
{
#if OVALUE64
    return ( x < VALUE_FALSE ) && ( x & SIGN_MASK ) != POSITIVE_ZERO;
#else
    return ( hi < HI( VALUE_FALSE ) ) && ( n != 0.0 );
#endif
}

inline const char* ovalue::c_str() const
{
    return as_string()->c_str();
}


inline bool ovalue::is_null() const
{
#if OVALUE64
    return x == VALUE_NULL;
#else
    return hi == HI( VALUE_NULL );
#endif
}

inline bool ovalue::is_undefined() const
{
#if OVALUE64
    return x == VALUE_UNDEFINED;
#else
    return hi == HI( VALUE_UNDEFINED );
#endif
}

inline bool ovalue::is_bool() const
{
#if OVALUE64
    return x == VALUE_FALSE || x == VALUE_TRUE;
#else
    return hi == HI( VALUE_FALSE ) || x == HI( VALUE_TRUE );
#endif
}

inline bool ovalue::is_number() const
{
#if OVALUE64
    return x <= MAX_FLOAT;
#else
    return hi <= HI( MAX_FLOAT );
#endif
}

inline bool ovalue::is_object() const
{
#if OVALUE64
    return x >= MIN_REFERENCE && x <= MAX_REFERENCE;
#else
    return hi >= HI( MIN_REFERENCE ) && hi <= HI( MAX_REFERENCE );
#endif
}

inline bool ovalue::is_string() const
{
#if OVALUE64
    return ( x & TAG_MASK ) == TAG_STRING;
#else
    return hi == HI( TAG_STRING );
#endif
}

inline bool ovalue::is_expand() const
{
#if OVALUE64
    return ( x & TAG_MASK ) == TAG_EXPAND;
#else
    return hi == HI( TAG_EXPAND );
#endif
}

inline bool ovalue::as_bool() const
{
#if OVALUE64
    if ( x == VALUE_FALSE )
        return false;
    else if ( x == VALUE_TRUE )
        return true;
    else
        throw oerror( "expected bool" );
#else
    if ( hi == HI( VALUE_FALSE ) )
        return false;
    else if ( hi == HI( VALUE_TRUE ) )
        return true;
    else
        throw oerror( "expected bool" );
#endif
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
#if OVALUE64
    if ( is_object() )
        return (obase*)( x & POINTER_MASK );
    else
        throw oerror( "expected object" );
#else
    if ( is_object() )
        return (obase*)lo;
    else
        throw oerror( "expected object" );
#endif
}

inline ostring* ovalue::as_string() const
{
#if OVALUE64
    if ( is_string() )
        return (ostring*)( x & POINTER_MASK );
    else
        throw oerror( "expected string" );
#else
    if ( is_string() )
        return (ostring*)lo;
    else
        throw oerror( "expected string" );
#endif
}

inline oexpand* ovalue::as_expand() const
{
#if OVALUE64
    if ( is_expand() )
        return (oexpand*)( x & POINTER_MASK );
    else
        throw oerror( "expected object" );
#else
    if ( is_expand() )
        return (oexpand*)lo;
    else
        throw oerror( "expected object" );
#endif
}


#ifdef OVALUE32
#undef HI
#endif




inline bool operator == ( ovalue a, ovalue b )
{
    if ( a.is_number() )
    {
        return a.n == b.n;
    }
    else if ( ! a.is_string() )
    {
        return a.x == b.x;
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



#if OVALUE64


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
    // On mark thread must use consume memory ordering.
    uint64_t x = value.x.load( std::memory_order_consume );
    if ( x >= ovalue::MIN_REFERENCE && x <= ovalue::MAX_REFERENCE )
    {
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        p->mark( work, colour );
    }
}



#endif




#endif
