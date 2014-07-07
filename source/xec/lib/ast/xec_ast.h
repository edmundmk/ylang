//
//  xec_ast.h
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_AST_H
#define XEC_AST_H


#include <deque>
#include <unordered_map>
#include <region.h>
#include "symbol.h"
#include "xec_token.h"


struct xec_scope;
struct xec_name;
struct xec_prototype;
struct xec_astnode;
struct xec_expr_list;
struct xec_keyvalue;



enum xec_scope_kind
{
    XEC_SCOPE_GLOBAL,
    XEC_SCOPE_FUNCTION,
    XEC_SCOPE_OBJECT,
    XEC_SCOPE_IMPLIED,
};


enum xec_astnode_kind
{
    // Functions.
    XEC_AST_FUNCTION,

    // Literals.
    XEC_EXPR_NULL,
    XEC_EXPR_BOOL,
    XEC_EXPR_NUMBER,
    XEC_EXPR_STRING,
    
    // Names.
    XEC_EXPR_LOCAL,
    XEC_EXPR_GLOBAL,
    XEC_EXPR_UPVAL,
    
    // Lookup.
    XEC_EXPR_KEY,
    XEC_EXPR_INKEY,
    XEC_EXPR_INDEX,

    // Operators.
    XEC_EXPR_PREOP,     // not valid on expression lists
    XEC_EXPR_POSTOP,    // not valid on expression lists
    XEC_EXPR_UNARY,
    XEC_EXPR_BINARY,
    XEC_EXPR_COMPARE,
    XEC_EXPR_LOGICAL,
    XEC_EXPR_QMARK,
    
    // New.
    XEC_NEW_NEW,
    XEC_NEW_OBJECT,
    XEC_NEW_LIST,
    XEC_NEW_TABLE,
    XEC_NEW_CLOSURE,
    
    // Unpack.
    XEC_EXPR_MONO,
    XEC_EXPR_CALL,
    XEC_EXPR_CALLKEY,
    XEC_EXPR_CALLINKEY,
    XEC_EXPR_YIELD,
    XEC_EXPR_VARARG,
    XEC_EXPR_UNPACK,
    XEC_EXPR_LIST,
    
    // Declaration.
    XEC_DECLARE,
    XEC_DECLARE_LIST,

    // Assignment.
    XEC_ASSIGN,
    XEC_ASSIGN_LIST,

    // Statements with scopes.
    XEC_STMT_COMPOUND,
    XEC_STMT_IF,
    XEC_STMT_WHILE,
    XEC_STMT_DO,
    XEC_STMT_FOREACH,
    XEC_STMT_FOR,
    XEC_STMT_SWITCH,
    XEC_STMT_TRY,
    XEC_STMT_CATCH,
    
    // Other statements.
    XEC_STMT_USING,
    XEC_STMT_DELETE,
    XEC_STMT_CASE,
    XEC_STMT_CONTINUE,
    XEC_STMT_BREAK,
    XEC_STMT_RETURN,
    XEC_STMT_THROW,

};



typedef std::deque< xec_astnode*,
    region_allocator< xec_astnode* > > xec_astnode_list;
typedef std::deque< xec_name*,
    region_allocator< xec_name* > > xec_name_list;
typedef std::deque< xec_token_kind,
    region_allocator< xec_token_kind > > xec_opkind_list;
typedef std::deque< xec_keyvalue,
    region_allocator< xec_keyvalue > > xec_keyvalue_list;
typedef std::unordered_map< symbol, xec_name*,
    region_allocator< std::pair< symbol, xec_name* > > > xec_name_map;



/*
    Scopes contain names and prototypes.
*/


struct xec_scope
{
    xec_scope_kind      kind;
    xec_astnode*        node;
    xec_scope*          outer;
    xec_name_map        names;
    xec_astnode_list    decls;
};


struct xec_name
{
    xec_scope*          scope;
    const char*         name;
    xec_scope*          implied;
    xec_prototype*      prototype;
};


struct xec_prototype
{
    xec_astnode_list    parameters;
    bool                yieldcall;
};



/*
    AST nodes.
*/


struct xec_astnode
{
    xec_astnode_kind    kind;
    int                 sloc;
};


/*
    Function.
*/

struct xec_ast_function : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode_list    parameters;
    xec_astnode*        stmt;
    bool                yieldcall;
};


/*
    Literals.
*/

struct xec_expr_null : public xec_astnode
{
};


struct xec_expr_bool : public xec_astnode
{
    bool                value;
};


struct xec_expr_number : public xec_astnode
{
    double              value;
};


struct xec_expr_string : public xec_astnode
{
    const char*         string;
    size_t              length;
};


/*
    Names.
*/

struct xec_expr_local : public xec_astnode
{
    xec_name*           name;
};


struct xec_expr_global : public xec_astnode
{
    const char*         name;
};


struct xec_expr_upval : public xec_astnode
{
    int                 index;
};


/*
    Lookup.
*/

struct xec_expr_key : public xec_astnode
{
    xec_astnode*        object;
    const char*         key;
};


struct xec_expr_inkey : public xec_astnode
{
    xec_astnode*        object;
    xec_astnode*        key;
};


struct xec_expr_index : public xec_astnode
{
    xec_astnode*        object;
    xec_astnode*        index;
};


/*
    Operators.
*/

struct xec_expr_preop : public xec_astnode
{
    xec_token_kind      opkind;
    xec_astnode*        lvalue;
};


struct xec_expr_postop : public xec_astnode
{
    xec_token_kind      opkind;
    xec_astnode*        lvalue;
};


struct xec_expr_unary : public xec_astnode
{
    xec_token_kind      opkind;
    xec_astnode*        operand;
};


struct xec_expr_binary : public xec_astnode
{
    xec_token_kind      opkind;
    xec_astnode*        lhs;
    xec_astnode*        rhs;
};


struct xec_expr_compare : public xec_astnode
{
    xec_astnode*        first;
    xec_opkind_list     opkinds;
    xec_astnode_list    terms;
};


struct xec_expr_logical : public xec_astnode
{
    xec_token_kind      opkind;
    xec_astnode*        lhs;
    xec_astnode*        rhs;
};


struct xec_expr_qmark : public xec_astnode
{
    xec_astnode*        condition;
    xec_astnode*        iftrue;
    xec_astnode*        iffalse;
};


struct xec_new_new : public xec_astnode
{
    xec_astnode*        proto;
    xec_expr_list*      arguments;
};


/*
    New.
*/

struct xec_new_object : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        proto;
    xec_astnode_list    members;
};


struct xec_new_list : public xec_astnode
{
    xec_expr_list*      elements;
};


struct xec_keyvalue
{
    xec_astnode*        key;
    xec_astnode*        value;
};


struct xec_new_table : public xec_astnode
{
    xec_keyvalue_list   elements;
};


struct xec_new_closure : public xec_astnode
{
    xec_ast_function*   function;
    xec_astnode_list    upvals;
};


/*
    Unpack.
*/

struct xec_expr_mono : public xec_astnode
{
    xec_astnode*        expr;
};


struct xec_expr_call : public xec_astnode
{
    xec_astnode*        function;
    xec_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};


struct xec_expr_callkey : public xec_astnode
{
    xec_astnode*        object;
    const char*         key;
    xec_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};


struct xec_expr_callinkey : public xec_astnode
{
    xec_astnode*        object;
    xec_astnode*        index;
    xec_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};


struct xec_expr_yield : public xec_astnode
{
    xec_expr_list*      arguments;
    bool                unpack;
};


struct xec_expr_vararg : public xec_astnode
{
};


struct xec_expr_unpack : public xec_astnode
{
    xec_astnode*        list;
};


struct xec_expr_list : public xec_astnode
{
    xec_astnode_list    values;
    xec_astnode*        unpack;
};


/*
    Declaration.
*/

struct xec_declare : public xec_astnode
{
    xec_name*           name;
    xec_astnode*        value;
};


struct xec_declare_list : public xec_astnode
{
    xec_name_list       names;
    xec_expr_list*      values;
};


/*
    Assignment.
*/

struct xec_assign : public xec_astnode
{
    xec_astnode*        lvalue;
    xec_astnode*        value;
};


struct xec_assign_list : public xec_astnode
{
    xec_astnode_list    lvalues;
    xec_expr_list*      values;
};


/*
    Statements w/ scopes.
*/

struct xec_stmt_compound : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode_list    stmts;
};


struct xec_stmt_if : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        condition;
    xec_astnode*        iftrue;
    xec_astnode*        iffalse;
};


struct xec_stmt_while : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        condition;
    xec_astnode*        body;
};


struct xec_stmt_do : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        body;
    xec_astnode*        condition;
};


struct xec_stmt_foreach : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        lvalues;
    xec_astnode*        list;
    xec_astnode*        body;
};


struct xec_stmt_for : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        init;
    xec_astnode*        condition;
    xec_astnode*        update;
    xec_astnode*        body;
};


struct xec_stmt_switch : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        value;
    xec_astnode*        body;
};


struct xec_stmt_try : public xec_astnode
{
    xec_astnode*        trybody;
    xec_astnode_list    catchlist;
    xec_astnode*        finallybody;
};


struct xec_stmt_catch : public xec_astnode
{
    xec_scope*          scope;
    xec_astnode*        lvalue;
    xec_astnode*        proto;
};


/*
    Statements.
*/

struct xec_stmt_using : public xec_astnode
{
    xec_astnode_list    values;
};


struct xec_stmt_delete : public xec_astnode
{
    xec_astnode_list    lvalues;
};


struct xec_stmt_case : public xec_astnode
{
    xec_astnode*        value;
};


struct xec_stmt_continue : public xec_astnode
{
    xec_astnode*        target;
};


struct xec_stmt_break : public xec_astnode
{
    xec_astnode*        target;
};


struct xec_stmt_return : public xec_astnode
{
    xec_expr_list*      values;
};


struct xec_stmt_throw : public xec_astnode
{
    xec_astnode*        value;
};




#endif
