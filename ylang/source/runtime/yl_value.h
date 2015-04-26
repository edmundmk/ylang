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
#include "yl_heapobj.h"


class yl_tagval;
class yl_value;
class yl_valref;



/*
    Small integer pointers representing singleton objects.
*/

static yl_heapobj* const yl_null    = nullptr;
static yl_heapobj* const yl_undef   = (yl_heapobj*)1;
static yl_heapobj* const yl_false   = (yl_heapobj*)2;
static yl_heapobj* const yl_true    = (yl_heapobj*)3;

bool yl_is_singular( yl_heapobj* heapobj );




/*
    A yl_tagval is two pointers in size, but stores the object kind
    explicitly, which means we don't have to disambiguate different kinds
    of objects before using them.  Could be termed a 'fat value'.
*/


class yl_tagval
{
public:

    yl_tagval();
    yl_tagval( const yl_valref& value );
    yl_tagval( double number );
    yl_tagval( yl_objkind kind, yl_heapobj* heapobj );

    yl_objkind      kind() const;
    double          number() const;
    yl_heapobj*     heapobj() const;


private:

    yl_objkind      _kind;
    union
    {
        double      _number;
        yl_heapobj* _heapobj;
    };
    
};



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
    
    pointer   0000000000000000pppppppppppppppp pppppppppppppppppppppppppppppppp
 
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

    void            set( const yl_tagval& value );
    void            set( double number );
    void            set( yl_heapobj* heapobj );

    yl_valref       get() const;


private:

    yl_value( const yl_value& ) = delete;
    yl_value& operator = ( const yl_value& ) = delete;


    union
    {
        std::atomic< uintptr_t >    _value;
        yl_heapobj*                 _p;
    };

};



/*
    A valref is the result of reading a yl_value.  It allows values to
    be manipulated without multiple inadvertent atomic reads (which the
    compiler can't coalesce).
*/


class yl_valref
{
public:

    bool            is_number() const;
    bool            is_heapobj() const;
    
    double          as_number() const;
    yl_heapobj*     as_heapobj() const;


private:

    friend class yl_value;

    explicit yl_valref( uintptr_t value );

    union
    {
        uintptr_t                   _value;
        yl_heapobj*                 _p;
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
    
    size_t          size() const;
    const yl_value& at( size_t index ) const;
    yl_value&       at( size_t index );


private:

    explicit yl_valarray( size_t size );

    size_t          _size;
    yl_value        _elements[ 0 ];

};




/*

*/



inline yl_tagval::yl_tagval()
    :   _kind( YLOBJ_NULL )
    ,   _heapobj( yl_undef )
{
}


inline yl_tagval::yl_tagval( const yl_valref& value )
{
    if ( value.is_number() )
    {
        _kind    = YLOBJ_NUMBER;
        _number  = value.as_number();
    }
    else
    {
        _heapobj = value.as_heapobj();
        if ( _heapobj > yl_true )
            _kind = _heapobj->kind();
        else
            _kind = (yl_objkind)std::min( (uintptr_t)_heapobj, (uintptr_t)2 );
    }
}


inline yl_tagval::yl_tagval( double number )
    :   _kind( YLOBJ_NUMBER )
    ,   _number( number )
{
}

inline yl_tagval::yl_tagval( yl_objkind kind, yl_heapobj* heapobj )
    :   _kind( kind )
    ,   _heapobj( heapobj )
{
}

inline yl_objkind yl_tagval::kind() const
{
    return _kind;
}

inline double yl_tagval::number() const
{
    return _number;
}

inline yl_heapobj* yl_tagval::heapobj() const
{
    return _heapobj;
}




inline yl_value::yl_value()
    :   _value( 0 )
{
}

inline yl_value::~yl_value()
{
}

inline void yl_value::set( const yl_tagval& value )
{
    if ( value.kind() == YLOBJ_NUMBER )
    {
        set( value.number() );
    }
    else
    {
        set( value.heapobj() );
    }
}

inline void yl_value::set( double n )
{
    // TODO: perform write barrier.

    if ( sizeof( uintptr_t ) == 8 )
    {
        union { double n; uintptr_t v; } bits;
        bits.n = n;
        this->_value.store( ~bits.v, std::memory_order_relaxed );
    }
    else
    {
        assert( ! "uinmplemented for this architecture" );
    }
}

inline void yl_value::set( yl_heapobj* o )
{
    // TODO: perform write barrier.

    this->_value.store( (uintptr_t)o, std::memory_order_relaxed );
}

inline yl_valref yl_value::get() const
{
    return yl_valref( this->_value.load( std::memory_order_relaxed ) );
}




inline yl_valref::yl_valref( uintptr_t value )
    :   _value( value )
{
}

inline bool yl_valref::is_number() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return ( _value & UINT64_C( 0xFFFF000000000000 ) ) != 0;
    }
    else
    {
        assert( ! "unimplemented for this architecture" );
    }
}

inline bool yl_valref::is_heapobj() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return ( _value & UINT64_C( 0xFFFF000000000000 ) ) == 0;
    }
    else
    {
        assert( ! "unimplemented for this architecture" );
    }
}

inline double yl_valref::as_number() const
{
    assert( is_number() );
    if ( sizeof( uintptr_t ) == 8 )
    {
        union { uintptr_t v; double n; } bits;
        bits.v = ~_value;
        return bits.n;
    }
    else
    {
        assert( ! "unimplemented for this architecture" );
    }
}

inline yl_heapobj* yl_valref::as_heapobj() const
{
    assert( is_heapobj() );
    return (yl_heapobj*)_value;
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


