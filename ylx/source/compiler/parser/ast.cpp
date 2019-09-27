//
//  ast.cpp
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright © 2014-2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "ast.h"

namespace yl
{


ast_script::ast_script()
{
}

ast_script::~ast_script()
{
}


ast_scope::ast_scope( ast_scope_kind kind, ast_scope* outer, ast_node* node, ast_function* function )
    :   kind( kind )
    ,   outer( outer )
    ,   node( node )
    ,   function( function )
    ,   block( nullptr )
    ,   continued( false )
    ,   do_while( false )
{
}


ast_name::ast_name( srcloc sloc, ast_scope* scope, const char* name )
    :   sloc( sloc )
    ,   scope( scope )
    ,   name( name )
    ,   super_this( nullptr )
    ,   prototype( nullptr )
    ,   upval( false )
    ,   continued( false )
{
}


ast_prototype::ast_prototype( srcloc sloc )
    :   sloc( sloc )
    ,   parameters( nullptr )
    ,   generator( false )
{
}


ast_upval::ast_upval( ast_name* name )
    :   kind( AST_UPVAL_LOCAL )
    ,   local( name )
{
}

ast_upval::ast_upval( ast_make_object* object )
    :   kind( AST_UPVAL_OBJECT )
    ,   object( object )
{
}

ast_upval::ast_upval( int upval )
    :   kind( AST_UPVAL_UPVAL )
    ,   upval( upval )
{
}

bool ast_upval::operator == ( const ast_upval& b ) const
{
    if ( kind == b.kind )
    {
        switch ( kind )
        {
        case AST_UPVAL_LOCAL:   return local == b.local;
        case AST_UPVAL_OBJECT:  return object == b.object;
        case AST_UPVAL_UPVAL:   return upval == b.upval;
        }
    }

    return false;
}

bool ast_upval::operator != ( const ast_upval& b ) const
{
    return !( *this == b );
}


ast_key_value::ast_key_value( ast_node* key, ast_node* value )
    :   key( key )
    ,   value( value )
{
}


ast_node::ast_node( ast_node_kind kind, srcloc sloc )
    :   kind( kind )
    ,   sloc( sloc )
{
}


ast_function::ast_function( srcloc sloc )
    :   ast_node( AST_FUNCTION, sloc )
    ,   name( "" )
    ,   scope( nullptr )
    ,   member_of( nullptr )
    ,   this_name( nullptr )
    ,   block( nullptr )
    ,   generator( false )
{
}


ast_expr_null::ast_expr_null( srcloc sloc )
    :   ast_node( AST_EXPR_NULL, sloc )
{
}


ast_expr_bool::ast_expr_bool( srcloc sloc, bool value )
    :   ast_node( AST_EXPR_BOOL, sloc )
    ,   value( value )
{
}


ast_expr_number::ast_expr_number( srcloc sloc, double value )
    :   ast_node( AST_EXPR_NUMBER, sloc )
    ,   value( value )
{
}


ast_expr_string::ast_expr_string( srcloc sloc, const char* string, size_t length )
    :   ast_node( AST_EXPR_STRING, sloc )
    ,   string( string )
    ,   length( length )
{
}


ast_expr_local::ast_expr_local( srcloc sloc, ast_name* name )
    :   ast_node( AST_EXPR_LOCAL, sloc )
    ,   name( name )
{
}


ast_expr_global::ast_expr_global( srcloc sloc, const char* name, bool gexplicit )
    :   ast_node( AST_EXPR_GLOBAL, sloc )
    ,   name( name )
    ,   gexplicit( gexplicit )
{
}


ast_expr_upref::ast_expr_upref( srcloc sloc, ast_function* func, int index )
    :   ast_node( AST_EXPR_UPREF, sloc )
    ,   func( func )
    ,   index( index )
{
}


ast_expr_objref::ast_expr_objref( srcloc sloc, ast_make_object* object )
    :   ast_node( AST_EXPR_OBJREF, sloc )
    ,   object( object )
{
}



ast_expr_superof::ast_expr_superof( srcloc sloc, ast_node* expr )
    :   ast_node( AST_EXPR_SUPEROF, sloc )
    ,   expr( expr )
{
}


ast_expr_key::ast_expr_key( srcloc sloc, ast_node* object, const char* key )
    :   ast_node( AST_EXPR_KEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


ast_expr_inkey::ast_expr_inkey(
                srcloc sloc, ast_node* object, ast_node* key )
    :   ast_node( AST_EXPR_INKEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


ast_expr_index::ast_expr_index(
                srcloc sloc, ast_node* object, ast_node* index )
    :   ast_node( AST_EXPR_INDEX, sloc )
    ,   object( object )
    ,   index( index )
{
}


ast_expr_responds::ast_expr_responds(
                srcloc sloc, ast_node* object, const char* key )
    :   ast_node( AST_EXPR_RESPONDS, sloc )
    ,   object( object )
    ,   key( key )
{
}

ast_expr_inresponds::ast_expr_inresponds(
                srcloc sloc, ast_node* object, ast_node* key )
    :   ast_node( AST_EXPR_INRESPONDS, sloc )
    ,   object( object )
    ,   key( key )
{
}


ast_expr_preop::ast_expr_preop( srcloc sloc, ast_op_kind opkind, ast_node* lvalue )
    :   ast_node( AST_EXPR_PREOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


ast_expr_postop::ast_expr_postop( srcloc sloc, ast_op_kind opkind, ast_node* lvalue )
    :   ast_node( AST_EXPR_POSTOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


ast_expr_unary::ast_expr_unary( srcloc sloc, ast_op_kind opkind, ast_node* operand )
    :   ast_node( AST_EXPR_UNARY, sloc )
    ,   opkind( opkind )
    ,   operand( operand )
{
}

ast_expr_binary::ast_expr_binary( srcloc sloc,
                ast_op_kind opkind, ast_node* lhs, ast_node* rhs )
    :   ast_node( AST_EXPR_BINARY, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


ast_expr_compare::ast_expr_compare( srcloc sloc, ast_node* first )
    :   ast_node( AST_EXPR_COMPARE, sloc )
    ,   first( first )
{
}


ast_expr_logical::ast_expr_logical( srcloc sloc, ast_op_kind opkind, ast_node* lhs, ast_node* rhs )
    :   ast_node( AST_EXPR_LOGICAL, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


ast_expr_qmark::ast_expr_qmark( srcloc sloc, ast_node* condition, ast_node* iftrue, ast_node* iffalse )
    :   ast_node( AST_EXPR_QMARK, sloc )
    ,   condition( condition )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}


ast_make_new::ast_make_new( srcloc sloc, ast_node* proto, ast_expr_list* args )
    :   ast_node( AST_MAKE_NEW, sloc )
    ,   proto( proto )
    ,   arguments( args )
{
}


ast_make_object::ast_make_object( srcloc sloc, ast_node* proto )
    :   ast_node( AST_MAKE_OBJECT, sloc )
    ,   scope( nullptr )
    ,   proto( proto )
    ,   upval( false )
{
}


ast_make_array::ast_make_array( srcloc sloc )
    :   ast_node( AST_MAKE_ARRAY, sloc )
    ,   final( nullptr )
{
}


ast_make_table::ast_make_table( srcloc sloc )
    :   ast_node( AST_MAKE_TABLE, sloc )
{
}


ast_expr_mono::ast_expr_mono( srcloc sloc, ast_node* expr )
    :   ast_node( AST_EXPR_MONO, sloc )
    ,   expr( expr )
{
}


ast_expr_call::ast_expr_call( srcloc sloc,
                ast_node* function, ast_expr_list* args, bool yieldcall )
    :   ast_node( AST_EXPR_CALL, sloc )
    ,   function( function )
    ,   arguments( args )
    ,   yieldcall( yieldcall )
    ,   unpack( false )
{
}




ast_expr_yield::ast_expr_yield( srcloc sloc, ast_node* args )
    :   ast_node( AST_EXPR_YIELD, sloc )
    ,   arguments( args )
    ,   unpack( false )
{
}


ast_expr_vararg::ast_expr_vararg( srcloc sloc )
    :   ast_node( AST_EXPR_VARARG, sloc )
{
}

ast_expr_unpack::ast_expr_unpack( srcloc sloc, ast_node* array )
    :   ast_node( AST_EXPR_UNPACK, sloc )
    ,   array( array )
{
}


ast_expr_list::ast_expr_list( srcloc sloc )
    :   ast_node( AST_EXPR_LIST, sloc )
    ,   final( nullptr )
{
}




ast_expr_assign::ast_expr_assign( srcloc sloc, ast_op_kind assignop )
    :   ast_node( AST_EXPR_ASSIGN, sloc )
    ,   assignop( assignop )
    ,   lvalue( nullptr )
    ,   rvalue( nullptr )
{
}


ast_expr_assign_list::ast_expr_assign_list( srcloc sloc, ast_op_kind assignop )
    :   ast_node( AST_EXPR_ASSIGN_LIST, sloc )
    ,   assignop( assignop )
    ,   rvalues( nullptr )
{
}



ast_stmt_block::ast_stmt_block( srcloc sloc )
    :   ast_node( AST_STMT_BLOCK, sloc )
    ,   scope( nullptr )
{
}

ast_stmt_if::ast_stmt_if( srcloc sloc )
    :   ast_node( AST_STMT_IF, sloc )
    ,   scope( nullptr )
    ,   condition( nullptr )
    ,   iftrue( nullptr )
    ,   iffalse( nullptr )
{
}

ast_stmt_switch::ast_stmt_switch( srcloc sloc )
    :   ast_node( AST_STMT_SWITCH, sloc )
    ,   scope( nullptr )
    ,   value( nullptr )
    ,   body( nullptr )
{
}

ast_stmt_while::ast_stmt_while( srcloc sloc )
    :   ast_node( AST_STMT_WHILE, sloc )
    ,   scope( nullptr )
    ,   condition( nullptr )
    ,   body( nullptr )
{
}

ast_stmt_do::ast_stmt_do( srcloc sloc )
    :   ast_node( AST_STMT_DO, sloc )
    ,   scope( nullptr )
    ,   body( nullptr )
    ,   condition( nullptr )
{
}

ast_stmt_foreach::ast_stmt_foreach( srcloc sloc )
    :   ast_node( AST_STMT_FOREACH, sloc )
    ,   scope( nullptr )
    ,   list( nullptr )
    ,   body( nullptr )
    ,   eachkey( false )
    ,   declare( false )
{
}

ast_stmt_for::ast_stmt_for( srcloc sloc )
    :   ast_node( AST_STMT_FOR, sloc )
    ,   scope( nullptr )
    ,   init( nullptr )
    ,   condition( nullptr )
    ,   update( nullptr )
    ,   body( nullptr )
{
}

ast_stmt_using::ast_stmt_using( srcloc sloc )
    :   ast_node( AST_STMT_USING, sloc )
    ,   uvalue( nullptr )
    ,   body( nullptr )
{
}

ast_stmt_try::ast_stmt_try( srcloc sloc, ast_node* tstmt )
    :   ast_node( AST_STMT_TRY, sloc )
    ,   tstmt( tstmt )
    ,   fstmt( nullptr )
{
}

ast_stmt_catch::ast_stmt_catch( srcloc sloc )
    :   ast_node( AST_STMT_CATCH, sloc )
    ,   scope( nullptr )
    ,   lvalue( nullptr )
    ,   proto( nullptr )
    ,   body( nullptr )
    ,   declare( false )
{
}



ast_stmt_delete::ast_stmt_delete( srcloc sloc )
    :   ast_node( AST_STMT_DELETE, sloc )
{
}

ast_stmt_case::ast_stmt_case( srcloc sloc, ast_node* value )
    :   ast_node( AST_STMT_CASE, sloc )
    ,   value( value )
{
}

ast_stmt_continue::ast_stmt_continue( srcloc sloc, ast_scope* scope, ast_scope* target )
    :   ast_node( AST_STMT_CONTINUE, sloc )
    ,   scope( scope )
    ,   target( target )
{
}

ast_stmt_break::ast_stmt_break( srcloc sloc, ast_scope* scope, ast_scope* target )
    :   ast_node( AST_STMT_BREAK, sloc )
    ,   scope( scope )
    ,   target( target )
{
}

ast_stmt_return::ast_stmt_return(
                srcloc sloc, ast_scope* scope, ast_node* values )
    :   ast_node( AST_STMT_RETURN, sloc )
    ,   scope( scope )
    ,   values( values )
{
}

ast_stmt_throw::ast_stmt_throw( srcloc sloc, ast_node* value )
    :   ast_node( AST_STMT_THROW, sloc )
    ,   value( value )
{
}



ast_bind_name::ast_bind_name( srcloc sloc, const char* name )
    :   ast_node( AST_BIND_NAME, sloc )
    ,   name( name )
{
}


ast_bind_qual::ast_bind_qual( srcloc sloc, ast_node* scope, const char* name )
    :   ast_node( AST_BIND_QUAL, sloc )
    ,   scope( scope )
    ,   name( name )
{
}


ast_bind_list::ast_bind_list( srcloc sloc )
    :   ast_node( AST_BIND_LIST, sloc )
    ,   varargs( false )
{
}


}

