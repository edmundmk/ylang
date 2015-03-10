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
    yssa_block_p entry_block = std::make_unique< yssa_block >();
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
    // Function op.
    yssa_function* function = funcmap.at( node );
    yssa_opinst* o = op( node->sloc, YL_CLOSURE, 0, 1 );
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
    push_op( o );

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
    size_t ovalue = push_op( o->operand[ 0 ] );
    assign_lvalue( node->lvalue, lvalue, o );
    
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

    yssa_opinst* o = op( node->sloc, YL_MUL, 2, 1 );
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
        operands = push_op( b );
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
    size_t operand = push( node->proto, 1 );
    
    // Create object using prototype.
    yssa_opinst* o = op( node->sloc, YL_OBJECT, 1, 1 );
    pop( operand, 1, o->operand );
    operand = push_op( o );
    
    // Declare object (as it can potentially be referenced as an upval).
    yssa_variable* object = varobj( node );
    assign( object, o );
    
    // Execute all member initializers.
    for ( size_t i = 0; i < node->members.size(); ++i )
    {
        execute( node->members.at( i ) );
    }

    // TODO: close upval if any?
    
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
    size_t function = 0;
    int funcount = 0;
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
        push_op( o );
        
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
        push_op( o );
        
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
    push_select( node->sloc, c, count );
    return count;
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
    push_select( node->sloc, y, count );
    return count;
}

int yssa_builder::visit( yl_expr_vararg* node, int count )
{
    if ( count != 0 )
    {
        yssa_opinst* o = op( node->sloc, YL_VARARG, 0, count );
        push_select( node->sloc, o, count );
    }
    return count;
}

int yssa_builder::visit( yl_expr_unpack* node, int count )
{
    size_t operand = push( node->array, 1 );
    if ( count != 0 )
    {
        yssa_opinst* o = op( node->sloc, YL_UNPACK, 1, count );
        pop( operand, 1, o->operand );
        push_select( node->sloc, o, count );
    }
    else
    {
        yssa_opinst* value = nullptr;
        pop( operand, 1, &value );
    }
    return count;
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
        assign( v, value );
        
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
        assign_lvalue( node->lvalue, lvalue, value );
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
        assign_lvalue( node->lvalue, lvalue, o );
        pop_lvalue( node->lvalue, lvalue );
        
        // Push result value onto stack as result.
        push_op( o );
        return 1;
    }
}

int yssa_builder::visit( yl_expr_assign_list* node, int count )
{
    // This is the most complex thing ever.

    if ( node->assignop == YL_ASTOP_DECLARE )
    {
    }
    else if ( node->assignop == YL_ASTOP_ASSIGN )
    {
    }
    else
    {
    }
}




yssa_opinst* yssa_builder::op(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
{
    // Allocate op from memory region.
    void* p = module->alloc.malloc(
            sizeof( yssa_opinst ) + sizeof( yssa_opinst* ) * operand_count );
    yssa_opinst* op = new ( p ) yssa_opinst(
            sloc, opcode, operand_count, result_count );
    
    // Add to current block.
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


void yssa_builder::push_select( int sloc, yssa_opinst* selop, int count )
{
    if ( count == -1 )
    {
        multival = selop;
    }
    else if ( count == 1 )
    {
        push_op( selop );
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
    }
}










