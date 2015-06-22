//
//  yl_gcheap.cpp
//
//  Created by Edmund Kapusniak on 19/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_gcheap.h"
#include "dlmalloc.h"



__thread yl_gcheap* yl_current_gcheap = nullptr;



yl_gcheap::yl_gcheap()
    :   _alloc( nullptr )
    ,   _alloc_list( nullptr )
    ,   _alloc_bytes( 0 )
    ,   _collect_state( IDLE )
    ,   _weak_locked( false )
    ,   _unmarked( YL_GCCOLOUR_PURPLE )
    ,   _marked( YL_GCCOLOUR_ORANGE )
    ,   _can_resurrect( false )
{
    // Create memory space.
    _alloc = create_mspace( 0, 0 );
    mspace_track_large_chunks( _alloc, 1 );
    
    // Launch collector thread.
    _collect_thread = std::thread( &yl_gcheap::collect_thread, this );
}

yl_gcheap::~yl_gcheap()
{
    update_state( IDLE | COLLECT, QUIT );
    _collect_thread.join();
    destroy_mspace( _alloc );
}



void yl_gcheap::register_type( uint8_t kind, const yl_gctype& type )
{
    collect_wait();
    size_t index = yl_kind_to_index( kind );
    if ( _types.size() <= index )
        _types.resize( index + 1 );
    _types[ index ] = type;
}


void* yl_gcheap::allocate( size_t size )
{
    std::lock_guard< std::mutex > lock( _alloc_mutex );

    // Allocate new object.
    assert( size >= sizeof( yl_gcheader ) );
    void* p = mspace_malloc( _alloc, size );
    void* next = mspace_next( _alloc, p );

    // Colour new object.
    yl_gcheader* gcheader = (yl_gcheader*)p;
    gcheader->colour.store( _marked, std::memory_order_relaxed );
    gcheader->gcflags.store( 0, std::memory_order_relaxed );
    gcheader->kind      = 0;
    gcheader->refcount  = 0;
    
    // Link it into the alloc list.
    if ( next )
    {
        yl_gcheader* gcnext = (yl_gcheader*)next;
        yl_gcheader* gcprev = gcnext->prev.load( std::memory_order_relaxed );
        
        // Update alloc list atomically, as GC thread does not lock.
        gcheader->prev.store( gcprev, std::memory_order_relaxed );
        gcnext->prev.store( gcheader, std::memory_order_release );
    }
    else
    {
        // Same goes for the head of the list.
        gcheader->prev.store( _alloc_list, std::memory_order_relaxed );
        _alloc_list.store( gcheader, std::memory_order_release );
    }
    
    // Extra release fence to ensure that colouring of the new object
    // is visible to the mark thread before any reference to it.
    std::atomic_thread_fence( std::memory_order_release );
    
    // Count how many bytes are allocated and kick off collections.
    _alloc_bytes += size;
    if ( _alloc_bytes >= COLLECT_THRESHOLD )
    {
        _alloc_bytes = 0;
        collect();
    }
    
    return p;
}


bool yl_gcheap::collect()
{
    // If the collector is busy do not do anything.
    if ( ! check_state( IDLE ) )
        return false;

    // Update epoch.
    std::swap( _unmarked, _marked );
    
    // Mark stack references.
    for ( const auto& stackref : _stackrefs )
    {
        mark( (yl_gcobject*)( *stackref ) );
    }
    
    // Mark roots.
    for ( const auto& root : _roots )
    {
        mark( root );
    }
    
    // Eager marking.
    for ( const auto& eager : _eager )
    {
        mark_object( eager );
    }
    
    // Weak objects can be resurrected from this point until the end of
    // the mark phase - after that they are irrevocably dead.
    _can_resurrect = true;

    // Kick off the collector thread.
    return update_state( IDLE, COLLECT );
}

void yl_gcheap::collect_wait()
{
    wait_state( IDLE | QUIT );
}




void yl_gcheap::set_oolref( yl_gcobject* object, yl_gcobject* ool )
{
    std::lock_guard< std::mutex > lock( _oolref_mutex );
    
    // If there was a previous value for the oolref, write barrier it
    // just like any other reference.
    if ( object->check_gcflags( YL_GCFLAG_OOLREF ) )
    {
        write_barrier( _oolrefs.at( object ) );
    }
    
    // Update oolref.
    _oolrefs[ object ] = ool;
    
    // Mark the object as having an oolref.  It doesn't matter if the
    // GC doesn't pick this up until the next epoch (after the next
    // handshake), as ool must be live in this epoch already.
    object->set_gcflags( YL_GCFLAG_OOLREF );

}

yl_gcobject* yl_gcheap::get_oolref( yl_gcobject* object )
{
    if ( object->check_gcflags( YL_GCFLAG_OOLREF ) )
    {
        return _oolrefs.at( object );
    }
    else
    {
        return nullptr;
    }
}



void yl_gcheap::weak_lock()
{
    assert( ! _weak_locked );
    _weak_mutex.lock();
    _weak_locked = true;
}

void yl_gcheap::weak_unlock()
{
    assert( _weak_locked );
    _weak_mutex.unlock();
    _weak_locked = false;
}




void yl_gcheap::eager_lock( yl_gcobject* object )
{
    // Set to be marked at the next handshake.
    _eager.insert( object );
    
    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour == _marked ) )
        return;

    // If the object is unmarked or grey both then we need to mark it now.
    std::lock_guard< std::mutex > lock( _grey_mutex );

    // Call eager_mark routine.
    uint8_t index = yl_kind_to_index( object->kind() );
    const yl_gctype& type = _types.at( index );
    if ( type.eager_mark )
    {
        type.eager_mark( this, object );
    }
            
    // Mark oolref.
    if ( object->check_gcflags( YL_GCFLAG_OOLREF ) )
    {
        eager_mark( _oolrefs.at( object ) );
    }
    
    // Colour it marked.
    gcheader->colour.store( _marked, std::memory_order_relaxed );
}

void yl_gcheap::eager_unlock( yl_gcobject* object )
{
    _eager.erase( object );
}





void yl_gcheap::weak_create_impl( yl_gcobject* weak )
{
    assert( _weak_locked );
    
    // Set weak flag.  The object must be live, so it will not be swept
    // until at least the next GC epoch (when this flag will be visible
    // to the GC due to the handshake).
    weak->set_gcflags( YL_GCFLAG_WEAK );

}






bool yl_gcheap::check_state( unsigned state )
{
    std::lock_guard< std::mutex > lock( _collect_mutex );
    return _collect_state & state;
}

bool yl_gcheap::update_state( unsigned current, collect_state update )
{
    std::lock_guard< std::mutex > lock( _collect_mutex );
    if ( _collect_state & current )
    {
        _collect_state = update;
        _collect_condition.notify_all();
    }
    return false;
}

yl_gcheap::collect_state yl_gcheap::wait_state( unsigned wait )
{
    std::unique_lock< std::mutex > lock( _collect_mutex );
    while ( ( _collect_state & wait ) == 0 )
    {
        _collect_condition.wait( lock );
    }
    return _collect_state;
}



void yl_gcheap::collect_thread()
{
    while ( true )
    {
        collect_state state = wait_state( COLLECT | QUIT );
        if ( state == QUIT )
        {
            break;
        }
        
        collect_mark();
        collect_sweep();
        update_state( COLLECT, IDLE );
    }
}




void yl_gcheap::write_barrier_impl( yl_gcobject* object )
{
    std::lock_guard< std::mutex > lock( _grey_mutex );
    eager_mark_impl( object );
}


bool yl_gcheap::weak_obtain_impl( yl_gcobject* object )
{
    assert( _weak_locked );
    assert( object->check_gcflags( YL_GCFLAG_WEAK ) );

    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour != _unmarked ) )
        return true;


    /*
        Resurrecting objects is not simple, because by resurrecting an object
        you not only make it live, but also all the objects it references
        must be made live as well.  Once the mark phase is over and sweeping
        has begun, any non-leaf unmarked weak objects are on death row and can
        no longer be resurrected.
    */
    
    std::lock_guard< std::mutex > lock( _grey_mutex );

    if ( ! object->check_gcflags( YL_GCFLAG_LEAF ) )
    {
        if ( _can_resurrect )
        {
            gcheader->colour.store( YL_GCCOLOUR_GREY, std::memory_order_relaxed );
            _grey_list.push_back( object );
            return true;
        }
    }
    else
    {
        gcheader->colour.store( _marked, std::memory_order_relaxed );
    }

    return false;
}


void yl_gcheap::mark_impl( yl_gcobject* object )
{
    yl_gcheader* gcheader = &object->_gcheader;
    if ( ! object->check_gcflags( YL_GCFLAG_LEAF ) )
    {
        gcheader->colour.store( YL_GCCOLOUR_GREY, std::memory_order_relaxed );
        _mark_list.push_back( object );
    }
    else
    {
        gcheader->colour.store( _marked, std::memory_order_relaxed );
    }
}


void yl_gcheap::eager_mark_impl( yl_gcobject* object )
{
    yl_gcheader* gcheader = &object->_gcheader;
    if ( ! object->check_gcflags( YL_GCFLAG_LEAF ) )
    {
        gcheader->colour.store( YL_GCCOLOUR_GREY, std::memory_order_relaxed );
        _grey_list.push_back( object );
    }
    else
    {
        gcheader->colour.store( _marked, std::memory_order_relaxed );
    }
}




void yl_gcheap::collect_mark()
{
    /*
        All objects we are going to mark were live at the beginning of this
        GC epoch (or created during it), including objects referenced by
        oolrefs.  Any object referenced by a _new_ oolref - an oolref which
        was created and had its flag set since the GC epoch - must either
        be a new object itself or was reachable through another reference
        at the start of the epoch.
        
        Therefore as long as the oolref flags we read are at least as new as
        the beginning of the epoch, we are ok.  The handshake with the
        mutator that kicks off collection should ensure this.
    */

    while ( true )
    {
        // Mark all objects reachable from the mark list.
        while ( _mark_list.size() )
        {
            yl_gcobject* object = _mark_list.back();
            _mark_list.pop_back();

            // Check if we've marked this object already.
            yl_gcheader* gcheader = &object->_gcheader;
            yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
            if ( YL_UNLIKELY( colour == _marked ) )
                continue;

            // Should have been marked grey when it was added to the list.
            assert( colour == YL_GCCOLOUR_GREY );

            // Mark the object.
            mark_object( object );
        }
    
    
        // Move objects from the mark list to the grey list.
        std::lock_guard< std::mutex > lock( _grey_mutex );
        _grey_list.swap( _mark_list );
        
        
        // If the mark is empty then we're done.
        if ( _mark_list.empty() )
        {
            // Too late to resurrect unmarked objects.
            _can_resurrect = false;
            break;
        }
    }
}


void yl_gcheap::mark_object( yl_gcobject* object )
{
    // Call mark routine.
    uint8_t index = yl_kind_to_index( object->kind() );
    const yl_gctype& type = _types.at( index );
    if ( type.mark )
    {
        type.mark( this, object );
    }
    
    // Mark oolref.
    if ( object->check_gcflags( YL_GCFLAG_OOLREF ) )
    {
        mark( _oolrefs.at( object ) );
    }
    
    // Mark as marked.
    yl_gcheader* gcheader = &object->_gcheader;
    gcheader->colour.store( _marked, std::memory_order_relaxed );
}



#if GC_MEMORY_ORDER_CONSUME
static const std::memory_order yl_memory_order_consume = std::memory_order_consume;
#else
static const std::memory_order yl_memory_order_consume = std::memory_order_relaxed;
#endif


void yl_gcheap::collect_sweep()
{
    /*
        All objects we are going to sweep were dead at the beginning of this
        GC epoch, which means the latest point where they were alive and able
        to have their oolref and weak flags set is before the handshake with
        the mutator.  Therefore those flags should be visible on the objects
        we sweep here.
    */

    /*
        The alloc list is traversed in a lock-free manner.  Only the mutator
        thread can add elements to the list.  Only this thread can remove
        them.
    */
    
    yl_gcheader* alloc = _alloc_list.load( yl_memory_order_consume );
    while ( alloc )
    {
        yl_gcobject* object = (yl_gcobject*)alloc;
        alloc = alloc->prev.load( yl_memory_order_consume );
        
        // Check colour.
        yl_gcheader* gcheader = &object->_gcheader;
        yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
        if ( YL_LIKELY( colour == _marked ) )
            continue;

        // All grey objects should have been marked.
        assert( colour != YL_GCCOLOUR_GREY );

        // Sweep object.
        uint8_t index = yl_kind_to_index( object->kind() );
        const yl_gctype& type = _types.at( index );


        // Call object destructor.
        if ( type.destroy )
        {
            if ( object->check_gcflags( YL_GCFLAG_WEAK ) )
            {
                // Must hold weak lock during destructor.
                std::unique_lock< std::mutex > lock( _weak_mutex );
                
                // Check for resurrection.
                colour = gcheader->colour.load( std::memory_order_relaxed );
                if ( colour == _marked )
                {
                    // Only leaf objects can resurrect during sweep.
                    assert( colour != YL_GCCOLOUR_GREY );
                    assert( object->check_gcflags( YL_GCFLAG_LEAF ) );
                    continue;
                }
                
                type.destroy( this, object );
            }
            else
            {
                // Just destroy.
                type.destroy( this, object );
            }
        }
        

        // Destroy oolref.
        if ( object->check_gcflags( YL_GCFLAG_OOLREF ) )
        {
            std::lock_guard< std::mutex > lock( _oolref_mutex );
            _oolrefs.erase( object );
        }
        

        // Deallocate object.
        std::lock_guard< std::mutex > lock( _alloc_mutex );
        
        
        // Remove object from the alloc list.
        void* next = mspace_next( _alloc, object );
        yl_gcheader* gcprev = gcheader->prev.load( std::memory_order_relaxed );
        if ( next )
        {
            yl_gcheader* gcnext = (yl_gcheader*)next;
            assert( gcnext->prev.load( std::memory_order_relaxed ) == gcheader );
            gcnext->prev.store( gcprev, std::memory_order_relaxed );
        }
        else
        {
            assert( _alloc_list.load( std::memory_order_relaxed ) == gcheader );
            _alloc_list.store( gcprev, std::memory_order_relaxed );
        }
        
        
        // Free the object.
        mspace_free( _alloc, object );
    }
}




yl_gcobject::yl_gcobject( uint8_t kind, uint8_t gcflags )
{
    assert( _gcheader.colour.load( std::memory_order_relaxed ) == yl_current_gcheap->_marked );
    assert( _gcheader.gcflags.load( std::memory_order_relaxed ) == 0 );
    assert( _gcheader.kind == 0 );
    assert( _gcheader.refcount == 0 );
    
    _gcheader.gcflags.store( gcflags, std::memory_order_relaxed );
    _gcheader.kind = kind;
}

void yl_gcobject::incref()
{
    assert( _gcheader.refcount < 0xFF );
    _gcheader.refcount += 1;
    if ( _gcheader.refcount == 1 )
    {
        yl_current_gcheap->_roots.insert( this );
    }
}

void yl_gcobject::decref()
{
    assert( _gcheader.refcount > 0 );
    _gcheader.refcount -= 1;
    if ( _gcheader.refcount == 0 )
    {
        yl_current_gcheap->_roots.erase( this );
    }
}


inline void yl_gcobject::set_gcflags( uint8_t flags ) const
{
    // This operation is not atomic.
    uint8_t gcflags = _gcheader.gcflags.load( std::memory_order_relaxed );
    _gcheader.gcflags.store( gcflags | flags, std::memory_order_relaxed );
}

inline void yl_gcobject::clear_gcflags( uint8_t flags ) const
{
    // This operation is not atomic.
    uint8_t gcflags = _gcheader.gcflags.load( std::memory_order_relaxed );
    _gcheader.gcflags.store( gcflags & ~flags, std::memory_order_relaxed );
}











