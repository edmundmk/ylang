//
//  xec_astprinter.h
//
//  Created by Edmund Kapusniak on 05/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_ASTPRINTER_H
#define XEC_ASTPRINTER_H


#include "xec_astvisitor.h"


class xec_astprinter : protected xec_astvisitor< xec_astprinter, void, int >
{
public:

    void print( xec_declaration* decl );
    void print( xec_expression* expr );
    void print( xec_statement* stmt );
    
    
protected:

    friend class xec_astvisitor< xec_astprinter, void, int >;

    using xec_astvisitor< xec_astprinter, void, int >::visit;

    void visit( xec_declaration_var* expr, int indent );
    void visit( xec_declaration_object* expr, int indent );
    void visit( xec_declaration_prototype* expr, int indent );
    void visit( xec_declaration_function* expr, int indent );

    void visit( xec_expression_null* expr, int indent );
    void visit( xec_expression_bool* expr, int indent );
    void visit( xec_expression_number* expr, int indent );
    void visit( xec_expression_string* expr, int indent );
    void visit( xec_expression_identifier* expr, int indent );
    void visit( xec_expression_lookup* expr, int indent );
    void visit( xec_expression_indexkey* expr, int indent );
    void visit( xec_expression_index* expr, int indent );
    void visit( xec_expression_yield* expr, int indent );
    void visit( xec_expression_call* expr, int indent );
    void visit( xec_expression_preop* expr, int indent );
    void visit( xec_expression_postop* expr, int indent );
    void visit( xec_expression_unary* expr, int indent );
    void visit( xec_expression_binary* expr, int indent );
    void visit( xec_expression_comparison* expr, int indent );
    void visit( xec_expression_logical* expr, int indent );
    void visit( xec_expression_conditional* expr, int indent );
    void visit( xec_expression_varargs* expr, int indent );
    void visit( xec_expression_unpack* expr, int indent );
    void visit( xec_expression_list* expr, int indent );
    void visit( xec_expression_assign* expr, int indent );
    void visit( xec_expression_mono* expr, int indent );
    void visit( xec_expression_declare* expr, int indent );
    void visit( xec_constructor_new* expr, int indent );
    void visit( xec_constructor_list* expr, int indent );
    void visit( xec_constructor_table* expr, int indent );
    void visit( xec_constructor_object* expr, int indent );
    void visit( xec_constructor_function* expr, int indent );
    
    void visit( xec_statement_declaration* decl, int indent );
    void visit( xec_statement_expression* decl, int indent );
    void visit( xec_statement_compound* decl, int indent );
    void visit( xec_statement_delete* decl, int indent );
    void visit( xec_statement_if* decl, int indent );
    void visit( xec_statement_switch* decl, int indent );
    void visit( xec_statement_case* decl, int indent );
    void visit( xec_statement_while* decl, int indent );
    void visit( xec_statement_do* decl, int indent );
    void visit( xec_statement_foreach* decl, int indent );
    void visit( xec_statement_for* decl, int indent );
    void visit( xec_statement_continue* decl, int indent );
    void visit( xec_statement_break* decl, int indent );
    void visit( xec_statement_return* decl, int indent );
    void visit( xec_statement_yield* decl, int indent );
    void visit( xec_statement_using* decl, int indent );
    void visit( xec_statement_usingscope* decl, int indent );
    void visit( xec_statement_try* decl, int indent );
    void visit( xec_statement_catch* decl, int indent );
    void visit( xec_statement_throw* decl, int indent );


    const char* opname( xec_operator_kind op );

};


#endif
