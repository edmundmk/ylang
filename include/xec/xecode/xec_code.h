//
//  xec_code.h
//
//  Created by Edmund Kapusniak on 21/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_CODE_H
#define XEC_CODE_H


#include <stdint.h>


class xec_value;
class xec_objkey;

class xec_opinst;
class xec_module;
class xec_function;


/*
    A simple instruction set designed to implement the xec language.  Owes
    a debt to the design of the Lua virtual machine (as does the language).
*/

enum xec_opcode
{
    XEC_NOP,
    
    XEC_NULL,       // r = null
    XEC_VALUE,      // r = constants[ c ]
    
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

    XEC_TEST,       // r = (bool)a
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
    XEC_SETNU,      // upvals[ c ] = r
    XEC_REFNU,      // r = upvals[ c ]
    XEC_CLOSE,      // close upvals[ c .. c + r ]

    XEC_SETUP,      // closure upvals[ c ] = r
    XEC_REFUP,      // r = closure upvals[ c ]
    
    XEC_EQ,         // r = a == b
    XEC_LT,         // r = a < b
    XEC_LE,         // r = a <= b
    XEC_IN,         // r = a in b
    XEC_IS,         // r = a is b

    XEC_JMP,        // pc = address of next instruction + j
    XEC_IFTRUE,     // if (bool)r pc = address of next instruction + j
    XEC_IFFALSE,    // if not (bool)r pc = address of next instruction + j
    
        // TODO RETHINK THIS A LOT.  ITERATORS SHOULD NOT BE GC OBJECTS.
    XEC_ITER,       // push iterator over indexes of a
    XEC_ITERKEY,    // push iterator over keys of a
    XEC_NEXT1,      // r = generate from top iterator, set/clear @iter
    XEC_NEXT2,      // r, a = generate from top iterator, set/clear @iter
    XEC_NEXT,       // r .. r + b = generate (iterator)a, set/clear @iter
    XEC_IFITER,     // if @iter pc = address of next instruction + j
    XEC_IFDONE,     // if not @iter pc = address of next instruction + j
    XEC_DONE,       // pop top iterator
    
    XEC_TABLE,      // r = new table (reserve c indexes)
    XEC_OBJECT,     // r = new object with proto a.

    XEC_ARRAY,      // r = new array (reserve c indexes)
    XEC_UNPACK,     // r .. set @mark = a[ (immediate)b .. end ]
    XEC_APPEND,     // append r to (array)a
    XEC_EXTEND,     // append r .. @mark to (array)a
    
    XEC_CLOSURE,    // r = function closure of functions[ c ]
    XEC_ENVNU,      // add upvals[ c ] to previous function closure
    XEC_ENVUP,      // add closure upvals[ c ] to previous function closure

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

class xec_opinst
{
public:

    xec_opinst( xec_opcode opcode, unsigned r, unsigned a, unsigned b );
    xec_opinst( xec_opcode opcode, unsigned r, unsigned c );
    xec_opinst( xec_opcode opcode, unsigned r, signed j );
    
    xec_opcode  opcode();
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

static const unsigned XEC_MARK = 0xFF;


/*
    This value means 'no value', where operands are optional.
*/

static const unsigned XEC_NOVAL = 0xFF;





/*
    A module is a compiled script file.
*/

class xec_module
{
public:

    ~xec_module();

    const char*     name();
    
    xec_function*   script_function();
    
    xec_objkey      key( unsigned k );
    xec_value       value( unsigned v );
    
    xec_function*   function( unsigned f );
    unsigned        function_count();


private:

    friend class xec_ssa_buildcode;

    xec_module();

    char*           mname;
    xec_objkey*     mkeys;
    uint32_t        mkeycount;
    xec_value*      mvalues;
    uint32_t        mvaluecount;
    xec_function**  mfuncs;
    uint32_t        mfunccount;

};



/*
    Code for a function.
*/

class xec_function
{
public:

    ~xec_function();

    xec_module*     module();
    const char*     name();
    
    xec_opinst      code( unsigned pc );
    unsigned        code_count();
    
    unsigned        param_count();
    unsigned        upval_count();
    unsigned        newup_count();
    unsigned        stack_count();
    bool            is_varargs();
    bool            is_coroutine();

    
private:

    friend class xec_ssa_buildcode;

    xec_function();

    xec_module*     mmodule;
    char*           mname;
    xec_opinst*     mcode;
    uint32_t        mcodecount;
    uint32_t        mparamcount;
    uint32_t        mupvalcount;
    uint32_t        mnewupcount;
    uint32_t        mstackcount;
    bool            mvarargs;
    bool            mcoroutine;

};



/*
    Print module for debugging.
*/

void xec_module_print( xec_module* module );












#endif
