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
    intptr_t    refcount    : INTPTR_BITS - 3;
    bool        marked      : 1;
    bool        isarray     : 1;
    bool        hastype     : 1;
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

void* gc_alloc( gc_type* type, size_t length )
{
    assert( sizeof( size_t ) == sizeof( void* ) );
    assert( sizeof( gc_type* ) == sizeof( void* ) );
    assert( sizeof( gc_refcount ) == sizeof( void* ) );
    
    length = std::max( length, (size_t)1 );
    
    size_t header_size = 0;
    header_size += length != 1 ? sizeof( size_t ) : 0;
    header_size += type ? sizeof( gc_type* ) : 0;
    header_size += sizeof( gc_refcount );
    header_size = align( header_size, type ? type->align : alignof( void* ) );
    size_t data_size = ( type ? type->size : sizeof( void* ) ) * length;
    
    void* p = malloc( header_size + data_size );
    void* object = (char*)p + header_size;
    
    gc_refcount* objref = gc_refcountof( object );
    objref->refcount    = 0;
    objref->marked      = false;
    objref->isarray     = length != 1;
    objref->hastype     = type != nullptr;
    
    if ( type )
    {
        gc_type** ptype = (gc_type**)( objref - 1 );
        *ptype = type;
        
        if ( length != 1 )
        {
        }
    }
    else if ( length != 1 )
    {
        
    }
}

void gc_free( void* object )
{
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
    void update_slot( gc_colour dirty, gc_slot_impl* slot, uintptr_t oldvalue );
    void sweep( std::vector< void* >* dead, void* object );
    
    std::thread                     thread;

    std::mutex                      root_mutex;
    gc_root_impl                    root_list;

    std::mutex                      mutex;
    std::condition_variable         condition;

    state                           state;
    gc_colour                       colour;
    std::list< gc_context* >        contexts;

    int                             countdown;
    std::vector< gc_slot_buffer* >  slot_buffers;
    std::vector< gc_mark_buffer* >  mark_buffers;
    
    std::unordered_set< void* >     new_objects_table;
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
    void safepoint();


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
    std::lock_guard< std::mutex > lock( root_mutex );
    proot->next = &root_list;
    proot->prev = root_list.next;
    proot->prev->next = proot;
    proot->next->prev = proot;
    proot->heap = this;
}

void gc_heap_impl::lose_root( gc_root_impl* proot )
{
    assert( proot->heap == this );
    std::lock_guard< std::mutex > lock( root_mutex );
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
        // All threads have paused at the handshake.
        state = STOPPED;
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
        // All threads have completed marking.
        state = RUNNING;
        condition.notify_all();
    }

}


void gc_heap_impl::collector()
{
    gc_colour dirty = BLACK;

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
            state = MARKING;
            dirty = colour;
            colour = dirty == PURPLE ? ORANGE : PURPLE;
            countdown = (int)contexts.size();
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
            while ( state == MARKING )
            {
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


    // TODO SOMEWHERE: Perform cycle detection.
        
        

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
                






gc_scope_impl::gc_scope_impl( gc_heap* h )
    :   heap( h->get() )
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

void gc_scope_impl::safepoint()
{
    // Begin handshake.
    heap->begin_handshake( context );

    // Mark all local stack references.
    for ( gc_auto_impl* a = context->autos; a != NULL; a = a->prev )
    {
        gc_mark( a->p );
    }
    
    for ( gc_mark_scope_impl* m = context->mark_scopes; m != NULL; m = m->prev )
    {
        m->mark();
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


void gc_record_slot( gc_slot_impl* slot, uintptr_t value )
{
    // Write barrier has encountered a clean slot.  Remember slot and its
    // current value in the slot buffer.
    if ( ! context->slots->add( slot, value ) )
    {
        // Ran out of space in the slot buffer.
        context->scope->get_heap()->refresh_slot_buffer( context );
        context->slots->add( slot, value );
    }

    // Ensure the write to the slot buffer is visible before the slot
    // update itself.
    std::atomic_thread_fence( std::memory_order_release );
}


void gc_safepoint_impl()
{
    context->scope->safepoint();
}


void gc_mark_impl( void* p )
{
    // Mark an object reachable from locals or roots.
    gc_refcountof( p )->marked = true;
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


gc_scope::gc_scope( gc_heap* heap )
    :   p( new gc_impl::gc_scope_impl( heap ) )
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



