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
#include "yl_context.h"
#include "yl_string.h"


class yl_valref;
class yl_value;
class yl_valarray;


/*
    NaN-boxing:
    
        We assume that all pointers to GC objects fit in 48-bits.  Doubles
        have the following bit patterns:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmm mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s1111111111100000000000000000000 00000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    qnan      s111111111111zzzzzzzzzzzzzzzzzzz zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

        Boxed values have the following general structure:
        
              111111111111tttthhhhhhhhhhhhhhhh llllllllllllllllllllllllllllllll

        Where the tag is the objkind of the value, and the rest of the mantissa
        is split between the high and low 32-bits.
        
        Tags:
 
                      11111111  object
                      11111110  expose
                      11111101  array
                      11111100  table
                      11111011  cothread
                      11111010  program
                      11111001
                      11111000  qnan
                      11110111  string
                      11110110  funcobj
                      11110101  thunkobj
                      11110100  cothread
                      11110011  program
                      11110010
                      11110001  null/undefined/false/true
                      11110000  infinity


*/



enum yl_null_t  { yl_null   };
enum yl_undef_t { yl_undef  };
enum yl_bool_t  { yl_false = 0, yl_true = 1 };


class yl_value
{
public:

    yl_value();
    yl_value( yl_undef_t );
    yl_value( yl_null_t );
    yl_value( yl_bool_t );
    yl_value( double number );
    yl_value( uint8_t kind, yl_gcobject* gcobject );

    yl_objkind      kind() const;

    bool            is_null() const;
    bool            is_undef() const;
    bool            is_false() const;
    bool            is_true() const;
    
    bool            is_number() const;
    bool            is_gcobject() const;
    
    bool            is( yl_objkind kind ) const;
    bool            is_object() const;

    double          number() const;
    yl_gcobject*     gcobject() const;


private:

    friend class yl_valref;
    
    friend hash32_t hash( yl_value v );
    friend bool     equal( yl_value a, yl_value b );
    friend bool     test( yl_value v );
    
    
    static const uint64_t POS_ZERO          = UINT64_C( 0x0000000000000000 );
    static const uint64_t NEG_ZERO          = UINT64_C( 0x8000000000000000 );

    static const uint64_t POS_NAN           = UINT64_C( 0x7FF8000000000000 );
    static const uint64_t NEG_INF           = UINT64_C( 0xFFF0000000000000 );
    static const uint64_t NEG_NAN           = UINT64_C( 0xFFF8000000000000 );
    
    static const uint64_t VALUE_BOXED       = UINT64_C( 0xFF00000000000000 );
    static const uint64_t VALUE_NULL        = UINT64_C( 0xFFF1000000000000 );
    static const uint64_t VALUE_UNDEF       = UINT64_C( 0xFFF1000100000000 );
    static const uint64_t VALUE_FALSE       = UINT64_C( 0xFFF1000200000000 );
    static const uint64_t VALUE_TRUE        = UINT64_C( 0xFFF1000300000000 );
    static const uint64_t FIRST_GCOBJECT    = UINT64_C( 0xFFF2000000000000 );
    static const uint64_t FIRST_OBJECT      = UINT64_C( 0xFFFC000000000000 );
    
    static const uint64_t TAG_BITS          = UINT64_C( 0x00FF000000000000 );
    static const uint64_t POINTER_BITS      = UINT64_C( 0x0000FFFFFFFFFFFF );

    static inline uint32_t HI( uint64_t x ) { return (uint32_t)( x >> 32 ); }
    

    explicit yl_value( uint64_t value );


    union
    {
        uint64_t    _value;
        double      _number;
        struct
        {
            uint32_t    _hi;
            uint32_t    _lo;
        };
    };


};



/*
    Hash values and check for equality.
*/


hash32_t    hash( yl_value v );
bool        equal( yl_value a, yl_value b );
bool        test( yl_value v );




/*
    A valref is a value stored on the heap.  Accesses MUST be atomic in order
    for garbage collection to work properly.
*/


class yl_valref
{
public:

    yl_valref();
    ~yl_valref();

    void        set( yl_value value );
    yl_value    get() const;
    
    void        mark() const;


private:

    yl_valref( const yl_valref& ) = delete;
    yl_valref& operator = ( const yl_valref& ) = delete;

    std::atomic< uint64_t > _value;

};





/*
    Garbage-collected arrays of values.
*/

class yl_valarray : public yl_gcobject
{
public:

    static yl_stackref< yl_valarray > alloc( size_t size );
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


/*
    yl_value
*/

inline yl_value::yl_value( uint64_t value )
    :   _value( value )
{
}

inline yl_value::yl_value()
    :   _value( VALUE_UNDEF )
{
}

inline yl_value::yl_value( yl_undef_t )
    :   _value( VALUE_UNDEF )
{
}

inline yl_value::yl_value( yl_null_t )
    :   _value( VALUE_NULL )
{
}

inline yl_value::yl_value( yl_bool_t truefalse )
    :   _value( truefalse ? VALUE_TRUE : VALUE_FALSE )
{
}

inline yl_value::yl_value( double number )
    :   _number( number )
{
    assert( ! isnan( number ) || _value == POS_NAN || _value == NEG_NAN );
}

inline yl_value::yl_value( uint8_t kind, yl_gcobject* gcobject )
    :   _value( VALUE_BOXED | (uint64_t)kind << 48 | (uint64_t)gcobject )
{
    assert( (uintptr_t)gcobject <= UINT64_C( 0x0000FFFFFFFFFFFF ) );
}


inline yl_objkind yl_value::kind() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return (yl_objkind)( _value >> 48 & 0xFF );
    }
    else
    {
        return (yl_objkind)( _hi >> 16 & 0xFF );
    }
}


inline bool yl_value::is_null() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value == VALUE_NULL;
    }
    else
    {
        return _hi == HI( VALUE_NULL );
    }
}

inline bool yl_value::is_undef() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value == VALUE_UNDEF;
    }
    else
    {
        return _hi == HI( VALUE_UNDEF );
    }
}

inline bool yl_value::is_false() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value == VALUE_FALSE;
    }
    else
    {
        return _hi == HI( VALUE_FALSE );
    }
}

inline bool yl_value::is_true() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value == VALUE_TRUE;
    }
    else
    {
        return _hi == HI( VALUE_TRUE );
    }
}

inline bool yl_value::is_number() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value <= NEG_INF || _value == NEG_NAN;
    }
    else
    {
        return _hi <= HI( NEG_INF ) || _hi == HI( NEG_NAN );
    }
}


inline bool yl_value::is_gcobject() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value >= FIRST_GCOBJECT && _value != NEG_NAN;
    }
    else
    {
        return _hi >= HI( FIRST_GCOBJECT ) && _hi != HI( NEG_NAN );
    }
}


inline bool yl_value::is( yl_objkind kind ) const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return ( _value & TAG_BITS ) == ( (uint64_t)kind << 48 );
    }
    else
    {
        return ( _hi & HI( TAG_BITS ) ) == ( (uint32_t)kind << 16 );
    }
}

inline bool yl_value::is_object() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return _value >= FIRST_OBJECT;
    }
    else
    {
        return _hi >= HI( FIRST_OBJECT );
    }
}


inline double yl_value::number() const
{
    return _number;
}


inline yl_gcobject* yl_value::gcobject() const
{
    if ( sizeof( uintptr_t ) == 8 )
    {
        return (yl_gcobject*)(uintptr_t)( _value & POINTER_BITS );
    }
    else
    {
        return (yl_gcobject*)(uintptr_t)_lo;
    }
}




inline hash32_t hash( yl_value v )
{
    if ( v.is( YLOBJ_STRING ) )
    {
        yl_string* string = (yl_string*)v.gcobject();
        return string->hash();
    }
    else
    {
        return hash32( &v._value, sizeof( uint64_t ) );
    }
}

inline bool equal( yl_value a, yl_value b )
{
    if ( a._value == b._value )
    {
        return true;
    }
    
    if ( a.is_number() && b.is_number() && a.number() == b.number() )
    {
        return true;
    }
    
    if ( a.is( YLOBJ_STRING ) && b.is( YLOBJ_STRING ) )
    {
        yl_string* sa = (yl_string*)a.gcobject();
        yl_string* sb = (yl_string*)b.gcobject();

        if ( sa->size() != sb->size() )
        {
            return false;
        }
        
        return memcmp( sa->c_str(), sb->c_str(), sa->size() ) == 0;
    }
    
    return false;
}


inline bool test( yl_value v )
{
    return v._value != yl_value::POS_ZERO
        && v._value != yl_value::NEG_ZERO
        && (   v._value < yl_value::VALUE_NULL
            || v._value > yl_value::VALUE_FALSE );
}






/*
    yl_valref
*/


inline yl_valref::yl_valref()
    :   _value( yl_value::VALUE_UNDEF )
{
}

inline yl_valref::~yl_valref()
{
}

inline void yl_valref::set( yl_value value )
{
    yl_value oldval = yl_value( _value.load( std::memory_order_relaxed ) );
    if ( oldval.is_gcobject() )
    {
        yl_current->write_barrier( oldval.gcobject() );
    }

    _value.store( value._value, std::memory_order_relaxed );
}

inline yl_value yl_valref::get() const
{
    return yl_value( _value.load( std::memory_order_relaxed ) );
}

inline void yl_valref::mark() const
{
    yl_value value = yl_value( _value.load( std::memory_order_relaxed ) );
    if ( value.is_gcobject() )
    {
        yl_current->mark( value.gcobject() );
    }
}




/*
    yl_valarray
*/

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


