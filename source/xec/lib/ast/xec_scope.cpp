//
//  xec_scope.cpp
//
//  Created by Edmund Kapusniak on 09/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_scope.h"
#include <assert.h>



xec_scope::xec_scope()
    :   kind( XEC_SCOPE_GLOBAL )
    ,   outer_scope( NULL )
{
}

xec_scope* xec_scope::make_child_scope( xec_scope_kind kind )
{
    xec_scope* scope = new xec_scope();
    scope->kind         = kind;
    scope->outer_scope  = this;
    children.emplace_back( scope );
    return scope;
}


void xec_scope::set_function( xec_constructor_function* function )
{
    this->function = function;
}


xec_scope_kind xec_scope::get_kind()
{
    return kind;
}

xec_scope* xec_scope::get_outer()
{
    return outer_scope;
}

xec_constructor_function* xec_scope::get_function()
{
    return function;
}


xec_scope* xec_scope::declare_scope( xec_scope_kind kind, const char* name )
{
    assert( scopes.find( name ) == scopes.end() );
    xec_scope* declare = make_child_scope( kind );
    scopes.emplace( name, declare );
    return declare;
}

xec_scope* xec_scope::lookup_scope( const char* name )
{
    auto i = scopes.find( name );
    if ( i != scopes.end() )
        return i->second;
    else
        return NULL;
}


xec_name* xec_scope::declare_name( xec_name_kind kind, const char* name )
{
    assert( names.find( name ) == names.end() );
    xec_name* declare = new xec_name( this, kind, name );
    names.emplace( name, std::unique_ptr< xec_name >( declare ) );
    return declare;
}

xec_name* xec_scope::lookup_name( const char* name )
{
    auto i = names.find( name );
    if ( i != names.end() )
        return i->second.get();
    else
        return NULL;
}







xec_name::xec_name( xec_scope* scope, xec_name_kind kind, const char* name )
    :   scope( scope )
    ,   kind( kind )
    ,   name( name )
    ,   prototype( NULL )
{
}

void xec_name::set_prototype( xec_declaration_prototype* prototype )
{
    this->prototype = prototype;
}

xec_scope* xec_name::get_scope()
{
    return scope;
}

xec_name_kind xec_name::get_kind()
{
    return kind;
}

const char* xec_name::get_name()
{
    return name;
}

xec_declaration_prototype* xec_name::get_prototype()
{
    return prototype;
}

