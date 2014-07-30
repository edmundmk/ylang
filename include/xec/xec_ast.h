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


struct xec_ast_scope;
struct xec_ast_name;
struct xec_ast_prototype;
struct xec_ast_upval;
struct xec_ast_node;
struct xec_ast_func;
struct xec_new_object;
struct xec_expr_list;
struct xec_stmt_block;
struct xec_unqual_name;
struct xec_key_value;



enum xec_ast_scope_kind
{
    XEC_SCOPE_SCRIPT,
    XEC_SCOPE_BLOCK,
    XEC_SCOPE_OBJECT,
    XEC_SCOPE_IMPLIED,
};


enum xec_ast_node_kind
{
    // Functions.
    XEC_AST_FUNC,

    // Literals.
    XEC_EXPR_NULL,
    XEC_EXPR_BOOL,
    XEC_EXPR_NUMBER,
    XEC_EXPR_STRING,
    
    // Names.
    XEC_EXPR_LOCAL,
    XEC_EXPR_GLOBAL,
    XEC_EXPR_UPREF,
    XEC_EXPR_OBJREF,
    
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
    
    // Unpack.
    XEC_EXPR_MONO,
    XEC_EXPR_CALL,
    XEC_EXPR_YIELD,
    XEC_EXPR_VARARG,
    XEC_EXPR_UNPACK,
    XEC_EXPR_LIST,
    
    // Assignment.
    XEC_EXPR_ASSIGN,
    XEC_EXPR_ASSIGN_LIST,

    // Statements with scopes.
    XEC_STMT_BLOCK,
    XEC_STMT_IF,
    XEC_STMT_SWITCH,
    XEC_STMT_WHILE,
    XEC_STMT_DO,
    XEC_STMT_FOREACH,
    XEC_STMT_FOR,
    XEC_STMT_USING,
    XEC_STMT_TRY,
    XEC_STMT_CATCH,
    
    // Other statements.
    XEC_STMT_DELETE,
    XEC_STMT_CASE,
    XEC_STMT_CONTINUE,
    XEC_STMT_BREAK,
    XEC_STMT_RETURN,
    XEC_STMT_THROW,
    
    // Unresolved and unqualified (should not appear in final AST).
    XEC_UNQUAL_NAME,
    XEC_UNQUAL_QUAL,
    XEC_UNQUAL_LIST,
    XEC_UNQUAL_PROTO,

};


enum xec_operator_kind
{
    XEC_OPERATOR_DECLARE,

    XEC_OPERATOR_ASSIGN,
    XEC_OPERATOR_MULASSIGN,
    XEC_OPERATOR_DIVASSIGN,
    XEC_OPERATOR_MODASSIGN,
    XEC_OPERATOR_INTDIVASSIGN,
    XEC_OPERATOR_ADDASSIGN,
    XEC_OPERATOR_SUBASSIGN,
    XEC_OPERATOR_LSHIFTASSIGN,
    XEC_OPERATOR_LRSHIFTASSIGN,
    XEC_OPERATOR_ARSHIFTASSIGN,
    XEC_OPERATOR_BITANDASSIGN,
    XEC_OPERATOR_BITXORASSIGN,
    XEC_OPERATOR_BITORASSIGN,
    
    XEC_OPERATOR_PREINC,
    XEC_OPERATOR_PREDEC,
    
    XEC_OPERATOR_POSTINC,
    XEC_OPERATOR_POSTDEC,

    XEC_OPERATOR_POSITIVE,
    XEC_OPERATOR_NEGATIVE,
    XEC_OPERATOR_LOGICNOT,
    XEC_OPERATOR_BITNOT,
    
    XEC_OPERATOR_MULTIPLY,
    XEC_OPERATOR_DIVIDE,
    XEC_OPERATOR_MODULUS,
    XEC_OPERATOR_INTDIV,
    XEC_OPERATOR_ADD,
    XEC_OPERATOR_SUBTRACT,
    XEC_OPERATOR_LSHIFT,
    XEC_OPERATOR_LRSHIFT,
    XEC_OPERATOR_ARSHIFT,
    XEC_OPERATOR_BITAND,
    XEC_OPERATOR_BITXOR,
    XEC_OPERATOR_BITOR,
    XEC_OPERATOR_CONCATENATE,
    
    XEC_OPERATOR_EQUAL,
    XEC_OPERATOR_NOTEQUAL,
    XEC_OPERATOR_LESS,
    XEC_OPERATOR_GREATER,
    XEC_OPERATOR_LESSEQUAL,
    XEC_OPERATOR_GREATEREQUAL,
    XEC_OPERATOR_IN,
    XEC_OPERATOR_NOTIN,
    XEC_OPERATOR_IS,
    XEC_OPERATOR_NOTIS,
    
    XEC_OPERATOR_LOGICAND,
    XEC_OPERATOR_LOGICXOR,
    XEC_OPERATOR_LOGICOR,
};


enum xec_ast_upval_kind
{
    XEC_UPVAL_LOCAL,
    XEC_UPVAL_OBJECT,
    XEC_UPVAL_UPVAL,
};



typedef std::deque< xec_ast_node*,
    region_allocator< xec_ast_node* > > xec_ast_node_list;
typedef std::deque< xec_ast_name*,
    region_allocator< xec_ast_name* > > xec_ast_name_list;
typedef std::deque< xec_unqual_name*,
    region_allocator< xec_unqual_name* > > xec_unqual_name_list;
typedef std::deque< xec_operator_kind,
    region_allocator< xec_operator_kind > > xec_opkind_list;
typedef std::deque< xec_key_value,
    region_allocator< xec_key_value > > xec_key_value_list;
typedef std::unordered_map
        <
            symkey,
            xec_ast_name*,
            std::hash< symkey >,
            std::equal_to< symkey >,
            region_allocator< std::pair< const symkey, xec_ast_name* > >
        >
        xec_ast_name_map;
typedef std::unordered_map
        <
            symkey,
            xec_ast_scope*,
            std::hash< symkey >,
            std::equal_to< symkey >,
            region_allocator< std::pair< const symkey, xec_ast_scope* > >
        >
        xec_ast_imply_map;
typedef std::deque< xec_ast_upval,
    region_allocator< xec_ast_upval > > xec_ast_upval_list;



/*
    Scopes contain names and prototypes.
*/


struct xec_ast_scope
{
    xec_ast_scope( xec_ast_scope_kind kind,
            xec_ast_scope* outer, xec_ast_node* node, xec_ast_func* func );

    xec_ast_scope_kind  kind;
    xec_ast_scope*      outer;
    xec_ast_node*       node;
    xec_ast_func*       func;
    xec_stmt_block*     block;
    xec_ast_imply_map   implied;
    xec_ast_name_map    names;
    xec_ast_name_list   decls;
};


struct xec_ast_name
{
    xec_ast_name( int sloc, xec_ast_scope* scope, const char* name );

    int                 sloc;
    xec_ast_scope*      scope;
    const char*         name;
    xec_ast_name*       superthis;
    xec_ast_prototype*  prototype;
    bool                upval;
};


struct xec_ast_prototype
{
    xec_ast_prototype( int sloc );

    int                 sloc;
    xec_unqual_name_list parameters;
    bool                varargs;
    bool                coroutine;
};



/*
    Upvals are used when accessing variables in outer scopes.
*/

struct xec_ast_upval
{
    xec_ast_upval( xec_ast_name* name );
    xec_ast_upval( xec_new_object* object );
    xec_ast_upval( int upval );

    bool operator == ( const xec_ast_upval& b ) const;
    bool operator != ( const xec_ast_upval& b ) const;

    xec_ast_upval_kind  kind;
    union
    {
        xec_ast_name*   local;
        xec_new_object* object;
        int             upval;
    };
};



/*
    AST nodes.
*/


struct xec_ast_node
{
    xec_ast_node( xec_ast_node_kind kind, int sloc );

    xec_ast_node_kind   kind;
    int                 sloc;
};



/*
    Functions.
*/

struct xec_ast_func : public xec_ast_node
{
    xec_ast_func( int sloc );

    const char*         funcname;
    xec_ast_scope*      scope;
    xec_ast_upval_list  upvals;
    xec_new_object*     memberof;
    xec_ast_name*       thisname;
    xec_ast_name_list   parameters;
    xec_stmt_block*     block;
    bool                varargs;
    bool                coroutine;
};


/*
    Literals.
*/

struct xec_expr_null : public xec_ast_node
{
    explicit xec_expr_null( int sloc );
};


struct xec_expr_bool : public xec_ast_node
{
    xec_expr_bool( int sloc, bool value );

    bool                value;
};


struct xec_expr_number : public xec_ast_node
{
    xec_expr_number( int sloc, double value );

    double              value;
};


struct xec_expr_string : public xec_ast_node
{
    xec_expr_string( int sloc, const char* string, size_t length );

    const char*         string;
    size_t              length;
};


/*
    Names.
*/

struct xec_expr_local : public xec_ast_node
{
    xec_expr_local( int sloc, xec_ast_name* name );

    xec_ast_name*       name;
};


struct xec_expr_global : public xec_ast_node
{
    xec_expr_global( int sloc, const char* name, bool gexplicit );

    const char*         name;
    bool                gexplicit;
};


struct xec_expr_upref : public xec_ast_node
{
    xec_expr_upref( int sloc, int index );

    int                 index;
};


struct xec_expr_objref : public xec_ast_node
{
    xec_expr_objref( int sloc, xec_new_object* object );
    
    xec_new_object*     object;
};


/*
    Lookup.
*/

struct xec_expr_key : public xec_ast_node
{
    xec_expr_key( int sloc, xec_ast_node* object, const char* key );

    xec_ast_node*       object;
    const char*         key;
};


struct xec_expr_inkey : public xec_ast_node
{
    xec_expr_inkey( int sloc, xec_ast_node* object, xec_ast_node* key );

    xec_ast_node*       object;
    xec_ast_node*       key;
};


struct xec_expr_index : public xec_ast_node
{
    xec_expr_index( int sloc, xec_ast_node* object, xec_ast_node* index );

    xec_ast_node*       object;
    xec_ast_node*       index;
};


/*
    Operators.
*/

struct xec_expr_preop : public xec_ast_node
{
    xec_expr_preop( int sloc, xec_operator_kind opkind, xec_ast_node* lvalue );

    xec_operator_kind   opkind;
    xec_ast_node*       lvalue;
};


struct xec_expr_postop : public xec_ast_node
{
    xec_expr_postop( int sloc, xec_operator_kind opkind, xec_ast_node* lvalue );

    xec_operator_kind   opkind;
    xec_ast_node*       lvalue;
};


struct xec_expr_unary : public xec_ast_node
{
    xec_expr_unary( int sloc, xec_operator_kind opkind, xec_ast_node* operand );

    xec_operator_kind   opkind;
    xec_ast_node*       operand;
};


struct xec_expr_binary : public xec_ast_node
{
    xec_expr_binary( int sloc,
            xec_operator_kind opkind, xec_ast_node* lhs, xec_ast_node* rhs );

    xec_operator_kind   opkind;
    xec_ast_node*       lhs;
    xec_ast_node*       rhs;
};


struct xec_expr_compare : public xec_ast_node
{
    xec_expr_compare( int sloc, xec_ast_node* first );
    
    xec_ast_node*       first;
    xec_opkind_list     opkinds;
    xec_ast_node_list   terms;
};


struct xec_expr_logical : public xec_ast_node
{
    xec_expr_logical( int sloc,
            xec_operator_kind opkind, xec_ast_node* lhs, xec_ast_node* rhs );

    xec_operator_kind   opkind;
    xec_ast_node*       lhs;
    xec_ast_node*       rhs;
};


struct xec_expr_qmark : public xec_ast_node
{
    xec_expr_qmark( int sloc, xec_ast_node* condition,
            xec_ast_node* iftrue, xec_ast_node* iffalse );

    xec_ast_node*       condition;
    xec_ast_node*       iftrue;
    xec_ast_node*       iffalse;
};



/*
    New.
*/

struct xec_new_new : public xec_ast_node
{
    xec_new_new( int sloc, xec_ast_node* proto, xec_expr_list* args );

    xec_ast_node*       proto;
    xec_expr_list*      arguments;
};


struct xec_new_object : public xec_ast_node
{
    xec_new_object( int sloc, xec_ast_node* proto );

    xec_ast_scope*      scope;
    xec_ast_node*       proto;
    xec_ast_node_list   members;
    bool                upval;
};


struct xec_new_list : public xec_ast_node
{
    xec_new_list( int sloc );

    xec_ast_node_list   values;
    xec_ast_node*       unpack;
};


struct xec_key_value
{
    xec_key_value( xec_ast_node* key, xec_ast_node* value );

    xec_ast_node*       key;
    xec_ast_node*       value;
};


struct xec_new_table : public xec_ast_node
{
    xec_new_table( int sloc );

    xec_key_value_list   elements;
};



/*
    Unpack.
*/

struct xec_expr_mono : public xec_ast_node
{
    xec_expr_mono( int sloc, xec_ast_node* expr );

    xec_ast_node*       expr;
};


struct xec_expr_call : public xec_ast_node
{
    xec_expr_call( int sloc, xec_ast_node* function, xec_expr_list* args );

    xec_ast_node*       function;
    xec_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};


struct xec_expr_yield : public xec_ast_node
{
    xec_expr_yield( int sloc, xec_ast_node* args );

    xec_ast_node*       arguments;
    bool                unpack;
};


struct xec_expr_vararg : public xec_ast_node
{
    xec_expr_vararg( int sloc );
};


struct xec_expr_unpack : public xec_ast_node
{
    xec_expr_unpack( int sloc, xec_ast_node* list );

    xec_ast_node*       list;
};


struct xec_expr_list : public xec_ast_node
{
    explicit xec_expr_list( int sloc );

    xec_ast_node_list   values;
    xec_ast_node*       final;
};



/*
    Assignment.
*/

struct xec_expr_assign : public xec_ast_node
{
    xec_expr_assign( int sloc, xec_operator_kind assignop );

    xec_operator_kind   assignop;
    xec_ast_node*       lvalue;
    xec_ast_node*       rvalue;
};


struct xec_expr_assign_list : public xec_ast_node
{
    xec_expr_assign_list( int sloc, xec_operator_kind assignop );

    xec_operator_kind   assignop;
    xec_ast_node_list   lvalues;
    xec_ast_node*       rvalues;
};


/*
    Statements w/ scopes.
*/

struct xec_stmt_block : public xec_ast_node
{
    xec_stmt_block( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node_list   stmts;
};


struct xec_stmt_if : public xec_ast_node
{
    xec_stmt_if( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node*       condition;
    xec_ast_node*       iftrue;
    xec_ast_node*       iffalse;
};


struct xec_stmt_switch : public xec_ast_node
{
    xec_stmt_switch( int sloc );
    
    xec_ast_scope*      scope;
    xec_ast_node*       value;
    xec_stmt_block*     body;
};


struct xec_stmt_while : public xec_ast_node
{
    xec_stmt_while( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node*       condition;
    xec_ast_node*       body;
};


struct xec_stmt_do : public xec_ast_node
{
    xec_stmt_do( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node*       body;
    xec_ast_node*       condition;
};


struct xec_stmt_foreach : public xec_ast_node
{
    xec_stmt_foreach( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node_list   lvalues;
    xec_ast_node*       list;
    xec_ast_node*       body;
    bool                eachkey;
    bool                declare;
};


struct xec_stmt_for : public xec_ast_node
{
    xec_stmt_for( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node*       init;
    xec_ast_node*       condition;
    xec_ast_node*       update;
    xec_ast_node*       body;
};


struct xec_stmt_using : public xec_ast_node
{
    xec_stmt_using( int sloc );
    
    xec_ast_scope*      scope;
    xec_ast_node*       uvalue;
    xec_ast_node*       body;
};

struct xec_stmt_try : public xec_ast_node
{
    xec_stmt_try( int sloc, xec_ast_node* tstmt );

    xec_ast_node*       tstmt;
    xec_ast_node_list   clist;
    xec_ast_node*       fstmt;
};


struct xec_stmt_catch : public xec_ast_node
{
    xec_stmt_catch( int sloc );

    xec_ast_scope*      scope;
    xec_ast_node*       lvalue;
    xec_ast_node*       proto;
    xec_ast_node*       body;
    bool                declare;
};


/*
    Statements.
*/

struct xec_stmt_delete : public xec_ast_node
{
    xec_stmt_delete( int sloc );

    xec_ast_node_list   delvals;
};


struct xec_stmt_case : public xec_ast_node
{
    xec_stmt_case( int sloc, xec_ast_node* value );

    xec_ast_node*       value;
};


struct xec_stmt_continue : public xec_ast_node
{
    xec_stmt_continue( int sloc, xec_ast_node* target );

    xec_ast_node*       target;
};


struct xec_stmt_break : public xec_ast_node
{
    xec_stmt_break( int sloc, xec_ast_node* target );

    xec_ast_node*       target;
};


struct xec_stmt_return : public xec_ast_node
{
    xec_stmt_return( int sloc, xec_ast_node* values );

    xec_ast_node*       values;
};


struct xec_stmt_throw : public xec_ast_node
{
    xec_stmt_throw( int sloc, xec_ast_node* value );

    xec_ast_node*       value;
};



/*
    Unresolved and unqualified.
*/

struct xec_unqual_name : public xec_ast_node
{
    xec_unqual_name( int sloc, const char* name );
    
    const char*         name;
};


struct xec_unqual_qual : public xec_ast_node
{
    xec_unqual_qual( int sloc, xec_ast_node* scope, const char* name );
    
    xec_ast_node*       scope;
    const char*         name;
};


struct xec_unqual_list : public xec_ast_node
{
    xec_unqual_list( int sloc );
    
    xec_unqual_name_list names;
};


struct xec_unqual_proto : public xec_ast_node
{
    xec_unqual_proto( int sloc, xec_ast_node* name, xec_expr_list* params );

    xec_ast_node*       name;
    xec_expr_list*      params;
    bool                coroutine;
};


/*
    Operator names.
*/

const char* xec_operator_name( xec_operator_kind op );



#endif
