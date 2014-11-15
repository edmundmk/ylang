//
//  yl_srcbuf.h
//
//  Created by Edmund Kapusniak on 10/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_SRCBUF_H
#define YL_SRCBUF_H


#include <stdarg.h>
#include <stdlib.h>


class yl_srcbuf
{
public:

    yl_srcbuf();
    ~yl_srcbuf();

    size_t      size() const;
    const char* c_str() const;

    void        printf( const char* format, ... )                   __attribute__(( __format__( printf, 2, 3 ) ));
    void        vprintf( const char* format, va_list arguments )    __attribute__(( __format__( printf, 2, 0 ) ));

private:

    size_t      bcapacity;
    size_t      bsize;
    char*       btext;

};


#endif
