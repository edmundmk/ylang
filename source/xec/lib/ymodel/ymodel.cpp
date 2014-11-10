//
//  ymodel.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ymodel.h"
#include "yobject.h"
#include "ystring.h"
#include "yexpand.h"
#include "yvm.h"
#include "yarray.h"
#include "ytable.h"



/*
    ymodel
*/

ymodel::ymodel()
    :   expand_empty( nullptr )
{
    yscope scope( this );
    expand_empty = yclass::alloc();
    array_proto = yarray::make_proto();
    table_proto = ytable::make_proto();
}

ymodel::~ymodel()
{
}


ystring* ymodel::make_symbol( const char* s )
{
    std::lock_guard< std::mutex > lock( symbols_mutex );

    // Look up symbol.
    size_t size = strlen( s );
    hash32_t hash = hash32( s, size );
    symkey key( hash, s, size );
    auto symbol = symbols.find( key );
    if ( symbol != symbols.end() )
    {
        // Mark symbol before returning, in case we are resurrecting it.
        symbol->second->mark( nullptr, yscope::scope->mark_colour );
        return symbol->second.get();
    }

    // Create new symbol.
    ystring* newsym = ystring::alloc( s );
    newsym->shash = hash;
    newsym->shashed = true;
    newsym->ssymbol = true;
    symbols.emplace( key, newsym );
    return newsym;
}


ystring* ymodel::make_symbol( ystring* s )
{
    std::lock_guard< std::mutex > lock( symbols_mutex );
    
    // Should already have checked if it's already a symbol.
    assert( ! s->ssymbol );

    // Look up symbol
    symkey key( s->hash(), s->data(), s->size() );
    auto symbol = symbols.find( key );
    if ( symbol != symbols.end() )
    {
        // Mark symbol mark returning, in case we are resurrecting it.
        symbol->second->mark( nullptr, yscope::scope->mark_colour );
        return symbol->second.get();
    }
    
    // Turn this string itself into a symbol and return it.
    assert( ! s->ssymbol );
    s->ssymbol = true;
    symbols.emplace( key, s );
    return s;
}




yclass* ymodel::empty_class()
{
    return expand_empty;
}

yclass* ymodel::array_class()
{
    return array_proto->empty_class();
}

yclass* ymodel::table_class()
{
    return table_proto->empty_class();
}


#if YSLOTS


yclass* ymodel::expand_class( yclass* klass, ysymbol key, bool is_number )
{
    std::lock_guard< std::mutex > lock( expand_mutex );
    
    // Check for existing expandclass.
    auto& expandtable = is_number ? klass->expandnum : klass->expandref;
    auto lookup = expandtable.lookup( key );
    if ( lookup )
    {
        return lookup->value;
    }


    // Build new class.
    yclass* expand = yclass::alloc();
    expand->prototype = klass->prototype;


    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;
    
    // Find size of current slot buffer.
    size_t current_size = klass->refslots + klass->numslots;
    current_size = std::max( ceil_pow2( current_size ), (size_t)8 );

    // We're adding a slot.  Find size of expanded slot buffer.
    expand->refslots = klass->refslots + ( is_number ? 0 : 1 );
    expand->numslots = klass->numslots + ( is_number ? 1 : 0 );
    size_t expand_size = expand->refslots + expand->numslots;
    expand_size = std::max( ceil_pow2( expand_size ), (size_t)8 );
    
    
    // Generate lookup for new class.
    bool made_dual = false;
    for ( auto i = klass->lookup.iter();
                    i != nullptr; i = klass->lookup.next( i ) )
    {
        yslotindex index = i->value;
        
        // Numbers, or the number part of dual slots, may end up moving.
        // References may be pushed downwards due to addition of slot 0.
        if ( index.dual == yslotindex::REFERENCE )
        {
            index.slot += offset;
        }
        else if ( index.dual == yslotindex::NUMBER )
        {
            index.slot += expand_size - current_size;
        }
        else if ( index.dual >= yslotindex::DUALSLOT )
        {
            index.slot += expand_size - current_size;
            index.dual += offset;
        }
        
        // If this is the key that is expanding, then it needs to become
        // a dual slot.
        if ( i->key == key )
        {
            assert( key );
            if ( is_number )
            {
                assert( index.dual == yslotindex::REFERENCE );
                index.dual = yslotindex::DUALSLOT + index.slot;
                index.slot = expand_size - expand->numslots;
            }
            else
            {
                assert( index.dual == yslotindex::NUMBER );
                index.dual = yslotindex::DUALSLOT + expand->refslots - 1;
            }
            
            made_dual = true;
        }
        
        expand->lookup.insert( i->key, index );
    }

    if ( key )
    {
        // Add key, if it wasn't a key that expanded.
        if ( ! made_dual )
        {
            yslotindex index;
            if ( is_number )
            {
                index.slot = expand_size - expand->numslots;
                index.dual = yslotindex::NUMBER;
            }
            else
            {
                index.slot = expand->refslots - 1;
                index.dual = yslotindex::REFERENCE;
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
        yslotindex index( 0, yslotindex::REFERENCE );
        expand->lookup.insert( key, index );
        expand->is_prototype = true;
    }
    
    
    // Remember new class.
    expandtable.insert( key, expand );
    return expand;
}


#else


yclass* ymodel::expand_class( yclass* klass, ysymbol key )
{
    std::lock_guard< std::mutex > lock( expand_mutex );
    
    // Check for existing class.
    auto lookup = klass->expand.lookup( key );
    if ( lookup )
    {
        return lookup->value;
    }


    // Build new class.
    yclass* expand = yclass::alloc();
    expand->prototype = klass->prototype;
    

    // If key is null then we're allocating slot 0 for the prototype.
    size_t offset = key ? 0 : 1;

    
    // Set up new class's layout.
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
    return expand;
}


#endif




void ymodel::mark_grey( yobject* object )
{
    // Currently, mutators can only perform the unmarked -> grey transition
    // with the mutex held.  This means more locking for mutators, but means
    // that if the mark thread has marked all reachable objects, locks, and
    // finds the grey list empty, it knows that marking is complete.
    std::lock_guard< std::mutex > lock( greylist_mutex );
    
    // Pass in the actual mark colour in case somehow
    object->mark( &greylist, yscope::scope->mark_colour );
}



/*
    yscope
*/


__thread yscope* yscope::scope = nullptr;



yscope::yscope( ymodel* model )
    :   previous( scope )
    ,   model( model )
    ,   mark_colour( Y_GREEN ) // TEMP - ask model for current colour
    ,   allocs( nullptr )
    ,   stack( new ystack() )
{
    // TODO: register scope with model.

    scope = this;
}

yscope::~yscope()
{
    // TODO: dump pending allocs.
    // TODO: deregister with model.
    
    assert( scope == this );
    scope = previous;
    
    delete stack;
}





