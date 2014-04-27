//
//  xec_declaration.h
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_DECLARATION_H
#define XEC_DECLARATION_H


class xec_expression;
class xec_expression_call;
class xec_expression_list;
class xec_statement_compound;


class xec_declaration
{
public:

    virtual ~xec_declaration();

    virtual void set_thiscall( bool thiscall );

};



/*
    var name, name;
    var name, name = expr, exrp;
*/

class xec_declaration_var : public xec_declaration
{
public:

    xec_declaration_var(
            xec_expression_list* name_list, xec_expression_list* expr_list );

};


/*
    name { decl; decl; }
    name : expr { decl; decl; }
*/

class xec_declaration_object : public xec_declaration
{
public:

    xec_declaration_object();

    void set_name( xec_expression* name );
    void set_prototype( xec_expression* prototype );
    void add_declaration( xec_declaration* declaration );
  
    xec_constructor_object* as_constructor( xec_token* token );
    
};



/*
    name( param, param )
*/

class xec_declaration_prototype : public xec_declaration
{
public:

    xec_declaration_prototype(
            xec_expression* name, xec_expression_list* params );

    void set_thiscall( bool thiscall );
    void set_coroutine( bool coroutine );
    

};


/*
    name( param, param ) { decl, decl }
*/

class xec_declaration_function : public xec_declaration_prototype
{
public:

    xec_declaration_function(
            xec_expression* name, xec_expression_list* params );

    void set_body( xec_statement_compound* body );

};




#endif





