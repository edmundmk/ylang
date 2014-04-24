//
//  xec_expression.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_EXPRESSION_H
#define XEC_EXPRESSION_H



class xec_expression_list;
class xec_declaration_function;
class xec_declaration_prototype;



enum xec_operator
{
    XEC_UNARY_PLUS,
    XEC_UNARY_MINUS,
    XEC_LOGICAL_NOT,
    XEC_BITWISE_NOT,
    XEC_PRE_INCREMENT,
    XEC_PRE_DECREMENT,
    XEC_POST_INCREMENT,
    XEC_POST_DECREMENT,
    XEC_MULTIPLY,
    XEC_DIVIDE,
    XEC_MODULUS,
    XEC_INTEGER_DIVIDE,
    XEC_ADD,
    XEC_SUBTRACT,
    XEC_LEFT_SHIFT,
    XEC_SIGNED_RIGHT_SHIFT,
    XEC_UNSIGNED_RIGHT_SHIFT,
    XEC_BITWISE_AND,
    XEC_BITWISE_XOR,
    XEC_BITWISE_OR,
    XEC_LOGICAL_AND,
    XEC_LOGICAL_XOR,
    XEC_LOGICAL_OR,
    XEC_EQUAL,
    XEC_NOT_EQUAL,
    XEC_LESS,
    XEC_GREATER,
    XEC_LESS_EQUAL,
    XEC_GREATER_EQUAL,
    XEC_IN,
    XEC_NOT_IN,
    XEC_IS,
    XEC_NOT_IS,
};



class xec_expression
{
public:

    virtual ~xec_expression();
    
    virtual xec_expression_list*    as_list();
    virtual xec_expression*         in_parentheses();
    
};




/*
    null
*/

class xec_expression_null : public xec_expression
{
};


/*
    true
    false
*/

class xec_expression_bool : public xec_expression
{
};


/*
    1337
*/

class xec_expression_number : public xec_expression
{
};


/*
    "hello, world!"
*/

class xec_expression_string : public xec_expression
{
};


/*
    identifier
*/

class xec_expression_identifier : public xec_expression
{
public:

    xec_expression_identifier( xec_token* token );

};


/*
    expr . lookup
*/

class xec_expression_lookup : public xec_expression
{
public:

    xec_expression_lookup( xec_expression* expr, xec_token* token );
    
};


/*
    expr.[ expr ]
*/

class xec_expression_lookup_value : public xec_expression
{
public:

    xec_expression_lookup_value( xec_expression* expr, xec_expression* value );

};


/*
    expr[ expr ]
*/

class xec_expression_index : public xec_expression
{
public:

    xec_expression_index( xec_expression* expr, xec_expression* index );
    

};


/*
    yield( expr, expr )
    yield( expr, expr ) ...
*/

class xec_expression_yield : public xec_expression
{
public:

    xec_expression_yield( xec_expression_list* args );
    
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

    bool set_yield_call( bool yield_call );

    xec_declaration_prototype*  as_prototype();
    xec_declaration_function*   as_function();

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
};


/*
    expr == expr != expr < expr > expr <=
        expr >= expr in expr !in expr is expr !is expr
*/

class xec_expression_comparison : public xec_expression
{
};


/*
    expr && expr
    expr ^^ expr
    expr || expr
*/

class xec_expression_logical : public xec_expression
{
};


/*
    expr ? expr : expr
*/

class xec_expression_conditional : public xec_expression
{
};


/*
    ...
*/

class xec_expression_varargs : public xec_expression
{
};


/*
    expr[] ...
*/

class xec_expression_unpack : public xec_expression
{
};


/*
    expr, expr, expr
*/

class xec_expression_list : public xec_expression
{
public:

    xec_expression_list();
    void append_expression( xec_expression* expression );
    void append_final( xec_expression* expression );

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
};



/*
    ( expr ... )
*/

class xec_expression_monovalue : public xec_expression
{
};



/*
    var name, name = expr, expr;
*/

class xec_expression_condition : public xec_expression
{
};





#endif
