//
//  xec_js_astvisitor.h
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_JS_ASTVISITOR_H
#define XEC_JS_ASTVISITOR_H


#include "xec_ast_visitor.h"


class xec_source_writer;


class xec_js_astvisitor
    :   public xec_ast_visitor< xec_js_astvisitor, void >
{
public:

    explicit xec_js_astvisitor( xec_source_writer* w );

    using xec_ast_visitor< xec_js_astvisitor, void >::visit;

    void fallback( xec_ast_node* node );
    
    void visit( xec_stmt_block* node );
    void visit( xec_stmt_if* node );
    void visit( xec_stmt_switch* node );
    void visit( xec_stmt_while* node );
    void visit( xec_stmt_do* node );
    void visit( xec_stmt_foreach* node );
    void visit( xec_stmt_for* node );
    void visit( xec_stmt_using* node );
    void visit( xec_stmt_try* node );
    void visit( xec_stmt_catch* node );
    void visit( xec_stmt_delete* node );
    void visit( xec_stmt_case* node );
    void visit( xec_stmt_continue* node );
    void visit( xec_stmt_break* node );
    void visit( xec_stmt_return* node );
    void visit( xec_stmt_throw* node );


private:

    xec_source_writer* w;

};





#endif
