//
//  xec_opt_constfold.h
//
//  Created by Edmund Kapusniak on 20/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_OPT_CONSTFOLD_H
#define XEC_OPT_CONSTFOLD_H


struct xec_ssa;
struct xec_ssa_func;
struct xec_ssa_opref;
struct xec_ssa_op;
struct xec_ssa_string;
class xec_ssa_dfo;


class xec_opt_constfold
{
public:

    explicit xec_opt_constfold( xec_ssa* root );
    void optimize( xec_ssa_func* func, xec_ssa_dfo* dfo );

private:

    xec_ssa_op* operand( xec_ssa_opref opref );
    bool number( xec_ssa_opref opref, double* number );
    bool string( xec_ssa_opref opref, xec_ssa_string** string );

    void optimize_op( xec_ssa_op& op );

    xec_ssa* root;
    xec_ssa_func* func;
    xec_ssa_dfo* dfo;
    
};




#endif
