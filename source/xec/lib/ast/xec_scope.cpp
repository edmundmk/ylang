//
//  xec_scope.cpp
//
//  Created by Edmund Kapusniak on 09/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_scope.h"
#include <assert.h>
#include <unordered_set>



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
    assert( this->kind != XEC_SCOPE_IMPLIED );
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



void xec_scope::print()
{
    print( 0 );
}

void xec_scope::print( int indent )
{
    const char* kind_name = "??";
    switch ( get_kind() )
    {
    case XEC_SCOPE_GLOBAL:      kind_name = "global";       break;
    case XEC_SCOPE_IMPLIED:     kind_name = "implied";      break;
    case XEC_SCOPE_FUNCTION:    kind_name = "function";     break;
    case XEC_SCOPE_OBJECT:      kind_name = "object";       break;
    case XEC_SCOPE_BLOCK:       kind_name = "block";        break;
    case XEC_SCOPE_LOOP:        kind_name = "loop";         break;
    case XEC_SCOPE_SWITCH:      kind_name = "switch";       break;
    }
    
    printf( "%s [%p]\n%*s{\n", kind_name, get_function(), indent, "" );
    
    std::unordered_set< xec_scope* > named_scopes;
    for ( auto i = scopes.begin(); i != scopes.end(); ++i )
    {
        named_scopes.insert( i->second );
        printf( "%*s%s : ", indent + 4, "", i->first.c_str() );
        i->second->print( indent + 4 );
    }
    
    for ( auto i = children.begin(); i != children.end(); ++i )
    {
        xec_scope* scope = i->get();
        if ( named_scopes.find( scope ) != named_scopes.end() )
            continue;
        
        printf( "%*s", indent + 4, "" );
        scope->print( indent + 4 );
    }
    
    for ( auto i = names.begin(); i != names.end(); ++i )
    {
        xec_name* name = i->second.get();
        
        const char* kind_name = "??";
        switch ( name->get_kind() )
        {
        case XEC_NAME_GLOBAL:       kind_name = "global";       break;
        case XEC_NAME_THIS:         kind_name = "this";         break;
        case XEC_NAME_BASE:         kind_name = "base";         break;
        case XEC_NAME_VARIABLE:     kind_name = "variable";     break;
        case XEC_NAME_PARAMETER:    kind_name = "parameter";    break;
        case XEC_NAME_PROTOTYPE:    kind_name = "prototype";    break;
        }
        
        printf( "%*s%s %s", indent + 4, "", kind_name, name->get_name() );
        if ( name->get_prototype() )
            printf( " [%p]", name->get_prototype() );
        printf( "\n" );
    }
    
    printf( "%*s}\n", indent, "" );
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

