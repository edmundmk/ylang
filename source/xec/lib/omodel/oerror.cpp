//
//  oerror.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oerror.h"
#include <stringf.h>


oerror::oerror( const char* format, ... )
{
    va_list arguments;
    va_start( arguments, format );
    message = vstringf( format, arguments );
    va_end( arguments );
}

const char* oerror::what() const throw()
{
    return message.c_str();
}




