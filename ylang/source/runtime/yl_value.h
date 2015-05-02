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
#include "yl_string.h"


class yl_valref;
class yl_value;
class yl_valarray;



/*
    Small integer pointers representing singleton objects.
*/

static yl_heapobj* const yl_null    = nullptr;
static yl_heapobj* const yl_undef   = (yl_heapobj*)1;
static yl_heapobj* const yl_false   = (yl_heapobj*)2;
static yl_heapobj* const yl_true    = (yl_heapobj*)3;



/*
    Check if a number is an integer.
*/

bool is_integer( double number );





/*
    A yl_valref is pointer-sized and can hold any ylang type.  That means a
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
 
 
    A yl_valref::value is the result of reading a yl_valref.  It allows values
    to be manipulated without multiple inadvertent atomic reads (which the
    compiler can't coalesce).

*/


class yl_valref
{
public:

    class value
    {
    public:

        bool            is_number() const;
        bool            is_heapobj() const;
        
        double          as_number() const;
        yl_heapobj*     as_heapobj() const;


    private:

        friend class yl_valref;

        explicit value( uintptr_t value );

        union
        {
            uintptr_t                   _value;
            yl_heapobj*                 _p;
        };

    };


    yl_valref();
    ~yl_valref();

    void    set( const yl_value& value );
    void    set( double number );
    void    set( yl_heapobj* heapobj );

    value   get() const;


private:

    yl_valref( const yl_valref& ) = delete;
    yl_valref& operator = ( const yl_valref& ) = delete;


    union
    {
        std::atomic< uintptr_t >    _value;
        yl_heapobj*                 _p;
    };

};





/*
    A yl_value is currently two pointers in size, but stores the object kind
    explicitly, which means we don't have to disambiguate different kinds
    of objects before using them.  Could be termed a 'fat value'.
*/


class yl_value
{
public:

    yl_value();
    yl_value( const yl_valref::value& value );
    yl_value( double number );
    yl_value( yl_objkind kind, yl_heapobj* heapobj );

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
    Hash values and check for equality.
*/


hash32_t hash( const yl_value& value );
bool equal( const yl_value& a, const yl_value& b );




/*
    Garbage-collected arrays of values.
*/

class yl_valarray : public yl_heapobj
{
public:

    static yl_valarray* alloc( size_t size );
    ~yl_valarray();
    
    size_t              size() const;
    const yl_valref&    at( size_t index ) const;
    yl_valref&          at( size_t index );


private:

    explicit yl_valarray( size_t size );

    size_t              _size;
    yl_valref           _elements[ 0 ];

};




/*

*/


inline bool is_integer( double number )
{
    return number == (long)number;
}




inline yl_valref::yl_valref()
    :   _value( 0 )
{
}

inline yl_valref::~yl_valref()
{
}

inline void yl_valref::set( const yl_value& value )
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

inline void yl_valref::set( double n )
{
    value value = get();
    if ( value.is_heapobj() )
    {
        yl_current->write_barrier( value.as_heapobj() );
    }

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

inline void yl_valref::set( yl_heapobj* o )
{
    value value = get();
    if ( value.is_heapobj() )
    {
        yl_current->write_barrier( value.as_heapobj() );
    }

    this->_value.store( (uintptr_t)o, std::memory_order_relaxed );
}

inline yl_valref::value yl_valref::get() const
{
    return value( this->_value.load( std::memory_order_relaxed ) );
}




inline yl_valref::value::value( uintptr_t value )
    :   _value( value )
{
}

inline bool yl_valref::value::is_number() const
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

inline bool yl_valref::value::is_heapobj() const
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

inline double yl_valref::value::as_number() const
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

inline yl_heapobj* yl_valref::value::as_heapobj() const
{
    assert( is_heapobj() );
    return (yl_heapobj*)_value;
}





inline yl_value::yl_value()
    :   _kind( YLOBJ_NULL )
    ,   _heapobj( yl_undef )
{
}


inline yl_value::yl_value( const yl_valref::value& value )
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


inline yl_value::yl_value( double number )
    :   _kind( YLOBJ_NUMBER )
    ,   _number( number )
{
}

inline yl_value::yl_value( yl_objkind kind, yl_heapobj* heapobj )
    :   _kind( kind )
    ,   _heapobj( heapobj )
{
}

inline yl_objkind yl_value::kind() const
{
    return _kind;
}

inline double yl_value::number() const
{
    return _number;
}

inline yl_heapobj* yl_value::heapobj() const
{
    return _heapobj;
}




inline hash32_t hash( const yl_value& value )
{
    if ( value.kind() == YLOBJ_STRING )
    {
        yl_string* string = (yl_string*)value.heapobj();
        return string->hash();
    }
    else if ( value.kind() == YLOBJ_NUMBER )
    {
        double number = value.number();
        return hash32( &number, sizeof( double ) );
    }
    else
    {
        yl_heapobj* heapobj = value.heapobj();
        return hash32( &heapobj, sizeof( yl_heapobj* ) );
    }
}

inline bool equal( const yl_value& a, const yl_value& b )
{
    if ( a.kind() != b.kind() )
    {
        return false;
    }
    
    if ( a.kind() == YLOBJ_NUMBER )
    {
        return a.number() == b.number();
    }

    if ( a.heapobj() == b.heapobj() )
    {
        return true;
    }
    
    if ( a.kind() == YLOBJ_STRING )
    {
        yl_string* sa = (yl_string*)a.heapobj();
        yl_string* sb = (yl_string*)b.heapobj();

        if ( sa->size() != sb->size() )
        {
            return false;
        }
        
        return memcmp( sa->c_str(), sb->c_str(), sa->size() ) == 0;
    }
    
    return false;
}








inline size_t yl_valarray::size() const
{
    return _size;
}

inline const yl_valref& yl_valarray::at( size_t index ) const
{
    assert( index < _size );
    return _elements[ index ];
}

inline yl_valref& yl_valarray::at( size_t index )
{
    assert( index < _size );
    return _elements[ index ];
}





#endif


