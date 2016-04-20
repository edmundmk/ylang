//
//  yl_interp.h
//
//  Created by Edmund Kapusniak on 14/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_INTERP_H
#define YL_INTERP_H


class yl_callframe;
class yl_cothread;


// Invoke interpreter.

void yl_invoke( yl_callframe& xf );


// Generate one value from cothread.

struct yl_genvalues { unsigned base; unsigned count; };
yl_genvalues yl_generate( yl_cothread* generator );




#endif
