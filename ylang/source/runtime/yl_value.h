//
//  yl_value.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_VALUE_H
#define YL_VALUE_H


#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "yl_heap.h"



/*
    Small values representing singleton objects.
*/

static const yl_heapobj* yl_null    = nullptr;
static const yl_heapobj* yl_undef   = (yl_heapobj*)1;
static const yl_heapobj* yl_false   = (yl_heapobj*)2;
static const yl_heapobj* yl_true    = (yl_heapobj*)3;



/*
    A yl_value is pointer-sized and can hold any ylang type.  That means a
    heapobj reference, a number, null, true, false, or undef.  Accesses MUST
    be atomic to allow the garbage collector to run concurrently.
    

    On 32-bit:
    
        There is not enough space to store a double.  Instead numbers are
        boxed.  Boxed numbers are not heapobjs and are not garbage collected.
        Instead they are owned by the yl_value.
 
        We don't bother with the 'small integer' packing that some VMs do,
        because values are stored unpacked on the stack and checking if a
        number is an integer whenever it's stored is a pain.  We can make
        allocation of boxed numbers very fast using a pool.

        A value with the low bit clear points to a heapobj pointer.  A value
        with the low bit set points to a boxed number.


    On 64-bit:
    
        We use a NaN-boxing strategy which favours pointer representation.
        We assume that all pointers to GC objects fit in 48-bits.  Doubles
        have the following bit patterns:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmm mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s1111111111100000000000000000000 00000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

        If the top bits are zero, then it's a pointer.  Otherwise the number
        can be recovered by inverting the entire value.
    
    pointer   0000000000000---pppppppppppppppp pppppppppppppppppppppppppppppppp
 
        False values:
        
    null      00000000000000000000000000000000 00000000000000000000000000000000
    undef     00000000000000000000000000000000 00000000000000000000000000000001
    false     00000000000000000000000000000000 00000000000000000000000000000010
    +zero     11111111111111111111111111111111 11111111111111111111111111111111
    -zero     01111111111111111111111111111111 11111111111111111111111111111111
 
*/


class yl_value
{
public:

    yl_value();
    ~yl_value();

    void            set( double n );
    void            set( yl_heapobj* o );

    double          as_number();
    yl_heapobj*     as_heapobj();


private:

    yl_value( const yl_value& ) = delete;
    yl_value& operator = ( const yl_value& ) = delete;


    union
    {
        std::atomic< uintptr_t >    v;
        yl_heapobj*                 p;
    };

};




/*
    Garbage-collected arrays of values.
*/

class yl_valarray : public yl_heapobj
{
public:

    static yl_valarray* alloc( size_t size );
    ~yl_valarray();
    
    size_t              size() const;
    const yl_value&     at( size_t index ) const;
    yl_value&           at( size_t index );


private:

    yl_valarray( size_t size );

    size_t      _size;
    yl_value    _elements[ 0 ];

};




/*

*/


inline yl_value::yl_value()
    :   v( 0 )
{
}

inline yl_value::~yl_value()
{
}

inline void yl_value::set( double n )
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        union { double n; uintptr_t v; } bits;
        bits.n = n;
        this->v.store( ~bits.v, std::memory_order_relaxed );
    }
    else
    {
        assert( ! "uinmplemented for this architecture" );
    }
}

inline void yl_value::set( yl_heapobj* o )
{
    this->v.store( (uintptr_t)o, std::memory_order_relaxed );
}

inline double yl_value::as_number()
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        uintptr_t v = this->v.load( std::memory_order_relaxed );
        if ( ( v & UINT64_C( 0xFFFF000000000000 ) ) != 0 )
        {
            union { uintptr_t v; double n; } bits;
            bits.v = ~v;
            return bits.n;
        }
        else
        {
            throw 0; // ??? TODO
        }
    }
    else
    {
        assert( ! "uinmplemented for this architecture" );
        return 0.0;
    }
}

inline yl_heapobj* yl_value::as_heapobj()
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        uintptr_t v = this->v.load( std::memory_order_relaxed );
        if ( ( v & UINT64_C( 0xFFFF000000000000 ) ) == 0 )
        {
            return (yl_heapobj*)v;
        }
        else
        {
            throw 0; // ??? TODO
        }
    }
    else
    {
        assert( ! "uinmplemented for this architecture" );
        return nullptr;
    }
}




inline size_t yl_valarray::size() const
{
    return _size;
}

inline const yl_value& yl_valarray::at( size_t index ) const
{
    assert( index < _size );
    return _elements[ index ];
}

inline yl_value& yl_valarray::at( size_t index )
{
    assert( index < _size );
    return _elements[ index ];
}





#endif


