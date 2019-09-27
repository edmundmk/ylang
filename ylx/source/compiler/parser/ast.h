//
//  ast.h
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright © 2014-2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef AST_H
#define AST_H

#include <memory>
#include <unordered_map>
#include "../common/alloc_region.h"
#include "../common/region_list.h"
#include "../common/string_hash.h"
#include "diagnostics.h"

namespace yl
{

struct ast_script;

struct ast_scope;
struct ast_name;
struct ast_prototype;
struct ast_upval;
struct ast_key_value;

struct ast_node;
struct ast_function;

struct ast_expr_null;
struct ast_expr_bool;
struct ast_expr_number;
struct ast_expr_string;

struct ast_expr_local;
struct ast_expr_global;
struct ast_expr_upref;
struct ast_expr_objref;

struct ast_expr_superof;
struct ast_expr_key;
struct ast_expr_inkey;
struct ast_expr_index;
struct ast_expr_responds;
struct ast_expr_inresponds;

struct ast_expr_preop;
struct ast_expr_postop;
struct ast_expr_unary;
struct ast_expr_binary;
struct ast_expr_compare;
struct ast_expr_logical;
struct ast_expr_qmark;

struct ast_make_new;
struct ast_make_object;
struct ast_make_array;
struct ast_make_table;

struct ast_expr_mono;
struct ast_expr_call;
struct ast_expr_yield;
struct ast_expr_vararg;
struct ast_expr_unpack;
struct ast_expr_list;

struct ast_expr_assign;
struct ast_expr_assign_list;

struct ast_stmt_block;
struct ast_stmt_if;
struct ast_stmt_switch;
struct ast_stmt_while;
struct ast_stmt_do;
struct ast_stmt_foreach;
struct ast_stmt_for;
struct ast_stmt_using;
struct ast_stmt_try;
struct ast_stmt_catch;

struct ast_stmt_delete;
struct ast_stmt_case;
struct ast_stmt_continue;
struct ast_stmt_break;
struct ast_stmt_return;
struct ast_stmt_throw;

struct ast_bind_name;
struct ast_bind_list;
struct ast_bind_qual;

enum ast_node_kind
{
    // Functions.
    AST_FUNCTION,

    // Literals.
    AST_EXPR_NULL,
    AST_EXPR_BOOL,
    AST_EXPR_NUMBER,
    AST_EXPR_STRING,

    // Names.
    AST_EXPR_LOCAL,
    AST_EXPR_GLOBAL,
    AST_EXPR_UPREF,
    AST_EXPR_OBJREF,

    // Lookup.
    AST_EXPR_SUPEROF,
    AST_EXPR_KEY,
    AST_EXPR_INKEY,
    AST_EXPR_INDEX,
    AST_EXPR_RESPONDS,
    AST_EXPR_INRESPONDS,

    // Operators.
    AST_EXPR_PREOP,     // not valid on expression lists
    AST_EXPR_POSTOP,    // not valid on expression lists
    AST_EXPR_UNARY,
    AST_EXPR_BINARY,
    AST_EXPR_COMPARE,
    AST_EXPR_LOGICAL,
    AST_EXPR_QMARK,

    // Object construction.
    AST_MAKE_NEW,
    AST_MAKE_OBJECT,
    AST_MAKE_ARRAY,
    AST_MAKE_TABLE,

    // Unpack.
    AST_EXPR_MONO,
    AST_EXPR_CALL,
    AST_EXPR_YIELD,
    AST_EXPR_VARARG,
    AST_EXPR_UNPACK,
    AST_EXPR_LIST,

    // Assignment.
    AST_EXPR_ASSIGN,
    AST_EXPR_ASSIGN_LIST,

    // Statements with scopes.
    AST_STMT_BLOCK,
    AST_STMT_IF,
    AST_STMT_SWITCH,
    AST_STMT_WHILE,
    AST_STMT_DO,
    AST_STMT_FOREACH,
    AST_STMT_FOR,
    AST_STMT_USING,
    AST_STMT_TRY,
    AST_STMT_CATCH,

    // Other statements.
    AST_STMT_DELETE,
    AST_STMT_CASE,
    AST_STMT_CONTINUE,
    AST_STMT_BREAK,
    AST_STMT_RETURN,
    AST_STMT_THROW,

    // Unresolved and unqualified (should not appear in final AST).
    AST_BIND_NAME,
    AST_BIND_LIST,
    AST_BIND_QUAL,
};

enum ast_op_kind
{
    AST_OP_DECLARE,

    AST_OP_ASSIGN,
    AST_OP_MULASSIGN,
    AST_OP_DIVASSIGN,
    AST_OP_MODASSIGN,
    AST_OP_INTDIVASSIGN,
    AST_OP_ADDASSIGN,
    AST_OP_SUBASSIGN,
    AST_OP_LSHIFTASSIGN,
    AST_OP_LRSHIFTASSIGN,
    AST_OP_ARSHIFTASSIGN,
    AST_OP_BITANDASSIGN,
    AST_OP_BITXORASSIGN,
    AST_OP_BITORASSIGN,

    AST_OP_PREINC,
    AST_OP_PREDEC,

    AST_OP_POSTINC,
    AST_OP_POSTDEC,

    AST_OP_POSITIVE,
    AST_OP_NEGATIVE,
    AST_OP_LOGICNOT,
    AST_OP_BITNOT,

    AST_OP_MULTIPLY,
    AST_OP_DIVIDE,
    AST_OP_MODULUS,
    AST_OP_INTDIV,
    AST_OP_ADD,
    AST_OP_SUBTRACT,
    AST_OP_LSHIFT,
    AST_OP_LRSHIFT,
    AST_OP_ARSHIFT,
    AST_OP_BITAND,
    AST_OP_BITXOR,
    AST_OP_BITOR,
    AST_OP_CONCATENATE,

    AST_OP_EQUAL,
    AST_OP_NOTEQUAL,
    AST_OP_LESS,
    AST_OP_GREATER,
    AST_OP_LESSEQUAL,
    AST_OP_GREATEREQUAL,
    AST_OP_IS,
    AST_OP_NOTIS,

    AST_OP_LOGICAND,
    AST_OP_LOGICXOR,
    AST_OP_LOGICOR,
};

/*
    A script file parsed into an AST.
*/

struct ast_script
{
    ast_script();
    ~ast_script();

    alloc_region alloc;
    std::vector< ast_function* > functions;
    std::vector< std::unique_ptr< ast_scope > > scopes;
};

/*
    Scopes.  Names are declared in a scope.  Implied scopes form a tree of
    declarations which are used to look up unqualified names.

    Scopes require destruction and are not allocated from the region.
*/

enum ast_scope_kind
{
    AST_SCOPE_SCRIPT,                   // global scope of script
    AST_SCOPE_BLOCK,                    // scope for a code block
    AST_SCOPE_SWITCH,                   // scope for a switch statement
    AST_SCOPE_IMPLICIT,                 // implicit scope for a switch case
    AST_SCOPE_OBJECT,                   // scope in an object declaration
    AST_SCOPE_IMPLIED,                  // scope implied by object/function declarations
};

typedef std::unordered_map< string_hash, ast_scope* > ast_scope_map;
typedef std::unordered_map< string_hash, ast_name* > ast_name_map;

struct ast_scope
{
    ast_scope( ast_scope_kind kind, ast_scope* outer, ast_node* node, ast_function* function );

    ast_scope_kind      kind;           // scope kind
    ast_scope*          outer;          // outer scope
    ast_node*           node;           // function, block, switch, or object declaration
    ast_function*       function;       // function, used to differentiate upvals
    ast_stmt_block*     block;          // block to which statements are appended
    ast_scope_map       implied;        // named implied child scopes
    ast_name_map        names;          // names declared in this scope
    bool                continued;      // set when we reach a 'continue' in a loop
    bool                do_while;       // set when we reach the end of the block in a do while loop
};

/*
    Lists used in the AST.  All trivial lists allocated from the region.
*/

typedef region_list< ast_node* > ast_node_list;
typedef region_list< ast_name* > ast_name_list;
typedef region_list< ast_bind_name* > ast_bind_name_list;
typedef region_list< ast_op_kind > ast_op_kind_list;
typedef region_list< ast_key_value > ast_key_value_list;
typedef region_list< ast_upval > ast_upval_list;

/*
    Names are what is found by lookup.  Allocated from region.
*/

struct ast_name
{
    ast_name( srcloc sloc, ast_scope* scope, const char* name );

    srcloc              sloc;           // source location
    ast_scope*          scope;          // scope containing this name
    const char*         name;           // name
    ast_name*           super_this;     // for 'super', links to 'this'
    ast_prototype*      prototype;      // for names implying prototypes
    bool                upval;          // name is an upval
    bool                continued;      // name was declared after first continue
};

/*
    If a name names a prototype, then information about it is stored here.
*/

struct ast_prototype
{
    ast_prototype( srcloc sloc );

    srcloc              sloc;           // source location
    ast_bind_name_list* parameters;     // parameter declarations
    bool                generator;      // is a generator
};

/*
    Upvals are used when accessing variables in outer scopes.
*/

enum ast_upval_kind
{
    AST_UPVAL_LOCAL,                    // refers to local in outer function
    AST_UPVAL_OBJECT,                   // refers to object being constructed
    AST_UPVAL_UPVAL,                    // refers to an upval chained from outer function
};

struct ast_upval
{
    ast_upval( ast_name* name );
    ast_upval( ast_make_object* object );
    ast_upval( int upval );

    bool operator == ( const ast_upval& b ) const;
    bool operator != ( const ast_upval& b ) const;

    ast_upval_kind      kind;           // upval kind
    union
    {
        ast_name*           local;      // name of local variable
        ast_make_object*    object;     // object being constructed
        int                 upval;      // index of upval
    };

};

/*
    Key/value pair.
*/

struct ast_key_value
{
    ast_key_value( ast_node* key, ast_node* value );

    ast_node*           key;
    ast_node*           value;
};

/*
    AST nodes.  Allocated from the region.
*/

struct ast_node
{
    ast_node( ast_node_kind kind, srcloc sloc );

    ast_node_kind       kind;
    srcloc              sloc;
};

/*
    Functions.
*/

struct ast_function : public ast_node
{
    ast_function( srcloc sloc );

    const char*         name;           // name of function
    ast_scope*          scope;          // scope of function (includes parameters)
    ast_upval_list      upvals;         // list of upvals this function requires
    ast_make_object*    member_of;      // object construction we're a method of
    ast_name*           this_name;      // 'this' parameter
    ast_name_list       parameters;     // explicit parameters
    ast_stmt_block*     block;          // block containing statements
    bool                varargs;        // parameter list ends in ...
    bool                generator;      // is a generator
};

/*
    Literals.
*/

struct ast_expr_null : public ast_node
{
    explicit ast_expr_null( srcloc sloc );
};

struct ast_expr_bool : public ast_node
{
    ast_expr_bool( srcloc sloc, bool value );

    bool                value;
};

struct ast_expr_number : public ast_node
{
    ast_expr_number( srcloc sloc, double value );

    double              value;
};


struct ast_expr_string : public ast_node
{
    ast_expr_string( srcloc sloc, const char* string, size_t length );

    const char*         string;
    size_t              length;
};


/*
    Names.
*/

struct ast_expr_local : public ast_node
{
    ast_expr_local( srcloc sloc, ast_name* name );

    ast_name*           name;
};


struct ast_expr_global : public ast_node
{
    ast_expr_global( srcloc sloc, const char* name, bool gexplicit );

    const char*         name;
    bool                gexplicit;
};


struct ast_expr_upref : public ast_node
{
    ast_expr_upref( srcloc sloc, ast_function* func, int index );

    ast_function*       func;
    int                 index;
};


struct ast_expr_objref : public ast_node
{
    ast_expr_objref( srcloc sloc, ast_make_object* object );

    ast_make_object*    object;
};

/*
    Lookup.
*/

struct ast_expr_superof : public ast_node
{
    ast_expr_superof( srcloc sloc, ast_node* expr );

    ast_node*           expr;
};

struct ast_expr_key : public ast_node
{
    ast_expr_key( srcloc sloc, ast_node* object, const char* key );

    ast_node*           object;
    const char*         key;
};


struct ast_expr_inkey : public ast_node
{
    ast_expr_inkey( srcloc sloc, ast_node* object, ast_node* key );

    ast_node*           object;
    ast_node*           key;
};


struct ast_expr_index : public ast_node
{
    ast_expr_index( srcloc sloc, ast_node* object, ast_node* index );

    ast_node*           object;
    ast_node*           index;
};


struct ast_expr_responds : public ast_node
{
    ast_expr_responds( srcloc sloc, ast_node* object, const char* key );

    ast_node*           object;
    const char*         key;
};


struct ast_expr_inresponds : public ast_node
{
    ast_expr_inresponds( srcloc sloc, ast_node* object, ast_node* key );

    ast_node*           object;
    ast_node*           key;
};

/*
    Operators.
*/

struct ast_expr_preop : public ast_node
{
    ast_expr_preop( srcloc sloc, ast_op_kind opkind, ast_node* lvalue );

    ast_op_kind         opkind;
    ast_node*           lvalue;
};


struct ast_expr_postop : public ast_node
{
    ast_expr_postop( srcloc sloc, ast_op_kind opkind, ast_node* lvalue );

    ast_op_kind         opkind;
    ast_node*           lvalue;
};


struct ast_expr_unary : public ast_node
{
    ast_expr_unary( srcloc sloc, ast_op_kind opkind, ast_node* operand );

    ast_op_kind         opkind;
    ast_node*           operand;
};


struct ast_expr_binary : public ast_node
{
    ast_expr_binary( srcloc sloc, ast_op_kind opkind, ast_node* lhs, ast_node* rhs );

    ast_op_kind         opkind;
    ast_node*           lhs;
    ast_node*           rhs;
};


struct ast_expr_compare : public ast_node
{
    ast_expr_compare( srcloc sloc, ast_node* first );

    ast_node*           first;
    ast_op_kind_list    opkinds;
    ast_node_list       terms;
};


struct ast_expr_logical : public ast_node
{
    ast_expr_logical( srcloc sloc, ast_op_kind opkind, ast_node* lhs, ast_node* rhs );

    ast_op_kind         opkind;
    ast_node*           lhs;
    ast_node*           rhs;
};


struct ast_expr_qmark : public ast_node
{
    ast_expr_qmark( srcloc sloc, ast_node* condition, ast_node* iftrue, ast_node* iffalse );

    ast_node*           condition;
    ast_node*           iftrue;
    ast_node*           iffalse;
};

/*
    New.
*/

struct ast_make_new : public ast_node
{
    ast_make_new( srcloc sloc, ast_node* proto, ast_expr_list* args );

    ast_node*           proto;
    ast_expr_list*      arguments;
};


struct ast_make_object : public ast_node
{
    ast_make_object( srcloc sloc, ast_node* proto );

    ast_scope*          scope;
    ast_node*           proto;
    ast_node_list       members;
    bool                upval;
};

struct ast_make_array : public ast_node
{
    ast_make_array( srcloc sloc );

    ast_node_list       values;
    ast_node*           final;
};

struct ast_make_table : public ast_node
{
    ast_make_table( srcloc sloc );

    ast_key_value_list  elements;
};

/*
    Unpack.
*/

struct ast_expr_mono : public ast_node
{
    ast_expr_mono( srcloc sloc, ast_node* expr );

    ast_node*           expr;
};

struct ast_expr_call : public ast_node
{
    ast_expr_call( srcloc sloc, ast_node* function, ast_expr_list* args, bool yieldcall );

    ast_node*           function;
    ast_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};

struct ast_expr_yield : public ast_node
{
    ast_expr_yield( srcloc sloc, ast_node* args );

    ast_node*           arguments;
    bool                unpack;
};


struct ast_expr_vararg : public ast_node
{
    ast_expr_vararg( srcloc sloc );
};


struct ast_expr_unpack : public ast_node
{
    ast_expr_unpack( srcloc sloc, ast_node* list );

    ast_node*           array;
};


struct ast_expr_list : public ast_node
{
    explicit ast_expr_list( srcloc sloc );

    ast_node_list       values;
    ast_node*           final;
};

/*
    Assignment.
*/

struct ast_expr_assign : public ast_node
{
    ast_expr_assign( srcloc sloc, ast_op_kind assignop );

    ast_op_kind         assignop;
    ast_node*           lvalue;
    ast_node*           rvalue;
};

struct ast_expr_assign_list : public ast_node
{
    ast_expr_assign_list( srcloc sloc, ast_op_kind assignop );

    ast_op_kind         assignop;
    ast_node_list       lvalues;
    ast_node*           rvalues;
};

/*
    Statements w/ scopes.
*/

struct ast_stmt_block : public ast_node
{
    ast_stmt_block( srcloc sloc );

    ast_scope*          scope;
    ast_node_list       stmts;
};

struct ast_stmt_if : public ast_node
{
    ast_stmt_if( srcloc sloc );

    ast_scope*          scope;
    ast_node*           condition;
    ast_node*           iftrue;
    ast_node*           iffalse;
};

struct ast_stmt_switch : public ast_node
{
    ast_stmt_switch( srcloc sloc );

    ast_scope*          scope;
    ast_node*           value;
    ast_stmt_block*      body;
};

struct ast_stmt_while : public ast_node
{
    ast_stmt_while( srcloc sloc );

    ast_scope*          scope;
    ast_node*           condition;
    ast_node*           body;
};

struct ast_stmt_do : public ast_node
{
    ast_stmt_do( srcloc sloc );

    ast_scope*          scope;
    ast_node*           body;
    ast_node*           condition;
};

struct ast_stmt_foreach : public ast_node
{
    ast_stmt_foreach( srcloc sloc );

    ast_scope*          scope;
    ast_node_list       lvalues;
    ast_node*           list;
    ast_node*           body;
    bool                eachkey;
    bool                declare;
};

struct ast_stmt_for : public ast_node
{
    ast_stmt_for( srcloc sloc );

    ast_scope*          scope;
    ast_node*           init;
    ast_node*           condition;
    ast_node*           update;
    ast_node*           body;
};

struct ast_stmt_using : public ast_node
{
    ast_stmt_using( srcloc sloc );

    ast_scope*          scope;
    ast_node*           uvalue;
    ast_node*           body;
};

struct ast_stmt_try : public ast_node
{
    ast_stmt_try( srcloc sloc, ast_node* tstmt );

    ast_node*           tstmt;
    ast_node_list       clist;
    ast_node*           fstmt;
};

struct ast_stmt_catch : public ast_node
{
    ast_stmt_catch( srcloc sloc );

    ast_scope*          scope;
    ast_node*           lvalue;
    ast_node*           proto;
    ast_node*           body;
    bool                declare;
};

/*
    Statements.
*/

struct ast_stmt_delete : public ast_node
{
    ast_stmt_delete( srcloc sloc );

    ast_node_list       delvals;
};

struct ast_stmt_case : public ast_node
{
    ast_stmt_case( srcloc sloc, ast_node* value );

    ast_node*           value;
};

struct ast_stmt_continue : public ast_node
{
    ast_stmt_continue( srcloc sloc, ast_scope* scope,  ast_scope* target );

    ast_scope*          scope;
    ast_scope*          target;
};

struct ast_stmt_break : public ast_node
{
    ast_stmt_break( srcloc sloc, ast_scope* scope, ast_scope* target );

    ast_scope*          scope;
    ast_scope*          target;
};

struct ast_stmt_return : public ast_node
{
    ast_stmt_return( srcloc sloc, ast_scope* scope, ast_node* values );

    ast_scope*          scope;
    ast_node*           values;
};

struct ast_stmt_throw : public ast_node
{
    ast_stmt_throw( srcloc sloc, ast_node* value );

    ast_node*           value;
};

/*
    Unresolved and unqualified.
*/

struct ast_bind_name : public ast_node
{
    ast_bind_name( srcloc sloc, const char* name );

    const char*         name;
};

struct ast_bind_list : public ast_node
{
    ast_bind_list( srcloc sloc );

    ast_bind_name_list  names;
    bool                varargs;
};


struct ast_bind_qual : public ast_node
{
    ast_bind_qual( srcloc sloc, ast_node* scope, const char* name );

    ast_node*           scope;
    const char*         name;
};

}

extern template class region_list< yl::ast_node* >;
extern template class region_list< yl::ast_name* >;
extern template class region_list< yl::ast_bind_name* >;
extern template class region_list< yl::ast_op_kind >;
extern template class region_list< yl::ast_key_value >;
extern template class region_list< yl::ast_upval >;

#endif

