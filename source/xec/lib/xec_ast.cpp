//
//  xec_ast.cpp
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast.h"




xec_ast_scope::xec_ast_scope( xec_ast_scope_kind kind,
                xec_ast_scope* outer, xec_ast_node* node, xec_ast_func* func )
    :   kind( kind )
    ,   outer( outer )
    ,   node( node )
    ,   func( func )
    ,   block( NULL )
{
}




xec_ast_node::xec_ast_node( xec_ast_node_kind kind, int sloc )
    :   kind( kind )
    ,   sloc( sloc )
{
}




xec_expr_null::xec_expr_null( int sloc )
    :   xec_ast_node( XEC_EXPR_NULL, sloc )
{
}


xec_expr_bool::xec_expr_bool( int sloc, bool value )
    :   xec_ast_node( XEC_EXPR_BOOL, sloc )
    ,   value( value )
{
}


xec_expr_number::xec_expr_number( int sloc, double value )
    :   xec_ast_node( XEC_EXPR_NUMBER, sloc )
    ,   value( value )
{
}


xec_expr_string::xec_expr_string( int sloc, const char* string, size_t length )
    :   xec_ast_node( XEC_EXPR_STRING, sloc )
    ,   string( string )
    ,   length( length )
{
}


xec_expr_key::xec_expr_key( int sloc, xec_ast_node* object, const char* key )
    :   xec_ast_node( XEC_EXPR_KEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


xec_expr_inkey::xec_expr_inkey(
                int sloc, xec_ast_node* object, xec_ast_node* key )
    :   xec_ast_node( XEC_EXPR_INKEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


xec_expr_index::xec_expr_index(
                int sloc, xec_ast_node* object, xec_ast_node* index )
    :   xec_ast_node( XEC_EXPR_INDEX, sloc )
    ,   object( object )
    ,   index( index )
{
}


xec_expr_preop::xec_expr_preop(
                int sloc, xec_token_kind opkind, xec_ast_node* lvalue )
    :   xec_ast_node( XEC_EXPR_PREOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


xec_expr_postop::xec_expr_postop(
                int sloc, xec_token_kind opkind, xec_ast_node* lvalue )
    :   xec_ast_node( XEC_EXPR_POSTOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


xec_expr_unary::xec_expr_unary(
                int sloc, xec_token_kind opkind, xec_ast_node* operand )
    :   xec_ast_node( XEC_EXPR_UNARY, sloc )
    ,   opkind( opkind )
    ,   operand( operand )
{
}

xec_expr_binary::xec_expr_binary( int sloc,
                xec_token_kind opkind, xec_ast_node* lhs, xec_ast_node* rhs )
    :   xec_ast_node( XEC_EXPR_BINARY, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


xec_expr_compare::xec_expr_compare( int sloc, xec_ast_node* first )
    :   xec_ast_node( XEC_EXPR_COMPARE, sloc )
    ,   first( first )
{
}


xec_expr_logical::xec_expr_logical( int sloc,
                xec_token_kind opkind, xec_ast_node* lhs, xec_ast_node* rhs )
    :   xec_ast_node( XEC_EXPR_LOGICAL, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


xec_expr_qmark::xec_expr_qmark( int sloc, xec_ast_node* condition,
                xec_ast_node* iftrue, xec_ast_node* iffalse )
    :   xec_ast_node( XEC_EXPR_QMARK, sloc )
    ,   condition( condition )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}


xec_new_new::xec_new_new( int sloc, xec_ast_node* proto, xec_expr_list* args )
    :   xec_ast_node( XEC_NEW_NEW, sloc )
    ,   proto( proto )
    ,   arguments( args )
{
}


xec_new_object::xec_new_object( int sloc, xec_ast_node* proto )
    :   xec_ast_node( XEC_NEW_OBJECT, sloc )
    ,   scope( NULL )
    ,   proto( proto )
{
}


xec_new_list::xec_new_list( int sloc )
    :   xec_ast_node( XEC_NEW_LIST, sloc )
    ,   unpack( NULL )
{
}


xec_key_value::xec_key_value( xec_ast_node* key, xec_ast_node* value )
    :   key( key )
    ,   value( value )
{
}

xec_new_table::xec_new_table( int sloc )
    :   xec_ast_node( XEC_NEW_TABLE, sloc )
{
}


xec_expr_mono::xec_expr_mono( int sloc, xec_ast_node* expr )
    :   xec_ast_node( XEC_EXPR_MONO, sloc )
    ,   expr( expr )
{
}


xec_expr_call::xec_expr_call(
                int sloc, xec_ast_node* function, xec_expr_list* args )
    :   xec_ast_node( XEC_EXPR_CALL, sloc )
    ,   function( function )
    ,   arguments( args )
    ,   yieldcall( false )
    ,   unpack( false )
{
}




xec_expr_yield::xec_expr_yield( int sloc, xec_ast_node* args )
    :   xec_ast_node( XEC_EXPR_YIELD, sloc )
    ,   arguments( args )
    ,   unpack( false )
{
}


xec_expr_vararg::xec_expr_vararg( int sloc )
    :   xec_ast_node( XEC_EXPR_VARARG, sloc )
{
}

xec_expr_unpack::xec_expr_unpack( int sloc, xec_ast_node* list )
    :   xec_ast_node( XEC_EXPR_UNPACK, sloc )
    ,   list( list )
{
}


xec_expr_list::xec_expr_list( int sloc )
    :   xec_ast_node( XEC_EXPR_LIST, sloc )
    ,   final( NULL )
{
}



xec_ast_declare::xec_ast_declare( int sloc )
    :   xec_ast_node( XEC_AST_DECLARE, sloc )
    ,   name( NULL )
    ,   value( NULL )
{
}

xec_ast_declare_list::xec_ast_declare_list( int sloc )
    :   xec_ast_node( XEC_AST_DECLARE_LIST, sloc )
    ,   values( NULL )
{
}



xec_ast_assign::xec_ast_assign( int sloc, xec_token_kind assignop )
    :   xec_ast_node( XEC_AST_ASSIGN, sloc )
    ,   assignop( assignop )
    ,   lvalue( NULL )
    ,   rvalue( NULL )
{
}


xec_ast_assign_list::xec_ast_assign_list( int sloc, xec_token_kind assignop )
    :   xec_ast_node( XEC_AST_ASSIGN_LIST, sloc )
    ,   rvalues( NULL )
{
}



xec_stmt_block::xec_stmt_block( int sloc )
    :   xec_ast_node( XEC_STMT_BLOCK, sloc )
    ,   scope( NULL )
{
}

xec_stmt_if::xec_stmt_if( int sloc )
    :   xec_ast_node( XEC_STMT_IF, sloc )
    ,   scope( NULL )
    ,   condition( NULL )
    ,   iftrue( NULL )
    ,   iffalse( NULL )
{
}

xec_stmt_switch::xec_stmt_switch( int sloc )
    :   xec_ast_node( XEC_STMT_SWITCH, sloc )
    ,   scope( NULL )
    ,   value( NULL )
    ,   body( NULL )
{
}

xec_stmt_while::xec_stmt_while( int sloc )
    :   xec_ast_node( XEC_STMT_WHILE, sloc )
    ,   scope( NULL )
    ,   condition( NULL )
    ,   body( NULL )
{
}

xec_stmt_do::xec_stmt_do( int sloc )
    :   xec_ast_node( XEC_STMT_DO, sloc )
    ,   scope( NULL )
    ,   body( NULL )
    ,   condition( NULL )
{
}

xec_stmt_foreach::xec_stmt_foreach( int sloc )
    :   xec_ast_node( XEC_STMT_FOREACH, sloc )
    ,   scope( NULL )
    ,   list( NULL )
    ,   body( NULL )
    ,   declare( false )
    ,   eachkey( false )
{
}

xec_stmt_for::xec_stmt_for( int sloc )
    :   xec_ast_node( XEC_STMT_FOR, sloc )
    ,   scope( NULL )
    ,   init( NULL )
    ,   condition( NULL )
    ,   update( NULL )
    ,   body( NULL )
{
}

xec_stmt_using::xec_stmt_using( int sloc )
    :   xec_ast_node( XEC_STMT_USING, sloc )
    ,   uvalue( NULL )
    ,   body( NULL )
{
}

xec_stmt_try::xec_stmt_try( int sloc, xec_ast_node* tstmt )
    :   xec_ast_node( XEC_STMT_TRY, sloc )
    ,   tstmt( tstmt )
    ,   fstmt( NULL )
{
}

xec_stmt_catch::xec_stmt_catch( int sloc )
    :   xec_ast_node( XEC_STMT_CATCH, sloc )
    ,   scope( NULL )
    ,   lvalue( NULL )
    ,   proto( NULL )
    ,   body( NULL )
    ,   declare( false )
{
}



xec_stmt_delete::xec_stmt_delete( int sloc )
    :   xec_ast_node( XEC_STMT_DELETE, sloc )
{
}

xec_stmt_case::xec_stmt_case( int sloc, xec_ast_node* value )
    :   xec_ast_node( XEC_STMT_CASE, sloc )
    ,   value( value )
{
}

xec_stmt_continue::xec_stmt_continue( int sloc, xec_ast_node* target )
    :   xec_ast_node( XEC_STMT_CONTINUE, sloc )
    ,   target( target )
{
}

xec_stmt_break::xec_stmt_break( int sloc, xec_ast_node* target )
    :   xec_ast_node( XEC_STMT_CONTINUE, sloc )
    ,   target( target )
{
}

xec_stmt_return::xec_stmt_return( int sloc, xec_ast_node* values )
    :   xec_ast_node( XEC_STMT_RETURN, sloc )
    ,   values( values )
{
}

xec_stmt_throw::xec_stmt_throw( int sloc, xec_ast_node* value )
    :   xec_ast_node( XEC_STMT_THROW, sloc )
    ,   value( value )
{
}



xec_unqual_name::xec_unqual_name( int sloc, const char* name )
    :   xec_ast_node( XEC_UNQUAL_NAME, sloc )
    ,   name( name )
{
}


xec_unqual_list::xec_unqual_list( int sloc )
    :   xec_ast_node( XEC_UNQUAL_LIST, sloc )
{
}


xec_unqual_proto::xec_unqual_proto(
                int sloc, xec_ast_node* name, xec_expr_list* params )
    :   xec_ast_node( XEC_UNQUAL_PROTO, sloc )
    ,   name( name )
    ,   params( params )
    ,   coroutine( false )
{
}

