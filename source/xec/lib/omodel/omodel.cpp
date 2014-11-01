//
//  omodel.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "omodel.h"
#include "omodel_impl.h"
#include <intmath.h>
#include "ovalue.h"
#include "oexpand.h"



/*
    Both the mutator and the marker threads are marking objects with the
    current mark colour.  Newly allocated objects are born already marked.
    
    The mutator's write barrier acts on unmarked objects when an object is
    modified.  The object containing the slot is marked in its entirety.  This
    means we always mark objects using their state at the GC snapshot (since
    objects may only be modified after they are marked).  This causes mutators
    to stall in order to do the mark, but many momentary stalls are better
    than one long pause.
    
    The marker thread works in the background, marking roots and following
    references from newly marked objects.
    
    
    The mark colours green/orange/purple are cycled through.  One colour is the
    current mark colour.  The next colour is for objects reachable at the
    last snapshot but not yet considered in this cycle: the unmarked colour.
    Objects of the final colour are unreachable (they were not reachable at
    the last snapshot time) that have not yet been swept.
 
    Marking state transitions work like this:
    
        unmarked -> marked   : for objects which have no children.
        unmarked -> pending  : when objects are added to the work list.
        
        pending  -> marking  : either the mutator or collector can mark.
        marking  -> marked   : uncontended mark completion.
 
        marking  -> **locked : if a mutator encounters a marking object.
        **locked -> marked   : accompanied by a broadcast on the condvar.
    
    Colour updates are atomic, but transitions in and out of the locked state
    are additionally restricted - they must only be performed with the marking
    mutex held.  This ensures that mutator threads cannot miss the mark
    completion broadcast.
    
    There is no ABA problem as in each collection epoch each object is
    coloured somewhere along the following progression, with no way to move
    backwards along the chain:

                unmarked -> pending -> marking -> locked -> marked

*/


namespace oimpl
{

__thread ocontext* context;


omodel::omodel()
{
}

omodel::~omodel()
{
}


void omodel::new_object( ocontext* context, ogctype* type, ogcbase* object )
{
    // It begins life already marked.
    uintptr_t marked_word = (uintptr_t)type | context->marked_colour;
    object->gctype_colour.store( marked_word, std::memory_order_relaxed );

    // Add to alloc list.
    std::lock_guard< std::mutex > lock( allocs_mutex );
    object->gcnext = allocs;
    allocs = object;
}

ostring* omodel::symbol( ocontext* context, const char* s )
{
    std::lock_guard< std::mutex > lock( symbols_mutex );

    // Look up symbol.
    size_t size = strlen( s );
    hash32_t hash = hash32( s, size );
    symkey key( hash, s, size );
    auto symbol = symbols.lookup( key );
    if ( symbol )
    {
        // Mark symbol before returning, in case we are resurrecting it.
        symbol->value->gcmark( context->marked_colour );
        return symbol->value;
    }

    // Create new symbol.
    ostring* newsym = ostring::alloc( s );
    newsym->shash = hash;
    newsym->shashed = true;
    newsym->ssymbol = true;
    symbols.insert( key, newsym );
    return newsym;
}

ostring* omodel::symbol( ocontext* context, ostring* s )
{
    std::lock_guard< std::mutex > lock( symbols_mutex );
    
    // Should already have checked if it's already a symbol.
    assert( ! s->ssymbol );

    // Look up symbol
    symkey key( s->hash(), s->data(), s->size() );
    auto symbol = symbols.lookup( key );
    if ( symbol )
    {
        // Mark symbol before returning, in case we are resurrecting it.
        symbol->value->gcmark( context->marked_colour );
        return symbol->value;
    }
    
    // Turn this string itself into a symbol and return it.
    assert( ! s->ssymbol );
    s->ssymbol = true;
    symbols.insert( key, s );
    return s;
}



ocontext* omodel::new_context()
{
}

void omodel::delete_context( ocontext* context )
{
}

void omodel::collect()
{
}

void omodel::safepoint()
{
}



void omodel::barrier( ocontext* context, ogcbase* object, uintptr_t word )
{
    ocolour marked_colour = context->marked_colour;
    ocolour unmarked_colour = context->unmarked_colour;

    ogctype* gctype = (ogctype*)( word & ~COLOUR );
    ocolour colour = (ocolour)( word & COLOUR );

    // Attempt unmarked/pending -> marking.
    while ( colour == unmarked_colour || colour == PENDING )
    {
        uintptr_t marking_word = (uintptr_t)gctype | MARKING;
        if ( ! object->gctype_colour.compare_exchange_weak(
                    word, marking_word, std::memory_order_relaxed ) )
        {
            // Could fail due to other threads performing marking.
            colour = (ocolour)( word & COLOUR );
            continue;
        }
        
        
        // Success, mark object.
        oworklist work;
        gctype->mark( object, &work, marked_colour, unmarked_colour );
        add_roots( &work );
        
        
        // Attempt marking -> marked.
        word = marking_word;
        uintptr_t marked_word = (uintptr_t)gctype | marked_colour;
        if ( object->gctype_colour.compare_exchange_strong(
                    word, marked_word, std::memory_order_relaxed ) )
        {
            // Marked, complete.
            return;
        }
        
        
        // Another thread must have performed a marking -> locked transition.
        colour = (ocolour)( word & COLOUR );
        assert( colour == LOCKED );


        // Perform locked -> marked transition and broadcast.
        std::lock_guard< std::mutex > lock( locked_mutex );
        object->gctype_colour.store( marked_word, std::memory_order_relaxed );
        locked_condition.notify_all();
        
        
        return;
    }
    
    
    // Another thread has beat us to marking this object.
    if ( colour == MARKING || colour == LOCKED )
    {
        // Obtain the mutex to ensure we see the locked -> marked broadcast.
        std::unique_lock< std::mutex > lock( locked_mutex );
        
        if ( colour == MARKING )
        {
            // Perform marking -> locked transition.  CAS is still required
            // even with the mutex, as another thread can do marking -> marked
            // without locking the mutex.
            uintptr_t locked_word = (uintptr_t)gctype | LOCKED;
            object->gctype_colour.compare_exchange_strong(
                            word, locked_word, std::memory_order_relaxed );
            
            // If CAS succeeded, we're now locked.  If it failed, either
            // another thread did marking -> locked before we obtained the
            // mutex, the entire sequence marking -> locked -> marked occured
            // before we obtained the mutex, or the thread performing the mark
            // managed marking -> marked before the CAS.  In all cases we are
            // now either locked or marked.
            colour = (ocolour)( word & COLOUR );
        }
        else
        {
            // We were locked already.  Reload colour now we have the mutex,
            // in case we are already unlocked and marked.
            word = object->gctype_colour.load( std::memory_order_relaxed );
            colour = (ocolour)( word & COLOUR );
        }
        
        
        // Here, we're either locked or already marked.  Wait for unlock.
        while ( colour == LOCKED )
        {
            locked_condition.wait( lock );
            word = object->gctype_colour.load( std::memory_order_relaxed );
            colour = (ocolour)( word & COLOUR );
        }
    }
    
    
    // Must now be marked.
    assert( colour == marked_colour );
}


void omodel::add_roots( oworklist* work )
{
    std::lock_guard< std::mutex > lock( roots_mutex );
    if ( roots.size() < work->size() )
        roots.swap( *work );
    for ( auto i = work->begin(); i != work->end(); ++i )
        roots.push_back( *i );
}




void omodel::marker()
{
    


}



void omodel::sweeper()
{
}







void omodel::expand_key(
                ocontext* context, oexpand* expand, osymbol key, ovalue value )
{
    // Locate current expand class.
    oexpandclass* oldclass = expand->expand->expandclass;
    if ( oldclass == nullptr )
    {
        oldclass = expand_empty;
    }
    
    
    // Note that property updates are NOT thread-safe - this lock ensures that
    // the expandclass structures remain consistent, but nothing more.
    std::lock_guard< std::mutex > lock( expand_mutex );
    

    // Find expandclass to update to.
    oexpandclass* newclass = nullptr;
    auto lookup = oldclass->children.lookup( key );
    if ( lookup )
    {
        // Already exists.
        newclass = lookup->value;
    }
    else
    {
        // Requires creation.
        newclass = alloc< oexpandclass >( context, &oexpandclass::gctype );
        
        newclass->prototype = oldclass->prototype;
        newclass->capacity  = ceil_pow2( oldclass->size + 1 );
        newclass->size      = oldclass->size + 1;
        for ( auto i = oldclass->lookup.begin();
                        i != oldclass->lookup.end(); ++i )
        {
            newclass->lookup.insert( i->key, i->value );
        }
        newclass->lookup.insert( key, oldclass->size );
        
        newclass->refcount      = 0;
        newclass->parent        = oldclass;
        newclass->parent_key    = key;
    }
    
    
    // Incref newclass.
    newclass->refcount += 1;
    
    
    // Switch expandclass.  This is the non-thread-safe bit - property updates
    // in other threads could be accessing expanddata while we modify it.
    if ( oldclass->capacity == newclass->capacity )
    {
        // No need to reallocate expanddata.
        expand->expand->expandclass = newclass;
        expand->expand->properties[ oldclass->size ] = value;
    }
    else
    {
        // Build new expanddata.
        oexpanddata* olddata = expand->expand;
        oexpanddata* newdata = (oexpanddata*)malloc(
            sizeof( oexpanddata ) + sizeof( ovalue ) * newclass->capacity );

        newdata->expandclass = newclass;
        for ( size_t i = 0; i < oldclass->size; ++i )
        {
            newdata->properties[ i ] = olddata->properties[ i ];
        }
        newdata->properties[ oldclass->size ] = value;
        
        expand->expand = newdata;
    }
    
    
    // Decref oldclass, possibly unlinking it so it can be garbage collected.
    oldclass->refcount -= 1;
    if ( oldclass->refcount == 0 )
    {
        oldclass->parent->children.remove( oldclass->parent_key );
        
    }
    
}


}



ocontext::ocontext( omodel* model )
{
    assert( oimpl::context == nullptr );
    oimpl::context = model->p->new_context();
}

ocontext::~ocontext()
{
    oimpl::context->model->delete_context( oimpl::context );
}

void ocollect()
{
    oimpl::context->model->collect();
}

void osafepoint()
{
    oimpl::context->model->safepoint();
}

ogcbase::~ogcbase()
{
}

void ogcbase::write_barrier_real( uintptr_t word )
{
    oimpl::context->model->barrier( oimpl::context, this, word );
}










