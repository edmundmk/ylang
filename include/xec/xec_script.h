//
//  xec_script.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SCRIPT_H
#define XEC_SCRIPT_H



#include <deque>
#include <string>
#include <region.h>
#include <symbol.h>


struct xec_ast;



/*
    A script object represents a xec script.  It stores:
    
        -   The filename.
        -   Newline offsets.
        -   Identifiers and strings.
        -   Errors.

*/



class xec_script
{
public:

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                error( int sloc, const char* format, ... );
    size_t              error_count();
    const char*         error( size_t index );


private:

    friend class xec_parser;

    region                          alloc;
    std::string                     filename;
    std::deque< int >               newlines;
    std::unordered_set< symkey >    identifiers;
    std::deque< std::string >       errors;

};





/*
    Parse a script file and generate an AST.
*/

bool xec_parse( xec_script* script, xec_ast* ast,
            const char* filename, size_t argc, const char* const* argv );





#endif





