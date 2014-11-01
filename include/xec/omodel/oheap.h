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
*/


#include <seglist.h>

class obase;



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

    void marked_grey( obase* object );
    

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
