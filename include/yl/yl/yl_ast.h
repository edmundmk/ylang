//
//  yl_ast.h
//
//  Created by Edmund Kapusniak on 07/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_AST_H
#define XEC_AST_H


#include <deque>
#include <unordered_map>
#include <region.h>
#include <symkey.h>


class yl_script;

struct yl_ast_scope;
struct yl_ast_name;
struct yl_ast_prototype;
struct yl_ast_upval;
struct yl_ast_node;
struct yl_ast_func;
struct yl_new_object;
struct yl_expr_list;
struct yl_stmt_block;
struct yl_name_name;
struct yl_name_list;
struct yl_key_value;



enum yl_ast_scope_kind
{
    XEC_SCOPE_SCRIPT,
    XEC_SCOPE_BLOCK,
    XEC_SCOPE_SWITCH,
    XEC_SCOPE_IMPLICIT,
    XEC_SCOPE_OBJECT,
    XEC_SCOPE_IMPLIED,
};


enum yl_ast_node_kind
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
    XEC_NEW_ARRAY,
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
    XEC_NAME_NAME,
    XEC_NAME_LIST,
    XEC_NAME_QUAL,

};


enum yl_ast_opkind
{
    XEC_ASTOP_DECLARE,

    XEC_ASTOP_ASSIGN,
    XEC_ASTOP_MULASSIGN,
    XEC_ASTOP_DIVASSIGN,
    XEC_ASTOP_MODASSIGN,
    XEC_ASTOP_INTDIVASSIGN,
    XEC_ASTOP_ADDASSIGN,
    XEC_ASTOP_SUBASSIGN,
    XEC_ASTOP_LSHIFTASSIGN,
    XEC_ASTOP_LRSHIFTASSIGN,
    XEC_ASTOP_ARSHIFTASSIGN,
    XEC_ASTOP_BITANDASSIGN,
    XEC_ASTOP_BITXORASSIGN,
    XEC_ASTOP_BITORASSIGN,
    
    XEC_ASTOP_PREINC,
    XEC_ASTOP_PREDEC,
    
    XEC_ASTOP_POSTINC,
    XEC_ASTOP_POSTDEC,

    XEC_ASTOP_POSITIVE,
    XEC_ASTOP_NEGATIVE,
    XEC_ASTOP_LOGICNOT,
    XEC_ASTOP_BITNOT,
    
    XEC_ASTOP_MULTIPLY,
    XEC_ASTOP_DIVIDE,
    XEC_ASTOP_MODULUS,
    XEC_ASTOP_INTDIV,
    XEC_ASTOP_ADD,
    XEC_ASTOP_SUBTRACT,
    XEC_ASTOP_LSHIFT,
    XEC_ASTOP_LRSHIFT,
    XEC_ASTOP_ARSHIFT,
    XEC_ASTOP_BITAND,
    XEC_ASTOP_BITXOR,
    XEC_ASTOP_BITOR,
    XEC_ASTOP_CONCATENATE,
    
    XEC_ASTOP_EQUAL,
    XEC_ASTOP_NOTEQUAL,
    XEC_ASTOP_LESS,
    XEC_ASTOP_GREATER,
    XEC_ASTOP_LESSEQUAL,
    XEC_ASTOP_GREATEREQUAL,
    XEC_ASTOP_IN,
    XEC_ASTOP_NOTIN,
    XEC_ASTOP_IS,
    XEC_ASTOP_NOTIS,
    
    XEC_ASTOP_LOGICAND,
    XEC_ASTOP_LOGICXOR,
    XEC_ASTOP_LOGICOR,
};


enum yl_ast_upval_kind
{
    XEC_UPVAL_LOCAL,
    XEC_UPVAL_OBJECT,
    XEC_UPVAL_UPVAL,
};



// TODO: replace std::deque with a container with a smaller chunk size

typedef std::deque< yl_ast_node*,
    region_allocator< yl_ast_node* > > yl_ast_node_list;
typedef std::deque< yl_ast_name*,
    region_allocator< yl_ast_name* > > yl_ast_name_list;
typedef std::deque< yl_name_name*,
    region_allocator< yl_name_name* > > yl_name_name_list;
typedef std::deque< yl_ast_opkind,
    region_allocator< yl_ast_opkind > > yl_opkind_list;
typedef std::deque< yl_key_value,
    region_allocator< yl_key_value > > yl_key_value_list;
typedef std::unordered_map
        <
            symkey,
            yl_ast_name*,
            std::hash< symkey >,
            std::equal_to< symkey >,
            region_allocator< std::pair< const symkey, yl_ast_name* > >
        >
        yl_ast_name_map;
typedef std::unordered_map
        <
            symkey,
            yl_ast_scope*,
            std::hash< symkey >,
            std::equal_to< symkey >,
            region_allocator< std::pair< const symkey, yl_ast_scope* > >
        >
        yl_ast_imply_map;
typedef std::deque< yl_ast_upval,
    region_allocator< yl_ast_upval > > yl_ast_upval_list;



/*
    A script file parsed into an AST.
*/


struct yl_ast
{
    yl_ast();
    ~yl_ast();

    region              alloc;
    yl_script*         script;
    yl_ast_func*       function;
    std::deque< yl_ast_func* > functions;

};




/*
    Scopes contain names and prototypes.
*/


struct yl_ast_scope
{
    yl_ast_scope( yl_ast_scope_kind kind,
            yl_ast_scope* outer, yl_ast_node* node, yl_ast_func* func );

    yl_ast_scope_kind  kind;
    yl_ast_scope*      outer;
    yl_ast_node*       node;
    yl_ast_func*       func;
    yl_stmt_block*     block;
    yl_ast_imply_map   implied;
    yl_ast_name_map    names;
    yl_ast_name_list   decls;
    bool                continued;
    bool                dowhile;
};


struct yl_ast_name
{
    yl_ast_name( int sloc, yl_ast_scope* scope, const char* name );

    int                 sloc;
    yl_ast_scope*      scope;
    const char*         name;
    yl_ast_name*       superthis;
    yl_ast_prototype*  prototype;
    bool                upval;
    bool                continued;
};


struct yl_ast_prototype
{
    yl_ast_prototype( int sloc );

    int                 sloc;
    yl_name_list*      parameters;
    bool                coroutine;
};



/*
    Upvals are used when accessing variables in outer scopes.
*/

struct yl_ast_upval
{
    yl_ast_upval( yl_ast_name* name );
    yl_ast_upval( yl_new_object* object );
    yl_ast_upval( int upval );

    bool operator == ( const yl_ast_upval& b ) const;
    bool operator != ( const yl_ast_upval& b ) const;

    yl_ast_upval_kind  kind;
    union
    {
        yl_ast_name*   local;
        yl_new_object* object;
        int             upval;
    };
};



/*
    AST nodes.
*/


struct yl_ast_node
{
    yl_ast_node( yl_ast_node_kind kind, int sloc );

    yl_ast_node_kind   kind;
    int                 sloc;
};



/*
    Functions.
*/

struct yl_ast_func : public yl_ast_node
{
    yl_ast_func( int sloc );

    const char*         funcname;
    yl_ast_scope*      scope;
    yl_ast_upval_list  upvals;
    yl_new_object*     memberof;
    yl_ast_name*       thisname;
    yl_ast_name_list   parameters;
    yl_stmt_block*     block;
    bool                varargs;
    bool                coroutine;
};


/*
    Literals.
*/

struct yl_expr_null : public yl_ast_node
{
    explicit yl_expr_null( int sloc );
};


struct yl_expr_bool : public yl_ast_node
{
    yl_expr_bool( int sloc, bool value );

    bool                value;
};


struct yl_expr_number : public yl_ast_node
{
    yl_expr_number( int sloc, double value );

    double              value;
};


struct yl_expr_string : public yl_ast_node
{
    yl_expr_string( int sloc, const char* string, size_t length );

    const char*         string;
    size_t              length;
};


/*
    Names.
*/

struct yl_expr_local : public yl_ast_node
{
    yl_expr_local( int sloc, yl_ast_name* name );

    yl_ast_name*       name;
};


struct yl_expr_global : public yl_ast_node
{
    yl_expr_global( int sloc, const char* name, bool gexplicit );

    const char*         name;
    bool                gexplicit;
};


struct yl_expr_upref : public yl_ast_node
{
    yl_expr_upref( int sloc, yl_ast_func* func, int index );

    yl_ast_func*       func;
    int                 index;
};


struct yl_expr_objref : public yl_ast_node
{
    yl_expr_objref( int sloc, yl_new_object* object );
    
    yl_new_object*     object;
};


/*
    Lookup.
*/

struct yl_expr_key : public yl_ast_node
{
    yl_expr_key( int sloc, yl_ast_node* object, const char* key );

    yl_ast_node*       object;
    const char*         key;
};


struct yl_expr_inkey : public yl_ast_node
{
    yl_expr_inkey( int sloc, yl_ast_node* object, yl_ast_node* key );

    yl_ast_node*       object;
    yl_ast_node*       key;
};


struct yl_expr_index : public yl_ast_node
{
    yl_expr_index( int sloc, yl_ast_node* object, yl_ast_node* index );

    yl_ast_node*       object;
    yl_ast_node*       index;
};


/*
    Operators.
*/

struct yl_expr_preop : public yl_ast_node
{
    yl_expr_preop( int sloc, yl_ast_opkind opkind, yl_ast_node* lvalue );

    yl_ast_opkind      opkind;
    yl_ast_node*       lvalue;
};


struct yl_expr_postop : public yl_ast_node
{
    yl_expr_postop( int sloc, yl_ast_opkind opkind, yl_ast_node* lvalue );

    yl_ast_opkind      opkind;
    yl_ast_node*       lvalue;
};


struct yl_expr_unary : public yl_ast_node
{
    yl_expr_unary( int sloc, yl_ast_opkind opkind, yl_ast_node* operand );

    yl_ast_opkind      opkind;
    yl_ast_node*       operand;
};


struct yl_expr_binary : public yl_ast_node
{
    yl_expr_binary( int sloc,
            yl_ast_opkind opkind, yl_ast_node* lhs, yl_ast_node* rhs );

    yl_ast_opkind      opkind;
    yl_ast_node*       lhs;
    yl_ast_node*       rhs;
};


struct yl_expr_compare : public yl_ast_node
{
    yl_expr_compare( int sloc, yl_ast_node* first );
    
    yl_ast_node*       first;
    yl_opkind_list     opkinds;
    yl_ast_node_list   terms;
};


struct yl_expr_logical : public yl_ast_node
{
    yl_expr_logical( int sloc,
            yl_ast_opkind opkind, yl_ast_node* lhs, yl_ast_node* rhs );

    yl_ast_opkind      opkind;
    yl_ast_node*       lhs;
    yl_ast_node*       rhs;
};


struct yl_expr_qmark : public yl_ast_node
{
    yl_expr_qmark( int sloc, yl_ast_node* condition,
            yl_ast_node* iftrue, yl_ast_node* iffalse );

    yl_ast_node*       condition;
    yl_ast_node*       iftrue;
    yl_ast_node*       iffalse;
};



/*
    New.
*/

struct yl_new_new : public yl_ast_node
{
    yl_new_new( int sloc, yl_ast_node* proto, yl_expr_list* args );

    yl_ast_node*       proto;
    yl_expr_list*      arguments;
};


struct yl_new_object : public yl_ast_node
{
    yl_new_object( int sloc, yl_ast_node* proto );

    yl_ast_scope*      scope;
    yl_ast_node*       proto;
    yl_ast_node_list   members;
    bool                upval;
};


struct yl_new_array : public yl_ast_node
{
    yl_new_array( int sloc );

    yl_ast_node_list   values;
    yl_ast_node*       final;
};


struct yl_key_value
{
    yl_key_value( yl_ast_node* key, yl_ast_node* value );

    yl_ast_node*       key;
    yl_ast_node*       value;
};


struct yl_new_table : public yl_ast_node
{
    yl_new_table( int sloc );

    yl_key_value_list   elements;
};



/*
    Unpack.
*/

struct yl_expr_mono : public yl_ast_node
{
    yl_expr_mono( int sloc, yl_ast_node* expr );

    yl_ast_node*       expr;
};


struct yl_expr_call : public yl_ast_node
{
    yl_expr_call( int sloc, yl_ast_node* function,
                    yl_expr_list* args, bool yieldcall );

    yl_ast_node*       function;
    yl_expr_list*      arguments;
    bool                yieldcall;
    bool                unpack;
};


struct yl_expr_yield : public yl_ast_node
{
    yl_expr_yield( int sloc, yl_ast_node* args );

    yl_ast_node*       arguments;
    bool                unpack;
};


struct yl_expr_vararg : public yl_ast_node
{
    yl_expr_vararg( int sloc );
};


struct yl_expr_unpack : public yl_ast_node
{
    yl_expr_unpack( int sloc, yl_ast_node* list );

    yl_ast_node*       array;
};


struct yl_expr_list : public yl_ast_node
{
    explicit yl_expr_list( int sloc );

    yl_ast_node_list   values;
    yl_ast_node*       final;
};



/*
    Assignment.
*/

struct yl_expr_assign : public yl_ast_node
{
    yl_expr_assign( int sloc, yl_ast_opkind assignop );

    yl_ast_opkind      assignop;
    yl_ast_node*       lvalue;
    yl_ast_node*       rvalue;
};


struct yl_expr_assign_list : public yl_ast_node
{
    yl_expr_assign_list( int sloc, yl_ast_opkind assignop );

    yl_ast_opkind      assignop;
    yl_ast_node_list   lvalues;
    yl_ast_node*       rvalues;
};


/*
    Statements w/ scopes.
*/

struct yl_stmt_block : public yl_ast_node
{
    yl_stmt_block( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node_list   stmts;
};


struct yl_stmt_if : public yl_ast_node
{
    yl_stmt_if( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node*       condition;
    yl_ast_node*       iftrue;
    yl_ast_node*       iffalse;
};


struct yl_stmt_switch : public yl_ast_node
{
    yl_stmt_switch( int sloc );
    
    yl_ast_scope*      scope;
    yl_ast_node*       value;
    yl_stmt_block*     body;
};


struct yl_stmt_while : public yl_ast_node
{
    yl_stmt_while( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node*       condition;
    yl_ast_node*       body;
};


struct yl_stmt_do : public yl_ast_node
{
    yl_stmt_do( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node*       body;
    yl_ast_node*       condition;
};


struct yl_stmt_foreach : public yl_ast_node
{
    yl_stmt_foreach( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node_list   lvalues;
    yl_ast_node*       list;
    yl_ast_node*       body;
    bool                eachkey;
    bool                declare;
};


struct yl_stmt_for : public yl_ast_node
{
    yl_stmt_for( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node*       init;
    yl_ast_node*       condition;
    yl_ast_node*       update;
    yl_ast_node*       body;
};


struct yl_stmt_using : public yl_ast_node
{
    yl_stmt_using( int sloc );
    
    yl_ast_scope*      scope;
    yl_ast_node*       uvalue;
    yl_ast_node*       body;
};

struct yl_stmt_try : public yl_ast_node
{
    yl_stmt_try( int sloc, yl_ast_node* tstmt );

    yl_ast_node*       tstmt;
    yl_ast_node_list   clist;
    yl_ast_node*       fstmt;
};


struct yl_stmt_catch : public yl_ast_node
{
    yl_stmt_catch( int sloc );

    yl_ast_scope*      scope;
    yl_ast_node*       lvalue;
    yl_ast_node*       proto;
    yl_ast_node*       body;
    bool                declare;
};


/*
    Statements.
*/

struct yl_stmt_delete : public yl_ast_node
{
    yl_stmt_delete( int sloc );

    yl_ast_node_list   delvals;
};


struct yl_stmt_case : public yl_ast_node
{
    yl_stmt_case( int sloc, yl_ast_node* value );

    yl_ast_node*       value;
};


struct yl_stmt_continue : public yl_ast_node
{
    yl_stmt_continue( int sloc, yl_ast_scope* scope,  yl_ast_scope* target );

    yl_ast_scope*      scope;
    yl_ast_scope*      target;
};


struct yl_stmt_break : public yl_ast_node
{
    yl_stmt_break( int sloc, yl_ast_scope* scope, yl_ast_scope* target );

    yl_ast_scope*      scope;
    yl_ast_scope*      target;
};


struct yl_stmt_return : public yl_ast_node
{
    yl_stmt_return( int sloc, yl_ast_scope* scope, yl_ast_node* values );

    yl_ast_scope*      scope;
    yl_ast_node*       values;
};


struct yl_stmt_throw : public yl_ast_node
{
    yl_stmt_throw( int sloc, yl_ast_node* value );

    yl_ast_node*       value;
};



/*
    Unresolved and unqualified.
*/

struct yl_name_name : public yl_ast_node
{
    yl_name_name( int sloc, const char* name );
    
    const char*         name;
};


struct yl_name_qual : public yl_ast_node
{
    yl_name_qual( int sloc, yl_ast_node* scope, const char* name );
    
    yl_ast_node*       scope;
    const char*         name;
};


struct yl_name_list : public yl_ast_node
{
    yl_name_list( int sloc );
    
    yl_name_name_list  names;
    bool                varargs;
};




/*
    AST printing.
*/

const char* yl_operator_name( yl_ast_opkind op );
void yl_ast_print( yl_ast* root );





#endif
