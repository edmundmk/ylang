//
//  xec_context.h
//
//  Created by Edmund Kapusniak on 29/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_CONTEXT_H
#define XEC_CONTEXT_H


#include <vector>
#include "xec_value.h"
#include "xec_object.h"


class xec_function;


class xec_context
{
public:

    xec_context();
    ~xec_context();

    void execute( xec_function* function, const char* params, ... );


private:

    void execute( unsigned fp, unsigned acount, unsigned rcount );

    void check_number( xec_value v );
    void check_integer( xec_value v );
    void check_string( xec_value v );
    void check_object( xec_value v );

    xec_object*                 g;
    std::vector< xec_value >    s;
    std::vector< xec_upval* >   u;
    bool                        iter;

};





#endif
