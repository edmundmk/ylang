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
        else
        {
            assert( parent->kind() == YLOBJ_SLOT );
            klass = (yl_slot*)parent;
        }
    }
}



yl_value yl_object::get_key( const yl_symbol& key ) const
{
    const yl_object* object = this;
    size_t index;

    yl_slot* klass = object->_klass.get();
    while ( true )
    {
        // Check for shortcut hash.
        if ( klass->_hash )
        {
            auto i = klass->_hash->lookup.find( key );
            if ( i != klass->_hash->lookup.end() )
            {
                index = i->second;
                break;
            }
            else if ( klass->_hash->superof )
            {
                object = klass->_hash->superof;
                klass = object->_klass.get();
            }
            else
            {
                return yl_value( YLOBJ_UNDEF, yl_undef );
            }
        }
        
        // Check if we've found the key.
        if ( klass->_symbol.get() == key )
        {
            index = klass->_index;
            break;
        }
        
        // Move to parent klass or prototype.
        yl_heapobj* parent = klass->_parent.get();
        if ( parent )
        {
            if ( parent->kind() == YLOBJ_SLOT )
            {
                klass = (yl_slot*)parent;
            }
            else
            {
                assert( parent->kind() & YLOBJ_IS_OBJECT );
                object = (yl_object*)parent;
                klass = object->_klass.get();
            }
        }
        else
        {
            return yl_value( YLOBJ_UNDEF, yl_undef );
        }
    }
    
    return object->_slots.get()->at( index ).get();
}

void yl_object::set_key( const yl_symbol& key, const yl_value& value )
{
}

void yl_object::del_key( const yl_symbol& key )
{
}



