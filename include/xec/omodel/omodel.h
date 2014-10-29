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

enum ocolour
{
    GREEN   = 0x00, // unmarked/marked/dead
    ORANGE  = 0x01, // unmarked/marked/dead
    PURPLE  = 0x02, // unmarked/marked/dead
    
    PENDING = 0x04, // added to work/global list
    MARKING = 0x05, // marking in progress
    LOCKED  = 0x06, // one or more mutators are blocked until mark completes
    
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


protected:

    void write_barrier();


private:

    void write_barrier_real( oimpl::ocolour object_colour );
    
    std::atomic_uintptr_t   gctype_colour;
    ogcbase*                gcnext;

};





/*
*/

namespace oimpl
{

struct ogctype
{
    ogctype*    parent;
    void        (*mark)( seglist< ogcbase* >* work );
    const char* name;
};

struct ocontext
{
    omodel*     model;
    ocolour     mark_colour;
};

extern __thread ocontext* context;

};


template < typename object_t >
bool ogcbase::is()
{
    uintptr_t t = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ogctype* gctype = (oimpl::ogctype*)( t & ~oimpl::COLOUR );
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
object_t* ogcbase::as()
{
    if ( ! is< object_t >() )
    {
        throw oerror( "expected %s", object_t::gctype->name );
    }
}



void ogcbase::write_barrier()
{
    uintptr_t c = gctype_colour.load( std::memory_order_relaxed );
    oimpl::ocolour colour = (oimpl::ocolour)( c & oimpl::COLOUR );
    if ( colour != oimpl::context->mark_colour )
    {
        write_barrier_real( colour );
    }
}






#endif
