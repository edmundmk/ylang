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

    explicit xec_parser( xec_ast* root );
    ~xec_parser();
    
    
    xec_ast*            get_root();
    template < typename object_t, typename ... arguments_t >
        object_t*       alloc( arguments_t ... arguments );
    

    bool                parse( const char* filename );
    
    
    void                newline( int sloc );
    template < typename ... arguments_t >
        xec_token*      make_token( arguments_t ... arguments );
    xec_token*          make_identifier( int sloc, region_buffer* buffer );
    void                destroy( xec_token* token );
    double              parse_number( xec_token* token );


    xec_ast_scope*      get_scope();
    xec_ast_scope*      block_scope( xec_ast_node* node );
    void                close_scope( xec_ast_scope* scope );
    void                close_switch( xec_ast_scope* scope );
    void                dowhile( xec_ast_scope* scope );


    xec_new_object*     object( int sloc, xec_ast_node* name,
                                xec_ast_node* proto );
    void                prototype( int sloc, xec_ast_node* name,
                                xec_name_list* params, bool yield );
    xec_ast_func*       function( int sloc, xec_ast_node* name,
                                xec_name_list* params, bool yield, bool dot );
    void                var( int sloc, xec_ast_node* names,
                                xec_ast_node* rvals );
    
    
    void                statement( xec_ast_node* stmt );
    xec_ast_scope*      continue_target( int sloc );
    xec_ast_scope*      break_target( int sloc );


    xec_ast_node*       lookup( int sloc, const char* identifier, bool outer );
    xec_ast_node*       key( int sloc, xec_ast_node* obj, const char* key );

    xec_ast_node*       lvalue( xec_ast_node* lv );
    void                lvalue_list(
                                xec_ast_node* list, xec_ast_node_list* lv );

    xec_name_list*      name_list( xec_ast_node* list );
    xec_expr_list*      expr_list( xec_ast_node* list );
    xec_ast_node*       mono( xec_ast_node* expr );
    xec_ast_node*       unpack( xec_ast_node* expr );
    xec_ast_node*       test_expr( xec_ast_node* expr );
    xec_ast_node*       compare( int sloc, xec_ast_opkind op,
                                xec_ast_node* lhs, xec_ast_node* rhs );
    xec_ast_node*       assign( int sloc, xec_ast_opkind op,
                                xec_ast_node* lv, xec_ast_node* rv );
    xec_ast_node*       varstmt( int sloc, xec_ast_node* names,
                                xec_ast_node* rvals );
    xec_ast_node*       delstmt( int sloc, xec_ast_node* expr );
    xec_stmt_foreach*   foreachstmt( int sloc, xec_ast_scope* scope,
                                xec_ast_node* lvals, xec_ast_node* expr,
                                        bool declare, bool eachkey );
    xec_stmt_catch*     catchstmt( int sloc, xec_ast_scope* scope,
                                xec_ast_node* lval, xec_ast_node* proto,
                                        bool declare );

    void                check_vararg( int sloc );
    void                check_yield( int sloc );

    
private:

    static const size_t BUFFER_SIZE = 4 * 1024 * 1024;
    static const size_t ERROR_LIMIT = 25;

    xec_ast_name*       declare( int sloc, const char* name );
    xec_ast_node*       declare( xec_ast_node* name );
    void                declare_list(
                                xec_ast_node* names, xec_ast_node_list* lv );
    void                declname( int sloc,
                                xec_ast_node* name, xec_ast_node* decl );
    xec_ast_node*       declqual( int sloc, xec_ast_node* name, bool outer );
    xec_ast_scope*      imply( xec_ast_scope* scope,
                                xec_ast_node* name, bool lookup );
    xec_ast_scope*      lookup_prototype( int sloc,
                                xec_ast_scope* outer, xec_ast_node* name,
                                        xec_name_list* params, bool yield );
    int                 upval( xec_ast_func* func, xec_ast_upval uv );
    xec_ast_node*       delval( xec_ast_node* dv );
    

    xec_ast*                        root;
    std::deque< void* >             recycle_tokens;
    std::deque< xec_ast_scope* >    scopes;
    
    
};



template < typename object_t, typename ... arguments_t >
object_t* xec_parser::alloc( arguments_t ... arguments )
{
    return new ( root->alloc ) object_t( arguments ... );
}



#endif /* XEC_ASTBUILDER */




