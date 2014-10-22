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
#include <functional>



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
    A scope inside which GC objects can be manipulated.
 
    No interaction with a GC heap is valid outside of a gc_scope.  Interacting
    with GC heaps (or objects allocated from such heaps) other than the one
    associated with the current gc_scope is not valid and is likely to cause
    major problems.
    
    Nesting of GC scopes is not supported.
*/


class gc_scope
{
public:

    explicit gc_scope( gc_heap* heap );
    gc_scope( const gc_scope& ) = delete;
    gc_scope& operator = ( const gc_scope& ) = delete;
    ~gc_scope();
    
};




/*
    Kick off garbage collection.
*/

void gc_collect();




/*
    A GC safepoint at which this thread may be suspended by the GC.
    
    In the future there may be a case for safe scopes in which the mutator
    thread can be interrupted.  This would be required where you have more
    than one mutator, at least one of which will not reach a safepoint for
    a non-trivial period.  It's impossible to check for a safepoint when a
    thread is blocked (e.g. on a mutex or condition, or on an IO operation).
*/

void gc_safepoint();





/*
    Allocation.
*/

struct gc_type
{
    static const ptrdiff_t END_SLOTS = -1;

    size_t      size;
    size_t      align;
    ptrdiff_t*  slots;
};


template< typename object_t >
gc_type* gc_typeof();


template < typename object_t, typename ... arguments_t >
gc_auto< object_t > gc_new( arguments_t&& ... arguments );

template < typename object_t >
gc_auto< object_t > gc_new_array( size_t length );





/*
    A global reference.
    
    Roots are neither atomic nor thread-safe.  Two mutators writing to the
    same root without locking is wrong.
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
    object_t& operator [] ( size_t index ) const;
    
    object_t* get() const;


private:

    explicit gc_root( object_t* p );
    gc_root& operator = ( object_t* p );

    gc_impl::gc_root_impl g;

};



/*
    A heap slot.
    
    Slots are neither atomic nor thread-safe - two mutators writing to the same
    slot without locking can cause a data race which will break the assumptions
    made by the GC and probably cause the program to crash.
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
    object_t& operator [] ( size_t index ) const;
    
    object_t* get() const;


private:

    explicit gc_slot( object_t* p );
    gc_slot& operator = ( object_t* p );

    gc_impl::gc_slot_impl g;

};



/*
    A reference from the C++ stack to a GC object.
    
    Autos must exist only on the stack of a mutator thread inside a GC scope,
    and must only be accessed by that particular mutator thread.
*/

namespace gc_impl
{

template < typename object_t >
gc_auto< object_t > make_gc_auto( object_t* p );

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
    object_t& operator [] ( size_t index ) const;
    
    object_t* get() const;


private:

    friend gc_auto gc_impl::make_gc_auto< object_t >( object_t* p );

    explicit gc_auto( object_t* p );
    gc_auto& operator = ( object_t* p );

    gc_impl::gc_auto_impl g;

};




/*
    A scope with custom marking of locals (i.e. not gc_autos).
*/

namespace gc_impl
{

struct gc_mark_scope_impl
{
    std::function< void() > mark;
    gc_mark_scope_impl*     prev;
};

}


class gc_mark_scope
{
public:

    explicit gc_mark_scope( std::function< void() > mark );
    gc_mark_scope( const gc_mark_scope& ) = delete;
    gc_mark_scope& operator = ( const gc_mark_scope& ) = delete;
    ~gc_mark_scope();
    
    
private:

    gc_impl::gc_mark_scope_impl g;

};


void gc_mark( void* p );






/*
    Implementation.
*/


namespace gc_impl
{


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
    gc_heap_impl*       heap;
    gc_auto_impl*       autos;
    gc_mark_scope_impl* markers;
    gc_slot_buffer*     slots;
    gc_mark_buffer*     mark;
    gc_mark_buffer*     allocs;
    std::atomic_bool    handshake;
};




/*
    Internal routines.
*/

void* gc_alloc( gc_type* type, size_t length );
void gc_new_root( gc_root_impl* proot );
void gc_delete_root( gc_root_impl* proot );
void gc_record_slot( gc_slot_impl* slot, uintptr_t value );
void gc_safepoint_impl();
void gc_mark_impl( void* p );



template < typename object_t >
gc_auto< object_t > make_gc_auto( object_t* p )
{
    return gc_auto< object_t >( p );
}


};





inline void gc_safepoint()
{
    if ( gc_impl::context->handshake.load( std::memory_order_relaxed ) )
    {
        gc_impl::gc_safepoint_impl();
    }
}


inline void gc_mark( void* p )
{
    if ( p )
    {
        gc_impl::gc_mark_impl( p );
    }
}






template < typename object_t, typename ... arguments_t >
inline gc_auto< object_t > gc_new( arguments_t&& ... arguments )
{
    void* p = gc_impl::gc_alloc( gc_typeof< object_t >(), 1 );
    object_t* o = new ( p ) object_t(
                    std::forward< arguments_t >( arguments ... ) ... );
    return gc_impl::make_gc_auto( o );
}

template < typename object_t >
inline gc_auto< object_t > gc_new_array( size_t length )
{
    void* p = gc_impl::gc_alloc( gc_typeof< object_t >(), length );
    for ( size_t i = 0; i < length; ++i )
    {
        void* q = (object_t*)p + i;
        new ( q ) object_t();
    }
    return gc_impl::make_gc_auto( (object_t*)p );
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
    :   g( (uintptr_t)p | gc_impl::GREEN )
{
    // Note that slots in new objects start green.
}

template < typename object_t >
inline gc_slot< object_t >& gc_slot< object_t >::operator = ( object_t* p )
{
    // Write barrier.
    uintptr_t value = g.p.load( std::memory_order_relaxed );
    gc_impl::gc_colour mark_colour = gc_impl::context->colour;
    gc_impl::gc_colour slot_colour =
                    (gc_impl::gc_colour)( value & gc_impl::COLOUR );
    assert( slot_colour != gc_impl::BLACK );
    
    // If the slot is clean (i.e. not the mark colour) then record the slot.
    if ( slot_colour != mark_colour )
    {
        gc_impl::gc_record_slot( &g, value );
    }
    
    // Update slot with mark colour.
    assert( ( (uintptr_t)p & gc_impl::COLOUR ) == 0 );
    value = (uintptr_t)p | mark_colour;
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
