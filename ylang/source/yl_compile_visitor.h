//
//  yl_compile_visitor.h
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_COMPILE_VISITOR_H
#define YL_COMPILE_VISITOR_H


#include "yl_ast_visitor.h"
#include <y_script.h>




/*
    yl uses a register-based virtual machine, which owes a lot to lua.
    
    The original compiler used an SSA-based intermediate representation and
    performed a register allocation pass to minimize moves.
    
    This compiler is much simpler.  The generated bytecode ensures that locals
    are always kept in the same register, and that the register file is used
    in a stack-like manner - all values up to the stack top are live.
    
    This simplifies exception handling, garbage collection, and debug info.
*/



/*
    The compiler's first pass visits the nodes of the AST and produces dumb
    bytecode, as if directly translating the operations of a stack machine.
    This initial bytecode is correct, but slow, containing many redundant
    move instructions.
*/




struct yl_compile_script
{
    yl_ast_func*                func;
    std::vector< yl_ast_func* > functions;
    std::vector< double >       numbers;
    std::vector< std::string >  strings;
    std::vector< std::string >  keys;
    std::vector< y_opinst >     code;
    std::vector< y_xframe >     xframes;
    std::vector< int >          slocs;
    std::vector< y_diname >     dnames;
};



class yl_compile_visitor
    :   private yl_ast_visitor< yl_compile_script, int, int >
{
public:

    yl_compile_visitor();
    std::unique_ptr< yl_compile_script > compile( yl_ast_func* func );


private:

    
    struct listval
    {
        listval() : r( -1 ), count( 0 ) {}
        listval( unsigned r, int count ) : r( r ), count( count ) {}
    
        unsigned    r;
        int         count;
    };
    
    struct lvalue
    {
        lvalue( yl_ast_node_kind kind, unsigned o, unsigned k )
            :   kind( kind ), o( o ), k( k ) {}
    
        yl_ast_node_kind kind;
        unsigned    o;
        unsigned    k;
    };
    
    struct branch
    {
        branch( yl_ast_scope* target ) : target( target ) {}
    
        yl_ast_scope*       target;
        std::vector< int >  jumps;
    };
    
    
    /*
        Each visit method takes the number of values we want to push
        onto the stack, and returns the number actually pushed (which
        is always the same number or fewer).  -1 as a request means
        'all available values', and as a return value means 'a
        variable number of values determined at runtime'.
    */

    using yl_ast_visitor< yl_compile_script, int, int >::visit;

    int         fallback( yl_ast_node* node, int count );
    
    int         visit( yl_stmt_block* node, int count );
    int         visit( yl_stmt_if* node, int count );
    int         visit( yl_stmt_switch* node, int count );
    int         visit( yl_stmt_while* node, int count );
    int         visit( yl_stmt_do* node, int count );
    int         visit( yl_stmt_foreach* node, int count );
    int         visit( yl_stmt_for* node, int count );
    int         visit( yl_stmt_using* node, int count );
    int         visit( yl_stmt_try* node, int count );
    int         visit( yl_stmt_catch* node, int count );
    int         visit( yl_stmt_delete* node, int count );
    int         visit( yl_stmt_case* node, int count );
    int         visit( yl_stmt_continue* node, int count );
    int         visit( yl_stmt_break* node, int count );
    int         visit( yl_stmt_return* node, int count );
    int         visit( yl_stmt_throw* node, int count );

    int         visit( yl_ast_func* node, int count );
    int         visit( yl_expr_null* node, int count );
    int         visit( yl_expr_bool* node, int count );
    int         visit( yl_expr_number* node, int count );
    int         visit( yl_expr_string* node, int count );
    int         visit( yl_expr_local* node, int count );
    int         visit( yl_expr_global* node, int count );
    int         visit( yl_expr_upref* node, int count );
    int         visit( yl_expr_objref* node, int count );
    int         visit( yl_expr_superof* node, int count );
    int         visit( yl_expr_key* node, int count );
    int         visit( yl_expr_inkey* node, int count );
    int         visit( yl_expr_index* node, int count );
    int         visit( yl_expr_preop* node, int count );
    int         visit( yl_expr_postop* node, int count );
    int         visit( yl_expr_unary* node, int count );
    int         visit( yl_expr_binary* node, int count );
    int         visit( yl_expr_compare* node, int count );
    int         visit( yl_expr_logical* node, int count );
    int         visit( yl_expr_qmark* node, int count );
    int         visit( yl_new_new* node, int count );
    int         visit( yl_new_object* node, int count );
    int         visit( yl_new_array* node, int count );
    int         visit( yl_new_table* node, int count );
    int         visit( yl_expr_mono* node, int count );
    int         visit( yl_expr_call* node, int count );
    int         visit( yl_expr_yield* node, int count );
    int         visit( yl_expr_vararg* node, int count );
    int         visit( yl_expr_unpack* node, int count );
    int         visit( yl_expr_list* node, int count );
    int         visit( yl_expr_assign* node, int count );
    int         visit( yl_expr_assign_list* node, int count );
    
    
    /*
        Helper functions, which emit opcodes, track the stack state, and
        implement control flow.
    */
    
    void        compare_op( int sloc,
                    yl_ast_opkind opkind, unsigned r, unsigned a, unsigned b );
    void        assign_op( int sloc,
                    yl_ast_opkind opkind, unsigned r, unsigned a, unsigned b );

    void        op( int sloc, y_opcode op, unsigned r, unsigned a, unsigned b );
    void        op( int sloc, y_opcode op, unsigned r, unsigned c );
    void        op( int sloc, y_opcode op, unsigned r, signed j );


    int         jump( int sloc, y_opcode opcode, unsigned r );
    int         label();
    void        patch( int jump, int label );

    
    unsigned    function( yl_ast_func* func );
    unsigned    string( const char* string, size_t length );
    unsigned    number( double number );
    unsigned    key( const char* key );

    
    void        execute( yl_ast_node* statement );
    
    unsigned    push();
    unsigned    push( yl_ast_node* expression );
    void        pop( unsigned r );
    
    listval     push_list( int count );
    listval     push_list( yl_ast_node* expression, int count );
    void        pop_list( listval lv );
    
    lvalue      push_lvalue( yl_ast_node* lvexpr );
    unsigned    push_evaluate_lvalue( lvalue lv );
    void        assign( lvalue lv, unsigned v );
    void        pop_lvalue( lvalue lv );
    
    
    void        open_scope( yl_ast_scope* scope );

    void        declare( unsigned r, bool upval, void* p, const char* name );
    unsigned    local_index( void* p );
    unsigned    upval_index( void* p );

    void        open_break( yl_ast_scope* target );
    void        add_break( int sloc, yl_ast_scope* target );
    void        close_break( yl_ast_scope* target, int label );
    void        open_continue( yl_ast_scope* target );
    void        add_continue( int sloc, yl_ast_scope* target );
    void        close_continue( yl_ast_scope* target, int label );
    
    unsigned    push_iterator();
    void        pop_iterator( unsigned i );

    void        open_xframe();
    int         close_xframe();
    void        xframe_handler( int xframe, int label );

    void        close_scope( yl_ast_scope* scope );
    

    yl_ast_func*                func;
    std::vector< branch >       break_stack;
    std::vector< branch >       continue_stack;
    std::vector< y_xframe >     xframe_stack;

    yl_compile_script*          s;

};




#endif




