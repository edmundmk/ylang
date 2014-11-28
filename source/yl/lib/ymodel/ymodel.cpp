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
    The collector has three colours, plus grey.  In each collection epoch,
    one colour represents dead objects, the next, unmarked objects, and the
    last, marked objects.  Grey objects are live and have been added to the
    grey list, but have not yet been marked.
    
    At the start of an epoch, a snapshot is taken.  Mutators are paused and
    local roots are added to the work list.
    
    Mutators are restarted and do work.  The write barrier marks or greys
    the original value of updated references.  This ensures that all objects
    reachable at the snapshot time are marked.  New objects are created using
    the current colour.
    
    The mark thread goes through the work list, or if the work list is empty,
    the grey list, and marks all reachable objects.  Once both lists are
    empty, marking is done.  It is guaranteed to read - for each reference -
    a valid value at least as new as the snapshot.
    
    The sweep thread goes through the entire heap and sweeps dead objects.
    
    Then the colours are cycled and the next epoch begins.
    
    
    e.g.
    
        this epoch:
    
            dead        green
            unmarked    purple
            marked      orange
        
        sweep thread finishes, no more green objects in heap.
        mark thread finishes, all live objects are purple.
        
        next epoch:
 
            dead        purple (unreachable in last epoch)
            unmarked    orange
            marked      green
            
        and we continue.
    
        
    Marking an object grey and adding it to the grey list is performed inside
    a critical section.  This ensures that when the marker is done and the
    grey list is empty, there are no reachable objects waiting to be marked.
    
 
    Because there is no sychronization around local reference updates,
    handshaking at safe points is required to communicate locals to the GC.
 
    Entering a safe period means:
        All heap updates to this point must be visible to the GC.
        All local references must be in known locations visible to the GC.
        This means a release fence, at least.
        
    During a safe period:
        Neither heap or local references can be modified.
        
    Exiting a safe period means:
        If the GC has begun marking this mutator's locals, wait for this
            to complete before continuing.
        After a handshake, take on the colours of the new epoch.
    

    We do not stop all mutator threads at the same time.  Each mutator is
    stopped one after another.  This means mutator threads do not block
    waiting for other mutator threads.  But it also means that different
    threads enter the new epoch at different times.
 
              |           green -> purple                   |
        A  ---|---||----------------------------------------|---
              |                                             |
              |   orange -> green        green -> purple    |
        B  ---|---------------------||----------------------|---
              |                                             |
 
    At the left barrier, the GC begins a new epoch.  All purple objects in the
    heap have been swept and all reachable objects are green.  At the right
    barrier, the GC has taken a snapshot of all locals, and heap marking
    begins.

    As long as B treats both green and purple objects as marked (i.e. it only
    changes the colour of orange objects) then both A in the new epoch and
    B in the previous epoch can run concurrently without problem.  Nothing is
    creating new orange objects - in fact there are no reachable orange objects
    for B to mark.
    
    However if there are mutable heap slots accessible to both A and B (or if
    a reference can otherwise travel from B to A) then live objects can 'slip
    through the cracks'.  A simple example:
    
        A performs its handshake.
        A copies a reference to an object O from the heap onto its stack.
        B clears the heap reference.  O is not purple and so is not marked.
 
    If that heap reference was the last heap reference to O, then the only
    remaining reference is in A's stack.  O is live, but it will not be
    marked, as the GC will not revisit A's stack until the next epoch.
    
    Sliding-views collectors solve this problem by performing multiple
    handshakes per epoch.  However a ylang environment is unlikely to be
    multithreaded (yexpands are not in general thread-safe), so complicating
    each GC cycle for a situation which will probably never arise is a bad
    tradeoff.
    
    Instead we add a kind of lock called a yguard.  Multiple mutators can
    be inside a yguard at the same time - a yguard does not provide
    synchronisation.  Entering a yguard merely ensures that this mutator has
    entered the new epoch (if the new epoch has begun).  Also, no other
    mutator will enter a _later_ epoch while the yguard is held.
    
    Notice that the problem arises because B cleared a reference accessible
    to A without marking it orange.  If the same sequence occurs _after_ B
    enters the new epoch, then everything is fine.
    
    Correct multithreaded programming already requires careful locking, so
    asking programmers to guard shared references is not unreasonable.
 
 
    Both yclasses and ysymbols are immutable once created (at least currently),
    and live under locks under the control of the ymodel itself, so handling
    them correctly is somewhat simpler.
 
*/





/*
    ymodel
*/

ymodel::ymodel()
    :   collection_request( false )
    ,   destroy_request( false )
    ,   countdown( 0 )
    ,   guard_epoch( 0 )
    ,   live_guard_count( 0 )
    ,   guard_count( 0 )
    ,   scope_epoch( 0 )
    ,   dead( Y_GREEN )
    ,   unmarked( Y_PURPLE )
    ,   marked( Y_ORANGE )
    ,   sweep_queue{ Y_GREY, Y_GREY }
    ,   objects_head( nullptr )
    ,   objects_last( nullptr )
{

    // Kick off collector threads.
    mark_thread  = std::move( std::thread( &ymodel::mark, this ) );
    sweep_thread = std::move( std::thread( &ymodel::sweep, this ) );


    // Construct expandclasses.
    make_expands();

}

ymodel::~ymodel()
{
    std::unique_lock< std::mutex > lock( collector_mutex );
    destroy_request = true;
    wait_for_request.notify_all();
    lock.unlock();
    
    mark_thread.join();
    sweep_thread.join();
    
    yobject* object = objects_head;
    while ( object )
    {
        yobject* next = object->get_next();
        free( object );
        object = next;
    }
}



void ymodel::make_expands()
{
    // These objects are allocated so we need to enter a yscope on ourselves.
    yscope scope( this );
    
    // Construct prototypes for basic ylang types.
    expand_empty_class      = yclass::alloc();
    expand_object_proto     = yexpand::make_proto();
    expand_array_proto      = yarray::make_proto();
    expand_table_proto      = ytable::make_proto();
    expand_boolean_proto    = ystandin::alloc( ystandin::BOOLEAN );
    expand_number_proto     = ystandin::alloc( ystandin::NUMBER );
    expand_string_proto     = ystandin::alloc( ystandin::STRING );
    expand_function_proto   = ystandin::alloc( ystandin::FUNCTION );
}



yexpand* ymodel::make_global()
{
    // Construct a global scope containing prototypes for ylang types.
    yexpand* global = yexpand::alloc();
    global->setkey( "object", expand_object_proto );
    global->setkey( "array", expand_array_proto );
    global->setkey( "table", expand_table_proto );
    global->setkey( "boolean", expand_boolean_proto );
    global->setkey( "number", expand_number_proto );
    global->setkey( "string", expand_string_proto );
    global->setkey( "function", expand_function_proto );
    return global;
}






yclass* ymodel::empty_class()
{
    return expand_empty_class;
}


yexpand* ymodel::object_proto()
{
    return expand_object_proto;
}

yexpand* ymodel::array_proto()
{
    return expand_array_proto;
}

yexpand* ymodel::table_proto()
{
    return expand_table_proto;
}

ystandin* ymodel::boolean_proto()
{
    return expand_boolean_proto;
}

ystandin* ymodel::number_proto()
{
    return expand_number_proto;
}

ystandin* ymodel::string_proto()
{
    return expand_string_proto;
}

ystandin* ymodel::function_proto()
{
    return expand_function_proto;
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
        symbol->second->mark_wb();
        return symbol->second.get();
    }

    // Create new symbol.
    ystring* newsym = ystring::alloc( s );
    newsym->shash = hash;
    newsym->shashed = true;
    newsym->ssymbol = true;
    symbols.emplace( symkey( hash, newsym->c_str(), size ), newsym );
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
        // Mark symbol before returning, in case we are resurrecting it.
        symbol->second->mark_wb();
        return symbol->second.get();
    }
    
    // Turn this string itself into a symbol and return it.
    assert( ! s->ssymbol );
    s->ssymbol = true;
    symbols.emplace( key, s );
    return s;
}




void ymodel::collect()
{
    std::lock_guard< std::mutex > lock( collector_mutex );
    collection_request = true;
    wait_for_request.notify_all();
}




void ymodel::mark_grey( yobject* object )
{
    // Currently, mutators can only perform the unmarked -> grey transition
    // with the mutex held.  This means more locking for mutators, but means
    // that if the mark thread has marked all reachable objects, locks, and
    // finds the grey list empty, it knows that marking is complete.

    std::lock_guard< std::mutex > lock( greylist_mutex );
    object->mark_ref( &greylist, yscope::scope->marked );
}






unsigned ymodel::lock_guard()
{
    std::unique_lock< std::mutex > lock( collector_mutex );
    
    yscope* scope = yscope::scope;
    assert( scope->state == yscope::UNSAFE );
    
    
    // This guard was locked for the guard epoch.
    scope->state = yscope::GUARD;
    unsigned guard_epoch = this->guard_epoch;
    guard_count += 1;
    

    if ( scope->epoch != guard_epoch )
    {
        // This thread is behind the guard epoch.  Wait until all previous
        // guards have released and we have entered the new epoch.
        while ( ! check_epoch( guard_epoch ) )
        {
            wait_for_epoch.wait( lock );
        }
        
        // Mark.
        mark_locals( lock, scope );
    }
    
    
    return guard_epoch;
}


void ymodel::unlock_guard( unsigned guard_epoch )
{
    std::unique_lock< std::mutex > lock( collector_mutex );

    yscope* scope = yscope::scope;
    assert( scope->state == yscope::GUARD );


    // Release guard.
    if ( this->guard_epoch == guard_epoch )
    {
        // Guard released during current epoch.
        assert( guard_count );
        guard_count -= 1;
    }
    else
    {
        // We are entering new epoch and waiting for this guard to release.
        assert( this->guard_epoch == guard_epoch + 1 );
        assert( live_guard_count );
        live_guard_count -= 1;

        // If all guards have released, wake collector threads.
        if ( live_guard_count == 0 )
        {
            wait_for_guard.notify_all();
        }
    }
    
    
    // Back to unsafe.
    scope->state = yscope::UNSAFE;
}



void ymodel::safe()
{
    std::unique_lock< std::mutex > lock( collector_mutex );

    yscope* scope = yscope::scope;
    assert( scope->state == yscope::UNSAFE );
    
    // If we're behind the epoch, mark.
    if ( ! check_epoch( scope->epoch ) )
    {
        mark_locals( lock, scope );
    }
    
    // Now safe.
    scope->state = yscope::SAFE;
}


void ymodel::unsafe()
{
    std::unique_lock< std::mutex > lock( collector_mutex );

    yscope* scope = yscope::scope;
    assert( scope->state != yscope::UNSAFE );
    
    // Wait for any in-progress marking to complete.
    while ( scope->state == yscope::MARKING )
    {
        wait_for_handshake.wait( lock );
    }
    
    // Now unsafe again.
    scope->state = yscope::UNSAFE;
}


void ymodel::safepoint()
{
    std::unique_lock< std::mutex > lock( collector_mutex );

    yscope* scope = yscope::scope;
    assert( scope->state == yscope::UNSAFE );
    
    // Mark.
    assert( ! check_epoch( scope->epoch ) );
    mark_locals( lock, scope );
}




void ymodel::add_grey( yworklist* worklist )
{
    std::lock_guard< std::mutex > lock( greylist_mutex );
    if ( greylist.empty() )
    {
        greylist.swap( *worklist );
    }
    else
    {
        while ( worklist->size() )
        {
            greylist.push_back( worklist->back() );
            worklist->pop_back();
        }
    }
}


void ymodel::mark_locals( std::unique_lock< std::mutex >& lock, yscope* scope )
{
    // Lock is held, now marking this scope.
    yscope::markstate state = scope->state;
    scope->state = yscope::MARKING;
    
    lock.unlock();
    
    
    // Mark locals on stack (can happen in parallel with other thread's marks).
    yworklist worklist;
    ystack* stack = scope->stack;
    for ( size_t i = 0; i < stack->mark; ++i )
    {
        yvalue value = stack->stack.at( i );
        if ( value.is_object() )
        {
            value.as_object()->mark_obj( &worklist, unmarked, marked );
        }
    }
    
    
    // Add marked objects to grey list.
    add_grey( &worklist );
    
    
    lock.lock();
    
    // Now marked with the right epoch.
    scope->epoch = this->scope_epoch.load( std::memory_order_relaxed );
    assert( scope->state == yscope::MARKING );
    scope->state = state;
    
    // One of the threads the marker was waiting on has handshaked.
    assert( countdown );
    countdown -= 1;
    
    // Wake threads that were waiting on this thread being marked.
    wait_for_handshake.notify_all();
    
}




void ymodel::mark()
{
    yworklist worklist;


    while ( true )
    {

    std::unique_lock< std::mutex > lock( collector_mutex );
    
    
    // Wait for GC request.
    while ( ! collection_request && ! destroy_request )
    {
        wait_for_request.wait( lock );
    }
    
    if ( destroy_request )
    {
        return;
    }
    
    collection_request = false;

    
    // Wait for sweep of objects of current dead colour to complete.  This
    // colour will be reused as the marked colour in the current epoch.
    while ( sweep_queue[ 0 ] == dead || sweep_queue[ 1 ] == dead )
    {
        wait_for_sweep.wait( lock );
    }
    assert( sweep_queue[ 1 ] == Y_GREY );


    // At this point, any new guards lock the new epoch (and will pause their
    // thread until the handshake).  All existing guards must unlock before we
    // can enter the new epoch and begin handshaking.
    guard_epoch += 1;

    assert( live_guard_count == 0 );
    live_guard_count = guard_count;
    guard_count = 0;
    
    while ( live_guard_count )
    {
        wait_for_guard.wait( lock );
    }


    // Cycle mark colours for new epoch.
    ycolour prev_dead = dead;
    dead     = unmarked;
    unmarked = marked;
    marked   = prev_dead;


    // Enter new epoch and request handshake with mutator threads.
    this->scope_epoch.store( guard_epoch, std::memory_order_relaxed );
    countdown = scopes.size();
    wait_for_epoch.notify_all();


    // Mark all safe threads.  If any threads become safe while we are marking
    // another then they should mark themselves.
    for ( auto i = scopes.begin(); i != scopes.end(); ++i )
    {
        yscope* scope = *i;
        if ( scope->state == yscope::SAFE && scope->epoch != guard_epoch )
        {
            mark_locals( lock, scope );
        }
    }
    
    
    // Wait for all remaining threads to mark.
    while ( countdown )
    {
        wait_for_handshake.wait( lock );
    }
    

    lock.unlock();
    
    
    // Perform actual marking.
    do
    {
        // Mark all unmarked objects reachable from work list.
        while ( worklist.size() )
        {
            yobject* object = worklist.back();
            worklist.pop_back();
            object->mark_obj( &worklist, unmarked, marked );
        }
        
        
        // Add objects greyed by write barriers to work list.
        std::lock_guard< std::mutex > lock( greylist_mutex );
        assert( worklist.empty() );
        worklist.swap( greylist );
    
    }
    while ( worklist.size() );


    lock.lock();


    // Kick off sweep of unmarked objects (which are unreachable), and will
    // be dead in the next epoch.  We begin sweeping as soon as possible
    // after marking - this sweep is actually the sweep from the next epoch.
    if ( sweep_queue[ 0 ] == Y_GREY )
        sweep_queue[ 0 ] = unmarked;
    else
        sweep_queue[ 1 ] = unmarked;
    wait_for_request.notify_all();


    }
    
}




void ymodel::sweep()
{
    while ( true )
    {
    
    std::unique_lock< std::mutex > lock( collector_mutex );

    
    // Wait for sweep request.
    while ( sweep_queue[ 0 ] == Y_GREY && ! destroy_request )
    {
        wait_for_request.wait( lock );
    }
    
    if ( destroy_request )
    {
        return;
    }
    
    ycolour dead = sweep_queue[ 0 ];


    lock.unlock();
    
    
    
    // Sweep dead objects.
    auto objects = get_objects();
    yobject* head = objects.first;
    yobject* prev = nullptr;
    yobject* curr = head;

    while ( curr )
    {
        yobject* next = curr->get_next();


        // If object is live, leave it in list.
        if ( curr->get_colour() != dead )
        {
            prev = curr;
            curr = next;
            continue;
        }
        
        
        // Remove symbols from symbol table.
        if ( curr->is< ystring >() && ( (ystring*)curr )->ssymbol )
        {
            std::lock_guard< std::mutex > lock( symbols_mutex );
            ystring* s = (ystring*)curr;
            
            // Might have been resurrected before we held the lock.
            if ( s->get_colour() != dead )
            {
                prev = curr;
                curr = next;
                continue;
            }
            
            // Remove from symbol table.
            symbols.erase( symkey( s->shash, s->sdata, s->ssize ) );
            
            // Fall through to destroy object.
        }
        
        
        // Free dead object.
        free( curr );


        // Move to next.
        if ( prev )
            prev->set_next( next );
        else
            head = next;
        curr = next;
    }
    
    if ( prev )
    {
        prev->set_next( nullptr );
    }
    
    
    // Add unswept objects back to list.
    add_objects( head, prev );
    

    lock.lock();
    

    // Done.
    assert( sweep_queue[ 0 ] == dead );
    sweep_queue[ 0 ] = sweep_queue[ 1 ];
    sweep_queue[ 1 ] = Y_GREY;
    wait_for_sweep.notify_all();
    
    
    }


}



std::pair< yobject*, yobject* > ymodel::get_objects()
{
    std::lock_guard< std::mutex > lock( objects_mutex );
    std::pair< yobject*, yobject* > objects( objects_head, objects_last );
    objects_head = nullptr;
    objects_last = nullptr;
    return objects;
}


void ymodel::add_objects( yobject* head, yobject* last )
{
    std::lock_guard< std::mutex > lock( objects_mutex );
    assert( last->get_next() == nullptr );
    if ( objects_head )
    {
        assert( objects_last );
        objects_last->set_next( head );
        objects_last = last;
    }
    else
    {
        assert( objects_last == nullptr );
        objects_head = head;
        objects_last = last;
    }
}





/*
    yscope
*/


__thread yscope* yscope::scope = nullptr;



yscope::yscope( ymodel* model )
    :   previous( scope )
    ,   model( model )
    ,   epoch( 0 )
    ,   state( UNSAFE )
    ,   unmarked( Y_GREEN )
    ,   marked( Y_PURPLE )
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





/*
    Safepoints
*/

void ysafe()
{
    yscope::scope->model->safe();
}

void yunsafe()
{
    yscope::scope->model->unsafe();
}



/*
    Request collection
*/

void ycollect()
{
    yscope::scope->model->collect();
}



