//
//  xec_declaration.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_declaration.h"
#include "xec_expression.h"
#include "xec_constructor.h"
#include "xec_statement.h"
#include <stddef.h>
#include "xec_token.h"




xec_declaration::~xec_declaration()
{
}

void xec_declaration::set_thiscall( bool thiscall )
{
}



/*
    var name, name;
    var name, name = expr, exrp;
*/

xec_declaration_var::xec_declaration_var( xec_token* token,
            xec_expression_list* name_list, xec_expression_list* expr_list )
    :   token( token )
    ,   name_list( name_list )
    ,   expr_list( expr_list )
{
}

xec_declaration_dispatch xec_declaration_var::visitor_dispatch()
{
    return XEC_DECLARATION_VAR;
}

int xec_declaration_var::get_location()
{
    return token->sloc;
}

xec_expression_list* xec_declaration_var::get_name_list()
{
    return name_list.get();
}

xec_expression_list* xec_declaration_var::get_expr_list()
{
    return expr_list.get();
}




/*
    name { decl; decl; }
    name : expr { decl; decl; }
*/

xec_declaration_object::xec_declaration_object(
                xec_expression* name, xec_constructor_object* object )
    :   name( name )
    ,   object( object )
{
}

xec_declaration_dispatch xec_declaration_object::visitor_dispatch()
{
    return XEC_DECLARATION_OBJECT;
}

int xec_declaration_object::get_location()
{
    return name->get_location();
}

xec_expression* xec_declaration_object::get_name()
{
    return name.get();
}

xec_constructor_object* xec_declaration_object::get_object()
{
    return object.get();
}





/*
    name( param, param )
*/

xec_declaration_prototype::xec_declaration_prototype(
                    xec_expression* name, xec_expression_list* params )
    :   name( name )
    ,   params( params )
    ,   varargs( false )
    ,   thiscall( false )
    ,   coroutine( false )
{
}

xec_declaration_dispatch xec_declaration_prototype::visitor_dispatch()
{
    return XEC_DECLARATION_PROTOTYPE;
}

int xec_declaration_prototype::get_location()
{
    return name->get_location();
}

void xec_declaration_prototype::set_varargs( bool varargs )
{
    this->varargs = varargs;
}

void xec_declaration_prototype::set_thiscall( bool thiscall )
{
    this->thiscall = thiscall;
}

void xec_declaration_prototype::set_coroutine( bool coroutine )
{
    this->coroutine = coroutine;
}

bool xec_declaration_prototype::get_varargs()
{
    return varargs;
}

bool xec_declaration_prototype::get_thiscall()
{
    return thiscall;
}

bool xec_declaration_prototype::get_coroutine()
{
    return coroutine;
}

xec_expression* xec_declaration_prototype::get_name()
{
    return name.get();
}

xec_expression_list* xec_declaration_prototype::get_parameters()
{
    return params.get();
}



/*
    name( param, param ) { decl, decl }
*/

xec_declaration_function::xec_declaration_function(
                    xec_expression* name, xec_constructor_function* function )
    :   name( name )
    ,   function( function )
{
}

xec_declaration_dispatch xec_declaration_function::visitor_dispatch()
{
    return XEC_DECLARATION_FUNCTION;
}

int xec_declaration_function::get_location()
{
    return name->get_location();
}

xec_expression* xec_declaration_function::get_name()
{
    return name.get();
}

xec_constructor_function* xec_declaration_function::get_function()
{
    return function.get();
}



