//
//  xec_seq_astvisitor.h
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SEQ_ASTVISITOR_H
#define XEC_SEQ_ASTVISITOR_H


#include "xec_ast_visitor.h"
#include "xec_seq.h"




class xec_seq_astvisitor
    :   public xec_ast_visitor< xec_seq_astvisitor, void >
{
public:

    explicit xec_seq_astvisitor( xec_seq* seq );

    using xec_ast_visitor< xec_seq_astvisitor, void >::visit;

    void fallback( xec_ast_node* node );
    
    void visit( xec_ast_func* node );
    void visit( xec_expr_null* node );
    void visit( xec_expr_bool* node );
    void visit( xec_expr_number* node );
    void visit( xec_expr_string* node );
    void visit( xec_expr_local* node );
    void visit( xec_expr_global* node );
    void visit( xec_expr_upref* node );
    void visit( xec_expr_objref* node );
    void visit( xec_expr_key* node );
    void visit( xec_expr_inkey* node );
    void visit( xec_expr_index* node );
    void visit( xec_expr_preop* node );
    void visit( xec_expr_postop* node );
    void visit( xec_expr_unary* node );
    void visit( xec_expr_binary* node );
    void visit( xec_expr_compare* node );
    void visit( xec_expr_logical* node );
    void visit( xec_expr_qmark* node );
    void visit( xec_new_new* node );
    void visit( xec_new_object* node );
    void visit( xec_new_array* node );
    void visit( xec_new_table* node );
    void visit( xec_expr_mono* node );
    void visit( xec_expr_call* node );
    void visit( xec_expr_yield* node );
    void visit( xec_expr_vararg* node );
    void visit( xec_expr_unpack* node );
    void visit( xec_expr_list* node );
    void visit( xec_expr_assign* node );
    void visit( xec_expr_assign_list* node );


private:

    void lvalue( xec_ast_node* lvalue );
    void seq_eval( xec_ast_node* node );

    xec_seq* seq;

};







#endif


