//
//  gc_heap.cpp
//
//  Created by Edmund Kapusniak on 19/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "gc_heap.h"



int x()
{
    gc_stackref< int > x;
    gc_stackref< int > y;
    x = y;
    x = new int();
    return 0;
}