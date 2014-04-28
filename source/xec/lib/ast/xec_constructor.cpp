//
//  xec_constructor.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_constructor.h"
#include "xec_statement.h"
#include "xec_token.h"




xec_constructor_new::xec_constructor_new(
        xec_token* token, xec_expression* proto, xec_expression_list* args )
    :   token( token )
    ,   proto( proto )
    ,   args( args )
{
}

int xec_constructor_new::get_location()
{
    return token->sloc;
}



xec_constructor_list::xec_constructor_list()
    :   token( NULL )
{
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
    this->final = std::unique_ptr< xec_expression >( final );
}






xec_constructor_table::xec_constructor_table()
    :   token( NULL )
{
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
    kv.key   = std::unique_ptr< xec_expression >( key );
    kv.value = std::unique_ptr< xec_expression >( value );
    keyvals.push_back( std::move( kv ) );
}




//xec_constructor_object



xec_constructor_function::xec_constructor_function( xec_token* token,
                xec_expression_list* params, xec_statement_compound* body )
    :   token( token )
    ,   params( params )
    ,   body( body )
    ,   thiscall( false )
    ,   coroutine( false )
{
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




