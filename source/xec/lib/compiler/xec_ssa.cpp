//
//  xec_ssa.cpp
//
//  Created by Edmund Kapusniak on 10/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa.h"





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
    ,   paramcount( paramcount )
    ,   varargs( varargs )
    ,   coroutine( coroutine )
{
}



xec_ssa_node::xec_ssa_node( int sloc, xec_ssa_opcode opcode )
    :   sloc( sloc )
    ,   opcode( opcode )
{
}

xec_ssa_packed* xec_ssa_node::as_packed()
{
    // TODO: assert that we are the correct type.
    return (xec_ssa_packed*)this;
}

xec_ssa_triple* xec_ssa_node::as_triple()
{
    // TODO: assert that we are the correct type.
    return (xec_ssa_triple*)this;
}

xec_ssa_expand* xec_ssa_node::as_expand()
{
    // TODO: assert that we are the correct type.
    return (xec_ssa_expand*)this;
}




xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( NULL )
    ,   operandb( NULL )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   operandb( NULL )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode,
            xec_ssa_node* operanda, xec_ssa_node* operandb )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operanda )
    ,   operandb( operandb )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand, const char* literal )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   literal( literal )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, xec_ssa_node* operand, int immediate )
    :   xec_ssa_node( sloc, opcode )
    ,   operanda( operand )
    ,   immediate( immediate )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc,
            xec_ssa_opcode opcode, const char* string, size_t length )
    :   xec_ssa_node( sloc, opcode )
    ,   string( string )
    ,   length( length )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode, bool boolean )
    :   xec_ssa_node( sloc, opcode )
    ,   boolean( boolean )
{
}

xec_ssa_packed::xec_ssa_packed( int sloc, xec_ssa_opcode opcode, double number )
    :   xec_ssa_node( sloc, opcode )
    ,   number( number )
{
}
    


xec_ssa_triple::xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, xec_ssa_node* index, xec_ssa_node* v )
    :   xec_ssa_node( sloc, opcode )
    ,   object( object )
    ,   index( index )
    ,   value( v )
{
}

xec_ssa_triple::xec_ssa_triple( int sloc, xec_ssa_opcode opcode,
                xec_ssa_node* object, const char* key, xec_ssa_node* v )
    :   xec_ssa_node( sloc, opcode )
    ,   object( object )
    ,   key( key )
    ,   value( v )
{
}



xec_ssa_expand::xec_ssa_expand( int sloc,
                xec_ssa_opcode opcode, int valcount )
    :   xec_ssa_node( sloc, opcode )
    ,   unpacked( NULL )
    ,   valcount( valcount )
{
}

xec_ssa_expand::xec_ssa_expand( int sloc,
                xec_ssa_opcode opcode, xec_ssa_func* func )
    :   xec_ssa_node( sloc, opcode )
    ,   func( func )
{
}




