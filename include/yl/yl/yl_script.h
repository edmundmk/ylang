//
//  yl_script.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_SCRIPT_H
#define YL_SCRIPT_H



#include <deque>
#include <string>
#include <memory>
#include <region.h>
#include <symkey.h>


struct yl_ast;
class yl_srcbuf;
class ymodule;



/*
    A script object represents a xec script.  It stores:
    
        -   The filename.
        -   Newline offsets.
        -   Identifiers and strings.
        -   Errors.
        -   The parsed AST for the script.

*/



class yl_script
{
public:

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                error( int sloc, const char* format, ... );
    size_t              error_count();
    const char*         error( size_t index );

    yl_ast*            get_ast();


private:

    friend class yl_parser;
    friend yl_script* yl_parse( const char*, size_t, const char* const* );

    yl_script();
    

    region                          alloc;
    std::string                     filename;
    std::deque< int >               newlines;
    std::unordered_set< symkey >    identifiers;
    std::deque< std::string >       errors;
    std::unique_ptr< yl_ast >      ast;

};





/*
    Parse a script file and generate an AST.
*/

yl_script* yl_parse( const char* path, size_t argc, const char* const* argv );



/*
    Compile an AST into luajit-compatiable lua.
*/

yl_srcbuf* yl_compile_lj( yl_ast* ast );



/*
    Compile an AST into a ycode module.
*/

ymodule* yl_compile( yl_ast* ast );






#endif





