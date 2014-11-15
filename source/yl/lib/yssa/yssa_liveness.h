//
//  yssa_liveness.h
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_LIVENESS_H
#define YSSA_LIVENESS_H


struct yssa_func;


/*
    Calculate inter-block liveness (the in set and the out set).  Values
    that are local to a particular block are easy to deal with.
*/


void yssa_liveness( yssa_func* func );





#endif
