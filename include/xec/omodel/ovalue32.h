//
//  ovalue32.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OVALUE32_H
#define OVALUE32_H


#include <cstddef>
#include <stdint.h>
#include <atomic>
#include <functional>
#include "oheap.h"
#include "obase.h"


class ostring;
class oexpand;


#define OVALUE32 1


/*
    On 32-bit systems ovalues are restricted to 32-bits, which means that
    numbers which are not 30-bit signed integers end up as boxed numbers.
    
                undefined   11111111111111111111111111111111
                -0          11111111111111111111111111111011
                false       11111111111111111111111111110111
                true        11111111111111111111111111110011
                integer     nnnnnnnnnnnnnnnnnnnnnnnnnnnnnn10
                boxed       pppppppppppppppppppppppppppppp01
                object      pppppppppppppppppppppppppppppp00

*/


class oboxed : public obase
{
public:

    static oboxed* alloc( double n );
    
    double get() const;


protected:

    friend class obase;
    static ometatype metatype;

    oboxed( ometatype* metatype, double n );


private:

    double n;

};



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

    static const uint32_t VALUE_UNDEFINED   = UINT32_C( 0xFFFFFFFF );
    static const uint32_t VALUE_NEG_ZERO    = UINT32_C( 0xFFFFFFFB );
    static const uint32_t VALUE_FALSE       = UINT32_C( 0xFFFFFFF7 );
    static const uint32_t VALUE_TRUE        = UINT32_C( 0xFFFFFFF3 );
    static const uint32_t VALUE_POS_ZERO    = UINT32_C( 0x00000002 );
    static const uint32_t VALUE_NULL        = UINT32_C( 0x00000000 );
    
    static const uint32_t TAG_INTEGER       = UINT32_C( 0x00000002 );
    static const uint32_t TAG_BOXED         = UINT32_C( 0x00000001 );
    static const uint32_t TAG_OBJECT        = UINT32_C( 0x00000000 );
    
    static const uint32_t TAG_MASK          = UINT32_C( 0x00000003 );
    static const uint32_t POINTER_MASK      = UINT32_C( 0xFFFFFFFC );
    
    static const int32_t  MAX_INTEGER       = INT32_C( 0x1FFFFFFF );
    static const int32_t  MIN_INTEGER       = -MAX_INTEGER - 1;
    
    explicit ovalue( uint32_t x );

    uint32_t x;

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
    
    std::atomic< uint32_t > x;
 
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
    oboxed
*/

inline oboxed* oboxed::alloc( double n )
{
    void* p = malloc( sizeof( oboxed ) );
    return new ( p ) oboxed( &metatype, n );
}

inline oboxed::oboxed( ometatype* metatype, double n )
    :   obase( metatype )
    ,   n( n )
{
}

inline double oboxed::get() const
{
    return n;
}



/*
    ovalue
*/


inline ovalue::ovalue( uint32_t x )
    :   x( x )
{
}

inline ovalue::ovalue()
    :   x( VALUE_UNDEFINED )
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
{
    if ( value == 0 )
    {
        x = signbit( value ) ? VALUE_NEG_ZERO : VALUE_POS_ZERO;
    }
    else
    {
        int32_t i = (int32_t)value;
        if ( i == value && i >= MIN_INTEGER && i <= MAX_INTEGER )
        {
            x = i << 2 | TAG_INTEGER;
        }
        else
        {
            x = (uint32_t)oboxed::alloc( value ) | TAG_BOXED;
        }
    }
}

inline ovalue::ovalue( const char* string )
    :   x( (uint32_t)ostring::alloc( string ) | TAG_OBJECT )
{
}

inline ovalue::ovalue( obase* object )
    :   x( (uint32_t)object | TAG_OBJECT )
{
}

inline ovalue::ovalue( ostring* string )
    :   x( (uint32_t)string | TAG_OBJECT )
{
}

inline ovalue::ovalue( oexpand* expand )
    :   x( (uint32_t)expand | TAG_OBJECT )
{
}

inline ovalue::operator bool() const
{
    return x != VALUE_NULL
        && x != VALUE_FALSE
        && x != VALUE_UNDEFINED
        && x != VALUE_NEG_ZERO
        && x != VALUE_POS_ZERO;
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
    return x == VALUE_TRUE || x == VALUE_FALSE;
}

inline bool ovalue::is_number() const
{
    return ( x & TAG_MASK ) == TAG_INTEGER
        || ( x & TAG_MASK ) == TAG_BOXED
        || x == VALUE_NEG_ZERO;
}

inline bool ovalue::is_object() const
{
    return ( x & TAG_MASK ) == TAG_OBJECT;
}

inline bool ovalue::is_string() const
{
    return ( x & TAG_MASK ) == TAG_OBJECT
        && ( (obase*)( x & POINTER_MASK ) )->is< ostring >();
}

inline bool ovalue::is_expand() const
{
    return ( x & TAG_MASK ) == TAG_OBJECT
        && ( (obase*)( x & POINTER_MASK ) )->is< oexpand >();
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
    if ( ( x & TAG_MASK ) == TAG_INTEGER )
    {
        return (int32_t)x >> 2;
    }
    else if ( ( x & TAG_MASK ) == TAG_BOXED )
    {
        return ( (oboxed*)( x & POINTER_MASK ) )->get();
    }
    else if ( x == VALUE_NEG_ZERO )
    {
        return -0.0;
    }
    else
    {
        throw oerror( "expected number" );
    }
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
    if ( ( a.x & ovalue::TAG_MASK ) == ovalue::TAG_BOXED )
    {
        if ( ( b.x & ovalue::TAG_MASK ) != ovalue::TAG_BOXED )
            return false;
        oboxed* abox = (oboxed*)( a.x & ovalue::POINTER_MASK );
        oboxed* bbox = (oboxed*)( b.x & ovalue::POINTER_MASK );
        return abox->get() == bbox->get();
    }
    else if ( ! a.is_string() )
    {
        return a.x == b.x
            || ( a.x == ovalue::VALUE_POS_ZERO && b.x == ovalue::VALUE_NEG_ZERO )
            || ( a.x == ovalue::VALUE_NEG_ZERO && b.x == ovalue::VALUE_POS_ZERO );
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




inline owb< ovalue >::owb()
    :   x( ovalue::VALUE_UNDEFINED )
{
}

inline owb< ovalue >::owb( ovalue q )
    :   x( q.x )
{
}

owb< ovalue >& owb< ovalue >::operator = ( ovalue q )
{
    // Perform write barrier on old value.
    uint32_t x = this->x.load( std::memory_order_relaxed );
    if ( x && ( x & ovalue::TAG_MASK ) <= ovalue::TAG_BOXED )
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

owb< ovalue >::operator ovalue () const
{
    return ovalue( x.load( std::memory_order_relaxed ) );
}


inline void omark< ovalue >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // On mark thread must use consume memory ordering.
    uint32_t x = value.x.load( std::memory_order_consume );
    if ( x && ( x & ovalue::TAG_MASK ) <= ovalue::TAG_BOXED )
    {
        obase* p = (obase*)( x & ovalue::POINTER_MASK );
        p->mark( work, colour );
    }
}




#endif
