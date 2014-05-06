//
//  xec_declaration.h
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_DECLARATION_H
#define XEC_DECLARATION_H


#include <memory>
#include <deque>



struct xec_token;
class xec_expression;
class xec_expression_call;
class xec_expression_list;
class xec_statement_compound;
class xec_constructor_object;
class xec_constructor_function;



enum xec_declaration_dispatch
{
    XEC_DECLARATION_VAR,
    XEC_DECLARATION_OBJECT,
    XEC_DECLARATION_PROTOTYPE,
    XEC_DECLARATION_FUNCTION,
};


class xec_declaration
{
public:

    virtual ~xec_declaration();

    virtual xec_declaration_dispatch    visitor_dispatch()  = 0;
    virtual int                         get_location()      = 0;
    
    virtual void set_thiscall( bool thiscall );

};



/*
    var name, name;
    var name, name = expr, exrp;
*/

class xec_declaration_var : public xec_declaration
{
public:

    xec_declaration_var( xec_token* token,
            xec_expression_list* name_list, xec_expression_list* expr_list );

    virtual xec_declaration_dispatch visitor_dispatch();
    virtual int get_location();
    
    xec_expression_list* get_name_list();
    xec_expression_list* get_expr_list();

private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  name_list;
    std::unique_ptr< xec_expression_list >  expr_list;

};


/*
    name { decl; decl; }
    name : expr { decl; decl; }
*/

class xec_declaration_object : public xec_declaration
{
public:

    xec_declaration_object(
            xec_expression* name, xec_constructor_object* object );

    virtual xec_declaration_dispatch visitor_dispatch();
    virtual int get_location();

    xec_expression* get_name();
    xec_constructor_object* get_object();

private:

    std::unique_ptr< xec_expression >           name;
    std::unique_ptr< xec_constructor_object >   object;
    
};



/*
    name( param, param )
*/

class xec_declaration_prototype : public xec_declaration
{
public:

    xec_declaration_prototype(
            xec_expression* name, xec_expression_list* params );

    virtual xec_declaration_dispatch visitor_dispatch();
    virtual int get_location();
    
    void set_thiscall( bool thiscall );
    void set_coroutine( bool coroutine );

    bool get_thiscall();
    bool get_coroutine();
    xec_expression* get_name();
    xec_expression_list* get_parameters();


protected:

    std::unique_ptr< xec_expression >       name;
    std::unique_ptr< xec_expression_list >  params;
    bool                                    thiscall;
    bool                                    coroutine;
    
};


/*
    name( param, param ) { decl, decl }
*/

class xec_declaration_function : public xec_declaration
{
public:

    xec_declaration_function(
            xec_expression* name, xec_constructor_function* function );

    virtual xec_declaration_dispatch visitor_dispatch();
    virtual int get_location();

    xec_expression* get_name();
    xec_constructor_function* get_function();

protected:

    std::unique_ptr< xec_expression >           name;
    std::unique_ptr< xec_constructor_function > function;

};




#endif





