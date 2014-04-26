//
//  xec_constructor.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_CONSTRUCTOR_H
#define XEC_CONSTRUCTOR_H


#include "xec_expression.h"


/*
    new expr( expr, expr )
*/

class xec_constructor_new : public xec_expression
{
public:

    xec_constructor_new( xec_expression* type, xec_expression_list* args );

};


/*
    [ expr, expr, ... ]
*/

class xec_constructor_list : public xec_expression
{
public:

    xec_constructor_list();

};


/*
    { expr : expr, expr : expr }
*/

class xec_constructor_table : public xec_expression
{
public:

    xec_constructor_table();
    
};


/*
    :{ decl; decl; }
    : expr { decl; decl; }
*/

class xec_constructor_object : public xec_expression
{
};


/*
    ?( param, param ) expr;
    ?( param, param ) { stmt; stmt; }
    .?( param, param ) expr;
    .?( param, param ) { stmt; stmt; }
*/

class xec_constructor_function : public xec_expression
{
};



#endif
