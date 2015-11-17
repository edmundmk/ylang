//
//  yl_exception.h
//
//  Created by Edmund Kapusniak on 17/11/2015.
//  Copyright © 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_EXCEPTION_H
#define YL_EXCEPTION_H


#include "yl_value.h"


/*
    Exceptions are thrown as C++ exceptions.  This structure contains the
    exception payload - a ylang value - and the stack unwind information.
*/

struct yl_exception_impl
{
    yl_rootval value;
};



#endif
