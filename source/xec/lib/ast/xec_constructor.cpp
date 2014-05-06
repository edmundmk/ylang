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

xec_expression* xec_constructor_new::get_proto()
{
    return proto.get();
}

xec_expression_list* xec_constructor_new::get_arguments()
{
    return args.get();
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

size_t xec_constructor_list::get_count()
{
    return values.size();
}

xec_expression* xec_constructor_list::get_expr( size_t index )
{
    return values.at( index ).get();
}

xec_expression* xec_constructor_list::get_final()
{
    return final.get();
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

size_t xec_constructor_table::get_count()
{
    return keyvals.size();
}

xec_expression* xec_constructor_table::get_key( size_t index )
{
    return keyvals.at( index ).key.get();
}

xec_expression* xec_constructor_table::get_value( size_t index )
{
    return keyvals.at( index ).value.get();
}


/*
    :{ decl; decl; }
    : expr { decl; decl; }
*/

xec_constructor_object::xec_constructor_object()
    :   token( NULL )
{
}

xec_expression_dispatch xec_constructor_object::visitor_dispatch()
{
    return XEC_CONSTRUCTOR_OBJECT;
}

int xec_constructor_object::get_location()
{
    if ( token )
        return token->sloc;
    else
        return -1;
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

xec_expression* xec_constructor_object::get_proto()
{
    return proto.get();
}

size_t xec_constructor_object::get_member_count()
{
    return members.size();
}

xec_declaration* xec_constructor_object::get_member( size_t index )
{
    return members.at( index ).get();
}




/*
    ?( param, param ) { stmt; stmt; }
    ?( param, param ) yield { stmt; stmt; }
    .?( param, param ) { stmt; stmt; }
    .?( param, param ) yield { stmt; stmt; }
*/

xec_constructor_function::xec_constructor_function(
                                xec_expression_list* params )
    :   token( NULL )
    ,   params( params )
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
    if ( token )
        return token->sloc;
    else
        return -1;
}

void xec_constructor_function::set_token( xec_token* token )
{
    this->token = token;
}

void xec_constructor_function::set_thiscall( bool thiscall )
{
    this->thiscall = thiscall;
}

void xec_constructor_function::set_coroutine( bool coroutine )
{
    this->coroutine = coroutine;
}

void xec_constructor_function::set_body( xec_statement_compound* body )
{
    this->body.reset( body );
}

bool xec_constructor_function::get_thiscall()
{
    return thiscall;
}

bool xec_constructor_function::get_coroutine()
{
    return coroutine;
}

xec_expression_list* xec_constructor_function::get_parameters()
{
    return params.get();
}

xec_statement_compound* xec_constructor_function::get_body()
{
    return body.get();
}





