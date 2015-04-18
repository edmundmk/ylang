//
//  yl_string.h
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_STRING_H
#define YL_STRING_H


#include <hash.h>
#include "yl_heapobj.h"



/*
    Garbage-collected strings.
    
    Strings have an optional hash, which is calculated the first time it is
    used in a table or as an property key.
 
    Strings can be symbols.  The system ensures that there is only one unique
    instance of each symbol with the same string.  This allows faster property
    lookups as testing for equality is a simple pointer test.
 
*/

class yl_string : public yl_heapobj
{
public:

    static yl_string*   alloc( const char* string );
    static yl_string*   alloc( const char* string, size_t size );

    static yl_string*   concat( yl_string* a, yl_string* b );
    
    hash32_t            hash() const;
    size_t              length() const;
    const char*         c_str() const;
    

private:

    explicit yl_string( size_t size );

    bool                _is_symbol;
    mutable bool        _has_hash;
    mutable hash32_t    _hash;
    uint32_t            _length;
    char                _s[];

};



/*

*/


inline hash32_t yl_string::hash() const
{
    if ( ! _has_hash )
    {
        _hash = hash32( _s, _length );
        _has_hash = true;
    }
    return _hash;
}

inline size_t yl_string::length() const
{
    return _length;
}

inline const char* yl_string::c_str() const
{
    return _s;
}



#endif



