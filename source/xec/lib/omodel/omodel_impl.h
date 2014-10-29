//
//  omodel_impl.h
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OMODEL_IMPL_H
#define OMODEL_IMPL_H


#include <list>
#include <thread>
#include "omodel.h"
#include "ostring.h"



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

    // Allocations.
    ogcbase*    alloc( ocontext* context, ogctype* type, size_t size );
    ostring*    symbol( ocontext* context, ostring* s );
    ostring*    symbol( ocontext* context, const char* s );

    // Garbage collection contexts.
    ocontext*   new_context();
    void        delete_context( ocontext* context );

    // Request collection.
    void        collect();
    
    // Thread safepoint.
    void        safepoint();
    
    // Write barrier.
    void        barrier( ocontext* context, ogcbase* object, ocolour objcol );



private:

    // Collector thread.
    void        collector_thread();


    // List of objects.
    std::mutex              allocs_mutex;
    ogcbase*                allocs;
    std::mutex              objects_mutex;
    ogcbase*                objects;
    
    // Collector state.
    std::thread             thread;
    ocolour                 mark_colour;
    std::list< ocontext* >  contexts;


};


}


#endif
