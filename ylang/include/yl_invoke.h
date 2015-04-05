//
//  yl_invoke.h
//
//  Created by Edmund Kapusniak on 28/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_INVOKE_H
#define YL_INVOKE_H


#include <cstddef>


class yl_function;


/*
    A yl_invoke is a C++ handle to a ylang function or callback.

*/


class yl_invoke
{
public:

    yl_invoke();
    yl_invoke( std::nullptr_t );
    yl_invoke& operator = ( std::nullptr_t );
    


private:

    explicit yl_invoke( yl_function* function );

    void acquire();
    void release();

    yl_function* _function;

};




#endif
