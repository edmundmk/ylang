//
//  oheap.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OHEAP_H
#define OHEAP_H


/*
    This garbage collected heap is a three-colour concurrent collector based
    on this paper:
            http://www.cs.bell-labs.com/who/lorenz/papers/ismm98.pdf

    Mutators are stopped only briefly in order to mark roots and update the
    mark colours.  The fourth colour (grey) indicates that the object has
    been added to a work list and is already visible to the mark thread.
    
    
    An overview:
    
    GC objects are C++ classes that inherit from obase.  Each subclass of obase
    provides an instance of ometaclass which tells the collector how to mark
    objects of this type.
    
    The members of object are value types.  Value types which are references
    must be protected by write barriers (a specialization of owb<>).
    Write barriers ensure that when references are updated, the previous value
    of the slot will be marked by the garbage collector (since the previous
    value was reachable when this GC cycle began).
    
    The correct write barrier to use for an unknown type is provided by
    omark< T >::wb_type.  This will either be the specialization owb< T >,
    or, for types which aren't references (including types such as okeyval
    which contain already write-barriered references), bare T itself.
 
    On the mark thread, the mark routines provided by each object's metaclass
    are used to mark the current value of each reference in an object.  Mark
    routines typically call omark< T >::mark( ... ) to mark the members of
    an object.
    
    Note that both the mark thread and the mutator can access objects
    (specifically, the object's mark colour and any references) concurrently.
    This means:
    
        -  You cannot place references in memory which the mark thread might
            miss (due to a race condition or otherwise) or which might be
            deallocated by the time the mark thread gets around to marking.
            This effectively prohibits GC objects from performing normal C++
            memory allocation.
        -  All references (implemented as owb< T >) must be atomic, otherwise
            it's undefined what the mark thread might read.
 
    Normally the write barriers ensure that race conditions don't matter.  The
    mark thread will read _one_ of the valid values of a reference since the
    GC cycle began, all of which are either new or were reachable at the start
    of the cycle (and therefore have not yet been swept), and any values it
    misses due to reference updates are communicated by the write barrier.
    
    However we do require fences to deal with new objects.  New objects
    are created on the mutator thread already marked.  The write to initialize
    the new object's mark colour by the mutator must be visible when the mark
    thread reads a reference that points to the new object.  This means:
    
        -  A release fence after writing the mark colour, to ensure that this
            write is visible before any write to a reference.
        -  Reading references on the mark thread must use the 'consume' memory
            order, to ensure that the dependent read of the mark colour reads
            the mark colour and not random junk.
 
    
    Each heap also has a table of symbol strings.  Creating a symbol is rare,
    but comparing them is very frequent.
 
*/


#include <seglist.h>

class obase;
class oclass;
class ostring;



/*
    Mark colours.  Green/orange/purple are cycled between.  Grey represents
    an object added to a root or work list but not yet marked.
*/

enum ocolour
{
    O_GREEN     = 0x00,
    O_ORANGE    = 0x01,
    O_PURPLE    = 0x02,
    O_GREY      = 0x03,
    O_COLOUR    = 0x03,
};


/*
    Work list containing objects to be marked.
*/

typedef seglist< obase* > oworklist;



/*
    Heap object.
*/

class oheap
{
public:

    ostring* make_symbol( const char* s );
    ostring* make_symbol( ostring* s );
    
    void mark_grey( obase* object );
    

};




/*
    Per-thread context.
*/

struct ocontext
{
    static __thread ocontext* context;

    oheap*      heap;
    ocolour     mark_colour;
    obase*      allocs;
    oclass*     empty;

};





/*
    GC metatype, which tells the collector how to mark reference objects.
*/

struct ometatype
{
    void (*mark)( oworklist*, obase*, ocolour );
};




/*
    Value types which are object references must be protected by a write
    barrier.  The write barrier is specialized for specific reference types.
    Write barriers should check the _previous_ target of the reference, and if
    it is unmarked and not grey, colour it grey and communicate it to the heap.
    This ensures that the marker thread will see all objects reachable at the
    snapshot time.
*/

template < typename reference_t >
class owb;




/*
    omark tells us which type to use to write barrier a value type, and how to
    mark it.  Values which are object references must provide a write barrier.
    Values containing references must provide a mark routine.  Value types
    requiring neither can use this default specialization.
*/

template < typename value_t >
struct omark
{
    typedef value_t wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};




/*
*/


template < typename value_t >
inline void omark< value_t >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    // Type does not require marking.
}







#endif
