//
//  yl_exception.h
//
//  Created by Edmund Kapusniak on 17/11/2015.
//  Copyright © 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_EXCEPTION_H
#define YL_EXCEPTION_H


#include <vector>
#include "yl_value.h"
#include "yl_program.h"


struct yl_exception_impl;
struct yl_unwound;



/*
    Exceptions are thrown as C++ exceptions.  This structure contains the
    exception payload - a ylang value - and the stack unwind information.
*/

struct yl_exception_impl
{
    yl_rootval                  value;
    std::vector< yl_unwound >   unwound;
};



/*
    As frames are unwound the exception tracks them so we can produce a
    stack trace if required.
*/

struct yl_unwound
{
    yl_rootref< yl_gcobject >   function;
    unsigned                    ip;
};





#endif
