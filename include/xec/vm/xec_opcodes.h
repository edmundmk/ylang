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
    XEC_CLEAR       = 0,
    XEC_LITERAL     = 1,
    
    XEC_NEW         = 2,
    XEC_FUNCTION    = 3,
    XEC_TABLE       = 4,
    XEC_LIST        = 5,
    XEC_OBJECT      = 6,

    XEC_GETGLOBAL   = 7,
    XEC_GETUPVAL    = 8,
    XEC_GETKEY      = 9,
    XEC_GETINDEXKEY = 10,
    XEC_GETINDEX    = 11,
    XEC_SETGLOBAL   = 12,
    XEC_SETUPVAL    = 13,
    XEC_SETKEY      = 14,
    XEC_SETINDEXKEY = 15,
    XEC_SETINDEX    = 16,
    XEC_DELETE      = 17,
    XEC_CLOSEUPVAL  = 18,

    XEC_VARARGS     = 19,
    XEC_CALL        = 20,
    XEC_YIELDCALL   = 21,
    XEC_UNPACK      = 22,
    XEC_ITERATE     = 23,
    
    XEC_MOVE        = 24,
    XEC_NEG         = 25,
    XEC_NOT         = 26,
    XEC_ADD         = 27,
    XEC_SUB         = 27,
    XEC_MUL         = 28,
    XEC_DIV         = 29,
    XEC_IDIV        = 30,
    XEC_MOD         = 31,
    XEC_LSL         = 32,
    XEC_ASR         = 33,
    XEC_LSR         = 34,
    XEC_AND         = 35,
    XEC_OR          = 36,
    XEC_XOR         = 37,
    XEC_CONCAT      = 38,
    
    XEC_EQ          = 39,
    XEC_LT          = 40,
    XEC_LE          = 41,
    XEC_IN          = 42,
    XEC_IS          = 43,

    XEC_LNOT        = 44,
    XEC_LXOR        = 45,
    XEC_ISTRUE      = 46,
    XEC_ISFALSE     = 47,

    XEC_JUMP        = 48,
    
    XEC_RETURN      = 49,
    XEC_YIELD       = 50,
    XEC_THROW       = 51,
    XEC_FINALLY     = 52,
    
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
