//
//  yl_compile_visitor.cpp
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_compile_visitor.h"
#include <assert.h>
#include <unordered_map>




y_opcode yl_astop_to_opcode( yl_ast_opkind op )
{
    switch ( op )
    {
    case YL_ASTOP_NEGATIVE:         return Y_NEG;
    case YL_ASTOP_BITNOT:           return Y_BITNOT;
    
    case YL_ASTOP_MULTIPLY:         return Y_MUL;
    case YL_ASTOP_DIVIDE:           return Y_DIV;
    case YL_ASTOP_MODULUS:          return Y_MOD;
    case YL_ASTOP_INTDIV:           return Y_INTDIV;
    case YL_ASTOP_ADD:              return Y_ADD;
    case YL_ASTOP_SUBTRACT:         return Y_SUB;
    case YL_ASTOP_LSHIFT:           return Y_LSL;
    case YL_ASTOP_LRSHIFT:          return Y_LSR;
    case YL_ASTOP_ARSHIFT:          return Y_ASR;
    case YL_ASTOP_BITAND:           return Y_BITAND;
    case YL_ASTOP_BITXOR:           return Y_BITXOR;
    case YL_ASTOP_BITOR:            return Y_BITOR;
    case YL_ASTOP_CONCATENATE:      return Y_CONCAT;
    
    case YL_ASTOP_EQUAL:            return Y_EQ;
    case YL_ASTOP_NOTEQUAL:         return Y_NE;
    case YL_ASTOP_LESS:             return Y_LT;
    case YL_ASTOP_GREATER:          return Y_GT;
    case YL_ASTOP_LESSEQUAL:        return Y_LE;
    case YL_ASTOP_GREATEREQUAL:     return Y_GE;
    
    case YL_ASTOP_IN:               return Y_IN;
    case YL_ASTOP_IS:               return Y_IS;
    
    default:
        assert( ! "unexpected operator" );
    }
}




void yl_compile_visitor::fallback( yl_ast_node* node )
{
    assert( ! "unknown AST node" );
}


void yl_compile_visitor::visit( yl_stmt_block* node )
{
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        execute( node->stmts.at( i ) );
    }
    
    if ( node->scope )
    {
        undeclare( node->scope );
    }
}

void yl_compile_visitor::visit( yl_stmt_if* node )
{
    /*
            test condition
            if ( failed ) goto else or endif
            statements
            goto endif
        elseif:
            statements
        endif:
    */

    unsigned v = push( node->condition );
    pop( v );
    int test_jump = jump( Y_JMPF, v );
    
    if ( node->iftrue )
    {
        execute( node->iftrue );
    }
    if ( node->iffalse )
    {
        int endif_jump = jump( Y_JMP, 0 );
        patch( test_jump, label() );
        execute( node->iffalse );
        patch( endif_jump, label() );
    }
    else
    {
        patch( test_jump, label() );
    }
    
    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_switch* node )
{
    /*
        It's possible that a case expression could clobber the comparison
        value.  Unless we can disprove this, we must keep a copy of the
        value on the stack during the case sequence.
    */
    
    unsigned u = push( node->value );
    
    /*
            push value
            eval value
            if ( equal ) goto case0
            eval value
            if ( equal ) goto case1
            goto default
     
        case0:
            statements
        case1:
        default:
            statements
        break:
    */
    
    open_break( node->scope );

    std::unordered_map< yl_stmt_case*, int > case_jumps;
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind != YL_STMT_CASE )
            continue;
        
        yl_stmt_case* cstmt = (yl_stmt_case*)stmt;
        if ( ! cstmt->value )
            continue;
        
        unsigned v = push( cstmt->value );
        pop( v );
        unsigned w = push();
        op( Y_EQ, w, u, v );
        pop( w );
        case_jumps.emplace( cstmt, jump( Y_JMPT, w ) );
    }
    
    pop( u );
    
    int default_jump = jump( Y_JMP, 0 );
    bool has_default = false;
    
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind == YL_STMT_CASE )
        {
            yl_stmt_case* cstmt = (yl_stmt_case*)stmt;
            if ( cstmt->value )
            {
                patch( case_jumps.at( cstmt ), label() );
            }
            else
            {
                patch( default_jump, label() );
                has_default = true;
            }
        }
        else
        {
            execute( stmt );
        }
    }
    
    int break_label = label();
    
    if ( ! has_default )
    {
        patch( default_jump, break_label );
    }
    
    close_break( node->scope, break_label );
    
    if ( node->body->scope )
    {
        undeclare( node->body->scope );
    }
    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_while* node )
{
    /*
        continue:
            test condition
            if ( failed ) goto break:
            statements
            goto continue
        break:
    */
    
    int continue_label = label();
    open_break( node->scope );
    open_continue( node->scope );

    unsigned v = push( node->condition );
    pop( v );
    int test_jump = jump( Y_JMPF, v );
    
    execute( node->body );
    
    close_continue( node->scope, continue_label );
    patch( jump( Y_JMP, 0 ), continue_label );
    
    int break_label = label();
    close_break( node->scope, break_label );
    patch( test_jump, break_label );
    
    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_do* node )
{
    /*
        top:
            statements
        continue:
            test
            if ( succeeded ) goto top
        break:
    */

    int top_label = label();
    open_break( node->scope );
    open_continue( node->scope );
    
    execute( node->body );
    
    int continue_label = label();
    close_continue( node->scope, continue_label );
    
    unsigned v = push( node->condition );
    pop( v );
    patch( jump( Y_JMPT, v ), top_label );
    
    int break_label = label();
    close_break( node->scope, break_label );
    
    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_foreach* node )
{
    /*
            iter/iterkey list
            goto entry
        continue:
            assign/declare iterator values
            statements
        entry:
            if ( have values ) goto continue
        break:
     
    */
    
    unsigned l = push( node->list );
    pop( l );
    unsigned i = push_iterator();
    op( node->eachkey ? Y_ITERKEY : Y_ITER, i, l, 0 );
    
    int entry_jump = jump( Y_JMP, 0 );
    
    int continue_label = label();
    open_break( node->scope );
    open_continue( node->scope );
    
    if ( node->declare )
    {
        auto rv = push_list( (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );

        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            declare( rv.r + (int)i, local->name );
        }
    }
    else
    {
        std::vector< yl_compile_visitor::lvalue > lv;
        lv.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lv.push_back( push_lvalue( node->lvalues.at( i ) ) );
        }
        
        auto rv = push_list( (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );
     
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assign( lv.at( i ), rv.r + (int)i );
        }
        
        pop_list( rv );
        for ( auto i = lv.rbegin(); i != lv.rend(); ++i )
        {
            pop_lvalue( *i );
        }
    }
    
    execute( node->body );
    
    int entry_label = label();
    patch( entry_jump, entry_label );
    
    patch( jump( Y_JMPITER, i ), continue_label );
    close_continue( node->scope, continue_label );
    
    int break_label = label();
    close_break( node->scope, break_label );

    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_for* node )
{
    /*
            init
        top:
            condition
            if ( failed ) goto break:
            statements
        continue:
            update
            goto top
        break:
    */


    if ( node->init )
    {
        pop( push( node->init ) );
    }
    
    int top_label = label();
    open_break( node->scope );
    open_continue( node->scope );
    
    int test_jump = -1;
    if ( node->condition )
    {
        int v = push( node->condition );
        pop( v );
        test_jump = jump( Y_JMPF, v );
    }
    
    execute( node->body );
    
    int continue_label = label();
    close_continue( node->scope, continue_label );

    if ( node->update )
    {
        pop( push( node->update ) );
    }
    
    patch( jump( Y_JMP, 0 ), top_label );
    
    int break_label = label();
    if ( test_jump != -1 )
    {
        patch( test_jump, break_label );
    }
    close_break( node->scope, break_label );
    
    undeclare( node->scope );
}

void yl_compile_visitor::visit( yl_stmt_using* node )
{
}

void yl_compile_visitor::visit( yl_stmt_try* node )
{
}

void yl_compile_visitor::visit( yl_stmt_catch* node )
{
    assert( ! "catch outside try" );
}

void yl_compile_visitor::visit( yl_stmt_delete* node )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        yl_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == YL_EXPR_KEY )
        {
            yl_expr_key* expr = (yl_expr_key*)node;
            int v = push( expr->object );
            pop( v );
            op( Y_DELKEY, 0, v, key( expr ) );
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            int v = push( expr->object );
            int u = push( expr->key );
            pop( u );
            pop( v );
            op( Y_DELINKEY, 0, v, u );
        }
    }
}

void yl_compile_visitor::visit( yl_stmt_case* node )
{
    assert( ! "case outside switch" );
}

void yl_compile_visitor::visit( yl_stmt_continue* node )
{
    add_continue( node->target );
}

void yl_compile_visitor::visit( yl_stmt_break* node )
{
    add_break( node->target );
}

void yl_compile_visitor::visit( yl_stmt_return* node )
{
    auto lv = push_list( node->values, -1 );
    pop_list( lv );
    op( Y_RETURN, 0, lv.r, lv.count );
}

void yl_compile_visitor::visit( yl_stmt_throw* node )
{
    int v = push( node->value );
    pop( v );
    op( Y_THROW, 0, v, 0 );
}
















void yl_compile_visitor::op( y_opcode op, unsigned r, unsigned a, unsigned b )
{
    s->code.emplace_back( op, r, a, b );
}

void yl_compile_visitor::op( y_opcode op, unsigned r, unsigned c )
{
    s->code.emplace_back( op, r, c );
}

void yl_compile_visitor::op( y_opcode op, unsigned r, signed j )
{
    s->code.emplace_back( op, r, j );
}



int yl_compile_visitor::label()
{
    return (int)s->code.size();
}

int yl_compile_visitor::jump( y_opcode opcode, unsigned r )
{
    s->code.emplace_back( opcode, r, (signed)0 );
    return (int)s->code.size();
}

void yl_compile_visitor::patch( int jump, int label )
{
    y_opinst& j = s->code.at( jump - 1 );
    j = y_opinst( j.opcode(), j.r(), (signed)( label - jump ) );
}





void yl_compile_visitor::open_break( yl_ast_scope* target )
{
    break_stack.emplace_back( target );
}

void yl_compile_visitor::add_break( yl_ast_scope* target )
{
    branch& branch_break = break_stack.back();
    assert( branch_break.target == target );
    branch_break.jumps.push_back( jump( Y_JMP, 0 ) );
}

void yl_compile_visitor::close_break( yl_ast_scope* target, int label )
{
    branch& branch_break = break_stack.back();
    assert( branch_break.target == target );
    for ( size_t i = 0; i < branch_break.jumps.size(); ++i )
    {
        patch( branch_break.jumps.at( i ), label );
    }
    break_stack.pop_back();
}

void yl_compile_visitor::open_continue( yl_ast_scope* target )
{
    continue_stack.emplace_back( target );
}

void yl_compile_visitor::add_continue( yl_ast_scope* target )
{
    branch& branch_continue = continue_stack.back();
    assert( branch_continue.target == target );
    branch_continue.jumps.push_back( jump( Y_JMP, 0 ) );
}

void yl_compile_visitor::close_continue( yl_ast_scope* target, int label )
{
    branch& branch_continue = continue_stack.back();
    assert( branch_continue.target == target );
    for ( size_t i = 0; i < branch_continue.jumps.size(); ++i )
    {
        patch( branch_continue.jumps.at( i ), label );
    }
    continue_stack.pop_back();
}












