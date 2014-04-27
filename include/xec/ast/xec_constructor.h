//
//  xec_constructor.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_CONSTRUCTOR_H
#define XEC_CONSTRUCTOR_H


#include "xec_expression.h"


class xec_statement_compound;


/*
    new expr( expr, expr )
*/

class xec_constructor_new : public xec_expression
{
public:

    xec_constructor_new(
        xec_token* token, xec_expression* type, xec_expression_list* args );

};


/*
    [ expr, expr, ... ]
*/

class xec_constructor_list : public xec_expression
{
public:

    xec_constructor_list();

    void append_value( xec_expression* value );
    void append_final( xec_expression* final );

};


/*
    { expr : expr, expr : expr }
*/

class xec_constructor_table : public xec_expression
{
public:

    xec_constructor_table();
    
    void append_keyval( xec_expression* key, xec_expression* value );
    
};


/*
    :{ decl; decl; }
    : expr { decl; decl; }
*/

class xec_constructor_object : public xec_expression
{


};


/*
    ?( param, param ) { stmt; stmt; }
    ?( param, param ) yield { stmt; stmt; }
    .?( param, param ) { stmt; stmt; }
    .?( param, param ) yield { stmt; stmt; }
*/

class xec_constructor_function : public xec_expression
{
public:

    xec_constructor_function(
            xec_token* token,
            xec_expression_list* params,
            xec_statement_compound* body );
    
    bool set_thiscall( bool thiscall );
    bool set_coroutine( bool coroutine );

};



#endif
