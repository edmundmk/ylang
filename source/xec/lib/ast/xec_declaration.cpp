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





xec_declaration_object::xec_declaration_object()
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

void xec_declaration_object::set_name( xec_expression* name )
{
    this->name.reset( name );
}

void xec_declaration_object::set_proto( xec_expression* proto )
{
    this->proto.reset( proto );
}

void xec_declaration_object::add_member( xec_declaration* decl )
{
    members.push_back( std::unique_ptr< xec_declaration >( decl ) );
}
  
xec_constructor_object*
        xec_declaration_object::as_constructor( xec_token* token )
{
    xec_constructor_object* object = new xec_constructor_object();
    object->set_token( token );
    object->set_proto( proto.release() );
    for ( auto i = members.begin(); i != members.end(); ++i )
        object->add_member( i->release() );
    delete this;
    return object;
}




xec_declaration_prototype::xec_declaration_prototype(
                    xec_expression* name, xec_expression_list* params )
    :   name( name )
    ,   params( params )
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

void xec_declaration_prototype::set_thiscall( bool thiscall )
{
    this->thiscall = thiscall;
}

void xec_declaration_prototype::set_coroutine( bool coroutine )
{
    this->coroutine = coroutine;
}




xec_declaration_function::xec_declaration_function(
                    xec_expression* name, xec_expression_list* params )
    :   xec_declaration_prototype( name, params )
{
}

xec_declaration_dispatch xec_declaration_function::visitor_dispatch()
{
    return XEC_DECLARATION_FUNCTION;
}

void xec_declaration_function::set_body( xec_statement_compound* body )
{
    this->body.reset( body );
}





