//
//  xec_parser.cpp
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_parser.h"
#include "xec_token.h"





bool xec_parse( xec_script* script, xec_ast* ast,
            const char* filename, size_t argc, const char* const* argv )
{
    region_scope rscope( ast->alloc );

    // Associate AST with script.
    assert( ast->script == NULL );
    ast->script = script;

    // Set up root function and scope.
    ast->function = new ( ast->alloc ) xec_ast_func( -1 );
    ast->functions.push_back( ast->function );
    ast->function->scope = new ( ast->alloc ) xec_ast_scope(
                    XEC_SCOPE_SCRIPT, NULL, ast->function, ast->function );
    ast->function->block = new ( ast->alloc ) xec_stmt_block( -1 );
    ast->function->scope->block = ast->function->block;
    
    
    // Preserve function name.
    char* funcname = (char*)script->alloc.malloc( strlen( filename ) + 1 );
    strcpy( funcname, filename );
    ast->function->funcname = funcname;
    
    
    // Declare parameters.
    for ( size_t i = 0; i < argc; ++i )
    {
        if ( strcmp( argv[ i ], "..." ) != 0 )
        {
            char* param = (char*)script->alloc.malloc( strlen( argv[ i ] ) + 1 );
            strcpy( param, argv[ i ] );
        
            // Check for duplicated parameters.
            if ( ast->function->scope->names.find( param )
                            != ast->function->scope->names.end() )
            {
                assert( ! "duplicate parameter" );
            }
            
            // Add name.
            xec_ast_name* name = new ( ast->alloc )
                            xec_ast_name( -1, ast->function->scope, param );
            ast->function->scope->names.emplace( param, name );
            ast->function->parameters.push_back( name );
        }
        else
        {
            // Varargs indicator.
            assert( i == argc - 1 && "varargs must be at the end" );
            ast->function->varargs = true;
        }
    }

    
    
    // Parse.
    xec_parser parser( ast );
    return parser.parse( filename );
}








xec_parser::xec_parser( xec_ast* root )
    :   root( root )
{
}


xec_parser::~xec_parser()
{
    for ( auto i = recycle_tokens.begin(); i != recycle_tokens.end();
                    i = recycle_tokens.erase( i ) )
        free( *i );
}



xec_ast* xec_parser::get_root()
{
    return root;
}




double xec_parser::parse_number( xec_token* token )
{
    char* endp = NULL;
    double number = strtod( token->text, &endp );
    if ( endp[ 0 ] != '\0' )
    {
        root->script->error(
            token->sloc, "unable to parse number '%s'", token->text );
    }
    else if ( number == HUGE_VAL || number == -HUGE_VAL )
    {
        root->script->error(
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

void xec_parser::close_switch( xec_ast_scope* scope )
{
    if ( scope->kind == XEC_SCOPE_IMPLICIT )
    {
        // Fallen through to the end of the switch.
        root->script->error( scope->node->sloc, "missing break" );
        close_scope( scope );
        scope = get_scope();
    }
    
    close_scope( scope );
}









xec_new_object* xec_parser::object(
                int sloc, xec_ast_node* name, xec_ast_node* proto )
{
    // Create object.
    xec_ast_scope* outer = get_scope();
    xec_new_object* object = alloc< xec_new_object >( sloc, proto );
    object->scope = alloc< xec_ast_scope >(
                    XEC_SCOPE_OBJECT, outer, object, outer->func );

    // Declare or assign.
    declname( sloc, name, object );

    // Imply scope.
    if ( name )
    {
        xec_ast_scope* scope = NULL;
        const char* s = NULL;
        
        if ( name->kind == XEC_NAME_NAME )
        {
            xec_name_name* qual = (xec_name_name*)name;
            scope = outer;
            s = qual->name;
        }
        else if ( name->kind == XEC_NAME_QUAL )
        {
            xec_name_qual* qual = (xec_name_qual*)name;
            scope = imply( outer, qual->scope, false );
            s = qual->name;
        }
        else
        {
            assert( ! "invalid declaration name" );
        }

        scope->implied.erase( s );
        scope->implied.emplace( s, object->scope );
    }
    
    
    // Enter object scope and return the object.
    scopes.push_back( object->scope );
    return object;
}


void xec_parser::prototype( int sloc,
                xec_ast_node* name, xec_name_list* params, bool yield )
{
    // Prototypes must sit directly in an object scope.
    if ( name->kind != XEC_NAME_NAME )
    {
        root->script->error( sloc, "invalid prototype name" );
        return;
    }
    
    
    // Declare prototype.
    xec_name_name* m = (xec_name_name*)name;
    xec_ast_name* n = declare( m->sloc, m->name );
    n->prototype = alloc< xec_ast_prototype >( sloc );
    n->prototype->parameters    = params;
    n->prototype->coroutine     = yield;
}


xec_ast_func* xec_parser::function( int sloc, xec_ast_node* name,
                xec_name_list* params, bool yield, bool dot )
{
    // Attempt to match with prototype.
    xec_ast_scope* outer = get_scope();
    outer = lookup_prototype( sloc, outer, name, params, yield );


    // Create function.
    xec_ast_func* func = alloc< xec_ast_func >( sloc );
    root->functions.push_back( func );
    func->scope = alloc< xec_ast_scope >( XEC_SCOPE_BLOCK, outer, func, func );
    
    if ( outer->kind == XEC_SCOPE_OBJECT )
    {
        // Function is a member of an object (so has implicit-this).
        assert( outer->node->kind == XEC_NEW_OBJECT );
        func->memberof = (xec_new_object*)outer->node;
    }
    
    
    // Create function block.
    func->block = alloc< xec_stmt_block >( sloc );
    func->scope->block = func->block;
    
    
    // Enter function scope to declare parameters.
    scopes.push_back( func->scope );

    if ( outer->kind == XEC_SCOPE_OBJECT || dot )
    {
        // Function has automatic this parameter (and super).
        func->thisname = declare( sloc, "this" );
        func->parameters.push_back( func->thisname );
        
        xec_ast_name* supername = declare( sloc, "super" );
        supername->superthis = func->thisname;
    }
    
    if ( params )
    {
        for ( size_t i = 0; i < params->names.size(); ++i )
        {
            xec_name_name* m = params->names[ i ];
            xec_ast_name* paramname = declare( m->sloc, m->name );
            func->parameters.push_back( paramname );
        }
        
        func->varargs = params->varargs;
    }

    func->coroutine = yield;
    
    scopes.pop_back();
    
    
    // Declare or assign.
    declname( sloc, name, func );


    // Get function name.
    if ( name )
    {
        std::string funcname;
        xec_ast_node* n = name;
        while ( name )
        {
            if ( funcname.size() )
            {
                funcname = "." + funcname;
            }
            if ( n->kind == XEC_NAME_NAME )
            {
                xec_name_name* nn = (xec_name_name*)n;
                funcname = nn->name + funcname;
                break;
            }
            else if ( n->kind == XEC_NAME_QUAL )
            {
                xec_name_qual* qq = (xec_name_qual*)n;
                funcname = qq->name + funcname;
                n = qq->scope;
            }
            else
            {
                assert( ! "invalid function name" );
                break;
            }
        }
        
        // Allocate function name from script's region, it can outlive the AST.
        char* s = (char*)root->script->alloc.malloc( funcname.size() + 1 );
        strcpy( s, funcname.c_str() );
        func->funcname = s;
    }
    else
    {
        if ( dot )
            func->funcname = ".?";
        else
            func->funcname = "?";
    }
    
    
    // Enter function scope and return the function.
    scopes.push_back( func->scope );
    return func;
}




void xec_parser::var( int sloc, xec_ast_node* names, xec_ast_node* rvals )
{
    xec_ast_scope* scope = get_scope();
    
    if ( scope->kind == XEC_SCOPE_OBJECT )
    {
        // Declare object member.
        assert( scope->node->kind == XEC_NEW_OBJECT );
        xec_new_object* object = (xec_new_object*)scope->node;

        // Declare.
        if ( names->kind == XEC_NAME_NAME )
        {
            xec_name_name* n = (xec_name_name*)names;
            declare( n->sloc, n->name );

            if ( ! rvals )
                return;
            
            xec_expr_assign* assign = alloc< xec_expr_assign >(
                            sloc, XEC_ASTOP_DECLARE );
            xec_expr_objref* objref = alloc< xec_expr_objref >( sloc, object );
            assign->lvalue = alloc< xec_expr_key> ( sloc, objref, n->name );
            assign->rvalue = rvals;
            object->members.push_back( assign );
        }
        else if ( names->kind == XEC_NAME_LIST )
        {
            xec_name_list* l = (xec_name_list*)names;
            for ( size_t i = 0; i < l->names.size(); ++i )
            {
                xec_name_name* n = l->names[ i ];
                declare( n->sloc, n->name );
            }

            if ( ! rvals )
                return;
         
            xec_expr_assign_list* assign = alloc< xec_expr_assign_list >(
                            sloc, XEC_ASTOP_DECLARE );
            for ( size_t i = 0; i < l->names.size(); ++i )
            {
                xec_name_name* n = l->names[ i ];
                xec_expr_objref* of = alloc< xec_expr_objref >( sloc, object );
                xec_expr_key* key = alloc< xec_expr_key >( sloc, of, n->name );
                assign->lvalues.push_back( key );
            }
            assign->rvalues = rvals;
            object->members.push_back( assign );
        }

    }
    else
    {
        // Var in a block.
        xec_ast_node* assign = varstmt( sloc, names, rvals );
        scope->block->stmts.push_back( assign );
    }
}



void xec_parser::statement( xec_ast_node* stmt )
{
    if ( ! stmt )
        return;

    xec_ast_scope* scope = get_scope();
    assert( scope->kind != XEC_SCOPE_OBJECT );
    assert( scope->kind != XEC_SCOPE_IMPLIED );
    
    /*
        switch statements are made up of case statements and implicit blocks.
        Each implicit block starts from a non-case statement and ends at a
        break.  We do it this way to allow declarations inside switch
        statements without risking skipping declarations.
    */
    
    if ( scope->kind == XEC_SCOPE_SWITCH )
    {
        /*
            In the switch scope, which means the switch has just been opened, or
            an implicit block has just been closed with a break.
        */
        
        if ( stmt->kind == XEC_STMT_CASE )
        {
            // Add case to switch block.
            scope->block->stmts.push_back( stmt );
        }
        else
        {
            // It's not a case - make sure there's been at least one case.
            assert( scope->node->kind == XEC_STMT_SWITCH );
            xec_stmt_switch* swstmt = (xec_stmt_switch*)scope->node;
            if ( swstmt->body->stmts.empty() )
            {
                root->script->error( stmt->sloc, "expected case" );
            }

            // Open implicit block.
            xec_stmt_block* block = alloc< xec_stmt_block >( stmt->sloc );
            block->scope = alloc< xec_ast_scope >(
                    XEC_SCOPE_IMPLICIT, scope, block, scope->func );
            scopes.push_back( block->scope );
            scope = block->scope;
            
            // Add statement to block.
            scope->block->stmts.push_back( stmt );
        }
    }
    else if ( scope->kind == XEC_SCOPE_IMPLICIT )
    {
        /*
            In an implicit block inside a switch statement.
        */
    
        if ( stmt->kind == XEC_STMT_CASE )
        {
            // Close the implicit block (with no break, it will fall-through).
            close_scope( scope );
            scope = get_scope();

            // Add the case to the switch scope.
            assert( scope->kind == XEC_SCOPE_SWITCH );
            scope->block->stmts.push_back( stmt );
        }
        else if ( stmt->kind == XEC_STMT_BREAK )
        {
            // Add the break to the implicit block.
            assert( ( (xec_stmt_break*)stmt )->target == scope );
            scope->block->stmts.push_back( stmt );
            
            // Close the block.
            close_scope( scope );
        }
        else
        {
            // Add statement to implicit block.
            scope->block->stmts.push_back( stmt );
        }
    }
    else
    {
        // Not in a switch statement.
        if ( stmt->kind == XEC_STMT_CASE )
        {
            root->script->error( stmt->sloc, "case outside switch" );
        }
        scope->block->stmts.push_back( stmt );
    }
}


xec_ast_scope* xec_parser::continue_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    root->script->error( sloc, "continue outside loop" );
    return NULL;
}


xec_ast_scope* xec_parser::break_target( int sloc )
{
    for ( xec_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == XEC_STMT_SWITCH
                || scope->node->kind == XEC_STMT_WHILE
                || scope->node->kind == XEC_STMT_DO
                || scope->node->kind == XEC_STMT_FOREACH
                || scope->node->kind == XEC_STMT_FOR )
            return scope;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    root->script->error( sloc, "break outside loop or switch" );
    return NULL;
}




xec_ast_node* xec_parser::lookup( int sloc, const char* identifier, bool outer )
{
    xec_ast_scope* local = get_scope();
    
    // Perform lookup.
    xec_ast_name* name = NULL;
    for ( xec_ast_scope* scope = local; scope != NULL; scope = scope->outer )
    {
        auto i = scope->names.find( identifier );
        if ( i != scope->names.end() )
        {
            name = i->second;
            break;
        }
    }
    
    if ( ! name )
    {
        // Not found, it's a global.
        return alloc< xec_expr_global >( sloc, identifier, false );
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
            nameref = alloc< xec_expr_local >(
                            sloc, lookname );
        }
        else
        {
            // It's an upval.
            nameref = alloc< xec_expr_upref >(
                            sloc, upval( local->func, lookname ) );
        }
        
        // If the name is 'super' then transform to 'this.super'.
        if ( name->superthis )
        {
            nameref = alloc< xec_expr_key >(
                            sloc, nameref, name->name );
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
            xec_expr_objref* objref = alloc< xec_expr_objref >( sloc, object );
            return alloc< xec_expr_key >( sloc, objref, identifier );
        }
        else if ( object == local->func->memberof )
        {
            // It's a member of the object we're declared in.
            assert( local->func->thisname );
            xec_expr_local* thisref = alloc< xec_expr_local >(
                            sloc, local->func->thisname );
            return alloc< xec_expr_key >( sloc, thisref, identifier );
        }
        else
        {
            // Object is an upval.
            xec_expr_upref* upref = alloc< xec_expr_upref >(
                            sloc, upval( local->func, object ) );
            return alloc< xec_expr_key >( sloc, upref, identifier );
        }
    
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
            root->script->error( g->sloc,
                            "undeclared identifier '%s'", g->name );
        }
        break;
    }
    
    default:
    {
        root->script->error( lvalue->sloc, "invalid lvalue" );
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
        for ( size_t i = 0; i < l->values.size(); ++i )
            lvalues->push_back( lvalue( l->values[ i ] ) );
        if ( l->final )
            lvalues->push_back( lvalue( l->final ) );
    }
    else
    {
        lvalues->push_back( lvalue( list ) );
    }
}



xec_name_list* xec_parser::name_list( xec_ast_node* list )
{
    if ( list->kind == XEC_NAME_LIST )
    {
        return (xec_name_list*)list;
    }
    else
    {
        assert( list->kind == XEC_NAME_NAME );
        xec_name_list* l = alloc< xec_name_list >( list->sloc );
        l->names.push_back( (xec_name_name*)list );
        return l;
    }
}



xec_expr_list* xec_parser::expr_list( xec_ast_node* list )
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


xec_ast_node* xec_parser::mono( xec_ast_node* expr )
{
    // Suppress multiple values and method calls.
    switch ( expr->kind )
    {
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
    case XEC_EXPR_CALL:
    case XEC_EXPR_YIELD:
    case XEC_EXPR_VARARG:
    case XEC_EXPR_UNPACK:
    case XEC_EXPR_LIST:
    case XEC_EXPR_ASSIGN_LIST:
        return alloc< xec_expr_mono >( expr->sloc, expr );
        
    default:
        return expr;
    }
}



xec_ast_node* xec_parser::unpack( xec_ast_node* expr )
{
    // Assign unpack to call expressions.
    switch ( expr->kind )
    {
    case XEC_EXPR_CALL:
    {
        xec_expr_call* call = (xec_expr_call*)expr;
        call->unpack = true;
    }
    
    case XEC_EXPR_YIELD:
    {
        xec_expr_yield* yield = (xec_expr_yield*)expr;
        yield->unpack = true;
    }
    
    default:
        root->script->error( expr->sloc, "invalid unpack expression" );
        break;
    }
    
    return expr;
}



xec_ast_node* xec_parser::compare( int sloc,
                xec_ast_opkind op, xec_ast_node* lhs, xec_ast_node* rhs )
{
    // Build a compare expression.
    xec_expr_compare* c;
    if ( lhs->kind == XEC_EXPR_COMPARE )
        c = (xec_expr_compare*)lhs;
    else
        c = alloc< xec_expr_compare >( lhs->sloc, lhs );
    
    c->opkinds.push_back( op );
    c->terms.push_back( rhs );

    return c;
}







xec_ast_node* xec_parser::assign( int sloc,
                xec_ast_opkind op, xec_ast_node* lv, xec_ast_node* rv )
{
    if ( lv->kind == XEC_EXPR_LIST )
    {
        xec_expr_assign_list* a;
        a = alloc< xec_expr_assign_list >( lv->sloc, op );
        lvalue_list( (xec_expr_list*)lv, &a->lvalues );
        a->rvalues = rv;
        return a;
    }
    else
    {
        xec_expr_assign* a;
        a = alloc< xec_expr_assign >( lv->sloc, op );
        a->lvalue = lvalue( lv );
        a->rvalue = rv;
        return a;
    }
}




xec_ast_node* xec_parser::varstmt(
                int sloc, xec_ast_node* names, xec_ast_node* rvals )
{
    // If there's no value, then initialize with null.
    if ( ! rvals )
        rvals = alloc< xec_expr_null >( sloc );
    
    // Declare.
    if ( names->kind == XEC_NAME_NAME )
    {
        xec_expr_assign* assign =
                alloc< xec_expr_assign >( sloc, XEC_ASTOP_DECLARE );
        assign->lvalue = declare( names );
        assign->rvalue = rvals;
        return assign;
    }
    else if ( names->kind == XEC_NAME_LIST )
    {
        xec_expr_assign_list* assign = alloc< xec_expr_assign_list >(
                        sloc, XEC_ASTOP_DECLARE );
        declare_list( names, &assign->lvalues );
        assign->rvalues = rvals;
        return assign;
    }
    
    return NULL;
}


xec_ast_node* xec_parser::delstmt( int sloc, xec_ast_node* expr )
{
    xec_stmt_delete* del = alloc< xec_stmt_delete >( sloc );
    if ( expr->kind == XEC_EXPR_LIST )
    {
        xec_expr_list* l = (xec_expr_list*)expr;
        for ( size_t i = 0; i < l->values.size(); ++l )
            del->delvals.push_back( delval( l->values[ i ] ) );
        if ( l->final )
            del->delvals.push_back( delval( l->final ) );
    }
    else
    {
        del->delvals.push_back( delval( expr ) );
    }
    return del;
}


xec_stmt_foreach* xec_parser::foreachstmt(
            int sloc, xec_ast_scope* scope, xec_ast_node* lvals,
                        xec_ast_node* expr, bool declare, bool eachkey )
{
    xec_stmt_foreach* foreach = alloc< xec_stmt_foreach >( sloc );
    foreach->scope = scope;
    foreach->scope->node = foreach;
    foreach->eachkey = eachkey;
    foreach->declare = declare;
    if ( foreach->declare )
        declare_list( lvals, &foreach->lvalues );
    else
        lvalue_list( lvals, &foreach->lvalues );
    foreach->list = expr;
    return foreach;
}


xec_stmt_catch* xec_parser::catchstmt(
            int sloc, xec_ast_scope* scope, xec_ast_node* lval,
                        xec_ast_node* proto, bool cdeclare )
{
    xec_stmt_catch* cstmt = alloc< xec_stmt_catch >( sloc );
    cstmt->scope = scope;
    cstmt->scope->node = cstmt;
    cstmt->declare = cdeclare;
    if ( cstmt->declare )
        cstmt->lvalue = declare( lval );
    else
        cstmt->lvalue = lvalue( lval );
    cstmt->proto = proto;
    return cstmt;
}



void xec_parser::check_vararg( int sloc )
{
    xec_ast_scope* scope = get_scope();
    if ( ! scope->func->varargs )
    {
        root->script->error( sloc, "varargs used outside vararg function" );
    }
}

void xec_parser::check_yield( int sloc )
{
    xec_ast_scope* scope = get_scope();
    if ( ! scope->func->coroutine )
    {
        root->script->error( sloc, "yield used outside coroutine" );
    }
}






xec_ast_name* xec_parser::declare( int sloc, const char* name )
{
    xec_ast_scope* scope = get_scope();
    
    // Error when you declare the same variable again.
    auto i = scope->names.find( name );
    if ( i != scope->names.end() )
    {
        root->script->error( sloc, "'%s' is already declared", name );
    }
    
    // Declare in scope.
    xec_ast_name* n = alloc< xec_ast_name >( sloc, scope, name );
    scope->names.emplace( name, n );
    scope->decls.push_back( n );
    
    // Return declared name.
    return n;
}


xec_ast_node* xec_parser::declare( xec_ast_node* name )
{
    assert( name->kind == XEC_NAME_NAME );
    xec_name_name* n = (xec_name_name*)name;
    xec_ast_name* m = declare( n->sloc, n->name );
    return alloc< xec_expr_local >( m->sloc, m );
}


void xec_parser::declare_list( xec_ast_node* names, xec_ast_node_list* lv )
{
    if ( names->kind == XEC_NAME_NAME )
    {
        lv->push_back( declare( names ) );
    }
    else if ( names->kind == XEC_NAME_LIST )
    {
        xec_name_list* l = (xec_name_list*)names;
        for ( size_t i = 0; i < l->names.size(); ++i )
        {
            lv->push_back( declare( l->names[ i ] ) );
        }
    }
    else
    {
        assert( ! "invalid declaration name" );
    }
}







void xec_parser::declname( int sloc, xec_ast_node* name, xec_ast_node* decl )
{
    if ( ! name )
    {
        return;
    }


    // Find object.
    xec_ast_scope* scope = get_scope();
    xec_new_object* object = NULL;
    if ( scope->kind == XEC_SCOPE_OBJECT )
    {
        assert( scope->node->kind == XEC_NEW_OBJECT );
        object = (xec_new_object*)scope->node;
    }


    // Resolve and create lvalue to assign to.
    xec_expr_assign* assign =
                    alloc< xec_expr_assign >( sloc, XEC_ASTOP_ASSIGN );
    
    if ( name->kind == XEC_NAME_NAME )
    {
        // Single names declare things.
        xec_name_name* n = (xec_name_name*)name;
        xec_ast_name* declname = declare( n->sloc, n->name );
        assign->assignop = XEC_ASTOP_DECLARE;
        
        if ( object )
        {
            const char* s = declname->name;
            xec_expr_objref* objref = alloc< xec_expr_objref >( sloc, object );
            assign->lvalue = alloc< xec_expr_key >( sloc, objref, s );
        }
        else
        {
            assign->lvalue = alloc< xec_expr_local >( sloc, declname );
        }
    }
    else if ( name->kind == XEC_NAME_QUAL )
    {
        assign->lvalue = declqual( sloc, name, object == NULL );
    }
    
    assign->rvalue = decl;


    // Add declaration to object or to statement block.
    if ( object )
        object->members.push_back( assign );
    else
        scope->block->stmts.push_back( assign );
}


xec_ast_node* xec_parser::declqual( int sloc, xec_ast_node* name, bool outer )
{
    if ( name->kind == XEC_NAME_NAME )
    {
        xec_name_name* n = (xec_name_name*)name;
        return lookup( n->sloc, n->name, outer );
    }
    else if ( name->kind == XEC_NAME_QUAL )
    {
        xec_name_qual* q = (xec_name_qual*)name;
        xec_ast_node* scope = declqual( q->sloc, q->scope, outer );
        return alloc< xec_expr_key >( sloc, scope, q->name );
    }
    assert( ! "invalid declaration name" );
}



xec_ast_scope* xec_parser::imply(
                xec_ast_scope* scope, xec_ast_node* name, bool lookup )
{
    // Each scope tracks object declarations in 'implied' scopes (even for
    // compound names).  Implied scopes are either truly implied or are
    // object scopes.  This allows us to bind function definitions to the
    // corresponding prototypes.

    const char* s = NULL;
    if ( name->kind == XEC_NAME_NAME )
    {
        // Declare/lookup using the name.
        xec_name_name* n = (xec_name_name*)name;
        s = n->name;
    }
    else if ( name->kind == XEC_NAME_QUAL )
    {
        // Recurse on scopes and get the rightmost name.
        xec_name_qual* q = (xec_name_qual*)name;
        scope = imply( scope, q->scope, lookup );
        if ( ! scope )
            return NULL;
        s = q->name;
    }
    else
    {
        assert( ! "invalid declaration name" );
    }
    
    // Lookup.
    auto i = scope->implied.find( s );
    if ( i != scope->implied.end() )
        return i->second;
    
    if ( lookup )
        return NULL;
    
    // Actually imply.
    xec_ast_scope* implied = alloc< xec_ast_scope >(
                XEC_SCOPE_IMPLIED, scope, nullptr, scope->func );
    scope->implied.emplace( s, implied );
    return implied;
}



xec_ast_scope* xec_parser::lookup_prototype( int sloc,
                xec_ast_scope* outer, xec_ast_node* name,
                        xec_name_list* params, bool yield )
{
    // Return the object scope that contains a matching prototype declaration.
    // Looks up using implied scopes rather than declared names.
    
    if ( ! name )
        return outer;
    
    xec_ast_scope* scope = NULL;
    const char* s = NULL;
    
    if ( name->kind == XEC_NAME_NAME )
    {
        // Use name.
        xec_name_name* n = (xec_name_name*)name;
        scope = outer;
        s = n->name;
    }
    else if ( name->kind == XEC_NAME_QUAL )
    {
        // Look up implied scopes and use rightmost name.
        xec_name_qual* q = (xec_name_qual*)name;
        scope = imply( outer, q->scope, true );
        s = q->name;
    }
    
    if ( ! scope || scope->kind != XEC_SCOPE_OBJECT )
        return outer;

    // Look up prototype.
    auto i = scope->names.find( s );
    if ( i == scope->names.end() )
        return outer;
    
    // If we're replacing a non-prototype member that's an error.
    xec_ast_prototype* proto = i->second->prototype;
    if ( ! proto )
    {
        root->script->error( sloc, "function overrides non-prototype" );
        return outer;
    }
    
    if ( proto->parameters == NULL && params == NULL )
    {
        // Matching empty parameter list.
        return scope;
    }
    
    if ( proto->parameters == NULL || params == NULL )
    {
        // Non-matching parameter list (one is empty).
        root->script->error( sloc, "function does not match prototype" );
        return outer;
    }
    
    if ( proto->parameters->names.size() != params->names.size()
            || proto->parameters->varargs != params->varargs
            || proto->coroutine != yield )
    {
        root->script->error( sloc, "function does not match prototype" );
        return outer;
    }
    
    for ( size_t i = 0; i < proto->parameters->names.size(); ++i )
    {
        xec_name_name* a = proto->parameters->names[ i ];
        xec_name_name* b = params->names[ i ];
        if ( strcmp( a->name, b->name ) != 0 )
        {
            root->script->error( sloc,
                "prototype parameter mismatch '%s' vs '%s'", a->name, b->name );
            return outer;
        }
    }
    
    return scope;
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






xec_ast_node* xec_parser::delval( xec_ast_node* delval )
{
    // Expression must be a single deletable lvalue.
    switch ( delval->kind )
    {
    case XEC_EXPR_KEY:
    case XEC_EXPR_INKEY:
        break;
    
    default:
        root->script->error( delval->sloc, "expression is not deletable" );
        break;
    }
    
    return delval;
}






