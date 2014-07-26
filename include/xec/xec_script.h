//
//  xec_script.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SCRIPT_H
#define XEC_SCRIPT_H


#include <stdlib.h>
#include <string>
#include <deque>
#include <region.h>


struct xec_ast_scope;
struct xec_ast_func;


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
    
    const char*         copy_string( const char* s );

    region                      alloc;
    std::string                 filename;
    std::deque< int >           newlines;
    std::deque< const char* >   diagnostics;
    xec_ast_func*               script;
    std::deque< xec_ast_func* > functions;


};



#if 0

#include <stdint.h>
#include <string>
#include <memory>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <region.h>
#include <symbol.h>


struct xec_token;
struct xec_ast;
struct xec_scope;
struct xec_astnode;

class xec_function;
class xec_constructor_function;


class xec_parser
{
public:

    xec_parser();
    explicit xec_parser( const char* filename );
    ~xec_parser();

    void                set_arguments( size_t argc, const char* argv[] );
    bool                parse( const char* filename );

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                diagnostic( int sloc, const char* format, ... );
    
    size_t              diagnostic_count();
    const char*         diagnostic( size_t index );

    xec_ast*            get_ast();

    xec_scope*          get_global_scope();
    xec_function*       get_function();
    xec_constructor_function* get_script();


/* lemon private: */

    region&             get_alloc();

    void                destroy( xec_token* token );
    double              token_number( xec_token* token );

    xec_astnode*        expr_name( xec_astnode* name );
    xec_astnode*        expr_proto( xec_astnode* proto );
    xec_astnode*        expr_call( xec_astnode* expr, xec_astnode* args );
    xec_astnode*        expr_compare( xec_token* token,
                                xec_astnode* lhs, xec_astnode* rhs );

    xec_scope*          get_scope();
    xec_scope*          open_scope();
    void                close_scope( xec_scope* scope );


private:

    friend class xec_astbuilder;

    static const size_t BUFFER_SIZE = 4 * 1024 * 1024;
    static const size_t ERROR_LIMIT = 25;
    
    template < typename ... arguments_t >
        xec_token* make_token( arguments_t ... arguments );
    xec_token* make_identifier( int sloc, region_buffer* buffer );

    region                          alloc;
    std::string                     filename;
    std::deque< int >               newlines;
    std::deque< const char* >       diagnostics;
    
    std::deque< void* >             recycle_tokens;
    std::unordered_set< symkey >    identifiers;
    /*
    xec_scope*                      global_scope;
    xec_function*                   script;
    */
    std::unique_ptr< xec_scope >    global_scope;
    std::unique_ptr< xec_constructor_function > script;
    
};


#endif


#endif
