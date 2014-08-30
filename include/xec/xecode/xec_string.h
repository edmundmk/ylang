//
//  xec_string.h
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_STRING_H
#define XEC_STRING_H


#include <stddef.h>
#include <assert.h>



class xec_string
{
public:

    static xec_string* create( const char* s );
    static xec_string* create( const char* s, size_t size );
    static xec_string* create( size_t size, char** buffer );
    
    void        incref();
    void        decref();
    
    size_t      size() const;
    const char* c_str() const;
    char        at( size_t i ) const;
    

private:

    xec_string();

    int         refcount;
    int         slength;
    char        schars[];
    
};





#endif
