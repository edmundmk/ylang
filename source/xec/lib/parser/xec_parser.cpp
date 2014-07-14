//
//  xec_parser.cpp
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_parser.h"



xec_parser::xec_parser( xec_script* script )
    :   script( script )
{
}



xec_script* xec_parser::get_script()
{
    return script;
}



double xec_parser::parse_number( xec_token* token )
{
    return 0;
}


xec_ast_node* xec_parser::expr_name( xec_ast_node* name )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_proto( xec_ast_node* proto )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_call( xec_ast_node* expr, xec_ast_node* args )
{
    return NULL;
}


xec_ast_node* xec_parser::expr_compare(
        xec_token* token, xec_ast_node* lhs, xec_ast_node* rhs )
{
    return NULL;
}



