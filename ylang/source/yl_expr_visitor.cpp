//
//  yl_expr_visitor.cpp
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_expr_visitor.h"
#include <assert.h>
#include <unordered_map>



yl_compile_statement::yl_compile_statement( yl_compile_script* c )
{
}


void yl_compile_statement::fallback( yl_ast_node* node )
{
    assert( ! "expected statement" );
}


void yl_compile_statement::visit( yl_stmt_block* node )
{
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        visit( node->stmts.at( i ) );
    }

    if ( node->scope )
    {
        c->close_scope( node->scope );
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

    int v = c->push( node->condition );
    int test_jump = c->jump( Y_JMPF, v );
    c->pop( v );
    
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
    
    c->close_scope( node->scope );
}

void yl_compile_statement::visit( yl_stmt_switch* node )
{
    /*
        It's possible that a case expression could clobber the comparison
        value.  Unless we can disprove this, we must keep a copy of the
        value on the stack during the case sequence.
    */
    
    int u = c->push( node->value );
    
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
        
        int v = c->push( cstmt->value );
        c->code.emplace_back( Y_EQ, v, u, v );
        case_jumps.emplace( cstmt, c->jump( Y_JMPT, v ) );
        c->pop( v );
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
        c->close_scope( node->body->scope );
    }
    c->close_scope( node->scope );
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

    int v = c->push( node->condition );
    int test_jump = c->jump( Y_JMPF, v );
    c->pop( v );
    
    visit( node->body );
    
    c->close_continue( node->scope, continue_label );
    c->patch( c->jump( Y_JMP, 0 ), continue_label );
    
    int break_label = c->label();
    c->close_break( node->scope, break_label );
    c->patch( test_jump, break_label );
    
    c->close_scope( node->scope );
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
    
    int v = c->push( node->condition );
    c->patch( c->jump( Y_JMPT, v ), top_label );
    c->pop( v );
    
    int break_label = c->label();
    c->close_break( node->scope, break_label );
}

void yl_compile_statement::visit( yl_stmt_foreach* node )
{
}

void yl_compile_statement::visit( yl_stmt_for* node )
{
}

void yl_compile_statement::visit( yl_stmt_using* node )
{
}

void yl_compile_statement::visit( yl_stmt_try* node )
{
}

void yl_compile_statement::visit( yl_stmt_catch* node )
{
}

void yl_compile_statement::visit( yl_stmt_delete* node )
{
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
}

void yl_compile_statement::visit( yl_stmt_throw* node )
{
}










int yl_compile_script::label()
{
    return (int)code.size();
}

int yl_compile_script::jump( y_opcode opcode, unsigned r )
{
    code.emplace_back( opcode, r, 0 );
    return (int)code.size();
}

void yl_compile_script::patch( int jump, int label )
{
    y_opinst& j = code.at( jump - 1 );
    j = y_opinst( j.opcode(), j.r(), label - jump );
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











#if 0


void yl_clobber_analysis::analyse( yl_ast_func* func, yl_ast_node* expr )
{
}


bool yl_clobber_analysis::is_clobbered( yl_ast_node* load )
{
    return clobbered.find( load ) != clobbered.end();
}


void yl_clobber_analysis::fallback( yl_ast_node* node )
{
    assert( ! "expected expression" );
}


void yl_clobber_analysis::visit( yl_ast_func* node )
{
}

void yl_clobber_analysis::visit( yl_expr_null* node )
{
}

void yl_clobber_analysis::visit( yl_expr_bool* node )
{
}

void yl_clobber_analysis::visit( yl_expr_number* node )
{
}

void yl_clobber_analysis::visit( yl_expr_string* node )
{
}

void yl_clobber_analysis::visit( yl_expr_local* node )
{
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_global* node )
{
}

void yl_clobber_analysis::visit( yl_expr_upref* node )
{
    yl_ast_upval& uv = func->upvals.at( node->index );
    if ( uv.kind == YL_UPVAL_LOCAL )
    {
        push( node );
    }
}

void yl_clobber_analysis::visit( yl_expr_objref* node )
{
}

void yl_clobber_analysis::visit( yl_expr_superof* node )
{
    int fp = open();
    visit( node->expr );
    close( fp );
}

void yl_clobber_analysis::visit( yl_expr_key* node )
{
    int fp = open();
    visit( node->object );
    close( fp );
}

void yl_clobber_analysis::visit( yl_expr_inkey* node )
{
    int fp = open();
    visit( node->object );
    visit( node->key );
    close( fp );
}

void yl_clobber_analysis::visit( yl_expr_index* node )
{
    int fp = open();
    visit( node->index );
    visit( node->object );
    close( fp );
}

void yl_clobber_analysis::visit( yl_expr_preop* node )
{
    /*
        lval
        lval, a
        lval, a, 1
        lval, b
        lval, b
        b
    */
    
    int fp = open();
    visit( node->lvalue );
    
    
    
    

    lvalue( node->lvalue );             // lval
    lveval( node->lvalue );             // lval, a
                                        // lval, a, 1
                                        // lval, b
    repush();                           // lval, b, b
    assign_swap( node->lvalue );        // b
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_postop* node )
{
    lvalue( node->lvalue );             // lval
    lveval( node->lvalue );             // lval, a
    repush();                           // lval, a, a
                                        // lval, a, a, 1
                                        // lval, a, b
    assign_swap( node->lvalue );        // a
}

void yl_clobber_analysis::visit( yl_expr_unary* node )
{
    visit( node->operand );
    pop( 1 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_binary* node )
{
    visit( node->lhs );
    visit( node->rhs );
    pop( 2 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_compare* node )
{
    /*
        Worst case for clobbering means no shortcut evaulation.
    */

    visit( node->first );               // a
    
    size_t final = node->terms.size() - 1;
    for ( size_t i = 0; i < final; ++i )
    {
        visit( node->terms.at( i ) );   // a, b
        pop_swap();                     // b
    }
    
    visit( node->terms.at( final ) );   // a, b
    pop( 2 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_logical* node )
{
    /*
        Worst case for clobbering means no shortcut evaulation.
    */

    visit( node->lhs );
    visit( node->rhs );
    pop( 2 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_qmark* node )
{
    visit( node->condition );
    pop( 1 );

    /*
        Either branch may be taken, so the state at the end of this operation
        must be the same after either branch.  So even if a result is a load
        it won't be elided anyway.  Evaluate each side individually, and
        represent the preserved result as the qmark node itself.
    */
    
    visit( node->iftrue );
    pop( 1 );
    visit( node->iffalse );
    pop( 1 );
    push( node );
}

void yl_clobber_analysis::visit( yl_new_new* node )
{
    /*
        Simplified to ignore the lookup of the contructor.
    */

    visit( node->proto );               // p
    pop( 1 );                           //
    push( node );                       // o
                                        // o, m, o
    void u = unpack( node->arguments );  // o, m, o ...
    function_call( u );
}

void yl_clobber_analysis::visit( yl_new_object* node )
{
    visit( node->proto );
    pop( 1 );
    push( node );
    
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        visit( node->members.at( i ) );
        pop( 1 );
    }
}

void yl_clobber_analysis::visit( yl_new_array* node )
{
    push( node );
    
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        visit( node->values.at( i ) );
        pop( 1 );
    }
    
    void u = unpack( node->final );
    pop( u );
}

void yl_clobber_analysis::visit( yl_new_table* node )
{
    push( node );
    
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        yl_key_value& kv = node->elements.at( i );
        visit( kv.key );
        visit( kv.value );
        pop( 2 );
    }
}

void yl_clobber_analysis::visit( yl_expr_mono* node )
{
    visit( node->expr );
}

void yl_clobber_analysis::visit( yl_expr_call* node )
{
    /*
        Ignore method lookup and pretend that calls only return one result.
    */
    
    visit( node->function );
    void u = unpack( node->arguments );
    function_call( u + 1 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_yield* node )
{
    /*
        Yields are pretty much function calls.
    */

    void u = unpack( node->arguments );
    function_call( u );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_vararg* node )
{
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_unpack* node )
{
    visit( node->array );
    pop( 1 );
    push( node );
}

void yl_clobber_analysis::visit( yl_expr_list* node )
{
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        visit( node->values.at( i ) );
    }

    void u = unpack( node->final );
    pop( (void)node->values.size() + u );
}

void yl_clobber_analysis::visit( yl_expr_assign* node )
{
    lvalue( node->lvalue );             // lval
    visit( node->rvalue );              // lval, v
    assign( node->lvalue );             // v
}

void yl_clobber_analysis::visit( yl_expr_assign_list* node )
{
    
}


/*

yl_clobber_analysis::lv yl_clobber_analysis::lvalue( yl_ast_node* node )
{
    lv lv;

    switch ( node->kind )
    {
    case YL_EXPR_LOCAL:
    case YL_EXPR_GLOBAL:
    case YL_EXPR_UPREF:
    {
        lv.value = nullptr;
        lv.index = nullptr;
        break;
    }
    
    case YL_EXPR_KEY:
    {
        yl_expr_key* keyexpr = (yl_expr_key*)node;
        visit( keyexpr->object );
        lv.value = keyexpr->object;
        lv.index = nullptr;
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        yl_expr_inkey* inkey = (yl_expr_inkey*)node;
        visit( inkey->object );
        visit( inkey->key );
        lv.value = inkey->object;
        lv.index = inkey->key;
        break;
    }
    
    case YL_EXPR_INDEX:
    {
        yl_expr_index* index = (yl_expr_index*)node;
        visit( index->object );
        visit( index->index );
        lv.value = index->object;
        lv.index = index->index;
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
    
    return lv;
}

*/




#if OLD



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





yl_expr_visitor::yl_expr_visitor( yl_builder* b )
    :   b( b )
{
}

void yl_expr_visitor::fallback( yl_ast_node* node )
{
    assert( ! "expected expression" );
}


void yl_expr_visitor::visit( yl_ast_func* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_null* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_bool* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_number* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_string* node )
{
    b->op_value( node );
}


void yl_expr_visitor::visit( yl_expr_local* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_global* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_upref* node )
{
    b->op_value( node );
}

void yl_expr_visitor::visit( yl_expr_objref* node )
{
    b->op_value( node );
}


void yl_expr_visitor::visit( yl_expr_superof* node )
{
    visit( node->expr );
    b->op_unary( Y_SUPER );
}

void yl_expr_visitor::visit( yl_expr_key* node )
{
    visit( node->object );
    b->op_lookup( node->key );
}

void yl_expr_visitor::visit( yl_expr_inkey* node )
{
    visit( node->object );
    visit( node->key );
    b->op_binary( Y_INKEY );
}

void yl_expr_visitor::visit( yl_expr_index* node )
{
    visit( node->object );
    visit( node->index );
    b->op_binary( Y_INDEX );
}

void yl_expr_visitor::visit( yl_expr_preop* node )
{
    /*
        open lvalue
        evaluate lvalue
        increment/decrement
        repush
        assign lvalue
    */
    
    b->lvalue_open( node->lvalue );
    b->lvalue_evaluate( node->lvalue );
    b->op_value_one();
    if ( node->opkind == YL_ASTOP_PREINC )
    {
        b->op_binary( Y_ADD );
    }
    else if ( node->opkind == YL_ASTOP_PREDEC )
    {
        b->op_binary( Y_SUB );
    }
    b->repush();
    b->lvalue_assign( node->lvalue );
}

void yl_expr_visitor::visit( yl_expr_postop* node )
{
    /*
        open lvalue
        evaluate lvalue
        repush
        increment/decrement
        assign lvalue
    */

    b->lvalue_open( node->lvalue );
    b->lvalue_evaluate( node->lvalue );
    b->op_value_one();
    if ( node->opkind == YL_ASTOP_POSTINC )
    {
        b->op_binary( Y_ADD );
    }
    else if ( node->opkind == YL_ASTOP_POSTDEC )
    {
        b->op_binary( Y_SUB );
    }
    b->repush();
    b->lvalue_assign( node->lvalue );
}

void yl_expr_visitor::visit( yl_expr_unary* node )
{
    visit( node->operand );
    if ( node->opkind == YL_ASTOP_POSITIVE )
    {
        b->op_unary( Y_NEG );
        b->op_unary( Y_NEG );
    }
    else if ( node->opkind == YL_ASTOP_LOGICNOT )
    {
        b->op_not();
    }
    else
    {
        b->op_unary( yl_astop_to_opcode( node->opkind ) );
    }
}

void yl_expr_visitor::visit( yl_expr_binary* node )
{
    visit( node->lhs );
    visit( node->rhs );
    b->op_binary( yl_astop_to_opcode( node->opkind ) );
}

void yl_expr_visitor::visit( yl_expr_compare* node )
{
    /*
        push first
        push next
        chain compare
        if_true
            push next
            compare
        if_end
    */
    
    visit( node->first );

    size_t final = node->opkinds.size() - 1;
    for ( size_t i = 0; i < final; ++i )
    {
        visit( node->terms.at( i ) );
        yl_ast_opkind opkind = node->opkinds.at( i );
        if ( opkind == YL_ASTOP_NOTIN )
        {
            b->if_chain_compare( Y_IN, false );
            
        }
        else if ( opkind == YL_ASTOP_NOTIS )
        {
            b->if_chain_compare( Y_IS, false );
        }
        else
        {
            b->if_chain_compare( yl_astop_to_opcode( opkind ), true );
        }
    }
    
    visit( node->terms.at( final ) );
    yl_ast_opkind opkind = node->opkinds.at( final );
    if ( opkind == YL_ASTOP_NOTIN )
    {
        b->op_binary( Y_IN );
        b->op_not();
    }
    else if ( opkind == YL_ASTOP_NOTIS )
    {
        b->op_binary( Y_IS );
        b->op_not();
    }
    else
    {
        b->op_binary( yl_astop_to_opcode( opkind ) );
    }

    for ( size_t i = 0; i < final; ++i )
    {
        b->if_end();
    }

}

void yl_expr_visitor::visit( yl_expr_logical* node )
{
    switch ( node->opkind )
    {
    case YL_ASTOP_LOGICAND:
        /*
            push lhs
            if_true
                push rhs
            end
        */
        
        visit( node->lhs );
        b->if_true();
        visit( node->rhs );
        b->if_end();
        break;
    
    case YL_ASTOP_LOGICOR:
        /*
            push lhs
            if_false
                push rhs
            end
        */
        
        visit( node->lhs );
        b->if_false();
        visit( node->rhs );
        b->if_end();
        break;
    
    case YL_ASTOP_LOGICXOR:
        visit( node->lhs );
        visit( node->rhs );
        b->op_binary( Y_LXOR );
        break;
        
    default:
        assert( ! "invalid logical operator" );
        break;
    }
}

void yl_expr_visitor::visit( yl_expr_qmark* node )
{
    /*
        push condition.
        if_true
            push true result.
        if_else
            push false result.
        if_end
    */
    
    visit( node->condition );
    b->if_true();
    visit( node->iftrue );
    b->if_else();
    visit( node->iffalse );
    b->if_end();
}

void yl_expr_visitor::visit( yl_new_new* node )
{
    /*
        prototype       ..., p
        create object   ..., o
        repush          ..., o, o
        method lookup   ..., o, m, o
        arguments       ..., o, m, o, ...
        call            ..., o
    */
    
    visit( node->proto );
    b->op_unary( Y_OBJECT );
    b->repush();
    b->repush();
    b->op_lookup( "this" );
    b->swap();
    void count = unpack( node->arguments, -1 );
    b->op( Y_CALL, count != -1 ? count + 2 : -1, 0 );
}

void yl_expr_visitor::visit( yl_new_object* node )
{
    if ( node->proto )
    {
        visit( node->proto );
    }
    else
    {
        b->op_value_null();
    }
    b->op_unary( Y_OBJECT );
    
    // Object members are represented in the AST as assignment expressions.
    b->open_object( node );
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        visit( node->members.at( i ) );
    }
    b->close_object( node );
}

void yl_expr_visitor::visit( yl_new_array* node )
{
    b->op_value( node );
    
}

void yl_expr_visitor::visit( yl_new_table* node )
{
}

void yl_expr_visitor::visit( yl_expr_mono* node )
{
}

void yl_expr_visitor::visit( yl_expr_call* node )
{
}

void yl_expr_visitor::visit( yl_expr_yield* node )
{
}

void yl_expr_visitor::visit( yl_expr_vararg* node )
{
}

void yl_expr_visitor::visit( yl_expr_unpack* node )
{
}

void yl_expr_visitor::visit( yl_expr_list* node )
{
}

void yl_expr_visitor::visit( yl_expr_assign* node )
{
}

void yl_expr_visitor::visit( yl_expr_assign_list* node )
{
}




#endif


#endif




