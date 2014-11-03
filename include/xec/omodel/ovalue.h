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
    Values are manipulated in NaN-encoding.  We assume that GC pointers fit
    in 48 bits on all platforms.  Strings, and expands have different tag
    bits (despite all being GC objects) to speed up type checks for string
    operations and lookups.  Tag bits at the bottom of a value allow fast
    conversion to and from the tagged-encoding.  Tag bits at the top of the
    value allow type checks on 32-bit systems without manipulations of the
    full 64-bit value.
 
 
    NaN-encoding:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

    nan       s111111111111000000000000000000000000000000000000000000000000000
    special   11111111111111110000000000000000ssssssssssssssssssssssssssssss11
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppp10
    object    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppp01
    expand    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppp00

    
    On 32-bit systems references which are visible to the GC must be stored
    in tagged-encoding, because reference updates must be atomic.  Atomic
    64-bit accesses are slow on these platforms.  Numbers cannot be stored
    in tagged-encoding - expand objects that store arbitrary values potentially
    store both a tagged-encoding and a separate double.
    

    Tagged-encoding:
    
    special   ssssssssssssssssssssssssssssss11
    string    pppppppppppppppppppppppppppppp10
    object    pppppppppppppppppppppppppppppp01
    expand    pppppppppppppppppppppppppppppp00


    Special values are clustered at the top of the unsigned space:

    undefined ...001111
    null      ...001011
    false     ...000111
    true      ...000011


*/


#if 1//! defined( __x86_64__ ) && ! defined( __arm64__ )
#define IS32BIT 1
#endif




/*
    ovalues are NaN-boxed values.
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
    bool        is_integer() const;
    
    bool        is_reference() const;
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

#if IS32BIT
    friend class otagref;
#else
    friend class owb< ovalue >;
#endif

    friend bool operator == ( ovalue a, ovalue b );
    friend struct std::hash< ovalue >;
    
    static const uint64_t POSITIVE_NAN      = UINT64_C( 0x7FF8000000000000 );
    static const uint64_t NEGATIVE_NAN      = UINT64_C( 0xFFF8000000000000 );
    static const uint64_t POSITIVE_ZERO     = UINT64_C( 0x0000000000000000 );
    static const uint64_t NEGATIVE_ZERO     = UINT64_C( 0x8000000000000000 );
    static const uint64_t SIGN_MASK         = UINT64_C( 0x7FFFFFFFFFFFFFFF );

    static const uint64_t VALUE_NULL        = UINT64_C( 0xFFFF00000000000F );
    static const uint64_t VALUE_UNDEFINED   = UINT64_C( 0xFFFF00000000000B );
    static const uint64_t VALUE_FALSE       = UINT64_C( 0xFFFF000000000007 );
    static const uint64_t VALUE_TRUE        = UINT64_C( 0xFFFF000000000003 );

#if IS32BIT
    static const uint32_t HI_TAG_SPECIAL    = UINT32_C( 0xFFFF0000 );
    static const uint32_t HI_TAG_STRING     = UINT32_C( 0xFFFE0000 );
    static const uint32_t HI_TAG_OBJECT     = UINT32_C( 0xFFFD0000 );
    static const uint32_t HI_TAG_EXPAND     = UINT32_C( 0xFFFC0000 );

    static const uint32_t HI_MAX_FLOAT      = UINT32_C( 0xFFF80000 );
    
    static const uint32_t LO_POINTER_MASK   = UINT32_C( 0xFFFFFFFC );
#endif

    static const uint64_t TAG_SPECIAL       = UINT64_C( 0xFFFF000000000003 );
    static const uint64_t TAG_STRING        = UINT64_C( 0xFFFD000000000002 );
    static const uint64_t TAG_OBJECT        = UINT64_C( 0xFFFE000000000001 );
    static const uint64_t TAG_EXPAND        = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_REFERENCE     = UINT64_C( 0xFFFEFFFFFFFFFFFF );
    static const uint64_t MIN_REFERENCE     = UINT64_C( 0xFFFC000000000000 );

    static const uint64_t MAX_FLOAT         = UINT64_C( 0xFFF8000000000000 );

    static const uint64_t TAG_MASK          = UINT64_C( 0xFFFF000000000003 );
    static const uint64_t POINTER_MASK      = UINT64_C( 0x0000FFFFFFFFFFFC );


    explicit ovalue( uint64_t x );
    void* p() const;
    
    union
    {
        double      n;
        uint64_t    x;
#if IS32BIT
        uint32_t    lo;
        uint32_t    hi;
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




#if IS32BIT



/*
    On 32-bit systems write-barriered ovalues are not valid.  otagref stores
    a tagged reference instead - you must arrange to store numbers elsewhere.
*/


class otagref
{
public:

    otagref();
    otagref( ovalue q );
    otagref& operator = ( ovalue q );
    otagref& operator = ( const otagref& q );
    
    operator ovalue () const;

private:

    static const uintptr_t VALUE_UNDEFINED  = 0x0000000B;

    static const uintptr_t TAG_SPECIAL      = 0x00000003;
    static const uintptr_t TAG_STRING       = 0x00000002;
    static const uintptr_t TAG_OBJECT       = 0x00000001;
    static const uintptr_t TAG_EXPAND       = 0x00000000;

    static const uintptr_t TAG_MASK         = 0x00000003;
    static const uintptr_t POINTER_MASK     = 0xFFFFFFFC;
    
    static const uint64_t  OVALUE_BITS      = UINT64_C( 0xFFFC000000000000 );

    std::atomic< uintptr_t > x;

};


template <>
struct omark< ovalue >
{
    /* left empty to prevent use of owb< ovalue > */
};


#else



/*
    On 64-bit systems we can use a normal write barrier.
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



#endif


/*

*/


#include <math.h>
#include "ostring.h"
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
#if IS32BIT
    return hi <= HI_MAX_FLOAT;
#else
    return x <= MAX_FLOAT;
#endif
}


inline bool ovalue::is_reference() const
{
#if IS32BIT
    return hi >= HI_TAG_EXPAND && hi <= HI_TAG_STRING;
#else
    return x >= MIN_REFERENCE && x <= MAX_REFERENCE;
#endif
}

inline bool ovalue::is_object() const
{
#if IS32BIT
    return hi == HI_TAG_OBJECT;
#else
    return ( x & TAG_MASK ) == TAG_OBJECT;
#endif
}

inline bool ovalue::is_string() const
{
#if IS32BIT
    return hi == HI_TAG_STRING;
#else
    return ( x & TAG_MASK ) == TAG_STRING;
#endif
}

inline bool ovalue::is_expand() const
{
#if IS32BIT
    return hi == HI_TAG_EXPAND;
#else
    return ( x & TAG_MASK ) == TAG_EXPAND;
#endif
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
        return (obase*)p();
    else
        throw oerror( "expected object" );
}

inline ostring* ovalue::as_string() const
{
    if ( is_string() )
        return (ostring*)p();
    else
        throw oerror( "expected string" );
}

inline oexpand* ovalue::as_expand() const
{
    if ( is_expand() )
        return (oexpand*)p();
    else
        throw oerror( "expected object" );
}

inline void* ovalue::p() const
{
#if IS32BIT
    return (void*)(uintptr_t)( lo & LO_POINTER_MASK );
#else
    return (void*)( x & POINTER_MASK );
#endif
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




#if IS32BIT


/*
    otagref
*/


inline otagref::otagref()
    :   x( VALUE_UNDEFINED )
{
}

inline otagref::otagref( ovalue q )
    :   x( (uintptr_t)q.x )
{
}

inline otagref& otagref::operator = ( ovalue q )
{
    // Perform write barrier on previous value.
    uintptr_t x = this->x.load( std::memory_order_relaxed );
    if ( ( x & TAG_MASK ) != TAG_SPECIAL )
    {
        ocolour mark_colour = ocontext::context->mark_colour;
        if ( ( x & TAG_MASK ) == TAG_STRING )
        {
            ostring* s = (ostring*)( x & POINTER_MASK );
            s->mark( mark_colour, mark_colour );
        }
        else
        {
            obase* o = (obase*)( x & POINTER_MASK );
            if ( o->mark( mark_colour, O_GREY ) )
            {
                ocontext::context->heap->marked_grey( o );
            }
        }
    }
    
    
    // Store new value.
    assert( ! q.is_number() );
    this->x.store( (uintptr_t)q.x, std::memory_order_relaxed );
    return *this;
}

inline otagref& otagref::operator = ( const otagref& q )
{
    return this->operator = ( (ovalue)q );
}

inline otagref::operator ovalue () const
{
    uintptr_t p = this->x.load( std::memory_order_relaxed );
    return ovalue( OVALUE_BITS | ( p & TAG_MASK ) << 48 | p );
}



#else


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
        ocolour mark_colour = ocontext::context->mark_colour;
        if ( q.is_string() )
        {
            ostring* s = (ostring*)q.p();
            s->mark( mark_colour, mark_colour );
        }
        else
        {
            obase* o = (obase*)q.p();
            if ( o->mark( mark_colour, O_GREY ) )
            {
                ocontext::context->heap->marked_grey( o );
            }
        }
    }
    
    // Store new value.
    this->x.store( q.x, std::memory_order_relaxed );
    return *this;
}

inline owb< ovalue >& owb< ovalue >::operator = ( const owb& q )
{
    return this->operator = ( (ovalue)q );
}



#endif



#endif



