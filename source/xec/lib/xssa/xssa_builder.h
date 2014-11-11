//
//  xssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_BUILDER_H
#define XSSA_BUILDER_H


#include "xssa_astvisitors.h"
#include "xssa.h"
#include <vector>
#include <utility>
#include <symbol.h>


struct xec_ast;
struct xec_ast_name;
struct xec_ast_scope;
struct xec_ast_node;
struct xec_ast_func;
struct xec_new_object;
struct xssa_follow;
struct xssa_build_func;
struct xssa_build_block;




/*
    An lvalue is not completely evaluated as it must be assigned to.
*/

struct xssa_lvalue
{
    xssa_lvalue();

    int             sloc;
    xssa_opcode     opcode;
    xssaop*         object;
    union
    {
    xssaop*         index;
    xec_ast_name*   local;
    const char*     key;
    int             upval;
    };
};



/*
    A list of unpacked values.
*/

struct xssa_valist
{
    xssa_valist();

    std::vector< xssaop* > values;
    xssaop*         multival;
};





/*
    Traverse an AST and build SSA form IR from it.
*/

class xssa_builder
{
public:

    explicit xssa_builder( xssa_module* module );
    ~xssa_builder();
    
    bool            build( xec_ast* ast );

    template < typename ... arguments_t > xssaop* op( arguments_t&& ... arguments );
    xssaop*         op( xssaop* op );

    const char*     key( const char* key );

    void            define( xec_ast_name* name, xssaop* value );
    void            define( xec_new_object* object, xssaop* value );
    void            define( xec_ast_node* temporary, xssaop* value );
    
    xssaop*         lookup( int sloc, xec_ast_name* name );
    xssaop*         lookup( xec_new_object* object );
    xssaop*         lookup( xec_ast_node* temporary );
    
    void            close_scope( xec_ast_scope* scope );
    
    void            ifthen( xssaop* condition );
    void            ifelse();
    void            ifend();
    
    void            switchopen( xssaop* value );
    void            switchcase();
    void            switchcase( xssaop* value );
    void            switchbreak();
    void            switchend();
    
    void            loopopen( bool dowhile );
    void            loopcontinue();
    void            loopbreak();
    void            loopdowhile();
    void            loopend();
    
    void            funcreturn();

    xssa_func*      func( xec_ast_func* func );
    xssaop*         expr( xec_ast_node* node );
    void            unpack( xssa_valist* l, xec_ast_node* node, int count );
    
    void            lvalue( xssa_lvalue* lvalue, xec_ast_node* node );
    xssaop*         lvalue_value( xssa_lvalue* lvalue );
    void            lvalue_assign( xssa_lvalue* lvalue, xssaop* val );
    
    
private:

    void            build_function( xec_ast_func* astf, xssa_func* ssaf );

    xssa_build_block* make_block();
    void            link( xssa_follow* follow, xssa_build_block* block );
    bool            follow_block();

    void            define_name( void* name, xssaop* value );
    xssaop*         lookup_name( void* name, int sloc, const char* text );

    xssaop*         lookup_block( xssa_build_block* loop,
                            xssa_build_block* block, void* name );
    void            seal_block( xssa_build_block* block );
    xssaop*         lookup_join( xssa_build_block* loop,
                            xssa_build_block* block, void* name,
                                    std::vector< xssaop* >* lookups );
    xssaop*         ensure_phi( xssa_build_block* block );
    
    void            remove_ref( xssa_block* block );
    xssaop*         remove_ref( xssaop* op );
    

    xssa_module*        module;
    xssa_build_expr     build_expr;
    xssa_build_unpack   build_unpack;
    xssa_build_stmt     build_stmt;
    std::unordered_map< symkey, const char* > keymap;
    std::unordered_map< xec_ast_func*, xssa_func* > funcmap;
    std::unordered_map< xec_ast_name*, xssa_string* > namemap;
    std::unique_ptr< xssa_build_func > b;

};


template < typename ... arguments_t >
xssaop* xssa_builder::op( arguments_t&& ... arguments )
{
    return op( xssaop::op( std::forward< arguments_t >( arguments ) ... ) );
}



#endif


