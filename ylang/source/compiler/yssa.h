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
    same register.  Each definition remembers which variable it was
    constructed from.  With this form:

     -  Any definition which is live after another definition of the same
            variable means the original value will be clobbered.  A load must
            be inserted to preserve the original value of the variable
            (which can then be allocated normally, like other temporaries).
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






#endif
