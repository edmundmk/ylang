//
//  yexpand.cpp
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yexpand.h"


ymetatype yexpand::metatype = { &mark_expand, "object" };



yexpand* yexpand::alloc()
{
    void* p = malloc( sizeof( yexpand ) );
    return new ( p ) yexpand( &metatype, nullptr );
}

yexpand* yexpand::alloc( yexpand* prototype )
{
    void* p = malloc( sizeof( yexpand ) );
    return new ( p ) yexpand( &metatype, prototype->empty_class() );
}


yexpand::yexpand( ymetatype* metatype, yclass* klass )
    :   yobject( metatype )
    ,   klass( klass ? klass : yscope::scope->model->empty_class() )
{
}



void yexpand::delkey( ysymbol key )
{
    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
#if YSLOTS
        yslotindex index = lookup->value;
        slots->set( index.slot, yvalue::undefined );
        if ( index.dual >= 2 )
        {
            slots->set( index.dual - 2, yvalue::undefined );
        }
#else
        slots->set( lookup->value, yvalue::undefined );
#endif
    }
}



yclass* yexpand::empty_class()
{
    if ( klass->is_prototype )
    {
#if YSLOTS
        return slots->get( 0 ).as< yclass >();
#else
        return slots->get( 0 ).get().as< yclass >();
#endif
    }
    else
    {
        // Create empty.
        yclass* empty = yclass::alloc();
        empty->prototype = this;
        
        // Assign to appropriate slot.  The class we expand to will
        // have is_prototype set and empty will be assigned slot 0.
#if YSLOTS
        expanddual( ysymbol(), yslotindex(), empty );
#else
        expandkey( ysymbol(), empty );
#endif
        
        // Return the created empty.
        return empty;
    }
}



#if YSLOTS


void yexpand::dualkey( ysymbol key, yslotindex index, yvalue value )
{
    if ( index.dual >= yslotindex::DUALSLOT )
    {
        // Dual property write.  Remember, slot is the _number_ slot, and
        // dual is the reference.  This is because any value can safely be
        // stored in 'number' slots, while references can't hold numbers.
        size_t dualslot = index.dual - yslotindex::DUALSLOT;
        if ( value.is_number() )
        {
            // Write number to number slot, and undefined to reference slot.
            slots->set( index.slot, value );
            slots->set( dualslot, yvalue::undefined );
        }
        else
        {
            // Write reference to both slots.
            slots->set( index.slot, value );
            slots->set( dualslot, value );
        }
    }
    else
    {
        // Otherwise, we need to promote the slot to a dual slot and
        // construct a new class.
        expanddual( key, index, value );
    }
}


void yexpand::expanddual( ysymbol key, yslotindex index, yvalue value )
{
    ymodel* model = yscope::scope->model;
    yclass* klass = this->klass;
    yslots* slots = this->slots;
    
    
    // Get (or build) our new class.
    bool is_number = value.is_number();
    yclass* expand = model->expand_class( klass, key, is_number );
    
    
    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;

    // Find old and new slot sizes.
    size_t current_size = klass->refslots + klass->numslots;
    current_size = std::max( ceil_pow2( current_size ), (size_t)8 );
    assert( slots->size() == current_size );
    
    size_t expand_size = expand->refslots + expand->numslots;
    expand_size = std::max( ceil_pow2( expand_size ), (size_t)8 );


    // Update class.
    this->klass = expand;
    
    
    // Add value in correct slot.
    size_t newslot;
    if ( is_number )
        newslot = expand_size - expand->numslots;
    else
        newslot = expand->refslots - 1;
    
    if ( current_size != expand_size )
    {
        // Slots require reallocation.
        yslots* newslots = yslots::alloc( expand_size );
        for ( size_t i = 0; i < klass->refslots; ++i )
        {
            newslots->set( offset + i, slots->get( i ) );
        }
        for ( size_t i = current_size - klass->numslots; i < current_size; ++i )
        {
            newslots->set( i + expand_size - current_size, slots->get( i ) );
        }
        
        if ( key )
        {
            storedual( index, newslot, value );
        }
        else
        {
            assert( ! is_number );
            newslots->set( 0, value );
        }
        
        newslots->set_watermark( expand->refslots );
        this->slots = newslots;
    }
    else if ( key )
    {
        // Store new slot.
        storedual( index, newslot, value );
        if ( klass->refslots != expand->refslots )
        {
            slots->set_watermark( expand->refslots );
        }
    }
    else
    {
        // Shuffle slots and add at slot 0.
        for ( size_t i = klass->refslots; i > 0; --i )
        {
            slots->set( i, slots->get( i - 1 ) );
        }
        
        assert( ! is_number );
        slots->set( 0, value );
        slots->set_watermark( expand->refslots );
    }
}


void yexpand::storedual( yslotindex index, size_t newslot, yvalue value )
{
    bool is_number = value.is_number();
    if ( index.slot != yslotindex::INVALID )
    {
        // Slot existed before the expand, this is now a dual slot.
        if ( is_number )
        {
            assert( index.dual == yslotindex::REFERENCE );
            slots->set( index.slot, yvalue::undefined );
            slots->set( newslot, value );
        }
        else
        {
            assert( index.dual == yslotindex::NUMBER );
            slots->set( index.slot, value );
            slots->set( newslot, value );
        }
    }
    else
    {
        // Single slot.
        if ( is_number )
        {
            slots->set( newslot, value );
        }
        else
        {
            slots->set( newslot, value );
        }
    }
}



#else



void yexpand::expandkey( ysymbol key, yvalue value )
{
    ymodel* model = yscope::scope->model;
    yclass* klass = this->klass;
    ytuple< yvalue >* slots = this->slots;


    // Get (or create) expanded class.
    yclass* expand = model->expand_class( klass, key );

    
    // Update class.
    this->klass = expand;
    
    if ( ! slots || slots->size() < expand->lookup.size() )
    {
        // Slots require reallocation.
        size_t size = ceil_pow2( expand->lookup.size() );
        size = std::max( size, (size_t)8 );


        // If key is null then we're allocating slot 0 for the prototype.
        size_t offset = key ? 0 : 1;


        // Copy slots over.
        ytuple< yvalue >* newslots = ytuple< yvalue >::alloc( size );
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



void yexpand::mark_expand( yobject* object, yworklist* work, ycolour colour )
{
    yexpand* expand = (yexpand*)object;
    ymark( expand->klass, work, colour );
    ymark( expand->slots, work, colour );
}








ymetatype yclass::metatype = { &mark_class, "class" };


yclass* yclass::alloc()
{
    void* p = malloc( sizeof( yclass ) );
    return new ( p ) yclass( &metatype );
}


yclass::yclass( ymetatype* metatype )
    :   yobject( metatype )
#if YSLOTS
    ,   refslots( 0 )
    ,   numslots( 0 )
#endif
    ,   is_prototype( false )
{
}


void yclass::mark_class( yobject* object, yworklist* work, ycolour colour )
{
    yclass* klass = (yclass*)object;
    ymark( klass->prototype, work, colour );
    ymark( klass->lookup, work, colour );
#if YSLOTS
    ymark( klass->expandref, work, colour );
    ymark( klass->expandnum, work, colour );
#else
    ymark( klass->expand, work, colour );
#endif
}














