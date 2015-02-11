//
//  yl_compile_visitor.cpp
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_compile_visitor.h"
#include <assert.h>
#include <unordered_map>





int yl_compile_visitor::fallback( yl_ast_node* node, int count )
{
    assert( ! "invalid AST node" );
    return 0;
}


int yl_compile_visitor::visit( yl_stmt_block* node, int count )
{
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        execute( node->stmts.at( i ) );
    }
    
    if ( node->scope )
    {
        undeclare( node->scope );
    }
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_if* node, int count )
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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_switch* node, int count )
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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_while* node, int count )
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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_do* node, int count )
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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_foreach* node, int count )
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
        
        switch ( rv.count )
        {
        case 1:
            op( Y_NEXT1, rv.r, 0, 0 );
            break;
            
        case 2:
            op( Y_NEXT2, rv.r, rv.r + 1, 0 );
            break;
            
        default:
            op( Y_NEXT, rv.r, rv.count, 0 );
            break;
        }

        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            declare( rv.r + (int)i, local->name );
        }
    }
    else
    {
        std::vector< lvalue > lv;
        lv.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lv.push_back( push_lvalue( node->lvalues.at( i ) ) );
        }
        
        auto rv = push_list( (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );
     
        switch ( rv.count )
        {
        case 1:
            op( Y_NEXT1, rv.r, 0, 0 );
            break;
            
        case 2:
            op( Y_NEXT2, rv.r, rv.r + 1, 0 );
            break;
            
        default:
            op( Y_NEXT, rv.r, rv.count, 0 );
            break;
        }

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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_for* node, int count )
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
        unsigned v = push( node->condition );
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
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_using* node, int count )
{
    // ...
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_try* node, int count )
{
    // ...
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_catch* node, int count )
{
    assert( ! "catch outside try" );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_delete* node, int count )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        yl_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == YL_EXPR_KEY )
        {
            yl_expr_key* expr = (yl_expr_key*)node;
            unsigned v = push( expr->object );
            pop( v );
            op( Y_DELKEY, 0, v, key( expr->key ) );
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            unsigned v = push( expr->object );
            unsigned u = push( expr->key );
            pop( u );
            pop( v );
            op( Y_DELINKEY, 0, v, u );
        }
    }
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_case* node, int count )
{
    assert( ! "case outside switch" );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_continue* node, int count )
{
    add_continue( node->target );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_break* node, int count )
{
    add_break( node->target );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_return* node, int count )
{
    auto lv = push_list( node->values, -1 );
    pop_list( lv );
    op( Y_RETURN, 0, lv.r, lv.count );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_throw* node, int count )
{
    unsigned v = push( node->value );
    pop( v );
    op( Y_THROW, 0, v, 0 );
    return 0;
}



int yl_compile_visitor::visit( yl_ast_func* node, int count )
{
    // hmmm ...
    return 0;
}

int yl_compile_visitor::visit( yl_expr_null* node, int count )
{
    unsigned r = push();
    op( Y_NULL, r, 1 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_bool* node, int count )
{
    unsigned r = push();
    op( Y_BOOL, r, node->value ? 1 : 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_number* node, int count )
{
    unsigned r = push();
    op( Y_NUMBER, r, number( node->value ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_string* node, int count )
{
    unsigned r = push();
    op( Y_STRING, r, string( node->string, node->length ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_local* node, int count )
{
    unsigned r = push();
    op( Y_MOV, r, local( node->name ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_global* node, int count )
{
    unsigned r = push();
    op( Y_GLOBAL, r, 0, key( node->name ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_upref* node, int count )
{
    unsigned r = push();
    op( Y_GETUP, r, (unsigned)node->index );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_objref* node, int count )
{
    unsigned r = push();
    op( Y_MOV, r, objref( node->object ), 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_superof* node, int count )
{
    unsigned o = push( node->expr );
    pop( o );
    unsigned r = push();
    op( Y_SUPER, r, o, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_key* node, int count )
{
    unsigned o = push( node->object );
    pop( o );
    unsigned r = push();
    op( Y_KEY, r, o, key( node->key ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_inkey* node, int count )
{
    unsigned o = push( node->object );
    unsigned k = push( node->key );
    pop( k );
    pop( o );
    unsigned r = push();
    op( Y_INKEY, r, o, k );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_index* node, int count )
{
    unsigned o = push( node->object );
    unsigned i = push( node->index );
    pop( i );
    pop( o );
    unsigned r = push();
    op( Y_INDEX, r, o, i );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_preop* node, int count )
{
    lvalue lv = push_lvalue( node->lvalue );    // lv
    unsigned v = push_evaluate_lvalue( lv );    // lv, v
    unsigned n = push();                        // lv, v, 1
    op( Y_NUMBER, n, number( 1.0 ) );
    pop( n );
    pop( v );
    unsigned w = push();                        // lv, w
    op( node->opkind == YL_ASTOP_PREINC ? Y_ADD : Y_SUB, w, v, n );
    assign( lv, w );                            // *lv = w
    pop( w );
    pop_lvalue( lv );
    unsigned r = push();
    op( Y_MOV, r, w );                          // w
    return 1;
}

int yl_compile_visitor::visit( yl_expr_postop* node, int count )
{
    lvalue lv = push_lvalue( node->lvalue );    // lv
    unsigned v = push_evaluate_lvalue( lv );    // lv, v
    unsigned u = push();                        // lv, v, u
    op( Y_MOV, u, v, 0 );
    unsigned n = push();                        // lv, v, u, 1
    op( Y_NUMBER, n, number( 1.0 ) );
    pop( n );
    pop( u );
    unsigned w = push();                        // lv, v, w
    op( node->opkind == YL_ASTOP_POSTINC ? Y_ADD : Y_SUB, w, u, n );
    assign( lv, w );                            // *lv = w
    pop( w );
    pop( v );
    pop_lvalue( lv );
    unsigned r = push();
    op( Y_MOV, r, v );                          // v
    return 1;
}

int yl_compile_visitor::visit( yl_expr_unary* node, int count )
{
    unsigned v = push( node->operand );
    pop( v );
    unsigned r = push();
    switch ( node->opkind )
    {
    case YL_ASTOP_POSITIVE:
        op( Y_NEG, r, v, 0 );
        op( Y_NEG, r, r, 0 );
        break;
        
    case YL_ASTOP_NEGATIVE:
        op( Y_NEG, r, v, 0 );
        break;
    
    case YL_ASTOP_LOGICNOT:
        op( Y_LNOT, r, v, 0 );
        break;

    case YL_ASTOP_BITNOT:
        op( Y_BITNOT, r, v, 0 );
        break;
        
    default:
        assert( ! "invalid unary operator" );
        break;
    }
    return 1;
}

int yl_compile_visitor::visit( yl_expr_binary* node, int count )
{
    unsigned a = push( node->lhs );
    unsigned b = push( node->rhs );
    pop( b );
    pop( a );
    unsigned r = push();
    switch ( node->opkind )
    {
    case YL_ASTOP_MULTIPLY:
        op( Y_MUL, r, a, b );
        break;
        
    case YL_ASTOP_DIVIDE:
        op( Y_DIV, r, a, b );
        break;
        
    case YL_ASTOP_MODULUS:
        op( Y_MOD, r, a, b );
        break;
        
    case YL_ASTOP_INTDIV:
        op( Y_INTDIV, r, a, b );
        break;
        
    case YL_ASTOP_ADD:
        op( Y_ADD, r, a, b );
        break;
        
    case YL_ASTOP_SUBTRACT:
        op( Y_SUB, r, a, b );
        break;
        
    case YL_ASTOP_LSHIFT:
        op( Y_LSL, r, a, b );
        break;
        
    case YL_ASTOP_LRSHIFT:
        op( Y_LSR, r, a, b );
        break;
        
    case YL_ASTOP_ARSHIFT:
        op( Y_ASR, r, a, b );
        break;
        
    case YL_ASTOP_BITAND:
        op( Y_BITAND, r, a, b );
        break;
        
    case YL_ASTOP_BITXOR:
        op( Y_BITXOR, r, a, b );
        break;
        
    case YL_ASTOP_BITOR:
        op( Y_BITOR, r, a, b );
        break;
        
    case YL_ASTOP_CONCATENATE:
        op( Y_CONCAT, r, a, b );
        break;
        
    default:
        assert( ! "invalid binary operator" );
        break;
    }
    return 1;
}

int yl_compile_visitor::visit( yl_expr_compare* node, int count )
{
    unsigned a = push( node->first );           // a
    
    std::vector< int > shortcut_jumps;
    shortcut_jumps.reserve( node->opkinds.size() - 1 );
    
    size_t last = node->opkinds.size() - 1;
    for ( size_t i = 0; i < last; ++i )
    {
        yl_ast_node* term = node->terms.at( i );
        
        unsigned b = push( term );              // a, b
        pop( b );
        pop( a );
        unsigned r = push();                    // r, [b]
        compare_op( node->opkinds.at( i ), r, a, b );
        pop( r );
        shortcut_jumps.push_back( jump( Y_JMPF, r ) );
        unsigned a = push();                    // a
        op( Y_MOV, a, b, 0 );
    }
    
    unsigned b = push( node->terms.at( last ) );
    pop( b );
    pop( a );
    unsigned r = push();
    compare_op( node->opkinds.at( last ), r, a, b );

    int shortcut_label = label();
    for ( size_t i = 0; i < shortcut_jumps.size(); ++i )
    {
        patch( shortcut_jumps.at( i ), shortcut_label );
    }
    
    return 1;
}

int yl_compile_visitor::visit( yl_expr_logical* node, int count )
{
    switch ( node->opkind )
    {
    case YL_ASTOP_LOGICAND:
    {
        unsigned a = push( node->lhs );         // a
        int shortcut_jump = jump( Y_JMPF, a );
        unsigned b = push( node->rhs );         // a, b
        pop( b );
        pop( a );
        unsigned r = push();                    // r, [b]
        op( Y_MOV, r, b, 0 );
        patch( shortcut_jump, label() );
        break;
    }
    
    case YL_ASTOP_LOGICXOR:
    {
        unsigned a = push( node->lhs );
        unsigned b = push( node->rhs );
        pop( b );
        pop( a );
        unsigned r = push();
        op( Y_LXOR, r, a, b );
        break;
    }
    
    case YL_ASTOP_LOGICOR:
    {
        unsigned a = push( node->lhs );         // a
        int shortcut_jump = jump( Y_JMPT, a );
        unsigned b = push( node->rhs );         // a, b
        pop( b );
        pop( a );
        unsigned r = push();                    // r, [b]
        op( Y_MOV, r, b, 0 );
        patch( shortcut_jump, label() );
        break;
    }
    
    default:
        assert( ! "invalid logical operator" );
        break;
    }

    return 1;
}

int yl_compile_visitor::visit( yl_expr_qmark* node, int count )
{
    unsigned v = push( node->condition );
    pop( v );
    int test_jump = jump( Y_JMPF, v );
    unsigned y = push( node->iftrue );
    int endif_jump = jump( Y_JMP, 0 );
    pop( y );
    patch( test_jump, label() );
    push( node->iffalse );
    patch( endif_jump, label() );
    return 1;
}

int yl_compile_visitor::visit( yl_new_new* node, int count )
{
    unsigned p = push( node->proto );           // p
    pop( p );
    unsigned o = push();                        // o
    op( Y_OBJECT, o, p, 0 );
    unsigned m = push();                        // o, m
    op( Y_KEY, m, o, key( "this" ) );
    unsigned t = push();                        // o, m, o
    op( Y_MOV, t, o, 0 );
    listval l = push_list( node->arguments, -1 ); // o, m, o, ...
    op( Y_CALL, m, l.count != -1 ? l.count + 2 : -1, 0 );
    pop_list( l );
    pop( t );
    pop( m );                                   // o
    return 1;
}

int yl_compile_visitor::visit( yl_new_object* node, int count )
{
    unsigned p = push( node->proto );
    pop( p );
    unsigned o = push();
    op( Y_OBJECT, o, p, 0 );
    declare_object( o, node );
    
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        execute( node->members.at( i ) );
    }
    
    undeclare( node->scope );
    undeclare_object( node );
    return 1;
}

int yl_compile_visitor::visit( yl_new_array* node, int count )
{
    unsigned r = push();
    op( Y_ARRAY, r, (unsigned)node->values.size() );
    
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        unsigned v = push( node->values.at( i ) );
        pop( v );
        op( Y_APPEND, r, v, 0 );
    }
    
    if ( node->final )
    {
        listval l = push_list( node->final, -1 );
        pop_list( l );
        op( Y_EXTEND, r, l.count != -1 ? l.count : -1, 0 );
    }
    
    return 1;
}

int yl_compile_visitor::visit( yl_new_table* node, int count )
{
    unsigned r = push();
    op( Y_TABLE, r, (unsigned)node->elements.size() );
    
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const yl_key_value& kv = node->elements.at( i );
        unsigned k = push( kv.key );
        unsigned v = push( kv.value );
        pop( v );
        pop( k );
        op( Y_SETINDEX, r, k, v );
    }
    
    return 1;
}

int yl_compile_visitor::visit( yl_expr_mono* node, int count )
{
    push( node->expr );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_call* node, int count )
{
    if ( ! node->unpack && count != 0 )
    {
        count = 1;
    }
    
    int f = -1;
    int t = -1;
    switch ( node->function->kind )
    {
    case YL_EXPR_KEY:
    {
        yl_expr_key* kexpr = (yl_expr_key*)node->function;
        unsigned o = push( kexpr->object );     // o
        pop( o );
        f = push();
        t = push();
        op( Y_METHOD, f, o, key( kexpr->key ) ); // m, o
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        yl_expr_inkey* inkey = (yl_expr_inkey*)node->function;
        unsigned o = push( inkey->object );     // o
        unsigned k = push( inkey->key );        // o, k
        pop( k );
        pop( o );
        f = push();
        t = push();
        op( Y_INKEY, t, o, k );                 // o, m
        op( Y_SWP, f, t, 0 );                   // m, o
        break;
    }
    
    default:
        f = push( node->function );
        break;
    }
    
    listval l = push_list( -1 );                // f, [o], ...
    pop_list( l );
    if ( t != -1 )
    {
        pop( t );
    }
    pop( f );

    listval r = push_list( count );
    assert( l.r == r.r );
    
    unsigned argcount = l.count != -1 ? ( l.count + ( t != -1 ? 1 : 0 ) ) : -1;
    y_opcode opcode = node->yieldcall ? Y_YCALL : Y_CALL;
    op( opcode, l.r, argcount, count );

    return count;
}

int yl_compile_visitor::visit( yl_expr_yield* node, int count )
{
    if ( ! node->unpack && count != 0 )
    {
        count = 1;
    }

    listval l = push_list( node->arguments, -1 );
    pop_list( l );
    listval r = push_list( count );
    assert( l.r == r.r );

    op( Y_YIELD, l.r, l.count != -1 ? l.count : -1, (unsigned)count );

    return count;
}

int yl_compile_visitor::visit( yl_expr_vararg* node, int count )
{
    if ( count != 0 )
    {
        listval r = push_list( count );
        op( Y_VARARG, r.r, r.count, 0 );
    }
    return count;
}

int yl_compile_visitor::visit( yl_expr_unpack* node, int count )
{
    if ( count != 0 )
    {
        unsigned v = push( node->array );
        pop( v );
        listval r = push_list( count );
        op( Y_UNPACK, v, r.r, r.count );
    }
    return count;
}

int yl_compile_visitor::visit( yl_expr_list* node, int count )
{
    int n = (int)node->values.size();
    if ( count != -1 )
    {
        n = std::min( n, count );
    }
    
    for ( size_t i = 0; i < n; ++i )
    {
        push( node->values.at( i ) );
    }
    
    for ( size_t i = n; i < node->values.size(); ++i )
    {
        execute( node->values.at( i ) );
    }

    if ( node->final )
    {
        if ( count == -1 )
        {
            push_list( node->final, -1 );
        }
        else if ( count > n )
        {
            push_list( node->final, count - n );
        }
        else
        {
            execute( node->final );
        }
        
        return count;
    }
    
    return n;
}

int yl_compile_visitor::visit( yl_expr_assign* node, int count )
{
    lvalue lv = push_lvalue( node->lvalue );    // lv
    unsigned v = push( node->rvalue );          // lv, v
    assign( lv, v );                            // *lv = v
    pop( v );
    pop_lvalue( lv );
    unsigned r = push();
    op( Y_MOV, r, v, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_assign_list* node, int count )
{
    if ( count != -1 )
    {
        count = std::min( count, (int)node->lvalues.size() );
    }
    else
    {
        count = (int)node->lvalues.size();
    }

    std::vector< lvalue > lv;
    lv.reserve( node->lvalues.size() );
    
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        lv.push_back( push_lvalue( node->lvalues.at( i ) ) );
    }
                                                // lv, lv, lv
    
    auto rv = push_list( node->rvalues, (int)node->lvalues.size() );
    assert( rv.count == (int)node->lvalues.size() );
                                                // lv, lv, lv, v, v, v
 
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        assign( lv.at( i ), rv.r + (int)i );
    }
    
    pop_list( rv );
    for ( auto i = lv.rbegin(); i != lv.rend(); ++i )
    {
        pop_lvalue( *i );
    }
    
    auto r = push_list( count );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
    {
        op( Y_MOV, r.r + (unsigned)i, rv.r + (unsigned)i, 0 );
    }
    
    return count;
}








void yl_compile_visitor::compare_op(
                yl_ast_opkind opkind, unsigned r, unsigned a, unsigned b )
{
    switch ( opkind )
    {
    case YL_ASTOP_EQUAL:
        op( Y_EQ, r, a, b );
        break;
        
    case YL_ASTOP_NOTEQUAL:
        op( Y_NE, r, a, b );
        break;
        
    case YL_ASTOP_LESS:
        op( Y_LT, r, a, b );
        break;
        
    case YL_ASTOP_GREATER:
        op( Y_GT, r, a, b );
        break;
        
    case YL_ASTOP_LESSEQUAL:
        op( Y_LE, r, a, b );
        break;
        
    case YL_ASTOP_GREATEREQUAL:
        op( Y_GE, r, a, b );
        break;
    
    case YL_ASTOP_IN:
        op( Y_IN, r, a, b );
        break;
        
    case YL_ASTOP_NOTIN:
        op( Y_IN, r, a, b );
        op( Y_LNOT, r, r, 0 );
        break;
    
    case YL_ASTOP_IS:
        op( Y_IS, r, a, b );
        break;
        
    case YL_ASTOP_NOTIS:
        op( Y_IS, r, a, b );
        op( Y_LNOT, r, r, 0 );
        break;
    
    default:
        assert( ! "unexpected comparison operator" );
        break;
    }
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












