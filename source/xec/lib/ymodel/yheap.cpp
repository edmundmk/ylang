//
//  yheap.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yheap.h"
#include "yobject.h"
#include "ystring.h"



__thread ocontext* ocontext::context = nullptr;


ostring* oheap::make_symbol( const char* s )
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
        symbol->second->mark( nullptr, ocontext::context->mark_colour );
        return symbol->second.get();
    }

    // Create new symbol.
    ostring* newsym = ostring::alloc( s );
    newsym->shash = hash;
    newsym->shashed = true;
    newsym->ssymbol = true;
    symbols.emplace( key, newsym );
    return newsym;
}


ostring* oheap::make_symbol( ostring* s )
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
        symbol->second->mark( nullptr, ocontext::context->mark_colour );
        return symbol->second.get();
    }
    
    // Turn this string itself into a symbol and return it.
    assert( ! s->ssymbol );
    s->ssymbol = true;
    symbols.emplace( key, s );
    return s;
}



void oheap::mark_grey( obase* object )
{
    // Currently, mutators can only perform the unmarked -> grey transition
    // with the mutex held.  This means more locking for mutators, but means
    // that if the mark thread has marked all reachable objects, locks, and
    // finds the grey list empty, it knows that marking is complete.
    std::lock_guard< std::mutex > lock( greylist_mutex );
    
    // Pass in the actual mark colour in case somehow
    object->mark( &greylist, ocontext::context->mark_colour );
}





