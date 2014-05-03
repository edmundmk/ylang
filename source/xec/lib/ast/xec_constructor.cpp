//
//  xec_constructor.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_constructor.h"
#include "xec_statement.h"
#include "xec_declaration.h"
#include "xec_token.h"



/*
    new expr( expr, expr )
*/

xec_constructor_new::xec_constructor_new(
        xec_token* token, xec_expression* proto, xec_expression_list* args )
    :   token( token )
    ,   proto( proto )
    ,   args( args )
{
}

xec_expression_dispatch xec_constructor_new::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_NEW;
}

int xec_constructor_new::get_location()
{
    return token->sloc;
}



/*
    [ expr, expr, ... ]
*/

xec_constructor_list::xec_constructor_list()
    :   token( NULL )
{
}

xec_expression_dispatch xec_constructor_list::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_LIST;
}

int xec_constructor_list::get_location()
{
    return token->sloc;
}

void xec_constructor_list::set_token( xec_token* token )
{
    this->token = token;
}

void xec_constructor_list::append_value( xec_expression* value )
{
    values.push_back( std::unique_ptr< xec_expression >( value ) );
}

void xec_constructor_list::append_final( xec_expression* final )
{
    this->final.reset( final );
}




/*
    { expr : expr, expr : expr }
*/

xec_constructor_table::xec_constructor_table()
    :   token( NULL )
{
}

xec_expression_dispatch xec_constructor_table::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_TABLE;
}

int xec_constructor_table::get_location()
{
    return token->sloc;
}

void xec_constructor_table::set_token( xec_token* token )
{
    this->token = token;
}
    
void xec_constructor_table::append_keyval(
                xec_expression* key, xec_expression* value )
{
    keyval kv;
    kv.key.reset( key );
    kv.value.reset( value );
    keyvals.push_back( std::move( kv ) );
}



/*
    :{ decl; decl; }
    : expr { decl; decl; }
*/

xec_constructor_object::xec_constructor_object()
{
}

xec_expression_dispatch xec_constructor_object::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_OBJECT;
}

int xec_constructor_object::get_location()
{
    return token->sloc;
}

void xec_constructor_object::set_token( xec_token* token )
{
    this->token = token;
}

void xec_constructor_object::set_proto( xec_expression* proto )
{
    this->proto.reset( proto );
}

void xec_constructor_object::add_member( xec_declaration* decl )
{
    members.push_back( std::unique_ptr< xec_declaration >( decl ) );
}




/*
    ?( param, param ) { stmt; stmt; }
    ?( param, param ) yield { stmt; stmt; }
    .?( param, param ) { stmt; stmt; }
    .?( param, param ) yield { stmt; stmt; }
*/

xec_constructor_function::xec_constructor_function( xec_token* token,
                xec_expression_list* params, xec_statement_compound* body )
    :   token( token )
    ,   params( params )
    ,   body( body )
    ,   thiscall( false )
    ,   coroutine( false )
{
}

xec_expression_dispatch xec_constructor_function::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_FUNCTION;
}

int xec_constructor_function::get_location()
{
    return token->sloc;
}

void xec_constructor_function::set_thiscall( bool thiscall )
{
    this->thiscall = thiscall;
}

void xec_constructor_function::set_coroutine( bool coroutine )
{
    this->coroutine = coroutine;
}




