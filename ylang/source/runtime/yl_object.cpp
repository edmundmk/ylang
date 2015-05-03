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
    yl_slot* slot = object->_klass.get();
    size_t index;

    while ( true )
    {
        // Check for shortcut hash.
        if ( slot->_hash )
        {
            auto i = slot->_hash->lookup.find( key );
            if ( i != slot->_hash->lookup.end() )
            {
                index = i->second;
                break;
            }
            else if ( slot->_hash->superof )
            {
                object = slot->_hash->superof;
                slot = object->_klass.get();
            }
            else
            {
                return yl_value( YLOBJ_UNDEF, yl_undef );
            }
        }
        
        // Check if this is a real slot or a pointer to the prototype.
        if ( slot->_index != yl_slot::EMPTY_KLASS )
        {
            // Check if we've found the key.
            if ( slot->_symbol.get() == key )
            {
                index = slot->_index;
                break;
            }
            
            // Check parent slot.
            yl_heapobj* parent = slot->_parent.get();
            assert( parent->kind() == YLOBJ_SLOT );
            slot = (yl_slot*)parent;
        }
        else
        {
            // Continue lookup in prototype.
            yl_heapobj* prototype = slot->_parent.get();
            if ( prototype )
            {
                assert( prototype->kind() & YLOBJ_IS_OBJECT );
                object = (yl_object*)prototype;
                slot  = object->_klass.get();
            }
            else
            {
                return yl_value( YLOBJ_UNDEF, yl_undef );
            }
        }
    }
    
    return object->_slots.get()->at( index ).get();
}

void yl_object::set_key( const yl_symbol& key, const yl_value& value )
{
    yl_slot* klass = _klass.get();
    
    // Check if slot exists.
    yl_slot* slot = klass;
    size_t index;
    
    while ( true )
    {
        // Check for shortcut hash.
        if ( slot->_hash )
        {
            auto i = slot->_hash->lookup.find( key );
            if ( i != slot->_hash->lookup.end() )
            {
                index = i->second;
                break;
            }
            else
            {
                goto notfound;
            }
        }
        
        // Check if this is a real slot or a pointer to the prototype.
        if ( slot->_index != yl_slot::EMPTY_KLASS )
        {
            // Check if we've found the key.
            if ( slot->_symbol.get() == key )
            {
                index = slot->_index;
                break;
            }
            
            // Check parent slot.
            yl_heapobj* parent = slot->_parent.get();
            assert( parent->kind() == YLOBJ_SLOT );
            slot = (yl_slot*)parent;
        }
        else
        {
            goto notfound;
        }
    }

    _slots.get()->at( index ).set( value );
    return;

notfound:

    // Need to add another slot with the key.  First check if an appropriate
    // slot already exists.  Otherwise, create a new one.
    
    // TODO: interact with GC, which may be removing unused slots.
    
    yl_slot* new_klass = nullptr;

    for ( yl_slot* slot = klass->_head; slot; slot = slot->_next )
    {
        assert( slot->_index != yl_slot::EMPTY_KLASS );
        if ( slot->_symbol.get() == key )
        {
            new_klass = slot;
            break;
        }
    }

    if ( ! new_klass )
    {
        new_klass = yl_slot::alloc( klass, key.string() );
        
        // Link new slot into the list of child slots from the current klass.
        new_klass->_next = klass->_head;
        klass->_head = new_klass;
    }

    
    // Increase size of slot array if required.
    yl_valarray* slots = _slots.get();
    if ( ! slots || slots->size() <= new_klass->_index )
    {
        yl_valarray* old_slots = slots;
        slots = yl_valarray::alloc( slots ? slots->size() * 2 : 4 );
        
        if ( old_slots )
        {
            for ( size_t i = 0; i < old_slots->size(); ++i )
            {
                slots->at( i ).set( old_slots->at( i ).get() );
            }
        }
        
        _slots.set( slots );
    }
    
    
    // Assign value into new slot.
    slots->at( new_klass->_index ).set( value );

}

void yl_object::del_key( const yl_symbol& key )
{
    yl_slot* klass = _klass.get();
    
    // Deleting the last-added property is easy.
    if ( klass->_index != yl_slot::EMPTY_KLASS && klass->_symbol.get() == key )
    {
        yl_heapobj* parent = klass->_parent.get();
        assert( parent->kind() == YLOBJ_SLOT );
        _klass.set( (yl_slot*)parent );
        return;
    }
    
    // Otherwise do a search and set the property to undef.
    yl_slot* slot = klass;
    size_t index;

    while ( true )
    {
        // Check for shortcut hash.
        if ( slot->_hash )
        {
            auto i = slot->_hash->lookup.find( key );
            if ( i != slot->_hash->lookup.end() )
            {
                index = i->second;
                break;
            }
            else
            {
                return;
            }
        }
        
        // Check if this is a real slot or a pointer to the prototype.
        if ( slot->_index != yl_slot::EMPTY_KLASS )
        {
            // Check if we've found the key.
            if ( slot->_symbol.get() == key )
            {
                index = slot->_index;
                break;
            }
            
            // Check parent slot.
            yl_heapobj* parent = slot->_parent.get();
            assert( parent->kind() == YLOBJ_SLOT );
            slot = (yl_slot*)parent;
        }
        else
        {
            return;
        }
    }
    
    _slots.get()->at( index ).set( yl_value( YLOBJ_UNDEF, yl_undef ) );
}






yl_slot* yl_slot::alloc( yl_object* prototype )
{
    void* p = yl_current->malloc( sizeof( yl_slot ) );
    return new ( p ) yl_slot( prototype );
}


yl_slot* yl_slot::alloc( yl_slot* parent, yl_string* symbol )
{
    void* p = yl_current->malloc( sizeof( yl_slot ) );
    return new ( p ) yl_slot( parent, symbol );
}


yl_slot::yl_slot( yl_object* prototype )
    :   yl_heapobj( YLOBJ_SLOT )
    ,   _parent( prototype )
    ,   _index( EMPTY_KLASS )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
}


yl_slot::yl_slot( yl_slot* parent, yl_string* symbol )
    :   yl_heapobj( YLOBJ_SLOT )
    ,   _parent( parent )
    ,   _symbol( symbol )
    ,   _index( parent->_index + 1 )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
}









