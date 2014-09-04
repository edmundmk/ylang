//
//  xec_live_target.cpp
//
//  Created by Edmund Kapusniak on 04/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_live_target.h"




xec_live_opformat::xec_live_opformat( int precedence, const char* format )
    :   precedence( precedence )
    ,   format( format )
{
}



xec_live_target::xec_live_target()
{
}

xec_live_target::~xec_live_target()
{
}



void xec_live_target::expression( xec_seq* seq )
{
    /*
        Translating out of seq-form produces a sequence of calls, assignments,
        and declarations, which capture the semantics of the original xec
        expression.  To perform this translation, conduct a linear scan of the
        sequence ops, doing a simple liveness analysis/alias analysis:
        
          - Any op with a single ref is live, and dead when it is used.
     
          - Any op with more than one ref must be realized and assigned to
            a temporary, to avoid evaluating it more than once.
            
          - Any op live across a call or assign (but not declarations, which
            cannot clobber any values) must be preserved in a temporary.
              
          - When an assignment is reached, all calls before it must be
            realised, before realising the assignment.  While scanning we keep
            a list of unrealised calls.  Calls which are arguments to other
            calls are removed from the list as they will be realised along
            with the outer call.
            
          - For targets with multiple assignment, runs of assignments can
            be treated as a single op - which may mean fewer temporaries as
            only ops live across the entire run must be preserved.
            
          - Control flow _may_ be collapsible into the target's shortcut
            operators, but notice that a && b returns a if a is falsy - the
            target must have both the same shortcut semantics and the same
            definition of truthiness.
     
        You can see why compiler people like functional languages which have no
        assignments and no side-effects!
    */
}




    
void xec_live_target::opformat(
                xec_seq_opcode opcode, int precedence, const char* format )
{
    opformats.emplace( opcode, xec_live_opformat( precedence, format ) );
}


