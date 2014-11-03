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


#if ! defined( __x86_64__ ) && ! defined( __arm64__ )
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
    
    void    store( ovalue v );
    ovalue  load() const;

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
    owb& operator = ( ovalue q );
    owb& operator = ( const owb& q );

    operator ovalue () const;


private:

    friend struct omark< ovalue >;
    
    static const uint64_t MIN_REFERENCE = UINT64_C( 0xFFFC000000000000 );
    static const uint64_t MAX_REFERENCE = UINT64_C( 0xFFFEFFFFFFFFFFFF );

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
#if IS32BIT
    return hi <= HI_MAX_FLOAT;
#else
    return x <= MAX_FLOAT;
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

inline void otagref::store( ovalue v )
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
    assert( ! v.is_number() );
    this->x.store( (uintptr_t)v.x, std::memory_order_relaxed );
}


inline ovalue otagref::load() const
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

inline owb< ovalue >& owb< ovalue >::operator = ( ovalue q )
{
    // Perform write barrier on old value.
    uint64_t x = this->x.load( std::memory_order_relaxed );
    if ( x >= MIN_REFERENCE && x <= MAX_REFERENCE )
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



#if OLD




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
        // Not sure if it's a win to do this here or if it would be better
        // to just store obase* in ovalues and check for strings/expands
        // when you try and perform operations on them.
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
