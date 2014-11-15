//
//  yssa_ycodeout.h
//
//  Created by Edmund Kapusniak on 13/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XSSA_YCODEOUT_H
#define XSSA_YCODEOUT_H


#include "ycode.h"
#include "ymodule.h"


class yl_script;

struct yssaop;
struct yssa_block;
struct yssa_linear;
struct yssa_rtgraph;



class yssa_ycodeout
{
public:

    yssa_ycodeout();
    
    void        build_func( yssa_linear* l );
    ymodule*    build_module( yl_script* script );
    
    
private:


    void    control_flow( yssa_block* block );

    int     k( const char* key );
    int     o( yssaop* operand );
    int     o( int r );

    void    inst( ycode opcode, int r, int a, int b );
    void    inst( ycode opcode, int r, int c );

    void    value( int r, yvalue value );

    void    arguments( yssaop* op );
    size_t  call( ycode opcode, yssa_linear* l, size_t index );
    size_t  select( yssa_linear* l, size_t index );

    void    move( yssa_rtgraph* rtg );

    void    phi( yssa_rtgraph* rtg, yssa_block* from, yssa_block* to );
    
    void    label( void* label );
    void    jump( ycode opcode, int r, void* label );
    void    branch( yssaop* condition, bool iftrue, void* label );
    
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
