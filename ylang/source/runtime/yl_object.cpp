//
//  yl_object.cpp
//
//  Created by Edmund Kapusniak on 13/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_object.h"



yl_gctype yl_object::gctype =
{
    &yl_object::destroy,
    &yl_object::mark,
    nullptr,
};



yl_stackref< yl_object > yl_object::alloc( yl_object* prototype )
{
    void* p = yl_current->allocate( sizeof( yl_object ) );
    return new ( p ) yl_object( YLOBJ_OBJECT, prototype );
}



yl_object::yl_object( yl_objkind kind, yl_object* prototype )
    :   yl_gcobject( kind )
    ,   _klass( yl_current->klassof( prototype ) )
    ,   _slots( nullptr )
{
}


void yl_object::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_object* self = (yl_object*)object;
    self->~yl_object();
}

void yl_object::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_object* self = (yl_object*)object;
    self->_klass.mark( heap );
    self->_slots.mark( heap );
}



yl_object* yl_object::superof() const
{
    yl_slot* slot = _klass.get();
    while ( true )
    {
        if ( slot->_hash )
        {
            return slot->_hash->superof;
        }
        
        if ( slot->_index != yl_slot::EMPTY_KLASS )
        {
            slot = (yl_slot*)slot->_parent.get();
        }
        else
        {
            return (yl_object*)slot->_parent.get();
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
                return yl_undef;
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
            yl_gcobject* parent = slot->_parent.get();
            assert( parent->kind() == YLOBJ_SLOT );
            slot = (yl_slot*)parent;
        }
        else
        {
            // Continue lookup in prototype.
            yl_gcobject* prototype = slot->_parent.get();
            if ( prototype )
            {
                object = (yl_object*)prototype;
                slot  = object->_klass.get();
            }
            else
            {
                return yl_undef;
            }
        }
    }
    
    return object->_slots.get()->at( index ).get();
}

void yl_object::set_key( const yl_symbol& key, yl_value value )
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
            yl_gcobject* parent = slot->_parent.get();
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

    yl_stackref< yl_slot > new_klass = nullptr;

    yl_current->weak_lock();

    for ( yl_slot* slot = klass->_head; slot; slot = slot->_next )
    {
        assert( slot->_index != yl_slot::EMPTY_KLASS );
        if ( slot->_symbol.get() == key )
        {
            new_klass = yl_current->weak_obtain( slot );
            if ( new_klass )
            {
                break;
            }
        }
    }

    if ( ! new_klass )
    {
        new_klass = yl_slot::alloc( klass, key.string() );
        
        // Link new slot into the list of child slots from the current klass.
        new_klass->_next = klass->_head;
        klass->_head = yl_current->weak_create( new_klass.get() );
    }
    
    yl_current->weak_unlock();

    
    // Increase size of slot array if required.
    yl_valarray* slots = _slots.get();
    if ( ! slots || slots->size() <= new_klass->_index )
    {
        yl_stackref< yl_valarray > new_slots =
            yl_valarray::alloc( slots ? slots->size() * 2 : 4 );
        
        if ( slots )
        {
            for ( size_t i = 0; i < slots->size(); ++i )
            {
                new_slots->at( i ).set( slots->at( i ).get() );
            }
        }
        
        _slots.set( new_slots.get() );
    }
    
    
    // Update klass and assign value into new slot.
    _klass.set( new_klass.get() );
    slots->at( new_klass->_index ).set( value );
}

void yl_object::del_key( const yl_symbol& key )
{
    yl_slot* klass = _klass.get();
    
    // Deleting the last-added property is easy.
    if ( klass->_index != yl_slot::EMPTY_KLASS && klass->_symbol.get() == key )
    {
        yl_gcobject* parent = klass->_parent.get();
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
            yl_gcobject* parent = slot->_parent.get();
            assert( parent->kind() == YLOBJ_SLOT );
            slot = (yl_slot*)parent;
        }
        else
        {
            return;
        }
    }
    
    _slots.get()->at( index ).set( yl_undef );
}





yl_gctype yl_slot::gctype =
{
    
};


yl_stackref< yl_slot > yl_slot::alloc( yl_object* prototype )
{
    void* p = yl_current->allocate( sizeof( yl_slot ) );
    return new ( p ) yl_slot( prototype );
}


yl_stackref< yl_slot > yl_slot::alloc( yl_slot* parent, yl_string* symbol )
{
    void* p = yl_current->allocate( sizeof( yl_slot ) );
    return new ( p ) yl_slot( parent, symbol );
}


void yl_slot::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_slot* self = (yl_slot*)object;

    // Remove from list of parent.  GC should be holding the weak lock.
    yl_gcobject* parent = self->_parent.get();
    if ( parent && parent->kind() == YLOBJ_SLOT )
    {
        yl_slot* parent_klass = (yl_slot*)parent;
        if ( parent_klass->_head == self )
        {
            parent_klass->_head = _next;
        }
        else for ( yl_slot* prev = _head; prev != nullptr; prev = prev->_next )
        {
            if ( prev->_next == self )
            {
                prev->_next = _next;
                break;
            }
        }
    }

    // Destroy.
    self->~yl_slot();
}

void yl_slot::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_slot* self = (yl_slot*)object;
    self->_parent.mark( heap );
    self->_symbol.mark( heap );
}


yl_slot::yl_slot( yl_object* prototype )
    :   yl_gcobject( YLOBJ_SLOT )
    ,   _parent( prototype )
    ,   _index( EMPTY_KLASS )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
}


yl_slot::yl_slot( yl_slot* parent, yl_string* symbol )
    :   yl_gcobject( YLOBJ_SLOT )
    ,   _parent( parent )
    ,   _symbol( symbol )
    ,   _index( parent->_index + 1 )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
}









