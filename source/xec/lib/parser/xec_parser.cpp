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
    char* endp = NULL;
    double number = strtod( token->text, &endp );
    if ( endp[ 0 ] != '\0' )
    {
        script->diagnostic(
            token->sloc, "unable to parse number '%s'", token->text );
    }
    else if ( number == HUGE_VAL || number == -HUGE_VAL )
    {
        script->diagnostic(
            token->sloc, "number out of range '%s'", token->text );
    }
    return number;
}




xec_ast_scope* xec_parser::get_scope()
{
    return scopes.back();
}


xec_ast_scope* xec_parser::block_scope( xec_ast_node* node )
{
    xec_ast_scope* outer = get_scope();
    xec_ast_scope* scope = alloc< xec_ast_scope >(
                    XEC_SCOPE_BLOCK, outer, node, outer->func );
    scopes.push_back( scope );
    return scope;
}


void xec_parser::close_scope( xec_ast_scope* scope )
{
    assert( scope == get_scope() );
    scopes.pop_back();
}






xec_ast_name* xec_parser::declare( xec_unqual_name* name )
{
    xec_ast_scope* scope = get_scope();
    
    // Error when you declare the same variable again.
    auto i = scope->names.find( name->name );
    if ( i != scope->names.end() )
    {
        script->diagnostic(
                name->sloc, "'%s' is already declared", name->name );
    }
    
    // Declare in scope.
    xec_ast_name* n = alloc< xec_ast_name >( name->sloc, scope, name->name );
    scope->names.emplace( name->name, n );
    scope->decls.push_back( n );
    
    // Return declared name.
    return n;
}


xec_expr_local* xec_parser::declare_local( xec_unqual_name* name )
{
    xec_ast_name* n = declare( name );
    return alloc< xec_expr_local >( n->sloc, n );
}



void xec_parser::declare_local_list(
                xec_unqual_list* list, xec_ast_node_list* lvalues )
{
    for ( size_t i = 0; i < list->names.size(); ++i )
    {
        lvalues->push_back( declare_local( list->names[ i ] ) );
    }
}





xec_new_object* xec_parser::object(
                int sloc, xec_ast_node* name, xec_ast_node* proto )
{
    // Create object.
    xec_ast_scope* outer = get_scope();
    xec_new_object* object = alloc< xec_new_object >( sloc, proto );
    object->scope = alloc< xec_ast_scope >(
                    XEC_SCOPE_OBJECT, outer, object, outer->func );
    object->scope->node = object;


    // Declare.


    // Imply scope.
    
    
    
    // Enter object scope and return the object.
    scopes.push_back( object->scope );
    return object;
}


void xec_parser::prototype( int sloc, xec_unqual_proto* proto )
{
    // Prototypes must sit directly in an object scope.
    if ( proto->name->kind != XEC_UNQUAL_NAME )
    {
        script->diagnostic(
                proto->sloc, "prototypes cannot be declared out-of-line" );
        return;
    }
    
    
    // Declare prototype.
    xec_ast_name* n = declare( (xec_unqual_name*)proto->name );
    n->prototype = alloc< xec_ast_prototype >( proto->sloc );

    for ( size_t i = 0; i < proto->params->values.size(); ++i )
    {
        xec_ast_node* param = proto->params->values[ i ];
        if ( param->kind == XEC_UNQUAL_NAME )
            n->prototype->parameters.push_back( (xec_unqual_name*)param );
        else
            script->diagnostic( param->sloc, "invalid parameter declaration" );
    }

    if ( proto->params->final )
    {
        xec_ast_node* param = proto->params->final;
        if ( param->kind == XEC_EXPR_VARARG )
            n->prototype->varargs = true;
        else
            script->diagnostic( param->sloc, "invalid parameter declaration" );
    }

    n->prototype->coroutine = proto->coroutine;
}


xec_ast_func* xec_parser::function( int sloc, xec_ast_node* name,
                xec_ast_node* params, bool coroutine, bool thisdot )
{
    return NULL;
}



void xec_parser::var( int sloc, xec_unqual_name* lval, xec_ast_node* rval )
{
}


void xec_parser::var_list(
                int sloc, xec_unqual_list* lvals, xec_ast_node* rvals )
{
}




void xec_parser::statement( xec_ast_node* stmt )
{
    if ( ! stmt )
        return;

    xec_ast_scope* scope = get_scope();
    assert( scope->block );
    assert( scope->kind == XEC_SCOPE_SCRIPT || scope->kind == XEC_SCOPE_BLOCK );
    scope->block->stmts.push_back( stmt );
}


xec_ast_node* xec_parser::continue_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope->node;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    script->diagnostic( sloc, "continue outside loop" );
    return NULL;
}


xec_ast_node* xec_parser::break_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_SWITCH
                || scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope->node;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    script->diagnostic( sloc, "break outside loop or switch" );
    return NULL;
}





xec_ast_node* xec_parser::resolve( xec_ast_node* name )
{
    // The name/proto grammar might end up being a declaration name or
    // a prototype (with parameter declarations), in which case it must
    // be left alone.  Or, it may be used in an expression, in which case
    // names must be resolved with lookup (for both lvalues and rvalues).
    
    if ( name->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)name;

        // Resolve all values in list.  Final value should already be resolved.
        for ( size_t i = 0; i < l->values.size(); ++i )
            l->values[ i ] = resolve( l->values[ i ] );

        return l;
    }
    
    if ( name->kind == XEC_UNQUAL_QUAL )
    {
        xec_unqual_qual* qual = (xec_unqual_qual*)name;
        
        // Resolve scope of qualified name.
        xec_ast_node* scope = resolve( qual->scope );
        
        // Check for special construction 'global.'.
        if ( scope->kind == XEC_EXPR_GLOBAL )
        {
            xec_expr_global* global = (xec_expr_global*)scope;
            if ( ! global->gexplicit && strcmp( global->name, "global" ) == 0 )
            {
                return alloc< xec_expr_global >(
                                global->sloc, qual->name, true );
            }
        }
        
        // Otherwise create key expr.
        return alloc< xec_expr_key >( scope->sloc, scope, qual->name );
    }
    
    
    if ( name->kind == XEC_UNQUAL_NAME )
    {
        xec_unqual_name* unqual = (xec_unqual_name*)name;
        xec_ast_scope* local = get_scope();
    
        // Perform lookup.
        xec_ast_name* name = NULL;
        for ( xec_ast_scope* scope = local; scope != NULL; scope = scope->outer )
        {
            auto i = scope->names.find( unqual->name );
            if ( i != scope->names.end() )
            {
                name = i->second;
            }
        }
        
        if ( ! name )
        {
            // Not found, it's a global.
            return alloc< xec_expr_global >( unqual->sloc, unqual->name, false );
        }
        
        if ( name->scope->kind != XEC_SCOPE_OBJECT )
        {
            // If the name is 'super' then we actually reference 'this'.
            xec_ast_name* lookname = name->superthis ? name->superthis : name;
            
            // It's a variable, either we're referencing an upval or a local.
            xec_ast_node* nameref = NULL;
            if ( name->scope->func == local->func )
            {
                // It's a local variable.
                nameref = alloc< xec_expr_local >( unqual->sloc, lookname );
            }
            else
            {
                // It's an upval.
                nameref = alloc< xec_expr_upref >(
                                unqual->sloc, upval( local->func, lookname ) );
            }
            
            // If the name is 'super' then transform to 'this.super'.
            if ( name->superthis )
            {
                nameref = alloc< xec_expr_key >(
                                unqual->sloc, nameref, name->name );
            }
            
            return nameref;
        }
        else
        {
            // It's an object member.
            assert( name->scope->node->kind == XEC_NEW_OBJECT );
            xec_new_object* object = (xec_new_object*)name->scope->node;
            
            if ( name->scope->func == local->func )
            {
                // In this function.
                xec_expr_objref* objref = alloc< xec_expr_objref >(
                                unqual->sloc, object );
                return alloc< xec_expr_key >(
                                unqual->sloc, objref, unqual->name );
            }
            else if ( object == local->func->memberof )
            {
                // It's a member of the object we're declared in.
                assert( local->func->thisname );
                xec_expr_local* thisref = alloc< xec_expr_local >(
                                unqual->sloc, local->func->thisname );
                return alloc< xec_expr_key >(
                                unqual->sloc, thisref, unqual->name );
            }
            else
            {
                // Object is an upval.
                xec_expr_upref* upref = alloc< xec_expr_upref >(
                                unqual->sloc, upval( local->func, object ) );
                return alloc< xec_expr_key >(
                                unqual->sloc, upref, unqual->name );
            }
        
        }
    }


    // Already resolved.
    return name;
}


xec_expr_call* xec_parser::resolve( xec_unqual_proto* proto )
{
    // Replace a proto used as an expression with a true call expression.
    assert( ! proto->coroutine );
    xec_ast_node* expr = resolve( proto->name );
    xec_expr_list* args = list( resolve( proto->params ) );
    return alloc< xec_expr_call >( expr->sloc, expr, args );
}



int xec_parser::upval( xec_ast_func* func, xec_ast_upval uv )
{
    // Find the upval in the outer function.
    assert( func->scope->outer );
    xec_ast_func* outer = func->scope->outer->func;
    switch ( uv.kind )
    {
    case XEC_UPVAL_LOCAL:
        if ( uv.local->scope->func != outer )
        {
            // Import upval into outer function.
            uv = upval( outer, uv );
        }
        else
        {
            // This local is referenced as an upval.
            uv.local->upval = true;
        }
        break;
    
    case XEC_UPVAL_OBJECT:
        if ( uv.object->scope->func != outer )
        {
            // Import upval into outer function.
            uv = upval( outer, uv );
        }
        else
        {
            // This object is referenced as an upval.
            uv.object->upval = true;
        }
        break;
    
    case XEC_UPVAL_UPVAL:
        assert( ! "cannot resolve already resolved upval" );
        break;
    }
    

    // Does the function contain the upval already?
    for ( int i = 0; i != (int)func->upvals.size(); ++i )
    {
        if ( func->upvals.at( i ) == uv )
            return i;
    }
    
    
    // Append it.
    int i = (int)func->upvals.size();
    func->upvals.push_back( uv );
    return i;
}



xec_ast_node* xec_parser::compare(
        xec_token* token, xec_ast_node* lhs, xec_ast_node* rhs )
{
    // Build a compare expression.
    xec_expr_compare* c;
    if ( lhs->kind == XEC_EXPR_COMPARE )
        c = (xec_expr_compare*)lhs;
    else
        c = alloc< xec_expr_compare >( lhs->sloc, lhs );
    
    c->opkinds.push_back( token->kind );
    c->terms.push_back( rhs );

    return c;
}



xec_ast_node* xec_parser::append( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->values.push_back( expr );
    return l;
}


xec_ast_node* xec_parser::final( xec_ast_node* list, xec_ast_node* expr )
{
    xec_expr_list* l;
    if ( list->kind != XEC_EXPR_LIST )
        l = alloc< xec_expr_list >( list->sloc );
    else
        l = (xec_expr_list*)list;
    l->final = expr;
    return l;
}


xec_expr_list* xec_parser::list( xec_ast_node* list )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        return (xec_expr_list*)list;
    }
    else
    {
        xec_expr_list* l = alloc< xec_expr_list >( list->sloc );
        l->values.push_back( list );
        return l;
    }
}




xec_ast_node* xec_parser::lvalue( xec_ast_node* lvalue )
{
    // The expression must be a single lvalue.  Only a restricted set of
    // expressions are lvalues, and global variables must be scoped using
    // 'global.'
    
    switch ( lvalue->kind )
    {
    case XEC_EXPR_LOCAL:
    case XEC_EXPR_UPREF:
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
    case XEC_EXPR_INDEX:
    {
        break;
    }
    
    case XEC_EXPR_GLOBAL:
    {
        xec_expr_global* g = (xec_expr_global*)lvalue;
        if ( ! g->gexplicit )
        {
            script->diagnostic( g->sloc,
                            "undeclared identifier '%s'", g->name );
        }
        break;
    }
    
    default:
    {
        script->diagnostic( lvalue->sloc, "invalid lvalue" );
        break;
    }
    }

    return lvalue;
}


void xec_parser::lvalue_list(
        xec_ast_node* list, xec_ast_node_list* lvalues )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)list;
        for ( size_t i = 0; i < l->values.size(); ++l )
            lvalues->push_back( lvalue( l->values[ i ] ) );
        if ( l->final )
            lvalues->push_back( lvalue( l->final ) );
    }
    else
    {
        lvalues->push_back( lvalue( list ) );
    }
}



xec_ast_node* xec_parser::delval( xec_ast_node* delval )
{
    // Expression must be a single deletable lvalue.
    
    switch ( delval->kind )
    {
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
    {
        break;
    }
    
    default:
    {
        script->diagnostic( delval->sloc, "expression is not deletable" );
        break;
    }
    }
    
    return delval;
}


void xec_parser::delval_list(
        xec_ast_node* list, xec_ast_node_list* delvals )
{
    if ( list->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)list;
        for ( size_t i = 0; i < l->values.size(); ++l )
            delvals->push_back( delval( l->values[ i ] ) );
        if ( l->final )
            delvals->push_back( delval( l->final ) );
    }
    else
    {
        delvals->push_back( delval( list ) );
    }
}



xec_ast_node* xec_parser::assign(
        xec_token* op, xec_ast_node* lv, xec_ast_node* rv )
{
    if ( lv->kind == XEC_EXPR_LIST )
    {
        xec_expr_assign_list* a;
        a = alloc< xec_expr_assign_list >( lv->sloc, op->kind );
        lvalue_list( (xec_expr_list*)lv, &a->lvalues );
        a->rvalues = rv;
        return a;
    }
    else
    {
        xec_expr_assign* a;
        a = alloc< xec_expr_assign >( lv->sloc, op->kind );
        a->lvalue = lvalue( lv );
        a->rvalue = rv;
        return a;
    }
}






