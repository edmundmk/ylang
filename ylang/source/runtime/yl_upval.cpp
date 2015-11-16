//
//  yl_upval.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#include "yl_upval.h"




yl_gctype yl_upval::gctype =
{
    YLOBJ_UPVAL,
    YL_GCFLAG_NONE,
    "upval",
    &yl_upval::destroy,
    &yl_upval::mark,
    nullptr
};


yl_upval::yl_upval( unsigned index )
    :   _index( index )
{
    set_gcflags( OPEN );
}

void yl_upval::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_upval* self = (yl_upval*)object;
    self->~yl_upval();
}

void yl_upval::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_upval* self = (yl_upval*)object;
    self->_value.mark( heap );
}


