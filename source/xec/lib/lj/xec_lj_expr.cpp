//
//  xec_lj_expr.cpp
//
//  Created by Edmund Kapusniak on 07/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_lj_expr.h"
#include <vector>
#include "xec_ast_visitor.h"
#include "xec_srcbuf.h"
#include "xec_lj_builder.h"









xec_lj_exprhasprologue::xec_lj_exprhasprologue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent )
    :   b( b )
    ,   v( v )
    ,   scope( scope )
    ,   indent( indent )
{
}


bool xec_lj_exprhasprologue::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}


bool xec_lj_exprhasprologue::visit( xec_ast_func* node )
{
    return false;
}


bool xec_lj_exprhasprologue::visit( xec_expr_null* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_bool* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_number* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_string* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_local* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_global* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_upref* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_objref* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_key* node )
{
    if ( visit( node->object ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_inkey* node )
{
    if ( visit( node->object ) || visit( node->key ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_index* node )
{
    if ( visit( node->object ) || visit( node->index ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_preop* node )
{
    v->pronodes.insert( node );
    return true;
}

bool xec_lj_exprhasprologue::visit( xec_expr_postop* node )
{
    v->pronodes.insert( node );
    return true;
}

bool xec_lj_exprhasprologue::visit( xec_expr_unary* node )
{
    if ( visit( node->operand ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_binary* node )
{
    if ( visit( node->lhs ) || visit( node->rhs ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_compare* node )
{
    if ( node->terms.size() > 1
            || visit( node->first )
            || visit( node->terms.at( 0 ) ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_logical* node )
{
    if ( node->opkind == XEC_ASTOP_LOGICAND
            || node->opkind == XEC_ASTOP_LOGICOR
            || visit( node->lhs )
            || visit( node->rhs ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_qmark* node )
{
    v->pronodes.insert( node );
    return true;
}


bool xec_lj_exprhasprologue::visit( xec_new_new* node )
{
    v->callnodes = true;
    if ( visit( node->proto ) ||
            ( node->arguments && visit( node->arguments ) ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_new_object* node )
{
    v->pronodes.insert( node );
    return true;
}

bool xec_lj_exprhasprologue::visit( xec_new_array* node )
{
    v->pronodes.insert( node );
    return true;
}

bool xec_lj_exprhasprologue::visit( xec_new_table* node )
{
    v->pronodes.insert( node );
    return true;
}


bool xec_lj_exprhasprologue::visit( xec_expr_mono* node )
{
    if ( visit( node->expr ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_call* node )
{
    v->callnodes = true;

    if ( node->function->kind == XEC_EXPR_INKEY )
    {
        v->pronodes.insert( node );
        return true;
    }
    
    if ( node->function->kind == XEC_EXPR_KEY )
    {
        xec_expr_key* key = (xec_expr_key*)node->function;
        if ( b->is_keyword( key->key ) )
        {
            v->pronodes.insert( node );
            return true;
        }
    }
    
    if ( visit( node->function ) ||
            ( node->arguments && visit( node->arguments ) ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_yield* node )
{
    v->callnodes = true;

    if ( node->arguments && visit( node->arguments ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_vararg* node )
{
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_unpack* node )
{
    if ( visit( node->array ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_list* node )
{
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        if ( visit( node->values.at( i ) ) )
        {
            v->pronodes.insert( node );
            return true;
        }
    }
    
    if ( node->final && visit( node->final ) )
    {
        v->pronodes.insert( node );
        return true;
    }
    
    return false;
}

bool xec_lj_exprhasprologue::visit( xec_expr_assign* node )
{
    v->pronodes.insert( node );
    return true;
}

bool xec_lj_exprhasprologue::visit( xec_expr_assign_list* node )
{
    v->pronodes.insert( node );
    return true;
}






xec_lj_exprprologue::xec_lj_exprprologue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent )
    :   b( b )
    ,   v( v )
    ,   scope( scope )
    ,   indent( indent )
{
}


void xec_lj_exprprologue::fallback( xec_ast_node* node )
{
    assert( ! "expected expression" );
}
    

void xec_lj_exprprologue::visit( xec_ast_func* node )
{
    // no prologue
}
    
void xec_lj_exprprologue::visit( xec_expr_null* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_bool* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_number* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_string* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_local* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_global* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_upref* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_objref* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_key* node )
{
    visit( node->object );
}

void xec_lj_exprprologue::visit( xec_expr_inkey* node )
{
    if ( has_prologue( node->key ) )
    {
        temporary( node->object );
        visit( node->key );
    }
    else
    {
        visit( node->object );
        visit( node->key );
    }
}

void xec_lj_exprprologue::visit( xec_expr_index* node )
{
    if ( has_prologue( node->index ) )
    {
        temporary( node->object );
        visit( node->index );
    }
    else
    {
        visit( node->object );
        visit( node->index );
    }
}

void xec_lj_exprprologue::visit( xec_expr_preop* node )
{
    const char* op = node->opkind == XEC_ASTOP_PREINC ? "+" : "-";
    xec_lj_lvprol lvalue;
    b->lvprol( &lvalue, node->lvalue, scope, indent );
    lvalue.prologue();
    int value = temporary();
    lvalue.value();
    b->printf( " %s 1\n", op );
    b->printf( "%*s", indent, "" );
    lvalue.value();
    b->printf( " = __t%d\n", value );
    v->temporaries.emplace( node, value );
}

void xec_lj_exprprologue::visit( xec_expr_postop* node )
{
    const char* op = node->opkind == XEC_ASTOP_POSTINC ? "+" : "-";
    xec_lj_lvprol lvalue;
    b->lvprol( &lvalue, node->lvalue, scope, indent );
    lvalue.prologue();
    int value = temporary();
    lvalue.value();
    b->printf( "\n" );
    b->printf( "%*s", indent, "" );
    lvalue.value();
    b->printf( " = __t%d %s 1\n", value, op );
    v->temporaries.emplace( node, value );
}

void xec_lj_exprprologue::visit( xec_expr_unary* node )
{
    visit( node->operand );
}

void xec_lj_exprprologue::visit( xec_expr_binary* node )
{
    if ( has_prologue( node->rhs ) )
    {
        temporary( node->lhs );
        visit( node->rhs );
    }
    else
    {
        visit( node->lhs );
        visit( node->rhs );
    }
}


void xec_lj_exprprologue::visit( xec_expr_compare* node )
{
    if ( node->terms.size() == 1 )
    {
        if ( has_prologue( node->terms.at( 0 ) ) )
        {
            temporary( node->first );
            visit( node->terms.at( 0 ) );
        }
        else
        {
            visit( node->first );
            visit( node->terms.at( 0 ) );
        }
    }
    else
    {
        xec_lj_value first;
        b->expr( &first, node->first, scope, indent );
        first.prologue();
        int lhsval = temporary();
        first.value();
        b->printf( "\n" );
        
        xec_lj_value term;
        b->expr( &term, node->terms.at( 0 ), scope, indent );
        term.prologue();
        int rhsval = temporary();
        term.value();
        b->printf( "\n" );

        int value = temporary();
        auto op = b->op( node->opkinds.at( 0 ) );
        if ( op.second == XEC_LJ_NONE )
            b->printf( "%s( __t%d, __t%d )\n", op.first, lhsval, rhsval );
        else
            b->printf( "__t%d %s __t%d\n", lhsval, op.first, rhsval );
        
        int indent = this->indent;
        for ( size_t i = 1; i < node->terms.size(); ++i )
        {
            b->printf( "%*sif __t%d then\n", indent, "", value );
            indent += INDENT;
            
            std::swap( lhsval, rhsval );
            
            xec_lj_value term;
            b->expr( &term, node->terms.at( i ), scope, indent );
            term.prologue();
            b->printf( "%*s__t%d = ", indent, "", rhsval );
            term.value();
            b->printf( "\n" );
            
            b->printf( "%*s__t%d = ", indent, "", value );
            auto op = b->op( node->opkinds.at( i ) );
            if ( op.second == XEC_LJ_NONE )
                b->printf( "%s( __t%d, __t%d )\n", op.first, lhsval, rhsval );
            else
                b->printf( "__t%d %s __t%d\n", lhsval, op.first, rhsval );
        }

        for ( size_t i = 1; i < node->terms.size(); ++i )
        {
            indent -= INDENT;
            b->printf( "%*send\n", indent, "" );
        }
        
        v->temporaries.emplace( node, value );
    }

}

void xec_lj_exprprologue::visit( xec_expr_logical* node )
{
    switch ( node->opkind )
    {
    case XEC_ASTOP_LOGICAND:
    {
        xec_lj_value lhs;
        b->expr( &lhs, node->lhs, scope, indent );
        lhs.prologue();
        int value = temporary();
        lhs.value();
        b->printf( "\n" );
        b->printf( "%*sif not __test( __t%d ) then\n", indent, "", value );
        xec_lj_value rhs;
        b->expr( &rhs, node->rhs, scope, indent + INDENT );
        rhs.prologue();
        b->printf( "%*s__t%d = ", indent + INDENT, "", value );
        rhs.value();
        b->printf( "\n" );
        b->printf( "%*send\n", indent, "" );
        v->temporaries.emplace( node, value );
        break;
    }
    
    case XEC_ASTOP_LOGICXOR:
    {
        if ( has_prologue( node->rhs ) )
        {
            temporary( node->lhs );
            visit( node->rhs );
        }
        else
        {
            visit( node->lhs );
            visit( node->rhs );
        }
        break;
    }
    
    case XEC_ASTOP_LOGICOR:
    {
        xec_lj_value lhs;
        b->expr( &lhs, node->lhs, scope, indent );
        lhs.prologue();
        int value = temporary();
        lhs.value();
        b->printf( "\n" );
        b->printf( "%*sif __test( __t%d ) then\n", indent, "", value );
        xec_lj_value rhs;
        b->expr( &rhs, node->rhs, scope, indent + INDENT );
        rhs.prologue();
        b->printf( "%*s__t%d = ", indent + INDENT, "", value );
        rhs.value();
        b->printf( "\n" );
        b->printf( "%*send\n", indent, "" );
        v->temporaries.emplace( node, value );
        break;
    }
    
    default:
        assert( ! "invalid logical operator" );
        break;
    }
}

void xec_lj_exprprologue::visit( xec_expr_qmark* node )
{
    int value = temporary();
    b->printf( "nil\n" );
    xec_lj_value condition;
    b->expr( &condition, node->condition, scope, indent );
    condition.prologue();
    b->printf( "%*sif __test( ", indent, "" );
    condition.value();
    b->printf( " ) then\n" );
    xec_lj_value iftrue;
    b->expr( &iftrue, node->iftrue, scope, indent + INDENT );
    iftrue.prologue();
    b->printf( "%*s__t%d = ", indent + INDENT, "", value );
    iftrue.value();
    b->printf( "\n" );
    b->printf( "%*selse\n", indent, "" );
    xec_lj_value iffalse;
    b->expr( &iffalse, node->iffalse, scope, indent + INDENT );
    iffalse.prologue();
    b->printf( "%*s__t%d = ", indent + INDENT, "", value );
    iffalse.value();
    b->printf( "\n" );
    b->printf( "%*send\n", indent, "" );
    v->temporaries.emplace( node, value );
}

    
void xec_lj_exprprologue::visit( xec_new_new* node )
{
    if ( node->arguments && has_prologue( node->arguments ) )
    {
        temporary( node->proto );
        visit( node->arguments );
    }
    else
    {
        visit( node->proto );
        if ( node->arguments )
            visit( node->arguments );
    }
}

void xec_lj_exprprologue::visit( xec_new_object* node )
{
    int object = b->object( node );
    xec_lj_value proto;
    if ( node->proto )
    {
        b->expr( &proto, node->proto, scope, indent );
        proto.prologue();
    }
    b->printf( "%*s__o%d = __newobj( ", indent, "", object );
    if ( node->proto )
    {
        proto.value();
    }
    else
    {
        b->printf( "__object" );
    }
    b->printf( " )\n" );
    
    if ( node->members.size() )
    {
        b->printf( "%*sdo\n", indent, "" );
        xec_lj_scope local;
        xec_lj_stmtvisitor visitor( b );
        for ( size_t i = 0; i < node->members.size(); ++i )
        {
            visitor.visit( node->members.at( i ), &local, indent + INDENT );
        }
        b->printf( "%*send\n", indent, "" );
    }
}

void xec_lj_exprprologue::visit( xec_new_array* node )
{
    int arrval = temporary();
    b->printf( "__newobj( __array )\n" );
    
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        xec_lj_value value;
        b->expr( &value, node->values.at( i ), scope, indent );
        value.prologue();
        b->printf( "%*s__t%d:append( ", indent, "", arrval );
        value.value();
        b->printf( " )\n" );
    }
    
    if ( node->final )
    {
        xec_lj_value final;
        b->expr( &final, node->final, scope, indent );
        final.prologue();
        b->printf( "%*s__t%d:append( ", indent, "", arrval );
        final.values();
        b->printf( " )\n" );
    }
    
    v->temporaries.emplace( node, arrval );
}

void xec_lj_exprprologue::visit( xec_new_table* node )
{
    int tblval = temporary();
    b->printf( "__newobj( __table )\n" );
    
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        xec_lj_value key;
        xec_lj_value value;
        b->expr( &key, node->elements.at( i ).key, scope, indent );
        b->expr( &value, node->elements.at( i ).value, scope, indent );
        key.prologue();
        value.prologue();
        b->printf( "%*s__t%d.__index[ ", indent, "", tblval );
        key.value();
        b->printf( " ] = " );
        value.value();
        b->printf( "\n" );
    }
    
    v->temporaries.emplace( node, tblval );
}


void xec_lj_exprprologue::visit( xec_expr_mono* node )
{
    visit( node->expr );
}

void xec_lj_exprprologue::visit( xec_expr_call* node )
{
    if ( node->arguments && has_prologue( node->arguments ) )
    {
        // function must be assigned to a temporary.
        if ( node->function->kind == XEC_EXPR_INKEY )
        {
            // this is referenced more than once.
            xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
            int objval = temporary( inkey->object );
            xec_lj_value key;
            b->expr( &key, inkey->key, scope, indent );
            key.prologue();
            int value = temporary();
            b->printf( "__t%d[ ", objval );
            key.value();
            b->printf( " ]\n" );
            v->temporaries.emplace( inkey, value );
        }
        else if ( node->function->kind == XEC_EXPR_KEY )
        {
            xec_expr_key* key = (xec_expr_key*)node->function;
            temporary( key->object );
        }
        else
        {
            temporary( node->function );
        }
    
        visit( node->arguments );
    }
    else
    {
        if ( node->function->kind == XEC_EXPR_INKEY )
        {
            // this is referenced more than once.
            xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
            temporary( inkey->object );
            visit( inkey->key );
        }
        else if ( node->function->kind == XEC_EXPR_KEY )
        {
            xec_expr_key* key = (xec_expr_key*)node->function;
            if ( b->is_keyword( key->key ) )
                temporary( key->object );
            else
                visit( key->object );
        }
        else
        {
            visit( node->function );
        }
    
        if ( node->arguments )
            visit( node->arguments );
    }

}

void xec_lj_exprprologue::visit( xec_expr_yield* node )
{
    if ( node->arguments )
        visit( node->arguments );
}

void xec_lj_exprprologue::visit( xec_expr_vararg* node )
{
    // no prologue
}

void xec_lj_exprprologue::visit( xec_expr_unpack* node )
{
    visit( node->array );
}

void xec_lj_exprprologue::visit( xec_expr_list* node )
{
    // If any of the expressions in the list have a prologue, then we
    // must evaluate the preceding expressions into temporaries.
        
    if ( has_prologue( node ) )
    {
        // Find last expression which has a prologue.
        xec_ast_node* last = NULL;
        for ( size_t i = 0; i < node->values.size(); ++i )
        {
            xec_ast_node* value = node->values.at( i );
            if ( has_prologue( value ) )
                last = value;
        }
        if ( node->final && has_prologue( node->final ) )
        {
            last = node->final;
        }
        
        if ( ! last )
            return;

        // Create temporaries.
        for ( size_t i = 0; i < node->values.size(); ++i )
        {
            xec_ast_node* value = node->values.at( i );
            if ( value == last )
                return;
            
            temporary( value );
        }
        
        // Final value never requires a temporary.
    }
}

void xec_lj_exprprologue::visit( xec_expr_assign* node )
{
    assert( node->assignop != XEC_ASTOP_DECLARE );

    xec_lj_lvprol lvalue;
    b->lvprol( &lvalue, node->lvalue, scope, indent );
    lvalue.prologue();
    
    int lvval = -1;
    if ( node->assignop != XEC_ASTOP_ASSIGN )
    {
        lvval = temporary();
        lvalue.value();
        b->printf( "\n" );
    }

    xec_lj_value rvalue;
    b->expr( &rvalue, node->rvalue, scope, indent );
    rvalue.prologue();
    
    int value = temporary();
    if ( node->assignop != XEC_ASTOP_ASSIGN )
    {
        auto op = b->op( node->assignop );
        if ( op.second == XEC_LJ_NONE )
        {
            b->printf( "%s( __t%d, ", op.first, lvval );
            rvalue.value();
            b->printf( " )\n" );
        }
        else
        {
            b->printf( "__t%d %s ", lvval, op.first );
            rvalue.value();
            b->printf( "\n" );
        }
    }
    else
    {
        rvalue.value();
        b->printf( "\n" );
    }
    
    b->printf( "%*s", indent, "" );
    lvalue.value();
    b->printf( " = __t%d\n", value );
    
    v->temporaries.emplace( node, value );
}

void xec_lj_exprprologue::visit( xec_expr_assign_list* node )
{
    assert( node->assignop != XEC_ASTOP_DECLARE );

    std::vector< xec_lj_lvprol > lvalues;
    lvalues.reserve( node->lvalues.size() );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        lvalues.emplace_back();
        b->lvprol( &lvalues.back(), node->lvalues.at( i ), scope, indent );
        lvalues.back().prologue();
    }
    
    std::vector< int > lvvals;
    if ( node->assignop != XEC_ASTOP_ASSIGN )
    {
        lvvals.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lvvals.push_back( temporary() );
            lvalues.at( i ).value();
            b->printf( "\n" );
        }
    }
    
    xec_lj_value rvalues;
    b->expr( &rvalues, node->rvalues, scope, indent );
    rvalues.prologue();

    int declared = -1;
    std::vector< int > rvvals;
    rvvals.reserve( node->lvalues.size() );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        auto temporary = scope->temporary();
        rvvals.push_back( temporary.first );
        if ( temporary.second )
        {
            if ( declared != -1 )
            {
                b->printf( ", __t%d", temporary.first );
            }
            else
            {
                b->printf( "%*slocal __t%d", indent, "", temporary.first );
                declared = (int)i;
            }
        }
        v->temporaries.emplace( node->lvalues.at( i ), temporary.first );
    }
    if ( declared > 0 )
    {
        b->printf( "\n" );
    }
    if ( declared != 0 )
    {
        b->printf( "%*s", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
                b->printf( ", __t%d", rvvals.at( i ) );
            else
                b->printf( "__t%d", rvvals.at( i ) );
        }
    }
    b->printf( " = " );
    rvalues.values();
    b->printf( "\n" );

    if ( node->assignop != XEC_ASTOP_ASSIGN )
    {
        auto op = b->op( node->assignop );
    
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
                b->printf( ", __t%d", rvvals.at( i ) );
            else
                b->printf( "__t%d", rvvals.at( i ) );
        }
        b->printf( " = " );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            int lvval = lvvals.at( i );
            int rvval = rvvals.at( i );
            if ( i > 0 )
                b->printf( ", " );
            if ( op.second == XEC_LJ_NONE )
                b->printf( "%s( __t%d, __t%d )", op.first, lvval, rvval );
            else
                b->printf( "__t%d %s __t%d", lvval, op.first, rvval );
        }
        b->printf( "\n" );
    }
    
    b->printf( "%*s", indent, "" );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        if ( i > 0 )
            b->printf( ", " );
        lvalues.at( i ).value();
    }
    b->printf( " = " );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        if ( i > 0 )
            b->printf( ", __t%d", rvvals.at( i ) );
        else
            b->printf( "__t%d", rvvals.at( i ) );
    }
    b->printf( "\n" );
    
}


bool xec_lj_exprprologue::has_prologue( xec_ast_node* node )
{
    return v->pronodes.find( node ) != v->pronodes.end();
}
    
int xec_lj_exprprologue::temporary()
{
    return b->temporary( scope, indent );
}

int xec_lj_exprprologue::temporary( xec_ast_node* node )
{
    int tmpval = b->temporary( node, scope, indent );
    v->temporaries.emplace( node, tmpval );
    return tmpval;
}







xec_lj_exprvalue::xec_lj_exprvalue(
        xec_lj_builder* b, xec_lj_value* v, xec_lj_scope* scope, int indent )
    :   b( b )
    ,   v( v )
    ,   scope( scope )
    ,   indent( indent )
{
}


void xec_lj_exprvalue::value( xec_ast_node* node, int precedence )
{
    auto i = v->temporaries.find( node );
    if ( i != v->temporaries.end() )
        b->printf( "__t%d", i->second );
    else
        visit( node, precedence );
}


void xec_lj_exprvalue::fallback( xec_ast_node* node, int precedence )
{
    assert( ! "expected expression" );
}

void xec_lj_exprvalue::visit( xec_ast_func* node, int precedence )
{
    b->printf( "function(" );
    for ( size_t i = 0; i < node->parameters.size(); ++i )
    {
        if ( i > 0 )
            b->printf( ", %s", node->parameters.at( i )->name );
        else
            b->printf( " %s", node->parameters.at( i )->name );
    }
    if ( node->parameters.size() )
        b->printf( " " );
    b->printf( ")\n" );
    b->body( node, indent + INDENT );
    b->printf( "%*send", indent, "" );
}

void xec_lj_exprvalue::visit( xec_expr_null* node, int precedence )
{
    b->printf( "null" );
}

void xec_lj_exprvalue::visit( xec_expr_bool* node, int precedence )
{
    b->printf( node->value ? "true" : "false" );
}

void xec_lj_exprvalue::visit( xec_expr_number* node, int precedence )
{
    b->printf( "%.17g", node->value );
}

void xec_lj_exprvalue::visit( xec_expr_string* node, int precedence )
{
    b->printf( "\"" );
    for ( size_t i = 0; i < node->length; ++i )
    {
        char c = node->string[ i ];
        if ( c == '\\' )
        {
            b->printf( "\\" );
        }
        else if ( c == '"' )
        {
            b->printf( "\"" );
        }
        else if ( c >= 0x20 && c <= 0x7E )
        {
            b->printf( "%c", c );
        }
        else switch ( c )
        {
        case 0x07: b->printf( "\\a" ); break;
        case 0x08: b->printf( "\\b" ); break;
        case 0x09: b->printf( "\\t" ); break;
        case 0x0A: b->printf( "\\n" ); break;
        case 0x0B: b->printf( "\\v" ); break;
        case 0x0C: b->printf( "\\f" ); break;
        case 0x0D: b->printf( "\\r" ); break;
        
        default:
            b->printf( "\\%03u", (unsigned)c );
            break;
        }
    }
    b->printf( "\"" );
}

void xec_lj_exprvalue::visit( xec_expr_local* node, int precedence )
{
    b->local( node->name->name );
}

void xec_lj_exprvalue::visit( xec_expr_global* node, int precedence )
{
    b->global( node->name );
}

void xec_lj_exprvalue::visit( xec_expr_upref* node, int precedence )
{
    xec_ast_upval* uv = b->upval( node );
    switch ( uv->kind )
    {
    case XEC_UPVAL_LOCAL:
        b->local( uv->local->name );
        break;
    
    case XEC_UPVAL_OBJECT:
        b->printf( "__o%d", b->object( uv->object ) );
        break;
    
    case XEC_UPVAL_UPVAL:
        assert( ! "upval should be resolved" );
        break;
    }
}

void xec_lj_exprvalue::visit( xec_expr_objref* node, int precedence )
{
    b->printf( "__o%d", b->object( node->object ) );
}

void xec_lj_exprvalue::visit( xec_expr_key* node, int precedence )
{
    value( node->object, XEC_LJ_POSTFIX );
    b->key( node->key );
}

void xec_lj_exprvalue::visit( xec_expr_inkey* node, int precedence )
{
    value( node->object, XEC_LJ_POSTFIX );
    b->printf( "[ " );
    value( node->key, XEC_LJ_NONE );
    b->printf( " ]" );
}

void xec_lj_exprvalue::visit( xec_expr_index* node, int precedence )
{
    value( node->object, XEC_LJ_POSTFIX );
    b->printf( ".__index[ " );
    value( node->index, XEC_LJ_NONE );
    b->printf( " ]" );
}

void xec_lj_exprvalue::visit( xec_expr_preop* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_expr_postop* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_expr_unary* node, int precedence )
{
    auto op = b->op( node->opkind );
    if ( op.second != XEC_LJ_NONE )
    {
        if ( ! parentheses( node, precedence, op.second ) )
        {
            b->printf( "%s", op.first );
            value( node->operand, op.second );
        }
    }
    else
    {
        b->printf( "%s( ", op.first );
        value( node->operand, XEC_LJ_NONE );
        b->printf( " )" );
    }
}

void xec_lj_exprvalue::visit( xec_expr_binary* node, int precedence )
{
    auto op = b->op( node->opkind );
    if ( op.second != XEC_LJ_NONE )
    {
        if ( ! parentheses( node, precedence, op.second ) )
        {
            value( node->lhs, op.second );
            b->printf( " %s ", op.first );
            value( node->rhs, -op.second );
        }
    }
    else
    {
        b->printf( "%s( ", op.first );
        value( node->lhs, XEC_LJ_NONE );
        b->printf( ", " );
        value( node->rhs, XEC_LJ_NONE );
        b->printf( " )" );
    }
}

void xec_lj_exprvalue::visit( xec_expr_compare* node, int precedence )
{
    if ( node->terms.size() > 1 )
    {
        assert( ! "expression should have temporary" );
        return;
    }
    
    auto op = b->op( node->opkinds.at( 0 ) );
    if ( op.second != XEC_LJ_NONE )
    {
        if ( ! parentheses( node, precedence, op.second ) )
        {
            value( node->first, op.second );
            b->printf( " %s ", op.first );
            value( node->terms.at( 0 ), op.second );
        }
    }
    else
    {
        b->printf( "%s( ", op.first );
        value( node->first, XEC_LJ_NONE );
        b->printf( ", " );
        value( node->terms.at( 0 ), XEC_LJ_NONE );
        b->printf( " )" );
    }
}

void xec_lj_exprvalue::visit( xec_expr_logical* node, int precedence )
{
    if ( node->opkind == XEC_ASTOP_LOGICXOR )
    {
        b->printf( "__lxor( " );
        value( node->lhs, XEC_LJ_NONE );
        b->printf( ", " );
        value( node->rhs, XEC_LJ_NONE );
        b->printf( " )" );
    }
    else
    {
        assert( ! "expression should have temporary" );
    }
}

void xec_lj_exprvalue::visit( xec_expr_qmark* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_new_new* node, int precedence )
{
    b->printf( "__new( " );
    value( node->proto, XEC_LJ_NONE );
    if ( node->arguments )
    {
        b->printf( ", " );
        value( node->arguments, XEC_LJ_MULTIPLE );
    }
    b->printf( " )" );
}

void xec_lj_exprvalue::visit( xec_new_object* node, int precedence )
{
    b->printf( "__o%d", b->object( node ) );
}

void xec_lj_exprvalue::visit( xec_new_array* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_new_table* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_expr_mono* node, int precedence )
{
    precedence = precedence == XEC_LJ_MULTIPLE ? XEC_LJ_NONE : precedence;
    value( node->expr, precedence );
}

void xec_lj_exprvalue::visit( xec_expr_call* node, int precedence )
{
    bool hasarg = false;

    if ( node->function->kind == XEC_EXPR_INKEY )
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node->function;
        int objtmp = v->temporaries.at( inkey->object );
        int funtmp = v->temporaries.at( inkey );
        b->printf( "__t%d( __t%d", funtmp, objtmp );
        hasarg = true;
    }
    else if ( node->function->kind == XEC_EXPR_KEY )
    {
        xec_expr_key* key = (xec_expr_key*)node->function;
        if ( b->is_keyword( key->key ) )
        {
            int objtmp = v->temporaries.at( key->object );
            b->printf( "__t%d[ \"%s\" ]( __t%d", objtmp, key->key, objtmp );
            hasarg = true;
        }
        else
        {
            value( key->object, XEC_LJ_POSTFIX );
            b->printf( ":%s(", key->key );
        }
    }
    else
    {
        value( node->function, XEC_LJ_POSTFIX );
        b->printf( "(" );
    }

    if ( node->arguments )
    {
        if ( hasarg )
            b->printf( ", " );
        else
            b->printf( " " );
        value( node->arguments, XEC_LJ_MULTIPLE );
        hasarg = true;
    }

    if ( hasarg )
        b->printf( " " );
    b->printf( ")" );
}

void xec_lj_exprvalue::visit( xec_expr_yield* node, int precedence )
{
    b->printf( "__yield(" );
    if ( node->arguments )
    {
        b->printf( " " );
        value( node->arguments, XEC_LJ_MULTIPLE );
        b->printf( " " );
    }
    b->printf( ")" );
}

void xec_lj_exprvalue::visit( xec_expr_vararg* node, int precedence )
{
    if ( precedence == XEC_LJ_MULTIPLE )
        b->printf( "..." );
    else
        b->printf( "( ... )" );
}

void xec_lj_exprvalue::visit( xec_expr_unpack* node, int precedence )
{
    if ( precedence == XEC_LJ_MULTIPLE )
    {
        b->printf( "__unpack( " );
        value( node->array, XEC_LJ_NONE );
        b->printf( " )" );
    }
    else
    {
        value( node->array, XEC_LJ_POSTFIX );
        b->printf( ".__index[ 0 ]" );
    }
}

void xec_lj_exprvalue::visit( xec_expr_list* node, int precedence )
{
    if ( precedence != XEC_LJ_MULTIPLE )
        b->printf( "__mono( " );
 
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        if ( i > 0 )
            b->printf( ", " );
        value( node->values.at( i ), XEC_LJ_NONE );
    }
    if ( node->final )
    {
        if ( node->values.size() )
            b->printf( ", " );
        value( node->final, XEC_LJ_MULTIPLE );
    }
    
    if ( precedence != XEC_LJ_MULTIPLE )
        b->printf( " )" );
}

void xec_lj_exprvalue::visit( xec_expr_assign* node, int precedence )
{
    assert( ! "expression should have temporary" );
}

void xec_lj_exprvalue::visit( xec_expr_assign_list* node, int precedence )
{
    if ( precedence == XEC_LJ_MULTIPLE )
    {
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            int lvval = v->temporaries.at( node->lvalues.at( i ) );
            if ( i > 0 )
                b->printf( ", " );
            b->printf( "__t%d", lvval );
        }
    }
    else
    {
        int lvval = v->temporaries.at( node->lvalues.at( 0 ) );
        b->printf( "__t%d", lvval );
    }
}



bool xec_lj_exprvalue::parentheses(
                xec_ast_node* node, int precedence, int exprprec )
{
    // precedence is the precedence of the expression we're in.  It's negative
    // on the right hand side of a binary expression.  exprprec is the
    // precedence of this expression.
    int absprec = abs( precedence );
    
    if ( absprec <= XEC_LJ_MULTIPLE )
        return false;
    
    if ( absprec > exprprec )
        return false;
    
    if ( absprec == exprprec )
    {
        if ( absprec != XEC_LJ_CONCAT && precedence > 0 )
            return false;
        if ( absprec == XEC_LJ_CONCAT && precedence < 0 )
            return false;
    }
    
    // Need to add parentheses.
    b->printf( "( " );
    visit( node, XEC_LJ_NONE );
    b->printf( " )" );
    return true;
}




