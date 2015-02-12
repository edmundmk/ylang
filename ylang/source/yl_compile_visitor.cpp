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
    int test_jump = jump( node->sloc, Y_JMPF, v );
    
    if ( node->iftrue )
    {
        execute( node->iftrue );
    }
    if ( node->iffalse )
    {
        int endif_jump = jump( node->sloc, Y_JMP, 0 );
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
        op( node->sloc, Y_EQ, w, u, v );
        pop( w );
        case_jumps.emplace( cstmt, jump( node->sloc, Y_JMPT, w ) );
    }
    
    pop( u );
    
    int default_jump = jump( node->sloc, Y_JMP, 0 );
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
    int test_jump = jump( node->sloc, Y_JMPF, v );
    
    execute( node->body );
    
    close_continue( node->scope, continue_label );
    patch( jump( node->sloc, Y_JMP, 0 ), continue_label );
    
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
    patch( jump( node->sloc, Y_JMPT, v ), top_label );
    
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
    op( node->sloc, node->eachkey ? Y_ITERKEY : Y_ITER, i, l, 0 );
    
    int entry_jump = jump( node->sloc, Y_JMP, 0 );
    
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
            op( node->sloc, Y_NEXT1, rv.r, 0, 0 );
            break;
            
        case 2:
            op( node->sloc, Y_NEXT2, rv.r, rv.r + 1, 0 );
            break;
            
        default:
            op( node->sloc, Y_NEXT, rv.r, rv.count, 0 );
            break;
        }

        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            declare
            (
                rv.r + (unsigned)i,
                local->name->upval,
                local->name,
                local->name->name
            );
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
            op( node->sloc, Y_NEXT1, rv.r, 0, i );
            break;
            
        case 2:
            op( node->sloc, Y_NEXT2, rv.r, rv.r + 1, i );
            break;
            
        default:
            op( node->sloc, Y_NEXT, rv.r, rv.count, i );
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
    
    patch( jump( node->sloc, Y_JMPITER, i ), continue_label );
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
        test_jump = jump( node->sloc, Y_JMPF, v );
    }
    
    execute( node->body );
    
    int continue_label = label();
    close_continue( node->scope, continue_label );

    if ( node->update )
    {
        pop( push( node->update ) );
    }
    
    patch( jump( node->sloc, Y_JMP, 0 ), top_label );
    
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
    /*
            o.acquire();
        [
            using block
        ]
        using:
            o.release();
            unwind
    */

    unsigned o = push( node->uvalue );          // o
    declare( o, false, node, "[using]" );
    unsigned m = push();                        // o, m
    unsigned t = push();                        // o, m, t
    op( node->sloc, Y_METHOD, m, o, key( "acquire" ) );
    pop( t );
    pop( m );                                   // o
    op( node->sloc, Y_CALL, m, 2, 0 );
    
    open_xframe();
    execute( node->body );
    int using_xframe = close_xframe();
    int using_label = label();
    xframe_handler( using_xframe, using_label );
    
    undeclare( node );                          // [o]
    m = push();
    t = push();
    op( node->sloc, Y_METHOD, m, o, key( "release" ) );
    pop( t );
    pop( m );
    op( node->sloc, Y_CALL, m, 2, 0 );
   
    op( node->sloc, Y_UNWIND, 0, 0, 0 );
    
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_try* node, int count )
{
    /*
        [
        [
            tstmt
            goto finally
        ]
        catch:
            filter
            if ( failed ) goto next_catch
            catch
            goto finally
        next_catch:
            filter
            if ( failed ) goto finally
            catch
            goto finally
        ]
        finally:
            finally statement
            unwind
     
    */


    if ( node->fstmt )
    {
        open_xframe();
    }
    
    if ( node->clist.size() )
    {
        open_xframe();
    }
    
    execute( node->tstmt );
    
    std::vector< int > finally_jumps;
    if ( node->clist.size() )
    {
        finally_jumps.push_back( jump( node->sloc, Y_JMP, 0 ) );
        int catch_xframe = close_xframe();
        int catch_label = label();
        xframe_handler( catch_xframe, catch_label );
        
        int catch_jump = -1;
        for ( size_t i = 0; i < node->clist.size(); ++i )
        {
            assert( node->clist.at( i )->kind == YL_STMT_CATCH );
            yl_stmt_catch* cstmt = (yl_stmt_catch*)node->clist.at( i );
        
            if ( catch_jump != -1 )
            {
                patch( catch_jump, label() );
            }
            
            // Test catch filter.
            if ( cstmt->proto )
            {
                unsigned e = push();
                op( cstmt->sloc, Y_EXCEPT, e, 0, 0 );
                unsigned p = push( cstmt->proto );
                pop( p );
                pop( e );
                unsigned r = push();
                op( cstmt->sloc, Y_IS, r, e, p );
                pop( r );
                catch_jump = jump( cstmt->sloc, Y_JMPF, r );
            }
            
            // Make exception assignment.
            if ( cstmt->lvalue && cstmt->declare )
            {
                lvalue lv = push_lvalue( cstmt->lvalue );
                unsigned e = push();
                op( cstmt->sloc, Y_EXCEPT, e, 0, 0 );
                assign( lv, e );
                pop( e );
                pop_lvalue( lv );
            }
            else if ( cstmt->lvalue )
            {
                assert( cstmt->lvalue->kind == YL_EXPR_LOCAL );
                yl_expr_local* local = (yl_expr_local*)cstmt->lvalue;
                unsigned e = push();
                op( cstmt->sloc, Y_EXCEPT, e, 0, 0 );
                declare
                (
                    e,
                    local->name->upval,
                    local->name,
                    local->name->name
                );
            }
            
            execute( cstmt->body );
            
            finally_jumps.push_back( jump( node->sloc, Y_JMP, 0 ) );
        }
        
        if ( catch_jump != -1 )
        {
            finally_jumps.push_back( catch_jump );
        }
        
    }
    
    int finally_xframe = close_xframe();
    int finally_label = label();
    for ( size_t i = 0; i < finally_jumps.size(); ++i )
    {
        patch( finally_jumps.at( i ), finally_label );
    }
    
    if ( node->fstmt )
    {
        xframe_handler( finally_xframe, finally_label );
        execute( node->fstmt );
        op( node->sloc, Y_UNWIND, 0, 0, 0 );
    }
    
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
            op( node->sloc, Y_DELKEY, 0, v, key( expr->key ) );
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            unsigned v = push( expr->object );
            unsigned u = push( expr->key );
            pop( u );
            pop( v );
            op( node->sloc, Y_DELINKEY, 0, v, u );
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
    add_continue( node->sloc, node->target );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_break* node, int count )
{
    add_break( node->sloc, node->target );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_return* node, int count )
{
    auto lv = push_list( node->values, -1 );
    pop_list( lv );
    op( node->sloc, Y_RETURN, lv.r, lv.count, 0 );
    return 0;
}

int yl_compile_visitor::visit( yl_stmt_throw* node, int count )
{
    unsigned v = push( node->value );
    pop( v );
    op( node->sloc, Y_THROW, v, 0, 0 );
    return 0;
}



int yl_compile_visitor::visit( yl_ast_func* node, int count )
{
    unsigned r = push();
    unsigned f = function( func );
    op( node->sloc, Y_CLOSURE, r, f );
    
    for ( size_t i = 0; i < func->upvals.size(); ++i )
    {
        const yl_ast_upval& uv = func->upvals.at( i );
        
        switch ( uv.kind )
        {
        case YL_UPVAL_LOCAL:
        {
            unsigned ui = upval_index( uv.local );
            unsigned li = local_index( uv.local );
            op( node->sloc, Y_UPLOCAL, (unsigned)i, ui, li );
            break;
        }
        
        case YL_UPVAL_OBJECT:
        {
            unsigned ui = upval_index( uv.object );
            unsigned li = local_index( uv.object );
            op( node->sloc, Y_UPLOCAL, (unsigned)i, ui, li );
            break;
        }
        
        case YL_UPVAL_UPVAL:
        {
            op( node->sloc, Y_UPUPVAL, (unsigned)i, uv.upval, 0 );
            break;
        }
        }
    }
    
    return 1;
}

int yl_compile_visitor::visit( yl_expr_null* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_NULL, r, 1, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_bool* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_BOOL, r, node->value ? 1 : 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_number* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_NUMBER, r, number( node->value ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_string* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_STRING, r, string( node->string, node->length ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_local* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_MOV, r, local_index( node->name ), 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_global* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_GLOBAL, r, 0, key( node->name ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_upref* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_GETUP, r, (unsigned)node->index, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_objref* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_MOV, r, local_index( node->object ), 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_superof* node, int count )
{
    unsigned o = push( node->expr );
    pop( o );
    unsigned r = push();
    op( node->sloc, Y_SUPER, r, o, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_key* node, int count )
{
    unsigned o = push( node->object );
    pop( o );
    unsigned r = push();
    op( node->sloc, Y_KEY, r, o, key( node->key ) );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_inkey* node, int count )
{
    unsigned o = push( node->object );
    unsigned k = push( node->key );
    pop( k );
    pop( o );
    unsigned r = push();
    op( node->sloc, Y_INKEY, r, o, k );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_index* node, int count )
{
    unsigned o = push( node->object );
    unsigned i = push( node->index );
    pop( i );
    pop( o );
    unsigned r = push();
    op( node->sloc, Y_INDEX, r, o, i );
    return 1;
}

int yl_compile_visitor::visit( yl_expr_preop* node, int count )
{
    lvalue lv = push_lvalue( node->lvalue );    // lv
    unsigned v = push_evaluate_lvalue( lv );    // lv, v
    unsigned n = push();                        // lv, v, 1
    op( node->sloc, Y_NUMBER, n, number( 1.0 ) );
    pop( n );
    pop( v );
    unsigned w = push();                        // lv, w
    op( node->sloc, node->opkind == YL_ASTOP_PREINC ? Y_ADD : Y_SUB, w, v, n );
    assign( lv, w );                            // *lv = w
    pop( w );
    pop_lvalue( lv );
    unsigned r = push();
    op( node->sloc, Y_MOV, r, w, 0 );           // w
    return 1;
}

int yl_compile_visitor::visit( yl_expr_postop* node, int count )
{
    lvalue lv = push_lvalue( node->lvalue );    // lv
    unsigned v = push_evaluate_lvalue( lv );    // lv, v
    unsigned u = push();                        // lv, v, u
    op( node->sloc, Y_MOV, u, v, 0 );
    unsigned n = push();                        // lv, v, u, 1
    op( node->sloc, Y_NUMBER, n, number( 1.0 ) );
    pop( n );
    pop( u );
    unsigned w = push();                        // lv, v, w
    op( node->sloc, node->opkind == YL_ASTOP_POSTINC ? Y_ADD : Y_SUB, w, u, n );
    assign( lv, w );                            // *lv = w
    pop( w );
    pop( v );
    pop_lvalue( lv );
    unsigned r = push();
    op( node->sloc, Y_MOV, r, v, 0 );           // v
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
        op( node->sloc, Y_NEG, r, v, 0 );
        op( node->sloc, Y_NEG, r, r, 0 );
        break;
        
    case YL_ASTOP_NEGATIVE:
        op( node->sloc, Y_NEG, r, v, 0 );
        break;
    
    case YL_ASTOP_LOGICNOT:
        op( node->sloc, Y_LNOT, r, v, 0 );
        break;

    case YL_ASTOP_BITNOT:
        op( node->sloc, Y_BITNOT, r, v, 0 );
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
        op( node->sloc, Y_MUL, r, a, b );
        break;
        
    case YL_ASTOP_DIVIDE:
        op( node->sloc, Y_DIV, r, a, b );
        break;
        
    case YL_ASTOP_MODULUS:
        op( node->sloc, Y_MOD, r, a, b );
        break;
        
    case YL_ASTOP_INTDIV:
        op( node->sloc, Y_INTDIV, r, a, b );
        break;
        
    case YL_ASTOP_ADD:
        op( node->sloc, Y_ADD, r, a, b );
        break;
        
    case YL_ASTOP_SUBTRACT:
        op( node->sloc, Y_SUB, r, a, b );
        break;
        
    case YL_ASTOP_LSHIFT:
        op( node->sloc, Y_LSL, r, a, b );
        break;
        
    case YL_ASTOP_LRSHIFT:
        op( node->sloc, Y_LSR, r, a, b );
        break;
        
    case YL_ASTOP_ARSHIFT:
        op( node->sloc, Y_ASR, r, a, b );
        break;
        
    case YL_ASTOP_BITAND:
        op( node->sloc, Y_BITAND, r, a, b );
        break;
        
    case YL_ASTOP_BITXOR:
        op( node->sloc, Y_BITXOR, r, a, b );
        break;
        
    case YL_ASTOP_BITOR:
        op( node->sloc, Y_BITOR, r, a, b );
        break;
        
    case YL_ASTOP_CONCATENATE:
        op( node->sloc, Y_CONCAT, r, a, b );
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
        compare_op( node->sloc, node->opkinds.at( i ), r, a, b );
        pop( r );
        shortcut_jumps.push_back( jump( node->sloc, Y_JMPF, r ) );
        unsigned a = push();                    // a
        op( node->sloc, Y_MOV, a, b, 0 );
    }
    
    unsigned b = push( node->terms.at( last ) );
    pop( b );
    pop( a );
    unsigned r = push();
    compare_op( node->sloc, node->opkinds.at( last ), r, a, b );

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
        pop( a );
        int shortcut_jump = jump( node->sloc, Y_JMPF, a );
        push( node->rhs );                      // b
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
        op( node->sloc, Y_LXOR, r, a, b );
        break;
    }
    
    case YL_ASTOP_LOGICOR:
    {
        unsigned a = push( node->lhs );         // a
        pop( a );
        int shortcut_jump = jump( node->sloc, Y_JMPT, a );
        push( node->rhs );                      // b
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
    int test_jump = jump( node->sloc, Y_JMPF, v );
    unsigned y = push( node->iftrue );
    int endif_jump = jump( node->sloc, Y_JMP, 0 );
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
    op( node->sloc, Y_OBJECT, o, p, 0 );
    unsigned m = push();                        // o, m
    unsigned t = push();                        // o, m, o
    op( node->sloc, Y_METHOD, m, o, key( "this" ) );
    listval l = push_list( node->arguments, -1 ); // o, m, o, ...
    pop_list( l );
    pop( t );
    pop( m );                                   // o
    op( node->sloc, Y_CALL, m, l.count != -1 ? l.count + 2 : -1, 0 );
    return 1;
}

int yl_compile_visitor::visit( yl_new_object* node, int count )
{
    unsigned p = push( node->proto );
    pop( p );
    unsigned o = push();
    op( node->sloc, Y_OBJECT, o, p, 0 );
    declare( o, node->upval, node, "[object]" );
    
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        execute( node->members.at( i ) );
    }
    
    undeclare( node->scope );
    undeclare( node );
    return 1;
}

int yl_compile_visitor::visit( yl_new_array* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_ARRAY, r, (unsigned)node->values.size() );
    
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        unsigned v = push( node->values.at( i ) );
        pop( v );
        op( node->sloc, Y_APPEND, r, v, 0 );
    }
    
    if ( node->final )
    {
        listval l = push_list( node->final, -1 );
        pop_list( l );
        op( node->sloc, Y_EXTEND, r, l.r, l.count != -1 ? l.count : -1 );
    }
    
    return 1;
}

int yl_compile_visitor::visit( yl_new_table* node, int count )
{
    unsigned r = push();
    op( node->sloc, Y_TABLE, r, (unsigned)node->elements.size() );
    
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const yl_key_value& kv = node->elements.at( i );
        unsigned k = push( kv.key );
        unsigned v = push( kv.value );
        pop( v );
        pop( k );
        op( node->sloc, Y_SETINDEX, r, k, v );
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
        op( node->sloc, Y_METHOD, f, o, key( kexpr->key ) ); // m, o
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
        op( node->sloc, Y_INKEY, t, o, k );     // o, m
        op( node->sloc, Y_SWP, f, t, 0 );       // m, o
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
    op( node->sloc, opcode, l.r, argcount, count );

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

    unsigned lcount = l.count != -1 ? l.count : -1;
    op( node->sloc, Y_YIELD, l.r, lcount, (unsigned)count );

    return count;
}

int yl_compile_visitor::visit( yl_expr_vararg* node, int count )
{
    if ( count != 0 )
    {
        listval r = push_list( count );
        op( node->sloc, Y_VARARG, r.r, r.count, 0 );
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
        op( node->sloc, Y_UNPACK, v, r.r, r.count );
    }
    return count;
}

int yl_compile_visitor::visit( yl_expr_list* node, int count )
{
    int rcount = (int)node->values.size();
    if ( count != -1 )
    {
        rcount = std::min( rcount, count );
    }
    
    for ( size_t i = 0; i < rcount; ++i )
    {
        push( node->values.at( i ) );
    }
    
    for ( size_t i = rcount; i < node->values.size(); ++i )
    {
        execute( node->values.at( i ) );
    }

    if ( node->final )
    {
        if ( count == -1 )
        {
            push_list( node->final, -1 );
        }
        else if ( count > rcount )
        {
            push_list( node->final, count - rcount );
        }
        else
        {
            execute( node->final );
        }
        
        return count;
    }
    
    return rcount;
}

int yl_compile_visitor::visit( yl_expr_assign* node, int count )
{
    if ( node->assignop == YL_ASTOP_DECLARE )
    {
        assert( node->lvalue->kind == YL_EXPR_LOCAL );
        yl_expr_local* local = (yl_expr_local*)node->lvalue;
        unsigned v = -1;
        if ( node->rvalue )
        {
            v = push( node->rvalue );
        }
        else
        {
            v = push();
            op( node->sloc, Y_NULL, v, 1, 0 );
        }
        declare( v, local->name->upval, local->name, local->name->name );
        unsigned r = push();
        op( node->sloc, Y_MOV, r, v, 0 );
        return 1;
    }
    else if ( node->assignop == YL_ASTOP_ASSIGN )
    {
        lvalue lv = push_lvalue( node->lvalue );    // lv
        unsigned v = push( node->rvalue );          // lv, v
        assign( lv, v );                            // *lv = v
        pop( v );
        pop_lvalue( lv );
        unsigned r = push();
        op( node->sloc, Y_MOV, r, v, 0 );
        return 1;
    }
    else
    {
        lvalue lv = push_lvalue( node->lvalue );    // lv
        unsigned v = push_evaluate_lvalue( lv );    // lv, v
        unsigned u = push( node->rvalue );          // lv, v, u
        pop( u );
        pop( v );
        unsigned w = push();                        // lv, w
        assign_op( node->sloc, node->assignop, w, v, u );
        assign( lv, w );
        pop( w );
        pop_lvalue( lv );
        unsigned r = push();
        op( node->sloc, Y_MOV, r, w, 0 );           // w
        return 1;
    }
}

int yl_compile_visitor::visit( yl_expr_assign_list* node, int count )
{
    int rcount = (int)node->lvalues.size();
    if ( count != -1 )
    {
        rcount = std::min( rcount, count );
    }
    
    if ( node->assignop == YL_ASTOP_DECLARE )
    {
    
        listval rv;
        if ( node->rvalues )
        {
            rv = push_list( node->rvalues, (int)node->lvalues.size() );
        }
        else
        {
            rv = push_list( (int)node->lvalues.size() );
            op( node->sloc, Y_NULL, rv.r, rv.count, 0 );
        }
        
        assert( rv.count == (int)node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            declare
            (
                rv.r + (int)i,
                local->name->upval,
                local->name,
                local->name->name
            );
        }
        
        listval r = push_list( rcount );
        for ( size_t i = 0; i < rcount; ++i )
        {
            op( node->sloc, Y_MOV, r.r + (unsigned)i, rv.r + (unsigned)i, 0 );
        }
        
        return rcount;
    }
    else if ( node->assignop == YL_ASTOP_ASSIGN )
    {

        std::vector< lvalue > lv;
        lv.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lv.push_back( push_lvalue( node->lvalues.at( i ) ) );
        }
                                    // lv, lv, lv
        
        listval rv = push_list( node->rvalues, (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );
                                    // lv, lv, lv, rv, rv, rv
     
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assign( lv.at( i ), rv.r + (int)i );
        }
        
        pop_list( rv );
        for ( auto i = lv.rbegin(); i != lv.rend(); ++i )
        {
            pop_lvalue( *i );
        }
        
        listval r = push_list( rcount );
        for ( size_t i = 0; i < rcount; ++i )
        {
            op( node->sloc, Y_MOV, r.r + (unsigned)i, rv.r + (unsigned)i, 0 );
        }
        
        return rcount;
    }
    else
    {

        std::vector< lvalue > lv;
        lv.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lv.push_back( push_lvalue( node->lvalues.at( i ) ) );
        }
                                    // lv, lv, lv
        
        std::vector< unsigned > v;
        v.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            v.push_back( push_evaluate_lvalue( lv.at( i ) ) );
        }
                                    // lv, lv, lv, v, v, v
        
        listval rv = push_list( node->rvalues, (int)node->lvalues.size() );
        assert( rv.count == (int)node->lvalues.size() );
                                    // lv, lv, lv, v, v, v, rv, rv, rv
     
        // Bit naughtly here - we should really pop all of the values and
        // repush, but it ends up the same anyway...
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assign_op
            (
                node->sloc,
                node->assignop,
                v.at( i ),
                v.at( i ),
                rv.r + (int)i
            );
        }

        pop_list( rv );
                                    // lv, lv, lv, w, w, w
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assign( lv.at( i ), v.at( i ) );
        }
        
        for ( auto i = v.rbegin(); i != v.rend(); ++i )
        {
            pop( *i );
        }
        
        for ( auto i = lv.rbegin(); i != lv.rend(); ++i )
        {
            pop_lvalue( *i );
        }
        
        listval r = push_list( rcount );
        for ( size_t i = 0; i < rcount; ++i )
        {
            op( node->sloc, Y_MOV, r.r + (unsigned)i, v.at( i ), 0 );
        }
        
        return rcount;
    }
}








void yl_compile_visitor::compare_op(
        int sloc, yl_ast_opkind opkind, unsigned r, unsigned a, unsigned b )
{
    switch ( opkind )
    {
    case YL_ASTOP_EQUAL:
        op( sloc, Y_EQ, r, a, b );
        break;
        
    case YL_ASTOP_NOTEQUAL:
        op( sloc, Y_NE, r, a, b );
        break;
        
    case YL_ASTOP_LESS:
        op( sloc, Y_LT, r, a, b );
        break;
        
    case YL_ASTOP_GREATER:
        op( sloc, Y_GT, r, a, b );
        break;
        
    case YL_ASTOP_LESSEQUAL:
        op( sloc, Y_LE, r, a, b );
        break;
        
    case YL_ASTOP_GREATEREQUAL:
        op( sloc, Y_GE, r, a, b );
        break;
    
    case YL_ASTOP_IN:
        op( sloc, Y_IN, r, a, b );
        break;
        
    case YL_ASTOP_NOTIN:
        op( sloc, Y_IN, r, a, b );
        op( sloc, Y_LNOT, r, r, 0 );
        break;
    
    case YL_ASTOP_IS:
        op( sloc, Y_IS, r, a, b );
        break;
        
    case YL_ASTOP_NOTIS:
        op( sloc, Y_IS, r, a, b );
        op( sloc, Y_LNOT, r, r, 0 );
        break;
    
    default:
        assert( ! "unexpected comparison operator" );
        break;
    }
}



void yl_compile_visitor::op(
                int sloc, y_opcode op, unsigned r, unsigned a, unsigned b )
{
    s->code.emplace_back( op, r, a, b );
    s->slocs.push_back( sloc );
}

void yl_compile_visitor::op(
                int sloc, y_opcode op, unsigned r, unsigned c )
{
    s->code.emplace_back( op, r, c );
    s->slocs.push_back( sloc );
}

void yl_compile_visitor::op(
                int sloc, y_opcode op, unsigned r, signed j )
{
    s->code.emplace_back( op, r, j );
    s->slocs.push_back( sloc );
}



int yl_compile_visitor::label()
{
    return (int)s->code.size();
}

int yl_compile_visitor::jump( int sloc, y_opcode opcode, unsigned r )
{
    s->code.emplace_back( opcode, r, (signed)0 );
    s->slocs.push_back( sloc );
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

void yl_compile_visitor::add_break( int sloc, yl_ast_scope* target )
{
    branch& branch_break = break_stack.back();
    assert( branch_break.target == target );
    branch_break.jumps.push_back( jump( sloc, Y_JMP, 0 ) );
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

void yl_compile_visitor::add_continue( int sloc, yl_ast_scope* target )
{
    branch& branch_continue = continue_stack.back();
    assert( branch_continue.target == target );
    branch_continue.jumps.push_back( jump( sloc, Y_JMP, 0 ) );
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












