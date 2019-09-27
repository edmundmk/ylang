//
//  yvm_value.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YVM_VALUE_H
#define YVM_VALUE_H

namespace yvm
{

struct object;
struct string;

/*
    A value is 'nun-boxed'.

        0000'0000'0000'0000 null
        0000'----'----'---- yvm_object*
        0001'0000'0000'0000 undefined
        0001'----'----'---- yvm_string*
        000E'FFFF'FFFF'FFFE true
        000E'FFFF'FFFF'FFFF false

        000F'FFFF'FFFF'FFFF -∞
        7FFF'FFFF'FFFF'FFFF -0
        8007'FFFF'FFFF'FFFE qNAN
        800F'FFFF'FFFF'FFFE sNaN
        800F'FFFF'FFFF'FFFF +∞
        FFFF'FFFF'FFFF'FFFF +0
*/

const uint64_t VALUE_NULL           = 0x0000'0000'0000'0000;
const uint64_t VALUE_UNDEFINED      = 0x0001'0000'0000'0000;
const uint64_t VALUE_TRUE           = 0x000E'FFFF'FFFF'FFFE;
const uint64_t VALUE_FALSE          = 0x000E'FFFF'FFFF'FFFF;
const uint64_t VALUE_NEGATIVE_ZERO  = 0x7FFF'FFFF'FFFF'FFFF;
const uint64_t VALUE_POSITIVE_ZERO  = 0xFFFF'FFFF'FFFF'FFFF;

enum { undefined };

struct value
{
    value( decltype( nullptr ) );
    value( decltype( undefined ) );
    value( bool b );
    value( double n );
    value( object* o );
    value( string* s );

    value( int i );
    value( unsigned u );
    explicit value( long i );
    explicit value( unsigned long u );
    explicit value( long long i );
    explicit value( unsigned long long u );

    bool is_null() const;
    bool is_undefined() const;
    bool is_bool() const;
    bool is_number() const;
    bool is_object() const;
    bool is_string() const;

    bool as_bool() const;
    double as_number() const;
    object* as_object() const;
    string* as_string() const;

    uint64_t v;
};

inline value::value( decltype( nullptr ) )      : v( VALUE_NULL ) {}
inline value::value( decltype( undefined ) )    : v( VALUE_UNDEFINED ) {}
inline value::value( bool b )                   : v( b ? VALUE_TRUE : VALUE_FALSE ) {}
inline value::value( double n )                 { uint64_t u; memcpy( &u, &n, sizeof( u ) ); v = ~u; }
inline value::value( object* o )                : v( (uint64_t)o ) {}
inline value::value( string* s )                : v( 0x0001'0000'0000'0000 | (uint64_t)s ) {}
inline value::value( int i )                    : value( (double)i ) {}
inline value::value( unsigned int u )           : value( (double)u ) {}
inline value::value( long i )                   : value( (double)i ) {}
inline value::value( unsigned long u )          : value( (double)u ) {}
inline value::value( long long i )              : value( (double)i ) {}
inline value::value( unsigned long long u )     : value( (double)u ) {}

inline bool value::is_null() const              { return v == VALUE_NULL; }
inline bool value::is_undefined() const         { return v == VALUE_UNDEFINED; }
inline bool value::is_bool() const              { return ( v & 0xFFFF'0000'0000'0000 ) == 0x000E'0000'0000'0000; }
inline bool value::is_number() const            { return v > 0x000E'FFFF'FFFF'FFFF; }
inline bool value::is_object() const            { return v < 0x0001'0000'0000'0000; }
inline bool value::is_string() const            { return ( v & 0xFFFF'0000'0000'0000 ) == 0x0001'0000'0000'0000; }

inline bool value::as_bool() const              { return v != VALUE_FALSE; }
inline double value::as_number() const          { uint64_t u = ~v; double n; memcpy( &n, &u, sizeof( d ) ); return n; }
inline object* value::as_object() const         { return (object*)v; }
inline string* value::as_string() const         { return (string*)( v & 0x0000'FFFF'FFFF'FFFF ); }

#endif

