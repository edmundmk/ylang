//
//  yssa_regalloc.cpp
//
//  Created by Edmund Kapusniak on 27/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_regalloc.h"
#include <queue>
#include <make_unique.h>
#include "yssa.h"



/*
    This is how we do register allocation:
    
     -  We identify all live ranges and the ops/variables that each range
            corresponds to.  Henceforth called values.
     -  Call-like ops participate in the VM's calling convention, and we must
            identify the 'stack top' register for each call.  We identify
            which values are live across each call op.
     -  'Argument' values have only a single use at a call-like op.
     

    We proceed thusly:

     -  There is a set of 'free' values, initially consisting of all
            non-argument values.
     -  The free value whose live range starts first is allocated such that
            it does not interfere with other values assigned to the same
            register.  We prefer:
        -  For arguments, a register based on the call op's stack top.
        -  For selects or calls, a register based on the call op's stack top.
        -  For parameters, the register
        -  The lowest numbered register.
     -  If all values which are live across a particular call op have been
            allocated, then we determine the stack top for that call.
            Argument values for the call are added to the set of free ops.


    Note that if there are no values live across a particular call then its
    arguments are in the initial free set.

    Essentially it's a greedy linear scan algorithm, but we treat argument
    values specially - looking ahead to place call ops at the correct place
    in the stack, then backtracking to place arguments.
    
    
    Since an argument value cannot span its call op (or any outer call op
    which has that call as an argument), the algorithm should always complete
    after allocating all values.
    
    
    The benefit from delaying allocation of arguments until the associated
    call's stack top is identified is probably quite small.  It's likely that
    arguments will be encountered after all values live across the call,
    meaning that in most cases there will be no delay after all.
 

*/



/*
    Make each value and each call op explicit.
*/


struct yssa_regvalue;
struct yssa_regcall;


typedef std::unique_ptr< yssa_regvalue > yssa_regvalue_p;
typedef std::unique_ptr< yssa_regcall > yssa_regcall_p;


struct yssa_regvalue
{
    yssa_live_range*                live;
    yssa_variable*                  variable;
    yssa_opinst*                    argof;
    std::vector< yssa_opinst* >     ops;
    std::vector< yssa_regcall* >    across;
};


struct yssa_regcall
{
    size_t                          index;
    yssa_opinst*                    op;
    std::vector< yssa_regvalue* >   arguments;
    size_t                          across_count;
};


struct yssa_regvalue_priority
{
    bool operator() ( yssa_regvalue* a, yssa_regvalue* b ) const
    {
        // First op has highest priority.
        return a->live->start >= b->live->start;
    }
};


typedef std::priority_queue
        <
            yssa_regvalue*,
            std::vector< yssa_regvalue* >,
            yssa_regvalue_priority
        >
        yssa_regvalue_queue;




/*
    Allocation record.
*/


struct yssa_regscan
{
    std::vector< std::vector< yssa_live_range* > > registers;
    
    uint8_t stacktop( size_t address );
    uint8_t allocate( yssa_live_range* range );
    bool    allocate( uint8_t r, yssa_live_range* range );
    bool    interfere( yssa_live_range* a, yssa_live_range* b );

};


uint8_t yssa_regscan::stacktop( size_t address )
{
    uint8_t r = registers.size();
    while ( r-- )
    {
        for ( yssa_live_range* live : registers.at( r ) )
        {
            for ( ; live; live = live->next )
            {
                if ( address >= live->start && address < live->final )
                {
                    return r + 1;
                }
            }
        }
    }
    
    return 0;
}

uint8_t yssa_regscan::allocate( yssa_live_range* range )
{
    for ( uint8_t r = 0; r <= registers.size(); ++r )
    {
        if ( allocate( r, range ) )
        {
            return r;
        }
    }
    
    assert( ! "final allocation should have succeeded" );
    return 0;
}

bool yssa_regscan::allocate( uint8_t r, yssa_live_range* range )
{
    if ( r >= registers.size() )
    {
        registers.resize( r + 1 );
        registers.at( r ).push_back( range );
        return true;
    }
    
    for ( yssa_live_range* live : registers.at( r ) )
    {
        if ( interfere( range, live ) )
        {
            return false;
        }
    }
    
    registers.at( r ).push_back( range );
    return true;
}

bool yssa_regscan::interfere( yssa_live_range* a, yssa_live_range* b )
{
    while ( true )
    {
        // Skip b spans that are wholly before the current a span.
        if ( b->final <= a->start )
        {
            b = b->next;
            if ( ! b )
            {
                // All b spans are before current a - no interference.
                return false;
            }
            continue;
        }
        
        // Skip a spans that are wholly before the current b span.
        if ( a->final <= b->start )
        {
            a = a->next;
            if ( ! a )
            {
                // All a spans are before current b - no interference.
                return false;
            }
            continue;
        }
        
        // Otherwise a and b overlap.
        return true;
    }
}



static uint8_t preferred_stacktop( yssa_function* function, yssa_regcall* call )
{
    auto i = function->argof.find( call->op );
    if ( i == function->argof.end() )
    {
        return yl_opinst::NOVAL;
    }
    
    // The call is itself an argument.
    yssa_opinst* argof = i->second;
    assert( argof->stacktop == yl_opinst::NOVAL );
    for ( size_t i = 0; i < argof->operand_count; ++i )
    {
        if ( argof->operand[ i ] == call->op )
        {
            return argof->stacktop + i;
        }
        
        if ( argof->has_multival() && argof->multival == call->op )
        {
            return argof->stacktop + argof->operand_count;
        }
    }
    
    return yl_opinst::NOVAL;
}



static uint8_t preferred_register( yssa_regvalue* value )
{
    // Check if the value is an argument.
    if ( value->argof )
    {
        yssa_opinst* argof = value->argof;
        assert( argof->is_call() );
        assert( argof->stacktop != yl_opinst::NOVAL );

        for ( yssa_opinst* op : value->ops )
        {
            for ( size_t i = 0; i < argof->operand_count; ++i )
            {
                if ( argof->operand[ i ] == op )
                {
                    return argof->stacktop + i;
                }
                
                if ( argof->has_multival() && argof->multival == op )
                {
                    return argof->stacktop + argof->operand_count;
                }
            }
        }
    }
    
    // Other preferred clauses depend on there being a single definition.
    if ( value->ops.size() != 1 )
    {
        return yl_opinst::NOVAL;
    }
    
    yssa_opinst* op = value->ops.at( 0 );
    
    // Check if the value is a call itself.
    if ( op->is_call() )
    {
        assert( op->stacktop != yl_opinst::NOVAL );
        return op->stacktop;
    }
    
    // Check if the value is a select.
    if ( op->opcode == YSSA_SELECT && op->operand[ 0 ]->is_call() )
    {
        assert( op->operand[ 0 ]->stacktop != yl_opinst::NOVAL );
        return op->operand[ 0 ]->stacktop + op->select;
    }
    
    // Or a parameter.
    if ( op->opcode == YSSA_PARAM )
    {
        // On entry, register 0 is the function object, with parameters after.
        return 1 + op->select;
    }
    
    return yl_opinst::NOVAL;
}



void yssa_regalloc( yssa_module* module, yssa_function* function )
{
    // Build values.
    std::unordered_map< void*, yssa_regvalue_p > values;
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );

        if ( ! op->live )
            continue;

        if ( op->variable )
        {
            auto j = values.find( op->variable );
            if ( j != values.end() )
            {
                yssa_regvalue* value = j->second.get();
                value->ops.push_back( op );
            }
            else
            {
                yssa_regvalue_p value = std::make_unique< yssa_regvalue >();
                value->live     = op->variable->live;
                value->variable = op->variable;
                value->argof    = op->variable->argof;
                value->ops.push_back( op );
                values.emplace( op->variable, std::move( value ) );
            }
        }
        else
        {
            assert( ! values.count( op ) );
            yssa_regvalue_p value = std::make_unique< yssa_regvalue >();
            value->live     = op->live;
            value->variable = nullptr;
            
            auto j = function->argof.find( op );
            if ( j != function->argof.end() )
            {
                value->argof = j->second;
            }
            else
            {
                value->argof = nullptr;
            }
            
            value->ops.push_back( op );
            values.emplace( op, std::move( value ) );
        }
    }
    
    
    // Build calls.
    std::vector< yssa_regcall_p > calls;
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );

        if ( ! op->is_call() )
        {
            continue;
        }
        
        yssa_regcall_p call = std::make_unique< yssa_regcall >();
        call->index         = i;
        call->op            = op;
        call->across_count  = 0;
        
        for ( const auto& v : values )
        {
            for ( yssa_live_range* live = v.second->live;
                        live; live = live->next )
            {
                // Note that if i == live->start then the live range begins at
                // the call itself.  Calls are not live across themselves.
                if ( i > live->start && i < live->final )
                {
                    v.second->across.push_back( call.get() );
                    call->across_count += 1;
                }
            }
            
            if ( v.second->argof == op )
            {
                call->arguments.push_back( v.second.get() );
            }
        }
        
        if ( call->across_count == 0 )
        {
            // No variables are live across the call.
            call->op->stacktop = 0;
            for ( yssa_regvalue* value : call->arguments )
            {
                value->argof = nullptr;
            }
        }
        
        calls.push_back( std::move( call ) );
    }
    
    
    // Identify free values.
    yssa_regvalue_queue free_values;
    for ( const auto& v : values )
    {
        yssa_regvalue* value = v.second.get();

        // Sort calls this value is across so that the last
        // call is allocated first.
        std::sort
        (
            value->across.begin(),
            value->across.end(),
            []( yssa_regcall* a, yssa_regcall* b )
            {
                return a->index > b->index;
            }
        );
        
        // If the value isn't an argument, it's free.
        if ( ! value->argof )
        {
            free_values.push( v.second.get() );
        }
    }
    
    
    // Perform allocations.
    yssa_regscan regscan;
    while ( free_values.size() )
    {
        yssa_regvalue* value = free_values.top();
        free_values.pop();
        
        // Identify preferred register (if any).
        uint8_t r = preferred_register( value );
        
        // Attempt to allocate to preferred register.
        if ( r != yl_opinst::NOVAL && ! regscan.allocate( r, value->live ) )
        {
            r = yl_opinst::NOVAL;
        }
        
        // Allocate to any register.
        if ( r == yl_opinst::NOVAL )
        {
            r = regscan.allocate( value->live );
        }
        
        // Set value.
        if ( value->variable )
        {
            assert( value->variable->r == yl_opinst::NOVAL );
            value->variable->r = r;
        }
        
        for ( yssa_opinst* op : value->ops )
        {
            assert( op->r == yl_opinst::NOVAL );
            op->r = r;
        }
        
        // Update calls now that this value has been allocated.
        for ( yssa_regcall* call : value->across )
        {
            assert( call->across_count > 0 );
            call->across_count -= 1;
            if ( call->across_count == 0 )
            {
                // All values live across this call have been allocated,
                // find the stack top at this call.
                
                // Find top of stack.
                uint8_t stacktop = regscan.stacktop( call->index );
                
                // Identify preferred stack top (if any).
                uint8_t preferred = preferred_stacktop( function, call );
                if ( preferred != yl_opinst::NOVAL && preferred >= stacktop )
                {
                    stacktop = preferred;
                }
                
                // Assign stack top.
                assert( call->op->stacktop == yl_opinst::NOVAL );
                call->op->stacktop = stacktop;
                
                // All arguments to this call are now free to be allocated.
                for ( yssa_regvalue* argument : call->arguments )
                {
                    assert( argument->argof == call->op );
                    free_values.push( argument );
                }
                
            }
            
        }
        
    }


}























