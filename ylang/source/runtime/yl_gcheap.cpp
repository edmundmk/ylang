//
//  yl_gcheap.cpp
//
//  Created by Edmund Kapusniak on 19/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_gcheap.h"
#include "dlmalloc.h"


#define GC_DEBUG_REPORT 1
#define GC_DEBUG_SCRIBBLE 1
#define GC_DEBUG_COLLECT_EVERY_TIME 0


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



void yl_gcheap::register_type( yl_gctype* type )
{
    collect_wait();
    size_t index = yl_kind_to_index( type->kind );
    if ( _types.size() <= index )
    {
        _types.resize( index + 1 );
    }
    _types[ index ] = type;
}


void* yl_gcheap::allocate( uint8_t kind, size_t size )
{
    assert( size >= sizeof( yl_gcheader ) );

    // Count how many bytes are allocated and kick off collections.  Must
    // collect before we colour the new object, as collecting will change
    // the object colours.
    _alloc_bytes += size;
    if ( _alloc_bytes >= COLLECT_THRESHOLD )
    {
        _alloc_bytes = 0;
        collect();
    }

#if GC_DEBUG_COLLECT_EVERY_TIME
    collect();
    collect_wait();
#endif
    
    // Allocate with the lock held.
    std::lock_guard< std::mutex > lock( _alloc_mutex );

    // Allocate new object.
    assert( size >= sizeof( yl_gcheader ) );
    void* p = mspace_malloc( _alloc, size );
    void* next = mspace_next( _alloc, p );

    // Colour new object.
    yl_gcheader* gcheader = (yl_gcheader*)p;
    gcheader->colour.store( _marked, std::memory_order_relaxed );
    gcheader->gcflags.store( 0, std::memory_order_relaxed );
    gcheader->kind      = kind;
    gcheader->refcount  = 1;
    
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
    
    // Clear rest of object.
    memset( gcheader + 1, 0, size - sizeof( yl_gcheader ) );
    
    // Add to roots.
    _roots.insert( (yl_gcobject*)p );
    
    // Extra release fence to ensure that colouring of the new object
    // is visible to the mark thread before any reference to it.
    std::atomic_thread_fence( std::memory_order_release );
    
    return p;
}


void yl_gcheap::add_root( yl_gcobject* root )
{
    auto i = _roots.insert( root );
    assert( i.second );
}

void yl_gcheap::remove_root( yl_gcobject* root )
{
    size_t count = _roots.erase( root );
    assert( count );
}


bool yl_gcheap::collect()
{
    // If the collector is busy do not do anything.
    if ( ! check_state( IDLE ) )
        return false;

    // Update epoch.
    std::swap( _unmarked, _marked );
    
#if GC_DEBUG_REPORT
    printf( "GC : collect()\n" );
    printf( "    unmarked   : %d\n", _unmarked );
    printf( "    marked     : %d\n", _marked );
    printf( "    roots      : %zu\n", _roots.size() );
    printf( "    eager      : %zu\n", _eager.size() );
#endif
    
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
    // Get type.
    uint8_t index = yl_kind_to_index( object->kind() );
    yl_gctype* type = _types.at( index );

    assert( ! object->check_gcflags( YL_GCFLAG_EAGER ) );
    assert( type->eager_mark );

    // Set to be marked at the next handshake.
    _eager.insert( object );
    
    // Lock eager before marking.
    std::lock_guard< std::mutex > lock_eager( _eager_mutex );

    // Set as eagerly marked.
    object->set_gcflags( YL_GCFLAG_EAGER );

    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour == _marked ) )
        return;

    // If the object is unmarked or grey both then we need to mark it now.
    std::lock_guard< std::mutex > lock_grey( _grey_mutex );

    // Call eager_mark routine.
    if ( type->eager_mark )
    {
        type->eager_mark( this, object );
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
    assert( object->check_gcflags( YL_GCFLAG_EAGER ) );
    _eager.erase( object );
    object->clear_gcflags( YL_GCFLAG_EAGER );
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
    mark_impl_mutator( object );
}


bool yl_gcheap::weak_obtain_impl( yl_gcobject* object )
{
    assert( _weak_locked );
    assert( object->check_gcflags( YL_GCFLAG_WEAK ) );

    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_UNLIKELY( colour == _unmarked ) )
    {
        
        /*
            Resurrecting objects is not simple, because by resurrecting an object
            you not only make it live, but also all the objects it references
            must be made live as well.  Once the mark phase is over and sweeping
            has begun, any non-leaf unmarked weak objects are on death row and can
            no longer be resurrected.
        */
        
        std::lock_guard< std::mutex > lock( _grey_mutex );
    
        if ( YL_UNLIKELY( object->check_gcflags( YL_GCFLAG_LEAF ) ) )
        {
            // Leaf objects can always be resurrected.
            gcheader->colour.store( _marked, std::memory_order_relaxed );
        }
        else
        {
            // Check if its too late to ressurect this object.
            if ( ! _can_resurrect )
            {
                return false;
            }

            // Mark object grey so that its children are marked.
            gcheader->colour.store( YL_GCCOLOUR_GREY, std::memory_order_relaxed );
            _grey_list.push_back( object );
        }        
    }
    
    // Obtain root reference to object.    
    gcheader->refcount += 1;
    if ( YL_UNLIKELY( gcheader->refcount == 1 ) )
    {
        add_root( object );
    }
    
    return true;
}


void yl_gcheap::mark_impl_collect( yl_gcobject* object )
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


void yl_gcheap::mark_impl_mutator( yl_gcobject* object )
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

            // Get type.
            uint8_t index = yl_kind_to_index( object->kind() );
            yl_gctype* type = _types.at( index );

            yl_gcheader* gcheader = &object->_gcheader;
            if ( ! type->eager_mark )
            {
                // Check if we've marked this object already.
                yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
                if ( YL_UNLIKELY( colour == _marked ) )
                    continue;

                // Should have been marked grey when it was added to the list.
                assert( colour == YL_GCCOLOUR_GREY );

                // Mark the object.
                mark_object( object );
            }
            else
            {
                // Lock eager during marking.
                std::lock_guard< std::mutex > lock_eager( _eager_mutex );
            
                // Check colour.
                yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
                if ( YL_UNLIKELY( colour == _marked ) )
                    continue;
                
                // Should have been marked grey when it was added to the list.
                assert( colour == YL_GCCOLOUR_GREY );

                // If it's unmarked then it must be unlocked.
                assert( ! object->check_gcflags( YL_GCFLAG_EAGER ) );

                // Mark it.
                mark_object( object );
            }
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
    yl_gctype* type = _types.at( index );
    if ( type->mark )
    {
        type->mark( this, object );
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

#if GC_DEBUG_REPORT
    std::vector< std::pair< size_t, size_t > > report_counts;
    report_counts.resize( _types.size() + 1 );
#endif
    
    yl_gcheader* alloc = _alloc_list.load( yl_memory_order_consume );
    while ( alloc )
    {
        yl_gcobject* object = (yl_gcobject*)alloc;
        alloc = alloc->prev.load( yl_memory_order_consume );
        
        // Get type index.
        uint8_t index = yl_kind_to_index( object->kind() );
#if GC_DEBUG_REPORT
        if ( index != yl_kind_to_index( 0 ) )
            report_counts[ index ].first += 1;
        else
            report_counts[ _types.size() ].first += 1;
#endif
        
        // Check colour.
        yl_gcheader* gcheader = &object->_gcheader;
        yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
        if ( YL_LIKELY( colour == _marked ) )
            continue;

#if GC_DEBUG_REPORT
        assert( index != yl_kind_to_index( 0 ) );
        report_counts[ index ].second += 1;
#endif

        // All grey objects should have been marked.
        assert( colour != YL_GCCOLOUR_GREY );
        
        // All roots should have been marked.
        assert( gcheader->refcount == 0 );

        // Call object destructor.
        yl_gctype* type = _types.at( index );
        if ( type->destroy )
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
                
                type->destroy( this, object );
            }
            else
            {
                // Just destroy.
                type->destroy( this, object );
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
        
#if GC_DEBUG_SCRIBBLE
        memset( object, 0xDE, mspace_usable_size( object ) );
#endif
        
        // Free the object.
        mspace_free( _alloc, object );
    }
    
#if GC_DEBUG_REPORT
    printf( "GC : collect_sweep()\n" );
    for ( size_t i = 0; i < _types.size(); ++i )
    {
        yl_gctype* type = _types.at( i );
        if ( type )
        {
            printf
            (
                "    %-10s : %zu / %zu\n",
                type->name,
                report_counts[ i ].first,
                report_counts[ i ].second
            );
        }
    }
    printf
    (
        "    %-10s : %zu / %zu\n",
        "(newobjs)",
        report_counts[ _types.size() ].first,
        report_counts[ _types.size() ].second
    );
#endif
}













