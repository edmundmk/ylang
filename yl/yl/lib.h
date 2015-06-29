//
//  lib.h
//
//  Created by Edmund Kapusniak on 04/05/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef LIB_H


#include <ylang.h>


namespace lib
{


yl_call int         atoi( const char* string );
yl_call double      max( double a, double b );
yl_call void        printf( yl_callframe& xf );
yl_call std::string sprintf( yl_callframe& xf );
yl_call double      sqrt( double x );


}


#endif
