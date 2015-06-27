//
//  yl_exception.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "ylang.h"
#include <stringf.h>




yl_exception::yl_exception( const char* format, ... )
{
    va_list arguments;
    va_start( arguments, format );
    message = vstringf( format, arguments );
    va_end( arguments );
}


const char* yl_exception::what() const throw()
{
    return message.c_str();
}



