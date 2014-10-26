//
//  ostring.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OSTRING_H
#define OSTRING_H


#include "omodel.h"


/*
    A string.  String data is stored inline.  Strings are the only GC object
    without a class pointer, as strings never have references to other objects
    and have their own inline tag in the value representation.
*/

class ostring : public obase
{
public:

    static ostring* alloc( const char* string );
    static ostring* alloc( size_t size );
    
    static ostring* strcat( ostring* a, ostring* b );
    static ostring* strcmp( ostring* a, ostring* b );

    size_t      size() const;
    const char* c_str() const;
    const char* data() const;
    char*       data();
    
    
private:
    
    size_t      ssize;
    char        sdata[];

};




#endif
