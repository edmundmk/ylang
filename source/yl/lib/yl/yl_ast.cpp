//
//  yl_ast.cpp
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yl_ast.h"



yl_ast::yl_ast()
    :   script( NULL )
    ,   function( NULL )
{
}

yl_ast::~yl_ast()
{
}


yl_ast_scope::yl_ast_scope( yl_ast_scope_kind kind,
                yl_ast_scope* outer, yl_ast_node* node, yl_ast_func* func )
    :   kind( kind )
    ,   outer( outer )
    ,   node( node )
    ,   func( func )
    ,   block( NULL )
    ,   continued( false )
    ,   dowhile( false )
{
}


yl_ast_name::yl_ast_name( int sloc, yl_ast_scope* scope, const char* name )
    :   sloc( sloc )
    ,   scope( scope )
    ,   name( name )
    ,   superthis( NULL )
    ,   prototype( NULL )
    ,   upval( false )
    ,   continued( false )
{
}


yl_ast_prototype::yl_ast_prototype( int sloc )
    :   sloc( sloc )
    ,   parameters( NULL )
    ,   coroutine( false )
{
}


yl_ast_upval::yl_ast_upval( yl_ast_name* name )
    :   kind( XEC_UPVAL_LOCAL )
    ,   local( name )
{
}

yl_ast_upval::yl_ast_upval( yl_new_object* object )
    :   kind( XEC_UPVAL_OBJECT )
    ,   object( object )
{
}

yl_ast_upval::yl_ast_upval( int upval )
    :   kind( XEC_UPVAL_UPVAL )
    ,   upval( upval )
{
}

bool yl_ast_upval::operator == ( const yl_ast_upval& b ) const
{
    if ( kind == b.kind )
    {
        switch ( kind )
        {
        case XEC_UPVAL_LOCAL:   return local == b.local;
        case XEC_UPVAL_OBJECT:  return object == b.object;
        case XEC_UPVAL_UPVAL:   return upval == b.upval;
        }
    }
    
    return false;
}

bool yl_ast_upval::operator != ( const yl_ast_upval& b ) const
{
    return !( *this == b );
}




yl_ast_node::yl_ast_node( yl_ast_node_kind kind, int sloc )
    :   kind( kind )
    ,   sloc( sloc )
{
}



yl_ast_func::yl_ast_func( int sloc )
    :   yl_ast_node( XEC_AST_FUNC, sloc )
    ,   funcname( "" )
    ,   scope( NULL )
    ,   memberof( NULL )
    ,   thisname( NULL )
    ,   parameters( NULL )
    ,   block( NULL )
    ,   coroutine( false )
{
}




yl_expr_null::yl_expr_null( int sloc )
    :   yl_ast_node( XEC_EXPR_NULL, sloc )
{
}


yl_expr_bool::yl_expr_bool( int sloc, bool value )
    :   yl_ast_node( XEC_EXPR_BOOL, sloc )
    ,   value( value )
{
}


yl_expr_number::yl_expr_number( int sloc, double value )
    :   yl_ast_node( XEC_EXPR_NUMBER, sloc )
    ,   value( value )
{
}


yl_expr_string::yl_expr_string( int sloc, const char* string, size_t length )
    :   yl_ast_node( XEC_EXPR_STRING, sloc )
    ,   string( string )
    ,   length( length )
{
}


yl_expr_local::yl_expr_local( int sloc, yl_ast_name* name )
    :   yl_ast_node( XEC_EXPR_LOCAL, sloc )
    ,   name( name )
{
}


yl_expr_global::yl_expr_global( int sloc, const char* name, bool gexplicit )
    :   yl_ast_node( XEC_EXPR_GLOBAL, sloc )
    ,   name( name )
    ,   gexplicit( gexplicit )
{
}


yl_expr_upref::yl_expr_upref( int sloc, yl_ast_func* func, int index )
    :   yl_ast_node( XEC_EXPR_UPREF, sloc )
    ,   func( func )
    ,   index( index )
{
}


yl_expr_objref::yl_expr_objref( int sloc, yl_new_object* object )
    :   yl_ast_node( XEC_EXPR_OBJREF, sloc )
    ,   object( object )
{
}



yl_expr_key::yl_expr_key( int sloc, yl_ast_node* object, const char* key )
    :   yl_ast_node( XEC_EXPR_KEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


yl_expr_inkey::yl_expr_inkey(
                int sloc, yl_ast_node* object, yl_ast_node* key )
    :   yl_ast_node( XEC_EXPR_INKEY, sloc )
    ,   object( object )
    ,   key( key )
{
}


yl_expr_index::yl_expr_index(
                int sloc, yl_ast_node* object, yl_ast_node* index )
    :   yl_ast_node( XEC_EXPR_INDEX, sloc )
    ,   object( object )
    ,   index( index )
{
}


yl_expr_preop::yl_expr_preop(
                int sloc, yl_ast_opkind opkind, yl_ast_node* lvalue )
    :   yl_ast_node( XEC_EXPR_PREOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


yl_expr_postop::yl_expr_postop(
                int sloc, yl_ast_opkind opkind, yl_ast_node* lvalue )
    :   yl_ast_node( XEC_EXPR_POSTOP, sloc )
    ,   opkind( opkind )
    ,   lvalue( lvalue )
{
}


yl_expr_unary::yl_expr_unary(
                int sloc, yl_ast_opkind opkind, yl_ast_node* operand )
    :   yl_ast_node( XEC_EXPR_UNARY, sloc )
    ,   opkind( opkind )
    ,   operand( operand )
{
}

yl_expr_binary::yl_expr_binary( int sloc,
                yl_ast_opkind opkind, yl_ast_node* lhs, yl_ast_node* rhs )
    :   yl_ast_node( XEC_EXPR_BINARY, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


yl_expr_compare::yl_expr_compare( int sloc, yl_ast_node* first )
    :   yl_ast_node( XEC_EXPR_COMPARE, sloc )
    ,   first( first )
{
}


yl_expr_logical::yl_expr_logical( int sloc,
                yl_ast_opkind opkind, yl_ast_node* lhs, yl_ast_node* rhs )
    :   yl_ast_node( XEC_EXPR_LOGICAL, sloc )
    ,   opkind( opkind )
    ,   lhs( lhs )
    ,   rhs( rhs )
{
}


yl_expr_qmark::yl_expr_qmark( int sloc, yl_ast_node* condition,
                yl_ast_node* iftrue, yl_ast_node* iffalse )
    :   yl_ast_node( XEC_EXPR_QMARK, sloc )
    ,   condition( condition )
    ,   iftrue( iftrue )
    ,   iffalse( iffalse )
{
}


yl_new_new::yl_new_new( int sloc, yl_ast_node* proto, yl_expr_list* args )
    :   yl_ast_node( XEC_NEW_NEW, sloc )
    ,   proto( proto )
    ,   arguments( args )
{
}


yl_new_object::yl_new_object( int sloc, yl_ast_node* proto )
    :   yl_ast_node( XEC_NEW_OBJECT, sloc )
    ,   scope( NULL )
    ,   proto( proto )
    ,   upval( false )
{
}


yl_new_array::yl_new_array( int sloc )
    :   yl_ast_node( XEC_NEW_ARRAY, sloc )
    ,   final( NULL )
{
}


yl_key_value::yl_key_value( yl_ast_node* key, yl_ast_node* value )
    :   key( key )
    ,   value( value )
{
}

yl_new_table::yl_new_table( int sloc )
    :   yl_ast_node( XEC_NEW_TABLE, sloc )
{
}


yl_expr_mono::yl_expr_mono( int sloc, yl_ast_node* expr )
    :   yl_ast_node( XEC_EXPR_MONO, sloc )
    ,   expr( expr )
{
}


yl_expr_call::yl_expr_call( int sloc,
                yl_ast_node* function, yl_expr_list* args, bool yieldcall )
    :   yl_ast_node( XEC_EXPR_CALL, sloc )
    ,   function( function )
    ,   arguments( args )
    ,   yieldcall( yieldcall )
    ,   unpack( false )
{
}




yl_expr_yield::yl_expr_yield( int sloc, yl_ast_node* args )
    :   yl_ast_node( XEC_EXPR_YIELD, sloc )
    ,   arguments( args )
    ,   unpack( false )
{
}


yl_expr_vararg::yl_expr_vararg( int sloc )
    :   yl_ast_node( XEC_EXPR_VARARG, sloc )
{
}

yl_expr_unpack::yl_expr_unpack( int sloc, yl_ast_node* array )
    :   yl_ast_node( XEC_EXPR_UNPACK, sloc )
    ,   array( array )
{
}


yl_expr_list::yl_expr_list( int sloc )
    :   yl_ast_node( XEC_EXPR_LIST, sloc )
    ,   final( NULL )
{
}




yl_expr_assign::yl_expr_assign( int sloc, yl_ast_opkind assignop )
    :   yl_ast_node( XEC_EXPR_ASSIGN, sloc )
    ,   assignop( assignop )
    ,   lvalue( NULL )
    ,   rvalue( NULL )
{
}


yl_expr_assign_list::yl_expr_assign_list( int sloc, yl_ast_opkind assignop )
    :   yl_ast_node( XEC_EXPR_ASSIGN_LIST, sloc )
    ,   assignop( assignop )
    ,   rvalues( NULL )
{
}



yl_stmt_block::yl_stmt_block( int sloc )
    :   yl_ast_node( XEC_STMT_BLOCK, sloc )
    ,   scope( NULL )
{
}

yl_stmt_if::yl_stmt_if( int sloc )
    :   yl_ast_node( XEC_STMT_IF, sloc )
    ,   scope( NULL )
    ,   condition( NULL )
    ,   iftrue( NULL )
    ,   iffalse( NULL )
{
}

yl_stmt_switch::yl_stmt_switch( int sloc )
    :   yl_ast_node( XEC_STMT_SWITCH, sloc )
    ,   scope( NULL )
    ,   value( NULL )
    ,   body( NULL )
{
}

yl_stmt_while::yl_stmt_while( int sloc )
    :   yl_ast_node( XEC_STMT_WHILE, sloc )
    ,   scope( NULL )
    ,   condition( NULL )
    ,   body( NULL )
{
}

yl_stmt_do::yl_stmt_do( int sloc )
    :   yl_ast_node( XEC_STMT_DO, sloc )
    ,   scope( NULL )
    ,   body( NULL )
    ,   condition( NULL )
{
}

yl_stmt_foreach::yl_stmt_foreach( int sloc )
    :   yl_ast_node( XEC_STMT_FOREACH, sloc )
    ,   scope( NULL )
    ,   list( NULL )
    ,   body( NULL )
    ,   eachkey( false )
    ,   declare( false )
{
}

yl_stmt_for::yl_stmt_for( int sloc )
    :   yl_ast_node( XEC_STMT_FOR, sloc )
    ,   scope( NULL )
    ,   init( NULL )
    ,   condition( NULL )
    ,   update( NULL )
    ,   body( NULL )
{
}

yl_stmt_using::yl_stmt_using( int sloc )
    :   yl_ast_node( XEC_STMT_USING, sloc )
    ,   uvalue( NULL )
    ,   body( NULL )
{
}

yl_stmt_try::yl_stmt_try( int sloc, yl_ast_node* tstmt )
    :   yl_ast_node( XEC_STMT_TRY, sloc )
    ,   tstmt( tstmt )
    ,   fstmt( NULL )
{
}

yl_stmt_catch::yl_stmt_catch( int sloc )
    :   yl_ast_node( XEC_STMT_CATCH, sloc )
    ,   scope( NULL )
    ,   lvalue( NULL )
    ,   proto( NULL )
    ,   body( NULL )
    ,   declare( false )
{
}



yl_stmt_delete::yl_stmt_delete( int sloc )
    :   yl_ast_node( XEC_STMT_DELETE, sloc )
{
}

yl_stmt_case::yl_stmt_case( int sloc, yl_ast_node* value )
    :   yl_ast_node( XEC_STMT_CASE, sloc )
    ,   value( value )
{
}

yl_stmt_continue::yl_stmt_continue(
                int sloc, yl_ast_scope* scope, yl_ast_scope* target )
    :   yl_ast_node( XEC_STMT_CONTINUE, sloc )
    ,   scope( scope )
    ,   target( target )
{
}

yl_stmt_break::yl_stmt_break(
                int sloc, yl_ast_scope* scope, yl_ast_scope* target )
    :   yl_ast_node( XEC_STMT_BREAK, sloc )
    ,   scope( scope )
    ,   target( target )
{
}

yl_stmt_return::yl_stmt_return(
                int sloc, yl_ast_scope* scope, yl_ast_node* values )
    :   yl_ast_node( XEC_STMT_RETURN, sloc )
    ,   scope( scope )
    ,   values( values )
{
}

yl_stmt_throw::yl_stmt_throw( int sloc, yl_ast_node* value )
    :   yl_ast_node( XEC_STMT_THROW, sloc )
    ,   value( value )
{
}



yl_name_name::yl_name_name( int sloc, const char* name )
    :   yl_ast_node( XEC_NAME_NAME, sloc )
    ,   name( name )
{
}


yl_name_qual::yl_name_qual( int sloc, yl_ast_node* scope, const char* name )
    :   yl_ast_node( XEC_NAME_QUAL, sloc )
    ,   scope( scope )
    ,   name( name )
{
}


yl_name_list::yl_name_list( int sloc )
    :   yl_ast_node( XEC_NAME_LIST, sloc )
    ,   varargs( false )
{
}


