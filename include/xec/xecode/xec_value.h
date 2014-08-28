//
//  xec_value.h
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_VALUE_H
#define XEC_VALUE_H


#include <assert.h>
#include <math.h>


/*
    On a system with 48-bit pointers (or less), we NaN-box values into doubles
    like this:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    qnan      s111111111111000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

    null      0111111111110001000000000000000000000000000000000000000000000000
    bool      011111111111001000000000000000000000000000000000000000000000000b
    native    0111111111110011pppppppppppppppppppppppppppppppppppppppppppppppp
    object    0111111111110101pppppppppppppppppppppppppppppppppppppppppppppppp
    string    0111111111110110pppppppppppppppppppppppppppppppppppppppppppppppp
    oolptr    0111111111110111pppppppppppppppppppppppppppppppppppppppppppppppp

    On systems with a truly 64-bit address space, we should ensure that values
    are allocated from the low 48-bits (or from a heap not larger than 48-bits)
    and introduce a special oolnative type to hold full 64-bit pointers with
    an extra level of indirection.  For now just be lazy - 256TiB should be
    enough for everyone!
 
*/


class xec_object;
class xec_string;
class xec_oolptr;



class xec_value
{
public:

    xec_value();
    xec_value( double number );
    xec_value( bool boolean );
    xec_value( void* native );
    xec_value( xec_object* object );
    xec_value( xec_string* string );

    explicit operator bool() const;

    bool            isnull() const;
    bool            isnumber() const;
    bool            isboolean() const;
    bool            isnative() const;
    bool            isobject() const;
    bool            isstring() const;

    double          number() const;
    bool            boolean() const;
    void*           native() const;
    xec_object*     object() const;
    xec_string*     string() const;

private:

    friend bool operator == ( const xec_value& a, const xec_value& b );
    friend bool operator != ( const xec_value& a, const xec_value& b );


    static const uint64_t FLOAT_XOR     = (uint64_t)0x0008 << 48;
    static const int64_t  FLOAT_MAX     = (int64_t)0x7FF8 << 48;

    static const uint64_t MARK_MASK     = (uint64_t)0xFFFF << 48;
    static const uint64_t MARK_NULL     = (uint64_t)0x7FF1 << 48;
    static const uint64_t MARK_BOOL     = (uint64_t)0x7FF2 << 48;
    static const uint64_t MARK_NATIVE   = (uint64_t)0x7FF3 << 48;
    static const uint64_t MARK_OBJECT   = (uint64_t)0x7FF5 << 48;
    static const uint64_t MARK_STRING   = (uint64_t)0x7FF6 << 48;
    static const uint64_t MARK_OOLPTR   = (uint64_t)0x7FF7 << 48;
    
    static const uint64_t BITS_FALSE    = MARK_BOOL | (uint64_t)false;
    static const uint64_t BITS_ZERO     = 0x0000000000000000ll;
    static const uint64_t BITS_NZERO    = 0x8000000000000000ll;
    
    union
    {
        double      f;
        uintptr_t   i;
    };

};


/*
    Note that this is _raw_ equality, where NaNs compare equal.
*/

bool operator == ( const xec_value& a, const xec_value& b );
bool operator != ( const xec_value& a, const xec_value& b );





inline xec_value::xec_value()
    :   i( MARK_NULL )
{
}

inline xec_value::xec_value( double number )
    :   f( number )
{
}

inline xec_value::xec_value( bool boolean )
    :   i( MARK_BOOL | (uint64_t)boolean )
{
}

inline xec_value::xec_value( void* native )
    :   i( MARK_NATIVE | (uint64_t)native )
{
    assert( (uint64_t)native < ( (uint64_t)1 << 48 ) );
}

inline xec_value::xec_value( xec_object* object )
    :   i( MARK_OBJECT | (uint64_t)object )
{
    assert( (uint64_t)object < ( (uint64_t)1 << 48 ) );
}

inline xec_value::xec_value( xec_string* string )
    :   i( MARK_STRING | (uint64_t)string )
{
    assert( (uint64_t)string < ( (uint64_t)1 << 48 ) );
}


inline xec_value::operator bool() const
{
    return i != MARK_NULL
        && i != BITS_FALSE
        && i != BITS_ZERO
        && i != BITS_NZERO;
}



inline bool xec_value::isnull() const
{
    return i == MARK_NULL;
}

inline bool xec_value::isnumber() const
{
    // Attempting to be clever here.
    return (int64_t)( i ^ FLOAT_XOR ) <= FLOAT_MAX;
}

inline bool xec_value::isboolean() const
{
    return ( i & MARK_MASK ) == MARK_BOOL;
}

inline bool xec_value::isnative() const
{
    return ( i & MARK_MASK ) == MARK_NATIVE;
}

inline bool xec_value::isobject() const
{
    return ( i & MARK_MASK ) == MARK_OBJECT;
}

inline bool xec_value::isstring() const
{
    return ( i & MARK_MASK ) == MARK_STRING;
}


inline double xec_value::number() const
{
    return f;
}

inline bool xec_value::boolean() const
{
    return (bool)( i & ~MARK_MASK );
}

inline void* xec_value::native() const
{
    return (void*)( i & ~MARK_MASK );
}

inline xec_object* xec_value::object() const
{
    return (xec_object*)( i & ~MARK_MASK );
}

inline xec_string* xec_value::string() const
{
    return (xec_string*)( i & ~MARK_MASK );
}



inline bool operator == ( const xec_value& a, const xec_value& b )
{
    return a.i == b.i;
}


inline bool operator != ( const xec_value& a, const xec_value& b )
{
    return a.i != b.i;
}



#endif
