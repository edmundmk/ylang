//
//  xec_ssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_BUILDER_H
#define XEC_SSA_BUILDER_H


#include "xec_ssa_astvisitors.h"
#include "xec_ssa.h"
#include <vector>


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
    xec_ssa_opref   object;
    union
    {
    xec_ssa_opref   index;
    xec_ast_name*   local;
    int             key;
    int             upval;
    };
};



/*
    A list of unpacked values.
*/

struct xec_ssa_valist
{
    xec_ssa_valist();

    std::vector< xec_ssa_opref > values;
    xec_ssa_opref   unpacked;
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

    int             key( const char* key );
    xec_ssa_string* string( const char* string, size_t length );
    xec_ssa_closure* closure( xec_ssa_func* function );
    xec_ssa_args*   args( int resultcount );
    
    template< typename ... arguments_t >
    xec_ssa_opref   op( arguments_t ... arguments );

    void            define( xec_ast_name* name, xec_ssa_opref value );
    void            define( xec_new_object* object, xec_ssa_opref value );
    void            define( xec_ast_node* temporary, xec_ssa_opref value );
    
    xec_ssa_opref   lookup( int sloc, xec_ast_name* name );
    xec_ssa_opref   lookup( xec_new_object* object );
    xec_ssa_opref   lookup( xec_ast_node* temporary );
    
    void            close_scope( xec_ast_scope* scope );
    
    void            ifthen( xec_ssa_opref condition );
    void            ifelse();
    void            ifend();
    
    void            switchopen( xec_ssa_opref value );
    void            switchcase();
    void            switchcase( xec_ssa_opref value );
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
    xec_ssa_opref   expr( xec_ast_node* node );
    void            unpack( xec_ssa_valist* l, xec_ast_node* node, int count );
    
    void            lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node );
    xec_ssa_opref   lvalue_value( xec_ssa_lvalue* lvalue );
    xec_ssa_opref   lvalue_assign( xec_ssa_lvalue* lvalue, xec_ssa_opref val );
    
    
private:

    xec_ssa_opref   addop( xec_ssa_op& op );
    xec_ssa_opref   addop( xec_ssa_slice* slice, xec_ssa_op& op );

    void            build_function( xec_ast_func* astf, xec_ssa_func* ssaf );

    xec_ssa_build_block* make_block();
    void            link( xec_ssa_follow* follow, xec_ssa_build_block* block );
    bool            follow_block();

    void            define_name( void* name, xec_ssa_opref value );
    xec_ssa_opref   lookup_name( void* name, int sloc, const char* text );

    xec_ssa_opref   lookup_block( xec_ssa_build_block* loop,
                            xec_ssa_build_block* block, void* name );
    void            seal_block( xec_ssa_build_block* block );
    xec_ssa_opref   lookup_join( xec_ssa_build_block* loop,
                            xec_ssa_build_block* block, void* name,
                                std::vector< xec_ssa_opref >* lookups );
    xec_ssa_opref   ensure_phi( xec_ssa_build_block* block );
    

    xec_ssa* root;
    xec_ssa_build_expr   build_expr;
    xec_ssa_build_unpack build_unpack;
    xec_ssa_build_stmt   build_stmt;
    std::unordered_map< symkey, int > keymap;
    std::unordered_map< xec_ast_func*, xec_ssa_func* > funcmap;
    std::unordered_map< xec_ast_name*, xec_ssa_name* > namemap;
    std::unique_ptr< xec_ssa_build_func > b;

};



inline xec_ssa_string* xec_ssa_builder::string(
                const char* string, size_t length )
{
    return new ( root->alloc ) xec_ssa_string( string, length );
}

inline xec_ssa_closure* xec_ssa_builder::closure( xec_ssa_func* function )
{
    return new ( root->alloc ) xec_ssa_closure( function );
}

inline xec_ssa_args* xec_ssa_builder::args( int resultcount )
{
    return new ( root->alloc ) xec_ssa_args( resultcount );
}

template< typename ... arguments_t >
inline xec_ssa_opref xec_ssa_builder::op( arguments_t ... arguments )
{
    xec_ssa_op op( arguments ... );
    return addop( op );
}



#endif


