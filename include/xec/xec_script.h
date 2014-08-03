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



#if 0


#include <stdlib.h>
#include <string>
#include <deque>
#include <region.h>












class xec_script
{
public:

    xec_script();
    explicit xec_script( const char* filename );
    ~xec_script();

    void                parameters( size_t argc, const char* argv[] );
    bool                parse( const char* filename );

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                diagnostic( int sloc, const char* format, ... );
    size_t              diagnostic_count();
    const char*         diagnostic( size_t index );

    xec_ast_func*       get_script();
    size_t              get_function_count();
    xec_ast_func*       get_function( size_t index );
    

private:

    friend class xec_parser;
    
    region                      alloc;
    std::string                 filename;
    std::deque< int >           newlines;
    std::deque< const char* >   diagnostics;
    xec_ast_func*               script;
    std::deque< xec_ast_func* > functions;


};



#endif

