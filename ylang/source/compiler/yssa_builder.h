//
//  yssa_builder.h
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_BUILDER_H
#define YSSA_BUILDER_H



#include <unordered_map>
#include <hash_pair.h>
#include "yssa.h"
#include "yl_ast_visitor.h"



/*
    Transform an AST into an SSA module.
*/


class yssa_builder : private yl_ast_visitor< yssa_builder, int, int >
{
public:

    explicit yssa_builder( yl_diagnostics* diagnostics );
    ~yssa_builder();

    bool                build( yl_ast* ast );
    yssa_module_p       get_module();


private:

    friend class yl_ast_visitor< yssa_builder, int, int >;
    
    
    /*
        Each block passes control either to the next block, or if it has a
        test and the test fails, to the fail block.
    */

    enum link_kind
    {
        NEXT,
        FAIL,
    };
    


    /*
        We build using a virtual stack.  When an expression node is visited
        its value(s) are pushed onto the stack.  When the node is used in
        an operation, it is popped.  When variables are clobbered, the stack
        is inspected to find clobbered values which must be replaced with
        loads.
    */
    
    struct stack_entry
    {
        yssa_block*                 block;
        size_t                      index;
        yssa_opinst*                value;
    };
    
    
    /*
        When you exit each scope, you might have to close upvals or
        iterators declared and pushed in the scope.  And each scope can
        have an exception handler block.
    */
    
    struct scope_entry
    {
        yl_ast_scope*               scope;
        uint8_t                     localups;
        uint8_t                     itercount;
        yssa_block*                 xchandler;
    };
    
    
    /*
        Break and continue into a scope requires closing upvals/iterators.
    */
    
    enum break_kind
    {
        BREAK,
        CONTINUE,
    };
    
    typedef std::pair< yl_ast_scope*, unsigned > break_key;
    
    struct break_entry
    {
        yl_ast_scope*               scope;
        break_kind                  kind;
        uint8_t                     localups;
        uint8_t                     itercount;
        std::vector< yssa_block* >  blocks;
    };
    
    

    /*
        We build a function at a time.
    */

    void build( yl_ast_func* astf );



    /*
        AST visit methods.  count is the number of values to push, and
        the return value is the number actually pushed (always the same
        or fewer).  A count of -1 means 'all available values', including
        potentially a multival.  The multival is not included in the
        returned result count.
    */

    using yl_ast_visitor< yssa_builder, int, int >::visit;

    int fallback( yl_ast_node* node, int count );
    
    int visit( yl_stmt_block* node, int count );
    int visit( yl_stmt_if* node, int count );
    int visit( yl_stmt_switch* node, int count );
    int visit( yl_stmt_while* node, int count );
    int visit( yl_stmt_do* node, int count );
    int visit( yl_stmt_foreach* node, int count );
    int visit( yl_stmt_for* node, int count );
    int visit( yl_stmt_using* node, int count );
    int visit( yl_stmt_try* node, int count );
    int visit( yl_stmt_catch* node, int count );
    int visit( yl_stmt_delete* node, int count );
    int visit( yl_stmt_case* node, int count );
    int visit( yl_stmt_continue* node, int count );
    int visit( yl_stmt_break* node, int count );
    int visit( yl_stmt_return* node, int count );
    int visit( yl_stmt_throw* node, int count );

    int visit( yl_ast_func* node, int count );
    int visit( yl_expr_null* node, int count );
    int visit( yl_expr_bool* node, int count );
    int visit( yl_expr_number* node, int count );
    int visit( yl_expr_string* node, int count );
    int visit( yl_expr_local* node, int count );
    int visit( yl_expr_global* node, int count );
    int visit( yl_expr_upref* node, int count );
    int visit( yl_expr_objref* node, int count );
    int visit( yl_expr_superof* node, int count );
    int visit( yl_expr_key* node, int count );
    int visit( yl_expr_inkey* node, int count );
    int visit( yl_expr_index* node, int count );
    int visit( yl_expr_preop* node, int count );
    int visit( yl_expr_postop* node, int count );
    int visit( yl_expr_unary* node, int count );
    int visit( yl_expr_binary* node, int count );
    int visit( yl_expr_compare* node, int count );
    int visit( yl_expr_logical* node, int count );
    int visit( yl_expr_qmark* node, int count );
    int visit( yl_new_new* node, int count );
    int visit( yl_new_object* node, int count );
    int visit( yl_new_array* node, int count );
    int visit( yl_new_table* node, int count );
    int visit( yl_expr_mono* node, int count );
    int visit( yl_expr_call* node, int count );
    int visit( yl_expr_yield* node, int count );
    int visit( yl_expr_vararg* node, int count );
    int visit( yl_expr_unpack* node, int count );
    int visit( yl_expr_list* node, int count );
    int visit( yl_expr_assign* node, int count );
    int visit( yl_expr_assign_list* node, int count );


    // Control flow.
    yssa_block* make_block( unsigned flags = 0 );
    yssa_block* next_block( unsigned flags = 0 );
    yssa_block* fail_block( unsigned flags = 0 );
    yssa_block* label_block( unsigned flags = 0 );
    void link_block( yssa_block* prev, link_kind kind, yssa_block* next );


    // Emitting instructions.
    yssa_opinst* make_op( int sloc, uint8_t opcode, uint8_t ocount, uint8_t rcount );
    yssa_opinst* op( int sloc, uint8_t opcode, uint8_t ocount, uint8_t rcount );
    yssa_opinst* assign_op( int sloc, yl_ast_opkind opkind, size_t operands );


    // Definitions and lookups.
    yssa_variable* variable( yl_ast_name* name );
    yssa_variable* varobj( yl_new_object* object );
    yssa_variable* temporary( const char* name, int sloc );
    
    yssa_opinst* declare( int sloc, yssa_variable* variable, yssa_opinst* value );
    yssa_opinst* assign( int sloc, yssa_variable* variable, yssa_opinst* value );
    yssa_opinst* lookup( yssa_variable* variable );
    yssa_opinst* lookup_block( yssa_block* block, yssa_variable* variable );
    void seal_block( yssa_block* block );

    void call( yssa_opinst* callop );
    void clobber( yssa_variable* v );


    // Virtual stack.
    void execute( yl_ast_node* statement );
    size_t push_all( yl_ast_node* expression, int* count );
    size_t push( yl_ast_node* expression, int count );
    size_t push_op( yssa_opinst* op );
    int push_select( int sloc, yssa_opinst* selop, int count );
    void pop( size_t index, int count, yssa_opinst** ops );
    yssa_opinst* peek( size_t index, size_t i );
    
    size_t push_lvalue( yl_ast_node* lvalue );
    size_t push_evalue( yl_ast_node* lvalue, size_t index );
    yssa_opinst* assign_lvalue(
            int sloc, yl_ast_node* lvalue, size_t index, yssa_opinst* value );
    void pop_lvalue( yl_ast_node* lvalue, size_t index );


    // Scope stack.
    void open_scope( yl_ast_scope* scope, yssa_block* xchandler = nullptr );
    void close_scope( int sloc, yl_ast_scope* scope );
    
    break_entry* open_break( yl_ast_scope* scope, break_kind kind );
    void close_break( break_entry* b, yssa_block* target );

    void close( int sloc, size_t closeups, size_t closeiter );


    yl_diagnostics*                 diagnostics;
    yssa_module_p                   module;
    std::unordered_map< yl_ast_func*, yssa_function* > funcmap;

    yssa_function*                  function;
    yssa_block*                     block;

    std::unordered_map< void*, yssa_variable* > variables;

    std::vector< stack_entry >      stack;
    yssa_opinst*                    multival;
    
    std::vector< yssa_variable* >   localups;
    size_t                          itercount;
    std::vector< scope_entry >      scopes;
    std::unordered_map< break_key, std::unique_ptr< break_entry > > breaks;

};




#endif
