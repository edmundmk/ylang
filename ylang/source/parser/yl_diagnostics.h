//
//  yl_diagnostics.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_DIAGNOSTICS_H
#define YL_DIAGNOSTICS_H



#include <deque>
#include <string>
#include <memory>
#include <symkey.h>




/*
    Converts source locations into line/column locations, and owns the list
    of errors.
*/


class yl_diagnostics
{
public:

    void            set_filename( const char* filename );
    void            newline( int sloc );

    const char*     get_filename();
    int             get_line( int sloc );
    int             get_column( int sloc );
    
    void            error( int sloc, const char* format, ... );
    
    size_t          error_count() const;
    const char*     error( size_t index ) const;

    
private:


    std::string                 filename;
    std::deque< int >           newlines;
    std::deque< std::string >   errors;

};



#endif





