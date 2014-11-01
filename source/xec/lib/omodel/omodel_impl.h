//
//  omodel_impl.h
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OMODEL_IMPL_H
#define OMODEL_IMPL_H


#include <new>
#include <list>
#include <thread>
#include <symbol.h>
#include "omodel.h"
#include "ostring.h"
#include "oexpand.h"



/*
    Private implementation of the omodel.


    Memory management is a concurrent garbage collector based on this paper:
 
        http://www.cs.bell-labs.com/who/lorenz/papers/ismm98.pdf
 
    The mutator thread is only paused to mark roots, and otherwise the
    collector runs concurrently.  There are two key insights in the paper.

    The first is that we can find all objects reachable at a snapshot time
    by having mutators communicate the old values of updated slots to the
    marker thread.
    
    The second is that the mark of the next epoch can run concurrently with
    the sweep of the current one - the sweeper only sweeps objects dead in
    the current epoch, and the marker marks only those live in the next.  The
    two sets cannot overlap as objects stay dead.


    This collector has ended up pretty different from the paper.
    
    We preserve the snapshot property with a write barrier that marks unmarked
    objects in their entirety on the mutator thread before any modifications
    are allowed.  Unmarked objects are effectively 'frozen' and cannot be
    modified until they are marked.  This means that marking is performed on
    the same object graph that was present at the snapshot, but means more
    complicated synchronization between the collector and mutator threads to
    ensure that marking completes before mutators can continue.
    
    The hidden class model means that writes can cause objects to be changed
    in ways not compatible with concurrent gabage collection.  This may change
    in the future - support for multiple concurrent mutators may require safe
    concurrent access to oexpanddata even during/after a class transition
    (probably meaning old expanddatas will have to be kept around and gced only
    when we are sure no thread is accessing them).
 
*/

namespace oimpl
{


class omodel
{
public:

    omodel();
    ~omodel();


    // Allocation.
    template < typename object_t >
    object_t*   alloc( ocontext* context,
                        ogctype* type, size_t size = sizeof( object_t ) );


    // Symbols.
    ostring*    symbol( ocontext* context, const char* s );
    ostring*    symbol( ocontext* context, ostring* s );

    // Garbage collection contexts.
    ocontext*   new_context();
    void        delete_context( ocontext* context );

    // Request collection.
    void        collect();
    
    // Thread safepoint.
    void        safepoint();
    
    // Write barrier.
    void        barrier( ocontext* context, ogcbase* object, uintptr_t word );

    // Expand.
    void        expand_key( ocontext* context,
                        oexpand* expand, osymbol key, ovalue value );


private:


    // Collector threads.
    void marker();
    void sweeper();
    
    
    // Private functions.
    void new_object( ocontext* context, ogctype* type, ogcbase* object );
    void add_roots( oworklist* work );


    // List of objects.
    ogcbase*                objects;
    std::mutex              allocs_mutex;
    ogcbase*                allocs;


    // Expand classes.
    std::mutex              expand_mutex;
    oexpandclass*           expand_empty;


    // Symbol table.
    std::mutex              symbols_mutex;
    hashtable< symkey, ostring* > symbols;

    
    // Collector.
    std::thread             mark_thread;
    std::thread             sweep_thread;
    
    // List of active contexts.
    std::list< ocontext* >  contexts;
    
    // Current mark colours.
    ocolour                 marked_colour;
    ocolour                 unmarked_colour;
    ocolour                 dead_colour;
    
    // Guards for transitioning into and out of LOCKED colour.
    std::mutex              locked_mutex;
    std::condition_variable locked_condition;
    
    // Roots to be marked.
    std::mutex              roots_mutex;
    seglist< ogcbase* >     roots;


};



template < typename object_t >
object_t* omodel::alloc( ocontext* context, ogctype* type, size_t size )
{
    // Allocate object.
    void* p = malloc( size );
    object_t* object = new ( p ) object_t();
    new_object( context, type, object );
    return object;
}




}


#endif
