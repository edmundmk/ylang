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



/*
    The slightly messy guts of parsing xec script.
*/


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


    xec_ast_scope*  get_scope();
    xec_ast_scope*  block_scope( xec_ast_node* node );
    void            close_scope( xec_ast_scope* scope );


    xec_ast_name*   declare( xec_unqual_name* name );
    xec_expr_local* declare_local( xec_unqual_name* name );
    void            declare_local_list(
                        xec_unqual_list* list, xec_ast_node_list* lvalues );


    xec_new_object* object( int sloc, xec_ast_node* name, xec_ast_node* proto );
    void            prototype( int sloc, xec_unqual_proto* proto );
    xec_ast_func*   function( int sloc, xec_ast_node* name,
                        xec_ast_node* params, bool coroutine, bool thisdot );
    void            var( int sloc, xec_unqual_name* lval, xec_ast_node* rval );
    void            var_list( int sloc,
                        xec_unqual_list* lvals, xec_ast_node* rvals );


    void            statement( xec_ast_node* stmt );
    xec_ast_node*   continue_target( int sloc );
    xec_ast_node*   break_target( int sloc );


    xec_ast_node*   resolve( xec_ast_node* unqual );
    xec_expr_call*  resolve( xec_unqual_proto* proto );
    int             upval( xec_ast_func* func, xec_ast_upval uv );
    xec_ast_node*   compare( xec_token* op,
                        xec_ast_node* lhs, xec_ast_node* rhs );
    xec_ast_node*   append( xec_ast_node* list, xec_ast_node* expr );
    xec_ast_node*   final( xec_ast_node* list, xec_ast_node* final );
    xec_expr_list*  list( xec_ast_node* list );
    xec_ast_node*   lvalue( xec_ast_node* lvalue );
    void            lvalue_list( xec_ast_node* list,
                            xec_ast_node_list* lvalues );
    xec_ast_node*   delval( xec_ast_node* delval );
    void            delval_list( xec_ast_node* list,
                            xec_ast_node_list* delvals );
    xec_ast_node*   assign( xec_token* op, xec_ast_node* lv, xec_ast_node* rv );
    
    
private:

    static const size_t BUFFER_SIZE         = 4 * 1024 * 1024;
    static const size_t DIAGNOSTIC_LIMIT    = 25;

    xec_ast_scope* imply( xec_ast_scope* scope,
                    xec_ast_node* name, bool create );
    xec_ast_scope* match_prototype( int sloc, xec_ast_scope* outer,
                    xec_ast_node* name, xec_unqual_list* params,
                        bool varargs, bool coroutine );
    void declname( int sloc, xec_ast_node* name, xec_ast_node* decl );

    xec_script*                     script;
    std::deque< void* >             recycle_tokens;
    std::unordered_set< symkey >    identifiers;
    std::deque< xec_ast_scope* >    scopes;
    

};



template < typename object_t, typename ... arguments_t >
object_t* xec_parser::alloc( arguments_t ... arguments )
{
    return new ( script->alloc ) object_t( arguments ... );
}



#endif /* XEC_ASTBUILDER */
