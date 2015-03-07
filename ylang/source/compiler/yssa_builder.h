//
//  yssa_builder.h
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_BUILDER_H
#define YSSA_BUILDER_H



#include <unordered_map>
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
        We build using a virtual stack.  When an expression node is visited
        its value(s) are pushed onto the stack.  When the node is used in
        an operation, it is popped.  When variables are clobbered, the stack
        is inspected to find clobbered values which must be replaced with
        loads.
    */
    
    struct stack_entry
    {
        yssa_block*     block;
        size_t          index;
        yssa_opinst*    value;
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


    // Conversion from AST ops to yl opcodes.
    size_t compare_op( int sloc, yl_ast_opkind opkind, size_t operands );
    

    // Control flow.
    yssa_block* next_block( unsigned flags = 0 );


    // Emitting instructions.
    yssa_opinst* op( int sloc, uint8_t opcode, uint8_t ocount, uint8_t rcount );


    // Definitions and lookups.
    yssa_variable* variable( yl_ast_name* name );
    yssa_variable* varobj( yl_new_object* object );
    yssa_variable* temporary();
    
    void assign( yssa_variable* variable, yssa_opinst* op );
    yssa_opinst* lookup( yssa_variable* variable );


    // Virtual stack.
    size_t  push( yl_ast_node* expression, int count );
    size_t  push( yssa_opinst* op );
    void    pop( size_t index, int count, yssa_opinst** ops );
    
    size_t  push_lvalue( yl_ast_node* lvalue );
    size_t  push_evalue( yl_ast_node* lvalue, size_t index );
    void    assign_lvalue( yl_ast_node* lvalue, size_t index, yssa_opinst* v );
    void    pop_lvalue( yl_ast_node* lvalue, size_t index );


    yl_diagnostics*     diagnostics;
    yssa_module_p       module;
    std::unordered_map< yl_ast_func*, yssa_function* > funcmap;
    yssa_function*      function;
    yssa_block*         block;
    std::vector< stack_entry > stack;

};




#endif
