//
//  oexpand.cpp
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oexpand.h"


ometatype oexpand::metatype = { &mark_expand, "object" };



oexpand* oexpand::alloc()
{
    void* p = malloc( sizeof( oexpand ) );
    return new ( p ) oexpand( &metatype, ocontext::context->empty );
}

oexpand* oexpand::alloc( oexpand* prototype )
{
    void* p = malloc( sizeof( oexpand ) );
    return new ( p ) oexpand( &metatype, prototype->empty() );
}


oexpand::oexpand( ometatype* metatype, oclass* klass )
    :   obase( metatype )
    ,   klass( klass )
{
}



void oexpand::delkey( osymbol key )
{
    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
#if OEXPANDSLOTS
        oslotindex index = lookup->value;
        slots->store( index.slot, ovalue::undefined );
        if ( index.dual >= 2 )
            slots->store( index.dual - 2, ovalue::undefined );
#else
        slots->at( lookup->value ) = ovalue::undefined;
#endif
    }
}



oclass* oexpand::empty()
{
    if ( klass->is_prototype )
    {
#if OEXPANDSLOTS
        return slots->load( 0 ).as< oclass >();
#else
        return slots->at( 0 ).load().as< oclass >();
#endif
    }
    else
    {
        // Create empty.
        oclass* empty = oclass::alloc();
        empty->prototype = this;
        
        // Assign to appropriate slot.  The class we expand to will
        // have is_prototype set and empty will be assigned slot 0.
#if OEXPANDSLOTS
        expanddual( osymbol(), oslotindex(), empty );
#else
        expandkey( osymbol(), empty );
#endif
        
        // Return the created empty.
        return empty;
    }
}



#if OEXPANDSLOTS

void oexpand::dualkey( osymbol key, oslotindex index, ovalue value )
{
    if ( index.dual >= oslotindex::DUALSLOT )
    {
        // Dual property write.  Remember, slot is the _number_ slot, and
        // dual is the reference.  This is because any value can safely be
        // stored in 'number' slots, while references can't hold numbers.
        size_t dualslot = index.dual - oslotindex::DUALSLOT;
        if ( value.is_number() )
        {
            // Write number to number slot, and undefined to reference slot.
            slots->store( index.slot, value );
            slots->store( dualslot, ovalue::undefined );
        }
        else
        {
            // Write reference to both slots.
            slots->store( index.slot, value );
            slots->store( dualslot, value );
        }
    }
    else
    {
        // Otherwise, we need to promote the slot to a dual slot and
        // construct a new class.
        expanddual( key, index, value );
    }
}


void oexpand::expanddual( osymbol key, oslotindex index, ovalue value )
{
    oclass* klass = this->klass;
    oslotlist* slots = this->slots;

    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;

    // If the new class resizes the slotlist then number slots (which
    // are allocated from the bottom up) will end up in different locations.
    size_t size = slots->size();
    size_t watermark = slots->watermark();
    size_t expand_size = watermark + klass->numbercount + 1;
    expand_size = std::max( ceil_pow2( expand_size ), (size_t)8 );
    
    // Where we would place the new value (assuming it's not slot 0).
    size_t reference_slot = watermark;
    size_t number_slot = expand_size - 1 - klass->numbercount;
    assert( number_slot >= reference_slot );
        
    // Find class describing expanded object.
    oclass* expand = nullptr;
    bool is_number = value.is_number();
    auto& expandtable = is_number ? klass->expandnum : klass->expandref;
    auto lookup = expandtable.lookup( key );
    if ( lookup )
    {
        // Expand class already exists, use it.
        expand = lookup->value;
    }
    else
    {
        // Build new class.
        expand = oclass::alloc();
        expand->prototype = klass->prototype;
        
        bool made_dual = false;
        for ( auto i = klass->lookup.iter();
                        i != nullptr; i = klass->lookup.next( i ) )
        {
            oslotindex index = i->value;
            
            // Numbers, or the number part of dual slots, may end up moving.
            // References may be pushed downwards due to addition of slot 0.
            if ( index.dual == oslotindex::REFERENCE )
            {
                index.slot += offset;
            }
            else if ( index.dual == oslotindex::NUMBER )
            {
                index.slot += expand_size - size;
            }
            else if ( index.dual >= oslotindex::DUALSLOT )
            {
                index.slot += expand_size - size;
                index.dual += offset;
            }
            
            // If this is the key that is expanding, then it needs to become
            // a dual slot.
            if ( i->key == key )
            {
                assert( key );
                if ( is_number )
                {
                    index.dual = oslotindex::DUALSLOT + index.slot;
                    index.slot = number_slot;
                }
                else
                {
                    index.dual = oslotindex::DUALSLOT + reference_slot;
                }
                
                made_dual = true;
            }
            
            expand->lookup.insert( i->key, index );
        }
        
        expand->numbercount = klass->numbercount + ( is_number ? 1 : 0 );
        
        if ( key )
        {
            // Add key, if it wasn't a key that expanded.
            if ( ! made_dual )
            {
                oslotindex index;
                if ( is_number )
                {
                    index.slot = number_slot;
                    index.dual = oslotindex::NUMBER;
                }
                else
                {
                    index.slot = reference_slot;
                    index.dual = oslotindex::REFERENCE;
                }
                
                expand->lookup.insert( key, index );
            }
            
            expand->is_prototype = klass->prototype;
        }
        else
        {
            // Add prototype reference at slot 0.
            assert( ! is_number );
            assert( ! klass->is_prototype );
            oslotindex index( 0, oslotindex::REFERENCE );
            expand->lookup.insert( key, index );
            expand->is_prototype = true;
        }
        
        
        // Remember new class.
        expandtable.insert( key, expand );
    }
    
    // Update class.
    this->klass = expand;
    
    if ( size != expand_size )
    {
        // Slots require reallocation.
        oslotlist* newslots = oslotlist::alloc( expand_size );
        for ( size_t i = 0; i < watermark; ++i )
        {
            newslots->store( offset + i, slots->load( i ) );
        }
        for ( size_t i = size - klass->numbercount; i < size; ++i )
        {
            newslots->store( i + expand_size - size, slots->load( i ) );
        }
        
        if ( key )
        {
            size_t slot = is_number ? number_slot : reference_slot;
            storedual( index, slot, value );
            newslots->set_watermark( watermark + ( is_number ? 0 : 1 ) );
        }
        else
        {
            assert( ! is_number );
            slots->store( 0, value );
            slots->set_watermark( watermark + 1 );
        }
        
        this->slots = slots;
    }
    else if ( key )
    {
        // Store new slot.
        size_t slot = is_number ? number_slot : reference_slot;
        storedual( index, slot, value );
        if ( ! is_number )
        {
            slots->set_watermark( watermark + 1 );
        }
    }
    else
    {
        // Shuffle slots and add at slot 0.
        for ( size_t i = watermark; i > 0; --i )
        {
            slots->store( i, slots->load( i - 1 ) );
        }
        
        assert( ! is_number );
        slots->store( 0, value );
        slots->set_watermark( watermark + 1 );
    }
}


void oexpand::storedual( oslotindex index, size_t newslot, ovalue value )
{
    bool is_number = value.is_number();
    if ( index.slot != oslotindex::INVALID )
    {
        // Slot existed before the expand, this is now a dual slot.
        if ( is_number )
        {
            assert( index.dual == oslotindex::REFERENCE );
            slots->store( index.slot, ovalue::undefined );
            slots->store( newslot, value );
        }
        else
        {
            assert( index.dual == oslotindex::NUMBER );
            slots->store( index.slot, value );
            slots->store( newslot, value );
        }
    }
    else
    {
        // Single slot.
        if ( is_number )
        {
            slots->store( newslot, value );
        }
        else
        {
            slots->store( newslot, value );
        }
    }
}



#else



void oexpand::expandkey( osymbol key, ovalue value )
{
    oclass* klass = this->klass;
    otuple< ovalue >* slots = this->slots;
    
    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;

    // Find class describing expanded object.
    oclass* expand = nullptr;
    auto lookup = klass->expand.lookup( key );
    if ( lookup )
    {
        // Expand class already exists, use it.
        expand = lookup->value;
    }
    else
    {
        // Build new class.
        expand = oclass::alloc();
        expand->prototype = klass->prototype;
        
        for ( auto i = klass->lookup.iter();
                        i != nullptr; i = klass->lookup.next( i ) )
        {
            expand->lookup.insert( i->key, offset + i->value );
        }
        
        if ( key )
        {
            expand->lookup.insert( key, klass->lookup.size() );
            expand->is_prototype = klass->is_prototype;
        }
        else
        {
            assert( ! klass->is_prototype );
            expand->lookup.insert( key, 0 );
            expand->is_prototype = true;
        }
        
        // Link it in so it gets reused.
        klass->expand.insert( key, expand );
    }
    
    // Update class.
    this->klass = expand;
    
    if ( ! slots || slots->size() < expand->lookup.size() )
    {
        // Slots require reallocation.
        size_t size = ceil_pow2( expand->lookup.size() );
        size = std::max( size, (size_t)8 );

        otuple< ovalue >* newslots = otuple< ovalue >::alloc( size );
        for ( size_t i = 0; i < klass->lookup.size(); ++i )
        {
            newslots->at( i + offset ) = slots->at( i );
        }
        
        if ( key )
            newslots->at( klass->lookup.size() ) = value;
        else
            newslots->at( 0 ) = value;
        
        this->slots = slots;
    }
    else if ( key )
    {
        // Add new value in its position.
        slots->at( klass->lookup.size() ) = value;
    }
    else
    {
        // Shuffle slots downwards to make room for slot 0.
        for ( size_t i = klass->lookup.size(); i > 0; --i )
        {
            slots->at( i ) = slots->at( i - 1 );
        }
        
        slots->at( 0 ) = value;
    }
}


#endif



void oexpand::mark_expand( oworklist* work, obase* object, ocolour colour )
{
    oexpand* expand = (oexpand*)object;
    omark< oclass* >::mark( expand->klass, work, colour );
#if OEXPANDSLOTS
    omark< oslotlist* >::mark( expand->slots, work, colour );
#else
    omark< otuple< ovalue >* >::mark( expand->slots, work, colour );
#endif
}








ometatype oclass::metatype = { &mark_class, "class" };


oclass* oclass::alloc()
{
    void* p = malloc( sizeof( oclass ) );
    return new ( p ) oclass( &metatype );
}


oclass::oclass( ometatype* metatype )
    :   obase( metatype )
#if OEXPANDCLASS
    ,   numbercount( 0 )
#endif
    ,   is_prototype( false )
{
}


void oclass::mark_class( oworklist* work, obase* object, ocolour colour )
{
    oclass* klass = (oclass*)object;
    omark< oexpand* >::mark( klass->prototype, work, colour );
#if OEXPANDSLOTS
    omark< okeytable< osymbol, oslotindex > >::mark( klass->lookup, work, colour );
    omark< okeytable< osymbol, oclass* > >::mark( klass->expandref, work, colour );
    omark< okeytable< osymbol, oclass* > >::mark( klass->expandnum, work, colour );
#else
    omark< okeytable< osymbol, size_t > >::mark( klass->lookup, work, colour );
    omark< okeytable< osymbol, oclass* > >::mark( klass->expand, work, colour );
#endif
}














