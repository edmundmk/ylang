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
    
    Treats ops that will compile to stack-instructions specially, as these
    instructions require their parameters to be in specific stack locations,
    and return results in the same locations.
*/


class xec_ssa_regalloc
{
public:

    explicit xec_ssa_regalloc( xec_ssa* root );
    
    void allocate( xec_ssa_func* func );
    
    
private:

    struct phiequiv
    {
        std::vector< xec_ssa_opref > values;
    };
    
    void build_phiequiv();
    void linear_scan();

    xec_ssa*        root;
    xec_ssa_func*   func;
    std::vector< std::unique_ptr< phiequiv > > equiv;
    std::unordered_map< xec_ssa_opref, phiequiv* > equivmap;

};



#endif
