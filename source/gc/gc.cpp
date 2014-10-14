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


namespace gc_impl
{


/*
    Per-thread GC context.
*/

__thread gc_context* context = nullptr;


gc_context::gc_context()
    :   colour( BLACK )
    ,   scope( nullptr )
    ,   autos( nullptr )
    ,   slots( nullptr )
    ,   mark( nullptr )
    ,   handshake( false )
{
}





/*
    Header for GC objects.
*/

static const int INTPTR_BITS = sizeof( intptr_t ) * 8;

struct gc_header
{
    union
    {
        struct
        {
            gc_type*    type;
            intptr_t    refcount : INTPTR_BITS - 1;
            bool        marked   : 1;
        };
        std::max_align_t align;
    };
};

gc_header* gc_headerof( void* p )
{
    return (gc_header*)p - 1;
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
    
    void refresh_slot_buffer( gc_context* buffer );
    void begin_handshake( gc_context* context );
    void complete_handshake( gc_context* context );
    
    
    
private:
    
    enum state
    {
        RUNNING,    // Mutators run as normal.
        PAUSING,    // Mutators begin handshake at next safepoint.
        STOPPED,    // Collector marks roots.
        MARKING,    // Some mutators running, some still marking locals.
        QUIT,
    };
    
    void collector();
    
    std::thread                     thread;
    
    
    std::mutex                      mutex;
    std::condition_variable         condition;

    state                           state;
    gc_colour                       colour;
    gc_root_impl                    root_list;
    std::list< gc_context* >        contexts;

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

    gc_scope_impl( gc_heap* h, gc_mark_callback mark, void* token );
    ~gc_scope_impl();

    gc_heap_impl* get_heap();
    void safepoint( void* safepoint );


private:

    gc_heap_impl*       heap;
    gc_mark_callback    mark_callback;
    void*               mark_token;
    
};




/*
    Buffer holding original values of dirty slots.
*/

class gc_slot_buffer
{
public:

    gc_slot_buffer();
    ~gc_slot_buffer();

    bool add( gc_slot_impl* slot );
    std::pair< bool, void* > find( gc_slot_impl* slot );
    void clear();


private:

    static const size_t RECORD_COUNT = 128;

    struct record
    {
        gc_slot_impl*   slot;
        void*           value;
    };

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
    GC heap and collection thread.
*/


gc_heap_impl::gc_heap_impl()
    :   state( RUNNING )
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
    {
        std::unique_lock< std::mutex > lock( mutex );

        // There should be no active scopes and the root list should be empty.
        assert( contexts.empty() );
        assert( root_list.next = &root_list );
    }


    // Perform full collection.
    
    // Request GC thread exit.

    // Wait for GC thread to complete.
    thread.join();

    // And the root list should be empty.
    
    // And all objects should be dead.
    
}


void gc_heap_impl::link_root( gc_root_impl* proot )
{
    assert( proot->heap == nullptr );
    std::lock_guard< std::mutex > lock( mutex );
    proot->next = &root_list;
    proot->prev = root_list.next;
    proot->prev->next = proot;
    proot->next->prev = proot;
    proot->heap = this;
}

void gc_heap_impl::lose_root( gc_root_impl* proot )
{
    assert( proot->heap == this );
    std::lock_guard< std::mutex > lock( mutex );
    proot->prev->next = proot->next;
    proot->next->prev = proot->prev;
    proot->prev = nullptr;
    proot->next = nullptr;
    proot->heap = nullptr;
}


void gc_heap_impl::link_context( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );
    
    while ( state != RUNNING )
    {
        condition.wait( lock );
    }

    context->slots  = new gc_slot_buffer();
    context->colour = colour;
    contexts.push_back( context );
}

void gc_heap_impl::lose_context( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );

    while ( state != RUNNING )
    {
        condition.wait( lock );
    }
    
    // Submit in-progress slot buffer.
    slot_buffers.push_back( context->slots );
    context->slots = nullptr;
}


void gc_heap_impl::refresh_slot_buffer( gc_context* context )
{
    std::lock_guard< std::mutex > lock( mutex );
    assert( state == RUNNING || state == PAUSING );

    // Slot buffer is full, submit the old one.
    slot_buffers.push_back( context->slots );
    context->slots = new gc_slot_buffer();
}


void gc_heap_impl::begin_handshake( gc_context* context )
{
    std::lock_guard< std::mutex > lock( mutex );
    assert( state == PAUSING );

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
        state = STOPPED;
        countdown = (int)contexts.size();
        condition.notify_all();
    }
}


void gc_heap_impl::complete_handshake( gc_context* context )
{
    std::unique_lock< std::mutex > lock( mutex );
    assert( state == STOPPED || state == MARKING );

    // Wait for the collector thread to release us.
    while ( state == STOPPED )
    {
        condition.wait( lock );
    }
    assert( state == MARKING );
    
    // Switch to marking with the new colour for this cycle.
    context->colour = colour;

    // Submit local marks.
    mark_buffers.push_back( context->mark );
    context->mark = nullptr;
    
    // This thread has completed marking.
    assert( countdown > 0 );
    countdown -= 1;
    if ( countdown == 0 )
    {
        state = RUNNING;
        condition.notify_all();
    }

}


void gc_heap_impl::collector()
{
    while ( true )
    {
        {
            std::unique_lock< std::mutex > lock( mutex );


            // Wait for a request to collect.
            while ( state == RUNNING )
            {
                condition.wait( lock );
            }
            
            
            // Check for quit.
            if ( state == QUIT )
            {
                return;
            }
        
        
            // Request a handshake with every active thread.
            assert( state == PAUSING );
            countdown = (int)contexts.size();
            for ( auto i = contexts.begin(); i != contexts.end(); ++i )
            {
                gc_context* context = *i;
                context->handshake.store( true, std::memory_order_relaxed );
            }
            

            // Wait until all threads have begun the handshake.
            while ( state == PAUSING )
            {
                condition.wait( lock );
            }


            // Mark all roots.
            assert( state == STOPPED );
            gc_mark_buffer* mark = new gc_mark_buffer();
            for ( gc_root_impl* root = root_list.next;
                            root != &root_list; root = root->next )
            {
                assert( root->p );
                gc_headerof( root->p )->marked = true;
                mark->add( root->p );
            }
            
            mark_buffers.push_back( mark );


            // Now all threads have begun the handshake (and nothing is
            // marking with the old colour), we can release mutator threads
            // to mark with the new colour.
            colour = colour == PURPLE ? ORANGE : PURPLE;
            state = MARKING;
            condition.notify_all();
            
        }
        
        
        // Update refcounts using the buffers given to us during the
        // handshake (and peeking into the new buffers).  After this step
        // is complete the heap contains no slots of the old colour.
        

        
        
        // Wait for all marking to complete.
        {
            std::unique_lock< std::mutex > lock( mutex );
            while ( state == MARKING )
            {
                condition.wait( lock );
            }
        }
        
        
        // Sweep unmarked objects with zero refcounts.
        
        
        // Perform cycle detection.
        

    
    }
}








gc_scope_impl::gc_scope_impl(
                gc_heap* h, gc_mark_callback mark, void* token )
    :   heap( h->get() )
    ,   mark_callback( mark )
    ,   mark_token( token )
{
    assert( context == nullptr );

    // Open GC scope on this thread.
    context = new gc_context();
    context->scope = this;
    context->handshake.store( false, std::memory_order_relaxed );

    // Register this context with heap.
    heap->link_context( context );
}

gc_scope_impl::~gc_scope_impl()
{
    assert( context->scope == this );
    assert( context->autos == nullptr );
    
    // Close GC scope on this thread.
    heap->lose_context( context );
    delete context;
    context = nullptr;
}

gc_heap_impl* gc_scope_impl::get_heap()
{
    return heap;
}

void gc_scope_impl::safepoint( void* safepoint )
{
    // Begin handshake.
    heap->begin_handshake( context );

    // Mark all local stack references.
    for ( gc_auto_impl* a = context->autos; a != NULL; a = a->prev )
    {
        gc_mark( a->p );
    }
    
    // Invoke user marking callback.
    if ( mark_callback )
    {
        mark_callback( mark_token, safepoint );
    }

    // Syncronize with the GC thread.
    heap->complete_handshake( context );
}









gc_slot_buffer::gc_slot_buffer()
    :   count( 0 )
{
}

gc_slot_buffer::~gc_slot_buffer()
{
}


bool gc_slot_buffer::add( gc_slot_impl* slot )
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
    uintptr_t v = slot->p.load( std::memory_order_relaxed );
    records[ count ].slot   = slot;
    records[ count ].value  = (void*)( v & ~COLOUR );
    
    // Update count with release order (so that the writes above complete
    // before the write to count is visible to the collector).
    count += 1;
    this->count.store( std::memory_order_release );
    
    return true;
}


std::pair< bool, void* > gc_slot_buffer::find( gc_slot_impl* slot )
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
            // TODO: Could do some reordering of records (move found records
            // to the end) to make subsequent finds for other slots faster.
            return std::make_pair( true, record.value );
        }
    }
    
    return std::make_pair( false, nullptr );
}


void gc_slot_buffer::clear()
{
    count.store( 0, std::memory_order_relaxed );
}







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













gc_type* gc_typeof_impl( void* p )
{
    return gc_headerof( p )->type;
}


void* gc_alloc_impl( gc_type* type )
{
    // TODO.
    return nullptr;
}


void gc_new_root( gc_root_impl* proot )
{
    // Unlink from current heap.
    if ( proot->heap )
    {
        proot->heap->lose_root( proot );
    }
    
    // Add to this thread's current heap.
    context->scope->get_heap()->link_root( proot );
}


void gc_delete_root( gc_root_impl* proot )
{
    proot->heap->lose_root( proot );
}


void gc_record_slot( gc_slot_impl* pslot )
{
    // Write barrier has encountered a clean slot.  Remember slot and its
    // current value in the slot buffer.
    if ( ! context->slots->add( pslot ) )
    {
        // Ran out of space in the slot buffer.
        context->scope->get_heap()->refresh_slot_buffer( context );
        context->slots->add( pslot );
    }

    // Ensure the write to the slot buffer is visible before the slot
    // update itself.
    std::atomic_thread_fence( std::memory_order_release );
}


void gc_safepoint_impl( void* safepoint )
{
    context->scope->safepoint( safepoint );
}


void gc_mark_impl( void* p )
{
    // Mark an object reachable from locals or roots.
    gc_headerof( p )->marked = true;
    context->mark->add( p );
}



}






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


gc_scope::gc_scope( gc_heap* heap, gc_mark_callback mark, void* token )
    :   p( new gc_impl::gc_scope_impl( heap, mark, token ) )
{
    
}

gc_scope::~gc_scope()
{
    delete p;
}

gc_impl::gc_scope_impl* gc_scope::get() const
{
    return p;
}






