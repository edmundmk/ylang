//
//  ythunk.cpp
//
//  Created by Edmund Kapusniak on 10/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ythunk.h"
#include "ymodel.h"
#include "yvm.h"




void yframe::grow_stack()
{
    ystack* stack = yscope::scope->stack;
    size_t sp = s - stack->stack.data();
    stack->ensure_stack( sp + 16 );
    s = stack->stack.data() + sp;
    limit = stack->stack.data() + stack->stack.size();
}

