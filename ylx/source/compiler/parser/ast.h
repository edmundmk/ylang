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


struct ast_stmt_block;
struct ast_make_object;

struct ast_decl_name;


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
    AST_DECL_NAME,
    AST_DECL_NAME_LIST,
    AST_DECL_QUAL_NAME,
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
typedef region_list< ast_decl_name* > ast_decl_name_list;
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
    ast_decl_name_list* parameters;     // parameter declarations
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

}

#endif

