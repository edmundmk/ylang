//
//  yl_function.h
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#ifndef YL_FUNCTION_H
#define YL_FUNCTION_H


#include <vector>
#include "yl_code.h"
#include "yl_heap.h"
#include "yl_value.h"



class yl_function;
class yl_program;
class yl_upref;




/*
    An instance of a function.
*/

class yl_function : public yl_heapobj
{
public:

    static yl_function* alloc( yl_program* program );


private:

    uint8_t                     _upcount;
    uint8_t                     _refcount;
    yl_heapref< yl_program >    _program;
    yl_heapref< yl_upref >      _uprefs[ 0 ];

};




/*
    A compiled function.
*/

class yl_program : public yl_heapobj
{
public:

    static yl_program*  alloc();
    
    

private:

    yl_heapref< yl_valarray >   _constants;
    std::vector< yl_opinst >    _opinsts;
    std::vector< yl_xframe >    _xframes;
    std::vector< yl_varname >   _varnames;

};



#endif
