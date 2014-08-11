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
    lookups always do the same thing.

redundant upval store removal
    setup instructions which do not reach any function call can be removed
    refup instructions can be replaced by references to the set value of the
    upval, if there is no call in the way.
    
    eg.
        not optimizable     optimizable     optimized
        
        setup               setup           --
        call                refup           (replace with value used in setup)
        refup               setup           setup (required to expose to call)
                            call            call
        
    possibly we can exploit information about whether any given upval is
    read or write-only inside closures?
        
redundant store removal
    similarly, with lookups, a.b = 4; a.b = 5; the first statement is
    redundant, and q( a.b, a.b ) both values are the same.
    note that stores to different constant keys cannot alias.  inkey, call,
    and stores to the same constant key, do alias.  Note that xec lookups
    cannot be overloaded and don't have side effects.

loop invariant code motion
    we can move operations outside of loops.

early type checking
    if we know a parameter/upval is going to be used as a number, check if
    it's a number at the start, and then the interpreter/jit can assume it
    passed the check when it comes time to run it.  This is because xec
    doesn't have operator overloading - arithmetic operations always
    operate on numbers.


*/






xec_ssa::xec_ssa()
    :   script( NULL )
    ,   function( NULL )
{
}

xec_ssa::~xec_ssa()
{
}



xec_ssa_func::xec_ssa_func( int sloc, const char* funcname,
            xec_ssa_block* block, int upvalcount, int paramcount,
                    bool varargs, bool coroutine )
    :   sloc( sloc )
    ,   funcname( funcname )
    ,   block( block )
    ,   upvalcount( upvalcount )
    ,   localupcount( 0 )
    ,   paramcount( paramcount )
    ,   varargs( varargs )
    ,   coroutine( coroutine )
{
}



xec_ssa_name::xec_ssa_name( int sloc, const char* name )
    :   sloc( sloc )
    ,   name( name )
{
}


xec_ssa_block::xec_ssa_block()
    :   condition( NULL )
    ,   iftrue( NULL )
    ,   iffalse( NULL )
{
}



xec_ssa_node::xec_ssa_node( int sloc, xec_ssa_opcode opcode )
    :   sloc( sloc )
    ,   opcode( opcode )
{
}



xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( NULL )
    ,   operandb( NULL )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   operandb( NULL )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_O );
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
            xec_ssa_node* operanda, xec_ssa_node* operandb )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operanda )
    ,   operandb( operandb )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_OO );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand, const char* literal )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   literal( literal )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_OL );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, const char* literal )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( NULL )
    ,   literal( literal )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_L );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand, int immediate )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   immediate( immediate )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_OI );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, int immediate )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( NULL )
    ,   immediate( immediate )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_I );
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, const char* string, size_t length )
    :   xec_ssa_node( sloc, opcode )
    ,   string( string )
    ,   length( length )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_S );
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode, bool boolean )
    :   xec_ssa_node( sloc, opcode )
    ,   boolean( boolean )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_B );
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode, double number )
    :   xec_ssa_node( sloc, opcode )
    ,   number( number )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_PACKED_N );
}
    


xec_ssa_triple::xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, xec_ssa_node* index, xec_ssa_node* v )
    :   xec_ssa_node( sloc, opcode )
    ,   object( object )
    ,   index( index )
    ,   value( v )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_TRIPLE );
}

xec_ssa_triple::xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, const char* key, xec_ssa_node* v )
    :   xec_ssa_node( sloc, opcode )
    ,   object( object )
    ,   key( key )
    ,   value( v )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_TRIPLE_K );
}



xec_ssa_expand::xec_ssa_expand( int sloc,
                xec_ssa_opcode opcode, int valcount )
    :   xec_ssa_node( sloc, opcode )
    ,   unpacked( NULL )
    ,   valcount( valcount )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_EXPAND );
}

xec_ssa_expand::xec_ssa_expand( int sloc,
                xec_ssa_opcode opcode, xec_ssa_func* func )
    :   xec_ssa_node( sloc, opcode )
    ,   func( func )
{
    assert( xec_ssa_decode( opcode ) == XEC_SSA_EXPAND_F );
}




