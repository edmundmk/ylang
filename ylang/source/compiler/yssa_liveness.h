//
//  yssa_liveness.h
//
//  Created by Edmund Kapusniak on 29/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_LIVENESS_H
#define YSSA_LIVENESS_H



struct yssa_module;
struct yssa_function;


void yssa_liveness( yssa_module* module, yssa_function* function );



#endif
