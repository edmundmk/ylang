//
//  yl_gcheap.h
//
//  Created by Edmund Kapusniak on 19/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_GCHEAP_H
#define YL_GCHEAP_H


/*
    This garbage collector provides fully concurrent collection for a single
    mutator thread and a small number of garbage-collected object types. It
    uses a write barrier for reference updates which is lock-free in the common
    case.
*/


#include <stdint.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <seglist.h>
#include <anyalias.h>


class yl_gcheap;
class yl_gcobject;
struct yl_gctype;
template < typename object_t > class yl_heapref;
template < typename object_t > class yl_stackref;


#ifdef __GNUC__
#define YL_LIKELY( condition )      __builtin_expect( condition, 1 )
#define YL_UNLIKELY( condition )    __builtin_expect( condition, 0 )
#else
#define YL_LIKELY( condition )      ( condition )
#define YL_UNLIKELY( condition )    ( condition )
#endif



/*
    Convert between object kinds (which for NaN-boxing reasons count down from
    0xFF) and type indexes (which are small integers counting up from 0).
*/

uint8_t yl_kind_to_index( uint8_t kind );




/*
    Mark routines can encounter references to objects allocated in this epoch.
    To ensure that the initialization of the new object is visible to the mark
    thread, the correct memory ordering when reading references is consume.
    However current compilers promote consume to acquire, which is heavier than
    needed, and current processor architectures imply consume on every load
    anyway.
*/

#define GC_MEMORY_ORDER_CONSUME 1

#if GC_MEMORY_ORDER_CONSUME
static const std::memory_order yl_memory_order_mark = std::memory_order_consume;
#else
static const std::memory_order yl_memory_order_mark = std::memory_order_relaxed;
#endif



/*
    Global GC heap.
*/

extern __thread yl_gcheap* yl_current_gcheap;



/*
    The collector uses two object colours.  Each epoch, the colours are swapped.
    After a garbage collection cycle there should be no unmarked objects.
*/

enum yl_gccolour : uint8_t
{
    YL_GCCOLOUR_GREY,
    YL_GCCOLOUR_PURPLE,
    YL_GCCOLOUR_ORANGE,
};



/*
    Per-object flags.  Updated ONLY from the mutator thread.  GC thread must
    restrict itself to checking them only.
*/

enum yl_gcflags : uint8_t
{
    YL_GCFLAG_LEAF      = ( 1 << 0 ),
    YL_GCFLAG_OOLREF    = ( 1 << 1 ),
    YL_GCFLAG_WEAK      = ( 1 << 2 ),
    YL_GCFLAG_USER1     = ( 1 << 4 ),
    YL_GCFLAG_USER2     = ( 1 << 5 ),
};



/*
    Header for GC objects.
*/

struct yl_gcheader
{
    std::atomic< yl_gcheader* >     prev;       // previous object in heap.
    std::atomic< yl_gccolour >      colour;     // mark colour.
    mutable std::atomic< uint8_t >  gcflags;    // flags.
    uint8_t                         kind;       // object kind.
    uint8_t                         refcount;   // nonzero for root objects.
};



/*
    GC types.
*/

struct yl_gctype
{
    void (*destroy)( yl_gcheap* heap, yl_gcobject* object );
    void (*mark)( yl_gcheap* heap, yl_gcobject* object );
    void (*eager_mark)( yl_gcheap* heap, yl_gcobject* object );
};



/*
    The GC heap.
*/

class yl_gcheap
{
public:

    yl_gcheap();
    ~yl_gcheap();
    
    
    /*
        Register GC type.
    */
    
    void register_type( uint8_t kind, const yl_gctype& type );
    
    
    /*
        Allocate a new object.  The object must be correctly constructed
        with the correct type kind and with all references given valid
        values before the next call to allocate() or collect().
        
        allocate() can trigger a collection, so all live objects must be
        reachable.
    */
    
    void* allocate( size_t size );
    
    
    /*
        Explicitly trigger a collection.  All live objects must be reachable
        from a root or from a stackref.  All reference updates must be
        write-barriered.  Call collect_wait() to wait for a collection to
        complete (except don't because that is slow).
    */
    
    bool collect();
    void collect_wait();
    
    
    /*
        Write barrier.  The argument is the previous value of an updated
        reference.
    */
    
    void write_barrier( yl_gcobject* object );
    
    
    /*
        Out-of-line references provide a way to associate an extra GC-visible
        reference with an object.  Used for situations where 99% of the time
        the extra reference would be null.
    */
    
    void set_oolref( yl_gcobject* object, yl_gcobject* ool );
    yl_gcobject* get_oolref( yl_gcobject* object );
    
    
    /*
        Weak reference locking.  Weak references are bare pointers stored in
        a data structure not visible to the GC.
        
        The mutator must do the following:
        
          -  Lock all accesses to data structures containing weakrefs with
             the weakref lock.
          -  When adding objects to such a data structure, mark an object
             as weakly referenced using weakref_create(), which expects a
             reachable object as its argument.
          -  When retrieving weakly referenced objects from such a data
             structure, call weak_obtain() to resurrect a strong reference
             from the weak reference.
          -  Provide a destructor for weakly referenced objects which
             removes the object from the data structure.  Destructors are
             called from the sweep thread, but weakly referenced objects
             will be destroyed with the weak lock held.
     
    */
    
    void weak_lock();
    template < typename object_t > object_t* weak_create( object_t* weak );
    template < typename object_t > yl_stackref< object_t > weak_obtain( object_t* weak );
    void weak_unlock();
    
    
    /*
        Eager marking avoids the cost of the write barrier for objects which
        experience very frequent reference updates.  References are not marked
        during the normal GC mark phase.  Instead references are marked either
        at the start of a collection or when the eagerly marked object is
        locked.  Updating references without locking the object is invalid.
        
        Objects supporting eager marking must implement both mark(), which
        should call mark() for all references, and eager_mark(), which should
        call eager_mark() for all references.
    */
    
    void eager_lock( yl_gcobject* eager );
    void eager_unlock( yl_gcobject* eager );
    
    
    
    /*
        Call only from a mark routine.  References should be loaded atomically
        using yl_gcmemory_order_mark.
    */
    
    void mark( yl_gcobject* object );
    void eager_mark( yl_gcobject* object );
    
    
    
private:

    friend class yl_gcobject;
    template < typename object_t > friend class yl_stackref;

    static const size_t COLLECT_THRESHOLD = 32 * 1024;
    
    enum collect_state
    {
        IDLE        = 0x01,
        COLLECT     = 0x02,
        QUIT        = 0x08,
    };
    
    void weak_create_impl( yl_gcobject* object );
    
    bool check_state( unsigned state );
    bool update_state( unsigned current, collect_state update );
    collect_state wait_state( unsigned wait );
    
    void collect_thread();

    void write_barrier_impl( yl_gcobject* object );
    bool weak_obtain_impl( yl_gcobject* object );
    void mark_impl( yl_gcobject* object );
    void eager_mark_impl( yl_gcobject* object );
    
    void collect_mark();
    void mark_object( yl_gcobject* object );
    
    void collect_sweep();


    // Types.
    std::vector< yl_gctype >    _types;

    // Allocator.
    std::mutex                  _alloc_mutex;
    void*                       _alloc;
    std::atomic< yl_gcheader* > _alloc_list;
    size_t                      _alloc_bytes;
    
    // Mutator state.
    seglist< anypointer_t* >    _stackrefs;
    std::unordered_set< yl_gcobject* > _roots;
    std::unordered_set< yl_gcobject* > _eager;
    
    // Out-of-line references.
    std::mutex                  _oolref_mutex;
    std::unordered_map< yl_gcobject* , yl_gcobject* > _oolrefs;
    
    // Weak references.
    std::mutex                  _weak_mutex;
    bool                        _weak_locked;
    
    // Collector thread.
    std::mutex                  _collect_mutex;
    std::condition_variable     _collect_condition;
    std::thread                 _collect_thread;
    collect_state               _collect_state;
    
    // Shared state that updates at the epoch.
    yl_gccolour                 _unmarked;
    yl_gccolour                 _marked;

    // Other shared state.
    std::mutex                  _grey_mutex;
    seglist< yl_gcobject* >     _grey_list;
    bool                        _can_resurrect;
    
    // Private state.
    seglist< yl_gcobject* >     _mark_list;

};


/*
    Base class for all GC objects.
*/

class yl_gcobject
{
public:

    uint8_t kind() const;

    void incref();
    void decref();


protected:

    explicit yl_gcobject( uint8_t kind, uint8_t gcflags = 0 );

    void set_gcflags( uint8_t flags ) const;
    void clear_gcflags( uint8_t flags ) const;
    uint8_t check_gcflags( uint8_t flags ) const;


private:

    friend class yl_gcheap;
    
    yl_gcheader _gcheader;

};




/*
    A reference to a GC object from another GC object.
*/

template < typename object_t >
class yl_heapref
{
public:

    yl_heapref();
    yl_heapref( object_t* p );
    
    void        mark() const;
    
    void        set( object_t* p );
    object_t*   get() const;

    
private:

    yl_heapref( const yl_heapref& ) = delete;
    yl_heapref& operator = ( const yl_heapref& ) = delete;
    
    std::atomic< object_t* > _p;

};



/*
    A reference to a GC object from the stack.
*/

template < typename object_t >
class yl_stackref
{
public:

    yl_stackref();
    yl_stackref( object_t* p );
    yl_stackref( const yl_stackref& p );
    template < typename derived_t > yl_stackref( const yl_stackref< derived_t >& p );
    ~yl_stackref();
    
    explicit operator bool () const;
    object_t* operator -> () const;
    object_t* get() const;
    
    object_t* incref() const;
    
    
private:

    object_t* _p;

};



/*
    Allocate new GC objects.
*/

template < typename object_t, typename ... arguments_t >
yl_stackref< object_t > yl_gcnew( arguments_t&& ... arguments );




/*

*/


inline uint8_t yl_kind_to_index( uint8_t kind )
{
    return ~kind;
}



/*
    yl_gcheap
*/

inline void yl_gcheap::write_barrier( yl_gcobject* object )
{
    if ( ! object )
        return;

    yl_gcheader* gcheader = &object->_gcheader;
    
    // Check colour.
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour != _unmarked ) )
        return;

    // Perform main work of write barrier.
    write_barrier_impl( object );
}

template < typename object_t >
inline object_t* yl_gcheap::weak_create( object_t* object )
{
    weak_create_impl( object );
    return object;
}

template < typename object_t >
inline yl_stackref< object_t > yl_gcheap::weak_obtain( object_t* object )
{
    // Check if weak object can be resurrected.
    if ( weak_obtain_impl( object ) )
    {
        return yl_stackref< object_t >( object );
    }
    else
    {
        return nullptr;
    }
}

inline void yl_gcheap::mark( yl_gcobject* object )
{
    if ( ! object )
        return;
    
    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour != _unmarked ) )
        return;

    // Perform main work of marking.
    mark_impl( object );
}

inline void yl_gcheap::eager_mark( yl_gcobject* object )
{
    if ( ! object )
        return;
    
    // Check colour.
    yl_gcheader* gcheader = &object->_gcheader;
    yl_gccolour colour = gcheader->colour.load( std::memory_order_relaxed );
    if ( YL_LIKELY( colour != _unmarked ) )
        return;

    // Perform main work of marking.
    eager_mark_impl( object );
}



/*
    yl_gcobject
*/

inline uint8_t yl_gcobject::kind() const
{
    return _gcheader.kind;
}

inline uint8_t yl_gcobject::check_gcflags( uint8_t flags ) const
{
    uint8_t gcflags = _gcheader.gcflags.load( std::memory_order_relaxed );
    return gcflags & flags;
}



/*
    yl_heapref
*/

template < typename object_t >
inline yl_heapref< object_t >::yl_heapref()
    :   _p( nullptr )
{
}

template < typename object_t >
inline yl_heapref< object_t >::yl_heapref( object_t* p )
    :   _p( p )
{
}

template < typename object_t >
inline void yl_heapref< object_t >::set( object_t* p )
{
    yl_current_gcheap->write_barrier( _p.load( std::memory_order_relaxed ) );
    _p.store( p, std::memory_order_relaxed );
}

template < typename object_t >
inline object_t* yl_heapref< object_t >::get() const
{
    return _p.load( std::memory_order_relaxed );
}

template < typename object_t >
inline void yl_heapref< object_t >::mark() const
{
    yl_current_gcheap->mark( _p.load( yl_memory_order_mark ) );
}



/*
    yl_stackref
*/

template < typename object_t >
inline yl_stackref< object_t >::yl_stackref()
    :   _p( nullptr )
{
    yl_current_gcheap->_stackrefs.push_back( (anypointer_t*)&_p );
}

template < typename object_t >
inline yl_stackref< object_t >::yl_stackref( object_t* p )
    :   _p( p )
{
    yl_current_gcheap->_stackrefs.push_back( (anypointer_t*)&_p );
}

template < typename object_t >
inline yl_stackref< object_t >::yl_stackref( const yl_stackref& p )
    :   _p( p._p )
{
    yl_current_gcheap->_stackrefs.push_back( (anypointer_t*)&_p );
}

template < typename object_t >
template < typename derived_t >
inline yl_stackref< object_t >::yl_stackref( const yl_stackref< derived_t >& p )
    :   _p( p.get() )
{
    yl_current_gcheap->_stackrefs.push_back( (anypointer_t*)&_p );
}

template < typename object_t >
inline yl_stackref< object_t >::~yl_stackref()
{
    anypointer_t* pp = yl_current_gcheap->_stackrefs.back();
    yl_current_gcheap->_stackrefs.pop_back();
    assert( pp == (anypointer_t*)&_p );
}


template < typename object_t >
inline yl_stackref< object_t >::operator bool () const
{
    return _p != nullptr;
}

template < typename object_t >
object_t* yl_stackref< object_t >::operator -> () const
{
    return _p;
}

template < typename object_t >
object_t* yl_stackref< object_t >::get() const
{
    return _p;
}

template < typename object_t >
object_t* yl_stackref< object_t >::incref() const
{
    _p->incref();
    return _p;
}



#endif




