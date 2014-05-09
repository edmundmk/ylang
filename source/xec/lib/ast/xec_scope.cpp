//
//  xec_scope.cpp
//
//  Created by Edmund Kapusniak on 09/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_scope.h"



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


xec_scope_kind xec_scope::get_kind()
{
    return kind;
}

xec_scope* xec_scope::get_outer()
{
    return outer_scope;
}