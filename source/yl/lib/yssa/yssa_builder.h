//
//  yssa_builder.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_BUILDER_H
#define YSSA_BUILDER_H


#include "yssa_astvisitors.h"
#include "yssa.h"
#include <vector>
#include <utility>
#include <symkey.h>


struct yl_ast;
struct yl_ast_name;
struct yl_ast_scope;
struct yl_ast_node;
struct yl_ast_func;
struct yl_new_object;
struct yssa_follow;
struct yssa_build_func;
struct yssa_build_block;


typedef std::unique_ptr< yssa_build_func > yssa_build_func_p;
typedef std::unique_ptr< yssa_build_block > yssa_build_block_p;



/*
    An lvalue is not completely evaluated as it must be assigned to.
*/

struct yssa_lvalue
{
    yssa_lvalue();

    int             sloc;
    yssa_opcode     opcode;
    yssaop*         object;
    union
    {
    yssaop*         index;
    yl_ast_name*   local;
    const char*     key;
    int             upval;
    };
};



/*
    A list of unpacked values.
*/

struct yssa_valist
{
    yssa_valist();

    std::vector< yssaop* > values;
    yssaop*         multival;
};





/*
    Traverse an AST and build SSA form IR from it.
*/

class yssa_builder
{
public:

    explicit yssa_builder( yssa_module* module );
    ~yssa_builder();
    
    bool            build( yl_ast* ast );

    template < typename ... arguments_t > yssaop* op( arguments_t&& ... arguments );
    yssaop*         op( yssaop* op );

    const char*     key( const char* key );

    void            define( yl_ast_name* name, yssaop* value );
    void            define( yl_new_object* object, yssaop* value );
    void            define( yl_ast_node* temporary, yssaop* value );
    
    yssaop*         lookup( int sloc, yl_ast_name* name );
    yssaop*         lookup( yl_new_object* object );
    yssaop*         lookup( yl_ast_node* temporary );
    
    void            close_scope( yl_ast_scope* scope );
    
    void            ifthen( yssaop* condition );
    void            ifelse();
    void            ifend();
    
    void            switchopen( yssaop* value );
    void            switchcase();
    void            switchcase( yssaop* value );
    void            switchbreak();
    void            switchend();
    
    void            loopopen( bool dowhile );
    void            loopcontinue();
    void            loopbreak();
    void            loopdowhile();
    void            loopend();
    
    void            funcreturn();

    yssa_func*      func( yl_ast_func* func );
    yssaop*         expr( yl_ast_node* node );
    void            unpack( yssa_valist* l, yl_ast_node* node, int count );
    
    void            lvalue( yssa_lvalue* lvalue, yl_ast_node* node );
    yssaop*         lvalue_value( yssa_lvalue* lvalue );
    void            lvalue_assign( yssa_lvalue* lvalue, yssaop* val );
    
    
private:

    void            build_function( yl_ast_func* astf, yssa_func* ssaf );

    yssa_build_block* make_block();
    void            link( yssa_follow* follow, yssa_build_block* block );
    bool            follow_block();

    void            define_name( void* name, yssaop* value );
    yssaop*         lookup_name( void* name, int sloc, const char* text );

    yssaop*         lookup_block( yssa_build_block* loop,
                            yssa_build_block* block, void* name );
    void            seal_block( yssa_build_block* block );
    yssaop*         lookup_join( yssa_build_block* loop,
                            yssa_build_block* block, void* name,
                                    std::vector< yssaop* >* lookups );
    yssaop*         ensure_phi( yssa_build_block* block );
    
    void            remove_ref( yssa_block* block );
    yssaop*         remove_ref( yssaop* op );
    

    yssa_module*        module;
    yssa_build_expr     build_expr;
    yssa_build_unpack   build_unpack;
    yssa_build_stmt     build_stmt;
    std::unordered_map< symkey, const char* > keymap;
    std::unordered_map< yl_ast_func*, yssa_func* > funcmap;
    std::unordered_map< yl_ast_name*, yssa_string* > namemap;
    yssa_build_func_p   b;

};


template < typename ... arguments_t >
yssaop* yssa_builder::op( arguments_t&& ... arguments )
{
    return op( yssaop::op( std::forward< arguments_t >( arguments ) ... ) );
}



#endif


