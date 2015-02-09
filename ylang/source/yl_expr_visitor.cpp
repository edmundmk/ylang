//
//  yl_expr_visitor.cpp
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_expr_visitor.h"
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




yl_compile_statement::yl_compile_statement( yl_compile_script* c )
    :   c( c )
{
}


void yl_compile_statement::fallback( yl_ast_node* node )
{
    c->pop( c->push( node ) );
}


void yl_compile_statement::visit( yl_stmt_block* node )
{
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        visit( node->stmts.at( i ) );
    }
    
    if ( node->scope )
    {
        c->undeclare( node->scope );
    }
}

void yl_compile_statement::visit( yl_stmt_if* node )
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

    unsigned v = c->push( node->condition );
    c->pop( v );
    int test_jump = c->jump( Y_JMPF, v );
    
    if ( node->iftrue )
    {
        visit( node->iftrue );
    }
    if ( node->iffalse )
    {
        int endif_jump = c->jump( Y_JMP, 0 );
        c->patch( test_jump, c->label() );
        visit( node->iffalse );
        c->patch( endif_jump, c->label() );
    }
    else
    {
        c->patch( test_jump, c->label() );
    }
    
    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_switch* node )
{
    /*
        It's possible that a case expression could clobber the comparison
        value.  Unless we can disprove this, we must keep a copy of the
        value on the stack during the case sequence.
    */
    
    unsigned u = c->push( node->value );
    
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
    
    c->open_break( node->scope );

    std::unordered_map< yl_stmt_case*, int > case_jumps;
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind != YL_STMT_CASE )
            continue;
        
        yl_stmt_case* cstmt = (yl_stmt_case*)stmt;
        if ( ! cstmt->value )
            continue;
        
        unsigned v = c->push( cstmt->value );
        c->pop( v );
        unsigned w = c->push();
        c->op( Y_EQ, w, u, v );
        c->pop( w );
        case_jumps.emplace( cstmt, c->jump( Y_JMPT, w ) );
    }
    
    c->pop( u );
    
    int default_jump = c->jump( Y_JMP, 0 );
    bool has_default = false;
    
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind == YL_STMT_CASE )
        {
            yl_stmt_case* cstmt = (yl_stmt_case*)stmt;
            if ( cstmt->value )
            {
                c->patch( case_jumps.at( cstmt ), c->label() );
            }
            else
            {
                c->patch( default_jump, c->label() );
                has_default = true;
            }
        }
        else
        {
            visit( stmt );
        }
    }
    
    int break_label = c->label();
    
    if ( ! has_default )
    {
        c->patch( default_jump, break_label );
    }
    
    c->close_break( node->scope, break_label );
    
    if ( node->body->scope )
    {
        c->undeclare( node->body->scope );
    }
    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_while* node )
{
    /*
        continue:
            test condition
            if ( failed ) goto break:
            statements
            goto continue
        break:
    */
    
    int continue_label = c->label();
    c->open_break( node->scope );
    c->open_continue( node->scope );

    unsigned v = c->push( node->condition );
    c->pop( v );
    int test_jump = c->jump( Y_JMPF, v );
    
    visit( node->body );
    
    c->close_continue( node->scope, continue_label );
    c->patch( c->jump( Y_JMP, 0 ), continue_label );
    
    int break_label = c->label();
    c->close_break( node->scope, break_label );
    c->patch( test_jump, break_label );
    
    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_do* node )
{
    /*
        top:
            statements
        continue:
            test
            if ( succeeded ) goto top
        break:
    */

    int top_label = c->label();
    c->open_break( node->scope );
    c->open_continue( node->scope );
    
    visit( node->body );
    
    int continue_label = c->label();
    c->close_continue( node->scope, continue_label );
    
    unsigned v = c->push( node->condition );
    c->pop( v );
    c->patch( c->jump( Y_JMPT, v ), top_label );
    
    int break_label = c->label();
    c->close_break( node->scope, break_label );
    
    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_foreach* node )
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
    
    unsigned l = c->push( node->list );
    c->pop( l );
    unsigned i = c->push_iterator();
    c->op( node->eachkey ? Y_ITERKEY : Y_ITER, i, l, 0 );
    
    int entry_jump = c->jump( Y_JMP, 0 );
    
    int continue_label = c->label();
    c->open_break( node->scope );
    c->open_continue( node->scope );
    
    if ( node->declare )
    {
        auto rv = c->push_list( (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );

        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            c->declare( rv.r + (int)i, local->name );
        }
    }
    else
    {
        std::vector< yl_compile_script::lvalue > lv;
        lv.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lv.push_back( c->push_lvalue( node->lvalues.at( i ) ) );
        }
        
        auto rv = c->push_list( (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );
     
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            c->assign( lv.at( i ), rv.r + (int)i );
        }
        
        c->pop_list( rv );
        for ( auto i = lv.rbegin(); i != lv.rend(); ++i )
        {
            c->pop_lvalue( *i );
        }
    }
    
    visit( node->body );
    
    int entry_label = c->label();
    c->patch( entry_jump, entry_label );
    
    c->patch( c->jump( Y_JMPITER, i ), continue_label );
    c->close_continue( node->scope, continue_label );
    
    int break_label = c->label();
    c->close_break( node->scope, break_label );

    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_for* node )
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
        c->pop( c->push( node->init ) );
    }
    
    int top_label = c->label();
    c->open_break( node->scope );
    c->open_continue( node->scope );
    
    int test_jump = -1;
    if ( node->condition )
    {
        int v = c->push( node->condition );
        c->pop( v );
        test_jump = c->jump( Y_JMPF, v );
    }
    
    visit( node->body );
    
    int continue_label = c->label();
    c->close_continue( node->scope, continue_label );

    if ( node->update )
    {
        c->pop( c->push( node->update ) );
    }
    
    c->patch( c->jump( Y_JMP, 0 ), top_label );
    
    int break_label = c->label();
    if ( test_jump != -1 )
    {
        c->patch( test_jump, break_label );
    }
    c->close_break( node->scope, break_label );
    
    c->undeclare( node->scope );
}

void yl_compile_statement::visit( yl_stmt_using* node )
{
}

void yl_compile_statement::visit( yl_stmt_try* node )
{
}

void yl_compile_statement::visit( yl_stmt_catch* node )
{
    assert( ! "catch outside try" );
}

void yl_compile_statement::visit( yl_stmt_delete* node )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        yl_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == YL_EXPR_KEY )
        {
            yl_expr_key* expr = (yl_expr_key*)node;
            int v = c->push( expr->object );
            c->pop( v );
            c->op( Y_DELKEY, 0, v, c->key( expr ) );
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            int v = c->push( expr->object );
            int u = c->push( expr->key );
            c->pop( u );
            c->pop( v );
            c->op( Y_DELINKEY, 0, v, u );
        }
    }
}

void yl_compile_statement::visit( yl_stmt_case* node )
{
    assert( ! "case outside switch" );
}

void yl_compile_statement::visit( yl_stmt_continue* node )
{
    c->add_continue( node->target );
}

void yl_compile_statement::visit( yl_stmt_break* node )
{
    c->add_break( node->target );
}

void yl_compile_statement::visit( yl_stmt_return* node )
{
    auto lv = c->push_list( node->values, -1 );
    c->pop_list( lv );
    c->op( Y_RETURN, 0, lv.r, lv.count );
}

void yl_compile_statement::visit( yl_stmt_throw* node )
{
    int v = c->push( node->value );
    c->pop( v );
    c->op( Y_THROW, 0, v, 0 );
}











yl_compile_script::yl_compile_script( yl_build_script* s )
    :   compile_statement( this )
    ,   compile_expression( this )
    ,   s( s )
{
}




void yl_compile_script::op( y_opcode op, unsigned r, unsigned a, unsigned b )
{
    s->code.emplace_back( op, r, a, b );
}

void yl_compile_script::op( y_opcode op, unsigned r, unsigned c )
{
    s->code.emplace_back( op, r, c );
}

void yl_compile_script::op( y_opcode op, unsigned r, signed j )
{
    s->code.emplace_back( op, r, j );
}



int yl_compile_script::label()
{
    return (int)s->code.size();
}

int yl_compile_script::jump( y_opcode opcode, unsigned r )
{
    s->code.emplace_back( opcode, r, (signed)0 );
    return (int)s->code.size();
}

void yl_compile_script::patch( int jump, int label )
{
    y_opinst& j = s->code.at( jump - 1 );
    j = y_opinst( j.opcode(), j.r(), (signed)( label - jump ) );
}





void yl_compile_script::open_break( yl_ast_scope* target )
{
    break_stack.emplace_back( target );
}

void yl_compile_script::add_break( yl_ast_scope* target )
{
    branch& branch_break = break_stack.back();
    assert( branch_break.target == target );
    branch_break.jumps.push_back( jump( Y_JMP, 0 ) );
}

void yl_compile_script::close_break( yl_ast_scope* target, int label )
{
    branch& branch_break = break_stack.back();
    assert( branch_break.target == target );
    for ( size_t i = 0; i < branch_break.jumps.size(); ++i )
    {
        patch( branch_break.jumps.at( i ), label );
    }
    break_stack.pop_back();
}

void yl_compile_script::open_continue( yl_ast_scope* target )
{
    continue_stack.emplace_back( target );
}

void yl_compile_script::add_continue( yl_ast_scope* target )
{
    branch& branch_continue = continue_stack.back();
    assert( branch_continue.target == target );
    branch_continue.jumps.push_back( jump( Y_JMP, 0 ) );
}

void yl_compile_script::close_continue( yl_ast_scope* target, int label )
{
    branch& branch_continue = continue_stack.back();
    assert( branch_continue.target == target );
    for ( size_t i = 0; i < branch_continue.jumps.size(); ++i )
    {
        patch( branch_continue.jumps.at( i ), label );
    }
    continue_stack.pop_back();
}












