//
//  yl_code.h
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_CODE_H
#define YL_CODE_H


#include <string>
#include "yl_heapobj.h"
#include "yl_string.h"


/*
    yl script bytecodes.  The yl bytecode interpreter owes a large debt to Lua.
*/

enum yl_opcode
{

    /*
        Basics.
    */
    
    YL_NOP,          // do nothing
    YL_MOV,          // r = a
    YL_SWP,          // swap r and a


    /*
        Constants.
    */
    
    YL_NULL,         // r = null
    YL_BOOL,         // r = a ? true : false
    YL_NUMBER,       // r = numbers[ c ]
    YL_STRING,       // r = strings[ c ]


    /*
        Globals
    */
    
    YL_GLOBAL,       // r = global with key b, where b is a key index
    YL_SETGLOBAL,    // global with key b, where b is a key index = r


    /*
        Arithmetic and logical instructions.
    */

    YL_NEG,          // r = -a
    YL_BITNOT,       // r = ~a

    YL_MUL,          // r = a * b
    YL_DIV,          // r = a / b
    YL_MOD,          // r = a % b
    YL_INTDIV,       // r = a ~ b
    YL_ADD,          // r = a + b
    YL_SUB,          // r = a - b
    YL_LSL,          // r = a << b
    YL_LSR,          // r = a >> b
    YL_ASR,          // r = a ~>> b
    YL_BITAND,       // r = a & b
    YL_BITXOR,       // r = a ^ b
    YL_BITOR,        // r = a | b
    YL_CONCAT,       // r = a .. b
    
    YL_EQ,           // r = ( a == b )
    YL_NE,           // r = ( a != b )
    YL_LT,           // r = ( a < b )
    YL_GT,           // r = ( a > b )
    YL_LE,           // r = ( a <= b )
    YL_GE,           // r = ( a >= b )
    
    YL_LNOT,         // r = not a
    YL_LXOR,         // r = a xor b


    /*
        Tests and branches.
    */
    
    YL_JMP,          // jump
    YL_JMPT,         // jump if r is true
    YL_JMPF,         // jump if r is false
    
    
    /*
        Calls.
    */
    
    YL_VARARG,       // r:b = ...
    YL_FUNCTION,     // r = close over functions[ c ], upval instructions follow
    YL_CALL,         // r:b = call r:a
    YL_YCALL,        // r:b = yield call r:a
    YL_YIELD,        // r:b = yield r:a
    YL_RETURN,       // return r:a, closes all upvals and iterators
    
    
    /*
        Iterators.
    */
    
    YL_ITER,         // iterators[ r ] = iterator over elements of a
    YL_ITERKEY,      // iterators[ r ] = iterator over keys/values of a
    YL_NEXT1,        // r = next from iterators[ a ]
    YL_NEXT2,        // r, b = next from iterators[ a ]
    YL_NEXT,         // r:b = next from iterators[ a ]

    YL_JMPV,         // jump if iterators[ r ] has values
    YL_JMPN,         // jump if iterators[ r ] is done


    /*
        Upvals.
    */
    
    YL_GETUP,        // r = value of upvals[ a ]
    YL_SETUP,        // value of upvals[ a ] = r


    /*
        Close upvals and iterators.
    */
    
    YL_CLOSE,        // close all localups down to a, all iterators down to b


    /*
        Object model instructions.
    */
    
    YL_OBJECT,       // r = object with prototype a
    YL_ARRAY,        // r = new array, c is element count hint
    YL_TABLE,        // r = new table, c is element count hint
    
    YL_SUPER,        // r = superof( a )
    
    YL_KEY,          // r = a.b, where b is a key index
    YL_INKEY,        // r = a.[ b ]
    YL_INDEX,        // r = a[ b ]
    YL_SETKEY,       // r.b = a, where b is a key index
    YL_SETINKEY,     // r.[ a ] = b
    YL_SETINDEX,     // r[ a ] = b
    YL_DELKEY,       // delete a.b, where b is a key index
    YL_DELINKEY,     // delete a.[ b ]
    
    YL_IN,           // r = ( a in b )
    YL_IS,           // r = ( a is b )

    YL_APPEND,       // append r to (array)a
    YL_EXTEND,       // append value list r:a to (array)b
    YL_UNPACK,       // r:b = unpack values from (array)a
    
    
    /*
        Exception handling.
    */
    
    YL_THROW,        // throw r
    YL_EXCEPT,       // r = current exception
    YL_UNWIND,       // continue exception unwinding (if unwinding)


    /*
        Initializing closure upvals.  These are special instructions which
        are only present after a YL_CLOSURE opcode.
    */
    
    YL_UPLOCAL,      // closure.upvals[ r ] = locups[ a ] using register b
    YL_UPUPVAL,      // closure.upvals[ r ] = upvals[ a ]


};



/*
    There are three instruction packings:
    
        | opcode  |    r    |    a    |    b    |
        | opcode  |    r    |         c         |
        | opcode  |    r    |         j         |

    Where r and a always index registers, and b either indexes a register or
    is an unsigned index.  c is an unsigned index, while j is a signed jump
    offset from the next instruction.
    
    a and b can also be register counts.
    
    The MARK indicates that instead of a fixed number of values, either unpack
    all available values and set the stack mark, or use all values from the
    base up to the stack mark.

*/

class yl_opinst
{
public:

    static const unsigned MARK  = 0xFF;
    static const unsigned NOVAL = 0xFF;

    yl_opinst();
    yl_opinst( yl_opcode opcode, unsigned r, unsigned a, unsigned b );
    yl_opinst( yl_opcode opcode, unsigned r, unsigned c );
    yl_opinst( yl_opcode opcode, unsigned r, signed j );
    
    yl_opcode   opcode() const;
    unsigned    r() const;
    unsigned    a() const;
    unsigned    b() const;
    unsigned    c() const;
    signed      j() const;


private:

    uint32_t i;
    
};



/*
    An exception frame.
*/

struct yl_xframe
{
    unsigned    start;              // First protected instruction.
    unsigned    end;                // After last protected instruction.
    uint8_t     close_upvals;       // Close upvals during unwind.
    uint8_t     close_iterators;    // Close iterators during unwind.
    unsigned    handler;            // First instruction of handler.
};



/*
    Debug information linking a variable to a register.
*/

struct yl_debugvar
{
    static const unsigned UPVAL = 0x8000;

    yl_heapref< yl_string > name;   // Name.
    unsigned    r;                  // Register or upval index.
};

struct yl_debugspan
{
    unsigned    varindex;           // Variable index.
    unsigned    start;              // First instruction in span.
    unsigned    end;                // One past last instruction in span.
};



/*
    Code printing.
*/

void ylop_print( size_t index, yl_opinst* op );




/*

*/


inline yl_opcode yl_opinst::opcode() const
{
    return (yl_opcode)(uint8_t)i;
}

inline unsigned yl_opinst::r() const
{
    return (uint8_t)( i >> 8 );
}

inline unsigned yl_opinst::a() const
{
    return (uint8_t)( i >> 16 );
}

inline unsigned yl_opinst::b() const
{
    return (uint8_t)( i >> 24 );
}

inline unsigned yl_opinst::c() const
{
    return (uint16_t)( i >> 16 );
}

inline signed yl_opinst::j() const
{
    return (int16_t)( i >> 16 );
}





#endif
