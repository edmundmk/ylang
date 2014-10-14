//
//  gc.h
//
//  Created by Edmund Kapusniak on 22/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef GC_H
#define GC_H


#include <stdint.h>
#include <assert.h>
#include <cstddef>
#include <new>
#include <atomic>



/*
    A GC which does collection concurrently with mutator threads.  It does
    require all threads to be stopped simultaneously, but attempts to minimize
    pause times.  Threads are only paused for:
    
        -  the time it takes to mark their local stack references.
        -  until all mutator threads have paused and all roots are marked.
        
    Other GC work proceeds concurrently.  Note that a thread does not need to
    wait for other threads to complete their stack marking operation.
    
    Threads can also be paused by the collector in a slot update if their slot
    buffer is full.  This depends on the size of the thread's working set of
    slots, the size of the slot buffer, and the frequency of collection cycles.
*/



class gc_heap;
class gc_scope;
struct gc_type;

template < typename object_t > class gc_root;
template < typename object_t > class gc_slot;
template < typename object_t > class gc_auto;


namespace gc_impl
{

class gc_heap_impl;
class gc_scope_impl;

struct gc_context;
extern __thread gc_context* context;

}






/*
    There can be multiple GC heaps in a single process.  The best way to
    reduce thread contention is to avoid accessing the same heap from more
    than one thread.
    
    There is currently no way to migrate objects between GC heaps.
*/

class gc_heap
{
public:

    gc_heap();
    gc_heap( const gc_heap& ) = delete;
    gc_heap& operator = ( const gc_heap& ) = delete;
    ~gc_heap();

    gc_impl::gc_heap_impl* get() const;


private:

    gc_impl::gc_heap_impl* p;

};





/*
    A scope inside which GC objects can be manipulated.  Pass in a callback if
    you need to do custom marking of locals (i.e. other than gc_auto).
    
    No interaction with a GC heap is valid outside of a gc_scope.  Interacting
    with GC heaps (or objects allocated from such heaps) other than the one
    associated with the current gc_scope is not valid and is likely to cause
    major problems.
    
    Nesting of GC scopes is not valid.
*/


typedef void (*gc_mark_callback)( void* token, void* safepoint );
void gc_mark( void* p );


class gc_scope
{
public:

    gc_scope( gc_heap* heap,
                    gc_mark_callback mark = nullptr, void* token = nullptr );
    gc_scope( const gc_scope& ) = delete;
    gc_scope& operator = ( const gc_scope& ) = delete;
    ~gc_scope();
    
    gc_impl::gc_scope_impl* get() const;

    
private:

    gc_impl::gc_scope_impl* p;
    

};



/*
    Kick off garbage collection.
*/

void gc_collect( void* safepoint = nullptr );




/*
    A GC safepoint at which this thread may be suspended by the GC.
    
    In the future there may be a case for safe scopes in which the mutator
    thread can be interrupted.  This would be required where you have more
    than one mutator, at least one of which will not reach a safepoint for
    a non-trivial period.  It's impossible to check for a safepoint when a
    thread is blocked (e.g. on a mutex or condition, or on an IO operation).
    
    You can achieve a similar effect by leaving the GC scope.
*/

void gc_safepoint( void* safepoint = nullptr );





/*
    Allocation.
*/

struct gc_type
{
};


template< typename object_t >
gc_type* gc_typeof();

template < typename object_t >
gc_type* gc_typeof( gc_auto< object_t > p );

template< typename object_t >
gc_auto< object_t > gc_alloc( gc_type* type );

template< typename object_t, typename ... arguments_t >
gc_auto< object_t > gc_new( arguments_t&& ... arguments );





/*
    A global reference.
*/

namespace gc_impl
{

struct gc_root_impl
{
    void*           p;
    gc_heap_impl*   heap;
    gc_root_impl*   prev;
    gc_root_impl*   next;
};

}


template < typename object_t >
class gc_root
{
public:

    gc_root();
    gc_root( std::nullptr_t );
    gc_root( const gc_root& p );
    template < typename derived_t > gc_root( const gc_root< derived_t >& p );
    template < typename derived_t > gc_root( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_root( const gc_auto< derived_t >& p );
    gc_root& operator = ( std::nullptr_t );
    gc_root& operator = ( const gc_root& p );
    template < typename derived_t > gc_root& operator = ( const gc_root< derived_t >& p );
    template < typename derived_t > gc_root& operator = ( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_root& operator = ( const gc_auto< derived_t >& p );
    ~gc_root();
    
    explicit operator bool () const;
    object_t& operator * () const;
    object_t* operator -> () const;
    
    object_t* get() const;


private:

    explicit gc_root( object_t* p );
    gc_root& operator = ( object_t* p );

    gc_impl::gc_root_impl g;

};



/*
    A heap slot.
*/

namespace gc_impl
{

struct gc_slot_impl
{
    gc_slot_impl( uintptr_t p );

    std::atomic_uintptr_t p;
};

}

template < typename object_t >
class gc_slot
{
public:

    gc_slot();
    gc_slot( std::nullptr_t );
    gc_slot( const gc_slot& p );
    template < typename derived_t > gc_slot( const gc_root< derived_t >& p );
    template < typename derived_t > gc_slot( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_slot( const gc_auto< derived_t >& p );
    gc_slot& operator = ( std::nullptr_t );
    gc_slot& operator = ( const gc_slot& p );
    template < typename derived_t > gc_slot& operator = ( const gc_root< derived_t >& p );
    template < typename derived_t > gc_slot& operator = ( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_slot& operator = ( const gc_auto< derived_t >& p );
    ~gc_slot();
    
    explicit operator bool() const;
    object_t& operator * () const;
    object_t* operator -> () const;
    
    object_t* get() const;


private:

    explicit gc_slot( object_t* p );
    gc_slot& operator = ( object_t* p );

    gc_impl::gc_slot_impl g;

};



/*
    A reference from the C++ stack to a GC object.
*/

namespace gc_impl
{

struct gc_auto_impl
{
    void*           p;
    gc_auto_impl*   prev;
};

}

template < typename object_t >
class gc_auto
{
public:

    gc_auto();
    gc_auto( std::nullptr_t );
    gc_auto( const gc_auto& p );
    template < typename derived_t > gc_auto( const gc_root< derived_t >& p );
    template < typename derived_t > gc_auto( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_auto( const gc_auto< derived_t >& p );
    gc_auto& operator = ( std::nullptr_t );
    gc_auto& operator = ( const gc_auto& p );
    template < typename derived_t > gc_auto& operator = ( const gc_root< derived_t >& p );
    template < typename derived_t > gc_auto& operator = ( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_auto& operator = ( const gc_auto< derived_t >& p );
    ~gc_auto();
    
    explicit operator bool() const;
    object_t& operator * () const;
    object_t* operator -> () const;
    
    object_t* get() const;


private:

    explicit gc_auto( object_t* p );
    gc_auto& operator = ( object_t* p );

    gc_impl::gc_auto_impl g;

};









/*
    Implementation.
*/


namespace gc_impl
{


/*
    Ensures that only pointers to GC objects are managed by our smart pointers.
*/

template < typename object_t >
class gc_pointer
{
public:

    gc_pointer( std::nullptr_t );
    explicit gc_pointer( object_t* o );
    template < typename derived_t > gc_pointer( gc_pointer< derived_t > p );
    template < typename derived_t > gc_pointer( const gc_root< derived_t >& p );
    template < typename derived_t > gc_pointer( const gc_slot< derived_t >& p );
    template < typename derived_t > gc_pointer( const gc_auto< derived_t >& p );
    object_t* get() const;
    
private:

    object_t* g;
    
};



/*
    Slot colours.
*/

enum gc_colour
{
    GREEN   = 0x000,
    PURPLE  = 0x001,
    ORANGE  = 0x002,
    BLACK   = 0x003,
    COLOUR  = 0x003,
};



/*
    GC per-thread context.
*/

class gc_slot_buffer;
class gc_mark_buffer;

struct gc_context
{
    gc_context();

    gc_colour           colour;
    gc_scope_impl*      scope;
    gc_auto_impl*       autos;
    gc_slot_buffer*     slots;
    gc_mark_buffer*     mark;
    std::atomic_bool    handshake;
};




/*
    Internal routines.
*/

gc_type* gc_typeof_impl( void* p );
void* gc_alloc_impl( gc_type* type );
void gc_new_root( gc_root_impl* proot );
void gc_delete_root( gc_root_impl* proot );
void gc_record_slot( gc_slot_impl* pslot );
void gc_safepoint_impl( void* safepoint );
void gc_mark_impl( void* p );


};





inline void gc_safepoint( void* safepoint )
{
    if ( gc_impl::context->handshake.load( std::memory_order_relaxed ) )
    {
        gc_impl::gc_safepoint_impl( safepoint );
    }
}


inline void gc_mark( void* p )
{
    if ( p )
    {
        gc_impl::gc_mark_impl( p );
    }
}





template < typename object_t >
inline gc_type* gc_typeof( gc_impl::gc_pointer< object_t > p )
{
    return gc_typeof_impl( p.get() );
}

template < typename object_t >
inline gc_impl::gc_pointer< object_t > gc_alloc( gc_type* type )
{
    object_t* o = (object_t*)gc_impl::gc_alloc_impl( type );
    return gc_impl::gc_pointer< object_t >( o );
}

template < typename object_t, typename ... arguments_t >
inline gc_impl::gc_pointer< object_t > gc_new( arguments_t&& ... arguments )
{
    void* p = gc_impl::gc_alloc_impl( gc_typeof< object_t >() );
    object_t* o = new ( p ) object_t(
                    std::forward< arguments_t >( arguments ... ) ... );
    return gc_impl::gc_pointer< object_t >( o );
}





template < typename object_t >
inline gc_root< object_t >::gc_root()
    :   gc_root( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_root< object_t >::gc_root( std::nullptr_t )
    :   gc_root( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_root< object_t >::gc_root( const gc_root& p )
    :   gc_root( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >::gc_root( const gc_root< derived_t >& p )
    :   gc_root( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >::gc_root( const gc_slot< derived_t >& p )
    :   gc_root( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >::gc_root( const gc_auto< derived_t >& p )
    :   gc_root( p.get() )
{
}

template < typename object_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( std::nullptr_t )
{
    return *this = (object_t*)nullptr;
}

template < typename object_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( const gc_root& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( const gc_root< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( const gc_slot< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( const gc_auto< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
inline gc_root< object_t >::~gc_root()
{
    gc_delete_root( &g );
}

template < typename object_t >
inline gc_root< object_t >::operator bool() const
{
    return get() != nullptr;
}

template < typename object_t >
inline object_t& gc_root< object_t >::operator * () const
{
    return *get();
}

template < typename object_t >
inline object_t* gc_root< object_t >::operator -> () const
{
    return get();
}

template < typename object_t >
inline object_t* gc_root< object_t >::get() const
{
    return (object_t*)g.p;
}

template < typename object_t >
inline gc_root< object_t >::gc_root( object_t* p )
{
    g.p     = nullptr;
    g.heap  = nullptr;
    g.prev  = nullptr;
    g.next  = nullptr;
    if ( p )
        gc_impl::gc_new_root( &g );
    g.p = p;
}

template < typename object_t >
inline gc_root< object_t >& gc_root< object_t >::operator = ( object_t* p )
{
    if ( p )
        gc_impl::gc_new_root( &g );
    else
        gc_impl::gc_delete_root( &g );
    g.p = p;
    return *this;
}




gc_impl::gc_slot_impl::gc_slot_impl( uintptr_t p )
    :   p( p )
{
}


template < typename object_t >
inline gc_slot< object_t >::gc_slot()
    :   gc_slot( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_slot< object_t >::gc_slot( std::nullptr_t )
    :   gc_slot( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_slot< object_t >::gc_slot( const gc_slot& p )
    :   gc_slot( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >::gc_slot( const gc_root< derived_t >& p )
    :   gc_slot( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >::gc_slot( const gc_slot< derived_t >& p )
    :   gc_slot( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >::gc_slot( const gc_auto< derived_t >& p )
    :   gc_slot( p.get() )
{
}

template < typename object_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( std::nullptr_t )
{
    return *this = (object_t*)nullptr;
}

template < typename object_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( const gc_slot& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( const gc_root< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( const gc_slot< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( const gc_auto< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
inline gc_slot< object_t >::~gc_slot()
{
    *this = nullptr;
}

template < typename object_t >
inline gc_slot< object_t >::operator bool() const
{
    return get() != nullptr;
}

template < typename object_t >
inline object_t& gc_slot< object_t >::operator * () const
{
    return *get();
}

template < typename object_t >
inline object_t* gc_slot< object_t >::operator -> () const
{
    return get();
}

template < typename object_t >
inline object_t* gc_slot< object_t >::get() const
{
    return (object_t*)( g.p & ~gc_impl::COLOUR );
}

template < typename object_t >
inline gc_slot< object_t >::gc_slot( object_t* p )
    :   g( (uintptr_t)p )
{
}

template < typename object_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( object_t* p )
{
    // Write barrier.
    gc_impl::gc_colour mark_colour = gc_impl::context->colour;
    gc_impl::gc_colour slot_colour = (gc_impl::gc_colour )(
            g.p.load( std::memory_order_relaxed ) & gc_impl::COLOUR );
    assert( slot_colour != gc_impl::BLACK );
    
    // If the slot is clean (i.e. not the mark colour) then record the slot.
    if ( slot_colour != mark_colour )
    {
        gc_impl::gc_record_slot( &g );
    }
    
    // Update slot with mark colour.
    assert( ( (uintptr_t)p & gc_impl::COLOUR ) == 0 );
    uintptr_t value = (uintptr_t)p | mark_colour;
    g.p.store( value, std::memory_order_relaxed );
    
    return *this;
}





template < typename object_t >
inline gc_auto< object_t >::gc_auto()
    :   gc_auto( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_auto< object_t >::gc_auto( std::nullptr_t )
    :   gc_auto( (object_t*)nullptr )
{
}

template < typename object_t >
inline gc_auto< object_t >::gc_auto( const gc_auto& p )
    :   gc_auto( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >::gc_auto( const gc_root< derived_t >& p )
    :   gc_auto( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >::gc_auto( const gc_slot< derived_t >& p )
    :   gc_auto( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >::gc_auto( const gc_auto< derived_t >& p )
    :   gc_auto( p.get() )
{
}

template < typename object_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( std::nullptr_t )
{
    return *this = (object_t*)nullptr;
}

template < typename object_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( const gc_auto& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( const gc_root< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( const gc_slot< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
template < typename derived_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( const gc_auto< derived_t >& p )
{
    return *this = p.get();
}

template < typename object_t >
inline gc_auto< object_t >::~gc_auto()
{
    assert( gc_impl::context->autos == &g );
    gc_impl::context->autos = g.prev;
}

template < typename object_t >
inline gc_auto< object_t >::operator bool() const
{
    return get() != nullptr;
}

template < typename object_t >
inline object_t& gc_auto< object_t >::operator * () const
{
    return *get();
}

template < typename object_t >
inline object_t* gc_auto< object_t >::operator -> () const
{
    return get();
}

template < typename object_t >
inline object_t* gc_auto< object_t >::get() const
{
    return (object_t*)g.p;
}

template < typename object_t >
inline gc_auto< object_t >::gc_auto( object_t* p )
{
    g.p     = p;
    g.prev  = gc_impl::context->autos;
    gc_impl::context->autos = &g;
}

template < typename object_t >
inline gc_auto< object_t >& gc_auto< object_t >::operator = ( object_t* p )
{
    g.p = p;
    return *this;
}








#endif
