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
class xec_scope;


/*
    new expr( expr, expr )
*/

class xec_constructor_new : public xec_expression
{
public:

    xec_constructor_new( xec_token* token,
                    xec_expression* proto, xec_expression_list* args );

    virtual xec_expression_kind get_kind();
    virtual int get_location();
    
    xec_expression* get_proto();
    xec_expression_list* get_arguments();

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

    virtual xec_expression_kind get_kind();
    virtual int get_location();

    void set_token( xec_token* token );
    void append_value( xec_expression* value );
    void append_final( xec_expression* final );
    
    size_t get_count();
    xec_expression* get_expr( size_t index );
    xec_expression* get_final();
    
    
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
    
    virtual xec_expression_kind get_kind();
    virtual int get_location();
    
    void set_token( xec_token* token );
    void append_keyval( xec_expression* key, xec_expression* value );
    
    size_t get_count();
    xec_expression* get_key( size_t index );
    xec_expression* get_value( size_t index );
    
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

    virtual xec_expression_kind get_kind();
    virtual int get_location();

    void set_token( xec_token* token );
    void set_proto( xec_expression* proto );
    void add_member( xec_declaration* decl );
    void set_scope( xec_scope* scope );

    xec_expression* get_proto();
    size_t get_member_count();
    xec_declaration* get_member( size_t index );
    xec_scope* get_scope();

private:

    xec_token*                                          token;
    std::unique_ptr< xec_expression >                   proto;
    std::deque< std::unique_ptr< xec_declaration > >    members;
    xec_scope*                                          scope;

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

    xec_constructor_function( xec_expression_list* params );
    
    virtual xec_expression_kind get_kind();
    virtual int get_location();
    
    void set_token( xec_token* token );
    void set_varargs( bool varargs );
    void set_thiscall( bool thiscall );
    void set_coroutine( bool coroutine );
    void set_body( xec_statement_compound* body );
    void set_scope( xec_scope* scope );
    
    bool get_varargs();
    bool get_thiscall();
    bool get_coroutine();
    xec_expression_list* get_parameters();
    xec_statement_compound* get_body();
    xec_scope* get_scope();
    

private:

    xec_token*                                  token;
    std::unique_ptr< xec_expression_list >      params;
    std::unique_ptr< xec_statement_compound >   body;
    xec_scope*                                  scope;
    bool                                        varargs;
    bool                                        thiscall;
    bool                                        coroutine;

};


#endif

