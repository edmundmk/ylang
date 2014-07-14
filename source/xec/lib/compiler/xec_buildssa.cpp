//
//  xec_buildssa.cpp
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_buildssa.h"
#include <deque>



/*
    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/




struct xec_buildssa_block
{
    std::deque< xec_buildssa_block* > predecessors;
    std::unordered_map< xec_ssaname, xec_ssavalue > definitions;
    std::unordered_map< xec_ssaname, xec_ssavalue > proxy_definitions;
    bool sealed;
};





/*

xec_buildssa::xec_buildssa()
    :   next_name( 0 )
    ,   block( NULL )
{
}


xec_ssaname xec_buildssa::declare( const char* text )
{
    xec_ssaname name = next_name;
    next_name += 1;
    names.emplace( name, text );
    active_names.insert( name );
    return name;
}

void xec_buildssa::retire( xec_ssaname name )
{
    active_names.erase( name );
}




void xec_buildssa::write( xec_ssaname name, xec_ssavalue value )
{
}

xec_ssavalue xec_buildssa::read( xec_ssaname name )
{
}





void xec_buildssa::loop_begin()
{
    
}

void xec_buildssa::loop_break()
{
}

void xec_buildssa::loop_continue()
{
}

void xec_buildssa::loop_end()
{
}

void xec_buildssa::if_true( xec_ssavalue condition )
{
}

void xec_buildssa::if_false( xec_ssavalue condition )
{
}

void xec_buildssa::if_else()
{
}

void xec_buildssa::if_end()
{
}




xec_ssavalue xec_buildssa::null()
{
}

xec_ssavalue xec_buildssa::literal( bool boolean )
{
}

xec_ssavalue xec_buildssa::literal( double number )
{
}

xec_ssavalue xec_buildssa::literal( const char* string, size_t length )
{
}

xec_ssavalue xec_buildssa::emit( int opcode, xec_ssavalue a )
{
}

xec_ssavalue xec_buildssa::emit( int opcode, xec_ssavalue a, xec_ssavalue b )
{
}

void xec_buildssa::parameter( xec_ssavalue a )
{
}

xec_ssavalue xec_buildssa::multiple( int opcode, int count )
{
}

xec_ssavalue xec_buildssa::result( xec_ssavalue v, int n )
{
}


*/






