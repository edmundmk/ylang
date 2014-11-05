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
        expandkey( osymbol(), empty );
        
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
        
    
    
    }
}


void oexpand::expandkey( osymbol key, ovalue value )
{
}




#else



void oexpand::expandkey( osymbol key, ovalue value )
{
    oclass* klass = this->klass;
    otuple< ovalue >* slots = this->slots;
    oclass* expand = nullptr;
    
    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;

    // Find class describing expanded object.
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
        size = std::min( size, (size_t)8 );

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














