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
    
    
    So this is yet another version of the compiler, which builds an SSA-like
    form which forces all definitions of the same source variable into the
    same register.  Each definition remembers which source variable it was
    constructed from.  With this form:

     -  At each instruction, there is only one live definition of each
            variable.  Values of variables clobbered by a new definition
            before the use are preserved using a load (a MOV op).  This
            load can be register-allocated like any other temporary value.
     -  Each function call both implicitly uses and clobbers all live upvals,
            keeping them alive and effectively redefining them.  Any value
            of an upval the use of which spans a function call must also be
            preserved with a load.
     -  Any variable that is referenced by an exception handler is live
            throughout the entire protected scope.  No definition of such
            a variable can be eliminated.
     -  phi-functions are present to allow use-def analysis, but must not
            merge definitions with overlapping live ranges, and consequently
            do not cause any moves in the final translation to VM code.

    During construction, source variables do not share definitions.  Trivial
    assignments such as y = x, which in a true SSA form would end up as a
    no-op, produce a new definition for y using a MOV.  Such loads could
    only be eliminated if we can prove that (this definition of) y is not
    live during any later definition of x.

*/


#include <stdint.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <region.h>
#include "yl_diagnostics.h"
#include "yl_code.h"


struct yssa_module;
struct yssa_opinst;
struct yssa_string;
struct yssa_variable;
struct yssa_function;
struct yssa_block;
struct yssa_loop;
struct yssa_live_range;


typedef std::unique_ptr< yssa_module >      yssa_module_p;
typedef std::unique_ptr< yssa_function >    yssa_function_p;
typedef std::unique_ptr< yssa_block >       yssa_block_p;
typedef std::unique_ptr< yssa_loop >        yssa_loop_p;



/*
    A source file is compiled to a module.
*/

struct yssa_module
{
    region                          alloc;
    std::vector< yssa_function_p >  functions;
};





/*
    SSA instructions are identical to ycode instructions, except most operands
    are pointers to other instructions, and there are additional instructions
    to support SSA features.
*/


enum yssa_opcode
{
    YSSA_PARAM      = 0x80,     // Select parameter (numbered from 0).
    YSSA_SELECT     = 0x81,     // Select a single result from an op.
    YSSA_IMPLICIT   = 0x82,     // Implicit reference to variable.
    YSSA_ITEREACH   = 0x83,     // Returns true if iterator b has values.
    YSSA_PHI        = 0x84,     // SSA phi-function.
    YSSA_REF        = 0x85,     // Used for incomplete phis and references.
    YSSA_VAR        = 0x86,     // Variable values on entry to catch block.
};


struct yssa_opinst
{
    static const uint8_t MULTIVAL = 0xFF;
    
    yssa_opinst( int sloc, uint8_t opcode,
                    uint8_t operand_count, uint8_t result_count );

    bool has_associated() const
    {
        return opcode == YL_UPLOCAL
            || opcode == YL_UPUPVAL
            || opcode == YSSA_IMPLICIT;
    }
    
    bool has_multival() const
    {
        return opcode == YL_RETURN
            || opcode == YL_EXTEND
            || opcode == YL_CALL
            || opcode == YL_YCALL
            || opcode == YL_YIELD;
    }
    
    bool is_call() const
    {
        return opcode == YL_RETURN
            || opcode == YL_EXTEND
            || opcode == YL_CALL
            || opcode == YL_YCALL
            || opcode == YL_YIELD;
    }
    

    int                 sloc : 24;
    uint8_t             stacktop;

    uint8_t             opcode;
    uint8_t             operand_count;  // Length of operand array.
    uint8_t             result_count;   // Number of results, or MULTIVAL.
    uint8_t             r;
    
    yssa_live_range*    live;           // Live range of op.
    
    union
    {
        yssa_variable*  variable;       // Variable this op defines.
        yssa_opinst*    associated;     // Op this op is associated with.
    };
    
    union
    {
        double          number;         // Constant number.
        bool            boolean;        // Constant bool.
        yssa_string*    string;         // Constant string.
        yssa_function*  function;       // Function to instantiate.
        const char*     key;            // Key for lookups.
        int             select;         // Which parameter or result to select.
        struct
        {
            uint8_t     a;              // ycode a operand.
            uint8_t     b;              // ycode b operand.
        };
        uint16_t        c;              // ycode c operand.
        yssa_opinst*    multival;       // Multival operand (after normal ones).
    };
    
    yssa_opinst*        operand[];      // Operand list.

};


struct yssa_string
{
    yssa_string( const char* string, size_t length );

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
    yssa_variable( const char* name, int sloc, bool xcref, uint8_t localup );

    const char*         name;
    int                 sloc;
    bool                xcref;      // Referenced from an exception handler?
    uint8_t             localup;    // Localup index.
    uint8_t             r;
    yssa_live_range*    live;
    yssa_opinst*        argof;
};



/*
    A function in SSA form.
*/

struct yssa_function
{
    yssa_function( int sloc, const char* funcname );

    const char*                 funcname;
    int                         sloc;
    std::vector< yssa_loop_p >  loops;
    std::vector< yssa_block_p > blocks;
    std::vector< yssa_opinst* > ops;
    std::unordered_map< yssa_opinst*, yssa_opinst* > argof;
};



/*
    A block in the control flow graph.
*/


enum yssa_block_flags
{
    YSSA_LOOP           = 0x0001,
    YSSA_XCHANDLER      = 0x0002,
    YSSA_UNSEALED       = 0x0004,
    YSSA_LOOKUP         = 0x0008,
    YSSA_PREFER_FAIL    = 0x0010,
    YSSA_LINEARIZED     = 0x8000,
};


struct yssa_block
{
    explicit yssa_block( unsigned flags = 0 );

    uint16_t                    flags;
    uint8_t                     xclocalups;   // For xchandlers, close.
    uint8_t                     xcitercount;  // Similarly.
    int                         index;        // Index in the DFO.
    yssa_loop*                  loop;         // Loop containing this block.
    std::vector< yssa_block* >  prev;         // Previous blocks in CFG.
    std::vector< yssa_opinst* > phi;          // phi-functions at head of block.
    std::vector< yssa_opinst* > ops;          // Op list
    size_t                      lstart;       // Index into function op list.
    size_t                      lfiphi;       // One past the last phi op.
    size_t                      lfinal;       // One past the end in the list.
    yssa_opinst*                test;         // Condition to test (if any).
    yssa_block*                 next;         // Next block (if condition passes).
    yssa_block*                 fail;         // Next block if condition fails.
    yssa_block*                 xchandler;    // Exception handler block.
    std::unordered_map< yssa_variable*, yssa_opinst* > definitions;
    std::unordered_set< yssa_opinst* > livein;
};



/*
    Loop structure of control flow graph.
*/

struct yssa_loop
{
    yssa_loop( yssa_loop* parent, yssa_block* header );

    yssa_loop*                          parent;
    std::unordered_set< yssa_loop* >    children;
    yssa_block*                         header;
    std::unordered_set< yssa_block* >   blocks;
};



/*
    Live range (in linearized op list) of an SSA op.
*/

struct yssa_live_range
{
    yssa_live_range( size_t start, size_t final, yssa_live_range* next );

    size_t              start;   // Index where this range starts.
    size_t              final;   // Index of the op where this live range dies.
    yssa_live_range*    next;
};




/*
    Linearize an SSA function by removing ops from blocks and putting them
    in order into the function directly.
*/

void yssa_linearize( yssa_function* function );




/*
    SSA printing.
*/

void yssa_print( yssa_module* module );



#endif
