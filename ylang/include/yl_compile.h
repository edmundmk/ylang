//
//  yl_compile.h
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_COMPILE_H
#define YL_COMPILE_H


#include <stddef.h>
#include "yl_invoke.h"



yl_invoke yl_compile( const char* path, size_t paramc, const char* paramv[] );



template < typename ... arguments_t >
yl_invoke yl_compile( const char* path, arguments_t ... arguments )
{
    const char* paramv[] = { arguments ... };
    return yl_compile( path, sizeof ... ( arguments ), paramv );
}



#endif
