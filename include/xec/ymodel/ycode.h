//
//  ycode.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YCODE_H
#define YCODE_H


/*
    A simple instruction set designed to implement the xec language.  Owes
    a debt to the design of the Lua virtual machine (as does the language).
*/

enum ycode
{

    Y_NOP,
    
    Y_NULL,         // r = null
    Y_VALUE,        // r = constants[ c ]
    
    Y_MOV,          // r = a
    Y_SWP,          // swap r and a

    Y_POS,          // r = (number)a
    Y_NEG,          // r = -a
    Y_NOT,          // r = ~a
    
    Y_MUL,          // r = a * b
    Y_DIV,          // r = a / b
    Y_MOD,          // r = a - floor( a / b ) * b
    Y_INTDIV,       // r = floor( a / b )
    Y_ADD,          // r = a + b
    Y_SUB,          // r = a - b
    Y_LSL,          // r = (uint32)a << (uint32)b
    Y_LSR,          // r = (uint32)a >> (uint32)b (logical)
    Y_ASR,          // r = (uint32)a >> (uint32)b (arithmetic)
    Y_AND,          // r = (uint32)a & (uint32)b
    Y_XOR,          // r = (uint32)a ^ (uint32)b
    Y_OR,           // r = (uint32)a | (uint32)b
    Y_CONCAT,       // r = (string) a + b

    Y_EQ,           // r = a == b
    Y_LT,           // r = a < b
    Y_LE,           // r = a <= b
    Y_IN,           // r = a in b
    Y_IS,           // r = a is b

    Y_TEST,         // r = (bool)a
    Y_LNOT,         // r = not (bool)a
    Y_LXOR,         // r = (bool)a xor (bool)b
    
    Y_KEY,          // r = a lookup keys[ b ]
    Y_INKEY,        // r = a lookup (string)b
    Y_INDEX,        // r = a index b
    Y_ELEM,         // r = a index (immediate)b
    Y_GLOBAL,       // r = global keys[ b ]
    
    Y_SETKEY,       // a lookup keys[ b ] = r
    Y_SETINKEY,     // a lookup (string)b = r
    Y_SETINDEX,     // a index b = r
    Y_SETGLOBAL,    // global keys[ b ] = r
    
    Y_DELKEY,       // a delete keys[ b ]
    Y_DELINKEY,     // a delete (string)b
    
    Y_NEWNU,        // upvals[ c ] = new upval, upvals[ c ] = r
    Y_SETNU,        // upvals[ c ] = r
    Y_REFNU,        // r = upvals[ c ]
    Y_CLONU,        // close upvals[ c .. c + r ]

    Y_SETUP,        // closure upvals[ c ] = r
    Y_REFUP,        // r = closure upvals[ c ]
    
    Y_JMP,          // pc = address of next instruction + j
    Y_IFTRUE,       // if (bool)r pc = address of next instruction + j
    Y_IFFALSE,      // if not (bool)r pc = address of next instruction + j
    
    Y_ITER,         // push iterator over indexes of a
    Y_ITERKEY,      // push iterator over keys of a
    Y_NEXT1,        // r = generate from top iterator, set/clear @iter
    Y_NEXT2,        // r, a = generate from top iterator, set/clear @iter
    Y_NEXT,         // r .. r + b = generate from top iterator, set/clear @iter
    Y_IFITER,       // if @iter pc = address of next instruction + j
    Y_IFNITER,      // if not @iter pc = address of next instruction + j
    Y_POPITER,      // pop c iterators
    
    Y_TABLE,        // r = new table (reserve c indexes)
    Y_OBJECT,       // r = new object with proto a.

    Y_ARRAY,        // r = new array (reserve c indexes)
    Y_UNPACK,       // r .. set @mark = a[ (immediate)b .. end ]
    Y_APPEND,       // append r to (array)a
    Y_EXTEND,       // append r .. @mark to (array)a
    
    Y_CLOSURE,      // r = function closure of functions[ c ]
    Y_ENVNU,        // add upvals[ c ] to previous function closure
    Y_ENVUP,        // add closure upvals[ c ] to previous function closure

    Y_VARARG,       // r = varargs[ c ] (numbered from one)
    Y_VARALL,       // r .. set @top = varargs
    
    Y_CALL,         // r .. r + b = call r .. r + a (with @mark)
    Y_YCALL,        // r .. r + b = yield call r .. r + a (with @mark)
    Y_YIELD,        // r .. r + b = yield r .. r + a (with @mark)
    
    Y_RETURN,       // return r .. r + a (with @mark)

};





/*
    There are three instruction packings:
    
        | opcode  |    r    |    a    |    b    |
        | opcode  |    r    |         c         |
        | opcode  |    r    |         j         |

    Where r and a always index registers, and b either indexes a register or
    is an unsigned index.  c is an unsigned index, while j is a signed jump
    offset from the next instruction.
    
    a and b can also be register counts, or the special value Y_MARK.
*/

class yinstruction
{
public:

    yinstruction();
    yinstruction( ycode opcode, unsigned r, unsigned a, unsigned b );
    yinstruction( ycode opcode, unsigned r, unsigned c );
    yinstruction( ycode opcode, unsigned r, signed j );
    
    ycode       opcode();
    unsigned    r();
    unsigned    a();
    unsigned    b();
    unsigned    c();
    signed      j();


private:

    uint32_t i;
    
};



/*
    This marker indicates that instead of a fixed number of values, either
    unpack all available values and set the @mark, or use all values from
    the base up to the @mark.
*/

static const unsigned Y_MARK = 0xFF;


/*
    This value means 'no value', where operands are optional.
*/

static const unsigned Y_NOVAL = 0xFF;




/*

*/


inline yinstruction::yinstruction()
    :   i( 0 )
{
}


inline yinstruction::yinstruction(
                ycode opcode, unsigned r, unsigned a, unsigned b )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint8_t)a << 16
            | (uint8_t)b << 24 )
{
}

inline yinstruction::yinstruction( ycode opcode, unsigned r, unsigned c )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint16_t)c << 16 )
{
}

inline yinstruction::yinstruction( ycode opcode, unsigned r, signed j )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (int16_t)j << 16 )
{
}


inline ycode yinstruction::opcode()
{
    return (ycode)(uint8_t)i;
}

inline unsigned yinstruction::r()
{
    return (uint8_t)( i >> 8 );
}

inline unsigned yinstruction::a()
{
    return (uint8_t)( i >> 16 );
}

inline unsigned yinstruction::b()
{
    return (uint8_t)( i >> 24 );
}

inline unsigned yinstruction::c()
{
    return (uint16_t)( i >> 16 );
}

inline signed yinstruction::j()
{
    return (int16_t)( i >> 16 );
}






#endif
