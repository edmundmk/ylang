//
//  xec_lj_builder.cpp
//
//  Created by Edmund Kapusniak on 10/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_lj_builder.h"
#include <make_unique>
#include "xec_srcbuf.h"



xec_lj_scope::xec_lj_scope()
    :   decltemp( 0 )
    ,   exprtemp( 0 )
{
}

void xec_lj_scope::reset()
{
    decltemp = 0;
    exprtemp = 0;
}

void xec_lj_scope::begin()
{
    exprtemp = 0;
}

std::pair< int, bool > xec_lj_scope::temporary()
{
    std::pair< int, bool > result( exprtemp, exprtemp >= decltemp );
    exprtemp += 1;
    decltemp = std::max( decltemp, exprtemp );
    return result;
}
    






xec_lj_value::xec_lj_value()
    :   b( NULL )
    ,   expr( NULL )
    ,   scope( NULL )
    ,   indent( 0 )
    ,   callnodes( false )
{
}


bool xec_lj_value::has_prologue()
{
    return ! pronodes.empty();
}

bool xec_lj_value::has_side_effect()
{
    return has_prologue() || callnodes;
}


void xec_lj_value::prologue()
{
    xec_lj_exprprologue visitor( b, this, scope, indent );
    visitor.visit( expr );
}


void xec_lj_value::value()
{
    xec_lj_exprvalue visitor( b, this, scope, indent );
    visitor.value( expr, XEC_LJ_NONE );
}


void xec_lj_value::values()
{
    xec_lj_exprvalue visitor( b, this, scope, indent );
    visitor.value( expr, XEC_LJ_MULTIPLE );
}





xec_lj_lvprol::xec_lj_lvprol()
    :   b( NULL )
    ,   expr( NULL )
    ,   scope( NULL )
    ,   indent( 0 )
    ,   objval( -1 )
    ,   idkval( -1 )
{
}
    
void xec_lj_lvprol::prologue()
{
    switch ( expr->kind )
    {
    case XEC_EXPR_LOCAL:
    case XEC_EXPR_GLOBAL:
    case XEC_EXPR_UPREF:
        break;
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)expr;
        objval = temporary( key->object );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)expr;
        objval = temporary( inkey->object );
        idkval = temporary( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)expr;
        objval = temporary( index->object );
        idkval = temporary( index->index );
        break;
    }
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

void xec_lj_lvprol::value()
{
    switch ( expr->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        xec_expr_local* local = (xec_expr_local*)expr;
        b->local( local->name->name );
        break;
    }

    case XEC_EXPR_GLOBAL:
    {
        xec_expr_global* global = (xec_expr_global*)expr;
        b->global( global->name );
        break;
    }
    
    case XEC_EXPR_UPREF:
    {
        xec_ast_upval* uv = b->upval( (xec_expr_upref*)expr );
        assert( uv->kind == XEC_UPVAL_LOCAL );
        b->local( uv->local->name );
        break;
    }
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)expr;
        b->printf( "__t%d", objval );
        b->key( key->key );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        b->printf( "__t%d[ __t%d ]", objval, idkval );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        b->printf( "__t%d.__index[ __t%d ]", objval, idkval );
        break;
    }
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }

}


int xec_lj_lvprol::temporary( xec_ast_node* expr )
{
    return b->temporary( expr, scope, indent );
}





xec_lj_builder::xec_lj_builder( xec_srcbuf* b )
    :   b( b )
{
}


void xec_lj_builder::printf( const char* format, ... )
{
    va_list arguments;
    va_start( arguments, format );
    b->vprintf( format, arguments );
    va_end( arguments );
}

void xec_lj_builder::vprintf( const char* format, va_list arguments )
{
    b->vprintf( format, arguments );
}



xec_ast_upval* xec_lj_builder::upval( xec_expr_upref* upref )
{
    xec_ast_func* func = upref->func;
    xec_ast_upval* uv = &func->upvals.at( upref->index );
    while ( uv->kind == XEC_UPVAL_UPVAL )
    {
        func = func->scope->outer->func;
        uv = &func->upvals.at( uv->upval );
    }
    return uv;
}





bool xec_lj_builder::is_keyword( const char* identifier )
{
    return strcmp( identifier, "and" ) == 0
        || strcmp( identifier, "break" ) == 0
        || strcmp( identifier, "do" ) == 0
        || strcmp( identifier, "else" ) == 0
        || strcmp( identifier, "elseif" ) == 0
        || strcmp( identifier, "end" ) == 0
        || strcmp( identifier, "false" ) == 0
        || strcmp( identifier, "for" ) == 0
        || strcmp( identifier, "function" ) == 0
        || strcmp( identifier, "if" ) == 0
        || strcmp( identifier, "in" ) == 0
        || strcmp( identifier, "local" ) == 0
        || strcmp( identifier, "nil" ) == 0
        || strcmp( identifier, "not" ) == 0
        || strcmp( identifier, "or" ) == 0
        || strcmp( identifier, "repeat" ) == 0
        || strcmp( identifier, "return" ) == 0
        || strcmp( identifier, "then" ) == 0
        || strcmp( identifier, "true" ) == 0
        || strcmp( identifier, "until" ) == 0
        || strcmp( identifier, "while" ) == 0;
}


void xec_lj_builder::local( const char* local )
{
    if ( is_keyword( local ) )
        printf( "__x_%s", local );
    else
        printf( "%s", local );
}

void xec_lj_builder::global( const char* global )
{
    if ( is_keyword( global ) )
        printf( "_G[ \"%s\" ]", global );
    else
        printf( "%s", global );
}

void xec_lj_builder::key( const char* key )
{
    if ( is_keyword( key ) )
        printf( "[ \"%s\" ]", key );
    else
        printf( ".%s", key );
}


int xec_lj_builder::object( xec_new_object* object )
{
    auto i = objects.find( object );
    if ( i != objects.end() )
    {
        return i->second;
    }
    else
    {
        int o = (int)objects.size();
        objects.emplace( object, o );
        return o;
    }
}





void xec_lj_builder::body( xec_ast_func* func, int indent )
{
    xec_lj_scope local;
    xec_lj_stmtvisitor stmtvisitor( this );
    stmtvisitor.body( func->block, &local, indent );
}


void xec_lj_builder::expr( xec_lj_value* value,
                xec_ast_node* expr, xec_lj_scope* scope, int indent )
{
    value->b        = this;
    value->expr     = expr;
    value->scope    = scope;
    value->indent   = indent;
    
    xec_lj_exprhasprologue visitor( this, value, scope, indent );
    visitor.visit( expr );
}

void xec_lj_builder::lvprol( xec_lj_lvprol* lvprol,
                xec_ast_node* lval, xec_lj_scope* scope, int indent )
{
    lvprol->b       = this;
    lvprol->expr    = lval;
    lvprol->scope   = scope;
    lvprol->indent  = indent;
}




int xec_lj_builder::temporary( xec_lj_scope* scope, int indent )
{
    auto tmpval = scope->temporary();
    if ( tmpval.second )
        b->printf( "%*slocal __t%d = ", indent, "", tmpval.first );
    else
        b->printf( "%*s__t%d = ", indent, "", tmpval.first );
    return tmpval.first;
}

int xec_lj_builder::temporary( xec_ast_node* node, xec_lj_scope* scope, int indent )
{
    xec_lj_value value;
    expr( &value, node, scope, indent );
    value.prologue();
    int tmpval = temporary( scope, indent );
    value.value();
    printf( "\n" );
    return tmpval;
}




std::pair< const char*, int > xec_lj_builder::op( xec_ast_opkind opkind )
{
    switch ( opkind )
    {
    case XEC_ASTOP_MULASSIGN:       return std::make_pair( "*", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_DIVASSIGN:       return std::make_pair( "/", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_MODASSIGN:       return std::make_pair( "%", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_INTDIVASSIGN:    return std::make_pair( "__intdiv", XEC_LJ_NONE );
    case XEC_ASTOP_ADDASSIGN:       return std::make_pair( "+", XEC_LJ_ADD );
    case XEC_ASTOP_SUBASSIGN:       return std::make_pair( "-", XEC_LJ_ADD );
    case XEC_ASTOP_LSHIFTASSIGN:    return std::make_pair( "__lsl", XEC_LJ_NONE );
    case XEC_ASTOP_LRSHIFTASSIGN:   return std::make_pair( "__lsr", XEC_LJ_NONE );
    case XEC_ASTOP_ARSHIFTASSIGN:   return std::make_pair( "__asr", XEC_LJ_NONE );
    case XEC_ASTOP_BITANDASSIGN:    return std::make_pair( "__and", XEC_LJ_NONE );
    case XEC_ASTOP_BITXORASSIGN:    return std::make_pair( "__xor", XEC_LJ_NONE );
    case XEC_ASTOP_BITORASSIGN:     return std::make_pair( "__or", XEC_LJ_NONE );
    
    case XEC_ASTOP_POSITIVE:        return std::make_pair( "- -", XEC_LJ_UNARY );
    case XEC_ASTOP_NEGATIVE:        return std::make_pair( "-", XEC_LJ_UNARY );
    case XEC_ASTOP_LOGICNOT:        return std::make_pair( "not ", XEC_LJ_UNARY );
    case XEC_ASTOP_BITNOT:          return std::make_pair( "__not", XEC_LJ_NONE );
    
    case XEC_ASTOP_MULTIPLY:        return std::make_pair( "*", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_DIVIDE:          return std::make_pair( "/", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_MODULUS:         return std::make_pair( "%", XEC_LJ_MULTIPLY );
    case XEC_ASTOP_INTDIV:          return std::make_pair( "__intdiv", XEC_LJ_NONE );
    case XEC_ASTOP_ADD:             return std::make_pair( "+", XEC_LJ_ADD );
    case XEC_ASTOP_SUBTRACT:        return std::make_pair( "-", XEC_LJ_ADD );
    case XEC_ASTOP_LSHIFT:          return std::make_pair( "__lsl", XEC_LJ_NONE );
    case XEC_ASTOP_LRSHIFT:         return std::make_pair( "__lsr", XEC_LJ_NONE );
    case XEC_ASTOP_ARSHIFT:         return std::make_pair( "__asr", XEC_LJ_NONE );
    case XEC_ASTOP_BITAND:          return std::make_pair( "__and", XEC_LJ_NONE );
    case XEC_ASTOP_BITXOR:          return std::make_pair( "__xor", XEC_LJ_NONE );
    case XEC_ASTOP_BITOR:           return std::make_pair( "__or", XEC_LJ_NONE );
    case XEC_ASTOP_CONCATENATE:     return std::make_pair( "..", XEC_LJ_CONCAT );
    
    case XEC_ASTOP_EQUAL:           return std::make_pair( "==", XEC_LJ_COMPARE );
    case XEC_ASTOP_NOTEQUAL:        return std::make_pair( "~=", XEC_LJ_COMPARE );
    case XEC_ASTOP_LESS:            return std::make_pair( "<", XEC_LJ_COMPARE );
    case XEC_ASTOP_GREATER:         return std::make_pair( ">", XEC_LJ_COMPARE );
    case XEC_ASTOP_LESSEQUAL:       return std::make_pair( "<=", XEC_LJ_COMPARE );
    case XEC_ASTOP_GREATEREQUAL:    return std::make_pair( ">=", XEC_LJ_COMPARE );
    case XEC_ASTOP_IN:              return std::make_pair( "__in", XEC_LJ_NONE );
    case XEC_ASTOP_NOTIN:           return std::make_pair( "not __in", XEC_LJ_NONE );
    case XEC_ASTOP_IS:              return std::make_pair( "__is", XEC_LJ_NONE );
    case XEC_ASTOP_NOTIS:           return std::make_pair( "not __is", XEC_LJ_NONE );
    
    default:
        assert( ! "invalid operator" );
        break;
    }
}




xec_srcbuf* xec_compile_lj( xec_ast* ast )
{
    std::unique_ptr< xec_srcbuf > srcbuf = std::make_unique< xec_srcbuf >();
    xec_lj_builder builder( srcbuf.get() );
    builder.body( ast->function, 0 );
    return srcbuf.release();
}







