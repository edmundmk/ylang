//
//  xec_source_writer.h
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SOURCE_WRITER_H
#define XEC_SOURCE_WRITER_H


#include <stddef.h>


class xec_source_writer
{
public:

    xec_source_writer();

    void keyword( const char* text );
    void identifier( const char* text );
    void symbol( const char* text );
    void open( const char* brace );
    void close( const char* brace );
    void string( const char* string, size_t length );
    void newline();
    void indent();
    void outdent();


private:

    int mindent;

};



#endif


