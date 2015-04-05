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

    void acquire();
    void release();


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

    static yl_program* alloc(
            size_t valcount, size_t opcount, size_t xfcount, size_t vncount );
    
    

private:

    uint16_t                    _valcount;
    size_t                      _opcount;
    size_t                      _xfcount;
    size_t                      _vncount;

    // yl_value                 _values[];
    // yl_opinst                _ops[];
    // yl_xframe                _xframes[];
    // yl_varname               _varnames[];

};



#endif




