//
//  ystring.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSTRING_H
#define YSTRING_H


#include <functional>
#include <hash.h>
#include "yobject.h"


/*
    A string.  String data is stored inline.  String hashes are calculated
    lazily.  Hashes are used to early-out when comparing strings for equality.
*/

class ostring : public obase
{
public:

    static ometatype metatype;

    static ostring*     alloc( const char* string );
    static ostring*     alloc( size_t size );

    static ostring*     strcat( ostring* a, ostring* b );
    static int          strcmp( ostring* a, ostring* b );

    hash32_t            hash() const;
    size_t              size() const;
    const char*         c_str() const;
    const char*         data() const;
    
    char*               buffer();


protected:
    
    ostring( ometatype* metatype, size_t size );
    

private:

    friend class oheap;
    friend class osymbol;

    mutable hash32_t    shash;
    size_t              ssize   : 30;
    mutable bool        shashed : 1;
    bool                ssymbol : 1;
    char                sdata[];

};






/*
    An osymbol is a special kind of ostring* which ensures fast comparisons
    by ensuring that symbols are in the symbol table.  Symbols are reference
    types requiring a custom write barrier.
*/

class osymbol
{
public:

    osymbol();
    osymbol( const char* s );
    osymbol( ostring* s );

    explicit operator bool () const;
    ostring* operator -> () const;
    ostring* get() const;
    

private:

    friend class owb< osymbol >;
    friend bool operator == ( osymbol a, osymbol b );
    friend struct std::hash< osymbol >;
    
    ostring* string;

};


bool operator == ( osymbol a, osymbol b );

namespace std
{
template <> struct hash< osymbol >
{
    size_t operator () ( const osymbol& a ) const;
};
}




template <>
class owb< osymbol >
{
public:

    owb();
    owb( osymbol q );
    owb& operator = ( osymbol q );
    owb& operator = ( const owb& q );

    operator osymbol () const;
    ostring* operator -> () const;
    

private:

    friend struct omark< osymbol >;

    std::atomic< ostring* > string;

};


template <>
struct omark< osymbol >
{
    typedef owb< osymbol > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};




/*

*/


#include <algorithm>


/*
    ostring
*/


inline hash32_t ostring::hash() const
{
    if ( ! shashed )
    {
        shash = hash32( sdata, ssize );
        shashed = true;
    }
    return shash;
}

inline size_t ostring::size() const
{
    return ssize;
}

inline const char* ostring::c_str() const
{
    return sdata;
}

inline const char* ostring::data() const
{
    return sdata;
}

inline char* ostring::buffer()
{
    // Only call this if you know what you are doing.
    assert( ! ssymbol );
    shashed = false;
    return sdata;
}






/*
    osymbol
*/

inline osymbol::osymbol()
    :   string( nullptr )
{
}

inline osymbol::osymbol( const char* s )
    :   string( ocontext::context->heap->make_symbol( s ) )
{
}

inline osymbol::osymbol( ostring* s )
    :   string( s->ssymbol ? s : ocontext::context->heap->make_symbol( s ) )
{
}

inline osymbol::operator bool () const
{
    return string != nullptr;
}

inline ostring* osymbol::operator -> () const
{
    return string;
}

inline ostring* osymbol::get() const
{
    return string;
}



inline bool operator == ( osymbol a, osymbol b )
{
    return a.string == b.string;
}

inline size_t std::hash< osymbol >::operator () ( const osymbol& a ) const
{
    return a->hash();
}






inline owb< osymbol >::owb()
    :   string( nullptr )
{
}

inline owb< osymbol >::owb( osymbol q )
    :   string( q.string )
{
}

inline owb< osymbol >& owb< osymbol >::operator = ( osymbol q )
{
    ostring* string = this->string.load( std::memory_order_relaxed );
    if ( string )
    {
        string->mark();
    }
    
    this->string.store( q.string, std::memory_order_relaxed );
    return *this;
}

inline owb< osymbol >& owb< osymbol >::operator = ( const owb& q )
{
    return this->operator = ( (osymbol)q );
}

inline owb< osymbol >::operator osymbol () const
{
    return osymbol( string.load( std::memory_order_relaxed ) );
}

inline ostring* owb< osymbol >::operator -> () const
{
    return string.load( std::memory_order_relaxed );
}



inline void omark< osymbol >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // On the mark thread, loading a reference, must be memory_order_consume.
    ostring* string = value.string.load( std::memory_order_consume );
    if ( string )
    {
        string->mark( work, colour );
    }
}



#endif




