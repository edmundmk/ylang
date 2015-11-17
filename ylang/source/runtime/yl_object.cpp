//
//  yl_object.cpp
//
//  Created by Edmund Kapusniak on 13/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_object.h"



yl_gctype yl_object::gctype =
{
    YLOBJ_OBJECT,
    YL_GCFLAG_NONE,
    "object",
    &yl_object::destroy,
    &yl_object::mark,
    nullptr,
};


yl_object::yl_object()
    :   yl_object( yl_current->proto_object() )
{
}

yl_object::yl_object( yl_object* prototype )
    :   _klass( yl_current->klassof( prototype ) )
    ,   _slots( nullptr )
{
    // Make sure all prototypes are sealed.
    if ( prototype )
    {
        prototype->seal();
    }
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



void yl_object::seal()
{
    set_gcflags( SEALED );
}


yl_object* yl_object::superof() const
{
    yl_slot* slot = _klass.get();
    while ( true )
    {
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


yl_value yl_object::get_key_impl( const yl_symbol& key, yl_ilcache* ilc ) const
{
    const yl_object* object = this;
    yl_slot* slot = object->_klass.get();
    size_t index;

    while ( true )
    {
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
    
    if ( ilc )
    {
        uintptr_t klassid = _klass.get()->_klassid;
        if ( object == this )
        {
            *ilc = yl_ilcache( klassid, index );
        }
        else
        {
            *ilc = yl_ilcache( klassid, &object->_slots.get()->at( index ) );
        }
    }
    
    return object->_slots.get()->at( index ).get();
}

bool yl_object::set_key_impl( const yl_symbol& key, yl_value value, yl_ilcache* ilc )
{
    yl_slot* klass = _klass.get();
    
    // Check if slot exists.
    yl_slot* slot = klass;
    size_t index;
    
    while ( true )
    {
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
    
    if ( ilc )
    {
        *ilc = yl_ilcache( klass->_klassid, index );
    }

    _slots.get()->at( index ).set( value );
    return true;

notfound:

    if ( check_gcflags( SEALED ) )
    {
        return false;
    }

    // Need to add another slot with the key.  First check if an appropriate
    // slot already exists.  Otherwise, create a new one.

    yl_rootref< yl_slot > new_klass = nullptr;

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
        new_klass = yl_gcnew< yl_slot >( klass, key.string() );
        
        // Link new slot into the list of child slots from the current klass.
        new_klass->_next = klass->_head;
        klass->_head = yl_current->weak_create( new_klass.get() );
    }
    
    yl_current->weak_unlock();

    
    // Increase size of slot array if required.
    yl_valarray* slots = _slots.get();
    if ( ! slots || slots->size() <= new_klass->_index )
    {
        yl_rootref< yl_valarray > new_slots =
            yl_valarray::alloc( slots ? slots->size() * 2 : 4 );
        
        if ( slots )
        {
            for ( size_t i = 0; i < slots->size(); ++i )
            {
                new_slots->at( i ).set( slots->at( i ).get() );
            }
        }
        
        slots = new_slots.get();
        _slots.set( slots );
    }
    
    
    // Update klass and assign value into new slot.
    _klass.set( new_klass.get() );
    
    if ( ilc )
    {
        *ilc = yl_ilcache( new_klass->_klassid, new_klass->_index );
    }
    
    slots->at( new_klass->_index ).set( value );
    return true;
}

bool yl_object::del_key( const yl_symbol& key )
{
    yl_slot* klass = _klass.get();
    
    if ( check_gcflags( SEALED ) )
    {
        return false;
    }
    
    // Deleting the last-added property is easy.
    if ( klass->_index != yl_slot::EMPTY_KLASS && klass->_symbol.get() == key )
    {
        yl_gcobject* parent = klass->_parent.get();
        assert( parent->kind() == YLOBJ_SLOT );
        _klass.set( (yl_slot*)parent );
        return true;
    }
    
    // Otherwise do a search and set the property to undef.
    yl_slot* slot = klass;
    size_t index;

    while ( true )
    {
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
            return true;
        }
    }
    
    _slots.get()->at( index ).set( yl_undef );
    return true;
}



bool yl_object::set_key( const char* key, yl_value value )
{
    return set_key( yl_string::alloc( key )->symbol().get(), value );
}





yl_gctype yl_slot::gctype =
{
    YLOBJ_SLOT,
    YL_GCFLAG_NONE,
    "slot",
    &yl_slot::destroy,
    &yl_slot::mark,
    nullptr
};



yl_slot::yl_slot( yl_object* prototype )
    :   _klassid( yl_current->klassid() )
    ,   _parent( prototype )
    ,   _index( EMPTY_KLASS )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
}


yl_slot::yl_slot( yl_slot* parent, yl_string* symbol )
    :   _klassid( yl_current->klassid() )
    ,   _parent( parent )
    ,   _symbol( symbol )
    ,   _index( parent->_index + 1 )
    ,   _head( nullptr )
    ,   _next( nullptr )
{
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
            parent_klass->_head = self->_next;
        }
        else
        {
            for ( yl_slot* prev = parent_klass->_head;
                            prev != nullptr; prev = prev->_next )
            {
                if ( prev->_next == self )
                {
                    prev->_next = self->_next;
                    break;
                }
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









