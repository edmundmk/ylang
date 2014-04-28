//
//  xec_declaration.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_declaration.h"
#include <stddef.h>




xec_declaration::~xec_declaration()
{
}

void xec_declaration::set_thiscall( bool thiscall )
{
}




xec_declaration_var::xec_declaration_var(
            xec_expression_list* name_list, xec_expression_list* expr_list )
{
}





xec_declaration_object::xec_declaration_object()
{
}

void xec_declaration_object::set_name( xec_expression* name )
{
}

void xec_declaration_object::set_prototype( xec_expression* prototype )
{
}

void xec_declaration_object::add_declaration( xec_declaration* declaration )
{
}
  
xec_constructor_object* xec_declaration_object::as_constructor( xec_token* token )
{
    return NULL;
}




xec_declaration_prototype::xec_declaration_prototype(
                    xec_expression* name, xec_expression_list* params )
{
}

void xec_declaration_prototype::set_thiscall( bool thiscall )
{
}

void xec_declaration_prototype::set_coroutine( bool coroutine )
{
}




xec_declaration_function::xec_declaration_function(
                    xec_expression* name, xec_expression_list* params )
    :   xec_declaration_prototype( name, params )
{
}

void xec_declaration_function::set_body( xec_statement_compound* body )
{
}





