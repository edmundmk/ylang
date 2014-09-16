//
//  xec_lj_stmt.h
//
//  Created by Edmund Kapusniak on 10/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_LJ_STMT_H
#define XEC_LJ_STMT_H


#include "xec_ast_visitor.h"


class xec_lj_scope;
class xec_lj_builder;


class xec_lj_stmtvisitor
    :   public xec_ast_visitor< xec_lj_stmtvisitor, void, xec_lj_scope*, int >
{
public:

    explicit xec_lj_stmtvisitor( xec_lj_builder* b );

    using xec_ast_visitor< xec_lj_stmtvisitor, void, xec_lj_scope*, int >::visit;
    
    void fallback( xec_ast_node* node, xec_lj_scope* scope, int indent );
    
    void visit( xec_expr_preop* node, xec_lj_scope* scope, int indent );
    void visit( xec_expr_postop* node, xec_lj_scope* scope, int indent );
    void visit( xec_expr_call* node, xec_lj_scope* scope, int indent );
    void visit( xec_expr_yield* node, xec_lj_scope* scope, int indent );
    void visit( xec_expr_assign* node, xec_lj_scope* scope, int indent );
    void visit( xec_expr_assign_list* node, xec_lj_scope* scope, int indent );
    
    void visit( xec_stmt_block* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_if* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_switch* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_while* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_do* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_foreach* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_for* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_using* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_try* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_catch* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_delete* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_case* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_continue* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_break* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_return* node, xec_lj_scope* scope, int indent );
    void visit( xec_stmt_throw* node, xec_lj_scope* scope, int indent );

    void body( xec_ast_node* node, xec_lj_scope* scope, int indent );
    

private:

    xec_lj_builder* b;

};



#endif
