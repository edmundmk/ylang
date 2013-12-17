//
//  intmath.h
//
//  Created by Edmund Kapusniak on 22/08/2013.
//  Copyright (c) 2013 Edmund Kapusniak. All rights reserved.
//


#ifndef INTMATH_H
#define INTMATH_H


#include <assert.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif



unsigned int        clz( unsigned int x );
unsigned long       clz( unsigned long x );
unsigned long long  clz( unsigned long long x );

unsigned int        log2i( unsigned int x );
unsigned long       log2i( unsigned long x );
unsigned long long  log2i( unsigned long long x );

bool                is_pow2( unsigned int x );
bool                is_pow2( unsigned long x );
bool                is_pow2( unsigned long long x );

unsigned int        ceil_pow2( unsigned int x );
unsigned long       ceil_pow2( unsigned long x );
unsigned long long  ceil_pow2( unsigned long long x );

unsigned int        align( unsigned int x, unsigned int alignment );
unsigned long       align( unsigned long x, unsigned long alignment );
unsigned long long  align( unsigned long long x, unsigned long long alignment );



inline unsigned int clz( unsigned int x )
{
#ifdef _MSC_VER

    unsigned long result;
    if ( _BitScanReverse( &result, x ) )
        return 31 - result;

#else

    if ( x != 0 )
        return __builtin_clz( x );

#endif

    return sizeof( unsigned int ) * 8;
}

inline unsigned long clz( unsigned long x )
{
#ifdef _MSC_VER

    unsigned long result;
    if ( _BitScanReverse( &result, x ) )
        return 31 - result;

#else

    if ( x != 0 )
        return __builtin_clzl( x );

#endif

    return sizeof( unsigned long ) * 8;
}


#ifndef _MSC_VER

inline unsigned long long clz( unsigned long long x )
{
#ifdef _MSC_VER

    unsigned __int64 result;
    if ( _BitScanReverse64( &result, x ) )
        return 63 - result;

#else

    if ( x != 0 )
        return __builtin_clzll( x );

#endif

    return sizeof( unsigned long long ) * 8;
}

#endif


inline unsigned int log2i( unsigned int x )
{
    if ( x != 0 )
        return sizeof( unsigned int ) * 8 - 1 - clz( x );
    else
        return 0;
}

inline unsigned long log2i( unsigned long x )
{
    if ( x != 0 )
        return sizeof( unsigned long ) * 8 - 1 - clz( x );
    else
        return 0;
}

inline unsigned long long log2i( unsigned long long x )
{
    if ( x != 0 )
        return sizeof( unsigned long long ) * 8 - 1 - clz( x );
    else
        return 0;
}



inline bool is_pow2( unsigned int x )
{
    return ( x & ( x - 1 ) ) == 0;
}

inline bool is_pow2( unsigned long x )
{
    return ( x & ( x - 1 ) ) == 0;
}

inline bool is_pow2( unsigned long long x )
{
    return ( x & ( x - 1 ) ) == 0;
}



inline unsigned int ceil_pow2( unsigned int x )
{
    if ( ! is_pow2( x ) )
        return (unsigned int)1 << ( log2i( x ) + 1 );
    else
        return x;
}

inline unsigned long ceil_pow2( unsigned long x )
{
    if ( ! is_pow2( x ) )
        return (unsigned long)1 << ( log2i( x ) + 1 );
    else
        return x;
}

inline unsigned long long ceil_pow2( unsigned long long x )
{
    if ( ! is_pow2( x ) )
        return (unsigned long long)1 << ( log2i( x ) + 1 );
    else
        return x;
}



inline unsigned int align( unsigned int x, unsigned int alignment )
{
    assert( is_pow2( alignment ) );
    return ( x + ( alignment - 1 ) ) & ~( alignment - 1 );
}

inline unsigned long align( unsigned long x, unsigned long alignment )
{
    assert( is_pow2( alignment ) );
    return ( x + ( alignment - 1 ) ) & ~( alignment - 1 );
}

inline unsigned long long align( unsigned long long x, unsigned long long alignment )
{
    assert( is_pow2( alignment ) );
    return ( x + ( alignment - 1 ) ) & ~( alignment - 1 );
}


#endif
