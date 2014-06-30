//
//  xec_opcodes.h
//
//  Created by Edmund Kapusniak on 21/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_OPCODES_H
#define XEC_OPCODES_H



enum xec_opcode
{
    XEC_SETNULL     = 0,
    XEC_SETVALUE    = 1,
    
    XEC_NEW         = 2,
    XEC_FUNCTION    = 3,
    XEC_TABLE       = 4,
    XEC_LIST        = 5,
    XEC_OBJECT      = 6,

    XEC_GETGLOBAL   = 7,
    XEC_GETUPVAL    = 8,
    XEC_GETKEY      = 9,
    XEC_GETXKEY     = 10,
    XEC_GETINDEX    = 11,
    XEC_PUTGLOBAL   = 12,
    XEC_PUTUPVAL    = 13,
    XEC_PUTKEY      = 14,
    XEC_PUTXKEY     = 15,
    XEC_PUTINDEX    = 16,
    XEC_DELETE      = 17,
    XEC_CLOSE       = 18,

    XEC_MOVE        = 19,
    XEC_NEG         = 20,
    XEC_NOT         = 21,
    XEC_ADD         = 22,
    XEC_SUB         = 23,
    XEC_MUL         = 24,
    XEC_DIV         = 25,
    XEC_IDIV        = 26,
    XEC_MOD         = 27,
    XEC_LSL         = 28,
    XEC_ASR         = 29,
    XEC_LSR         = 30,
    XEC_AND         = 31,
    XEC_OR          = 32,
    XEC_XOR         = 33,
    XEC_CONCAT      = 34,
    
    XEC_EQ          = 35,
    XEC_LT          = 36,
    XEC_LE          = 37,
    XEC_IN          = 38,
    XEC_IS          = 39,

    XEC_LNOT        = 40,
    XEC_LXOR        = 41,
    XEC_IFTRUE      = 42,
    XEC_IFFALSE     = 43,
    XEC_JUMP        = 44,
    
    XEC_UNPACK      = 45,
    XEC_VARARGS     = 46,
    XEC_CALL        = 47,
    XEC_YIELDCALL   = 48,
    XEC_YIELD       = 49,
    XEC_RETURN      = 50,

    XEC_THROW       = 51,
    XEC_FINALLY     = 52,
    
    XEC_ITERATE     = 53,

};



class xec_instruction
{
public:

    xec_instruction();
    xec_instruction( xec_opcode opcode, unsigned r, unsigned a, unsigned b );

    xec_opcode opcode() const;
    unsigned r() const;
    unsigned a() const;
    unsigned b() const;

private:

    uint32_t i;

};




#endif
