//
//  y_string.h
//
//  Created by Edmund Kapusniak on 02/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_STRING_H
#define Y_STRING_H


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "y_object.h"



/*
    Immutable strings.
*/

class y_string : public y_object
{
public:

    static yauto< y_string > alloc( const char* s );
    static yauto< y_string > alloc( const char* format, ... );
    static yauto< y_string > alloc( const char* format, va_list arguments );

    static yauto< y_string > strcat( yref< y_string > a, yref< y_string > b );
    static int strcmp( yref< y_string > a, yref< y_string > b );

    size_t size() const;
    const char* c_str() const;


private:

    friend class y_object;
    
    y_string();

    char _s[];

};



/*
*/



inline int y_string::strcmp( yref< y_string > a, yref< y_string > b )
{
    return ::strcmp( a->c_str(), b->c_str() );
}

inline size_t y_string::size() const
{
    return strlen( _s );
}

inline const char* y_string::c_str() const
{
    return _s;
}


#endif
