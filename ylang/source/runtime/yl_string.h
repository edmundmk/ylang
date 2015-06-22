//
//  yl_string.h
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_STRING_H
#define YL_STRING_H


#include <hash.h>
#include <assert.h>
#include "yl_context.h"



/*
    Garbage-collected strings.
    
    Strings have an optional hash, which is calculated the first time it is
    used in a table or as an property key.
 
    Strings can be symbols.  The system ensures that there is only one unique
    instance of each symbol with the same string.  This allows faster property
    lookups as testing for equality is a simple pointer test.
 
*/

class yl_string : public yl_gcobject
{
public:

    static yl_gctype gctype;

    static yl_stackref< yl_string > alloc( const char* string );
    static yl_stackref< yl_string > alloc( const char* string, size_t size );

    static yl_string*   concat( yl_string* a, yl_string* b );
    
    yl_stackref< yl_string > symbol();
    
    hash32_t            hash() const;
    size_t              size() const;
    const char*         c_str() const;
    

private:

    friend class yl_context_impl;
    friend class yl_symbol;
    
    enum
    {
        HASH    = YL_GCFLAG_USER1,
        SYMBOL  = YL_GCFLAG_USER2,
    };

    explicit yl_string( size_t size );
    
    static void destroy( yl_gcheap* heap, yl_gcobject* object );

    mutable hash32_t    _hash;
    size_t              _size;
    char                _s[];

};



/*
    A reference to a string used as a symbol.  Note that these references
    are not GC-visible - the associated string must be reachable through
    normal references.
*/

class yl_symbol
{
public:

    yl_symbol( yl_string* string );

    yl_string* string() const;

private:

    friend bool operator == ( const yl_symbol& a, const yl_symbol& b );
    friend struct std::hash< yl_symbol >;

    yl_string* _symbol;


};


bool operator == ( const yl_symbol& a, const yl_symbol& b );


namespace std
{

template <> struct hash< yl_symbol >
{
    size_t operator () ( const yl_symbol& s ) const;
};

}





/*

*/


/*
    yl_string
*/

inline yl_stackref< yl_string > yl_string::symbol()
{
    if ( check_gcflags( SYMBOL ) )
    {
        return this;
    }
    else
    {
        return yl_current->symbol( this );
    }
}


inline hash32_t yl_string::hash() const
{
    if ( ! check_gcflags( HASH ) )
    {
        _hash = hash32( _s, _size );
        set_gcflags( HASH );
    }
    return _hash;
}

inline size_t yl_string::size() const
{
    return _size;
}

inline const char* yl_string::c_str() const
{
    return _s;
}


/*
    yl_symbol
*/

inline yl_symbol::yl_symbol( yl_string* string )
    :   _symbol( string )
{
    assert( _symbol->check_gcflags( yl_string::SYMBOL ) );
}

inline yl_string* yl_symbol::string() const
{
    return _symbol;
}

inline bool operator == ( const yl_symbol& a, const yl_symbol& b )
{
    return a._symbol == b._symbol;
}

inline size_t std::hash< yl_symbol >::operator () ( const yl_symbol& s ) const
{
    return s._symbol->hash();
}



#endif



