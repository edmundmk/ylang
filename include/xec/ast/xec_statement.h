//
//  xec_statement.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_STATEMENT_H
#define XEC_STATEMENT_H


struct xec_token;
class xec_declaration;
class xec_expression;
class xec_expression_list;
class xec_statement_catch;



class xec_statement
{
public:

    virtual ~xec_statement();
};




/*
    decl;
*/

class xec_statement_declaration : public xec_statement
{
public:

    explicit xec_statement_declaration( xec_declaration* decl );

};



/*
    expr, expr;
*/

class xec_statement_expression : public xec_statement
{
public:

    explicit xec_statement_expression( xec_expression* expr );

};


/*
    { stmt; stmt; }
*/

class xec_statement_compound : public xec_statement
{
public:

    xec_statement_compound();
    
    void append_statement( xec_statement* stmt );

};


/*
    delete expr;
*/

class xec_statement_delete : public xec_statement
{
public:

    xec_statement_delete( xec_token* token, xec_expression_list* expr_list );

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
};


/*
    case expr:
    default:
*/

class xec_statement_case : public xec_statement
{
public:

    xec_statement_case( xec_token* token, xec_expression* expr );

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
    
};


/*
    do stmt; while ( expr );
*/

class xec_statement_do : public xec_statement
{
public:

    xec_statement_do(
            xec_token* token, xec_expression* expr, xec_statement* body );
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

    xec_statement_foreach(
            xec_token* token,
            xec_expression_list* expr_list,
            xec_expression* expr,
            xec_statement* body );

    void set_eachkey( bool eachkey );
    void set_declare( bool declare );

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

};


/*
    continue;
*/

class xec_statement_continue : public xec_statement
{
public:

    explicit xec_statement_continue( xec_token* token );

};


/*
    break;
*/

class xec_statement_break : public xec_statement
{
public:

    explicit xec_statement_break( xec_token* token );

};


/*
    return;
    return expr, expr;
*/

class xec_statement_return : public xec_statement
{
public:

    xec_statement_return( xec_token* token, xec_expression_list* expr_list );
    
};


/*
    yield;
    yield expr, expr;
*/

class xec_statement_yield : public xec_statement
{
public:

    xec_statement_yield( xec_token* token, xec_expression_list* expr_list );

};


/*
    using expr, expr;
    using condition;
*/

class xec_statement_using : public xec_statement
{
public:

    xec_statement_using( xec_token* token, xec_expression* expr );

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
    
    void set_body( xec_token* token, xec_statement* body );
    void append_catch( xec_statement_catch* cstmt );
    void set_finally( xec_token* token, xec_statement* final );
    
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

    void set_declare( bool declare );

};


/*
    throw expr;
*/

class xec_statement_throw : public xec_statement
{
public:

    explicit xec_statement_throw( xec_expression* expr );
    
};



#endif


