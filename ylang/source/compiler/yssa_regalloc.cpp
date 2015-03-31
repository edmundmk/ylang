//
//  yssa_regalloc.cpp
//
//  Created by Edmund Kapusniak on 27/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_regalloc.h"
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
            it does not interefere with other values assigned to the same
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
 

*/







/*
    This is how we do register allocation:
 
     -  Variables and temporaries which have a single use as an argument
            of a call-like instruction, are 'argument values'.
     -  Parameters are allocated to the appropriate registers.
     -  Perform a linear scan in reverse order to greedily allocate
            registers.  Performing the scan in reverse allows us to allocate
            argument values to the correct registers more often than not.


    The restriction on overlapping definitions of a variable means that
    all operands to a phi-function will be allocated to the same register.
    The previous compiler used a true SSA form without this restriction.
    We probably produce worse code (and the new way complicates the SSA
    construction process since temporaries needed to be generated), but:

     -  'Open' upvals can live on the stack like other variables.
     -  No special phi-handling for values used in exception handlers.
     -  Debug information is much simpler.


    The previous compiler also performed a bottom-up scan, but it attempted
    to assign _all_ arguments to the optimal register.  Again, we probably
    produce worse code, but the allocated registers are more likely to be
    tightly clustered and the total size of stack frames is likely to be
    smaller.  I think.

*/


/*
    But for now let's forget some of that and do brute-force interference
    analysis, top-down, but delaying allocation of argument ops.
*/


struct yssa_regscan
{
    std::vector< std::vector< yssa_live_range* > > registers;
    
    uint8_t stack_top( size_t address );
    uint8_t allocate_call( size_t address, yssa_live_range* range );
    uint8_t allocate( yssa_live_range* range );
    bool    allocate( uint8_t r, yssa_live_range* range );
    bool    interfere( yssa_live_range* a, yssa_live_range* b );

};


uint8_t yssa_regscan::stack_top( size_t address )
{
    uint8_t r = registers.size();
    while ( r-- )
    {
        for ( yssa_live_range* live : registers.at( r ) )
        {
            for ( ; live; live = live->next )
            {
                if ( address >= live->start || address < live->final )
                {
                    return r + 1;
                }
            }
        }
    }
    
    return 0;
}

uint8_t yssa_regscan::allocate_call( size_t address, yssa_live_range* range )
{
    for ( uint8_t r = stack_top( address ); r <= registers.size(); ++r )
    {
        if ( allocate( r, range ) )
        {
            return r;
        }
    }
    
    assert( ! "final allocation should have succeeded" );
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






void yssa_regalloc( yssa_module* module, yssa_function* function )
{
    yssa_regscan regscan;
    std::unordered_map< yssa_opinst*, std::unordered_set< yssa_opinst* > > argops;
    std::unordered_map< yssa_opinst*, std::unordered_set< yssa_variable* > > argvars;
    
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );
        
    }

}



















