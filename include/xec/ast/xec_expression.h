//
//  xec_expression.h
//
//  Created by Edmund Kapusniak on 21/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_EXPRESSION_H
#define XEC_EXPRESSION_H




class xec_expression
{
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
    expr . lookup
*/

class xec_expression_lookup : public xec_expression
{
};


/*
    expr[ expr ]
    expr.[ expr ]
*/

class xec_expression_index : public xec_expression
{
};



/*
    yield( expr, expr )
    yield( expr, expr ) ...
*/

class xec_expression_yield : public xec_expression
{
};



/*
    expr( expr, expr )
    expr( expr, expr ) yield
    expr( expr, expr ) ...
    expr( expr, expr ) yield ...
*/

class xec_expression_call : public xec_expression
{
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
    var name, name = expr, expr;
*/

class xec_condition : public xec_expression
{
};





#endif
