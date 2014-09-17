//
//  xec_lj_builder.h
//
//  Created by Edmund Kapusniak on 10/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_LJ_BUILDER_H
#define XEC_LJ_BUILDER_H


#include <stdarg.h>
#include <unordered_map>
#include <unordered_set>
#include "xec_lj_expr.h"
#include "xec_lj_stmt.h"



class xec_srcbuf;


static const int INDENT = 4;



class xec_lj_scope
{
public:

    xec_lj_scope();

    void    reset();
    void    begin();
    std::pair< int, bool > temporary();
    

private:

    int     decltemp;
    int     exprtemp;

};




class xec_lj_value
{
public:

    xec_lj_value();

    bool has_prologue();
    bool has_side_effect();
    void prologue();
    void value();
    void values();


private:

    friend class xec_lj_builder;
    friend class xec_lj_exprhasprologue;
    friend class xec_lj_exprprologue;
    friend class xec_lj_exprvalue;

    xec_lj_builder*     b;
    xec_ast_node*       expr;
    xec_lj_scope*       scope;
    int                 indent;
    std::unordered_set< xec_ast_node* > pronodes;
    std::unordered_map< xec_ast_node*, int > temporaries;
    bool                callnodes;

};



class xec_lj_lvprol
{
public:

    xec_lj_lvprol();
    
    void prologue();
    void value();
    
    
private:
    
    friend class xec_lj_builder;
    
    int temporary( xec_ast_node* expr );
    
    xec_lj_builder*     b;
    xec_ast_node*       expr;
    xec_lj_scope*       scope;
    int                 indent;
    int                 objval;
    int                 idkval;
    
    
};




class xec_lj_builder
{
public:

    explicit xec_lj_builder( xec_srcbuf* b );

    void printf( const char* format, ... )                   __attribute__(( __format__( printf, 2, 3 ) ));
    void vprintf( const char* format, va_list arguments )    __attribute__(( __format__( printf, 2, 0 ) ));
    
    xec_ast_upval* upval( xec_expr_upref* upref );

    bool is_keyword( const char* identifier );
    void local( const char* local );
    void global( const char* global );
    void key( const char* key );

    int  object( xec_new_object* object );

    void body( xec_ast_func* func, int indent );
    
    void expr( xec_lj_value* value,
                    xec_ast_node* expr, xec_lj_scope* scope, int indent );
    void lvprol( xec_lj_lvprol* value,
                    xec_ast_node* lval, xec_lj_scope* scope, int indent );

    int  temporary( xec_lj_scope* scope, int indent );
    int  temporary( xec_ast_node* expr, xec_lj_scope* scope, int indent );

    std::pair< const char*, int > op( xec_ast_opkind opkind );


private:

    xec_srcbuf* b;
    std::unordered_map< xec_new_object*, int > objects;

};




#endif
