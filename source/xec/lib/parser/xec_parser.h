//
//  xec_parser.h
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_PARSER_H
#define XEC_PARSER_H


#include <deque>
#include <unordered_set>
#include <symbol.h>
#include "xec_script.h"
#include "xec_ast.h"



class region_buffer;
class xec_script;

struct xec_token;



class xec_parser
{
public:

    explicit xec_parser( xec_script* script );
    
    xec_script*     get_script();
    template < typename object_t, typename ... arguments_t >
        object_t*   alloc( arguments_t ... arguments );
    

    bool            parse( const char* filename );


    void            newline( int sloc );
    template < typename ... arguments_t >
        xec_token*  make_token( arguments_t ... arguments );
    xec_token*      make_identifier( int sloc, region_buffer* buffer );
    

    void            destroy( xec_token* token );
    double          parse_number( xec_token* token );


    xec_ast_node*   expr_name( xec_ast_node* name );
    xec_ast_node*   expr_proto( xec_ast_node* proto );
    xec_ast_node*   expr_call( xec_ast_node* expr, xec_ast_node* args );
    xec_ast_node*   expr_compare( xec_token* op, xec_ast_node* lhs, xec_ast_node* rhs );
    xec_ast_node*   expr_append( xec_ast_node* list, xec_ast_node* expr );
    xec_ast_node*   expr_final( xec_ast_node* list, xec_ast_node* final );
    xec_expr_list*  expr_list( xec_ast_node* list );
    xec_ast_node*   expr_lvalue( xec_ast_node* lvalue );
    void            expr_lvalue_list( xec_expr_list* list, xec_ast_node_list* lvalues );
    xec_ast_node*   expr_assign( xec_token* op, xec_ast_node* lv, xec_ast_node* rv );
    

private:

    static const size_t BUFFER_SIZE         = 4 * 1024 * 1024;
    static const size_t DIAGNOSTIC_LIMIT    = 25;

    xec_script*                     script;
    std::deque< void* >             recycle_tokens;
    std::unordered_set< symkey >    identifiers;
    

};



template < typename object_t, typename ... arguments_t >
object_t* xec_parser::alloc( arguments_t ... arguments )
{
    return new ( script->alloc ) object_t( arguments ... );
}



#endif /* XEC_ASTBUILDER */
