//
//  xec_code.h
//
//  Created by Edmund Kapusniak on 21/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_CODE_H
#define XEC_CODE_H


#include <stdint.h>


/*
    A simple instruction set designed to implement the xec language.  Owes
    a debt to the design of the Lua virtual machine (as does the language).
    
    Potential extensions allowing very large functions which exceed some of
    the current limits are shown commented out.
*/

enum xec_opcode
{
    XEC_NOP,
    
    XEC_NULL,       // r = null
    XEC_K,          // r = constants[ c ]
    
//  XEC_SPILL,      // longr[ c ] = r
//  XEC_LOAD,       // r = longr[ c ]
    
    XEC_MOV,        // r = a
    XEC_SWP,        // swap r and a

    XEC_POS,        // r = (number)a
    XEC_NEG,        // r = -a
    XEC_NOT,        // r = ~a
    
    XEC_MUL,        // r = a * b
    XEC_DIV,        // r = a / b
    XEC_MOD,        // r = a - floor( a / b ) * b
    XEC_INTDIV,     // r = floor( a / b )
    XEC_ADD,        // r = a + b
    XEC_SUB,        // r = a - b
    XEC_LSL,        // r = (uint32)a << b
    XEC_LSR,        // r = (uint32)a >> b (logical)
    XEC_ASR,        // r = (uint32)a >> b (arithmetic)
    XEC_AND,        // r = a & b
    XEC_XOR,        // r = a ^ b
    XEC_OR,         // r = a | b
    XEC_CONCAT,     // r = (string) a + b

    XEC_LVAL,       // r = (bool)a
    XEC_LNOT,       // r = not (bool)a
    XEC_LXOR,       // r = (bool)a xor (bool)b
    
    XEC_KEY,        // r = a lookup keys[ b ]
    XEC_INKEY,      // r = a lookup (string)b
    XEC_INDEX,      // r = a index b
    XEC_ELEM,       // r = a index (immediate)b
    XEC_GLOBAL,     // r = global keys[ b ]
    
    XEC_SETKEY,     // a lookup keys[ b ] = r
    XEC_SETINKEY,   // a lookup (string)b = r
    XEC_SETINDEX,   // a index b = r
    XEC_SETGLOBAL,  // global keys[ b ] = r
    
    XEC_DELKEY,     // a delete keys[ b ]
    XEC_DELINKEY,   // a delete (string)b
    
    XEC_NEWUP,      // upvals[ c ] = new upval, upvals[ c ] = r
    XEC_SETUP,      // upvals[ c ] = r
    XEC_REFUP,      // r = upvals[ c ]
    XEC_CLOSE,      // close upvals[ r .. r + c ]
    
    XEC_EQ,         // r = a == b
    XEC_LT,         // r = a < b
    XEC_LE,         // r = a <= b
    XEC_IN,         // r = a in b
    XEC_IS,         // r = a is b

    XEC_JMP,        // pc = address of next instruction + j
    XEC_IFTRUE,     // if (bool)r pc = address of next instruction + j
    XEC_IFFALSE,    // if not (bool)r pc = address of next instruction + j
//  XEC_LJMP,       // pc = longjmps[ c ]
//  XEC_LIFTRUE,    // if (bool)r pc = longjmps[ c ]
//  XEC_LIFFALSE,   // if not (bool)r pc = longjmps[ c ]

    XEC_ITER,       // r = iterator over indexes of a
    XEC_ITERKEY,    // r = iterator over keys of a
    XEC_NEXT,       // r .. r + b = generate (iterator)a

    XEC_TABLE,      // r = new table (reserve c indexes)

    XEC_ARRAY,      // r = new array (reserve c indexes)
    XEC_UNPACK,     // r .. set @mark = a[ (immediate)b .. end ]
    XEC_APPEND,     // append r to (array)a
    XEC_EXTEND,     // append r .. @mark to (array)a
    
    XEC_CLOSURE,    // r = function closure of functions[ c ]
    XEC_ENVUP,      // add upvals[ c ] to previous function closure

    XEC_VARARG,     // r = varargs[ c ]
    XEC_VARALL,     // r .. set @top = varargs
    
    XEC_CALL,       // r .. r + b = call r .. r + a (with @mark)
    XEC_YCALL,      // r .. r + b = yield call r .. r + a (with @mark)
    XEC_YIELD,      // r .. r + b = yield r .. r + a (with @mark)
    XEC_NEW,        // r = new a .. a + b (with @mark)
    
    XEC_RETURN,     // return r .. r + a (with @mark)

};



/*
    There are three instruction packings:
    
        | opcode  |    r    |    a    |    b    |
        | opcode  |    r    |         c         |
        | opcode  |    r    |         j         |

    Where r and a always index registers, and b either indexes a register or
    is an unsigned index.  c is an unsigned index, while j is a signed jump
    offset from the next instruction.
    
    a and b can also be register counts, or the special value XEC_MARK.
*/

struct xec_opinst
{
    xec_opinst( xec_opcode opcode, unsigned r, unsigned a, unsigned b );
    xec_opinst( xec_opcode opcode, unsigned r, unsigned c );
    xec_opinst( xec_opcode opcode, unsigned r, signed j );
    
    xec_opcode  opcode();
    unsigned    r();
    unsigned    a();
    unsigned    b();
    unsigned    c();
    signed      j();


    uint32_t i;
    
};



/*
    This marker indicates that instead of a fixed number of values, either
    unpack all available values and set the @mark, or use all values from
    the base up to the @mark.
*/

static const unsigned XEC_MARK = 0xFF;





/*
*/


inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, unsigned a, unsigned b )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint8_t)a << 16
            | (uint8_t)b << 24 )
{
}

inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, unsigned c )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint16_t)c << 16 )
{
}

inline xec_opinst::xec_opinst(
                xec_opcode opcode, unsigned r, signed j )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (int16_t)j << 16 )
{
}


inline xec_opcode   xec_opinst::opcode()    { return (xec_opcode)(uint8_t)i; }
inline unsigned     xec_opinst::r()         { return (uint8_t)( i >> 8 );    }
inline unsigned     xec_opinst::a()         { return (uint8_t)( i >> 16 );   }
inline unsigned     xec_opinst::b()         { return (uint8_t)( i >> 24 );   }
inline unsigned     xec_opinst::c()         { return (uint16_t)( i >> 16 );  }
inline signed       xec_opinst::j()         { return (int16_t)( i >> 16 );   }






#endif
