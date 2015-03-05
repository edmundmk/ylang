//
//  yssa.h
//
//  Created by Edmund Kapusniak on 04/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_H
#define YSSA_H


/*
    Compiling for a register machine is difficult.  In any language where
    variables can be redefined during expressions (e.g. increment/decrement,
    assignment subexpressions, or function closures assigning to upvals),
    you cannot rely on a variable having the same value at the point of use
    as it did when it was 'loaded' by being mentioned in the expression.
    
    e.g. in { i = 4; i = i + ( i = 5 ); }, i should have the value 9, even
    though at the add instruction it will already be 5.
 
    Load each variable onto a 'stack' when it is referenced (allowing the
    original to be clobbered without affecting the copy), and you may as well
    use a stack machine.  So you have to do at least some kind of analysis.
 
 
    Acting like a compiler for a real machine (translate to SSA-form, perform
    register allocation for each definition of each variable, merge using
    phi-functions at block entry), is attractive but introduces complications
    that are overkill for a VM:
 
     -  Lua-like 'open upvals reference stack slots' implementation of
            function closures is impossible if a local used as an upval
            might be assigned to different stack slots at different points.
     -  Exception handlers are run rarely, but must either generate a
            control flow edge at each potentially-exception-throwing
            operation (interfering with optimization of the normal path),
            or enough information to build a complex 'phi forest' - moving
            variables into predictable registers from source registers
            depending on the location from which the exception was thrown.
     -  Register allocation is less predictable, meaning that for precise
            GC scanning of stack roots ideally you would want a list of live
            stack roots at each safepoint.  Compare to a stack machine, where
            everything below the stack top is live.
     -  Debug information similarly will be much more complicated.
    
    
    So this is yet another version of the compiler, which builds an SSA form
    but which forces all definitions of the same source variable into the
    same register.  Each definition remembers which source variable it was
    constructed from.  With this form:

     -  Any definition which is live after another definition of the same
            variable means the original value will be clobbered.  A load must
            be inserted to preserve the original value of the variable
            (which can then be allocated normally, like other temporaries).
     -  Each function call clobbers all live upvals, effectively redefining
            them.  Any definition of an upval which spans a function call must
            also be preserved with a load.
     -  Any variable that is referenced by an exception handler is live
            throughout the entire protected scope.  No definition can be
            eliminated - there is an implicit use of each definition by the
            handler, which sits at the end of the protected scope.
     -  Similarly, but less drastically, there is an implicit use of the
            latest definition of each upval by each function call, preventing
            elimination of (most) upval definitions.
     -  phi-functions are present to allow use-def analysis, but do not
            cause any moves in the final translation to VM code.


*/


#include <stdint.h>
#include <region.h>
#include "yl_diagnostics.h"


struct yssa_module;
struct yssa_opinst;
struct yssa_string;
struct yssa_variable;
struct yssa_function;
struct yssa_block;



/*
    A source file is compiled to a module.
*/

struct yssa_module
{
    region          region;
    yl_diagnostics* diagnostics;
    
};





/*
    SSA instructions are identical to ycode instructions, except most operands
    are pointers to other instructions, and there are additional instructions
    to support SSA features.
*/


enum yssa_opcode
{
    YSSA_SELECT         = 0x80,     // Select a single result from an op.
    YSSA_IMPLICIT       = 0x81,     // Implicit reference to variable(s).
    YSSA_CLOBBER        = 0x82,     // Potential store, redefining a variable.
    YSSA_PHI            = 0x83,     // SSA phi-function.
    YSSA_LOAD           = 0x84,     // Make temporary to avoid clobbering.
};


struct yssa_opinst
{
    static const uint8_t MULTIVAL = 0xFF;

    int sloc;

    uint8_t opcode;
    uint8_t operand_count;          // Length of operand array.
    uint8_t result_count;           // Number of results, or MULTIVAL.
    uint8_t r;
    
    yssa_variable*      variable;   // Variable this op defines.
    
    union
    {
        double          number;     // Constant number.
        bool            boolean;    // Constant bool.
        yssa_string*    string;     // Constant string.
        yssa_function*  function;   // Function to instantiate.
        const char*     key;        // Key for lookups.
        struct
        {
            uint8_t     a;          // ycode a operand.
            uint8_t     b;          // ycode b operand.
        };
        uint16_t        c;          // ycode c operand.
        yssa_opinst*    multival;   // Multival operand (after normal ones).
        yssa_opinst*    associated; // Op this op is associated with.
    };
    
    yssa_opinst*        operand[];  // Operand list.

};


struct yssa_string
{
    const char*         string;
    size_t              length;
};




/*
    A variable from the source code (or a temporary that survives its
    block). All definitions that might be merged by a phi-function must
    reference the same variable.  Clobber analysis inserts loads to ensure
    that no definition of a variable interferes with any other, and the
    register allocator will allocate a variable to a single register.
*/

struct yssa_variable
{
    int                 sloc;
    const char*         name;
    bool                upval;  // Is this variable an upval?
    bool                xcept;  // Referenced from an exception handler?
};







#endif
