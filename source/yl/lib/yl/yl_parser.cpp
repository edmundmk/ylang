//
//  yl_parser.cpp
//
//  Created by Edmund Kapusniak on 14/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_parser.h"
#include <make_unique>
#include "yl_token.h"





yl_script* yl_parse( const char* path, size_t argc, const char* const* argv )
{
    yl_script* script = new yl_script();
    script->ast = std::make_unique< yl_ast>();

    region_scope rscope( script->ast->alloc );

    // Associate AST with script.
    script->ast->script = script;
    yl_ast* ast = script->ast.get();

    // Set up root function and scope.
    ast->function = new ( ast->alloc ) yl_ast_func( -1 );
    ast->functions.push_back( ast->function );
    ast->function->scope = new ( ast->alloc ) yl_ast_scope(
                    YL_SCOPE_SCRIPT, NULL, ast->function, ast->function );
    ast->function->block = new ( ast->alloc ) yl_stmt_block( -1 );
    ast->function->scope->block = ast->function->block;
    
    
    // Preserve function name.
    char* funcname = (char*)script->alloc.malloc( strlen( path ) + 1 );
    strcpy( funcname, path );
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
            yl_ast_name* name = new ( ast->alloc )
                            yl_ast_name( -1, ast->function->scope, param );
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
    yl_parser parser( ast );
    parser.parse( path );


    // Return.
    return script;
}








yl_parser::yl_parser( yl_ast* root )
    :   root( root )
{
}


yl_parser::~yl_parser()
{
    for ( auto i = recycle_tokens.begin(); i != recycle_tokens.end();
                    i = recycle_tokens.erase( i ) )
        free( *i );
}



yl_ast* yl_parser::get_root()
{
    return root;
}




double yl_parser::parse_number( yl_token* token )
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




yl_ast_scope* yl_parser::get_scope()
{
    return scopes.back();
}


yl_ast_scope* yl_parser::block_scope( yl_ast_node* node )
{
    yl_ast_scope* outer = get_scope();
    yl_ast_scope* scope = alloc< yl_ast_scope >(
                    YL_SCOPE_BLOCK, outer, node, outer->func );
    scopes.push_back( scope );
    return scope;
}

void yl_parser::close_scope( yl_ast_scope* scope )
{
    assert( scope == get_scope() );
    scopes.pop_back();
}


yl_ast_scope* yl_parser::switch_scope( yl_ast_node* node )
{
    yl_ast_scope* outer = get_scope();
    yl_ast_scope* scope = alloc< yl_ast_scope >(
                    YL_SCOPE_SWITCH, outer, node, outer->func );
    scopes.push_back( scope );
    return scope;
}

void yl_parser::close_switch( yl_ast_scope* switch_scope )
{
    assert( switch_scope->kind == YL_SCOPE_SWITCH );

    yl_ast_scope* scope = get_scope();
    if ( scope->kind == YL_SCOPE_IMPLICIT )
    {
        // Fallen through to the end of the switch.
        root->script->error( switch_scope->node->sloc, "missing break" );
        yl_stmt_block* block = scope->block;
        close_scope( scope );
        scope = get_scope();
        scope->block->stmts.push_back( block );
    }
    
    assert( scope == switch_scope );
    close_scope( scope );
}


void yl_parser::dowhile( yl_ast_scope* scope )
{
    // This is the continue point of the dowhile loop.  It is no longer
    // valid to refer to variables that were declared after the first continue.
    scope->dowhile = true;
}












yl_new_object* yl_parser::object(
                int sloc, yl_ast_node* name, yl_ast_node* proto )
{
    // Create object.
    yl_ast_scope* outer = get_scope();
    yl_new_object* object = alloc< yl_new_object >( sloc, proto );
    object->scope = alloc< yl_ast_scope >(
                    YL_SCOPE_OBJECT, outer, object, outer->func );

    // Declare or assign.
    declname( sloc, name, object );

    // Imply scope.
    if ( name )
    {
        yl_ast_scope* scope = NULL;
        const char* s = NULL;
        
        if ( name->kind == YL_NAME_NAME )
        {
            yl_name_name* qual = (yl_name_name*)name;
            scope = outer;
            s = qual->name;
        }
        else if ( name->kind == YL_NAME_QUAL )
        {
            yl_name_qual* qual = (yl_name_qual*)name;
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


void yl_parser::prototype( int sloc,
                yl_ast_node* name, yl_name_list* params, bool yield )
{
    // Prototypes must sit directly in an object scope.
    if ( name->kind != YL_NAME_NAME )
    {
        root->script->error( sloc, "invalid prototype name" );
        return;
    }
    
    
    // Declare prototype.
    yl_name_name* m = (yl_name_name*)name;
    yl_ast_name* n = declare( m->sloc, m->name );
    n->prototype = alloc< yl_ast_prototype >( sloc );
    n->prototype->parameters    = params;
    n->prototype->coroutine     = yield;
}


yl_ast_func* yl_parser::function( int sloc, yl_ast_node* name,
                yl_name_list* params, bool yield, bool dot )
{
    // Attempt to match with prototype.
    yl_ast_scope* outer = get_scope();
    outer = lookup_prototype( sloc, outer, name, params, yield );


    // Create function.
    yl_ast_func* func = alloc< yl_ast_func >( sloc );
    root->functions.push_back( func );
    func->scope = alloc< yl_ast_scope >( YL_SCOPE_BLOCK, outer, func, func );
    
    if ( outer->kind == YL_SCOPE_OBJECT )
    {
        // Function is a member of an object (so has implicit-this).
        assert( outer->node->kind == YL_NEW_OBJECT );
        func->memberof = (yl_new_object*)outer->node;
    }
    
    
    // Create function block.
    func->block = alloc< yl_stmt_block >( sloc );
    func->scope->block = func->block;
    
    
    // Enter function scope to declare parameters.
    scopes.push_back( func->scope );

    if ( outer->kind == YL_SCOPE_OBJECT || dot )
    {
        // Function has automatic this parameter (and super).
        func->thisname = declare( sloc, "this" );
        func->parameters.push_back( func->thisname );
        
        yl_ast_name* supername = declare( sloc, "super" );
        supername->superthis = func->thisname;
    }
    
    if ( params )
    {
        for ( size_t i = 0; i < params->names.size(); ++i )
        {
            yl_name_name* m = params->names[ i ];
            yl_ast_name* paramname = declare( m->sloc, m->name );
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
        yl_ast_node* n = name;
        while ( name )
        {
            if ( funcname.size() )
            {
                funcname = "." + funcname;
            }
            if ( n->kind == YL_NAME_NAME )
            {
                yl_name_name* nn = (yl_name_name*)n;
                funcname = nn->name + funcname;
                break;
            }
            else if ( n->kind == YL_NAME_QUAL )
            {
                yl_name_qual* qq = (yl_name_qual*)n;
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




void yl_parser::var( int sloc, yl_ast_node* names, yl_ast_node* rvals )
{
    yl_ast_scope* scope = get_scope();
    
    if ( scope->kind == YL_SCOPE_OBJECT )
    {
        // Declare object member.
        assert( scope->node->kind == YL_NEW_OBJECT );
        yl_new_object* object = (yl_new_object*)scope->node;

        // Declare.
        if ( names->kind == YL_NAME_NAME )
        {
            yl_name_name* n = (yl_name_name*)names;
            declare( n->sloc, n->name );

            if ( ! rvals )
                return;
            
            yl_expr_assign* assign = alloc< yl_expr_assign >(
                            sloc, YL_ASTOP_DECLARE );
            yl_expr_objref* objref = alloc< yl_expr_objref >( sloc, object );
            assign->lvalue = alloc< yl_expr_key> ( sloc, objref, n->name );
            assign->rvalue = rvals;
            object->members.push_back( assign );
        }
        else if ( names->kind == YL_NAME_LIST )
        {
            yl_name_list* l = (yl_name_list*)names;
            for ( size_t i = 0; i < l->names.size(); ++i )
            {
                yl_name_name* n = l->names[ i ];
                declare( n->sloc, n->name );
            }

            if ( ! rvals )
                return;
         
            yl_expr_assign_list* assign = alloc< yl_expr_assign_list >(
                            sloc, YL_ASTOP_DECLARE );
            for ( size_t i = 0; i < l->names.size(); ++i )
            {
                yl_name_name* n = l->names[ i ];
                yl_expr_objref* of = alloc< yl_expr_objref >( sloc, object );
                yl_expr_key* key = alloc< yl_expr_key >( sloc, of, n->name );
                assign->lvalues.push_back( key );
            }
            assign->rvalues = rvals;
            object->members.push_back( assign );
        }

    }
    else
    {
        // Var in a block.
        yl_ast_node* assign = varstmt( sloc, names, rvals );
        scope->block->stmts.push_back( assign );
    }
}



void yl_parser::statement( yl_ast_node* stmt )
{
    if ( ! stmt )
        return;

    yl_ast_scope* scope = get_scope();
    assert( scope->kind != YL_SCOPE_OBJECT );
    assert( scope->kind != YL_SCOPE_IMPLIED );


    /*
        case statements are only allowed inside a switch block.
    */
    
    if ( stmt->kind == YL_STMT_CASE
            && scope->kind != YL_SCOPE_SWITCH
            && scope->kind != YL_SCOPE_IMPLICIT )
    {
        root->script->error( stmt->sloc, "case outside switch" );
    }

    
    /*
        switch statements are made up of case statements and implicit blocks.
        Each implicit block starts from a non-case statement and ends at a
        break.  We do it this way to allow declarations inside switch
        statements without risking skipping declarations.
    */
    
    if ( scope->kind == YL_SCOPE_SWITCH )
    {
        /*
            In the switch scope, which means the switch has just been opened, or
            an implicit block has just been closed with a break.
        */
        
        if ( stmt->kind == YL_STMT_CASE )
        {
            // Add case to switch block.
            scope->block->stmts.push_back( stmt );
        }
        else
        {
            // It's not a case - make sure there has been at least one
            // case since the last block statement.
            if ( scope->block->stmts.empty()
                    || scope->block->stmts.back()->kind != YL_STMT_CASE )
            {
                root->script->error( stmt->sloc, "expected case" );
                if ( stmt->kind == YL_STMT_BREAK )
                {
                    return;
                }
            }

            // Open implicit block.
            yl_stmt_block* block = alloc< yl_stmt_block >( stmt->sloc );
            block->scope = alloc< yl_ast_scope >(
                    YL_SCOPE_IMPLICIT, scope, block, scope->func );
            block->scope->block = block;
            scopes.push_back( block->scope );
            scope = block->scope;
            
            // Add statement to block.
            scope->block->stmts.push_back( stmt );
        }
    }
    else if ( scope->kind == YL_SCOPE_IMPLICIT )
    {
        /*
            In an implicit block inside a switch statement.
        */
    
        assert( scope->outer->kind == YL_SCOPE_SWITCH );
    
        if ( stmt->kind == YL_STMT_CASE )
        {
            // Close the implicit block (with no break, it will fall-through).
            yl_stmt_block* block = scope->block;
            close_scope( scope );
            scope = get_scope();

            // Add the implicit block to the switch.
            assert( scope->kind == YL_SCOPE_SWITCH );
            scope->block->stmts.push_back( block );

            // Add the case to the switch scope.
            scope->block->stmts.push_back( stmt );
        }
        else if ( stmt->kind == YL_STMT_BREAK )
        {
            // Add the break to the implicit block.
            assert( ( (yl_stmt_break*)stmt )->target == scope->outer );
            scope->block->stmts.push_back( stmt );
            
            // Close the block.
            yl_stmt_block* block = scope->block;
            close_scope( scope );
            scope = get_scope();

            // Add the implicit block to the switch.
            assert( scope->kind == YL_SCOPE_SWITCH );
            scope->block->stmts.push_back( block );
        }
        else
        {
            // Add statement to implicit block.
            scope->block->stmts.push_back( stmt );
        }
    }
    else
    {
        scope->block->stmts.push_back( stmt );
    }
}


yl_ast_scope* yl_parser::continue_target( int sloc )
{
    for ( yl_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == YL_STMT_WHILE
                || scope->node->kind == YL_STMT_DO
                || scope->node->kind == YL_STMT_FOREACH
                || scope->node->kind == YL_STMT_FOR )
            return scope;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    root->script->error( sloc, "continue outside loop" );
    return get_scope();
}


yl_ast_scope* yl_parser::break_target( int sloc )
{
    for ( yl_ast_scope* scope = get_scope();
                    scope != NULL; scope = scope->outer )
    {
        if ( scope->node->kind == YL_STMT_SWITCH
                || scope->node->kind == YL_STMT_WHILE
                || scope->node->kind == YL_STMT_DO
                || scope->node->kind == YL_STMT_FOREACH
                || scope->node->kind == YL_STMT_FOR )
            return scope;
    
        if ( scope->outer && scope->outer->func != scope->func )
            break;
    }
    
    root->script->error( sloc, "break outside loop or switch" );
    return get_scope();
}




yl_ast_node* yl_parser::lookup( int sloc, const char* identifier, bool outer )
{
    yl_ast_scope* local = get_scope();
    
    // Perform lookup.
    yl_ast_name* name = NULL;
    for ( yl_ast_scope* scope = local; scope != NULL; scope = scope->outer )
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
        return alloc< yl_expr_global >( sloc, identifier, false );
    }
    
    if ( name->scope->kind != YL_SCOPE_OBJECT )
    {
        // If the name is 'super' then we actually reference 'this'.
        yl_ast_name* lookname = name->superthis ? name->superthis : name;
        
        // If it's a continued variable in a closed dowhile scope, it's not
        // valid.
        if ( name->continued && name->scope->dowhile )
        {
            root->script->error( sloc,
                    "declaration of '%s' is skipped by continue", name->name );
            return alloc< yl_expr_global >( sloc, identifier, false );
        }
        
        // It's a variable, either we're referencing an upval or a local.
        yl_ast_node* nameref = NULL;
        if ( name->scope->func == local->func )
        {
            // It's a local variable.
            nameref = alloc< yl_expr_local >( sloc, lookname );
        }
        else
        {
            // It's an upval.
            nameref = alloc< yl_expr_upref >(
                        sloc, local->func, upval( local->func, lookname ) );
        }
        
        // If the name is 'super' then transform to 'superof( this )'
        if ( name->superthis )
        {
            nameref = alloc< yl_expr_superof >( sloc, nameref );
        }
        
        return nameref;
    }
    else
    {
        // It's an object member.
        assert( name->scope->node->kind == YL_NEW_OBJECT );
        yl_new_object* object = (yl_new_object*)name->scope->node;
        
        if ( name->scope->func == local->func )
        {
            // In this function.
            yl_expr_objref* objref = alloc< yl_expr_objref >( sloc, object );
            return alloc< yl_expr_key >( sloc, objref, identifier );
        }
        else if ( object == local->func->memberof )
        {
            // It's a member of the object we're declared in.
            assert( local->func->thisname );
            yl_expr_local* thisref = alloc< yl_expr_local >(
                            sloc, local->func->thisname );
            return alloc< yl_expr_key >( sloc, thisref, identifier );
        }
        else
        {
            // Object is an upval.
            yl_expr_upref* upref = alloc< yl_expr_upref >(
                            sloc, local->func, upval( local->func, object ) );
            return alloc< yl_expr_key >( sloc, upref, identifier );
        }
    
    }

}


yl_ast_node* yl_parser::key( int sloc, yl_ast_node* obj, const char* key )
{
    if ( obj->kind == YL_EXPR_GLOBAL )
    {
        yl_expr_global* global = (yl_expr_global*)obj;
        if ( ! global->gexplicit && strcmp( global->name, "global" ) == 0 )
        {
            // It's an explicit global.
            global->gexplicit = true;
            global->name = key;
            return global;
        }
    }

    return alloc< yl_expr_key >( sloc, obj, key );
}



yl_ast_node* yl_parser::lvalue( yl_ast_node* lvalue )
{
    // The expression must be a single lvalue.  Only a restricted set of
    // expressions are lvalues, and global variables must be scoped using
    // 'global.'
    
    switch ( lvalue->kind )
    {
    case YL_EXPR_LOCAL:
    case YL_EXPR_UPREF:
    case YL_EXPR_KEY:
    case YL_EXPR_INKEY:
    case YL_EXPR_INDEX:
    {
        break;
    }
    
    case YL_EXPR_GLOBAL:
    {
        yl_expr_global* g = (yl_expr_global*)lvalue;
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


void yl_parser::lvalue_list(
        yl_ast_node* list, yl_ast_node_list* lvalues )
{
    if ( list->kind == YL_EXPR_LIST )
    {
        yl_expr_list* l = (yl_expr_list*)list;
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



yl_name_list* yl_parser::name_list( yl_ast_node* list )
{
    if ( list->kind == YL_NAME_LIST )
    {
        return (yl_name_list*)list;
    }
    else
    {
        assert( list->kind == YL_NAME_NAME );
        yl_name_list* l = alloc< yl_name_list >( list->sloc );
        l->names.push_back( (yl_name_name*)list );
        return l;
    }
}



yl_expr_list* yl_parser::expr_list( yl_ast_node* list )
{
    if ( list->kind == YL_EXPR_LIST )
    {
        return (yl_expr_list*)list;
    }
    else
    {
        yl_expr_list* l = alloc< yl_expr_list >( list->sloc );
        l->values.push_back( list );
        return l;
    }
}


yl_ast_node* yl_parser::mono( yl_ast_node* expr )
{
    // Suppress multiple values and method calls.
    switch ( expr->kind )
    {
    case YL_EXPR_KEY:
    case YL_EXPR_INKEY:
    case YL_EXPR_CALL:
    case YL_EXPR_YIELD:
    case YL_EXPR_VARARG:
    case YL_EXPR_UNPACK:
    case YL_EXPR_LIST:
    case YL_EXPR_ASSIGN_LIST:
        return alloc< yl_expr_mono >( expr->sloc, expr );
        
    default:
        return expr;
    }
}



yl_ast_node* yl_parser::unpack( yl_ast_node* expr )
{
    // Assign unpack to call expressions.
    switch ( expr->kind )
    {
    case YL_EXPR_CALL:
    {
        yl_expr_call* call = (yl_expr_call*)expr;
        call->unpack = true;
        break;
    }
    
    case YL_EXPR_YIELD:
    {
        yl_expr_yield* yield = (yl_expr_yield*)expr;
        yield->unpack = true;
        break;
    }
    
    default:
        root->script->error( expr->sloc, "invalid unpack expression" );
        break;
    }
    
    return expr;
}





yl_ast_node* yl_parser::compare( int sloc,
                yl_ast_opkind op, yl_ast_node* lhs, yl_ast_node* rhs )
{
    // Build a compare expression.
    yl_expr_compare* c;
    if ( lhs->kind == YL_EXPR_COMPARE )
        c = (yl_expr_compare*)lhs;
    else
        c = alloc< yl_expr_compare >( lhs->sloc, lhs );
    
    c->opkinds.push_back( op );
    c->terms.push_back( rhs );

    return c;
}







yl_ast_node* yl_parser::assign( int sloc,
                yl_ast_opkind op, yl_ast_node* lv, yl_ast_node* rv )
{
    if ( lv->kind == YL_EXPR_LIST )
    {
        yl_expr_assign_list* a;
        a = alloc< yl_expr_assign_list >( lv->sloc, op );
        lvalue_list( (yl_expr_list*)lv, &a->lvalues );
        a->rvalues = rv;
        return a;
    }
    else
    {
        yl_expr_assign* a;
        a = alloc< yl_expr_assign >( lv->sloc, op );
        a->lvalue = lvalue( lv );
        a->rvalue = rv;
        return a;
    }
}




yl_ast_node* yl_parser::varstmt(
                int sloc, yl_ast_node* names, yl_ast_node* rvals )
{
    // If there's no value, then initialize with null.
    if ( ! rvals )
        rvals = alloc< yl_expr_null >( sloc );
    
    // Declare.
    if ( names->kind == YL_NAME_NAME )
    {
        yl_expr_assign* assign =
                alloc< yl_expr_assign >( sloc, YL_ASTOP_DECLARE );
        assign->lvalue = declare( names );
        assign->rvalue = rvals;
        return assign;
    }
    else if ( names->kind == YL_NAME_LIST )
    {
        yl_expr_assign_list* assign = alloc< yl_expr_assign_list >(
                        sloc, YL_ASTOP_DECLARE );
        declare_list( names, &assign->lvalues );
        assign->rvalues = rvals;
        return assign;
    }
    
    return NULL;
}


yl_ast_node* yl_parser::delstmt( int sloc, yl_ast_node* expr )
{
    yl_stmt_delete* del = alloc< yl_stmt_delete >( sloc );
    if ( expr->kind == YL_EXPR_LIST )
    {
        yl_expr_list* l = (yl_expr_list*)expr;
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


yl_stmt_foreach* yl_parser::foreachstmt(
            int sloc, yl_ast_scope* scope, yl_ast_node* lvals,
                        yl_ast_node* expr, bool declare, bool eachkey )
{
    yl_stmt_foreach* foreach = alloc< yl_stmt_foreach >( sloc );
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


yl_stmt_catch* yl_parser::catchstmt(
            int sloc, yl_ast_scope* scope, yl_ast_node* lval,
                        yl_ast_node* proto, bool cdeclare )
{
    yl_stmt_catch* cstmt = alloc< yl_stmt_catch >( sloc );
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



void yl_parser::check_vararg( int sloc )
{
    yl_ast_scope* scope = get_scope();
    if ( ! scope->func->varargs )
    {
        root->script->error( sloc, "varargs used outside vararg function" );
    }
}

void yl_parser::check_yield( int sloc )
{
    yl_ast_scope* scope = get_scope();
    if ( ! scope->func->coroutine )
    {
        root->script->error( sloc, "yield used outside coroutine" );
    }
}






yl_ast_name* yl_parser::declare( int sloc, const char* name )
{
    yl_ast_scope* scope = get_scope();
    
    // Error when you declare the same variable again.
    auto i = scope->names.find( name );
    if ( i != scope->names.end() )
    {
        root->script->error( sloc, "'%s' is already declared", name );
    }
    
    // Declare in scope.
    yl_ast_name* n = alloc< yl_ast_name >( sloc, scope, name );
    n->continued = scope->continued;
    scope->names.emplace( name, n );
    scope->decls.push_back( n );
    
    // Return declared name.
    return n;
}


yl_ast_node* yl_parser::declare( yl_ast_node* name )
{
    assert( name->kind == YL_NAME_NAME );
    yl_name_name* n = (yl_name_name*)name;
    yl_ast_name* m = declare( n->sloc, n->name );
    return alloc< yl_expr_local >( m->sloc, m );
}


void yl_parser::declare_list( yl_ast_node* names, yl_ast_node_list* lv )
{
    if ( names->kind == YL_NAME_NAME )
    {
        lv->push_back( declare( names ) );
    }
    else if ( names->kind == YL_NAME_LIST )
    {
        yl_name_list* l = (yl_name_list*)names;
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







void yl_parser::declname( int sloc, yl_ast_node* name, yl_ast_node* decl )
{
    if ( ! name )
    {
        return;
    }


    // Find object.
    yl_ast_scope* scope = get_scope();
    yl_new_object* object = NULL;
    if ( scope->kind == YL_SCOPE_OBJECT )
    {
        assert( scope->node->kind == YL_NEW_OBJECT );
        object = (yl_new_object*)scope->node;
    }


    // Resolve and create lvalue to assign to.
    yl_expr_assign* assign =
                    alloc< yl_expr_assign >( sloc, YL_ASTOP_ASSIGN );
    
    if ( name->kind == YL_NAME_NAME )
    {
        // Single names declare things.
        yl_name_name* n = (yl_name_name*)name;
        yl_ast_name* declname = declare( n->sloc, n->name );
        assign->assignop = YL_ASTOP_DECLARE;
        
        if ( object )
        {
            const char* s = declname->name;
            yl_expr_objref* objref = alloc< yl_expr_objref >( sloc, object );
            assign->lvalue = alloc< yl_expr_key >( sloc, objref, s );
        }
        else
        {
            assign->lvalue = alloc< yl_expr_local >( sloc, declname );
        }
    }
    else if ( name->kind == YL_NAME_QUAL )
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


yl_ast_node* yl_parser::declqual( int sloc, yl_ast_node* name, bool outer )
{
    if ( name->kind == YL_NAME_NAME )
    {
        yl_name_name* n = (yl_name_name*)name;
        return lookup( n->sloc, n->name, outer );
    }
    else if ( name->kind == YL_NAME_QUAL )
    {
        yl_name_qual* q = (yl_name_qual*)name;
        yl_ast_node* scope = declqual( q->sloc, q->scope, outer );
        return key( sloc, scope, q->name );
    }
    assert( ! "invalid declaration name" );
}



yl_ast_scope* yl_parser::imply(
                yl_ast_scope* scope, yl_ast_node* name, bool lookup )
{
    // Each scope tracks object declarations in 'implied' scopes (even for
    // compound names).  Implied scopes are either truly implied or are
    // object scopes.  This allows us to bind function definitions to the
    // corresponding prototypes.

    const char* s = NULL;
    if ( name->kind == YL_NAME_NAME )
    {
        // Declare/lookup using the name.
        yl_name_name* n = (yl_name_name*)name;
        s = n->name;
    }
    else if ( name->kind == YL_NAME_QUAL )
    {
        // Recurse on scopes and get the rightmost name.
        yl_name_qual* q = (yl_name_qual*)name;
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
    yl_ast_scope* implied = alloc< yl_ast_scope >(
                YL_SCOPE_IMPLIED, scope, nullptr, scope->func );
    scope->implied.emplace( s, implied );
    return implied;
}



yl_ast_scope* yl_parser::lookup_prototype( int sloc,
                yl_ast_scope* outer, yl_ast_node* name,
                        yl_name_list* params, bool yield )
{
    // Return the object scope that contains a matching prototype declaration.
    // Looks up using implied scopes rather than declared names.
    
    if ( ! name )
        return outer;
    
    yl_ast_scope* scope = NULL;
    const char* s = NULL;
    
    if ( name->kind == YL_NAME_NAME )
    {
        // Use name.
        yl_name_name* n = (yl_name_name*)name;
        scope = outer;
        s = n->name;
    }
    else if ( name->kind == YL_NAME_QUAL )
    {
        // Look up implied scopes and use rightmost name.
        yl_name_qual* q = (yl_name_qual*)name;
        scope = imply( outer, q->scope, true );
        s = q->name;
    }
    
    if ( ! scope || scope->kind != YL_SCOPE_OBJECT )
        return outer;

    // Look up prototype.
    auto i = scope->names.find( s );
    if ( i == scope->names.end() )
        return outer;
    
    // If we're replacing a non-prototype member that's an error.
    yl_ast_prototype* proto = i->second->prototype;
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
        yl_name_name* a = proto->parameters->names[ i ];
        yl_name_name* b = params->names[ i ];
        if ( strcmp( a->name, b->name ) != 0 )
        {
            root->script->error( sloc,
                "prototype parameter mismatch '%s' vs '%s'", a->name, b->name );
            return outer;
        }
    }
    
    return scope;
}



int yl_parser::upval( yl_ast_func* func, yl_ast_upval uv )
{
    // Find the upval in the outer function.
    assert( func->scope->outer );
    yl_ast_func* outer = func->scope->outer->func;
    switch ( uv.kind )
    {
    case YL_UPVAL_LOCAL:
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
    
    case YL_UPVAL_OBJECT:
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
    
    case YL_UPVAL_UPVAL:
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






yl_ast_node* yl_parser::delval( yl_ast_node* delval )
{
    // Expression must be a single deletable lvalue.
    switch ( delval->kind )
    {
    case YL_EXPR_KEY:
    case YL_EXPR_INKEY:
        break;
    
    default:
        root->script->error( delval->sloc, "expression is not deletable" );
        break;
    }
    
    return delval;
}






