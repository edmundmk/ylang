//
//  yl_upval.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#include "yl_upval.h"




yl_gctype yl_upval::gctype =
{
    "upval",
    &yl_upval::destroy,
    &yl_upval::mark,
    nullptr
};


yl_stackref< yl_upval > yl_upval::alloc( unsigned index )
{
    void* p = yl_current->allocate( sizeof( yl_upval ) );
    return new ( p ) yl_upval( index );
}

yl_upval::yl_upval( unsigned index )
    :   yl_gcobject( YLOBJ_UPVAL, OPEN )
    ,   _index( index )
{
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


