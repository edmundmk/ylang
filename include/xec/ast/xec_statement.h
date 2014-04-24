//
//  xec_statement.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_STATEMENT_H
#define XEC_STATEMENT_H




class xec_statement
{
};




/*
    decl;
*/

class xec_statement_declaration : public xec_statement
{
};



/*
    expr, expr;
*/

class xec_statement_expression : public xec_statement
{
};


/*
    { stmt; stmt; }
*/

class xec_statement_compound : public xec_statement
{
};


/*
    delete expr;
*/

class xec_statement_delete : public xec_statement
{
};


/*
    if ( expr ) stmt; else stmt;
    if ( condition ) stmt; else stmt;
*/

class xec_statement_if : public xec_statement
{
};


/*
    switch ( expr ) { stmt; stmt; }
    switch ( condition ) { stmt; stmt; }
*/

class xec_statement_switch : public xec_statement
{
};


/*
    case expr:
    default:
*/

class xec_statement_case : public xec_statement
{
};


/*
    while ( expr ) stmt;
    while ( condition ) stmt;
*/

class xec_statement_while : public xec_statement
{
};


/*
    do stmt; while ( expr );
*/

class xec_statement_do : public xec_statement
{
};


/*
    for ( expr, expr : expr ) stmt;
    for ( expr, expr :: expr ) stmt;
    for ( condition : expr ) stmt;
    for ( condition :: expr ) stmt;
*/

class xec_statement_foreach : public xec_statement
{
};


/*
    for ( expr; expr; expr ) stmt;
    for ( condition; expr; expr ) stmt;
*/

class xec_statement_for : public xec_statement
{
};


/*
    continue;
*/

class xec_statement_continue : public xec_statement
{
};


/*
    break;
*/

class xec_statement_break : public xec_statement
{
};


/*
    return;
    return expr, expr;
*/

class xec_statement_return : public xec_statement
{
};


/*
    yield;
    yield expr, expr;
*/

class xec_statement_yield : public xec_statement
{
};


/*
    using expr, expr;
    using condition;
*/

class xec_statement_using : public xec_statement
{
};


/*
    using ( expr, expr ) stmt;
    using ( condition ) stmt;
*/

class xec_statement_using_scope : public xec_statement
{
};


/*
    try stmt; catch ( : expr ) stmt; finally stmt;
    try stmt; catch ( expr : expr ) stmt; finally stmt;
    try stmt; catch ( var name : expr ) stmt; finally stmt;
*/

class xec_statement_try : public xec_statement
{
};


/*
    throw expr;
*/

class xec_statement_throw : public xec_statement
{
};



#endif


