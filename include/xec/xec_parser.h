//
//  xec_parser.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_PARSER_H
#define XEC_PARSER_H


#include <stdint.h>
#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <region.h>
#include <symbol.h>


struct xec_token;


class xec_parser
{
public:

    xec_parser();
    explicit xec_parser( const char* filename );
    ~xec_parser();

    bool                parse( const char* filename );

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                diagnostic( int sloc, const char* format, ... );
    size_t              diagnostic_count();
    const char*         diagnostic( size_t index );


/* lemon private: */


    void destroy( xec_token* token );


private:

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
    
};



#endif
