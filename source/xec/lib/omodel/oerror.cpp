//
//  oerror.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oerror.h"


oerror::oerror( const char* error )
    :   error( error )
{
}

const char* oerror::what() const throw()
{
    return error;
}
