//
//  xec_statement.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_STATEMENT_H
#define XEC_STATEMENT_H


#include <memory>
#include <deque>


struct xec_token;
class xec_declaration;
class xec_expression;
class xec_expression_list;
class xec_statement_catch;



class xec_statement
{
public:

    virtual ~xec_statement();
    
    virtual int get_location();
    
};




/*
    decl;
*/

class xec_statement_declaration : public xec_statement
{
public:

    explicit xec_statement_declaration( xec_declaration* decl );

    virtual int get_location();
    
private:

    std::unique_ptr< xec_declaration > decl;

};



/*
    expr, expr;
*/

class xec_statement_expression : public xec_statement
{
public:

    explicit xec_statement_expression( xec_expression* expr );

    virtual int get_location();
    
private:

    std::unique_ptr< xec_expression > expr;

};


/*
    { stmt; stmt; }
*/

class xec_statement_compound : public xec_statement
{
public:

    xec_statement_compound();
    
    virtual int get_location();
    
    void set_token( xec_token* token );
    void append_statement( xec_statement* stmt );

private:

    xec_token*                                      token;
    std::deque< std::unique_ptr< xec_statement > >  statements;

};


/*
    delete expr;
*/

class xec_statement_delete : public xec_statement
{
public:

    xec_statement_delete( xec_token* token, xec_expression_list* expr_list );
    
    virtual int get_location();
    
private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  expr_list;

};


/*
    if ( expr ) stmt; else stmt;
    if ( condition ) stmt; else stmt;
*/

class xec_statement_if : public xec_statement
{
public:

    xec_statement_if( xec_token* token, xec_expression* expr,
                    xec_statement* iftrue, xec_statement* iffalse );

    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_statement >    iftrue;
    std::unique_ptr< xec_statement >    iffalse;

};


/*
    switch ( expr ) { stmt; stmt; }
    switch ( condition ) { stmt; stmt; }
*/

class xec_statement_switch : public xec_statement
{
public:

    xec_statement_switch( xec_token* token,
                    xec_expression* expr, xec_statement_compound* body );

    virtual int get_location();
    
private:

    xec_token*                                  token;
    std::unique_ptr< xec_expression >           expr;
    std::unique_ptr< xec_statement_compound >   body;

};


/*
    case expr:
    default:
*/

class xec_statement_case : public xec_statement
{
public:

    xec_statement_case( xec_token* token, xec_expression* expr );

    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;

};


/*
    while ( expr ) stmt;
    while ( condition ) stmt;
*/

class xec_statement_while : public xec_statement
{
public:

    xec_statement_while(
            xec_token* token, xec_expression* expr, xec_statement* body );
    
    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_statement >    body;
    
};


/*
    do stmt; while ( expr );
*/

class xec_statement_do : public xec_statement
{
public:

    xec_statement_do(
            xec_token* token, xec_statement* body, xec_expression* expr );

    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_statement >    body;
    std::unique_ptr< xec_expression >   expr;

};


/*
    for ( expr, expr : expr ) stmt;
    for ( expr, expr :: expr ) stmt;
    for ( condition : expr ) stmt;
    for ( condition :: expr ) stmt;
*/

class xec_statement_foreach : public xec_statement
{
public:

    xec_statement_foreach( xec_token* token, xec_expression_list* expr_list,
            xec_expression* expr, xec_statement* body );

    virtual int get_location();

    void set_eachkey( bool eachkey );
    void set_declare( bool declare );

private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  expr_list;
    std::unique_ptr< xec_expression >       expr;
    std::unique_ptr< xec_statement >        body;
    bool                                    eachkey;
    bool                                    declare;

};


/*
    for ( expr; expr; expr ) stmt;
    for ( condition; expr; expr ) stmt;
*/

class xec_statement_for : public xec_statement
{
public:

    xec_statement_for( xec_token* token, xec_expression* init,
        xec_expression* expr, xec_expression* update, xec_statement* body );

    virtual int get_location();

private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   init;
    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_expression >   update;
    std::unique_ptr< xec_statement >    body;

};


/*
    continue;
*/

class xec_statement_continue : public xec_statement
{
public:

    explicit xec_statement_continue( xec_token* token );

    virtual int get_location();
    
private:

    xec_token* token;

};


/*
    break;
*/

class xec_statement_break : public xec_statement
{
public:

    explicit xec_statement_break( xec_token* token );

    virtual int get_location();
    
private:

    xec_token* token;

};


/*
    return;
    return expr, expr;
*/

class xec_statement_return : public xec_statement
{
public:

    xec_statement_return( xec_token* token, xec_expression_list* expr_list );

    virtual int get_location();
    
private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  expr_list;
    
};


/*
    yield;
    yield expr, expr;
*/

class xec_statement_yield : public xec_statement
{
public:

    xec_statement_yield( xec_token* token, xec_expression_list* expr_list );

    virtual int get_location();
    
private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  expr_list;
    
};


/*
    using expr, expr;
    using condition;
*/

class xec_statement_using : public xec_statement
{
public:

    xec_statement_using( xec_token* token, xec_expression* expr );

    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;

};


/*
    using ( expr, expr ) stmt;
    using ( condition ) stmt;
*/

class xec_statement_using_scope : public xec_statement
{
public:

    xec_statement_using_scope(
            xec_token* token, xec_expression* expr, xec_statement* body );
    
    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_statement >    body;

};


/*
    try stmt; catch ( : expr ) stmt; finally stmt;
    try stmt; catch ( expr : expr ) stmt; finally stmt;
    try stmt; catch ( var name : expr ) stmt; finally stmt;
*/

class xec_statement_try : public xec_statement
{
public:

    xec_statement_try();

    virtual int get_location();
    
    void set_body( xec_token* token, xec_statement* body );
    void append_catch( xec_statement_catch* cstmt );
    void set_finally( xec_token* token, xec_statement* final );
    
private:

    xec_token*                                              token;
    std::unique_ptr< xec_statement >                        body;
    std::deque< std::unique_ptr< xec_statement_catch > >    catches;
    xec_token*                                              ftoken;
    std::unique_ptr< xec_statement >                        final;
    
};


/*
    catch ( : expr ) stmt;
    catch ( name : expr ) stmt;
    catch ( var name : expr ) stmt;
*/

class xec_statement_catch : public xec_statement
{
public:

    xec_statement_catch( xec_token* token,
        xec_expression* lvalue, xec_expression* proto, xec_statement* body );

    virtual int get_location();

    void set_declare( bool declare );

private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   lvalue;
    std::unique_ptr< xec_expression >   proto;
    std::unique_ptr< xec_statement >    body;
    bool                                declare;

};


/*
    throw expr;
*/

class xec_statement_throw : public xec_statement
{
public:

    explicit xec_statement_throw( xec_token* token, xec_expression* expr );
    
    virtual int get_location();
    
private:

    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr;
    
};



#endif


