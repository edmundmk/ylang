//
//  yl_program.h
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#ifndef YL_PROGRAM_H
#define YL_PROGRAM_H


#include <vector>
#include "yl_code.h"
#include "yl_heap.h"
#include "yl_value.h"


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
