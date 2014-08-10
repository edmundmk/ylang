//
//  xec_ssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_BUILDER_H
#define XEC_SSA_BUILDER_H


#include "xec_ssa_build_visitors.h"
#include "xec_ssa.h"


struct xec_ast;
struct xec_ast_name;
struct xec_ast_scope;
struct xec_ast_node;
struct xec_ast_func;
struct xec_new_object;
struct xec_ssa_follow;
struct xec_ssa_build_func;
struct xec_ssa_build_block;




/*
    An lvalue is not completely evaluated as it must be assigned to.
*/


struct xec_ssa_lvalue
{
    xec_ssa_lvalue();

    int             sloc;
    xec_ssa_opcode  opcode;
    xec_ssa_node*   object;
    union
    {
    xec_ssa_node*   index;
    xec_ast_name*   local;
    const char*     literal;
    int             upval;
    };
};



/*
    A list of unpacked values.
*/

struct xec_ssa_valist
{
    xec_ssa_valist();

    std::deque< xec_ssa_node* > values;
    xec_ssa_node*   unpacked;
};





/*
    Traverse an AST and build SSA form IR from it.
*/

class xec_ssa_builder
{
public:

    explicit xec_ssa_builder( xec_ssa* root );
    ~xec_ssa_builder();
    
    bool            build( xec_ast* ast );
    
    template< typename ... arguments_t >
    xec_ssa_packed* packed( arguments_t ... arguments );
    template< typename ... arguments_t >
    xec_ssa_triple* triple( arguments_t ... arguments );
    template< typename ... arguments_t >
    xec_ssa_expand* expand( arguments_t ... arguments );

    xec_ssa_node*   node( xec_ssa_node* node );

    void            define( xec_ast_name* name, xec_ssa_node* value );
    void            define( xec_new_object* object, xec_ssa_node* value );
    void            define( xec_ast_node* temporary, xec_ssa_node* value );
    
    xec_ssa_node*   lookup( int sloc, xec_ast_name* name );
    xec_ssa_node*   lookup( xec_new_object* object );
    xec_ssa_node*   lookup( xec_ast_node* temporary );
    
    void            close_scope( xec_ast_scope* scope );
    
    void            ifthen( xec_ssa_node* condition );
    void            ifelse();
    void            ifend();
    
    void            switchopen( xec_ssa_node* value );
    void            switchcase();
    void            switchcase( xec_ssa_node* value );
    void            switchbreak();
    void            switchend();
    
    void            loopopen();
    void            loopcontinue();
    void            loopbreak();
    void            loopend();
    
    void            excepttry();
    void            exceptcatch();
    void            exceptfinally();
    
    void            funcreturn();

    xec_ssa_func*   func( xec_ast_func* func );
    xec_ssa_node*   expr( xec_ast_node* node );
    void            unpack( xec_ssa_valist* l, xec_ast_node* node, int count );
    
    void            lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node );
    xec_ssa_node*   lvalue_value( xec_ssa_lvalue* lvalue );
    xec_ssa_node*   lvalue_assign( xec_ssa_lvalue* lvalue, xec_ssa_node* val );
    
    
private:

    void            build_function( xec_ast_func* astf, xec_ssa_func* ssaf );

    xec_ssa_build_block* make_block();
    void            link( xec_ssa_follow* follow, xec_ssa_build_block* block );
    bool            follow_block();

    void            define_name( void* name, xec_ssa_node* value );
    xec_ssa_node*   lookup_name( void* name );
    xec_ssa_node*   lookup_name( xec_ssa_build_block* block, void* name );
    void            seal_block( xec_ssa_build_block* block );
    

    xec_ssa* root;
    xec_ssa_build_expr   build_expr;
    xec_ssa_build_unpack build_unpack;
    xec_ssa_build_stmt   build_stmt;
    std::unordered_map< xec_ast_func*, xec_ssa_func* > funcmap;
    std::unordered_map< xec_ast_name*, xec_ssa_name* > namemap;
    std::unique_ptr< xec_ssa_build_func > b;

};



template < typename ... arguments_t >
inline xec_ssa_packed* xec_ssa_builder::packed( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_packed( arguments ... );
}

template < typename ... arguments_t >
inline xec_ssa_triple* xec_ssa_builder::triple( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_triple( arguments ... );
}

template < typename ... arguments_t >
inline xec_ssa_expand* xec_ssa_builder::expand( arguments_t ... arguments )
{
    return new ( root->alloc ) xec_ssa_expand( arguments ... );
}


#endif


