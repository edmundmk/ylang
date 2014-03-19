//
//  xec_parser.h
//
//  Created by Edmund Kapusniak on 16/03/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_PARSER_H
#define XEC_PARSER_H


#include <stdint.h>
#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <region.h>
#include <symbol.h>


struct xec_token;
struct xec_token_lookup;

struct xec_ast_node;
struct xec_ast_block;
struct xec_ast_values;


typedef std::deque
        <
            xec_ast_node*,
            region_allocator< xec_ast_node* >
        >
        xec_ast_node_list;

typedef std::unordered_map
        <
            symkey,
            xec_ast_node*,
            std::hash< symkey >,
            std::equal_to< symkey >,
            region_allocator< std::pair< symkey, xec_ast_node* > >
        >
        xec_ast_node_map;


enum xec_ast_kind
{
    XEC_AST_BLOCK,
    XEC_AST_VALUES,
    XEC_AST_DECL,
    XEC_AST_VAR,
    XEC_AST_NAME,
    XEC_AST_INTEGER,
    XEC_AST_FLOAT,
    XEC_AST_STRING,
    XEC_AST_BOOLEAN,
    XEC_AST_NULL,
    XEC_AST_VARARGS,
    XEC_AST_LOOKUP,
    XEC_AST_INDEXLOOKUP,
    XEC_AST_INDEX,
    XEC_AST_CALL,
    XEC_AST_UNARYOP,
    XEC_AST_BINARYOP,
    XEC_AST_QMARK,
    XEC_AST_NEW,
    XEC_AST_NEWARRAY,
    XEC_AST_NEWTABLE,
    XEC_AST_KEYVAL,
    XEC_AST_NEWOBJ,
    XEC_AST_NEWFUNC,
    XEC_AST_ASSIGN,
    XEC_AST_UNPACK,
};

enum xec_ast_unaryop_kind
{
    XEC_AST_POSTINCREMENT,
    XEC_AST_POSTDECREMENT,
    XEC_AST_UNARYPLUS,
    XEC_AST_UNARYMINUS,
    XEC_AST_LOGICNOT,
    XEC_AST_BITNOT,
    XEC_AST_PREINCREMENT,
    XEC_AST_PREDECREMENT,
};

enum xec_ast_binaryop_kind
{
    XEC_AST_MUL,
    XEC_AST_DIV,
    XEC_AST_MOD,
    XEC_AST_INTDIV,
    XEC_AST_ADD,
    XEC_AST_SUB,
    XEC_AST_LSHIFT,
    XEC_AST_RSHIFT,
    XEC_AST_URSHIFT,
    XEC_AST_BITAND,
    XEC_AST_BITXOR,
    XEC_AST_BITOR,
    XEC_AST_LOGICAND,
    XEC_AST_LOGICXOR,
    XEC_AST_LOGICOR,
};


struct xec_ast_node
{
    xec_ast_kind            kind;
    int                     sloc;
};

struct xec_ast_block : public xec_ast_node
{
    xec_ast_block*          outer;
    xec_ast_node_map        decls;
    xec_ast_node_list       stmts;
};

struct xec_ast_values : public xec_ast_node
{
    xec_ast_node_list       values;
    xec_ast_node*           final;
};

struct xec_ast_decl  : public xec_ast_node
{
    xec_ast_node*           name;
    xec_ast_node*           decl;
};

struct xec_ast_var : public xec_ast_node
{
    xec_ast_node*           name;
};

struct xec_ast_name : public xec_ast_node
{
    hash32_t                hash;
    const char*             text;
    size_t                  size;
};

struct xec_ast_integer : public xec_ast_node
{
    intmax_t                number;
};

struct xec_ast_float : public xec_ast_node
{
    double                  number;
};

struct xec_ast_string : public xec_ast_node
{
    const char*             string;
    size_t                  length;
};

struct xec_ast_boolean : public xec_ast_node
{
    bool                    boolean;
};

struct xec_ast_lookup : public xec_ast_node
{
    xec_ast_node*           expr;
    xec_ast_name*           lookup;
};

struct xec_ast_indexlookup : public xec_ast_node
{
    xec_ast_node*           expr;
    xec_ast_node*           lookup;
};

struct xec_ast_index : public xec_ast_node
{
    xec_ast_node*           expr;
    xec_ast_node*           index;
};

struct xec_ast_call : public xec_ast_node
{
    xec_ast_node*           func;
    xec_ast_values*         args;
    bool                    isyield;
};

struct xec_ast_unaryop : public xec_ast_node
{
    xec_ast_unaryop_kind    opkind;
    xec_ast_node*           expr;
};

struct xec_ast_binaryop : public xec_ast_node
{
    xec_ast_binaryop_kind   opkind;
    xec_ast_node*           left;
    xec_ast_node*           right;
};

struct xec_ast_qmark : public xec_ast_node
{
    xec_ast_node*           condition;
    xec_ast_node*           iftrue;
    xec_ast_node*           iffalse;
};

struct xec_ast_new : public xec_ast_node
{
    xec_ast_node*           proto;
    xec_ast_values*         args;
};

struct xec_ast_newarray : public xec_ast_node
{
    xec_ast_values*         elements;
};

struct xec_ast_newtable : public xec_ast_node
{
    xec_ast_node_list       entries;
};

struct xec_ast_keyval : public xec_ast_node
{
    xec_ast_node*           key;
    xec_ast_node*           val;
};

struct xec_ast_newobj : public xec_ast_node
{
    xec_ast_node*           proto;
    xec_ast_block*          block;
};

struct xec_ast_newfunc : public xec_ast_node
{
    xec_ast_values*         params;
    xec_ast_block*          block;
};

struct xec_ast_assign : public xec_ast_node
{
    xec_ast_values*         lvals;
    xec_ast_values*         rvals;
};

struct xec_ast_unpack : public xec_ast_node
{
    xec_ast_node*           array;
};








class xec_parser
{
public:

    xec_parser();
    explicit xec_parser( const char* filename );
    ~xec_parser();

    bool                parse( const char* filename );

    const char*         get_filename();
    int                 get_line( int sloc );
    int                 get_column( int sloc );
    
    void                diagnostic( int sloc, const char* format, ... );
    size_t              diagnostic_count();
    const char*         diagnostic( size_t index );


/* lemon private: */


private:

    static const size_t BUFFER_SIZE = 4 * 1024 * 1024;
    static const size_t ERROR_LIMIT = 25;
    
    template < typename ... arguments_t >
        xec_token* make_token( arguments_t ... arguments );
    
    xec_token_lookup lookup_identifier( region_buffer* data );
    bool encode_utf8( region_buffer* data, uint32_t codepoint );

    
    region                          alloc;
    std::string                     filename;
    std::deque< int >               newlines;
    std::deque< const char* >       diagnostics;
    std::deque< void* >             recycle_tokens;
    std::unordered_set< symkey >    identifiers;
    
};



#endif
