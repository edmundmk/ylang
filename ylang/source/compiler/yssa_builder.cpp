//
//  yssa_builder.cpp
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_builder.h"
#include <make_unique.h>
#include "yl_code.h"




yssa_builder::yssa_builder( yl_diagnostics* diagnostics )
    :   diagnostics( diagnostics )
    ,   module( std::make_unique< yssa_module >() )
{
}

yssa_builder::~yssa_builder()
{
}



bool yssa_builder::build( yl_ast* ast )
{
    // Construct each function.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* astf = ast->functions.at( i );
        yssa_function_p ssaf = std::make_unique< yssa_function >
        (
            astf->sloc,
            module->alloc.strdup( astf->funcname )
        );
        funcmap.emplace( std::make_pair( astf, ssaf.get() ) );
        module->functions.push_back( std::move( ssaf ) );
    }
    
    
    // Build SSA for each function.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* astf = ast->functions.at( i );
        build( astf );
    }


    return diagnostics->error_count() == 0;
}


yssa_module* yssa_builder::get_module()
{
    return module.get();
}



void yssa_builder::build( yl_ast_func* astf )
{
    function = funcmap.at( astf );
    
    assert( stack.empty() );
    assert( scopes.empty() );
    assert( localups.empty() );
    assert( itercount == 0 );


    // Create entry block.
    yssa_block_p entry_block = std::make_unique< yssa_block >();
    block = entry_block.get();
    function->blocks.push_back( std::move( entry_block ) );


    // Enter scope.
    open_scope( astf->scope );
    
    
    // Add parameter ops.
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        yl_ast_name* param = astf->parameters.at( i );
        yssa_opinst* o = op( param->sloc, YSSA_PARAM, 0, 1 );
        o->select = (int)i;
        declare( param->sloc, variable( param ), o );
    }

    
    // Visit the block.
    visit( astf->block, 0 );


    // Close scope.
    close_scope( astf->sloc, astf->scope );

    
    // Each function must end with a return statement.
    op( astf->sloc, YL_RETURN, 0, 0 );


    assert( stack.empty() );
    assert( scopes.empty() );
    assert( localups.empty() );
    assert( itercount == 0 );

}




int yssa_builder::fallback( yl_ast_node* node, int count )
{
    assert( ! "invalid AST node" );
    return 0;
}

    
int yssa_builder::visit( yl_stmt_block* node, int count )
{
    /*
            statements
            close upvals
    */

    if ( node->scope )
    {
        open_scope( node->scope );
    }
    
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        execute( node->stmts.at( i ) );
    }
    
    if ( node->scope )
    {
        close_scope( node->sloc, node->scope );
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_if* node, int count )
{
    /*
            condition
                ? goto iftrue : goto iffalse
     
        iftrue:
            ...
            goto final
            
        iffalse:
            ...
            goto final
            
        final:
            close upvals
    */

    open_scope( node->scope );

    // Check if condition.
    size_t operand = push( node->condition, 1 );
    yssa_opinst* value = nullptr;
    pop( operand, 1, &value );

    if ( block )
    {
        assert( block->test == nullptr );
        block->test = value;
    }

    yssa_block* test_next = block;
    yssa_block* test_fail = block;
    block = nullptr;

    
    // True branch.
    if ( node->iftrue )
    {
        if ( test_next )
        {
            block = test_next;
            block = next_block();
            test_next = nullptr;
        }
        execute( node->iftrue );
    }
    
    yssa_block* true_block = block;
    block = nullptr;

    // False branch.
    if ( node->iffalse )
    {
        if ( test_fail )
        {
            block = test_fail;
            block = fail_block();
            test_fail = nullptr;
        }
        execute( node->iffalse );
    }
    
    yssa_block* false_block = block;
    block = nullptr;
    
    // Final block.
    if ( test_next || test_fail || true_block || false_block )
    {
        block = label_block();
        assert( block );
        
        if ( test_next )
        {
            link_block( test_next, NEXT, block );
        }
        if ( test_fail )
        {
            link_block( test_fail, FAIL, block );
        }
        if ( true_block )
        {
            link_block( true_block, NEXT, block );
        }
        if ( false_block )
        {
            link_block( false_block, NEXT, block );
        }
    }
    
    close_scope( node->sloc, node->scope );

    return 0;
}

int yssa_builder::visit( yl_stmt_switch* node, int count )
{
    /*
            eval value
            eval case0value
            if ( equal ) goto case0
            eval case1value
            if ( equal ) goto case1
            goto default
     
        case0:
            statements
        case1:
        default:
            statements

        break:
            close upvals
    */
    
    open_scope( node->scope );
    
    // Push the switch value.
    size_t operands = push( node->value, 1 );
    
    // Construct the dispatch table.
    std::unordered_map< yl_stmt_case*, yssa_block* > case_gotos;
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        // Find case statements.
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind != YL_STMT_CASE )
            continue;
        
        yl_stmt_case* cstmt = (yl_stmt_case*)stmt;
        
        // Ignore the default case.
        if ( ! cstmt->value )
            continue;
        
        // Open block.
        if ( block && case_gotos.size() )
        {
            block = fail_block();
        }
        
        // Push the case value.
        push( cstmt->value, 1 );

        // Perform comparison.
        yssa_opinst* o = op( cstmt->sloc, YL_EQ, 2, 1 );
        pop( operands, 2, o->operand );
        
        // Move to next comparison (if test fails).
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = o;
        }
        case_gotos.emplace( cstmt, block );
        
        // Repush switch value.
        operands = push_op( o->operand[ 0 ] );
    }
    
    // Pop value.
    yssa_opinst* value = nullptr;
    pop( operands, 1, &value );
    
    // If execution gets here then all tests have failed.
    yssa_block* default_goto = block;
    block = nullptr;
    
    // Break to end of switch.
    break_entry* sbreak = open_break( node->scope, BREAK );
    
    // Evaluate actual case blocks.
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        yl_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind == YL_STMT_CASE )
        {
            yl_stmt_case* cstmt = (yl_stmt_case*)stmt;

            // Work out which block to link.
            yssa_block* jump_block = nullptr;
            link_kind jump_link = NEXT;
            
            if ( cstmt->value )
            {
                jump_block      = case_gotos.at( cstmt );
                jump_link       = NEXT;
            }
            else
            {
                jump_block      = default_goto;
                jump_link       = case_gotos.empty() ? NEXT : FAIL;
                default_goto    = nullptr;
            }
            
            if ( ! jump_block )
            {
                continue;
            }
            
            // Make a block to link to.
            block = label_block();
            
            // Link it.
            assert( block );
            link_block( jump_block, jump_link, block );
        }
        else
        {
            execute( stmt );
        }
    }
    
    if ( default_goto || sbreak->blocks.size() )
    {
        // Make break target.
        block = label_block();
        assert( block );
        
        // If there was no default case, the default gets here.
        if ( default_goto )
        {
            link_block( default_goto, case_gotos.empty() ? NEXT : FAIL, block );
        }
    }
    
    // Link break.
    close_break( sbreak, block );

    // Close scope.
    close_scope( node->sloc, node->scope );
    
    return 0;
}

int yssa_builder::visit( yl_stmt_while* node, int count )
{
    /*
        continue:
            test condition
            if ( failed ) goto exit:
            statements
            close upvals
            goto continue
        exit:
            close upvals
        break:
    */

    // Enter loop.
    if ( block )
    {
        block = next_block( YSSA_LOOP | YSSA_UNSEALED );
    }
    yssa_block* loop = block;
    
    // Enter scope.
    open_scope( node->scope );
    break_entry* lbreak = open_break( node->scope, BREAK );
    break_entry* lcontinue = open_break( node->scope, CONTINUE );
    
    // Test condition.
    size_t test = push( node->condition, 1 );
    yssa_opinst* value = nullptr;
    pop( test, 1, &value );
    
    if ( block )
    {
        assert( block->test == nullptr );
        block->test = value;
    }
    
    yssa_block* test_fail = block;
    
    if ( block )
    {
        block = next_block();
    }
    
    // Remember enough information to construct the close op
    // on the exit branch.
    assert( scopes.back().scope == node->scope );
    assert( scopes.back().itercount == itercount );
    std::vector< yssa_variable* > close;
    close.insert
    (
        close.end(),
        localups.begin() + scopes.back().localups,
        localups.end()
    );
    
    // Statements.
    execute( node->body );
    close_scope( node->sloc, node->scope );
    
    // Link back to top of loop.
    close_break( lcontinue, loop );
    if ( block && loop )
    {
        link_block( block, NEXT, loop );
    }
    block = nullptr;
    if ( loop )
    {
        seal_block( loop );
    }
    
    // Fill in failure branch.
    if ( test_fail )
    {
        block = label_block();
        link_block( test_fail, FAIL, block );
        
        // Need to close upvals.
        if ( close.size() )
        {
            yssa_opinst* o = op( node->sloc, YL_CLOSE, close.size(), 0 );
            o->a = localups.size();
            o->b = itercount;
            for ( size_t i = 0; i < close.size(); ++i )
            {
                o->operand[ i ] = lookup( close.at( i ) );
            }
        }
    }
    
    // Fill in breaks.
    if ( lbreak->blocks.size() )
    {
        block = label_block();
    }
    close_break( lbreak, block );

    return 0;
}

int yssa_builder::visit( yl_stmt_do* node, int count )
{
    /*
        top:
            statements
        continue:
            test
            close upvals
            if ( succeeded ) goto top
        break:
    */
    
    // Enter loop.
    if ( block )
    {
        block = next_block( YSSA_LOOP | YSSA_UNSEALED );
    }
    yssa_block* loop = block;
    
    // Enter scope.
    open_scope( node->scope );
    break_entry* lbreak = open_break( node->scope, BREAK );
    break_entry* lcontinue = open_break( node->scope, CONTINUE );
    
    // Statements.
    execute( node->body );
    
    // Link continue.
    if ( lcontinue->blocks.size() )
    {
        block = label_block();
    }
    close_break( lcontinue, block );
    
    // Perform test.
    size_t test = push( node->condition, 1 );
    yssa_opinst* value = nullptr;
    pop( test, 1, &value );
    
    if ( block )
    {
        assert( block->test == nullptr );
        block->test = value;
    }
    
    // Close scope.
    close_scope( node->sloc, node->scope );
    
    // Link the success condition to the top of the loop.
    if ( block && loop )
    {
        link_block( block, NEXT, loop );
    }
    if ( loop )
    {
        seal_block( loop );
    }

    // Failure exits the loop.
    if ( block )
    {
        block = fail_block();
    }
    close_break( lbreak, block );

    return 0;
}

int yssa_builder::visit( yl_stmt_foreach* node, int count )
{
    /*
            iter/iterkey list
            goto entry
        continue:
            assign/declare iterator values
            statements
            close upvals
        entry:
            if ( have values ) goto continue
            close iterator
        break:
    */
    
    // Open scope that declares the iterator.  We break into a scope that
    // does not contain the iterator.
    open_scope( node->scope );
    break_entry* lbreak = open_break( node->scope, BREAK );
    
    // Push iterator.
    size_t operand = push( node->list, 1 );
    yl_opcode opcode = node->eachkey ? YL_ITERKEY : YL_ITER;
    yssa_opinst* o = op( node->sloc, opcode, 1, 0 );
    pop( operand, 1, o->operand );
    size_t iterindex = itercount;
    o->r = iterindex;
    itercount += 1;
    
    // Goto entry.
    yssa_block* entry_block = block;
    block = nullptr;
    
    // Declare body scope, for upvals contained in the body.  We continue
    // into a scope that contains the iterator.
    open_scope( node->scope );
    break_entry* lcontinue = open_break( node->scope, CONTINUE );
    
    // Create (unsealed) continue block.  This block is not the loop
    // header, as it is dominated by the entry block.
    block = label_block( YSSA_UNSEALED );
    yssa_block* body_block = block;
    
    // Work out which opcode to use to request values.
    opcode = YL_NOP;
    if ( node->lvalues.size() == 1 )
        opcode = YL_NEXT1;
    else if ( node->lvalues.size() == 2 )
        opcode = YL_NEXT2;
    else
        opcode = YL_NEXT;

    if ( node->declare )
    {
        // Request correct number of ops from iterator.
        yssa_opinst* o = op( node->sloc, opcode, 0, node->lvalues.size() );
        o->b = iterindex;
        
        // Declare each variable.
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)node->lvalues.at( i );
            yssa_variable* v = variable( local->name );
        
            if ( opcode == YL_NEXT )
            {
                declare( node->sloc, v, o );
            }
            else
            {
                yssa_opinst* select = op( node->sloc, YSSA_SELECT, 1, 1 );
                select->operand[ 0 ] = o;
                select->select = (int)i;
                declare( node->sloc, v, select );
            }
        }
    }
    else
    {
        // Push each lvalue.
        std::vector< size_t > lvalues;
        lvalues.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            size_t lvalue = push_lvalue( node->lvalues.at( i ) );
            lvalues.push_back( lvalue );
        }
        
        // Request correct number of ops from iterator.
        yssa_opinst* o = op( node->sloc, opcode, 0, node->lvalues.size() );
        o->b = iterindex;
        
        // Build select ops.
        std::vector< yssa_opinst* > selects;
        selects.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( opcode == YL_NEXT )
            {
                selects.push_back( o );
            }
            else
            {
                yssa_opinst* select = op( node->sloc, YSSA_SELECT, 1, 1 );
                select->operand[ 0 ] = o;
                select->select = (int)i;
                selects.push_back( select );
            }
        }
        
        // Assign each variable.
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            yssa_opinst* o = selects.at( i );
            assign_lvalue
            (
                node->sloc,
                node->lvalues.at( i ),
                lvalues.at( i ),
                o
            );
        }
        
        // Pop lvalues.
        for ( size_t i = node->lvalues.size(); i-- > 0; )
        {
            pop_lvalue( node->lvalues.at( i ), lvalues.at( i ) );
        }
    }
    
    // Perform statements.
    execute( node->body );
    
    // Exit upval scope.
    close_scope( node->sloc, node->scope );
    
    // Loop entry block.
    if ( block || entry_block )
    {
        block = label_block( YSSA_LOOP );
        assert( block );

        if ( entry_block )
        {
            link_block( entry_block, NEXT, block );
        }
    }
    
    // Perform loop check.
    o = op( node->sloc, YSSA_ITEREACH, 0, 0 );
    o->b = iterindex;
    
    if ( block )
    {
        assert( block->test == nullptr );
        block->test = o;
        
        if ( body_block )
        {
            link_block( block, NEXT, body_block );
        }
        
        block = fail_block();
    }

    // Link continue.
    close_break( lcontinue, body_block );
    if ( body_block )
    {
        seal_block( body_block );
    }
    
    // Exit iterator scope.
    close_scope( node->sloc, node->scope );
    
    // Link break.
    if ( lbreak->blocks.size() )
    {
        block = label_block();
    }
    close_break( lbreak, block );
    
    return 0;
}

int yssa_builder::visit( yl_stmt_for* node, int count )
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
            close upvals
    */
    
    // Scope of for init/condition/update covers all iterations of loop.
    open_scope( node->scope );
    break_entry* lbreak = open_break( node->scope, BREAK );
    break_entry* lcontinue = open_break( node->scope, CONTINUE );
    
    // Init.
    if ( node->init )
    {
        execute( node->init );
    }
    
    // Loop.
    if ( block )
    {
        block = next_block( YSSA_LOOP | YSSA_UNSEALED );
    }
    yssa_block* loop = block;
    
    // Condition.
    yssa_block* test_fail = nullptr;
    if ( node->condition )
    {
        size_t test = push( node->condition, 1 );
        yssa_opinst* value = nullptr;
        pop( test, 1, &value );
        
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = value;
        }
        
        test_fail = block;
        
        if ( block )
        {
            block = next_block();
        }
    }
    
    // Body.  A for body should have its own scope to declare upvals in.
    execute( node->body );
    
    // Continue location.
    if ( lcontinue->blocks.size() )
    {
        block = label_block();
    }
    close_break( lcontinue, block );
    
    if ( node->update )
    {
        execute( node->update );
    }
    
    if ( block && loop )
    {
        link_block( block, NEXT, loop );
    }
    if ( loop )
    {
        seal_block( loop );
    }
    block = nullptr;
    
    // Break location.
    if ( test_fail || lbreak->blocks.size() )
    {
        block = label_block();
        assert( block );
        
        if ( test_fail )
        {
            link_block( test_fail, FAIL, block );
        }
    }
    close_break( lbreak, block );

    // Close scope.
    close_scope( node->sloc, node->scope );
    return 0;
}

int yssa_builder::visit( yl_stmt_using* node, int count )
{
    /*
            o0.acquire();
        [
            o1.acquire();
        [
            using block
        ]
            o1.release();
            unwind
        ]
            o0.release();
            unwind
    */

    
    // Call o.acquire().
    int valcount = 0;
    size_t uvalues = push_all( node->uvalue, &valcount );
    
    // Using does support multiple values (more fool me).
    std::vector< yssa_block_p > xchandlers;
    for ( int i = 0; i < valcount; ++i )
    {
        // Call acquire().
        yssa_opinst* m = op( node->sloc, YL_KEY, 1, 1 );
        m->operand[ 0 ] = peek( uvalues, i );
        m->key = "acquire";
        
        yssa_opinst* c = op( node->sloc, YL_CALL, 2, 0 );
        c->operand[ 0 ] = m;
        c->operand[ 1 ] = peek( uvalues, i );
        call( c );
        
        // Open protected context.
        yssa_block_p xchandler =
                std::make_unique< yssa_block >( YSSA_XCHANDLER );
        xchandler->xclocalups = localups.size();
        xchandler->xcitercount = itercount;
        xchandler->xchandler =
                scopes.size() ? scopes.back().xchandler : nullptr;
        open_scope( node->scope, xchandler.get() );
        xchandlers.push_back( std::move( xchandler ) );
        
        if ( block )
        {
            block = next_block();
            assert( block->xchandler == xchandlers.back().get() );
        }
    }
    
    // Using block.  Leave all the uvalues on the stack - there's
    // only one definition of each.
    execute( node->body );
    
    // Close each scope and implement handler.
    for ( int i = valcount; i-- > 0; )
    {
        // Leave protected context.
        close_scope( node->sloc, node->scope );
        
        // Handler is next block.
        yssa_block* xchandler = xchandlers.at( i ).get();
        function->blocks.push_back( std::move( xchandlers.at( i ) ) );
        if ( block )
        {
            link_block( block, NEXT, xchandler );
            block = xchandler;
        }
        
        // Call release().
        yssa_opinst* m = op( node->sloc, YL_KEY, 1, 1 );
        m->operand[ 0 ] = peek( uvalues, i );
        m->key = "release";
        
        yssa_opinst* c = op( node->sloc, YL_CALL, 2, 0 );
        c->operand[ 0 ] = m;
        c->operand[ 1 ] = peek( uvalues, i );
        call( c );
        
        // Continue potential exception unwind.
        op( node->sloc, YL_UNWIND, 0, 0 );
        
        // Pop uvalue.
        yssa_opinst* value = nullptr;
        pop( uvalues + i, 1, &value );
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_try* node, int count )
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
    
    /*
        There are two potential protected contexts - one that
        passes control to the finally block (which can also be
        entered normally), and one that passes control to the
        exeption filter chain of catch () statements.
    */
    
    yssa_block_p xcfinally = nullptr;
    yssa_block_p xccatch = nullptr;
    
    if ( node->fstmt )
    {
        xcfinally = std::make_unique< yssa_block >( YSSA_XCHANDLER );
        xcfinally->xclocalups = localups.size();
        xcfinally->xcitercount = itercount;
        xcfinally->xchandler =
                scopes.size() ? scopes.back().xchandler : nullptr;
        open_scope( nullptr, xcfinally.get() );
    }
    
    if ( node->clist.size() )
    {
        xccatch = std::make_unique< yssa_block >( YSSA_XCHANDLER );
        xccatch->xclocalups = localups.size();
        xccatch->xcitercount = itercount;
        xccatch->xchandler =
                scopes.size() ? scopes.back().xchandler : nullptr;
        open_scope( nullptr, xccatch.get() );
    }
    
    if ( block && ( node->fstmt || node->clist.size() ) )
    {
        block = next_block();
        assert( block->xchandler );
    }
    
    execute( node->tstmt );
    
    // Construct exception filter from the catch clauses.
    yssa_block* catch_jump = nullptr;
    std::vector< yssa_block* > finally_jumps;
    if ( node->clist.size() )
    {
        close_scope( node->sloc, nullptr );

        if ( block )
        {
            finally_jumps.push_back( block );
            block = nullptr;
        }
        
        // Enter handler block for catch.
        block = xccatch.get();
        function->blocks.push_back( std::move( xccatch ) );
        
        // If the catch failed, go to the next one.
        for ( size_t i = 0; i < node->clist.size(); ++i )
        {
            assert( node->clist.at( i )->kind == YL_STMT_CATCH );
            yl_stmt_catch* cstmt = (yl_stmt_catch*)node->clist.at( i );
            
            // Link from previous failed catch filter expression.
            if ( catch_jump )
            {
                block = label_block();
                link_block( catch_jump, FAIL, block );
            }
            
            // Get exception.
            yssa_opinst* e = nullptr;
            if ( cstmt->proto || cstmt->lvalue )
            {
                e = op( cstmt->sloc, YL_EXCEPT, 0, 1 );
            }
            
            // Test catch filter.
            if ( cstmt->proto )
            {
                size_t operands = push_op( e );
                push( cstmt->proto, 1 );

                yssa_opinst* o = op( cstmt->sloc, YL_IS, 2, 1 );
                pop( operands, 2, o->operand );
                
                if ( block )
                {
                    assert( block->test == nullptr );
                    block->test = o;
                    catch_jump = block;
                    block = next_block();
                }
            }
            
            // Catch block for when test passes.
            open_scope( cstmt->scope );
            
            // Make exception assignment.
            if ( cstmt->lvalue && cstmt->declare )
            {
                assert( cstmt->lvalue->kind == YL_EXPR_LOCAL );
                yl_expr_local* local = (yl_expr_local*)cstmt->lvalue;
                yssa_variable* v = variable( local->name );
                declare( cstmt->sloc, v, e );
            }
            else if ( cstmt->lvalue )
            {
                size_t lvalue = push_lvalue( cstmt->lvalue );
                assign_lvalue( cstmt->sloc, cstmt->lvalue, lvalue, e );
                pop_lvalue( cstmt->lvalue, lvalue );
            }
            
            execute( cstmt->body );
            close_scope( node->sloc, cstmt->scope );
            
            if ( block )
            {
                finally_jumps.push_back( block );
                block = nullptr;
            }
        }
        
    }
    
    if ( node->fstmt )
    {
        close_scope( node->sloc, nullptr );

        if ( block )
        {
            finally_jumps.push_back( block );
            block = nullptr;
        }

        block = xcfinally.get();
        function->blocks.push_back( std::move( xcfinally ) );
    }
    else if ( catch_jump || finally_jumps.size() )
    {
        block = label_block();
    }
    
    if ( catch_jump )
    {
        link_block( catch_jump, FAIL, block );
    }
    
    for ( size_t i = 0; i < finally_jumps.size(); ++i )
    {
        link_block( finally_jumps.at( i ), NEXT, block );
    }
    
    if ( node->fstmt )
    {
        execute( node->fstmt );
        op( node->sloc, YL_UNWIND, 0, 0 );
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_catch* node, int count )
{
    assert( ! "catch outside try" );
    return 0;
}

int yssa_builder::visit( yl_stmt_delete* node, int count )
{
    for ( size_t i = 0; i < node->delvals.size(); ++i )
    {
        yl_ast_node* delval = node->delvals.at( i );
        if ( delval->kind == YL_EXPR_KEY )
        {
            yl_expr_key* expr = (yl_expr_key*)node;
            size_t operand = push( expr->object, 1 );
            yssa_opinst* o = op( node->sloc, YL_DELKEY, 1, 0 );
            pop( operand, 1, o->operand );
            o->key = module->alloc.strdup( expr->key );
        }
        else if ( delval->kind == YL_EXPR_INKEY )
        {
            yl_expr_inkey* expr = (yl_expr_inkey*)node;
            size_t operands = push( expr->object, 1 );
            push( expr->key, 1 );
            yssa_opinst* o = op( node->sloc, YL_DELINKEY, 2, 0 );
            pop( operands, 2, o->operand );
        }
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_case* node, int count )
{
    assert( ! "case outside switch" );
    return 0;
}

int yssa_builder::visit( yl_stmt_continue* node, int count )
{
    // Get break entry.
    auto i = breaks.find( break_key( node->target, CONTINUE ) );
    if ( i == breaks.end() )
    {
        assert( ! "continue outside continuable scope" );
        return 0;
    }
    
    break_entry* b = i->second.get();
    
    // Close.
    close( node->sloc, b->localups, b->itercount );
    
    // This branch terminates at the continue (which should be
    // linked when the continuable scope closes).
    if ( block )
    {
        b->blocks.push_back( block );
        block = nullptr;
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_break* node, int count )
{
    // Get break entry.
    auto i = breaks.find( break_key( node->target, BREAK ) );
    if ( i == breaks.end() )
    {
        assert( "break outside breakable scope" );
        return 0;
    }
    
    break_entry* b = i->second.get();
    
    // Close.
    close( node->sloc, b->localups, b->itercount );
    
    // This branch terminates at the break (which should be
    // linked when the breakable scope closes).
    if ( block )
    {
        b->blocks.push_back( block );
        block = nullptr;
    }
    
    return 0;
}

int yssa_builder::visit( yl_stmt_return* node, int count )
{
    // Push return values.
    int valcount = 0;
    size_t operands = push_all( node->values, &valcount );

    // Close everything.
    close( node->sloc, 0, 0 );
    
    // Return.
    yssa_opinst* o = op( node->sloc, YL_RETURN, valcount, 0 );
    pop( operands, valcount, o->operand );
    o->multival = multival;
    multival = nullptr;
    
    block = nullptr;
    
    return 0;
}

int yssa_builder::visit( yl_stmt_throw* node, int count )
{
    // The stack unwind should take care of closing things.
    size_t operand = push( node->value, 1 );
    yssa_opinst* o = op( node->sloc, YL_THROW, 1, 0 );
    pop( operand, 1, o->operand );
    return 0;
}







int yssa_builder::visit( yl_ast_func* node, int count )
{
    // Function op.
    yssa_function* function = funcmap.at( node );
    yssa_opinst* o = op( node->sloc, YL_FUNCTION, 0, 1 );
    o->function = function;
    
    // Upval initializers.
    for ( size_t i = 0; i < node->upvals.size(); ++i )
    {
        const yl_ast_upval& uv = node->upvals.at( i );
        switch ( uv.kind )
        {
        case YL_UPVAL_LOCAL:
        {
            // Make an upval from a local variable.
            yssa_variable* v = variable( uv.local );
            yssa_opinst* u = op( node->sloc, YL_UPLOCAL, 1, 0 );
            u->r = i;
            u->associated = o;
            u->a = v->localup;
            u->operand[ 0 ] = lookup( v );
            break;
        }
        
        case YL_UPVAL_OBJECT:
        {
            // Make an upval from a local object under constrution.
            yssa_variable* v = varobj( uv.object );
            yssa_opinst* u = op( node->sloc, YL_UPLOCAL, 1, 0 );
            u->r = i;
            u->associated = o;
            u->a = v->localup;
            u->operand[ 0 ] = lookup( v );
            break;
        }
        
        case YL_UPVAL_UPVAL:
        {
            // Make an upval from an existing upval.
            yssa_opinst* u = op( node->sloc, YL_UPUPVAL, 0, 0 );
            u->r = i;
            u->associated = o;
            u->a = uv.upval;
            break;
        }
        }
    }
    
    // Push onto virtual stack.
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_null* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_NULL, 0, 1 );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_bool* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_BOOL, 0, 1 );
    o->a = node->value ? 1 : 0;
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_number* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_NUMBER, 0, 1 );
    o->number = node->value;
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_string* node, int count )
{
    // Copy string.
    char* string = (char*)module->alloc.malloc( node->length + 1 );
    memcpy( string, node->string, node->length );
    string[ node->length ] = '\0';

    // Push value.
    yssa_opinst* o = op( node->sloc, YL_STRING, 0, 1 );
    o->string = new ( module->alloc ) yssa_string( string, node->length );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_local* node, int count )
{
    // Find definition of the variable which reaches this point.
    yssa_variable* v = variable( node->name );
    push_op( lookup( v ) );
    return 1;
}

int yssa_builder::visit( yl_expr_global* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_GLOBAL, 0, 1 );
    o->key = module->alloc.strdup( node->name );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_upref* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_GETUP, 0, 1 );
    o->a = (uint8_t)node->index;
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_objref* node, int count )
{
    // Find definition of object.
    yssa_variable* v = varobj( node->object );
    push_op( lookup( v ) );
    return 1;
}

int yssa_builder::visit( yl_expr_superof* node, int count )
{
    size_t operand = push( node->expr, 1 );
    yssa_opinst* o = op( node->sloc, YL_SUPER, 1, 1 );
    pop( operand, 1, o->operand );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_key* node, int count )
{
    size_t operand = push( node->object, 1 );
    yssa_opinst* o = op( node->sloc, YL_KEY, 1, 1 );
    pop( operand, 1, o->operand );
    o->key = module->alloc.strdup( node->key );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_inkey* node, int count )
{
    size_t operands = push( node->object, 1 );
    push( node->key, 1 );
    yssa_opinst* o = op( node->sloc, YL_INKEY, 2, 1 );
    pop( operands, 2, o->operand );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_index* node, int count )
{
    size_t operands = push( node->object, 1 );
    push( node->index, 1 );
    yssa_opinst* o = op( node->sloc, YL_INDEX, 2, 1 );
    pop( operands, 2, o->operand );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_preop* node, int count )
{
    size_t lvalue = push_lvalue( node->lvalue );            // lv
    size_t evalue = push_evalue( node->lvalue, lvalue );    // lv, ev

    // No increment/decrement opcode so do add/sub 1.0
    yssa_opinst* n = op( node->sloc, YL_NUMBER, 0, 1 );
    n->number = 1.0;

    // perform operation.
    yl_opcode opcode = ( node->opkind == YL_ASTOP_PREINC ) ? YL_ADD : YL_SUB;
    yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
    pop( evalue, 1, o->operand );
    o->operand[ 1 ] = n;
    
    // perform assignment.
    o = assign_lvalue( node->sloc, node->lvalue, lvalue, o );
    
    // push updated value onto stack.
    pop_lvalue( node->lvalue, lvalue );
    push_op( o );

    return 1;
}

int yssa_builder::visit( yl_expr_postop* node, int count )
{
    size_t lvalue = push_lvalue( node->lvalue );            // lv
    size_t evalue = push_evalue( node->lvalue, lvalue );    // lv, ev
    
    // No increment/decrement opcode so do add/sub 1.0
    yssa_opinst* n = op( node->sloc, YL_NUMBER, 0, 1 );
    n->number = 1.0;

    // perform operation
    yl_opcode opcode = ( node->opkind == YL_ASTOP_POSTINC ) ? YL_ADD : YL_SUB;
    yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
    pop( evalue, 1, o->operand );
    o->operand[ 1 ] = n;
    
    // perform assignment (which might clobber the original value).
    size_t ovalue = push_op( o->operand[ 0 ] );
    assign_lvalue( node->sloc, node->lvalue, lvalue, o );
    
    // push original value onto stack.
    yssa_opinst* value = nullptr;
    pop( ovalue, 1, &value );
    pop_lvalue( node->lvalue, lvalue );
    push_op( value );
    return 1;
}

int yssa_builder::visit( yl_expr_unary* node, int count )
{
    size_t operand = push( node->operand, 1 );

    yl_opcode opcode = YL_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_POSITIVE:
    {
        // Implemented as negative negative.
        yssa_opinst* o = op( node->sloc, YL_NEG, 1, 1 );
        pop( operand, 1, o->operand );
        operand = push_op( o );
        opcode = YL_NEG;
        break;
    }
    
    case YL_ASTOP_NEGATIVE:
        opcode = YL_NEG;
        break;
    
    case YL_ASTOP_LOGICNOT:
        opcode = YL_LNOT;
        break;
    
    case YL_ASTOP_BITNOT:
        opcode = YL_BITNOT;
        break;
        
    default:
        assert( ! "invalid unary operator" );
        break;
    }

    yssa_opinst* o = op( node->sloc, opcode, 1, 1 );
    pop( operand, 1, o->operand );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_binary* node, int count )
{
    size_t operands = push( node->lhs, 1 );
    push( node->rhs, 1 );

    yl_opcode opcode = YL_NOP;
    switch ( node->opkind )
    {
    case YL_ASTOP_MULTIPLY:
        opcode = YL_MUL;
        break;
    
    case YL_ASTOP_DIVIDE:
        opcode = YL_DIV;
        break;
    
    case YL_ASTOP_MODULUS:
        opcode = YL_MOD;
        break;
        
    case YL_ASTOP_INTDIV:
        opcode = YL_INTDIV;
        break;
        
    case YL_ASTOP_ADD:
        opcode = YL_ADD;
        break;
        
    case YL_ASTOP_SUBTRACT:
        opcode = YL_SUB;
        break;
        
    case YL_ASTOP_LSHIFT:
        opcode = YL_LSL;
        break;
        
    case YL_ASTOP_LRSHIFT:
        opcode = YL_LSR;
        break;
    
    case YL_ASTOP_ARSHIFT:
        opcode = YL_ASR;
        break;
    
    case YL_ASTOP_BITAND:
        opcode = YL_BITAND;
        break;
    
    case YL_ASTOP_BITXOR:
        opcode = YL_BITXOR;
        break;
    
    case YL_ASTOP_BITOR:
        opcode = YL_BITOR;
        break;
    
    case YL_ASTOP_CONCATENATE:
        opcode = YL_CONCAT;
        break;
    
    default:
        assert( ! "invalid binary operator" );
        break;
    }

    yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
    pop( operands, 2, o->operand );
    push_op( o );
    return 1;
}

int yssa_builder::visit( yl_expr_compare* node, int count )
{
    /*
            operand a
            operand b
            result = comparison
                ? goto next1 : goto final
     
        next1:
            operand c
            result = comparison
                ? goto next2 : goto final
     
        next2:
            operand d
            result = comparison
                goto final
     
        final:
    */
    
    
    // With shortcut evaluation, the result could be any of the intermediate
    // comparison results.  This requires a temporary.
    yssa_variable* result = node->opkinds.size() > 1 ?
            temporary( "[comparison]", node->sloc ) : nullptr;
    
    // Evaluate first term.
    size_t operands = push( node->first, 1 );               // a
    
    // Intermediate comparisons.
    std::vector< yssa_block* > blocks;
    size_t last = node->opkinds.size() - 1;
    for ( size_t i = 0; ; ++i )
    {
        push( node->terms.at( i ), 1 );                     // a, b
        
        // Perform comparison.
        yl_ast_opkind opkind = node->opkinds.at( i );
        yl_opcode opcode = YL_NOP;
        switch ( opkind )
        {
        case YL_ASTOP_EQUAL:
            opcode = YL_EQ;
            break;
            
        case YL_ASTOP_NOTEQUAL:
            opcode = YL_NE;
            break;
            
        case YL_ASTOP_LESS:
            opcode = YL_LT;
            break;
            
        case YL_ASTOP_GREATER:
            opcode = YL_GT;
            break;
            
        case YL_ASTOP_LESSEQUAL:
            opcode = YL_LE;
            break;
            
        case YL_ASTOP_GREATEREQUAL:
            opcode = YL_GE;
            break;
            
        case YL_ASTOP_IN:
            opcode = YL_IN;
            break;
            
        case YL_ASTOP_NOTIN:
            opcode = YL_IN;
            break;
            
        case YL_ASTOP_IS:
            opcode = YL_IS;
            break;
            
        case YL_ASTOP_NOTIS:
            opcode = YL_IS;
            break;
            
        default:
            assert( ! "unexpected comparison operator" );
            break;
        }
        
        yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
        pop( operands, 2, o->operand );
        size_t index = push_op( o );
        
        yssa_opinst* b = o->operand[ 1 ];
        
        if ( opkind == YL_ASTOP_NOTIN || opkind == YL_ASTOP_NOTIS )
        {
            o = op( node->sloc, YL_LNOT, 1, 1 );
            pop( index, 1, o->operand );
            index = push_op( o );
        }


        // Assign to temporary.
        if ( result )
        {
            pop( index, 1, &o );
            o = assign( node->sloc, result, o );
        }
        
        // No shortcut after last comparison in chain.
        if ( i >= last )
        {
            break;
        }
        
        // Only continue to next block if comparison is true.
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = o;
            blocks.push_back( block );
            block = next_block();
        }

        // Ensure that operand b is on the top of the virtual stack.
        operands = push_op( b );
    }
    
    // Any failed comparison shortcuts to the end.
    if ( blocks.size() )
    {
        block = label_block();
        for ( size_t i = 0; i < blocks.size(); ++i )
        {
            yssa_block* shortcut = blocks.at( i );
            link_block( shortcut, FAIL, block );
        }
    }

    // Push lookup of temporary.
    if ( result )
    {
        push_op( lookup( result ) );
    }
    return 1;
}

int yssa_builder::visit( yl_expr_logical* node, int count )
{
    switch ( node->opkind )
    {
    case YL_ASTOP_LOGICAND:
    {
        /*
                result = lhs
                    ? goto next : goto final
        
            next:
                result = rhs
                
            final:
        */
        
        yssa_variable* result = temporary( "[shortcut and]", node->sloc );
        
        // Evaluate left hand side.
        size_t operand = push( node->lhs, 1 );
        yssa_opinst* value = nullptr;
        pop( operand, 1, &value );
        value = assign( node->sloc, result, value );
        
        // Only continue to next block if lhs is true.
        yssa_block* lhs_block = block;
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = value;
            block = next_block();
        }
        
        // Evaluate right hand side.
        operand = push( node->rhs, 1 );
        pop( operand, 1, &value );
        assign( node->sloc, result, value );

        // Link in the shortcut.
        if ( lhs_block )
        {
            block = label_block();
            link_block( lhs_block, FAIL, block );
        }
        
        // Push lookup of temporary.
        push_op( lookup( result ) );
        return 1;
    }
    
    case YL_ASTOP_LOGICXOR:
    {
        size_t operands = push( node->lhs, 1 );
        push( node->rhs, 1 );
        yssa_opinst* o = op( node->sloc, YL_LXOR, 2, 1 );
        pop( operands, 2, o->operand );
        push_op( o );
        return 1;
    }
    
    case YL_ASTOP_LOGICOR:
    {
        /*
                result = lhs
                    ? goto final : goto next
        
            next:
                result = rhs
                
            final:
        */
    
        yssa_variable* result = temporary( "[shortcut or]", node->sloc );
        
        // Evaluate left hand side.
        size_t operand = push( node->lhs, 1 );
        yssa_opinst* value = nullptr;
        pop( operand, 1, &value );
        value = assign( node->sloc, result, value );
        
        // Only continue to next block if lhs is false.
        yssa_block* lhs_block = block;
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = value;
            block = fail_block();
        }
        
        // Evaluate right hand side.
        operand = push( node->rhs, 1 );
        pop( operand, 1, &value );
        assign( node->sloc, result, value );

        // Link in the shortcut.
        if ( lhs_block )
        {
            block = label_block();
            link_block( lhs_block, NEXT, block );
        }
        
        // Push lookup of temporary.
        push_op( lookup( result ) );
        return 1;
    }
    
    default:
        assert( ! "invalid logical operator" );
        return 0;
    }
}

int yssa_builder::visit( yl_expr_qmark* node, int count )
{
    /*
            condition
                ? goto iftrue : goto iffalse

        iftrue:
            result = trueexpr
            goto final
        
        iffalse:
            result = falseexpr
            goto final
            
        final:
    */
    
    // Evaluate condition.
    size_t operand = push( node->condition, 1 );
    yssa_opinst* value = nullptr;
    pop( operand, 1, &value );
    
    // True block.
    yssa_block* test_block = block;
    if ( block )
    {
        assert( block->test == nullptr );
        block->test = value;
        block = next_block();
    }
    
    // Result is a temporary since there is more than one definition.
    yssa_variable* result = temporary( "[qmark]", node->sloc );
    
    // Evaluate true branch.
    operand = push( node->iftrue, 1 );
    pop( operand, 1, &value );
    assign( node->sloc, result, value );
    
    yssa_block* true_block = block;

    // False block.
    block = test_block;
    if ( block )
    {
        block = fail_block();
    }
    
    // Evaluate false branch.
    operand = push( node->iffalse, 1 );
    pop( operand, 1, &value );
    assign( node->sloc, result, value );
    
    yssa_block* false_block = block;
    block = nullptr;
    
    // Final block.
    if ( true_block || false_block )
    {
        block = label_block();
        if ( true_block )
        {
            link_block( true_block, NEXT, block );
        }
        if ( false_block )
        {
            link_block( false_block, NEXT, block );
        }
    }

    // Push lookup of temporary.
    push_op( lookup( result ) );
    return 1;
}

int yssa_builder::visit( yl_new_new* node, int count )
{
    // Evaluate object prototype.
    size_t operand = push( node->proto, 1 );
    
    // Create object using prototype.
    yssa_opinst* o = op( node->sloc, YL_OBJECT, 1, 1 );
    pop( operand, 1, o->operand );
    operand = push_op( o );
    
    // Call 'this' method.
    yssa_opinst* m = op( node->sloc, YL_KEY, 1, 1 );
    m->operand[ 0 ] = o;
    m->key = "this";
    size_t method = push_op( m );
    
    // Repush the object (as this argument).
    push_op( o );
    
    // Push arguments.
    int argcount = 0;
    push_all( node->arguments, &argcount );
    
    // Perform call.
    yssa_opinst* c = op( node->sloc, YL_CALL, argcount + 2, 0 );
    pop( method, argcount + 2, c->operand );
    c->multival = multival;
    multival = nullptr;
    call( c );
    
    // Only thing on virtual stack should be the constructed object.
    return 1;
}

int yssa_builder::visit( yl_new_object* node, int count )
{
    // Evaluate prototype.
    yssa_opinst* o = nullptr;
    if ( node->proto )
    {
        size_t operand = push( node->proto, 1 );
        o = op( node->sloc, YL_OBJECT, 1, 1 );
        pop( operand, 1, o->operand );
    }
    else
    {
        o = op( node->sloc, YL_OBJECT, 0, 1 );
    }
    
    // Push object onto virtual stack.
    push_op( o );
    
    // Declare object (as it can potentially be referenced as an upval).
    open_scope( nullptr );
    yssa_variable* object = varobj( node );
    declare( node->sloc, object, o );
    
    // Execute all member initializers.
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        execute( node->members.at( i ) );
    }

    // Close upval (if any).
    close_scope( node->sloc, nullptr );
    
    // object should still be on the virtual stack.
    return 1;
}

int yssa_builder::visit( yl_new_array* node, int count )
{
    // Construct new array.
    yssa_opinst* o = op( node->sloc, YL_ARRAY, 0, 1 );
    o->c = node->values.size();
    push_op( o );
    
    // Append each element.
    for ( size_t i = 0; i < node->values.size(); ++i )
    {
        size_t element = push( node->values.at( i ), 1 );
        yssa_opinst* a = op( node->sloc, YL_APPEND, 2, 0 );
        a->operand[ 0 ] = o;
        pop( element, 1, &a->operand[ 1 ] );
    }
    
    // Extend final elements.
    if ( node->final )
    {
        int count = 0;
        size_t extend = push_all( node->final, &count );
        yssa_opinst* e = op( node->sloc, YL_EXTEND, count + 1, 0 );
        e->operand[ 0 ] = o;
        pop( extend, count, &e->operand[ 1 ] );
        e->multival = multival;
        multival = nullptr;
    }
    
    // array should still be on virtual stack.
    return 1;
}

int yssa_builder::visit( yl_new_table* node, int count )
{
    // Construct new table.
    yssa_opinst* o = op( node->sloc, YL_TABLE, 0, 1 );
    o->c = node->elements.size();
    push_op( o );
    
    // Append each element.
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        const yl_key_value& kv = node->elements.at( i );
        size_t operands = push( kv.key, 1 );
        push( kv.value, 1 );
        yssa_opinst* e = op( node->sloc, YL_SETINDEX, 2, 0 );
        pop( operands, 2, e->operand );
    }
    
    // table should still be on virtual stack
    return 1;
}

int yssa_builder::visit( yl_expr_mono* node, int count )
{
    // Even when used in a context that requests multiple values,
    // push only a single value onto the virtual stack.
    push( node->expr, 1 );
    return 1;
}

int yssa_builder::visit( yl_expr_call* node, int count )
{
    // Restrict call expressions without unpack ellipsis to a single result.
    if ( ! node->unpack && count != 0 )
    {
        count = 1;
    }
    
    // Check for method call.
    size_t function;
    int funcount;
    switch ( node->function->kind )
    {
    case YL_EXPR_KEY:
    {
        yl_expr_key* knode = (yl_expr_key*)node->function;
        
        // Look up key for method.
        size_t operand = push( knode->object, 1 );
        yssa_opinst* o = op( knode->sloc, YL_KEY, 1, 1 );
        pop( operand, 1, o->operand );
        o->key = module->alloc.strdup( knode->key );
        function = push_op( o );
        
        // Push the object onto the virtual stack as the 'this' parameter.
        push_op( o->operand[ 0 ] );
        funcount = 2;
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        yl_expr_inkey* knode = (yl_expr_inkey*)node->function;
        
        // Look up index for method.
        size_t operands = push( knode->object, 1 );
        push( knode->key, 1 );
        yssa_opinst* o = op( knode->sloc, YL_INKEY, 2, 1 );
        pop( operands, 2, o->operand );
        function = push_op( o );
        
        // Push the object onto virtual stack as the 'this' parameter.
        push_op( o->operand[ 0 ] );
        funcount = 2;
        break;
    }
    
    default:
    {
        function = push( node->function, 1 );
        funcount = 1;
        break;
    }
    }
    
    // Push arguments.
    int argcount = 0;
    push_all( node->arguments, &argcount );
    
    // Perform call.
    yl_opcode opcode = node->yieldcall ? YL_YCALL : YL_CALL;
    yssa_opinst* c = op( node->sloc, opcode, funcount + argcount, count );
    pop( function, funcount + argcount, c->operand );
    c->multival = multival;
    multival = nullptr;
    call( c );
    
    // Return results appropriately on the stack.
    return push_select( node->sloc, c, count );
}

int yssa_builder::visit( yl_expr_yield* node, int count )
{
    // Restrict expressions without unpack to a single result.
    if ( ! node->unpack && count != 0 )
    {
        count = 1;
    }
    
    // Push arguments.
    int argcount = 0;
    size_t arguments = push_all( node->arguments, &argcount );
    
    // Perform yield.
    yssa_opinst* y = op( node->sloc, YL_YIELD, argcount, count );
    pop( arguments, argcount, y->operand );
    y->multival = multival;
    multival = nullptr;
    call( y );
    
    // Return results appropriately on the stack.
    return push_select( node->sloc, y, count );
}

int yssa_builder::visit( yl_expr_vararg* node, int count )
{
    if ( count != 0 )
    {
        yssa_opinst* o = op( node->sloc, YL_VARARG, 0, count );
        return push_select( node->sloc, o, count );
    }
    else
    {
        return 0;
    }
}

int yssa_builder::visit( yl_expr_unpack* node, int count )
{
    if ( count != 0 )
    {
        size_t operand = push( node->array, 1 );
        yssa_opinst* o = op( node->sloc, YL_UNPACK, 1, count );
        pop( operand, 1, o->operand );
        return push_select( node->sloc, o, count );
    }
    else
    {
        execute( node->array );
        return 0;
    }
}

int yssa_builder::visit( yl_expr_list* node, int count )
{
    // Work out how many values we need to actually push.
    int value_count = (int)node->values.size();
    if ( count != -1 )
    {
        value_count = std::min( value_count, count );
    }
    
    // Push value_count values.
    for ( size_t i = 0; i < value_count; ++i )
    {
        push( node->values.at( i ), 1 );
    }
    
    // Execute remaining values without leaving them on the stack.
    for ( size_t i = value_count; i < node->values.size(); ++i )
    {
        execute( node->values.at( i ) );
    }
    
    // If there's an unpack value at the end of the list, then unpack it.
    if ( node->final )
    {
        if ( count == -1 )
        {
            int final_count = 0;
            push_all( node->final, &final_count );
            value_count += final_count;
        }
        else if ( count > value_count )
        {
            push( node->final, count - value_count );
            value_count = count;
        }
        else
        {
            execute( node->final );
        }
    }
    
    return value_count;
}

int yssa_builder::visit( yl_expr_assign* node, int count )
{
    if ( node->assignop == YL_ASTOP_DECLARE )
    {
        // Lvalue is a new variable.
        assert( node->lvalue->kind == YL_EXPR_LOCAL );
        yl_expr_local* local = (yl_expr_local*)node->lvalue;
        yssa_variable* v = variable( local->name );
        
        // Evaluate rvalue.
        yssa_opinst* value = nullptr;
        if ( node->rvalue )
        {
            size_t rvalue = push( node->rvalue, 1 );
            pop( rvalue, 1, &value );
        }
        else
        {
            value = op( node->sloc, YL_NULL, 0, 1 );
        }
        
        // Assign.
        value = declare( node->sloc, v, value );
        
        // Push value onto stack as result.
        push_op( value );
        return 1;
    }
    else if ( node->assignop == YL_ASTOP_ASSIGN )
    {
        // Assign rvalue to lvalue.
        size_t lvalue = push_lvalue( node->lvalue );        // lv
        size_t rvalue = push( node->rvalue, 1 );            // lv, rv
        yssa_opinst* value = nullptr;
        pop( rvalue, 1, &value );
        value = assign_lvalue( node->sloc, node->lvalue, lvalue, value );
        pop_lvalue( node->lvalue, lvalue );
        
        // Push result value onto stack as result.
        push_op( value );
        return 1;
    }
    else
    {
        // Evaluate lvalue and assign it with the assign op.
        size_t lvalue = push_lvalue( node->lvalue );            // lv
        size_t evalue = push_evalue( node->lvalue, lvalue );    // lv, ev
        push( node->rvalue, 1 );                                // lv, ev, rv
        yssa_opinst* o = assign_op( node->sloc, node->assignop, evalue );
        o = assign_lvalue( node->sloc, node->lvalue, lvalue, o );
        pop_lvalue( node->lvalue, lvalue );
        
        // Push result value onto stack as result.
        push_op( o );
        return 1;
    }
}

int yssa_builder::visit( yl_expr_assign_list* node, int count )
{
    // This is the most complex thing ever.

    // Work out how many values we actually need to push.  We only have
    // as many values as there are lvalues on the left hand side.
    int value_count = (int)node->lvalues.size();
    if ( count != -1 )
    {
        value_count = std::min( value_count, count );
    }

    if ( node->assignop == YL_ASTOP_DECLARE )
    {
        // Evaluate rvalue list.
        size_t rvalues = push( node->rvalues, (int)node->lvalues.size() );
        
        // Assign them all.
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            // Lvalue is a new variable.
            yl_ast_node* lnode = node->lvalues.at( i );
            assert( lnode->kind == YL_EXPR_LOCAL );
            yl_expr_local* local = (yl_expr_local*)lnode;
            yssa_variable* v = variable( local->name );
            
            // Rvalue is on stack.
            declare( node->sloc, v, peek( rvalues, i ) );
        }

        // Pop values.
        std::vector< yssa_opinst* > values;
        values.resize( node->lvalues.size() );
        pop( rvalues, (int)node->lvalues.size(), values.data() );
        
        // Push correct number of values back.
        assert( value_count <= (int)node->lvalues.size() );
        for ( int i = 0; i < value_count; ++i )
        {
            push_op( values.at( i ) );
        }
        return value_count;
    }
    else if ( node->assignop == YL_ASTOP_ASSIGN )
    {
        /*
            a, b = b, a;
         
            a = 4;
            b = a, a = 3, a + 3;
            assert( a == 7 );
            assert( b == 3 ); // possibly!
        */
    
        // Push each lvalue.
        std::vector< size_t > lvalues;
        lvalues.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            size_t lvalue = push_lvalue( node->lvalues.at( i ) );
            lvalues.push_back( lvalue );
        }
    
        // Evaluate rvalue list.
        size_t rvalues = push( node->rvalues, (int)node->lvalues.size() );
        
        // Assign them all.
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assign_lvalue
            (
                node->sloc,
                node->lvalues.at( i ),
                lvalues.at( i ),
                peek( rvalues, i )
            );
        }

        // Pop rvalues.
        std::vector< yssa_opinst* > values;
        values.resize( node->lvalues.size() );
        pop( rvalues, (int)node->lvalues.size(), values.data() );
        
        // Pop lvalues.
        for ( size_t i = node->lvalues.size(); i-- > 0; )
        {
            pop_lvalue( node->lvalues.at( i ), lvalues.at( i ) );
        }
        
        // Push correct number of values back.
        assert( value_count <= (int)node->lvalues.size() );
        for ( int i = 0; i < value_count; ++i )
        {
            push_op( values.at( i ) );
        }
        return value_count;
    }
    else
    {
        // Push each lvalue.
        std::vector< size_t > lvalues;
        lvalues.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            size_t lvalue = push_lvalue( node->lvalues.at( i ) );
            lvalues.push_back( lvalue );
        }
        
        // Evaluate each lvalue.
        size_t evalues = stack.size();
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            size_t lvalue = lvalues.at( i );
            push_evalue( node->lvalues.at( i ), lvalue );
        }
        assert( stack.size() == evalues + node->lvalues.size() );
    
        // Evaluate rvalue list.
        size_t rvalues = push( node->rvalues, (int)node->lvalues.size() );
     
        // Perform assignment operations and assignments.
        size_t ovalues = stack.size();
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            // Push operands.
            size_t operands = push_op( peek( evalues, i ) );
            push_op( peek( rvalues, i ) );
            
            // Perform assignment.
            yssa_opinst* o = assign_op( node->sloc, node->assignop, operands );
            o = assign_lvalue
            (
                node->sloc,
                node->lvalues.at( i ),
                lvalues.at( i ),
                o
            );
            push_op( o );
        }
        assert( stack.size() == ovalues + node->lvalues.size() );
        
        // Pop values.
        std::vector< yssa_opinst* > values;
        values.resize( node->lvalues.size() );
        pop( ovalues, (int)node->lvalues.size(), values.data() );
        
        // Pop rvalues.
        std::vector< yssa_opinst* > poprv;
        poprv.resize( node->lvalues.size() );
        pop( rvalues, (int)node->lvalues.size(), poprv.data() );
        
        // Pop lvalues.
        for ( size_t i = node->lvalues.size(); i-- > 0; )
        {
            pop_lvalue( node->lvalues.at( i ), lvalues.at( i ) );
        }
        
        // Push correct number of values back.
        assert( value_count <= (int)node->lvalues.size() );
        for ( int i = 0; i < value_count; ++i )
        {
            push_op( values.at( i ) );
        }
        return value_count;
    }
}




/*
    Control flow.
*/

yssa_block* yssa_builder::make_block( unsigned flags )
{
    function->blocks.emplace_back( new yssa_block( flags ) );
    return function->blocks.back().get();
}

yssa_block* yssa_builder::next_block( unsigned flags )
{
    yssa_block* next = make_block( flags );
    if ( block )
    {
        link_block( block, NEXT, next );
    }
    return next;
}

yssa_block* yssa_builder::fail_block( unsigned flags )
{
    yssa_block* fail = make_block( flags );
    if ( block )
    {
        link_block( block, FAIL, fail );
    }
    return fail;
}

yssa_block* yssa_builder::label_block( unsigned flags )
{
    /*
        Create the next block as the target of a jump.
    */
    
    if ( block && block->ops.empty() )
    {
        // Current block is empty, so can link to it directly.
        return block;
    }
    else
    {
        // Return next block.
        return next_block( flags );
    }
}

void yssa_builder::link_block( yssa_block* prev, link_kind kind, yssa_block* next )
{
    switch ( kind )
    {
    case NEXT:
        assert( prev->next == nullptr );
        prev->next = next;
        next->prev.push_back( prev );
        break;
    
    case FAIL:
        assert( prev->fail == nullptr );
        prev->fail = next;
        next->prev.push_back( prev );
        break;
    
    default:
        assert( ! "unknown link kind" );
        break;
    }
}



/*
    Emitting instructions.
*/

yssa_opinst* yssa_builder::make_op(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
{
    // Allocate op from memory region.
    void* p = module->alloc.malloc(
            sizeof( yssa_opinst ) + sizeof( yssa_opinst* ) * operand_count );
    return new ( p ) yssa_opinst( sloc, opcode, operand_count, result_count );
}


yssa_opinst* yssa_builder::op(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
{
    // Create op and add to current block.
    yssa_opinst* op = make_op( sloc, opcode, operand_count, result_count );
    if ( block )
    {
        block->ops.push_back( op );
    }
    
    return op;
}


yssa_opinst* yssa_builder::assign_op(
                int sloc, yl_ast_opkind opkind, size_t operands )
{
    uint8_t opcode = YL_NOP;
    switch ( opkind )
    {
    case YL_ASTOP_MULASSIGN:        opcode = YL_MUL;        break;
    case YL_ASTOP_DIVASSIGN:        opcode = YL_DIV;        break;
    case YL_ASTOP_MODASSIGN:        opcode = YL_MOD;        break;
    case YL_ASTOP_INTDIVASSIGN:     opcode = YL_INTDIV;     break;
    case YL_ASTOP_ADDASSIGN:        opcode = YL_ADD;        break;
    case YL_ASTOP_SUBASSIGN:        opcode = YL_SUB;        break;
    case YL_ASTOP_LSHIFTASSIGN:     opcode = YL_LSL;        break;
    case YL_ASTOP_LRSHIFTASSIGN:    opcode = YL_LSR;        break;
    case YL_ASTOP_ARSHIFTASSIGN:    opcode = YL_ASR;        break;
    case YL_ASTOP_BITANDASSIGN:     opcode = YL_BITAND;     break;
    case YL_ASTOP_BITXORASSIGN:     opcode = YL_BITXOR;     break;
    case YL_ASTOP_BITORASSIGN:      opcode = YL_BITOR;      break;

    default:
        assert( ! "unexpected compound assignment operator" );
        break;
    }
    
    yssa_opinst* o = op( sloc, opcode, 2, 1 );
    pop( operands, 2, o->operand );
    return o;
}



/*
    Definitions and lookups.
*/

yssa_variable* yssa_builder::variable( yl_ast_name* name )
{
    auto i = variables.find( name );
    if ( i != variables.end() )
    {
        return i->second;
    }
    
    yssa_variable* v = new ( module->alloc ) yssa_variable
    (
        module->alloc.strdup( name->name ),
        name->sloc,
        false,              // Not (yet) referenced from an exception handler.
        name->upval ? localups.size() : yl_opinst::NOVAL
    );
    
    variables.emplace( name, v );
    return v;
}

yssa_variable* yssa_builder::varobj( yl_new_object* object )
{
    auto i = variables.find( object );
    if ( i != variables.end() )
    {
        return i->second;
    }
    
    yssa_variable* v = new ( module->alloc ) yssa_variable
    (
        "[object]",
        object->sloc,
        false,              // Not (yet) referenced from an exception handler.
        object->upval ? localups.size() : yl_opinst::NOVAL
    );
    
    variables.emplace( object, v );
    return v;
}

yssa_variable* yssa_builder::temporary( const char* name, int sloc )
{
    yssa_variable* v = new ( module->alloc ) yssa_variable
    (
        name,
        sloc,
        false,              // You know by now!
        yl_opinst::NOVAL    // No way to refer to this variable in inner scope.
    );
    
    return v;
}




yssa_opinst* yssa_builder::declare(
                int sloc, yssa_variable* variable, yssa_opinst* value )
{
    // Upvals exist from their declaration to the end of the closing scope.
    if ( variable->localup != yl_opinst::NOVAL )
    {
        localups.push_back( variable );
    }

    // Otherwise acts like an ordinary assignment.
    return assign( sloc, variable, value );
}


yssa_opinst* yssa_builder::assign(
                int sloc, yssa_variable* variable, yssa_opinst* value )
{
    if ( value->variable )
    {
        // The definition was already a definition of another variable.
        // One op can't define two variables, so the new definition is
        // a move from the previous one.
        yssa_opinst* move = op( sloc, YL_MOV, 1, 1 );
        move->operand[ 0 ] = value;
        value = move;
    }

    // This op is a definition of the variable.
    value->variable = variable;
    
    // Record the definition as the latest definition in the current block.
    if ( block )
    {
        block->definitions[ variable ] = value;
    }
    
    return value;
}



/*
    Some inspiration taken from:

    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/


/*
    When a variable is referenced, we must find the definition that reaches
    the point of the reference.  We search predecessor blocks recursively,
    adding phi functions as necessary.  If we reach a block with no ancestors
    without finding a definition then the variable is undefined (this is
    an error).  If we reach a block we have already visited, then the
    definition found on that path is the same definition that reaches the
    already-visited block.
*/


static yssa_opinst* YSSA_UNDEF  = (yssa_opinst*)-1;
static yssa_opinst* YSSA_SELF   = (yssa_opinst*)-2;



yssa_opinst* yssa_builder::lookup( yssa_variable* variable )
{
    // Lookup the variable.
    if ( block )
    {
        return lookup_block( block, variable, nullptr );
    }
    else
    {
        return nullptr;
    }
}

yssa_opinst* yssa_builder::lookup_block(
        yssa_block* block, yssa_variable* variable, yssa_opinst* open_ref )
{
    // Check for a local definition.
    auto i = block->definitions.find( variable );
    if ( i != block->definitions.end() )
    {
        yssa_opinst* def = i->second;

        // Resolved references are transparent.
        while ( def->opcode == YSSA_REF && def->operand[ 0 ] )
        {
            def = def->operand[ 0 ];
        }

        return def;
    }
    
    
    // Check if we have visited this block already on our search.
    if ( block->flags & YSSA_LOOKUP )
    {
        return YSSA_SELF;
    }
    
    
    // If it's an unsealed block, then we can't proceed, add an incomplete phi.
    if ( block->flags & YSSA_UNSEALED )
    {
        yssa_opinst* incomplete = make_op( -1, YSSA_REF, 1, 1 );
        incomplete->variable = variable;
        block->phi.push_back( incomplete );
        block->definitions.emplace( variable, incomplete );
        return incomplete;
    }
    
        
    // If the variable is live in an exception handler block, mark it.
    if ( block->flags & YSSA_XCHANDLER )
    {
        variable->xcref = true;
        
        // If it has no predecessors then the only way to enter
        // this block is with an exception.  Use YSSA_VAR to indicate
        // that the current value of the variable should be used.
        if ( block->prev.empty() )
        {
            yssa_opinst* var = make_op( -1, YSSA_VAR, 0, 1 );
            var->variable = variable;
            block->phi.push_back( var );
            block->definitions.emplace( variable, var );
            return var;
        }
        
        // Otherwise there should be at least one definition of the
        // variable reaching this block.
    }
    
    
    // If the block has no predecessors then the name is undefined.
    if ( block->prev.empty() )
    {
        return YSSA_UNDEF;
    }
    

    // Mark this block to prevent infinite recursion in lookups.
    block->flags |= YSSA_LOOKUP;

    
    // If there is only one predecessor then continue search.
    if ( block->prev.size() == 1 )
    {
        yssa_opinst* def = lookup_block( block->prev.at( 0 ), variable, open_ref );
        block->flags &= ~YSSA_LOOKUP;
        return def;
    }
    
    
    // Find which definition reaches each incoming edge.
    yssa_opinst* sole_def = YSSA_SELF;
    std::vector< yssa_opinst* > defs;
    defs.reserve( block->prev.size() );
    for ( size_t i = 0; i < block->prev.size(); ++i )
    {
        yssa_opinst* def = lookup_block( block->prev.at( i ), variable, open_ref );
        
        // If any definition is undefined, so is the entire thing.
        if ( def == YSSA_UNDEF )
        {
            block->flags &= ~YSSA_LOOKUP;
            return YSSA_UNDEF;
        }
        
        // If all of the defs are the same, or YSSA_SELF, then that's the
        // sole definition which reaches this point.  Treat any reference to
        // the open ref as YSSA_SELF.
        if ( def != YSSA_SELF && def != open_ref )
        {
            if ( sole_def == YSSA_SELF )
            {
                sole_def = def;
            }
            
            if ( sole_def != def )
            {
                sole_def = nullptr;
            }
        }
    
        // Add to the list of definitions.
        defs.push_back( def );
    }

    
    // Done with lookups.
    block->flags &= ~YSSA_LOOKUP;
    
    
    // Return sole definition if the phi-function collapsed.
    if ( sole_def )
    {
        return sole_def;
    }
    
    
    // Create phi function.
    yssa_opinst* phi = make_op( -1, YSSA_PHI, defs.size(), 1 );
    phi->variable = variable;
    for ( size_t i = 0; i < defs.size(); ++i )
    {
        yssa_opinst* def = defs.at( i );
        if ( def != YSSA_SELF )
        {
            phi->operand[ i ] = def;
        }
        else
        {
            phi->operand[ i ] = phi;
        }
    }
    block->phi.push_back( phi );
    block->definitions.emplace( variable, phi );
    return phi;
}

void yssa_builder::seal_block( yssa_block* block )
{
    // Seal block.
    assert( block->flags & YSSA_UNSEALED );
    block->flags &= ~YSSA_UNSEALED;
    
    // Remove existing phi ops (which should all be refs).
    std::vector< yssa_opinst* > refs;
    std::swap( block->phi, refs );
    
    for ( size_t i = 0; i < refs.size(); ++i )
    {
        yssa_opinst* ref = refs.at( i );
        assert( ref->opcode == YSSA_REF );
        
        // Remove current definition.
        yssa_opinst* def = block->definitions.at( ref->variable );
        block->definitions.erase( ref->variable );
        
        // Perform real lookup now that the block is sealed.  Pass in the
        // open ref, because any lookup which finds that value should be
        // treated as if lookup revisited this block (similar to YSSA_SELF).
        yssa_opinst* phi = lookup_block( block, ref->variable, ref );    
        assert( phi != YSSA_UNDEF );
        assert( phi != YSSA_SELF );
        
        if ( phi != ref )
        {
            ref->operand[ 0 ] = phi;
        }
        
        // If the existing definition was the ref, lookup should find
        // the phi op in the future.
        if ( def == ref )
        {
            block->definitions[ ref->variable ] = phi;
        }
    }
    
}




void yssa_builder::call( yssa_opinst* callop )
{
    /*
        Any call or yield instruction must implicitly reference all active
        upvals, and also clobbers them (we can't prove whether or not they
        are written to by the function being called), so any references to
        them that are on the virtual stack must be replaced with temporaries.
    */
    
    // Add implicit reference to all active upvals (since they could be
    // referenced by anything in the function).
    if ( localups.size() )
    {
        yssa_opinst* o = op( callop->sloc, YSSA_IMPLICIT, localups.size(), 0 );
        o->associated = callop;
        for ( size_t i = 0; i < localups.size(); ++i )
        {
            yssa_variable* localup = localups.at( i );
            o->operand[ i ] = lookup( localup );
        }
    }
    
    // Clobber all local upvals (since they could be written by anything
    // in the function).
    for ( size_t i = 0; i < localups.size(); ++i )
    {
        clobber( localups.at( i ) );
    }
    
}


void yssa_builder::clobber( yssa_variable* v )
{
    /*
        Something (potentially) changed the value of v with a new definition.
        Any definition of v which is on the stack must be replaced with an
        explicit load, at the point where the value was pushed, to ensure that
        no two definitions of the same variable are live at the same time.
    */
    
    yssa_opinst* load = nullptr;
    yssa_opinst* load_value = nullptr;
    stack_entry* load_entry = nullptr;
    
    for ( size_t i = 0; i < stack.size(); ++i )
    {
        stack_entry& sentry = stack.at( i );

        // Locations of other values in the same block will have moved now
        // we have inserted a load.
        if ( load_entry
                && sentry.block == load_entry->block
                && sentry.index >= load_entry->index )
        {
            sentry.index += 1;
        }

        if ( sentry.value->variable != v )
        {
            continue;
        }
        
        // Potentially create load.  The oldest copy of the variable
        // definition will be lowest in the stack.
        if ( ! load )
        {
            // Create load.
            void* p = module->alloc.malloc(
                    sizeof( yssa_opinst ) + sizeof( yssa_opinst* ) * 1 );
            load = new ( p ) yssa_opinst( sentry.value->sloc, YL_MOV, 1, 1 );
            load->operand[ 0 ] = sentry.value;
            
            // Insert at point where the value was pushed.
            sentry.block->ops.insert
            (
                sentry.block->ops.begin() + sentry.index,
                load
            );
            
            // Remember this value.
            load_value = sentry.value;
            load_entry = &sentry;
        }
        else
        {
            // All definitions of v at this point should be the same.
            assert( sentry.value == load_value );
        }
        
        // Replace value on stack with the new temporary.
        sentry.value = load;
    }
    
}





/*
    Virtual stack.
*/

void yssa_builder::execute( yl_ast_node* statement )
{
    int valcount = visit( statement, 0 );
    assert( valcount == 0 || valcount == 1 );
    assert( multival == nullptr );
    if ( valcount == 1 )
    {
        stack.pop_back();
    }
}

size_t yssa_builder::push_all( yl_ast_node* expression, int* count )
{
    size_t result = stack.size();
    if ( expression )
    {
        *count = visit( expression, -1 );
    }
    else
    {
        *count = 0;
    }
    return result;
}

size_t yssa_builder::push( yl_ast_node* expression, int count )
{
    size_t result = stack.size();
    int valcount = visit( expression, count );
    assert( valcount <= count || valcount == 1 );
    if ( valcount < count )
    {
        yssa_opinst* o = op( expression->sloc, YL_NULL, 0, 1 );
        for ( int i = valcount; i < count; ++i )
        {
            push_op( o );
        }
    }
    else if ( valcount > count )
    {
        assert( count == 0 );
        assert( valcount == 1 );
        stack.pop_back();
    }
    assert( stack.size() == result + count );
    return result;
}

size_t yssa_builder::push_op( yssa_opinst* o )
{
    /*
        Don't do the following things between creating an op and pushing it:
        
            -   Pushing an expression.
            -   Assign to variable.
            -   Notify a call.
            
        We track the block location where each op is pushed, so if we need
        to insert a temporary to replace it, the temporary is created at the
        point where the op was pushed, not where it is used.
    */
    
    size_t result = stack.size();
    
    stack_entry sentry;
    sentry.block = block;
    sentry.index = block ? block->ops.size() : 0;
    sentry.value = o;
    stack.push_back( sentry );
    
    return result;
}

int yssa_builder::push_select( int sloc, yssa_opinst* selop, int count )
{
    if ( count == -1 )
    {
        multival = selop;
        return 0;
    }
    else if ( count == 1 )
    {
        push_op( selop );
        return 1;
    }
    else
    {
        for ( int i = 0; i < count; ++i )
        {
            yssa_opinst* o = op( sloc, YSSA_SELECT, 1, 1 );
            o->operand[ 0 ] = selop;
            o->select = i;
            push_op( o );
        }
        return count;
    }
}

void yssa_builder::pop( size_t index, int count, yssa_opinst** ops )
{
    assert( index + count == stack.size() );
    for ( int i = 0; i < count; ++i )
    {
        ops[ i ] = stack.at( index + i ).value;
    }
    stack.erase( stack.begin() + index, stack.begin() + index + count );
}

yssa_opinst* yssa_builder::peek( size_t index, size_t i )
{
    assert( index + i < stack.size() );
    return stack.at( index + i ).value;
}


size_t yssa_builder::push_lvalue( yl_ast_node* lvalue )
{
    size_t result;

    switch ( lvalue->kind )
    {
    case YL_EXPR_LOCAL:
    case YL_EXPR_GLOBAL:
    case YL_EXPR_UPREF:
    {
        // Don't push anything.
        result = stack.size();
        break;
    }
    
    case YL_EXPR_KEY:
    {
        // Just the object.
        yl_expr_key* keyexpr = (yl_expr_key*)lvalue;
        result = push( keyexpr->object, 1 );
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        // Object and key.
        yl_expr_inkey* inkey = (yl_expr_inkey*)lvalue;
        result = push( inkey->object, 1 );
        push( inkey->key, 1 );
        break;
    }
    
    case YL_EXPR_INDEX:
    {
        // Object and index.
        yl_expr_index* index = (yl_expr_index*)lvalue;
        result = push( index->object, 1 );
        push( index->index, 1 );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
    
    return result;
}

size_t yssa_builder::push_evalue( yl_ast_node* lvalue, size_t index )
{
    switch ( lvalue->kind )
    {
    case YL_EXPR_LOCAL:
    {
        yl_expr_local* local = (yl_expr_local*)lvalue;
        yssa_variable* v = variable( local->name );
        return push_op( lookup( v ) );
    }
    
    case YL_EXPR_GLOBAL:
    {
        yl_expr_global* global = (yl_expr_global*)lvalue;
        yssa_opinst* o = op( global->sloc, YL_GLOBAL, 0, 1 );
        o->key = module->alloc.strdup( global->name );
        return push_op( o );
    }
    
    case YL_EXPR_UPREF:
    {
        yl_expr_upref* upref = (yl_expr_upref*)lvalue;
        yssa_opinst* o = op( lvalue->sloc, YL_GETUP, 0, 1 );
        o->a = upref->index;
        return push_op( o );
    }
    
    case YL_EXPR_KEY:
    {
        yl_expr_key* keyexpr = (yl_expr_key*)lvalue;
        yssa_opinst* o = op( lvalue->sloc, YL_KEY, 1, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->key = module->alloc.strdup( keyexpr->key );
        return push_op( o );
    }
    
    case YL_EXPR_INKEY:
    {
        yssa_opinst* o = op( lvalue->sloc, YL_INKEY, 2, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->operand[ 1 ] = peek( index, 1 );
        return push_op( o );
    }
    
    case YL_EXPR_INDEX:
    {
        yssa_opinst* o = op( lvalue->sloc, YL_INDEX, 2, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->operand[ 1 ] = peek( index, 1 );
        return push_op( o );
    }
        
    default:
    {
        assert( ! "invalid lvalue" );
        yssa_opinst* o = op( lvalue->sloc, YL_NULL, 0, 1 );
        return push_op( o );
    }
    }
}

yssa_opinst* yssa_builder::assign_lvalue(
        int sloc, yl_ast_node* lvalue, size_t index, yssa_opinst* value )
{
    switch ( lvalue->kind )
    {
    case YL_EXPR_LOCAL:
    {
        yl_expr_local* local = (yl_expr_local*)lvalue;
        yssa_variable* v = variable( local->name );
        return assign( sloc, v, value );
    }
    
    case YL_EXPR_GLOBAL:
    {
        yl_expr_global* global = (yl_expr_global*)lvalue;
        yssa_opinst* o = op( global->sloc, YL_SETGLOBAL, 1, 0 );
        o->operand[ 0 ] = value;
        o->key = module->alloc.strdup( global->name );
        return value;
    }
    
    case YL_EXPR_UPREF:
    {
        yl_expr_upref* upref = (yl_expr_upref*)lvalue;
        yssa_opinst* o = op( lvalue->sloc, YL_SETUP, 1, 0 );
        o->operand[ 0 ] = value;
        o->a = upref->index;
        return value;
    }
    
    case YL_EXPR_KEY:
    {
        yl_expr_key* keyexpr = (yl_expr_key*)lvalue;
        yssa_opinst* o = op( lvalue->sloc, YL_SETKEY, 2, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->operand[ 1 ] = value;
        o->key = module->alloc.strdup( keyexpr->key );
        return value;
    }
    
    case YL_EXPR_INKEY:
    {
        yssa_opinst* o = op( lvalue->sloc, YL_SETINKEY, 3, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->operand[ 1 ] = peek( index, 1 );
        o->operand[ 2 ] = value;
        return value;
    }
    
    case YL_EXPR_INDEX:
    {
        yssa_opinst* o = op( lvalue->sloc, YL_SETINDEX, 3, 1 );
        o->operand[ 0 ] = peek( index, 0 );
        o->operand[ 1 ] = peek( index, 1 );
        o->operand[ 2 ] = value;
        return value;
    }
        
    default:
        assert( ! "invalid lvalue" );
        return value;
    }
}

void yssa_builder::pop_lvalue( yl_ast_node* lvalue, size_t index )
{
    switch ( lvalue->kind )
    {
    case YL_EXPR_LOCAL:
    case YL_EXPR_GLOBAL:
    case YL_EXPR_UPREF:
    {
        break;
    }
    
    case YL_EXPR_KEY:
    {
        // Just the object.
        yssa_opinst* value = nullptr;
        pop( index, 1, &value );
        break;
    }
    
    case YL_EXPR_INKEY:
    {
        // Object and key.
        yssa_opinst* values[ 2 ] = { nullptr, nullptr };
        pop( index, 2, values );
        break;
    }
    
    case YL_EXPR_INDEX:
    {
        // Object and index.
        yssa_opinst* values[ 2 ] = { nullptr, nullptr };
        pop( index, 2, values );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}



/*
    Scope stack.
*/

void yssa_builder::open_scope( yl_ast_scope* scope, yssa_block* xchandler )
{
    scope_entry s;
    s.scope         = scope;
    s.localups      = localups.size();
    s.itercount     = itercount;

    if ( xchandler )
        s.xchandler = xchandler;
    else if ( scopes.size() )
        s.xchandler = scopes.back().xchandler;
    else
        s.xchandler = nullptr;

    scopes.push_back( s );
}


void yssa_builder::close_scope( int sloc, yl_ast_scope* scope )
{
    const scope_entry& s = scopes.back();
    assert( s.scope == scope );

    // Close.
    close( sloc, s.localups, s.itercount );

    // Pop upvals and reset iterators.
    assert( s.localups <= localups.size() );
    assert( s.itercount <= itercount );
    localups.resize( s.localups );
    itercount = s.itercount;

    scopes.pop_back();
}


yssa_builder::break_entry* yssa_builder::open_break(
                        yl_ast_scope* scope, break_kind kind )
{
    // Create break entry.
    std::unique_ptr< break_entry > b = std::make_unique< break_entry >();
    b->scope        = scope;
    b->kind         = kind;
    b->localups     = localups.size();
    b->itercount    = itercount;

    // Add break entry to map.
    break_entry* bentry = b.get();
    breaks.emplace( break_key( scope, kind ), std::move( b ) );
    return bentry;
}

void yssa_builder::close_break( break_entry* b, yssa_block* target )
{
    if ( target )
    {
        for ( size_t i = 0; i < b->blocks.size(); ++i )
        {
            yssa_block* break_block = b->blocks.at( i );
            assert( break_block );
            link_block( break_block, NEXT, target );
        }
    }
    
    breaks.erase( break_key( b->scope, b->kind ) );
}


void yssa_builder::close( int sloc, size_t closeups, size_t closeiter )
{
    // Don't need to do anything if there's nothing to close.
    if ( closeups == localups.size() && closeiter == itercount )
    {
        return;
    }
    
    assert( closeups <= localups.size() );
    assert( closeiter <= itercount );

    // Otherwise, close everything relevant.
    size_t close_count = localups.size() - closeups;
    yssa_opinst* o = op( sloc, YL_CLOSE, close_count, 0 );
    o->a = closeups;
    o->b = closeiter;
    for ( size_t i = 0; i < close_count; ++i )
    {
        o->operand[ i ] = lookup( localups.at( closeups + i ) );
    }
}





