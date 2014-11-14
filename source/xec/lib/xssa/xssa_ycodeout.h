//
//  xssa_ycodeout.h
//
//  Created by Edmund Kapusniak on 13/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_YCODEOUT_H
#define XSSA_YCODEOUT_H


#include "ycode.h"
#include "ymodule.h"


class xec_script;

struct xssaop;
struct xssa_block;
struct xssa_linear;
struct xssa_rtgraph;



class xssa_ycodeout
{
public:

    xssa_ycodeout();
    
    void        build_func( xssa_linear* l );
    ymodule*    build_module( xec_script* script );
    
    
private:


    void    control_flow( xssa_block* block );

    int     k( const char* key );
    int     o( xssaop* operand );
    int     o( int r );

    void    inst( ycode opcode, int r, int a, int b );
    void    inst( ycode opcode, int r, int c );

    void    value( int r, yvalue value );

    void    arguments( xssaop* op );
    size_t  call( ycode opcode, xssa_linear* l, size_t index );
    size_t  select( xssa_linear* l, size_t index );

    void    move( xssa_rtgraph* rtg );

    void    phi( xssa_rtgraph* rtg, xssa_block* from, xssa_block* to );
    
    void    label( void* label );
    void    jump( ycode opcode, int r, void* label );
    void    branch( xssaop* condition, bool iftrue, void* label );
    
    void    build_value_slots( ymodule* module );
    

    struct jumplabel
    {
        int                     label;
        std::vector< int >      jumps;
    };


    std::vector< yvalue >       values;
    std::vector< ysymbol >      keys;
    std::vector< yrecipe* >     recipes;
    
    std::unordered_map< void*, jumplabel > labels;
    std::vector< yinstruction > code;
    int                         maxstack;


};




#endif
