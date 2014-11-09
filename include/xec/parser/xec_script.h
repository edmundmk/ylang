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
#include <memory>
#include <region.h>
#include <symbol.h>


struct xec_ast;
class xec_srcbuf;
class ymodule;



/*
    A script object represents a xec script.  It stores:
    
        -   The filename.
        -   Newline offsets.
        -   Identifiers and strings.
        -   Errors.
        -   The parsed AST for the script.

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

    xec_ast*            get_ast();


private:

    friend class xec_parser;
    friend xec_script* xec_parse( const char*, size_t, const char* const* );

    xec_script();
    

    region                          alloc;
    std::string                     filename;
    std::deque< int >               newlines;
    std::unordered_set< symkey >    identifiers;
    std::deque< std::string >       errors;
    std::unique_ptr< xec_ast >      ast;

};





/*
    Parse a script file and generate an AST.
*/

xec_script* xec_parse( const char* path, size_t argc, const char* const* argv );



/*
    Compile an AST into luajit-compatiable lua.
*/

xec_srcbuf* xec_compile_lj( xec_ast* ast );



/*
    Compile an AST into a ycode module.
*/

ymodule* xec_compile( xec_ast* ast );






#endif





