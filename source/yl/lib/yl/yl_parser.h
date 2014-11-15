//
//  yl_parser.h
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_PARSER_H
#define YL_PARSER_H


#include <deque>
#include <unordered_set>
#include <symkey.h>
#include "yl_script.h"
#include "yl_ast.h"



class region_buffer;
class yl_script;
struct yl_token;



/*
    The slightly messy guts of parsing xec script.
*/


class yl_parser
{
public:

    explicit yl_parser( yl_ast* root );
    ~yl_parser();
    
    
    yl_ast*            get_root();
    template < typename object_t, typename ... arguments_t >
        object_t*       alloc( arguments_t ... arguments );
    

    bool                parse( const char* filename );
    
    
    void                newline( int sloc );
    template < typename ... arguments_t >
        yl_token*      make_token( arguments_t ... arguments );
    yl_token*          make_identifier( int sloc, region_buffer* buffer );
    void                destroy( yl_token* token );
    double              parse_number( yl_token* token );


    yl_ast_scope*      get_scope();
    yl_ast_scope*      block_scope( yl_ast_node* node );
    void                close_scope( yl_ast_scope* scope );
    void                close_switch( yl_ast_scope* scope );
    void                dowhile( yl_ast_scope* scope );


    yl_new_object*     object( int sloc, yl_ast_node* name,
                                yl_ast_node* proto );
    void                prototype( int sloc, yl_ast_node* name,
                                yl_name_list* params, bool yield );
    yl_ast_func*       function( int sloc, yl_ast_node* name,
                                yl_name_list* params, bool yield, bool dot );
    void                var( int sloc, yl_ast_node* names,
                                yl_ast_node* rvals );
    
    
    void                statement( yl_ast_node* stmt );
    yl_ast_scope*      continue_target( int sloc );
    yl_ast_scope*      break_target( int sloc );


    yl_ast_node*       lookup( int sloc, const char* identifier, bool outer );
    yl_ast_node*       key( int sloc, yl_ast_node* obj, const char* key );

    yl_ast_node*       lvalue( yl_ast_node* lv );
    void                lvalue_list(
                                yl_ast_node* list, yl_ast_node_list* lv );

    yl_name_list*      name_list( yl_ast_node* list );
    yl_expr_list*      expr_list( yl_ast_node* list );
    yl_ast_node*       mono( yl_ast_node* expr );
    yl_ast_node*       unpack( yl_ast_node* expr );
    yl_ast_node*       compare( int sloc, yl_ast_opkind op,
                                yl_ast_node* lhs, yl_ast_node* rhs );
    yl_ast_node*       assign( int sloc, yl_ast_opkind op,
                                yl_ast_node* lv, yl_ast_node* rv );
    yl_ast_node*       varstmt( int sloc, yl_ast_node* names,
                                yl_ast_node* rvals );
    yl_ast_node*       delstmt( int sloc, yl_ast_node* expr );
    yl_stmt_foreach*   foreachstmt( int sloc, yl_ast_scope* scope,
                                yl_ast_node* lvals, yl_ast_node* expr,
                                        bool declare, bool eachkey );
    yl_stmt_catch*     catchstmt( int sloc, yl_ast_scope* scope,
                                yl_ast_node* lval, yl_ast_node* proto,
                                        bool declare );

    void                check_vararg( int sloc );
    void                check_yield( int sloc );

    
private:

    static const size_t BUFFER_SIZE = 4 * 1024 * 1024;
    static const size_t ERROR_LIMIT = 25;

    yl_ast_name*       declare( int sloc, const char* name );
    yl_ast_node*       declare( yl_ast_node* name );
    void                declare_list(
                                yl_ast_node* names, yl_ast_node_list* lv );
    void                declname( int sloc,
                                yl_ast_node* name, yl_ast_node* decl );
    yl_ast_node*       declqual( int sloc, yl_ast_node* name, bool outer );
    yl_ast_scope*      imply( yl_ast_scope* scope,
                                yl_ast_node* name, bool lookup );
    yl_ast_scope*      lookup_prototype( int sloc,
                                yl_ast_scope* outer, yl_ast_node* name,
                                        yl_name_list* params, bool yield );
    int                 upval( yl_ast_func* func, yl_ast_upval uv );
    yl_ast_node*       delval( yl_ast_node* dv );
    

    yl_ast*                        root;
    std::deque< void* >             recycle_tokens;
    std::deque< yl_ast_scope* >    scopes;
    
    
};



template < typename object_t, typename ... arguments_t >
object_t* yl_parser::alloc( arguments_t ... arguments )
{
    return new ( root->alloc ) object_t( arguments ... );
}



#endif /* YL_ASTBUILDER */




