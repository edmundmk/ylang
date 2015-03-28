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
 
     -  All ops are flattened out into a single list.
     -  Live ranges for each variable and for each op are calculated.  Our
            modified SSA form mandates that live ranges of definitions of
            the same variable do not overlap.  Live ranges can have holes,
            where ops are not live in particular blocks.  Variables
            referenced from exception handlers are live throughout all
            blocks protected by that handler.
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


void yssa_regalloc( yssa_function* function )
{
    // Calculate live ranges for each op and for each variable.
    
}










