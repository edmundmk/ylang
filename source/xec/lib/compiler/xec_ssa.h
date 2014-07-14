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
#include "xec_opcodes.h"


typedef int xec_ssavalue;
typedef int xec_ssaname;

struct xec_ssaop;
struct xec_ssafunc;
struct xec_ssastring;



/*
    SSA instructions.  For debugging, each instruction has the source location
    it came from and the original variable name it defines (if any).  There
    are several forms of instructions:
    
        -   Literals.
        -   Opcodes with one or two operands inline.
        -   With multiple operands, and producing perhaps multiple results.
        -   Jumps, where operandb is actually a jump target.

    Operands for instructions with more than two operands (or with mutliple
    results) are stored out of line in a second array.
*/

struct xec_ssaop
{
    xec_opcode              opcode      : 8;
    xec_ssaname             name        : 24;
    int                     sloc;
    union
    {
        // Literals.
        double              number;
        bool                boolean;
        xec_ssastring*      string;
        
        // One or two-operand instructions.
        struct
        {
            xec_ssavalue    operanda;
            xec_ssavalue    operandb;
        };
        
        // Multi-operand instructions producing multiple values.
        struct
        {
            int             valcount    : 20;
            int             oolindex    : 20;
            int             oolcount    : 20;
        };
    };
};



/*
    A function in SSA form.
*/

struct xec_ssafunc
{
    const char*                 funcname;
    int                         upvalcount;
    std::deque< xec_ssaop >     ops;
    std::deque< xec_ssavalue >  ool;
    std::unordered_map< xec_ssaname, const char* > names;
    std::deque< std::unique_ptr< xec_ssastring > > strings;
};



/*
    Literal strings must have length in case they contain \0s.
*/

struct xec_ssastring
{
    size_t          length;
    const char*     string;
};




#endif



