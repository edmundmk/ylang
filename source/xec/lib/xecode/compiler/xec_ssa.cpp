//
//  xec_ssa.cpp
//
//  Created by Edmund Kapusniak on 10/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa.h"


/*

    Some ideas for compiler optimizations:
        
        
    prune ref
        all references to ref instructions could be replaced with references
        to the target of the ref...

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
        
    explicit incref/decref
        if we keep with incref/decref on the stack...
 
    inlining
        if we know which function is being called (when is this?).


*/



template < typename containera_t, typename containerb_t >
static inline void extend( containera_t* a, const containerb_t& b )
{
    a->insert( a->end(), b.begin(), b.end() );
}



void xec_ssa_op::get_operands( std::vector< xec_ssa_opref >* operands )
{
    operands->clear();
    
    if ( opcode >= XEC_SSA_FIRST_REF
            && opcode <= XEC_SSA_LAST_REF )
    {
        operands->reserve( 2 );
        if ( operanda )
        {
            operands->push_back( operanda );
        }
        if ( operandb )
        {
            operands->push_back( operandb );
        }
    }
    else if ( opcode >= XEC_SSA_FIRST_KEY
                && opcode <= XEC_SSA_LAST_KEY )
    {
        operands->reserve( 1 );
        if ( operanda )
        {
            operands->push_back( operanda );
        }
    }
    else if ( opcode >= XEC_SSA_FIRST_IMM
                && opcode <= XEC_SSA_LAST_IMM )
    {
        operands->reserve( 1 );
        if ( operanda )
        {
            operands->push_back( operanda );
        }
    }
    else if ( opcode >= XEC_SSA_FIRST_SET
                && opcode <= XEC_SSA_LAST_SET )
    {
        operands->reserve( 3 );
        if ( operanda )
        {
            operands->push_back( operanda );
        }
        if ( opcode != XEC_SSA_SETKEY && operandb )
        {
            operands->push_back( operandb );
        }
        if ( operandv )
        {
            operands->push_back( operandv );
        }
    }
    else if ( opcode >= XEC_SSA_FIRST_ARG
                && opcode <= XEC_SSA_LAST_ARG )
    {
        operands->reserve( args->args.size() + 1 );
        extend( operands, args->args );
        if ( args->unpacked )
        {
            operands->push_back( args->unpacked );
        }
    }
    else if ( opcode == XEC_SSA_PHI )
    {
        operands->reserve( phi->definitions.size() );
        extend( operands, phi->definitions );
    }
    else if ( opcode == XEC_SSA_CLOSURE )
    {
        operands->reserve( closure->upvals.size() );
        extend( operands, closure->upvals );
    }



}





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
    ,   live( NULL )
    ,   phi( NULL )
    ,   ops( NULL )
    ,   iftrue( NULL )
    ,   iffalse( NULL )
{
}



xec_ssa_slice::xec_ssa_slice( int index, xec_ssa_block* block )
    :   index( index )
    ,   block( block )
{
}



