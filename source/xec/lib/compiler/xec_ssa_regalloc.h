//
//  xec_ssa_regalloc.h
//
//  Created by Edmund Kapusniak on 23/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_REGALLOC_H
#define XEC_SSA_REGALLOC_H


#include <unordered_map>
#include <memory>
#include "xec_ssa.h"


/*
    Performs register allocation for a xec program in SSA form.  Attempts
    to allocate values in the same 'ɸ equivalence class' in order to reduce
    the number of move instructions in the generated code.
*/


class xec_ssa_dfo;
struct xec_ssa_fscan;


class xec_ssa_regalloc
{
public:

    explicit xec_ssa_regalloc( xec_ssa* root );
    
    void allocate( xec_ssa_func* func, xec_ssa_dfo* dfo );
    
    
private:

    enum event_kind { LIVE, DEAD, SLEEP, WAKE };

    struct event
    {
        event_kind      kind;
        xec_ssa_opref   at;
        xec_ssa_opref   value;
        int             prev;
    };


    xec_ssa_opref   equiv( xec_ssa_opref opref );

    void            build_phiequiv();
    void            attempt_equiv( xec_ssa_opref a, xec_ssa_opref b );
    bool            interfere( xec_ssa_opref a, xec_ssa_opref b );
    int             refcmp( xec_ssa_opref a, xec_ssa_opref b );
    int             spancmp( xec_ssa_opref a, xec_ssa_opref b );
    uint64_t        opindex( xec_ssa_opref opref );
    uint64_t        opuntil( xec_ssa_opref live );

    void            forward_scan();
    void            forward_slice( xec_ssa_fscan* scan, xec_ssa_slice* slice );
    
    void            reverse_scan();

    void            print_events();


    xec_ssa*        root;
    xec_ssa_func*   func;
    xec_ssa_dfo*    dfo;
    std::unordered_map< xec_ssa_opref, xec_ssa_opref > equivmap;
    std::deque< event > events;

};



#endif
