//
//  yl_object.cpp
//
//  Created by Edmund Kapusniak on 13/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_object.h"



yl_object* yl_object::alloc( yl_object* prototype )
{
    void* p = yl_current->malloc( sizeof( yl_object ) );
    return new ( p ) yl_object( prototype );
}



yl_object::yl_object( yl_object* prototype )
    :   yl_heapobj( YLOBJ_OBJECT )
    ,   _klass( yl_current->klassof( prototype ) )
    ,   _slots( nullptr )
{
}

yl_object* yl_object::superof() const
{
    yl_slot* klass = _klass.get();
    while ( true )
    {
        if ( klass->_hash )
        {
            return klass->_hash->superof;
        }
        
        yl_heapobj* parent = klass->_parent.get();
        if ( ! parent )
        {
            return nullptr;
        }
        if ( parent->kind() & YLOBJ_IS_OBJECT )
        {
            return (yl_object*)parent;
        }
        
        assert( parent->kind() == YLOBJ_SLOT );
        klass = (yl_slot*)parent;
    }
}



