//
//  xec_expression.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_EXPRESSION_H
#define XEC_EXPRESSION_H


#include <memory>
#include <deque>


struct xec_token;
class xec_expression_list;
class xec_expression_comparison;
class xec_declaration_function;
class xec_declaration_prototype;



enum xec_expression_dispatch
{
    XEC_EXPRESSION_NULL,
    XEC_EXPRESSION_NUMBER,
    XEC_EXPRESSION_STRING,
    XEC_EXPRESSION_IDENTIFIER,
    XEC_EXPRESSION_LOOKUP,
    XEC_EXPRESSION_INDEXKEY,
    XEC_EXPRESSION_INDEX,
    XEC_EXPRESSION_YIELD,
    XEC_EXPRESSION_CALL,
    XEC_EXPRESSION_UNARY,
    XEC_EXPRESSION_BINARY,
    XEC_EXPRESSION_COMPARISON,
    XEC_EXPRESSION_LOGICAL,
    XEC_EXPRESSION_CONDITIONAL,
    XEC_EXPRESSION_VARARGS,
    XEC_EXPRESSION_UNPACK,
    XEC_EXPRESSION_LIST,
    XEC_EXPRESSION_ASSIGN,
    XEC_EXPRESSION_MONO,
    XEC_EXPRESSION_DECLARE,
    XEC_CONSTRUCTOR_NEW,
    XEC_CONSTRUCTOR_LIST,
    XEC_CONSTRUCTOR_TABLE,
    XEC_CONSTRUCTOR_OBJECT,
    XEC_CONSTRUCTOR_FUNCTION,
};




class xec_expression
{
public:

    virtual ~xec_expression();

    virtual xec_expression_dispatch     visitor_dispatch();
    virtual int                         get_location();
    virtual xec_expression_list*        as_list();
    virtual xec_expression*             as_mono();
    virtual xec_expression_comparison*  as_comparison();
    
};




/*
    null
*/

class xec_expression_null : public xec_expression
{
public:

    xec_expression_null( xec_token* token );

    virtual int get_location();

private:

    xec_token* token;

};


/*
    true
    false
*/

class xec_expression_bool : public xec_expression
{
public:

    xec_expression_bool( xec_token* token );

    virtual int get_location();

private:

    xec_token* token;

};


/*
    1337
*/

class xec_expression_number : public xec_expression
{
public:

    xec_expression_number( xec_token* token );

    virtual int get_location();
    
private:

    xec_token* token;

};


/*
    "hello, world!"
*/

class xec_expression_string : public xec_expression
{
public:

    xec_expression_string( xec_token* token );

    virtual int get_location();

private:

    xec_token* token;

};


/*
    identifier
*/

class xec_expression_identifier : public xec_expression
{
public:

    xec_expression_identifier( xec_token* token );

    virtual int get_location();

private:

    xec_token* token;

};


/*
    expr . lookup
*/

class xec_expression_lookup : public xec_expression
{
public:

    xec_expression_lookup( xec_expression* expr, xec_token* token );

    virtual int get_location();


private:

    std::unique_ptr< xec_expression >   expr;
    xec_token*                          token;

};


/*
    expr.[ expr ]
*/

class xec_expression_indexkey : public xec_expression
{
public:

    xec_expression_indexkey( xec_expression* expr, xec_expression* index );

    virtual int get_location();

private:

    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_expression >   index;

};


/*
    expr[ expr ]
*/

class xec_expression_index : public xec_expression
{
public:

    xec_expression_index( xec_expression* expr, xec_expression* index );
    
    virtual int get_location();

private:

    std::unique_ptr< xec_expression >   expr;
    std::unique_ptr< xec_expression >   index;


};


/*
    yield( expr, expr )
    yield( expr, expr ) ...
*/

class xec_expression_yield : public xec_expression
{
public:

    xec_expression_yield( xec_token* token, xec_expression_list* args );
    
    virtual int get_location();
    virtual xec_expression* as_mono();
    
    void set_unpack( bool unpack );
    
private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  args;
    bool                                    unpack;
    
};



/*
    expr( expr, expr )
    expr( expr, expr ) yield
    expr( expr, expr ) ...
    expr( expr, expr ) yield ...
*/

class xec_expression_call : public xec_expression
{
public:

    xec_expression_call( xec_expression* expr, xec_expression_list* args );

    virtual int get_location();
    virtual xec_expression* as_mono();

    void set_yieldcall( bool yieldcall );
    void set_unpack( bool unpack );

    xec_declaration_prototype*  as_prototype();
    xec_declaration_function*   as_function();

private:

    std::unique_ptr< xec_expression >       expr;
    std::unique_ptr< xec_expression_list >  args;
    bool                                    yieldcall;
    bool                                    unpack;
    

};


/*
    +expr
    -expr
    !expr
    ~expr
    ++expr
    ++expr
    expr++
    expr++
*/

class xec_expression_unary : public xec_expression
{
public:

    xec_expression_unary( xec_expression* expr, xec_token* token );

    virtual int get_location();

private:

    std::unique_ptr< xec_expression >   expr;
    xec_token*                          token;

};


/*
    expr * expr
    expr / expr
    expr % expr
    expr ~ expr
    expr + expr
    expr - expr
    expr << expr
    expr >> expr
    expr >>> expr
    expr & expr
    expr ^ expr
    expr | expr
*/

class xec_expression_binary : public xec_expression
{
public:

    xec_expression_binary(
        xec_expression* expr_a, xec_token* token, xec_expression* expr_b );

    virtual int get_location();

private:

    std::unique_ptr< xec_expression >   expr_a;
    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr_b;

};


/*
    expr == expr != expr < expr > expr <=
        expr >= expr in expr !in expr is expr !is expr
*/

class xec_expression_comparison : public xec_expression
{
public:

    xec_expression_comparison( xec_expression* expr );
    
    virtual int get_location();
    virtual xec_expression_comparison* as_comparison();
    
    void add_comparison( xec_token* token, xec_expression* expr );

private:

    struct comparison
    {
        xec_token*                          token;
        std::unique_ptr< xec_expression >   expr;
    };

    std::unique_ptr< xec_expression >   expr;
    std::deque< comparison >            comparisons;

};


/*
    expr && expr
    expr ^^ expr
    expr || expr
*/

class xec_expression_logical : public xec_expression
{
public:

    xec_expression_logical(
        xec_expression* expr_a, xec_token* token, xec_expression* expr_b );

    virtual int get_location();
    
private:

    std::unique_ptr< xec_expression >   expr_a;
    xec_token*                          token;
    std::unique_ptr< xec_expression >   expr_b;

};


/*
    expr ? expr : expr
*/

class xec_expression_conditional : public xec_expression
{
public:

    xec_expression_conditional( xec_expression* condition,
                    xec_expression* iftrue, xec_expression* iffalse );

    virtual int get_location();

private:

    std::unique_ptr< xec_expression >   condition;
    std::unique_ptr< xec_expression >   iftrue;
    std::unique_ptr< xec_expression >   iffalse;

};


/*
    ...
*/

class xec_expression_varargs : public xec_expression
{
public:

    explicit xec_expression_varargs( xec_token* token );

    virtual int get_location();
    virtual xec_expression* as_mono();
    
private:

    xec_token* token;
    
};


/*
    expr[] ...
*/

class xec_expression_unpack : public xec_expression
{
public:

    explicit xec_expression_unpack( xec_expression* expr );

    virtual int get_location();
    virtual xec_expression* as_mono();

private:

    std::unique_ptr< xec_expression > expr;

};


/*
    expr, expr, expr
*/

class xec_expression_list : public xec_expression
{
public:

    xec_expression_list();
    
    virtual int get_location();
    virtual xec_expression* as_mono();
    virtual xec_expression_list* as_list();
    
    void append_expression( xec_expression* expression );
    void append_final( xec_expression* final );

private:

    std::deque< std::unique_ptr< xec_expression > > expressions;
    std::unique_ptr< xec_expression >               final;

};


/*
    expr = expr
    expr *= expr
    expr /= expr
    expr %= expr
    expr ~= expr
    expr += expr
    expr -= expr
    expr <<= expr
    expr >>= expr
    expr >>>= expr
    expr &= expr
    expr |= expr
    expr ^= expr
*/

class xec_expression_assign : public xec_expression
{
public:

    xec_expression_assign(
            xec_expression* lvalue, xec_token* op, xec_expression* rvalue );

    virtual int get_location();
    virtual xec_expression* as_mono();

private:

    std::unique_ptr< xec_expression >   lvalue;
    xec_token*                          op;
    std::unique_ptr< xec_expression >   rvalue;

};



/*
    ( expr, ... )
*/

class xec_expression_mono : public xec_expression
{
public:

    explicit xec_expression_mono( xec_expression* expr );

    virtual int get_location();

private:

    std::unique_ptr< xec_expression > expr;

};



/*
    var name, name = expr, expr;
*/

class xec_expression_declare : public xec_expression
{
public:

    xec_expression_declare( xec_token* token,
            xec_expression_list* name_list, xec_expression_list* expr_list );
    
    virtual int get_location();
    virtual xec_expression* as_mono();

private:

    xec_token*                              token;
    std::unique_ptr< xec_expression_list >  name_list;
    std::unique_ptr< xec_expression_list >  expr_list;

};





#endif
