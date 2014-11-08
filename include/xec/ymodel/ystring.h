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

class ystring : public yobject
{
public:

    static ystring*     alloc( size_t size );
    static ystring*     alloc( size_t size, const char* data );
    static ystring*     alloc( const char* string );

    static ystring*     strcat( ystring* a, ystring* b );
    static int          strcmp( ystring* a, ystring* b );

    hash32_t            hash() const;
    size_t              size() const;
    const char*         c_str() const;
    const char*         data() const;
    
    char*               buffer();


protected:

    friend class yobject;
    friend class yvalue;
    static ymetatype metatype;
    
    ystring( ymetatype* metatype, size_t size );
    

private:

    friend class ymodel;
    friend class ysymbol;

    mutable hash32_t    shash;
    size_t              ssize   : 30;
    mutable bool        shashed : 1;
    bool                ssymbol : 1;
    char                sdata[];

};






/*
    An ysymbol is a special kind of ystring* which ensures fast comparisons
    by ensuring that symbols are in the symbol table.  Symbols are reference
    types requiring a custom write barrier.
*/

class ysymbol
{
public:

    ysymbol();
    ysymbol( const char* s );
    ysymbol( ystring* s );

    explicit operator bool () const;
    ystring* operator -> () const;
    ystring* get() const;
    

private:

    friend bool operator == ( ysymbol a, ysymbol b );
    friend struct std::hash< ysymbol >;
    
    ystring* string;

};


bool operator == ( ysymbol a, ysymbol b );

namespace std
{
template <> struct hash< ysymbol >
{
    size_t operator () ( const ysymbol& a ) const;
};
}




template <>
class ywb< ysymbol >
{
public:

    ywb();
    ywb( const ywb& q );
    ywb( ysymbol q );
    ywb& operator = ( const ywb& q );
    ywb& operator = ( ysymbol q );

    operator ysymbol () const;
    ystring* operator -> () const;
    ystring* get() const;
    

private:

    friend struct ymarktraits< ywb< ysymbol > >;

    std::atomic< ystring* > string;

};


template <>
struct ywbtraits< ysymbol >
{
    typedef ywb< ysymbol > wb;
};

template <>
struct ymarktraits< ywb< ysymbol > >
{
    static void mark( ywb< ysymbol >& wb, yworklist* work, ycolour colour );
};




/*

*/


#include <algorithm>


/*
    ystring
*/


inline hash32_t ystring::hash() const
{
    if ( ! shashed )
    {
        shash = hash32( sdata, ssize );
        shashed = true;
    }
    return shash;
}

inline size_t ystring::size() const
{
    return ssize;
}

inline const char* ystring::c_str() const
{
    return sdata;
}

inline const char* ystring::data() const
{
    return sdata;
}

inline char* ystring::buffer()
{
    // Only call this if you know what you are doing.
    assert( ! ssymbol );
    shashed = false;
    return sdata;
}






/*
    ysymbol
*/

inline ysymbol::ysymbol()
    :   string( nullptr )
{
}

inline ysymbol::ysymbol( const char* s )
    :   string( ycontext::context->model->make_symbol( s ) )
{
}

inline ysymbol::ysymbol( ystring* s )
    :   string( s->ssymbol ? s : ycontext::context->model->make_symbol( s ) )
{
}

inline ysymbol::operator bool () const
{
    return string != nullptr;
}

inline ystring* ysymbol::operator -> () const
{
    return string;
}

inline ystring* ysymbol::get() const
{
    return string;
}



inline bool operator == ( ysymbol a, ysymbol b )
{
    return a.string == b.string;
}

inline size_t std::hash< ysymbol >::operator () ( const ysymbol& a ) const
{
    return a->hash();
}






inline ywb< ysymbol >::ywb()
    :   string( nullptr )
{
}

inline ywb< ysymbol >::ywb( const ywb& q )
    :   ywb( (ysymbol)q )
{
}

inline ywb< ysymbol >::ywb( ysymbol q )
    :   string( q.get() )
{
}

inline ywb< ysymbol >& ywb< ysymbol >::operator = ( ysymbol q )
{
    ystring* string = this->string.load( std::memory_order_relaxed );
    if ( string )
    {
        string->mark();
    }
    
    this->string.store( q.get(), std::memory_order_relaxed );
    return *this;
}

inline ywb< ysymbol >& ywb< ysymbol >::operator = ( const ywb& q )
{
    return this->operator = ( (ysymbol)q );
}

inline ywb< ysymbol >::operator ysymbol () const
{
    return ysymbol( string.load( std::memory_order_relaxed ) );
}

inline ystring* ywb< ysymbol >::operator -> () const
{
    return string.load( std::memory_order_relaxed );
}



inline void ymarktraits< ywb< ysymbol > >::mark(
                ywb< ysymbol >& wb, yworklist* work, ycolour colour )
{
    // On the mark thread, loading a reference, must be memory_order_consume.
    ystring* string = wb.string.load( std::memory_order_consume );
    if ( string )
    {
        string->mark( work, colour );
    }
}



#endif




