//
//  yssa.cpp
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa.h"




yssa_opinst::yssa_opinst(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   operand_count( operand_count )
    ,   result_count( result_count )
    ,   r( yl_opinst::NOVAL )
    ,   variable( nullptr )
    ,   number( 0.0 )
{
    for ( size_t i = 0; i < operand_count; ++i )
    {
        operand[ i ] = nullptr;
    }
}




yssa_string::yssa_string( const char* string, size_t length )
    :   string( string )
    ,   length( length )
{
}




yssa_function::yssa_function( int sloc, const char* funcname )
    :   sloc( sloc )
    ,   funcname( funcname )
{
}





yssa_block::yssa_block( unsigned flags )
    :   flags( flags )
    ,   test( nullptr )
    ,   next( nullptr )
    ,   fail( nullptr )
    ,   xchandler( nullptr )
{
}



