//
//  gc.cpp
//
//  Created by Edmund Kapusniak on 04/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "gc.h"
#include <list>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <intmath.h>
#include <delegate.h>



/*
    This is a refcounting snapshot collector, with a corresponding cycle
    collector, based on the ideas in:

        An On-the-Fly Reference Counting Garbage Collector for Java
            Levanoni and Petrank, 2001
            
        An Efficient On-the-Fly Cycle Collection
            Paz, Bacon, Kolodner, Petrank, and Rajan, 2003
            
    Unlike the on-the-fly collectors described, my collector does stop the
    world briefly during the handshake - we operate not on a sliding view
    of the heap but on a heap snapshot from the time of the handshake.  This
    makes implementation much simpler.  We can avoid the need to clear dirty
    flags on slots by switching between two different dirty 'colours'.
    
    A cycle collector also operates on this snapshot.
    
    
    Instead of marking whole objects dirty, individual slots are marked.  This
    is because we want this to be implementable in C++ without too much hassle.
    Trying to find the object containing a particular slot is possible (we
    could template slots with an offset) but tedious and difficult to set up.
 
    Slots have four colours, stored in the bottom four bits of the pointer:
    
        GREEN slots are clean and unmodified since the mark phase cleared them.
        PURPLE or ORANGE slots are either clean (they were dirty in the
            previous cycle) or dirty (slots are being marked this colour
            in this cycle).  Each cycle, PURPLE and ORANGE switch roles.
        BLACK slots are for immutable slots which should not participate
            in garbage collection.  This is useful for xoi data images.
 
    
    The slot write barrier checks for clean slots and stores the clean value
    in a thread-local slot buffer before updating the slot with the new (dirty)
    value.
    
 
    A garbage collection cycle proceeds like this:
    
        RUNNING
            mutators mutate, dirtying slots.
            all heap slots are either GREEN or new dirty.
        PAUSING
            a collection has been requested.
            mutators enter the handshake at their next safepoint.
            mutators which have entered the handshake:
                submit their slot buffers to the collector.
                begin marking locals.
            other mutators continue mutating, marking slots dirty.
        STOPPED
            all mutators have entered the handshake.
            from this point it is impossible for a slot to
                become the (now old) dirty colour.
            global roots are marked.
        MARKING
            mutators are free to resume mutating, but with new dirty colour.
            some mutators may still be marking locals.
            updated slots are those in the submitted buffers.
            the slot value at the snapshot is searched for:
                in the heap, if the slot is still the old dirty colour.  in
                    this case, the slot is atomically made GREEN instead.
                in the current slot buffers, if a mutator has
                    already dirtied it this cycle.
            refcounts are updated.
        RUNNING
            entering this stage all heap slots are either GREEN or marked with
                the new dirty colour.  all old dirty slots were made GREEN.
            all mutators have finished marking locals.
            collector begins sweeping objects given new refcounts and marks.
            collector then waits until next GC cycle is requested.
 
 
    Most reference updates will be in new objects, and new objects must be
    treated as candidates for cycle detection.  To reduce traffic on the slot
    buffers, newly allocated objects are born with their slots already marked
    dirty.  Increments for these slots are generated on the first collection
    after the object is allocated.
    
    
    The cycle collector has as its candidates for cycle roots:
        all objects marked as local or global roots in the last snapshot.
            (may no longer be roots in this snapshot - effectively a decref).
        all objects allocated since the previous collection.
            (can form dead cycles without decref).
        any objects decrefed but not dead during the current collection.
            (including those decrefed due to the sweeping of other objects).
        
    But can exclude from its set:
        objects we can prove to be acyclic using the type system.
            (most trivially, objects with no slots at all).
        objects marked as roots in the current snapshot.
            (effectively have an extra reference outside of any cycle).
        objects with slots dirtied after the snapshot.
            (it must be live if a mutator can reach it).
 
 
*/




namespace gc_impl
{


/*
    Per-thread GC context.
*/

__thread gc_context* context = nullptr;


gc_context::gc_context()
    :   colour( BLACK )
    ,   heap( nullptr )
    ,   autos( nullptr )
    ,   markers( nullptr )
    ,   slots( nullptr )
    ,   mark( nullptr )
    ,   allocs( nullptr )
    ,   handshake( false )
{
}



/*
    The header for GC objects is laid out before the object like this:
 
            size_t      length;     // only if refcount->isarray == true
            gc_type*    type;       // only if refcount->hastype == true
            gc_refcount refcount;
            -------------
            ...
*/

static const int INTPTR_BITS = sizeof( intptr_t ) * 8;


struct gc_refcount
{
    intptr_t    refcount    : INTPTR_BITS - 4;
    bool        marked      : 1;
    bool        isarray     : 1;
    bool        hastype     : 1;
    bool        acyclic     : 1;
};


gc_refcount* gc_refcountof( void* p )
{
    return (gc_refcount*)( (void**)p - 1 );
}

gc_type* gc_typeof( void* p )
{
    gc_refcount* objref = gc_refcountof( p );
    if ( objref->hastype )
    {
        return *(gc_type**)( (void**)p - 2 );
    }
    else
    {
        return NULL;
    }
}

size_t gc_lengthof( void* p )
{
    gc_refcount* objref = gc_refcountof( p );
    if ( objref->isarray )
    {
        if ( objref->hastype )
        {
            return *(size_t*)( (void**)p - 3 );
        }
        else
        {
            return *(size_t*)( (void**)p - 2 );
        }
    }
    else
    {
        return 1;
    }
}



/*
    GC heap and collection thread.
*/


class gc_heap_impl
{
public:

    gc_heap_impl();
    ~gc_heap_impl();
    
    void link_root( gc_root_impl* proot );
    void lose_root( gc_root_impl* proot );
    
    void link_context( gc_context* context );
    void lose_context( gc_context* context );
    
    void request_collection( gc_context* context );
    void refresh_slot_buffer( gc_context* context );
    void handshake( gc_context* context );
    
    
    
private:
    
    enum request
    {
        IDLE,       // Don't do anything.
        COLLECT,    // Run a GC collection.
        QUIT,       // Run a GC collection and quit.
    };
    
    enum state
    {
        RUNNING,    // Mutators run as normal.
        PAUSING,    // Mutators begin handshake at next safepoint.
        STOPPED,    // Collector marks roots.
        MARKING,    // Some mutators running, some still marking locals.
    };
    

    void collector();
    void update_slot( gc_colour dirty, gc_slot_impl* slot, uintptr_t oldvalue );
    void sweep( std::vector< void* >* dead, void* object );
    
    std::thread                     thread;

    std::mutex                      root_mutex;
    gc_root_impl                    root_list;

    std::mutex                      mutex;
    std::condition_variable         condition;

    request                         request;
    state                           state;
    gc_colour                       colour;
    std::list< gc_context* >        contexts;
    std::vector< gc_slot_buffer* >  full_slot_buffers;
    std::vector< gc_mark_buffer* >  abandoned_allocs;

    int                             countdown;
    std::vector< gc_slot_buffer* >  slot_buffers;
    std::vector< gc_mark_buffer* >  mark_buffers;
    
    std::unordered_set< void* >     zero_count_table;
    
    
};



/*
    GC scope.
*/

class gc_scope_impl
{
public:

    gc_scope_impl( gc_heap* h );
    ~gc_scope_impl();

    gc_heap_impl* get_heap();


private:

    gc_heap_impl*       heap;
    
};




/*
    Buffer holding original values of dirty slots.
*/

class gc_slot_buffer
{
public:

    struct record
    {
        gc_slot_impl*   slot;
        uintptr_t       value;
    };


    gc_slot_buffer();
    ~gc_slot_buffer();

    bool add( gc_slot_impl* slot, uintptr_t value );
    std::pair< bool, uintptr_t > find( gc_slot_impl* slot );
    void clear();

    record* begin();
    record* end();
    

private:

    static const size_t RECORD_COUNT = 128;

    std::atomic_size_t  count;
    record              records[ RECORD_COUNT ];

};



/*
    Buffer holding marked locals/roots.
*/

class gc_mark_buffer
{
private:

    struct chunk;


public:

    gc_mark_buffer();
    ~gc_mark_buffer();

    void add( void* p );
    void clear();

    class iterator
    {
    public:

        bool operator == ( const iterator& o ) const;
        bool operator != ( const iterator& o ) const;
        
        void* operator * () const;
        iterator& operator ++ ();
        
    
    private:
    
        friend class gc_mark_buffer;
    
        iterator( chunk* chunk, size_t index );
    
        chunk*          ichunk;
        size_t          index;

        
    };

    iterator begin();
    iterator end();


private:


    static const size_t CHUNK_SIZE = 128;

    struct chunk
    {
        chunk*  next;
        void*   elements[ CHUNK_SIZE ];
    };

    chunk*  first;
    chunk*  last;
    size_t  index;

};



/*
    Collect, alloc, free.
*/


void gc_collect()
{
    context->heap->request_collection( context );
}



void* gc_alloc( gc_type* type, size_t length )
{
    // Make sure header items have the expected sizes.
    assert( sizeof( size_t ) == sizeof( void* ) );
    assert( sizeof( gc_type* ) == sizeof( void* ) );
    assert( sizeof( gc_refcount ) == sizeof( void* ) );
    
    
    // Work out size of header and size of object data.
    size_t header_size = sizeof( void* );
    header_size += type != nullptr ? sizeof( void* ) : 0;
    header_size += length != 1 ? sizeof( void* ) : 0;
    header_size = align( header_size, type ? type->align : alignof( void* ) );


    // This is a safepoint.
    gc_safepoint();


    // Allocate object.
    size_t data_size = ( type ? type->size : sizeof( void* ) ) * length;
    
    void* p = malloc( header_size + data_size );
    void* object = (char*)p + header_size;
    
    
    // Fill in header.
    gc_refcount* objref = gc_refcountof( object );
    objref->refcount    = 0;
    objref->marked      = false;
    objref->isarray     = length != 1;
    objref->hastype     = type != nullptr;
    
    if ( objref->hastype )
    {
        *(gc_type**)( (void**)p - 2 ) = type;

        if ( objref->isarray )
        {
            *(size_t*)( (void**) p - 3 ) = length;
        }
    }
    else if ( length != 1 )
    {
        *(size_t*)( (void**)p - 2 ) = length;
    }


    // Add object to the alloc buffer.  The caller must ensure that slots
    // are initialized before the next safepoint.
    context->allocs->add( object );
    
    return object;
}

void gc_free( void* object )
{
    // Free object memory only - should be called as part of sweeping.
    gc_refcount* objref = gc_refcountof( object );
    gc_type* type = gc_typeof( object );

    size_t header_size = sizeof( void* );
    header_size += objref->hastype ? sizeof( void* ) : 0;
    header_size += objref->isarray ? sizeof( void* ) : 0;
    header_size = align( header_size, type ? type->align : alignof( void* ) );
    
    void* p = (char*)object - header_size;
    free( p );
}










/*
    GC heap and collection thread.
*/


gc_heap_impl::gc_heap_impl()
    :   state( RUNNING )
    ,   request( IDLE )
    ,   colour( PURPLE )
{
    // Root node list sentinel node.
    root_list.p     = nullptr;
    root_list.heap  = this;
    root_list.prev  = &root_list;
    root_list.next  = &root_list;
    
    // Start heap thread.
    thread = std::thread( &gc_heap_impl::collector, this );
}

gc_heap_impl::~gc_heap_impl()
{
    // Request collection and for the GC thread to quit.
    {
        std::unique_lock< std::mutex > lock( mutex );

        // There should be no active scopes and the root list should be empty.
        assert( contexts.empty() );
        assert( root_list.next = &root_list );
        
        // Make request.
        request = QUIT;
        condition.notify_all();
    }

    // Wait for GC thread to complete.
    thread.join();

    // All objects should be dead.
    assert( zero_count_table.empty() );
}


void gc_heap_impl::link_root( gc_root_impl* proot )
{
    std::lock_guard< std::mutex > lock( root_mutex );

    // Add to linked list of roots.
    assert( proot->heap == nullptr );
    proot->next = &root_list;
    proot->prev = root_list.next;
    proot->prev->next = proot;
    proot->next->prev = proot;
    proot->heap = this;
}

void gc_heap_impl::lose_root( gc_root_impl* proot )
{
    std::lock_guard< std::mutex > lock( root_mutex );

    // Remove from linked list of roots.
    assert( proot->heap == this );
    proot->prev->next = proot->next;
    proot->next->prev = proot->prev;
    proot->prev = nullptr;
    proot->next = nullptr;
    proot->heap = nullptr;
}


void gc_heap_impl::link_context( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );
    
    assert( context->heap == nullptr );
    assert( context->slots == nullptr );
    assert( context->allocs == nullptr );
    
    // Avoid in-progress handshake.
    while ( state != RUNNING && state != MARKING )
    {
        condition.wait( lock );
    }

    // Add context.
    context->colour = colour;
    context->heap   = this;
    context->slots  = new gc_slot_buffer();
    context->allocs = new gc_mark_buffer();
    contexts.push_back( context );
}

void gc_heap_impl::lose_context( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );

    assert( context->heap == this );
    assert( context->autos == nullptr );
    assert( context->markers == nullptr );

    // If we're pausing then we need to handshake to avoid deadlock.
    if ( state == PAUSING )
    {
        handshake( context );
    }

    // Remove context.
    assert( ! context->handshake.load( std::memory_order_relaxed ) );
    contexts.remove( context );

    // Submit in-progress slot buffer.
    full_slot_buffers.push_back( context->slots );
    context->slots = nullptr;
    
    // And the table of allocs.
    abandoned_allocs.push_back( context->allocs );
    context->allocs = nullptr;
}


void gc_heap_impl::request_collection( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );
    
    // Request collection.
    request = COLLECT;
    condition.notify_all();
    
    // Wait for next handshake.
    while ( state != PAUSING )
    {
        condition.wait( lock );
    }
    
    // Perform handshake.
    assert( context->handshake.load( std::memory_order_relaxed ) );
    handshake( context );
}


void gc_heap_impl::refresh_slot_buffer( gc_context* context )
{
    std::lock_guard< std::mutex > lock( mutex );

    // Slot buffer is full, submit the old one.
    full_slot_buffers.push_back( context->slots );
    context->slots = new gc_slot_buffer();
    
    // Request GC at next opportunity (slot updates are not safepoints).
    request = COLLECT;
    condition.notify_all();
}


void gc_heap_impl::handshake( gc_context* context )
{
    // Handshake entry.
    {
        std::lock_guard< std::mutex > lock( mutex );
    
        // Handshake should be in response to a request from the GC thread.
        assert( context->handshake.load( std::memory_order_relaxed ) );
        assert( state == PAUSING );

        // Add all newly allocated objects to the zero-count-table.
        for ( auto i = context->allocs->begin();
                        i != context->allocs->end(); ++i )
        {
            void* object = *i;
            assert( gc_refcountof( object )->refcount == 0 );
            zero_count_table.insert( object );
        }
        context->allocs->clear();

        // Submit slot buffer to be used in the refcount update stage.
        slot_buffers.push_back( context->slots );
        context->slots = new gc_slot_buffer();
        
        // Get mark buffer for local marking.
        context->mark = new gc_mark_buffer();
        
        // This thread has paused at the handshake.
        context->handshake.store( false, std::memory_order_relaxed );
        assert( countdown > 0 );
        countdown -= 1;
        if ( countdown == 0 )
        {
            // All threads have paused at the handshake.
            state = STOPPED;
            condition.notify_all();
        }
    }



    // Mark all local stack references.
    for ( gc_auto_impl* a = context->autos; a != NULL; a = a->prev )
    {
        gc_mark( a->p );
    }
    
    // And run custom local marking routines.
    for ( gc_mark_scope_impl* m = context->markers; m != NULL; m = m->prev )
    {
        m->mark();
    }


    // Handshake completion.
    {
        std::unique_lock< std::mutex > lock( mutex );

        // Submit local marks.
        mark_buffers.push_back( context->mark );
        context->mark = nullptr;
        
        // Switch to marking with the new colour for this cycle.
        context->colour = colour;

        // Wait for the collector thread to release us.
        while ( state != MARKING )
        {
            assert( state == STOPPED );
            condition.wait( lock );
        }
        
        // This thread has completed marking.
        assert( countdown > 0 );
        countdown -= 1;
        if ( countdown == 0 )
        {
            // All threads have completed marking.
            state = RUNNING;
            condition.notify_all();
        }
    }

}


void gc_heap_impl::collector()
{
    gc_colour dirty = BLACK;
    enum request action = IDLE;

    while ( action != QUIT )
    {
        // Wait for collection request, and handshake with mutators.
        {
            std::unique_lock< std::mutex > lock( mutex );


            // Wait for a request.
            while ( request == IDLE )
            {
                condition.wait( lock );
            }
            
            action = request;


            // Request a handshake with every active thread.
            countdown = (int)contexts.size();

            for ( auto i = contexts.begin(); i != contexts.end(); ++i )
            {
                gc_context* context = *i;
                context->handshake.store( true, std::memory_order_relaxed );
            }

            if ( countdown )
            {
                // Waiting for one or more mutator threads to pause.
                state = PAUSING;
            }
            else
            {
                // No mutator threads to wait for.
                state = STOPPED;
            }
            
            condition.notify_all();
            

            // Wait until all threads have begun the handshake.
            while ( state != STOPPED )
            {
                assert( state == PAUSING );
                condition.wait( lock );
            }
            

            // Submit full slot buffers.
            slot_buffers.insert( slot_buffers.end(),
                    full_slot_buffers.begin(), full_slot_buffers.end() );
            full_slot_buffers.clear();


            // Add all abandoned allocs to the zero-count-table.
            for ( auto i = abandoned_allocs.begin();
                            i != abandoned_allocs.end(); ++i )
            {
                gc_mark_buffer* allocs = *i;
                for ( auto i = allocs->begin(); i != allocs->end(); ++i )
                {
                    zero_count_table.insert( *i );
                }
                delete allocs;
            }
            abandoned_allocs.clear();
            

            // Mark all roots.
            gc_mark_buffer* mark = new gc_mark_buffer();
            
            {
                std::lock_guard< std::mutex > lock( root_mutex );
                for ( gc_root_impl* root = root_list.next;
                                root != &root_list; root = root->next )
                {
                    assert( root->p );
                    gc_refcountof( root->p )->marked = true;
                    mark->add( root->p );
                }
            }

            mark_buffers.push_back( mark );


            // Now all threads have begun the handshake (and nothing is
            // marking with the old colour), we can release mutator threads
            // to mark with the new colour.

            // Mark with new colour - but remember current dirty colour.
            dirty = colour;
            colour = dirty == PURPLE ? ORANGE : PURPLE;

            // New collect requests after this point should kick off a new
            // collection.
            request = IDLE;
            
            // Release threads.
            countdown = (int)contexts.size();
            if ( countdown )
            {
                // Release mutators to finish marking locals.
                state = MARKING;
            }
            else
            {
                // No mutators to finish marking locals.
                state = RUNNING;
            }

            condition.notify_all();
            
        }
        
        
        // Update refcounts using the buffers given to us during the
        // handshake (and peeking into the new buffers).  After this step
        // is complete the heap contains no slots of the dirty colour.
        
        // We are guaranteed that each slot only appears once in the slot
        // buffers, as any time one thread writes to a slot and then another
        // thread writes to that same slot should have been protected by a
        // client lock.
        
        for ( auto i = slot_buffers.begin(); i != slot_buffers.end(); ++i )
        {
            gc_slot_buffer* slots = *i;
            
            // Update with slots.
            for ( auto j = slots->begin(); j != slots->end(); ++j )
            {
                update_slot( dirty, j->slot, j->value );
            }
            
            // Done with this slot buffer.
            delete slots;
        }
        
        slot_buffers.clear();

        
        // Wait for all marking to complete.
        {
            std::unique_lock< std::mutex > lock( mutex );
            while ( state != RUNNING )
            {
                assert( state == MARKING );
                condition.wait( lock );
            }
        }
        
        
        // Sweep unmarked objects with zero refcounts.
        std::vector< void* > dead;
        for ( auto i = zero_count_table.begin();
                        i != zero_count_table.end(); )
        {
            void* object = *i;
            if ( gc_refcountof( object )->marked )
            {
                // Marked, still alive.
                ++i;
            }
            else
            {
                // Unmarked, garbage.
                i = zero_count_table.erase( i );
                sweep( &dead, object );
            }
        }
        
        while ( dead.size() )
        {
            void* object = dead.back();
            dead.pop_back();
            sweep( &dead, object );
        }
        
        
        // Reset marks.
        for ( auto i = mark_buffers.begin(); i != mark_buffers.end(); ++i )
        {
            gc_mark_buffer* marks = *i;

            for ( auto j = marks->begin(); j != marks->end(); ++j )
            {
                void* object = *j;
                gc_refcountof( object )->marked = false;
            }
            
            delete marks;
        }
        
        mark_buffers.clear();
    
    }

}



void gc_heap_impl::update_slot(
            gc_colour dirty, gc_slot_impl* slot, uintptr_t oldvalue )
{
    // Check current colour of slot.
    uintptr_t newvalue = slot->p.load( std::memory_order_relaxed );
    gc_colour colour = (gc_colour)( newvalue & COLOUR );
    
    if ( colour == dirty )
    {
        // Attempt to turn slot from dirty to green.
        uintptr_t expected = newvalue;
        uintptr_t greenvalue = ( newvalue & ~COLOUR ) | GREEN;
        slot->p.compare_exchange_strong(
                        expected, greenvalue, std::memory_order_relaxed );
    }
    else
    {
        // The only way the slot can be green is if we encountered it already,
        // which should be impossible unless there was a data race caused by
        // mutators updating the same slot without locking.
        assert( colour != GREEN );
    
        // Slot is not dirty but instead the new mark colour.  Value at the
        // snapshot will be stored in the slot buffers of one of the mutators.
        
        // Search through current slot buffers.  Hopefully this case is rare,
        // and when it does trigger it will hopefully be quickly after the
        // mutators have resumed, so mutators' slot buffers should be
        // relatively empty.

        std::lock_guard< std::mutex > lock( mutex );
        auto found = std::make_pair( false, (uintptr_t)0 );
        for ( auto i = contexts.begin(); i != contexts.end(); ++i )
        {
            gc_context* context = *i;
            found = context->slots->find( slot );
            if ( found.first )
            {
                break;
            }
        }
        
        assert( found.first );
        newvalue = found.second;
    }
    
    
    // oldvalue and newvalue are now correct.  Perform reference count update.
    void* oldobject = (void*)( oldvalue & ~COLOUR );
    void* newobject = (void*)( newvalue & ~COLOUR );
    gc_refcount* oldref = gc_refcountof( oldobject );
    gc_refcount* newref = gc_refcountof( newobject );

    assert( newref->refcount >= 0 );
    if ( newref->refcount == 0 )
    {
        size_t erased = zero_count_table.erase( newobject );
        assert( erased );
    }

    newref->refcount += 1;
    oldref->refcount -= 1;

    assert( oldref->refcount >= 0 );
    if ( oldref->refcount == 0 )
    {
        zero_count_table.insert( oldobject );
    }
}
                
                
void gc_heap_impl::sweep( std::vector< void* >* dead, void* object )
{
    assert( gc_refcountof( object )->refcount == 0 );
    assert( gc_refcountof( object )->marked == false );
    
    // Get type.
    gc_type* type = gc_typeof( object );
    if ( ! type || ! type->slots )
    {
        // No slots.
        gc_free( object );
        return;
    }
    
    // Decref all slots.
    size_t length = gc_lengthof( object );
    for ( size_t i = 0; i < length; ++i )
    {
        void* address = (char*)object + type->size * i;

        for ( size_t j = 0; type->slots[ j ] != gc_type::END_SLOTS; ++j )
        {
            void* slotaddr = (char*)address + type->slots[ j ];
            gc_slot_impl* slot = (gc_slot_impl*)slotaddr;
            
            // Load value of slot.
            uintptr_t p = slot->p.load( std::memory_order_relaxed );
            void* object = (void*)( p & ~COLOUR );

            // Decref.
            gc_refcount* objref = gc_refcountof( object );
            objref->refcount -= 1;
            assert( objref->refcount >= 0 );
            if ( objref->refcount == 0 )
            {
                if ( objref->marked )
                {
                    // Still alive with zero refcount.
                    zero_count_table.insert( object );
                }
                else
                {
                    // Dead, sweep it.
                    dead->push_back( objref );
                }
            }
        }
    }
    
    
    // Free memory.
    gc_free( object );
}
                









/*
    Slot buffer.
*/


gc_slot_buffer::gc_slot_buffer()
    :   count( 0 )
{
}

gc_slot_buffer::~gc_slot_buffer()
{
}


bool gc_slot_buffer::add( gc_slot_impl* slot, uintptr_t value )
{
    // There is only a single thread modifying a slot buffer, but both the
    // mutator thread and the collector thread can be reading it at the same
    // time.  We therefore do the update using atomic operations.
    
    // Count should have the value we last wrote to it.
    size_t count = this->count.load( std::memory_order_relaxed );
    if ( count == RECORD_COUNT )
    {
        // Slot buffer is full.
        return false;
    }
    
    // Write the slot record.
    records[ count ].slot   = slot;
    records[ count ].value  = value;
    
    // Update count with release order (so that the writes above complete
    // before the write to count is visible to the collector).
    count += 1;
    this->count.store( std::memory_order_release );
    
    return true;
}


std::pair< bool, uintptr_t > gc_slot_buffer::find( gc_slot_impl* slot )
{
    // We're now on the collector thread.
    
    // Get the current value of count, this time with acquire order (so that
    // we can safely read records written up to count).
    size_t count = this->count.load( std::memory_order_acquire );
    
    // Do a linear scan to check if we have a record of the slot.
    for ( size_t i = 0; i < count; ++i )
    {
        const record& record = records[ i ];
        if ( record.slot == slot )
        {
            return std::make_pair( true, record.value );
        }
    }
    
    return std::make_pair( false, 0 );
}


void gc_slot_buffer::clear()
{
    count.store( 0, std::memory_order_relaxed );
}


gc_slot_buffer::record* gc_slot_buffer::begin()
{
    return records;
}

gc_slot_buffer::record* gc_slot_buffer::end()
{
    size_t count = this->count.load( std::memory_order_acquire );
    return records + count;
}





/*
    Mark buffer.
*/


gc_mark_buffer::gc_mark_buffer()
    :   first( new chunk() )
    ,   last( first )
    ,   index( 0 )
{
    first->next = nullptr;
}

gc_mark_buffer::~gc_mark_buffer()
{
    for ( chunk* c = first; c != nullptr; )
    {
        chunk* d = c;
        c = c->next;
        delete d;
    }
}


void gc_mark_buffer::add( void* p )
{
    if ( index == CHUNK_SIZE )
    {
        if ( ! last->next )
        {
            last->next = new chunk();
            last->next->next = nullptr;
        }
        last = last->next;
        index = 0;
    }
    
    last->elements[ index ] = p;
    index += 1;
}

void gc_mark_buffer::clear()
{
    // Don't delete any memory, just clear.
    last = first;
    index = 0;
}


gc_mark_buffer::iterator::iterator( chunk* chunk, size_t index )
    :   ichunk( chunk )
    ,   index( index )
{
}

bool gc_mark_buffer::iterator::operator == ( const iterator& o ) const
{
    return ichunk == o.ichunk && index == o.index;
}

bool gc_mark_buffer::iterator::operator != ( const iterator& o ) const
{
    return !( *this == o );
}
    
void* gc_mark_buffer::iterator::operator * () const
{
    return ichunk->elements[ index ];
}

gc_mark_buffer::iterator& gc_mark_buffer::iterator::operator ++ ()
{
    index += 1;
    if ( index >= CHUNK_SIZE )
    {
        ichunk = ichunk->next;
        index = 0;
    }

    return *this;
}


gc_mark_buffer::iterator gc_mark_buffer::begin()
{
    return iterator( first, 0 );
}

gc_mark_buffer::iterator gc_mark_buffer::end()
{
    return iterator( last, index );
}








/*
    Semi-public interface.
*/


void gc_new_root( gc_root_impl* proot )
{
    // Unlink from current heap.
    if ( proot->heap )
    {
        proot->heap->lose_root( proot );
    }
    
    // Add to this thread's current heap.
    context->heap->link_root( proot );
}


void gc_delete_root( gc_root_impl* proot )
{
    proot->heap->lose_root( proot );
}


void gc_record_slot( gc_slot_impl* slot, uintptr_t value )
{
    // Write barrier has encountered a clean slot.  Remember slot and its
    // current value in the slot buffer.
    if ( ! context->slots->add( slot, value ) )
    {
        // Ran out of space in the slot buffer.
        context->heap->refresh_slot_buffer( context );
        context->slots->add( slot, value );
    }

    // Ensure the write to the slot buffer is visible before the slot
    // update itself.
    std::atomic_thread_fence( std::memory_order_release );
}


void gc_safepoint_impl()
{
    context->heap->handshake( context );
}


void gc_mark_impl( void* p )
{
    // Mark an object reachable from locals or roots.
    gc_refcountof( p )->marked = true;
    context->mark->add( p );
}



}





/*
    Public interface.
*/



gc_heap::gc_heap()
    :   p( new gc_impl::gc_heap_impl() )
{
}

gc_heap::~gc_heap()
{
    delete p;
}

gc_impl::gc_heap_impl* gc_heap::get() const
{
    return p;
}






gc_scope::gc_scope( gc_heap* heap )
{
    assert( gc_impl::context == nullptr );
    gc_impl::context = new gc_impl::gc_context();
    heap->get()->link_context( gc_impl::context );
}

gc_scope::~gc_scope()
{
    assert( gc_impl::context );
    gc_impl::context->heap->lose_context( gc_impl::context );
    delete gc_impl::context;
}





gc_mark_scope::gc_mark_scope( std::function< void() > mark )
{
    g.mark = mark;
    g.prev = gc_impl::context->markers;
    gc_impl::context->markers = &g;
}

gc_mark_scope::~gc_mark_scope()
{
    assert( gc_impl::context->markers == &g );
    gc_impl::context->markers = g.prev;
}







/*
    gc_typeof<>() for primitive types.
*/


#define GC_TYPEOF_FUNDAMENTAL( x ) \
    template <> gc_type* gc_typeof< x >() \
    { \
        if ( sizeof( x ) <= sizeof( void* ) \
                && alignof( x ) <= alignof( void* ) ) \
        { \
            return NULL; \
        } \
        else \
        { \
            static gc_type TYPE = { sizeof( x ), alignof( x ), NULL }; \
            return &TYPE; \
        } \
    }


GC_TYPEOF_FUNDAMENTAL( bool );
GC_TYPEOF_FUNDAMENTAL( signed char );
GC_TYPEOF_FUNDAMENTAL( unsigned char );
GC_TYPEOF_FUNDAMENTAL( char );
GC_TYPEOF_FUNDAMENTAL( wchar_t );
GC_TYPEOF_FUNDAMENTAL( char16_t );
GC_TYPEOF_FUNDAMENTAL( char32_t );
GC_TYPEOF_FUNDAMENTAL( short int );
GC_TYPEOF_FUNDAMENTAL( unsigned short int );
GC_TYPEOF_FUNDAMENTAL( int );
GC_TYPEOF_FUNDAMENTAL( unsigned int );
GC_TYPEOF_FUNDAMENTAL( long int );
GC_TYPEOF_FUNDAMENTAL( unsigned long int );
GC_TYPEOF_FUNDAMENTAL( long long int );
GC_TYPEOF_FUNDAMENTAL( unsigned long long int );
GC_TYPEOF_FUNDAMENTAL( float );
GC_TYPEOF_FUNDAMENTAL( double );
GC_TYPEOF_FUNDAMENTAL( long double );



