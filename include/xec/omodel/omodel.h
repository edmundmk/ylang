//
//  omodel.h
//
//  Created by Edmund Kapusniak on 25/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OMODEL_H
#define OMODEL_H


#include <stdint.h>
#include <assert.h>
#include <atomic>
#include <seglist.h>


class omodel;
class ocontext;
class ogcbase;
class oexpanddata;


namespace oimpl
{

class omodel;

struct ocontext;
struct ogctype;

typedef seglist< ogcbase* > oworklist;

enum ocolour
{
    GREEN   = 0x00,     // unmarked/marked/dead
    ORANGE  = 0x01,     // unmarked/marked/dead
    PURPLE  = 0x02,     // unmarked/marked/dead
    
    PENDING = 0x04,     // added to work/global list
    MARKING = 0x05,     // marking in progress
    LOCKED  = 0x06,     // mutators are blocked until mark completes
    
    COLOUR  = 0x07,
};

}




/*
    The omodel itself.
*/

class omodel
{
public:

    omodel();
    ~omodel();

private:

    friend class ocontext;
    
    oimpl::omodel* p;

};



/*
    A thread must open an ocontext before it can manipulate omodel objects.
*/

class ocontext
{
public:

    explicit ocontext( omodel* model );
    ~ocontext();

};



/*
    Indicate a safepoint (where GC handshakes can occur), and request a
    garbage collection cycle.
*/

void ocollect();
void osafepoint();




/*
    Base class for all objects in the omodel.  Stores a word containing both
    a pointer to the object's gctype (which is immutable) and the mark colour
    (which is updated during GC marking).
*/

class ogcbase
{
public:

    template < typename object_t > bool is();
    template < typename object_t > object_t* as();
    template < typename object_t > object_t* unchecked_as();

    void gcmark( oimpl::oworklist* work, oimpl::ocolour unmarked );
    void gcmark( oimpl::ocolour marked );


protected:

    ~ogcbase();

    void write_barrier();


private:

    friend class oimpl::omodel;

    void write_barrier_real( uintptr_t word );
    
    std::atomic_uintptr_t   gctype_colour;
    ogcbase*                gcnext;

};





/*
*/

namespace oimpl
{

struct alignas( 8 ) ogctype
{
    ogctype*    parent;
    void        (*mark)( ogcbase*, oworklist*, ocolour, ocolour );
    void        (*free)( ogcbase* );
    const char* name;
};

struct ocontext
{
    omodel*     model;
    ocolour     marked_colour;
    ocolour     unmarked_colour;
};

extern __thread ocontext* context;

};


template < typename object_t >
inline bool ogcbase::is()
{
    uintptr_t word = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ogctype* gctype = (oimpl::ogctype*)( word & ~oimpl::COLOUR );
    
    do
    {
        if ( gctype == &object_t::gctype )
            return true;
        gctype = gctype->parent;
    }
    while ( gctype );
    
    return false;
}


template < typename object_t >
inline object_t* ogcbase::as()
{
    if ( ! is< object_t >() )
    {
        throw oerror( "expected %s", object_t::gctype->name );
    }
}


inline void ogcbase::write_barrier()
{
    // Check colour.
    uintptr_t word = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ocolour colour = (oimpl::ocolour)( word & oimpl::COLOUR );
    
    // If we're not marked, we have work to do to unfreeze this object.
    if ( colour != oimpl::context->marked_colour )
    {
        write_barrier_real( word );
    }
}


inline void ogcbase::gcmark( oimpl::oworklist* work, oimpl::ocolour unmarked )
{
    // Load gc word.
    uintptr_t word = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ogctype* gctype = (oimpl::ogctype*)( word & ~oimpl::COLOUR );
    oimpl::ocolour colour = (oimpl::ocolour)( word & oimpl::COLOUR );

    // Objects with references require full marking.
    assert( gctype->mark );

    // Attempt unmarked -> pending transition.
    if ( colour == unmarked )
    {
        uintptr_t pending_word = (uintptr_t)gctype | oimpl::PENDING;
        if ( gctype_colour.compare_exchange_strong(
                        word, pending_word, std::memory_order_relaxed ) )
        {
            work->push_back( this );
        }
        else
        {
            // We were beaten to the punch, nothing to do.
        }
    }
}


inline void ogcbase::gcmark( oimpl::ocolour marked )
{
    // Load gc word.
    uintptr_t word = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ogctype* gctype = (oimpl::ogctype*)( word & ~oimpl::COLOUR );

    // Objects without references can do unmarked -> marked directly.
    assert( ! gctype->mark );
    
    // Just update unconditionally.
    uintptr_t marked_word = (uintptr_t)gctype | marked;
    gctype_colour.store( marked_word, std::memory_order_relaxed );
}






#endif
