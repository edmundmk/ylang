//
//  ocode.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OCODE_H
#define OCODE_H


/*
    A simple instruction set designed to implement the xec language.  Owes
    a debt to the design of the Lua virtual machine (as does the language).
*/

enum ocode
{

    O_NOP,
    
    O_NULL,         // r = null
    O_VALUE,        // r = constants[ c ]
    
    O_MOV,          // r = a
    O_SWP,          // swap r and a

    O_POS,          // r = (number)a
    O_NEG,          // r = -a
    O_NOT,          // r = ~a
    
    O_MUL,          // r = a * b
    O_DIV,          // r = a / b
    O_MOD,          // r = a - floor( a / b ) * b
    O_INTDIV,       // r = floor( a / b )
    O_ADD,          // r = a + b
    O_SUB,          // r = a - b
    O_LSL,          // r = (uint32)a << (uint32)b
    O_LSR,          // r = (uint32)a >> (uint32)b (logical)
    O_ASR,          // r = (uint32)a >> (uint32)b (arithmetic)
    O_AND,          // r = (uint32)a & (uint32)b
    O_XOR,          // r = (uint32)a ^ (uint32)b
    O_OR,           // r = (uint32)a | (uint32)b
    O_CONCAT,       // r = (string) a + b

    O_TEST,         // r = (bool)a
    O_LNOT,         // r = not (bool)a
    O_LXOR,         // r = (bool)a xor (bool)b
    
    O_KEY,          // r = a lookup keys[ b ]
    O_INKEY,        // r = a lookup (string)b
    O_INDEX,        // r = a index b
    O_ELEM,         // r = a index (immediate)b
    O_GLOBAL,       // r = global keys[ b ]
    
    O_SETKEY,       // a lookup keys[ b ] = r
    O_SETINKEY,     // a lookup (string)b = r
    O_SETINDEX,     // a index b = r
    O_SETGLOBAL,    // global keys[ b ] = r
    
    O_DELKEY,       // a delete keys[ b ]
    O_DELINKEY,     // a delete (string)b
    
    O_NEWUP,        // upvals[ c ] = new upval, upvals[ c ] = r
    O_SETNU,        // upvals[ c ] = r
    O_REFNU,        // r = upvals[ c ]
    O_CLOSE,        // close upvals[ c .. c + r ]

    O_SETUP,        // closure upvals[ c ] = r
    O_REFUP,        // r = closure upvals[ c ]
    
    O_EQ,           // r = a == b
    O_LT,           // r = a < b
    O_LE,           // r = a <= b
    O_IN,           // r = a in b
    O_IS,           // r = a is b

    O_JMP,          // pc = address of next instruction + j
    O_IFTRUE,       // if (bool)r pc = address of next instruction + j
    O_IFFALSE,      // if not (bool)r pc = address of next instruction + j
    
    O_ITER,         // push iterator over indexes of a
    O_ITERKEY,      // push iterator over keys of a
    O_NEXT1,        // r = generate from top iterator, set/clear @iter
    O_NEXT2,        // r, a = generate from top iterator, set/clear @iter
    O_NEXT,         // r .. r + b = generate (iterator)a, set/clear @iter
    O_IFITER,       // if @iter pc = address of next instruction + j
    O_IFDONE,       // if not @iter pc = address of next instruction + j
    O_DONE,         // pop top iterator
    
    O_TABLE,        // r = new table (reserve c indexes)
    O_OBJECT,       // r = new object with proto a.

    O_ARRAY,        // r = new array (reserve c indexes)
    O_UNPACK,       // r .. set @mark = a[ (immediate)b .. end ]
    O_APPEND,       // append r to (array)a
    O_EXTEND,       // append r .. @mark to (array)a
    
    O_CLOSURE,      // r = function closure of functions[ c ]
    O_ENVNU,        // add upvals[ c ] to previous function closure
    O_ENVUP,        // add closure upvals[ c ] to previous function closure

    O_VARARG,       // r = varargs[ c ]
    O_VARALL,       // r .. set @top = varargs
    
    O_CALL,         // r .. r + b = call r .. r + a (with @mark)
    O_YCALL,        // r .. r + b = yield call r .. r + a (with @mark)
    O_YIELD,        // r .. r + b = yield r .. r + a (with @mark)
    O_NEW,          // r = new a .. a + b (with @mark)
    
    O_RETURN,       // return r .. r + a (with @mark)

};





/*
    There are three instruction packings:
    
        | opcode  |    r    |    a    |    b    |
        | opcode  |    r    |         c         |
        | opcode  |    r    |         j         |

    Where r and a always index registers, and b either indexes a register or
    is an unsigned index.  c is an unsigned index, while j is a signed jump
    offset from the next instruction.
    
    a and b can also be register counts, or the special value O_MARK.
*/

class oinstruction
{
public:

    oinstruction();
    oinstruction( ocode opcode, unsigned r, unsigned a, unsigned b );
    oinstruction( ocode opcode, unsigned r, unsigned c );
    oinstruction( ocode opcode, unsigned r, signed j );
    
    ocode       opcode();
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

static const unsigned O_MARK = 0xFF;


/*
    This value means 'no value', where operands are optional.
*/

static const unsigned O_NOVAL = 0xFF;




/*

*/


inline oinstruction::oinstruction()
    :   i( 0 )
{
}


inline oinstruction::oinstruction(
                ocode opcode, unsigned r, unsigned a, unsigned b )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint8_t)a << 16
            | (uint8_t)b << 24 )
{
}

inline oinstruction::oinstruction( ocode opcode, unsigned r, unsigned c )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (uint16_t)c << 16 )
{
}

inline oinstruction::oinstruction( ocode opcode, unsigned r, signed j )
    :   i( (uint8_t)opcode
            | (uint8_t)r << 8
            | (int16_t)j << 16 )
{
}


inline ocode oinstruction::opcode()
{
    return (ocode)(uint8_t)i;
}

inline unsigned oinstruction::r()
{
    return (uint8_t)( i >> 8 );
}

inline unsigned oinstruction::a()
{
    return (uint8_t)( i >> 16 );
}

inline unsigned oinstruction::b()
{
    return (uint8_t)( i >> 24 );
}

inline unsigned oinstruction::c()
{
    return (uint16_t)( i >> 16 );
}

inline signed oinstruction::j()
{
    return (int16_t)( i >> 16 );
}






#endif
