//
//  ostring.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OSTRING_H
#define OSTRING_H


#include <functional>
#include <hash.h>
#include "obase.h"


/*
    A string.  String data is stored inline.  String hashes are calculated
    lazily.  Hashes are used to early-out when comparing strings for equality.
*/

class ostring : public obase
{
public:

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

    static ometatype metatype;
    static void mark_string( oworklist* work, obase* object, ocolour colour );
    
    ostring( ometatype* metatype, size_t size );
    

private:

    friend class osymbol;

    mutable hash32_t    shash;
    size_t              ssize   : 30;
    mutable bool        shashed : 1;
    bool                ssymbol : 1;
    char                sdata[];

};



/*
    Strings have a specialized write barrier which marks them without adding
    them to the grey list.
*/

template <>
class owb< ostring* >
{
public:

    owb();
    owb& operator = ( ostring* q );
    owb& operator = ( const owb& q );
    
    operator ostring* () const;
    ostring* operator -> () const;
    

private:

    friend struct omark< ostring* >;

    std::atomic< ostring* > p;

};

template <>
struct omark< ostring* >
{
    typedef owb< ostring* > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
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

    ostring* operator -> () const;
    

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

inline ostring* ostring::alloc( const char* string )
{
    ostring* s = alloc( strlen( string ) );
    memcpy( s->buffer(), string, s->size() );
    return s;
}

inline ostring* ostring::alloc( size_t size )
{
    void* p = malloc( sizeof( ostring ) + size + 1 );
    return new ( p ) ostring( &metatype, size );
}

inline ostring::ostring( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   shash( 0 )
    ,   ssize( size )
    ,   shashed( false )
    ,   ssymbol( false )
{
    sdata[ ssize ] = '\0';
}

inline ostring* ostring::strcat( ostring* a, ostring* b )
{
    ostring* result = alloc( a->size() + b->size() );
    memcpy( result->buffer(), a->data(), a->size() );
    memcpy( result->buffer() + a->size(), b->data(), b->size() );
    return result;
}

inline int ostring::strcmp( ostring* a, ostring* b )
{
    if ( a == b )
        return 0;
    size_t size = std::min( a->size(), b->size() );
    int result = memcmp( a->data(), b->data(), size );
    if ( result != 0 )
        return result;
    else if ( a->size() < b->size() )
        return -1;
    else if ( a->size() > b->size() )
        return 1;
    else
        return 0;
}

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






inline owb< ostring* >::owb()
    :   p( nullptr )
{
}

inline owb< ostring* >& owb< ostring* >::operator = ( ostring* q )
{
    // If old value is not marked, mark it.  Strings contain no references
    // and therefore have no need to be marked grey and added to the work list.
    ostring* p = this->p.load( std::memory_order_relaxed );
    if ( p )
    {
        ocolour mark_colour = ocontext::context->mark_colour;
        p->mark( mark_colour, mark_colour );
    }
    
    // Update value.
    this->p.store( q, std::memory_order_relaxed );
    return *this;
}

inline owb< ostring* >& owb< ostring* >::operator = ( const owb& q )
{
    return this->operator = ( (ostring*)q );
}

inline owb< ostring* >::operator ostring* () const
{
    return p.load( std::memory_order_relaxed );
}

inline ostring* owb< ostring* >::operator -> () const
{
    return p.load( std::memory_order_relaxed );
}

inline void omark< ostring* >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // Again, just mark without submitting to work list.  As this is the
    // collector thread, we must consume the pointer to ensure we see the
    // correct mark colour of newly created objects.
    ostring* p = value.p.load( std::memory_order_consume );
    if ( p )
    {
        p->mark( colour, colour );
    }
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

inline ostring* osymbol::operator -> () const
{
    return string;
}



bool operator == ( osymbol a, osymbol b )
{
    return a.string == b.string;
}

size_t std::hash< osymbol >::operator () ( const osymbol& a ) const
{
    return a->hash();
}






inline owb< osymbol >::owb()
    :   string( nullptr )
{
}

inline owb< osymbol >& owb< osymbol >::operator = ( osymbol q )
{
    ostring* string = this->string.load( std::memory_order_relaxed );
    if ( string )
    {
        ocolour mark_colour= ocontext::context->mark_colour;
        string->mark( mark_colour, mark_colour );
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
        string->mark( colour, colour );
    }
}



#endif




