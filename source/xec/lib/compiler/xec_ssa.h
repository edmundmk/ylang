//
//  xec_ssa.h
//
//  Created by Edmund Kapusniak on 09/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_H
#define XEC_SSA_H


#include <memory>
#include <unordered_map>
#include <string>
#include <deque>
#include <region.h>



struct xec_ssa_func;
struct xec_ssa_name;
struct xec_ssa_block;
struct xec_ssa_node;
struct xec_ssa_bool;
struct xec_ssa_number;
struct xec_ssa_string;
struct xec_ssa_operand;



/*
    SSA form for scripts,.
*/


enum xec_ssa_opcode
{
    XEC_SSA_PHI,
    XEC_SSA_PARAM,
    
    XEC_SSA_NULL,
    XEC_SSA_BOOL,
    XEC_SSA_NUMBER,
    XEC_SSA_STRING,
};


typedef std::deque< xec_ssa_func*,
    region_allocator< xec_ssa_func* > > xec_ssa_func_list;
typedef std::deque< xec_ssa_node*,
    region_allocator< xec_ssa_node* > > xec_ssa_node_list;



struct xec_ssa
{
    region              alloc;
    xec_script*         script;
    xec_ssa_func*       function;
    std::deque< xec_ssa_func* > functions;
};


struct xec_ssa_func
{
    int                 sloc;
    const char*         funcname;
    xec_ssa_block*      block;
    int                 upvalcount;
    int                 paramcount;
    bool                varargs;
    bool                coroutine;
};


struct xec_ssa_name
{
    int                 sloc;
    const char*         name;
};



struct xec_ssa_block
{
    xec_ssa_block*      prev;
    xec_ssa_node*       node;
    xec_ssa_node*       condition;
    union
    {
        xec_ssa_block*  next;
        struct
        {
        xec_ssa_block*  iftrue;
        xec_ssa_block*  iffalse;
        };
    };
};


struct xec_ssa_node
{
    int                 sloc;
    xec_ssa_opcode      opcode;
    xec_ssa_node*       next;
    xec_ssa_name*       name;
    
    size_t              operand_count();
    xec_ssa_node*       operand( size_t index );

    xec_ssa_bool*       as_bool();
    xec_ssa_number*     as_number();
    xec_ssa_string*     as_string();
    
};


struct xec_ssa_bool : public xec_ssa_node
{
    bool                value;
};


struct xec_ssa_number : public xec_ssa_node
{
    double              value;
};


struct xec_ssa_string : public xec_ssa_node
{
    const char*         string;
    size_t              length;
};





#endif



