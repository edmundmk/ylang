//
//  gc_heap.h
//
//  Created by Edmund Kapusniak on 19/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef GC_HEAP_H
#define GC_HEAP_H


/*
    This garbage collector provides fully concurrent collection for a single
    mutator thread and a small number of garbage-collected object types.
    
    It uses a write barrier for reference updates which is lock-free in the
    common case.
       
    The collector provides:
 
        -  Allocations.
        -  Write barrier for reference updates.
        -  Root set updates.
        -  Stack reference tracking.
        -  An 'extra reference' per-object for optional out-of-line data.
        -  Weak references, including a kind of finalization.
        -  Lazy marking for objects with frequent reference updates.
        -  Extraction of references from NaN-boxed values.
 
 
    You register types with the collector.  Each type is indexed with a
    small integer.  A GC type has the following:
    
        small integer
        mark information:
            offset to number of elements (0 for non-arrays)
            stride (0 for non-arrays)
            offset to each reference (
        destructor
        weak reference lock
        lazy mark flag
 
 
    weak references work like this:
    
        the mutator has a strong reference to the object.
        the mutator locks the weakref lock
        the mutator adds a weakref to the object to some data structure.
        the mutator unlocks the weakref lock
        
        the mutator locks the weakref lock
        the mutator retrieves the object from the data structure.
            and notifies the GC that the object is resurrected (converts
                weakref to strong ref).
        the mutator unlocks the weakref lock.
        
        the sweeper locks the weakref lock.
        checks if the object is still dead (hasn't been resurrected).
        and calls the destructor of the object.
            which removes the object from the weakref data structure.
        the sweeper unlocks the weakref lock.
        
        
    lazy marking works like this:
 
        all objects of this type start off locked.
        reference updates on locked objects are not valid.
        
        the collector knows about unlocked objects and will mark them
            on the mutator thread at the start of a collection.
 
        to unlock an object, all of its references are marked on the mutator
            thread.
            
        I think...
 
 
 
    object
        <objref>
        <objref>
 
    expobj
        <objref>
        <objref>
        <native pointer>
 
    array
        <objref>
 
    table
        size_t
        <objref>
 
    string
        ---
        (weak - symbol table)
 
    funcobj
        ---
        <objref>
        <objref>
 
    thunkobj
        ---
 
    cothread
        (lazy marking)
 
    program
        <string>
        <valref...>
 
    valarrray
        size
        <elements>
 
    bucketlist
        size
        elements:
            <object>
            <object>
            next
 
    slot
        <heapobj>
        <string>
        (weak - slot tree)
 
    upval
        bool
        unsigned
        <value>
 
*/


#include <stdint.h>
#include <vector>
#include <unordered_set>
#include <atomic>
#include <seglist.h>
#include <anyalias.h>


class gc_heap;
class gc_object;
struct gc_type;
template < typename object_t > class gc_heapref;
template < typename object_t > class gc_stackref;



/*
    Convert between object kinds (which for NaN-boxing reasons count down from
    0xFF) and type indexes (which are small integers counting up from 0).
*/

uint8_t gc_kind_to_index( uint8_t kind );
uint8_t gc_index_to_kind( uint8_t index );




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
static const std::memory_order gc_memory_order_mark = std::memory_order_consume;
#else
static const std::memory_order gc_memory_order_mark = std::memory_order_relaxed;
#endif


/*
    Global GC heap.
*/

extern __thread gc_heap* gc_current;






/*
    The collector uses three object colours.  Each epoch, the colours are
    cycled, dead <- unmarked <- marked.  Objects that were:
    
      -  Marked in the previous epoch are live but unmarked in this one.
      -  Unmarked in the previous epoch are now dead.
      -  Dead have been swept, and the dead colour reycled as the mark colour.
 
*/

enum gc_colour : uint8_t
{
    GC_YELLOW,              // marked/unmarked/dead
    GC_PURPLE,              // marked/unmarked/dead
    GC_ORANGE,              // marked/unmarked/dead
};


/*
 
*/

enum gc_flags : uint8_t
{
};



/*
    GC types.
*/

struct gc_type
{
    void (*mark)( gc_heap* heap, gc_object* object );
    void (*destroy)( gc_heap* heap, gc_object* object );
};



/*
    The GC heap.
*/

class gc_heap
{
public:

    gc_heap();
    ~gc_heap();
    
    
    /*
        Register GC type.
    */
    
    void add_type( uint8_t kind, const gc_type& type );
    
    
    /*
        Allocate a new object.  The object must be correctly constructed
        with the correct type kind and with all references given valid
        values before the next call to allocate() or collect().
        
        alloc() can trigger a collection, so all live objects must be
        reachable.
    */
    
    void* allocate( size_t size );
    
    
    /*
        Explicitly trigger a collection.  All live objects must be reachable
        from a root or from a stackref.  All reference updates must be
        write-barriered.  Call collect_wait() to wait for a collection to
        complete (except don't because that is slow).
    */
    
    void collect();
    void collect_wait();
    
    
    /*
        Write barrier.  The argument is the previous value of an updated
        reference.
    */
    
    void write_barrier( gc_object* object );
    
    
    /*
        Out-of-line references provide a way to associate an extra GC-visible
        reference with an object.  Used for situations where 99% of the time
        the extra reference would be null.
    */
    
    void set_oolref( gc_object* object, gc_object* ool );
    gc_object* get_oolref( gc_object* object );
    
    
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
             structure, call weak_obtain() to ressurect a strong reference
             from the weak reference.
          -  Provide a destructor for weakly referenced objects which
             removes the object from the data structure.  Destructors are
             called from the sweep thread, but weakly referenced objects
             will be destroyed with the weak lock held.
     
    */
    
    void weak_lock();
    gc_object* weak_create( gc_object* weak );
    template < typename object_t > gc_stackref< object_t > weak_obtain( gc_object* weak );
    void weak_unlock();
    
    
    /*
        Eager marking avoids the cost of the write barrier for objects which
        experience very frequent reference updates.  References are not marked
        during the normal GC mark phase.  Instead references are marked either
        at the start of a collection or when the eagerly marked object is
        locked.  Updating references without locking the object is invalid.
    */
    
    void eager_lock( gc_object* eager );
    void eager_unlock( gc_object* eager );
    
    
    
    /*
        Call only from a mark routine.  References should be loaded atomically
        using gc_memory_order_mark.
    */
    
    void mark( gc_object* object );
    
    
    
private:


    // Allocator.
    std::mutex  _alloc_mutex;
    void*       _alloc;
    

    // Types.
    
    std::vector< gc_type > _types;


    // Mutator state.

    std::unordered_set< gc_object* > _roots;
    seglist< anypointer_t* > _stackrefs;
    
    
    // Shared state that updates at the epoch.
    
    gc_colour _dead;
    gc_colour _unmarked;
    gc_colour _marked;


};


/*
    Base class for all GC objects.
*/

class gc_object
{
public:

    uint8_t kind() const;

    void incref();
    void decref();


protected:

    explicit gc_object( uint8_t kind );


private:

    friend class gc_heap;

    std::atomic< gc_colour > _colour;

    uint8_t _kind;      // object type code.
    uint8_t _refcount;  // nonzero for root objects.
    uint8_t _flags;     // flags.

};



/*
    A reference to a GC object from another GC object.
*/

template < typename object_t >
class gc_heapref
{
public:

    gc_heapref();
    gc_heapref( object_t* p );
    
    void        set( object_t* p );
    object_t*   get() const;
    
    
private:

    gc_heapref( const gc_heapref& ) = delete;
    gc_heapref& operator = ( const gc_heapref& ) = delete;
    
    std::atomic< object_t* > _p;

};



/*
    A reference to a GC object from the stack.
*/

template < typename object_t >
class gc_stackref
{
public:

    gc_stackref();
    gc_stackref( object_t* p );
    ~gc_stackref();
    
    
private:

    object_t* _p;

};



/*
    Allocate new GC objects.
*/

template < typename object_t, typename ... arguments_t >
gc_stackref< object_t > gc_new( arguments_t&& ... arguments );




/*

*/


inline uint8_t gc_kind_to_index( uint8_t kind )
{
    return ~kind;
}

inline uint8_t gc_index_to_kind( uint8_t index )
{
    return ~index;
}



inline void gc_heap::write_barrier( gc_object* object )
{
    gc_colour colour = object->_colour.load( std::memory_order_relaxed );
    if ( colour != _marked )
    {
    }
    
}






#endif




