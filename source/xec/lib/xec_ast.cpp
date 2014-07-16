//
//  xec_ast.cpp
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast.h"




xec_ast_node::xec_ast_node( xec_ast_node_kind kind, int sloc )
    :   kind( kind )
    ,   sloc( sloc )
{
}




xec_expr_null::xec_expr_null( int sloc )
    :   xec_ast_node( XEC_EXPR_NULL, sloc )
{
}


xec_expr_bool::xec_expr_bool( int sloc, bool value )
    :   xec_ast_node( XEC_EXPR_BOOL, sloc )
    ,   value( value )
{
}


xec_expr_number::xec_expr_number( int sloc, double value )
    :   xec_ast_node( XEC_EXPR_NUMBER, sloc )
    ,   value( value )
{
}


xec_expr_string::xec_expr_string( int sloc, const char* string, size_t length )
    :   xec_ast_node( XEC_EXPR_STRING, sloc )
    ,   string( string )
    ,   length( length )
{
}


xec_expr_unqual::xec_expr_unqual( int sloc, const char* name )
    :   xec_ast_node( XEC_EXPR_UNQUAL, sloc )
    ,   name( name )
{
}


xec_expr_key::xec_expr_key( int sloc, xec_ast_node* object, const char* key )
    :   xec_ast_node( XEC_EXPR_KEY, sloc )
    ,   object( object )
    ,   key( key )
{
}







