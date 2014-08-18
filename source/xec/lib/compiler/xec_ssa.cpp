//
//  xec_ssa.cpp
//
//  Created by Edmund Kapusniak on 10/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa.h"


/*

    Some ideas for compiler optimizations:
        
        
    prune phi
        any referece to a phi-instruction that refers to a single value can
        be replaced with that value.  this is more a weakness of our ssa
        construction than anything else.

    fold constants
        3 * 4 can be replaced with 12

    not removal
        Lots to do here.
        if ( ! a ) can eliminate the not by swapping iftrue and iffalse.
        ! ! a -> (bool)a, and ! ! ! a -> ! a.  ! a ^^ b -> a xnor b, and 
        ! a ^^ ! b -> a ^^ b.

    common subexpression elimination
        we can avoid redoing operations, especially lookups.  Note that xec
        lookups always do the same thing.  See aliasing issues.

    redundant upval store removal
        setup instructions which do not reach any function call can be
        removed refup instructions can be replaced by references to the set
        value of the upval, if there is no call in the way.
        
        eg.
            not optimizable optimizable optimized
            
            setup           setup       --
            call            refup       (replace with value used in setup)
            refup           setup       setup (required to expose to call)
                            call        call
            
        possibly we can exploit information about whether any given upval is
        read or write-only inside closures?
            
    redundant store removal
        similarly, with lookups, a.b = 4; a.b = 5; the first statement is
        redundant, and q( a.b, a.b ) both values are the same.
        note that stores to different constant keys cannot alias.  inkey,
        call, and stores to the same constant key, do alias.  Note that xec
        lookups cannot be overloaded and don't have side effects.

    loop invariant code motion
        we can move operations outside of loops.

    early type checking
        if we know a parameter/upval is going to be used as a number, check if
        it's a number at the start, and then the interpreter/jit can assume it
        passed the check when it comes time to run it.  This is because xec
        doesn't have operator overloading - arithmetic operations always
        operate on numbers.
        
    inlining
        if we know which function is being called (when is this?).


*/





xec_ssa_name::xec_ssa_name( int sloc, const char* name )
    :   sloc( sloc )
    ,   name( name )
{
}




xec_ssa::xec_ssa()
    :   script( NULL )
    ,   function( NULL )
{
}

xec_ssa::~xec_ssa()
{
}



xec_ssa_func::xec_ssa_func( int sloc, const char* funcname )
    :   sloc( sloc )
    ,   funcname( funcname )
    ,   block( NULL )
    ,   upvalcount( 0 )
    ,   localupcount( 0 )
    ,   paramcount( 0 )
    ,   varargs( false )
    ,   coroutine( false )
{
}



xec_ssa_block::xec_ssa_block()
    :   index( -1 )
    ,   condition( XEC_SSA_INVALID )
    ,   pre( NULL )
    ,   phi( NULL )
    ,   ops( NULL )
    ,   iftrue( NULL )
    ,   iffalse( NULL )
{
}



xec_ssa_slice::xec_ssa_slice( int index )
    :   index( index )
{
}



