//
//  xec_inline.h
//
//  Created by Edmund Kapusniak on 30/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_INLINE_H
#define XEC_INLINE_H

#include "xec_code.h"
#include "xec_object.h"
#include "xec_string.h"
#include "xec_value.h"


/*
    I would like a lot of methods inlined, but their classes have dependences
    on each other.  So all the inline functions are gathered here.
*/



/*
    xec_code.h
*/


inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, unsigned a, unsigned b )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint8_t)a << 16
            | (uint8_t)b << 24 )
{
}

inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, unsigned c )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint16_t)c << 16 )
{
}

inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, signed j )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (int16_t)j << 16 )
{
}


inline xec_opcode xec_opinst::opcode()
{
    return (xec_opcode)(uint8_t)i;
}

inline unsigned xec_opinst::r()
{
    return (uint8_t)( i >> 8 );
}

inline unsigned xec_opinst::a()
{
    return (uint8_t)( i >> 16 );
}

inline unsigned xec_opinst::b()
{
    return (uint8_t)( i >> 24 );
}

inline unsigned xec_opinst::c()
{
    return (uint16_t)( i >> 16 );
}

inline signed xec_opinst::j()
{
    return (int16_t)( i >> 16 );
}




inline const char* xec_module::name()
{
    return mname;
}
    
inline xec_function* xec_module::script_function()
{
    assert( 0 < mfunccount );
    return mfuncs[ 0 ];
}
    
inline xec_objkey xec_module::key( unsigned k )
{
    assert( k < mkeycount );
    return mkeys[ k ];
}

inline xec_value xec_module::value( unsigned v )
{
    assert( v < mvaluecount );
    return mvalues[ v ];
}

inline xec_function* xec_module::function( unsigned f )
{
    assert( f < mfunccount );
    return mfuncs[ f ];
}

inline unsigned xec_module::function_count()
{
    return mfunccount;
}










/*
    xec_object.h
*/


inline bool operator == ( xec_objkey a, xec_objkey b )
{
    return a.k == b.k;
}

inline bool operator != ( xec_objkey a, xec_objkey b )
{
    return a.k != b.k;
}

inline size_t std::hash< xec_objkey >::operator()( const xec_objkey& k ) const
{
    return std::hash< void* >()( k.k );
}



inline void xec_upval::incref()
{
    refcount += 1;
}

inline void xec_upval::decref()
{
    refcount -= 1;
    if ( refcount == 0 )
    {
        delete this;
    }
}


inline xec_value xec_upval::get() const
{
    return mvalue;
}

inline void xec_upval::assign( xec_value value )
{
    value.incref();
    mvalue.decref();
    mvalue = value;
}



inline void xec_object::incref()
{
    refcount += 1;
}

inline void xec_object::decref()
{
    refcount -= 1;
    if ( refcount == 0 )
    {
        // TODO.
/*
        if ( isarray() )
            delete &array();
        else if ( istable() )
            delete &table();
        else if ( isclosure() )
            delete &closure();
        else if ( isiter() )
            delete &iter();
        else
            delete this;
*/
    }
}

inline xec_value xec_object::key( xec_objkey key ) const
{
    return mkeys.at( key );
}

inline void xec_object::setkey( xec_objkey key, xec_value value )
{
    xec_value& v = mkeys[ key ];
    value.incref();
    v.decref();
    v = value;
}

inline bool xec_object::isarray() const
{
    return mkind == ARRAY;
}

inline bool xec_object::istable() const
{
    return mkind == TABLE;
}

inline bool xec_object::isclosure() const
{
    return mkind == CLOSURE;
}

inline bool xec_object::isiter() const
{
    return mkind == ITER;
}

inline xec_array& xec_object::array()
{
    return *(xec_array*)this;
}

inline xec_table& xec_object::table()
{
    return *(xec_table*)this;
}

inline xec_closure& xec_object::closure()
{
    return *(xec_closure*)this;
}

inline xec_iter& xec_object::iter()
{
    return *(xec_iter*)this;
}




inline size_t xec_array::size() const
{
    return mvalues.size();
}

inline xec_value xec_array::index( size_t index ) const
{
    return mvalues.at( index );
}

inline void xec_array::setindex( size_t index, xec_value value )
{
    xec_value& v = mvalues[ index ];
    value.incref();
    v.decref();
    v = value;
}




inline xec_value xec_table::index( xec_value index ) const
{
    return mvalues.at( index );
}

inline void xec_table::setindex( xec_value index, xec_value value )
{
    xec_value& v = mvalues[ index ];
    value.incref();
    v.decref();
    v = value;
}






inline xec_function* xec_closure::function() const
{
    return mfunction;
}

inline xec_upval* xec_closure::upval( unsigned i ) const
{
    assert( i < mfunction->upval_count() );
    return mupvals[ i ];
}





/*
    xec_string.h
*/


inline size_t xec_string::size() const
{
    return slength;
}

inline const char* xec_string::c_str() const
{
    return schars;
}

inline char xec_string::at( size_t i ) const
{
    assert( i < slength );
    return schars[ i ];
}





/*
    xec_value.h
*/


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


inline bool xec_value::test() const
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

inline xec_object& xec_value::object() const
{
    return *(xec_object*)( i & ~MARK_MASK );
}

inline xec_string& xec_value::string() const
{
    return *(xec_string*)( i & ~MARK_MASK );
}


inline void xec_value::incref()
{
/*
    if ( isobject() )
    {
        object().decref();
    }
    else if ( isstring() )
    {
        string().decref();
    }
*/

    if ( ( i & REF_MASK ) == REF_MASK )
    {
        int& refcount = *(int*)( i & ~MARK_MASK );
        refcount += 1;
    }
}

inline void xec_value::decref()
{
/*
    if ( isobject() )
    {
        object().decref();
    }
    else if ( isstring() )
    {
        string().decref();
    }
*/
    if ( ( i & REF_MASK ) == REF_MASK )
    {
        int& refcount = *(int*)( i & ~MARK_MASK );
        if ( refcount > 1 )
        {
            refcount -= 1;
        }
        else
        {
            if ( isobject() )
            {
                object().decref();
            }
            else if ( isstring() )
            {
                string().decref();
            }
        }
    }
}



inline bool operator == ( const xec_value& a, const xec_value& b )
{
    return a.i == b.i;
}

inline bool operator != ( const xec_value& a, const xec_value& b )
{
    return a.i != b.i;
}

inline size_t std::hash< xec_value >::operator()( const xec_value& v ) const
{
    return std::hash< uint64_t >()( v.i );
}





#endif
