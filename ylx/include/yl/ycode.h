//
//  ycode.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YL_YCODE_H
#define YL_YCODE_H

#include <stdint.h>

namespace yl
{

/*
    ycode bytecode.  The ylang abytecode interpreter owes a large debt to Lua.
*/

enum opcode
{
    // Move and swap.
    OP_NOP,          // do nothing
    OP_MOV,          // r = a
    OP_SWP,          // swap r and a

    // Constants.
    OP_NULL,         // r = null
    OP_BOOL,         // r = a ? true : false
    OP_NUMBER,       // r = numbers[ c ]
    OP_STRING,       // r = strings[ c ]

    // Globals.
    OP_GLOBAL,       // r = global with key b, where b is a key index
    OP_SETGLOBAL,    // global with key b, where b is a key index = r

    // Arithmetic and logical instructions.
    OP_NEG,          // r = -a
    OP_BITNOT,       // r = ~a
    OP_MUL,          // r = a * b
    OP_DIV,          // r = a / b
    OP_MOD,          // r = a % b
    OP_INTDIV,       // r = a ~ b
    OP_ADD,          // r = a + b
    OP_SUB,          // r = a - b
    OP_LSL,          // r = a << b
    OP_LSR,          // r = a >> b
    OP_ASR,          // r = a ~>> b
    OP_BITAND,       // r = a & b
    OP_BITXOR,       // r = a ^ b
    OP_BITOR,        // r = a | b
    OP_CONCAT,       // r = a .. b
    OP_EQ,           // r = ( a == b )
    OP_NE,           // r = ( a != b )
    OP_LT,           // r = ( a < b )
    OP_GT,           // r = ( a > b )
    OP_LE,           // r = ( a <= b )
    OP_GE,           // r = ( a >= b )
    OP_LNOT,         // r = not a
    OP_LXOR,         // r = a xor b

    // Tests and branches.
    OP_JMP,          // jump
    OP_JMPT,         // jump if r is true
    OP_JMPF,         // jump if r is false

    // Calls.
    OP_VARARG,       // r:b = ...
    OP_FUNCTION,     // r = close over functions[ c ], upval instructions follow
    OP_CALL,         // r:b = call r:a
    OP_YCALL,        // r:b = yield call r:a
    OP_YIELD,        // r:b = yield r:a
    OP_RETURN,       // return r:a, closes all upvals and iterators

    // Iterators.
    OP_ITER,         // iterators[ r ] = iterator over elements of a
    OP_ITERKEY,      // iterators[ r ] = iterator over keys/values of a
    OP_NEXT1,        // r = next from iterators[ a ]
    OP_NEXT2,        // r, b = next from iterators[ a ]
    OP_NEXT,         // r:b = next from iterators[ a ]
    OP_JMPV,         // jump if iterators[ r ] has values
    OP_JMPN,         // jump if iterators[ r ] is done

    // Upvals.
    OP_GETUP,        // r = value of upvals[ a ]
    OP_SETUP,        // value of upvals[ a ] = r

    // Close upvals and iterators.
    OP_CLOSE,        // close all localups down to a, all iterators down to b

    // Object model instructions.
    OP_OBJECT,       // r = object with prototype a
    OP_ARRAY,        // r = new array, c is element count hint
    OP_TABLE,        // r = new table, c is element count hint
    OP_SUPER,        // r = superof( a )
    OP_KEY,          // r = a.b, where b is a key index
    OP_INKEY,        // r = a.[ b ]
    OP_INDEX,        // r = a[ b ]
    OP_SETKEY,       // r.b = a, where b is a key index
    OP_SETINKEY,     // r.[ a ] = b
    OP_SETINDEX,     // r[ a ] = b
    OP_RESPONDS,     // r = a.&b
    OP_DELKEY,       // delete a.b, where b is a key index
    OP_DELINKEY,     // delete a.[ b ]
    OP_IS,           // r = ( a is b )
    OP_APPEND,       // append r to (array)a
    OP_EXTEND,       // append value list r:a to (array)b
    OP_UNPACK,       // r:b = unpack values from (array)a

    // Exception handling.
    OP_THROW,        // throw r
    OP_EXCEPT,       // r = current exception
    OP_HANDLE,       // exception filter succeeded, stop unwinding
    OP_UNWIND,       // continue exception unwinding (if unwinding)

    // Initialization of upvals in closure.  These are special instructions
    // which are only present after a OP_CLOSURE opcode.
    OP_UPLOCAL,      // closure.upvals[ r ] = locups[ a ] using register b
    OP_UPUPVAL,      // closure.upvals[ r ] = upvals[ a ]

    // After all key instructions there should be an inline cache instruction
    // naming an ilcache slot.
    OP_ILCACHE,     // use ilcache[ c ]
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

const uint8_t MARK = 0xFF;
const uint8_t NONE = 0xFF;

struct op
{
    op();
    op( opcode opcode, uint8_t r, uint8_t a, uint8_t b );
    op( opcode opcode, uint8_t r, uint16_t c );
    op( opcode opcode, uint8_t r, int16_t j );

    uint8_t opcode;
    uint8_t r;
    union
    {
        struct
        {
            uint8_t a;
            uint8_t b;
        };
        uint16_t c;
        int16_t j;
    };
};

/*
    An exception frame.
*/

struct xframe
{
    uint32_t lower;                 // First protected instruction.
    uint32_t upper;                 // After last protected instruction.
    uint32_t handler;               // First instruction of handler.
    op unwind_close;                // Must be YL_CLOSE, closes during unwind.
};

/*
    A compiled module starts with a code_module structure.
*/

const uint32_t CODE_MODULE_MAGIC = 0x5B2A2A5D; // '[**]'

struct code_module
{
    uint32_t magic;                 // Must be CODE_MODULE_MAGIC
    uint32_t total_size;            // Total size of the entire compiled module
    uint32_t file_name;             // Filename, index into debug string heap
    uint32_t program_count;         // Count of function programs
    uint32_t number_count;          // Count of constant numbers
    uint32_t string_count;          // Count of constant strings
    uint32_t debug_heap_size;       // Size of debug string heap
    uint32_t debug_line_size;       // Count of line entries
/*
    code_program programs[ program_count ];
    double numbers[ number_count ];
    code_string strings[ string_count ];
    char debug_heap[ debug_heap_size ]; // null-terminated strings
    uint32_t debug_lines[ debug_line_size ]; // one entry per source line
*/
};

/*
    Followed by a set of programs, one per function.
*/

struct code_program
{
    uint32_t total_size;            // Total size of this program
    uint32_t instruction_count;     // Instruction count
    uint32_t xframe_count;          // xframe count
    uint16_t number_count;          // Size of number table
    uint16_t string_count;          // Size of string table
    uint16_t function_count;        // Size of function table
    uint16_t ilcache_count;         // Size of ilcache table
    uint8_t upval_count;            // Number of upvals used by closure
    uint8_t param_count;            // Number of non-vararg parameters
    uint8_t stack_count;            // Maximum stack space used by function
    uint8_t uploc_count;            // Maximum uploc space used by function
    uint8_t iters_count;            // Maximum iters space used by function
    uint8_t is_varargs;             // True if function is varargs
    uint8_t is_generator;           // True if function is generator
    uint8_t has_debug_info;         // True if debug info is present
/*
    op instructions[ instruction_count ];
    xframe xframes[ xframe_count ];
    uint32_t numbers[ number_count ];
    uint32_t strings[ string_count ];
*/
};

/*
    Each program optionally contains debug info, directly following.
*/

struct code_debug_program
{
    uint32_t function_name;         // Function name, index into debug string heap
    uint32_t debug_variable_count;  // Number of entries in debug variable table
    uint32_t debug_var_span_count;  // Number of entries in debug variable span table
/*
    uint32_t source_locations[ instruction_count ];
    code_debug_variable variables[ debug_variable_count ];
    code_debug_var_span var_spans[ debug_var_span_count ];
*/
};

struct code_debug_variable
{
    uint32_t variable_name : 24;    // Offset of variable name in debug heap
    uint32_t r : 8;                 // Register or upval index for this variable
};

struct code_debug_var_span
{
    uint32_t variable_index;        // Index into variables table
    uint32_t lower;                 // First instruction of span
    uint32_t upper;                 // After last instruction of span
};

/*
    Literal strings.
*/

struct code_strings
{
    uint32_t length;
    char s[];
};

}

#endif

