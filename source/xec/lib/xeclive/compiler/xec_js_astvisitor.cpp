//
//  xec_js_astvisitor.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_js_astvisitor.h"
#include "xec_source_writer.h"
#include "xec_seq.h"
#include "xec_seq_builder.h"




xec_js_astvisitor::xec_js_astvisitor( xec_source_writer* w )
    :   w( w )
{
}


void xec_js_astvisitor::fallback( xec_ast_node* node )
{
    // Build seq from expression.
    xec_seq seq;
    xec_seq_builder builder( &seq );
    builder.build( node );
    xec_seq_print( &seq );
    
    // Turn seq into Javascript.
    // TODO.


}


void xec_js_astvisitor::visit( xec_stmt_block* node )
{
    w->open( "{" );
    w->indent();
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        xec_ast_node* stmt = node->stmts.at( i );
        visit( stmt );
        w->newline();
    }
    w->outdent();
    w->close( "}" );
}

void xec_js_astvisitor::visit( xec_stmt_if* node )
{
    
}

void xec_js_astvisitor::visit( xec_stmt_switch* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_while* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_do* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_foreach* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_for* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_using* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_try* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_catch* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_delete* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_case* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_continue* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_break* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_return* node )
{
}

void xec_js_astvisitor::visit( xec_stmt_throw* node )
{
}

