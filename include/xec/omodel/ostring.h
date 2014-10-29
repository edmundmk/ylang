//
//  ostring.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OSTRING_H
#define OSTRING_H


#include <assert.h>
#include <functional>
#include <hash.h>
#include "omodel.h"


class ostring;
class osymbol;


/*
    A string.  String data is stored inline.  String hashes are calculated
    lazily.  They are useful to early-out when comparing strings for equality.

    Strings are also used as object keys.  Only strings which are symbols can
    be used for key lookup.  Symbolizing a string involves adding them to the
    global symbol table, to ensure that there is only one instance of each
    symbol.  This avoids the need to compare symbols byte-by-byte.
*/

class ostring : public ogcbase
{
public:

    static oimpl::ogctype gctype;
    
    static ostring* alloc( const char* string );
    static ostring* alloc( size_t size );
    
    static ostring* strcat( ostring* a, ostring* b );
    static int      strcmp( ostring* a, ostring* b );

    hash32_t        hash() const;
    size_t          size() const;
    const char*     c_str() const;
    const char*     data() const;
    
    char*           buffer();
    
    
private:

    friend class osymbol;
    friend struct std::hash< osymbol >;
    
    mutable hash32_t    shash;
    uint32_t            ssize   : 30;
    mutable bool        shashed : 1;
    mutable bool        ssymbol : 1;
    char                sdata[];

};



/*
    An osymbol is a reference to a string that is present in the global symbol
    table.  Symbols are treated specially by the garbage collector.
*/

class osymbol
{
public:

    osymbol( const char* s );
    osymbol( ostring* s );
    
    const char* c_str();
    
    
private:

    friend bool operator == ( osymbol a, osymbol b );
    friend struct std::hash< osymbol >;
    
    ostring* string;

};





/*

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




inline const char* osymbol::c_str()
{
    return string->c_str();
}

inline bool operator == ( osymbol a, osymbol b )
{
    return a.string == b.string;
}

namespace std
{
template <>
struct hash< osymbol >
{
    size_t operator () ( osymbol s ) { return s.string->shash; }
};
}






#endif
