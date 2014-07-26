//
//  xec_script.cpp
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_script.h"
#include "xec_parser.h"



xec_script::xec_script()
    :   script( NULL )
{
}

xec_script::xec_script( const char* filename )
    :   script( NULL )
{
    parse( filename );
}

xec_script::~xec_script()
{
}


void xec_script::parameters( size_t argc, const char* argv[] )
{
    assert( script == NULL );
    

    // Set up global scope, script function, and script scope.
    script = new ( alloc ) xec_ast_func( -1 );
    script->scope = new ( alloc )
            xec_ast_scope( XEC_SCOPE_SCRIPT, NULL, script, script );
    script->block = new ( alloc ) xec_stmt_block( -1 );
    script->scope->block = script->block;
    
  
    // Declare parameters.
    for ( size_t i = 0; i < argc; ++i )
    {
        if ( strcmp( argv[ i ], "..." ) != 0 )
        {
            const char* param = copy_string( argv[ i ] );
        
            // Check for duplicated parameters.
            if ( script->scope->names.find( param )
                            != script->scope->names.end() )
            {
                assert( ! "duplicate parameter" );
            }
            
            // Add name.
            xec_ast_name* name = new ( alloc )
                            xec_ast_name( -1, script->scope, param );
            script->scope->names.emplace( param, name );
            script->parameters.push_back( name );
        }
        else
        {
            // Varargs indicator.
            assert( i == argc - 1 && "varargs must be at the end" );
            script->varargs = true;
        }
    }
}


bool xec_script::parse( const char* filename )
{
    // Default parameters.
    if ( ! script )
    {
        const char* argv[] = { "..." };
        parameters( 1, argv );
    }


    // Set function name.
    script->funcname = copy_string( filename );
    
    
    // Actually parse function.
    xec_parser parser( this );
    return parser.parse( filename );
}



const char* xec_script::get_filename()
{
    return filename.c_str();
}

int xec_script::get_line( int sloc )
{
    auto i = std::lower_bound( newlines.begin(), newlines.end(), sloc );
    return (int)( i - newlines.begin() ) + 1;
}

int xec_script::get_column( int sloc )
{
    int line_start = 0;
    auto i = std::upper_bound( newlines.begin(), newlines.end(), sloc );
    if ( i != newlines.begin() )
        line_start = *( i - 1 );
    return sloc - line_start;
}



void xec_script::diagnostic( int sloc, const char* format, ... )
{
    va_list arguments;
    va_list vlist;
    va_start( arguments, format );

    int bufsize = snprintf( NULL, 0, "%s:%d:%d: ",
            get_filename(), get_line( sloc ), get_column( sloc ) );
    va_copy( vlist, arguments );
    bufsize += vsnprintf( NULL, 0, format, vlist );
    va_end( vlist );
    
    char* buffer = (char*)alloc.malloc( bufsize + 1 );
    int printsize = snprintf( buffer, bufsize, "%s:%d:%d ",
            get_filename(), get_line( sloc ), get_column( sloc ) );
    va_copy( vlist, arguments );
    vsnprintf( buffer + printsize, bufsize - printsize, format, vlist );
    va_end( vlist );

    diagnostics.push_back( buffer );
}

size_t xec_script::diagnostic_count()
{
    return diagnostics.size();
}

const char* xec_script::diagnostic( size_t index )
{
    return diagnostics[ index ];
}



xec_ast_func* xec_script::get_script()
{
    return script;
}

size_t xec_script::get_function_count()
{
    return functions.size();
}

xec_ast_func* xec_script::get_function( size_t index )
{
    return functions.at( index );
}




const char* xec_script::copy_string( const char* s )
{
    char* c = (char*)alloc.malloc( strlen( s ) + 1 );
    strcpy( c, s );
    return c;
}



