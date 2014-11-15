//
//  yssa_regalloc.h
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_REGALLOC_H
#define YSSA_REGALLOC_H


struct yssa_linear;


/*
    Perform register allocation on linearized SSA.
*/

void yssa_regalloc( yssa_linear* l );



#endif
