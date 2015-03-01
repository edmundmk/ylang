//
//  yl_code.h
//
//  Created by Edmund Kapusniak on 07/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_CODE_H
#define YL_CODE_H



/*
    yl script bytecodes.  The yl bytecode interpreter owes a large debt to Lua.
*/

enum yl_opcode
{

    /*
        Basics.
    */
    
    Y_NOP,          // do nothing
    Y_MOV,          // r = a
    Y_SWP,          // swap r and a


    /*
        Constants.
    */
    
    Y_NULL,         // r:a = null
    Y_BOOL,         // r = a ? true : false
    Y_NUMBER,       // r = numbers[ c ]
    Y_STRING,       // r = strings[ c ]


    /*
        Globals
    */
    
    Y_GLOBAL,       // r = global with key b, where b is a key index
    Y_SETGLOBAL,    // global with key b, where b is a key index = r


    /*
        Arithmetic and logical instructions.
    */

    Y_NEG,          // r = -a
    Y_BITNOT,       // r = ~a

    Y_MUL,          // r = a * b
    Y_DIV,          // r = a / b
    Y_MOD,          // r = a % b
    Y_INTDIV,       // r = a ~ b
    Y_ADD,          // r = a + b
    Y_SUB,          // r = a - b
    Y_LSL,          // r = a << b
    Y_LSR,          // r = a >> b
    Y_ASR,          // r = a ~>> b
    Y_BITAND,       // r = a & b
    Y_BITXOR,       // r = a ^ b
    Y_BITOR,        // r = a | b
    Y_CONCAT,       // r = a .. b
    
    Y_EQ,           // r = ( a == b )
    Y_NE,           // r = ( a != b )
    Y_LT,           // r = ( a < b )
    Y_GT,           // r = ( a > b )
    Y_LE,           // r = ( a <= b )
    Y_GE,           // r = ( a >= b )
    
    Y_LNOT,         // r = not a
    Y_LXOR,         // r = a xor b


    /*
        Tests and branches.
    */
    
    Y_JMP,          // jump
    Y_JMPT,         // jump if r is true
    Y_JMPF,         // jump if r is false
    
    
    /*
        Calls.
    */
    
    Y_VARARG,       // r:a = ...
    Y_CLOSURE,      // r = close over functions[ c ], upval instructions follow
    Y_CALL,         // r:b = call r:a
    Y_YCALL,        // r:b = yield call r:a
    Y_YIELD,        // r:b = yield r:a
    Y_RETURN,       // return r:a, closes all upvals and iterators
    
    
    /*
        Iterators.
    */
    
    Y_ITER,         // iterators[ r ] = iterator over elements of a
    Y_ITERKEY,      // iterators[ r ] = iterator over keys/values of a
    Y_JMPITER,      // jump if iterators[ r ] is done
    Y_NEXT1,        // r = next from iterators[ b ]
    Y_NEXT2,        // r, a = next from iterators[ b ]
    Y_NEXT,         // r:a = next from iterators[ b ]


    /*
        Upvals.
    */
    
    Y_GETUP,        // r = value of upvals[ a ]
    Y_SETUP,        // value of upvals[ a ] = r


    /*
        Close upvals and iterators.
    */
    
    Y_CLOSE,        // close all upvals up to a, all iterators up to b


    /*
        Object model instructions.
    */
    
    Y_OBJECT,       // r = object with prototype a
    Y_ARRAY,        // r = new array, c is element count hint
    Y_TABLE,        // r = new table, c is element count hint
    
    Y_SUPER,        // r = superof( a )
    
    Y_KEY,          // r = a.b, where b is a key index
    Y_METHOD,       // r = a.b (similarly), next register after r = a
    Y_INKEY,        // r = a.[ b ]
    Y_INDEX,        // r = a[ b ]
    Y_SETKEY,       // r.b = a,
    Y_SETINKEY,     // r.[ a ] = b
    Y_SETINDEX,     // r[ a ] = b
    Y_DELKEY,       // delete a.b, where b is a key index
    Y_DELINKEY,     // delete a.[ b ]
    
    Y_IN,           // r = ( a in b )
    Y_IS,           // r = ( a is b )

    Y_APPEND,       // append a to (array)r
    Y_EXTEND,       // append value list a:b to (array)r
    Y_UNPACK,       // a:b = unpack values from (array)r
    
    
    /*
        Exception handling.
    */
    
    Y_THROW,        // throw r
    Y_EXCEPT,       // r = current exception
    Y_UNWIND,       // continue exception unwinding (if unwinding)


    /*
        Initializing closure upvals.  These are special instructions which
        are only present after a Y_CLOSURE opcode.
    */
    
    Y_UPLOCAL,      // closure.upvals[ r ] = upvals[ a ] (using register b)
    Y_UPUPVAL,      // closure.upvals[ r ] = upvals[ a ]


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

    y_opinst();
    y_opinst( y_opcode opcode, unsigned r, unsigned a, unsigned b );
    y_opinst( y_opcode opcode, unsigned r, unsigned c );
    y_opinst( y_opcode opcode, unsigned r, signed j );
    
    y_opcode    opcode();
    unsigned    r();
    unsigned    a();
    unsigned    b();
    unsigned    c();
    signed      j();


private:

    uint32_t i;
    
};



/*
    An exception frame.
*/

struct yl_xframe
{
    int start;              // First protected instruction.
    int end;                // After last protected instruction.
    int close_upvals;       // Close upvals during unwind.
    int close_iterators;    // Close iterators during unwind.
    int handler;            // First instruction of handler.
};



/*
    Debug information linking a variable to a register.
*/

enum yl_diname_kind
{
    Y_DINAME_UPVAL,
    Y_DINAME_LOCAL,
    Y_DINAME_CLOSE,
};

struct yl_diname
{
    int             instruction;
    y_diname_kind   kind;
    unsigned        r;
    std::string     name;
};




#endif
