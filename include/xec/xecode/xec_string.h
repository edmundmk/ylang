//
//  xec_string.h
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_STRING_H
#define XEC_STRING_H



class xec_string
{
public:

    static xec_string* create( const char* s );
    static xec_string* create( const char* s, size_t size );
    static xec_string* create( size_t size, char** buffer );
    
    size_t      size();
    const char* c_str();
    

private:

    xec_string();

    int         refcount;
    int         slength;
    char        schars[];
    
};




#endif
