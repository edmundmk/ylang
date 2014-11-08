//
//  yvalue.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YVALUE_H
#define YVALUE_H



#include <cstddef>
#include <stdint.h>
#include <atomic>
#include <functional>
#include "ymodel.h"
#include "yobject.h"


class ystring;
class yexpand;
class yslots;



/*
    yvalues are NaN-boxed values.
    
    We assume that GC pointers fit in 48 bits on all platforms.  Strings and
    expands have different tag bits (despite all being GC objects) to speed
    up type checks for string operations and lookups.
 
 
    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmm mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s1111111111100000000000000000000 00000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    
    nan       s1111111111110000000000000000000 00000000000000000000000000000000
    undefined 11111111111111111111111111111111 11111111111111111111111111111111
    null      11111111111111111111111111111110 11111111111111111111111111111111
    false     11111111111111111111111111111101 11111111111111111111111111111111
    true      11111111111111111111111111111100 11111111111111111111111111111111
    string    1111111111111110pppppppppppppppp pppppppppppppppppppppppppppppppp
    object    1111111111111101pppppppppppppppp pppppppppppppppppppppppppppppppp
    expand    1111111111111100pppppppppppppppp pppppppppppppppppppppppppppppppp

    
*/


class yvalue
{
public:

    static yvalue undefined;
    
    yvalue();
    yvalue( std::nullptr_t );
    yvalue( bool value );
    yvalue( double value );
    yvalue( const char* string );
    yvalue( yobject* object );
    yvalue( ystring* string );
    yvalue( yexpand* expand );

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
    yobject*    as_object() const;
    ystring*    as_string() const;
    yexpand*    as_expand() const;
    
    template < typename object_t > bool is() const;
    template < typename object_t > object_t* as() const;
    
    yobject*    get() const;
    

private:

    friend class yslots;
    friend class ywb< yvalue >;
    friend struct ymarktraits< ywb< yvalue > >;

    friend bool operator == ( yvalue a, yvalue b );
    friend struct std::hash< yvalue >;
    
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

    explicit yvalue( uint64_t x );
#if ! Y64BIT
    explicit yvalue( uint32_t lo, uint32_t hi );
#endif
    
    union
    {
        double   n;
        uint64_t x;
#if ! Y64BIT
        struct
        {
            uint32_t lo;
            uint32_t hi;
        };
#endif
    };

};


bool operator == ( yvalue a, yvalue b );
bool operator != ( yvalue a, yvalue b );
bool operator <  ( yvalue a, yvalue b );
bool operator <= ( yvalue a, yvalue b );
bool operator >  ( yvalue a, yvalue b );
bool operator >= ( yvalue a, yvalue b );


namespace std
{
template <> struct hash< yvalue >
{
    size_t operator () ( const yvalue& v ) const;
};
}




#if Y64BIT


/*
    On systems with fast 64-bit atomic accesses, the write barrier is simple.
*/

template <>
class ywb< yvalue >
{
public:

    ywb();
    ywb( const ywb& q );
    ywb( yvalue q );
    ywb& operator = ( const ywb& q );
    ywb& operator = ( yvalue q );

    operator yvalue () const;
    yvalue get() const;


private:

    friend struct ymarktraits< ywb< yvalue > >;

    std::atomic< uint64_t > x;
 
};


template <>
struct ywbtraits< yvalue >
{
    typedef ywb< yvalue > wb;
};

template <>
struct ymarktraits< ywb< yvalue > >
{
    static void mark( ywb< yvalue >& wb, yworklist* work, ycolour colour );
};



#else


/*
    On 32-bit, we have issues.

    The garbage collector must be able to decide if a value is a valid pointer
    with only one atomic access.  However, with NaN-boxing, you must check the
    entire value to determine this.  Even using one or two bits as a tag means
    that we can no longer fit doubles into our 64-bit values.

    Where feasible we avoid write-barriering yvalues directly.  See yslots and
    the dual slot logic in yclass.  However in some cases this is just too
    complicated, and we fall back to this solution, which uses 12 bytes (rather
    than the normal 8) to store a yvalue, by splitting out the reference.
    
    Other approaches considered:
        - Just use atomic 64-bit operations.  On x86, a 64-bit atomic store
          is more than 30 times slower than the same store in x86_64 on the
          same processor.
 
        - Give each object a 20-bit unique index into a global array of
          objects.  This index fits into the high word alongside the
          mantissa.  This imposes a hard limit of 1M objects.

        - Box numbers (perhaps excluding 31-bit integers).  They would only
          need to be boxed when stored into the write barrier slot, and
          we could reuse boxes.  Or, we could reduce a yvalue to 32-bits and
          use immutable boxed numbers all the time.
          
*/

template <>
class ywb< yvalue >
{
public:

    ywb();
    ywb( const ywb& q );
    ywb( yvalue q );
    ywb& operator = ( const ywb& q );
    ywb& operator = ( yvalue q );

    operator yvalue () const;
    yvalue get() const;


private:

    friend struct ymarktraits< ywb< yvalue > >;

    std::atomic< yobject* > p;
    uint32_t                lo;
    uint32_t                hi;
 
};


template <>
struct ywbtraits< yvalue >
{
    typedef ywb< yvalue > wb;
};

template <>
struct ymarktraits< ywb< yvalue > >
{
    static void mark( ywb< yvalue >& wb, oworklist* work, ocolour colour );
};


#endif





/*

*/





#include <math.h>
#include "yerror.h"
#include "ystring.h"
#include "yexpand.h"


#if ! Y64BIT
#define HI( value ) (uint32_t)( value >> 32 )
#endif


/*
    yvalue
*/


inline yvalue::yvalue( uint64_t x )
    :   x( x )
{
}

#if ! Y64BIT

inline yvalue::yvalue( uint32_t lo, uint32_t hi )
    :   lo( lo )
    ,   hi( hi )
{
}

#endif

inline yvalue::yvalue()
    :   x( VALUE_NULL )
{
}

inline yvalue::yvalue( std::nullptr_t )
    :   x( VALUE_NULL )
{
}

inline yvalue::yvalue( bool value )
    :   x( value ? VALUE_TRUE : VALUE_FALSE )
{
}

inline yvalue::yvalue( double value )
    :   n( ! isnan( value ) ? value : signbit( value ) ? NEGATIVE_NAN : POSITIVE_NAN  )
{
}

inline yvalue::yvalue( const char* string )
    :   x( TAG_STRING | (uintptr_t)( ystring::alloc( string ) ) )
{
}

inline yvalue::yvalue( yobject* object )
    :   x( object ? TAG_OBJECT | (uintptr_t)object : VALUE_NULL )
{
    assert( ! object || ( ! object->is< ystring >() && ! object->is< yexpand >() ) );
}

inline yvalue::yvalue( ystring* string )
    :   x( string ? TAG_STRING | (uintptr_t)string : VALUE_NULL )
{
}

inline yvalue::yvalue( yexpand* expand )
    :   x( expand ? TAG_EXPAND | (uintptr_t)expand : VALUE_NULL )
{
}


inline yvalue::operator bool() const
{
#if Y64BIT
    return ( x < VALUE_FALSE ) && ( x & SIGN_MASK ) != POSITIVE_ZERO;
#else
    return ( hi < HI( VALUE_FALSE ) ) && ( n != 0.0 );
#endif
}

inline const char* yvalue::c_str() const
{
    return as_string()->c_str();
}


inline bool yvalue::is_null() const
{
#if Y64BIT
    return x == VALUE_NULL;
#else
    return hi == HI( VALUE_NULL );
#endif
}

inline bool yvalue::is_undefined() const
{
#if Y64BIT
    return x == VALUE_UNDEFINED;
#else
    return hi == HI( VALUE_UNDEFINED );
#endif
}

inline bool yvalue::is_bool() const
{
#if Y64BIT
    return x == VALUE_FALSE || x == VALUE_TRUE;
#else
    return hi == HI( VALUE_FALSE ) || x == HI( VALUE_TRUE );
#endif
}

inline bool yvalue::is_number() const
{
#if Y64BIT
    return x <= MAX_FLOAT;
#else
    return hi <= HI( MAX_FLOAT );
#endif
}

inline bool yvalue::is_object() const
{
#if Y64BIT
    return x >= MIN_REFERENCE && x <= MAX_REFERENCE;
#else
    return hi >= HI( MIN_REFERENCE ) && hi <= HI( MAX_REFERENCE );
#endif
}

inline bool yvalue::is_string() const
{
#if Y64BIT
    return ( x & TAG_MASK ) == TAG_STRING;
#else
    return hi == HI( TAG_STRING );
#endif
}

inline bool yvalue::is_expand() const
{
#if Y64BIT
    return ( x & TAG_MASK ) == TAG_EXPAND;
#else
    return hi == HI( TAG_EXPAND );
#endif
}

inline bool yvalue::as_bool() const
{
#if Y64BIT
    if ( x == VALUE_FALSE )
        return false;
    else if ( x == VALUE_TRUE )
        return true;
    else
        throw yerror( "expected bool" );
#else
    if ( hi == HI( VALUE_FALSE ) )
        return false;
    else if ( hi == HI( VALUE_TRUE ) )
        return true;
    else
        throw yerror( "expected bool" );
#endif
}

inline double yvalue::as_number() const
{
    if ( is_number() )
        return n;
    else
        throw yerror( "expected number" );
}

inline yobject* yvalue::as_object() const
{
#if Y64BIT
    if ( is_object() )
        return (yobject*)( x & POINTER_MASK );
    else
        throw yerror( "expected object" );
#else
    if ( is_object() )
        return (yobject*)lo;
    else
        throw yerror( "expected object" );
#endif
}

inline ystring* yvalue::as_string() const
{
#if Y64BIT
    if ( is_string() )
        return (ystring*)( x & POINTER_MASK );
    else
        throw yerror( "expected string" );
#else
    if ( is_string() )
        return (ystring*)lo;
    else
        throw yerror( "expected string" );
#endif
}

inline yexpand* yvalue::as_expand() const
{
#if Y64BIT
    if ( is_expand() )
        return (yexpand*)( x & POINTER_MASK );
    else
        throw yerror( "expected object" );
#else
    if ( is_expand() )
        return (yexpand*)lo;
    else
        throw yerror( "expected object" );
#endif
}


template < typename object_t >
inline bool yvalue::is() const
{
    return is_object() && as_object()->is< object_t >();
}

template < typename object_t >
inline object_t* yvalue::as() const
{
    if ( is< object_t >() )
    {
        object_t* o = as_object()->as< object_t >();
        if ( o )
        {
            return o;
        }
    }
    throw yerror( "expected %s", object_t::metatype.name );
}


#if !Y64BIT
#undef HI
#endif




inline bool operator == ( yvalue a, yvalue b )
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
        ystring* astr = a.as_string();
        ystring* bstr = b.as_string();
        if ( astr == bstr )
            return true;
        if ( astr->size() != bstr->size() )
            return false;
        if ( astr->hash() != bstr->hash() )
            return false;
        return ystring::strcmp( astr, bstr ) == 0;
    }
}

inline bool operator != ( yvalue a, yvalue b )
{
    return !( a == b );
}

inline bool operator <  ( yvalue a, yvalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ystring::strcmp( a.as_string(), b.as_string() ) < 0;
}

inline bool operator <= ( yvalue a, yvalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ystring::strcmp( a.as_string(), b.as_string() ) <= 0;
}

inline bool operator >  ( yvalue a, yvalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ystring::strcmp( a.as_string(), b.as_string() ) > 0;
}

inline bool operator >= ( yvalue a, yvalue b )
{
    if ( ! a.is_string() )
        return a.as_number() < b.as_number();
    else
        return ystring::strcmp( a.as_string(), b.as_string() ) >= 0;
}


inline size_t std::hash< yvalue >::operator () ( const yvalue& v ) const
{
    if ( v.is_string() )
        return v.as_string()->hash();
    else
        return std::hash< uint64_t >()( v.x );
}






#if Y64BIT


/*
    ywb< yvalue >
*/


inline ywb< yvalue >::ywb()
    :   ywb( yvalue::undefined )
{
}

inline ywb< yvalue >::ywb( const ywb& q )
    :   ywb( q.get() )
{
}

inline ywb< yvalue >::ywb( yvalue q )
    :   x( q.x )
{
}

inline ywb< yvalue >& ywb< yvalue >::operator = ( const ywb& q )
{
    return this->operator = ( q.get() );
}

inline ywb< yvalue >& ywb< yvalue >::operator = ( yvalue q )
{
    // Perform write barrier on old value.
    uint64_t x = this->x.load( std::memory_order_relaxed );
    if ( x >= yvalue::MIN_REFERENCE && x <= yvalue::MAX_REFERENCE )
    {
        yobject* p = (yobject*)( x & yvalue::POINTER_MASK );
        p->mark();
    }
    
    // Store new value.
    this->x.store( q.x, std::memory_order_relaxed );
    return *this;
}

inline ywb< yvalue >::operator yvalue () const
{
    return yvalue( x.load( std::memory_order_relaxed ) );
}

inline yvalue ywb< yvalue >::get() const
{
    return yvalue( x.load( std::memory_order_relaxed ) );
}



inline void ymarktraits< ywb< yvalue > >::mark(
                ywb< yvalue >& wb, yworklist* work, ycolour colour )
{
    // On mark thread must use consume memory ordering.
    uint64_t x = wb.x.load( std::memory_order_consume );
    if ( x >= yvalue::MIN_REFERENCE && x <= yvalue::MAX_REFERENCE )
    {
        yobject* p = (yobject*)( x & yvalue::POINTER_MASK );
        p->mark( work, colour );
    }
}



#else



inline ywb< yvalue >::ywb()
    :   ywb( yvalue::undefined )
{
}

inline ywb< yvalue >::ywb( const ywb& q )
    :   ywb( q.get() )
{
}

inline ywb< yvalue >::ywb( yvalue q )
    :   p( q.is_object() ? (yobject*)q.lo : nullptr )
    ,   lo( q.lo )
    ,   hi( q.hi )
{
}


inline ywb< yvalue >& ywb< yvalue >::operator = ( const ywb& q )
{
    return this->operator = ( (yvalue)q );
}

inline ywb< yvalue >& ywb< yvalue >::operator = ( yvalue q )
{
    // Perform write barrier.
    yobject* p = this->p.load( std::memory_order_relaxed );
    if ( p )
    {
        p->mark();
    }
    
    // Store new value.
    p = q.is_object() ? (yobject*)q.lo : nullptr;
    this->p.store( p, std::memory_order_relaxed );
    lo = q.lo;
    hi = q.hi;
    return *this;
}

inline ywb< yvalue >::operator yvalue () const
{
    return yvalue( lo, hi );
}

inline yvalue ywb< yvalue >::load() const
{
    return yvalue( lo, hi );
}



inline void ymarktraits< ywb< yvalue > >::mark(
                ywb< yvalue >& wb, yworklist* work, ycolour colour )
{
    // On mark thread must use consume memory ordering.
    yobject* p = wb.p.load( std::memory_order_consume );
    if ( p )
    {
        p->mark( work, colour );
    }
}




#endif




#endif
