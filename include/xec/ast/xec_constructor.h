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
class xec_declaration;


/*
    new expr( expr, expr )
*/

class xec_constructor_new : public xec_expression
{
public:

    xec_constructor_new( xec_token* token,
                    xec_expression* proto, xec_expression_list* args );

    virtual int get_location();

private:

    xec_token*                              token;
    std::unique_ptr< xec_expression >       proto;
    std::unique_ptr< xec_expression_list >  args;


};


/*
    [ expr, expr, ... ]
*/

class xec_constructor_list : public xec_expression
{
public:

    xec_constructor_list();

    virtual int get_location();

    void set_token( xec_token* token );
    void append_value( xec_expression* value );
    void append_final( xec_expression* final );
    
    
private:

    xec_token*                                      token;
    std::deque< std::unique_ptr< xec_expression > > values;
    std::unique_ptr< xec_expression >               final;

};


/*
    { expr : expr, expr : expr }
*/

class xec_constructor_table : public xec_expression
{
public:

    xec_constructor_table();
    
    virtual int get_location();
    
    void set_token( xec_token* token );
    void append_keyval( xec_expression* key, xec_expression* value );
    
private:

    struct keyval
    {
        std::unique_ptr< xec_expression > key;
        std::unique_ptr< xec_expression > value;
    };

    xec_token*              token;
    std::deque< keyval >    keyvals;
    
};


/*
    :{ decl; decl; }
    : expr { decl; decl; }
*/

class xec_constructor_object : public xec_expression
{
public:

    xec_constructor_object();

    virtual int get_location();

    void set_token( xec_token* token );
    void set_proto( xec_expression* proto );
    void add_member( xec_declaration* decl );


private:

    xec_token*                                          token;
    std::unique_ptr< xec_expression >                   proto;
    std::deque< std::unique_ptr< xec_declaration > >    members;

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

    xec_constructor_function( xec_token* token,
            xec_expression_list* params, xec_statement_compound* body );
    
    virtual int get_location();
    
    void set_thiscall( bool thiscall );
    void set_coroutine( bool coroutine );

private:

    xec_token*                                  token;
    std::unique_ptr< xec_expression_list >      params;
    std::unique_ptr< xec_statement_compound >   body;
    bool                                        thiscall;
    bool                                        coroutine;

};



#endif
