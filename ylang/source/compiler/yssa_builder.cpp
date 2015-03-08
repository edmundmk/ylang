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
        module->functions.emplace_back( std::move( ssaf ) );
    }
    
    
    // Build SSA for each function.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* astf = ast->functions.at( i );
        build( astf );
    }


    return diagnostics->error_count() == 0;
}





void yssa_builder::build( yl_ast_func* astf )
{
    function = funcmap.at( astf );
    
    // Create entry block.
    yssa_block_p entry_block = std::make_unique< yssa_block >( nullptr );
    block = entry_block.get();
    function->blocks.emplace_back( std::move( entry_block ) );
    
    
    // Add parameter ops.
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        yl_ast_name* param = astf->parameters.at( i );
        yssa_opinst* o = op( param->sloc, YSSA_PARAM, 0, 1 );
        o->variable = variable( param );
        o->select = (int)i;
    }

    
    // Visit the block.
    visit( astf->block, 0 );

}





int yssa_builder::visit( yl_ast_func* node, int count )
{
}

int yssa_builder::visit( yl_expr_null* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_NULL, 0, 1 );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_bool* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_BOOL, 0, 1 );
    o->a = node->value ? 1 : 0;
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_number* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_NUMBER, 0, 1 );
    o->number = node->value;
    push( o );
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
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_local* node, int count )
{
    // Find definition of the variable which reaches this point.
}

int yssa_builder::visit( yl_expr_global* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_GLOBAL, 0, 1 );
    o->key = module->alloc.strdup( node->name );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_upref* node, int count )
{
    yssa_opinst* o = op( node->sloc, YL_GETUP, 0, 1 );
    o->a = (uint8_t)node->index;
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_objref* node, int count )
{
    // Find definition of object.
}

int yssa_builder::visit( yl_expr_superof* node, int count )
{
    size_t operand = push( node->expr, 1 );
    yssa_opinst* o = op( node->sloc, YL_SUPER, 1, 1 );
    pop( operand, 1, o->operand );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_key* node, int count )
{
    size_t operand = push( node->object, 1 );
    yssa_opinst* o = op( node->sloc, YL_KEY, 1, 1 );
    pop( operand, 1, o->operand );
    o->key = module->alloc.strdup( node->key );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_inkey* node, int count )
{
    size_t operands = push( node->object, 1 );
    push( node->key, 1 );
    yssa_opinst* o = op( node->sloc, YL_INKEY, 2, 1 );
    pop( operands, 2, o->operand );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_index* node, int count )
{
    size_t operands = push( node->object, 1 );
    push( node->index, 1 );
    yssa_opinst* o = op( node->sloc, YL_INDEX, 2, 1 );
    pop( operands, 2, o->operand );
    push( o );
    return 1;
}

int yssa_builder::visit( yl_expr_preop* node, int count )
{
    size_t lvalue = push_lvalue( node->lvalue );            // lv
    size_t evalue = push_evalue( node->lvalue, lvalue );    // lv, ev

    // perform operation.
    yl_opcode opcode = ( node->opkind == YL_ASTOP_PREINC ) ? YL_ADD : YL_SUB;
    yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
    pop( evalue, 1, o->operand );
    o->operand[ 1 ] = op( node->sloc, YL_NUMBER, 0, 1 );
    o->operand[ 1 ]->number = 1.0;;
    
    // perform assignment.
    assign_lvalue( node->lvalue, lvalue, o );
    
    // push updated value onto stack.
    pop_lvalue( node->lvalue, lvalue );
    push( o );

    return 1;
}

int yssa_builder::visit( yl_expr_postop* node, int count )
{
    size_t lvalue = push_lvalue( node->lvalue );            // lv
    size_t evalue = push_evalue( node->lvalue, lvalue );    // lv, ev
    
    // perform operation
    yl_opcode opcode = ( node->opkind == YL_ASTOP_POSTINC ) ? YL_ADD : YL_SUB;
    yssa_opinst* o = op( node->sloc, opcode, 2, 1 );
    pop( evalue, 1, o->operand );
    o->operand[ 1 ] = op( node->sloc, YL_NUMBER, 0, 1 );
    o->operand[ 1 ]->number = 1.0;;
    
    // perform assignment (which might clobber the original value).
    size_t ovalue = push( o->operand[ 0 ] );
    assign_lvalue( node->lvalue, lvalue, o );
    
    // push original value onto stack.
    yssa_opinst* value = nullptr;
    pop( ovalue, 1, &value );
    pop_lvalue( node->lvalue, lvalue );
    push( value );
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
        operand = push( o );
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
    push( o );
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

    yssa_opinst* o = op( node->sloc, YL_MUL, 2, 1 );
    pop( operands, 2, o->operand );
    push( o );
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
    yssa_variable* result = node->opkinds.size() > 1 ? temporary() : nullptr;
    
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
        size_t index = push( o );
        
        yssa_opinst* b = o->operand[ 1 ];
        
        if ( opkind == YL_ASTOP_NOTIN || opkind == YL_ASTOP_NOTIS )
        {
            o = op( node->sloc, YL_LNOT, 1, 1 );
            pop( index, 1, o->operand );
            index = push( o );
        }


        // Assign to temporary.
        if ( result )
        {
            pop( index, 1, &o );
            assign( result, o );
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
        operands = push( b );
    }
    
    // Any failed comparison shortcuts to the end.
    if ( blocks.size() )
    {
        block = next_block();
        for ( size_t i = 0; i < blocks.size(); ++i )
        {
            yssa_block* shortcut = blocks.at( i );
            assert( shortcut->fail == nullptr );
            shortcut->fail = block;
            block->prev.push_back( shortcut );
        }
    }

    // Push lookup of temporary.
    if ( result )
    {
        push( lookup( result ) );
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
        
        yssa_variable* result = temporary();
        
        // Evaluate left hand side.
        size_t operand = push( node->lhs, 1 );
        yssa_opinst* value = nullptr;
        pop( operand, 1, &value );
        assign( result, value );
        
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
        assign( result, value );

        // Link in the shortcut.
        if ( block )
        {
            block = next_block();
            if ( lhs_block )
            {
                assert( lhs_block->fail == nullptr );
                lhs_block->fail = block;
                block->prev.push_back( lhs_block );
            }
        }
        
        // Push lookup of temporary.
        push( lookup( result ) );
        return 1;
    }
    
    case YL_ASTOP_LOGICXOR:
    {
        size_t operands = push( node->lhs, 1 );
        push( node->rhs, 1 );
        yssa_opinst* o = op( node->sloc, YL_LXOR, 2, 1 );
        pop( operands, 2, o->operand );
        push( o );
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
    
        yssa_variable* result = temporary();
        
        // Evaluate left hand side.
        size_t operand = push( node->lhs, 1 );
        yssa_opinst* value = nullptr;
        pop( operand, 1, &value );
        assign( result, value );
        
        // Only continue to next block if lhs is false.
        yssa_block* lhs_block = block;
        if ( block )
        {
            assert( block->test == nullptr );
            block->test = value;
            block = next_block();
            lhs_block->fail = block;
            lhs_block->next = nullptr;
        }
        
        // Evaluate right hand side.
        operand = push( node->rhs, 1 );
        pop( operand, 1, &value );
        assign( result, value );

        // Link in the shortcut.
        if ( block )
        {
            block = next_block();
            if ( lhs_block )
            {
                assert( lhs_block->next == nullptr );
                lhs_block->next = block;
                block->prev.push_back( lhs_block );
            }
        }
        
        // Push lookup of temporary.
        push( lookup( result ) );
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
        assert( block->test = nullptr );
        block->test = value;
        block = next_block();
    }
    
    // Result is a temporary since there is more than one definition.
    yssa_variable* result = temporary();
    
    // Evaluate true branch.
    operand = push( node->iftrue, 1 );
    pop( operand, 1, &value );
    assign( result, value );
    
    // False block.
    yssa_block* true_block = block;
    if ( block )
    {
        block = make_block();
        if ( test_block )
        {
            assert( test_block->fail == nullptr );
            test_block->fail = block;
            block->prev.push_back( test_block );
        }
    }
    
    // Evaluate false branch.
    operand = push( node->iffalse, 1 );
    pop( operand, 1, &value );
    assign( result, value );
    
    // Final block.
    if ( block )
    {
        block = next_block();
        if ( true_block )
        {
            assert( true_block->next == nullptr );
            true_block->next = block;
            block->prev.push_back( true_block );
        }
    }

    // Push lookup of temporary.
    push( lookup( result ) );
    return 1;
}

int yssa_builder::visit( yl_new_new* node, int count )
{
}

int yssa_builder::visit( yl_new_object* node, int count )
{
}

int yssa_builder::visit( yl_new_array* node, int count )
{
}

int yssa_builder::visit( yl_new_table* node, int count )
{
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
}

int yssa_builder::visit( yl_expr_yield* node, int count )
{
}

int yssa_builder::visit( yl_expr_vararg* node, int count )
{
}

int yssa_builder::visit( yl_expr_unpack* node, int count )
{
}

int yssa_builder::visit( yl_expr_list* node, int count )
{
}

int yssa_builder::visit( yl_expr_assign* node, int count )
{
}

int yssa_builder::visit( yl_expr_assign_list* node, int count )
{
}




yssa_variable* yssa_builder::variable( yl_ast_name* name )
{
    // TODO.
    return nullptr;
}


yssa_opinst* yssa_builder::op(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
{
    void* p = module->alloc.malloc(
            sizeof( yssa_opinst ) + sizeof( yssa_opinst* ) * operand_count );
    yssa_opinst* op = new ( p ) yssa_opinst(
            sloc, opcode, operand_count, result_count );
    
    // TODO: add to current block.
    
    return op;
}











