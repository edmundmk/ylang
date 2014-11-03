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
#include "obase.h"


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
    the desirable property that 'falsey' values are clustered together at the
    top of the value space (when seen as an unsigned integer):

                 F   F   F   x   0   0   0   0   0   0   0   0   0   0   0   0
    nan       s111111111111000000000000000000000000000000000000000000000000000
    null      1111111111111111111111111111111111111111111111111111111111111111
    undefined 1111111111111111111111111111111111111111111111111111111111111110
    false     1111111111111111111111111111111111111111111111111111111111111101
    true      1111111111111111111111111111111111111111111111111111111111111100
    string    1111111111111110pppppppppppppppppppppppppppppppppppppppppppppp00
    expand    1111111111111101pppppppppppppppppppppppppppppppppppppppppppppp00
    object    1111111111111100pppppppppppppppppppppppppppppppppppppppppppppp00

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
    bool        is_integer() const;
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
    On 64-bit systems this is an atomic version of the value bits.

    But we have a problem with concurrent GC on 32-bit systems, as on these
    systems (x86, ARM32) atomic 64-bit loads and stores are expensive.  On
    these systems we fall back to a tagged-pointer representation.  Most of
    the time values do actually fit in 32 bits.
 
            special         ------------------------------11
                null        11111111111111111111111111111111
                undefined   11111111111111111111111111111011
                false       11111111111111111111111111110111
                true        11111111111111111111111111110011
            signed integer  iiiiiiiiiiiiiiiiiiiiiiiiiiiiii10
            boxed number    pppppppppppppppppppppppppppppp01
            object          pppppppppppppppppppppppppppppp00
    
    Any numbers which aren't 31-bit signed integers get boxed.  Note that
    we only compress values into this representation when they are actually
    stored into the write-barriered value.
  
    We also reuse boxes as values are updated - if a boxed double is assigned
    into a slot it's likely that subsequent stores will also be numbers.  We
    assume that numbers in different objects are likely to be different, or at
    least the results of different calculations.
    
    I'm not sure if small ints are a win or not, considering we have to check
    for them on every assignment.  The special cases (string, expand, small
    ints, boxed floats) are adding up.  Might be better to box _all_ numbers
    and to treat _all_ objects as of unknown type (including in the write
    barrier and when marking - means adding strings to the grey list).

*/


#if ! defined( __x86_64__ ) && ! defined( __arm64__ )
#define OVALUE_BOXING 1
#endif



#ifdef OVALUE_BOXING

class oboxed : public obase
{
public:

    static oboxed* alloc( double number );
    
    void    set( double number );
    double  get() const;


protected:

    friend class obase;
    static ometatype metatype;
    static void mark_boxed( oworklist* work, obase* object, ocolour colour );

    oboxed( ometatype* metatype, double number );

    
private:

    double number;

};

#endif


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

#ifdef OVALUE_BOXING

    static const uint64_t  SPECIAL_HIBITS   = INT64_C( 0xFFFFFFFFC0000000 );
    static const uintptr_t VALUE_UNDEFINED  = 0xFFFFFFFBu;
    static const uintptr_t TAG_SPECIAL      = 0x00000003u;
    static const uintptr_t TAG_INTEGER      = 0x00000002u;
    static const uintptr_t TAG_BOXED        = 0x00000001u;
    static const uintptr_t TAG_OBJECT       = 0x00000000u;
    static const uintptr_t TAG_MASK         = 0x00000003u;
    static const uintptr_t POINTER_MASK     = 0xFFFFFFFCu;
    static const intptr_t  MAX_INTEGER      = 0x1FFFFFFF;
    static const intptr_t  MIN_INTEGER      = -MAX_INTEGER - 1;
    
    std::atomic< uintptr_t > v;

#else

    std::atomic< uint64_t > v;

#endif
    
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
#include "oexpand.h"


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


#ifndef OVALUE_BOXING


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

inline owb< ovalue >::operator ovalue () const
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
    // Reading reference from mark thread, must be a consume operation.
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


#else


inline oboxed* oboxed::alloc( double number )
{
    void* p = malloc( sizeof( oboxed ) );
    return new ( p ) oboxed( &metatype, number );
}

inline oboxed::oboxed( ometatype* metatype, double number )
    :   obase( metatype )
    ,   number( number )
{
}

inline void oboxed::set( double number )
{
    this->number = number;
}

inline double oboxed::get() const
{
    return number;
}



inline owb< ovalue >::owb()
    :   v( VALUE_UNDEFINED )
{
}

inline owb< ovalue >& owb< ovalue >::operator = ( ovalue q )
{
    // Check previous value, marking it.
    uintptr_t v = this->v.load( std::memory_order_relaxed );
    uintptr_t tag = v & TAG_MASK;
    ocolour mark_colour = ocontext::context->mark_colour;
    
    if ( tag == TAG_OBJECT )
    {
        obase* p = (obase*)v;
        if ( p->is< ostring >() )
        {
            // Mark strings without adding to grey list.
            p->mark( mark_colour, mark_colour );
        }
        else
        {
            // Add to grey list.
            if ( p->mark( mark_colour, O_GREY ) )
            {
                ocontext::context->heap->marked_grey( p );
            }
        }
    }
    else if ( tag == TAG_BOXED )
    {
        oboxed* boxed = (oboxed*)( v & POINTER_MASK );

        // If q is a number, early out without modifying reference.
        if ( q.x <= ovalue::HIGHEST_FLOAT )
        {
            boxed->set( q.n );
            return *this;
        }
        
        // Otherwise mark boxed without adding to grey list.
        boxed->mark( mark_colour, mark_colour );
    }
    

    // Box value.
    if ( q.x <= ovalue::HIGHEST_FLOAT )
    {
        intptr_t i = (intptr_t)q.n;
        if ( i == q.n && i >= MIN_INTEGER && i <= MAX_INTEGER )
        {
            this->v.store( i << 2 | TAG_INTEGER, std::memory_order_relaxed );
        }
        else
        {
            oboxed* b = oboxed::alloc( q.n );
            this->v.store( (uintptr_t)b | TAG_BOXED, std::memory_order_relaxed );
        }
    }
    else if ( q.x >= ovalue::TAG_OBJECT && q.x < ovalue::TAG_MASK )
    {
        obase* p = (obase*)( q.x & ovalue::POINTER_MASK );
        this->v.store( (uintptr_t)p | TAG_OBJECT, std::memory_order_relaxed );
    }
    else
    {
        assert( ( q.x & SPECIAL_HIBITS ) == SPECIAL_HIBITS );
        uintptr_t s = (uintptr_t)q.x << 2 | TAG_SPECIAL;
        this->v.store( s, std::memory_order_relaxed );
    }
    
    return *this;
}

inline owb< ovalue >& owb< ovalue >::operator = ( const owb& q )
{
    // Could do a fast-path since the incoming value already uses boxing.
    // Remember, however, that boxed numbers are mutable so we'd need our
    // own copy of any boxed value.  For now, unpack and repack value.
    return this->operator = ( (ovalue)q );
}

inline owb< ovalue >::operator ovalue () const
{
    return value();
}

inline ovalue owb< ovalue >::value() const
{
    // Reconstruct boxed value.
    uintptr_t v = this->v.load( std::memory_order_relaxed );
    uintptr_t tag = v & TAG_MASK;
    if ( tag == TAG_OBJECT )
    {
        obase* p = (obase*)v;
        if ( p->is< ostring >() )
            return ovalue( (ostring*)p );
        else if ( p->is< oexpand >() )
            return ovalue( (oexpand*)p );
        else
            return ovalue( p );
    }
    else if ( tag == TAG_BOXED )
    {
        oboxed* b = (oboxed*)( v & POINTER_MASK );
        return ovalue( b->get() );
    }
    else if ( tag == TAG_INTEGER )
    {
        intptr_t i = (intptr_t)v >> 2;
        return ovalue( (double)i );
    }
    else if ( tag == TAG_SPECIAL )
    {
        return ovalue( SPECIAL_HIBITS | v >> 2 );
    }
    
    assert( ! "unknown boxed tag" );
    return ovalue();
}


inline void omark< ovalue >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // Reading reference from mark thread, must be a consume operation.
    uintptr_t v = value.v.load( std::memory_order_consume );
    
    // Check pointer tag.
    uintptr_t tag = v & wb_type::TAG_MASK;
    if ( tag == wb_type::TAG_OBJECT )
    {
        obase* p = (obase*)v;
        if ( p->is< ostring >() )
        {
            // Mark string values directly.
            p->mark( colour, colour );
        }
        else
        {
            // Mark objects grey and add to work list.
            if ( p->mark( colour, O_GREY ) )
            {
                work->push_back( p );
            }
        }
    }
    else if ( tag == wb_type::TAG_BOXED )
    {
        // Mark boxed values directly.
        oboxed* b = (oboxed*)( v & wb_type::POINTER_MASK );
        b->mark( colour, colour );
    }
}




#endif




#endif
