//
//  ymodel.h
//
//  Created by Edmund Kapusniak on 07/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YMODEL_H
#define YMODEL_H


#include <unordered_map>
#include <thread>
#include <symbol.h>
#include <seglist.h>


class yobject;
class yclass;
class ystring;
class ysymbol;
class yexpand;


/*
    32-bit machines without fast 64-bit atomics present several problems.
*/

#if defined( __x86_64__ ) || defined( __arm64__ )
#define Y64BIT 1
#endif


/*
    Whether to differentiate between references and numbers in yclasses.
*/

#if 1//! Y64BIT
#define YSLOTS 1
#endif






/*
    The core of ymodel is a concurrent, tri-colour garbage collector.
    Mutators are stopped only to find their roots.  Mutators run concurrently
    with both a mark and a sweep thread.  All references in heap objects
    must be write-barriered, visited by a mark routine, and accessible using
    only a single atomic read.
*/


enum ycolour
{
    Y_GREEN     = 0x00,
    Y_PURPLE    = 0x01,
    Y_ORANGE    = 0x02,
    Y_GREY      = 0x03,
};

static const uintptr_t Y_COLOUR_MASK = (uintptr_t)0x03;

typedef seglist< yobject* > yworklist;



/*
    The ymodel is an environment which supports garbage-collected yobjects,
    fast-hash and fast-compare ysymbols, and an implicit class tree for
    yexpands.  Multiple mutator threads can access the ymodel concurrently,
    but note that lookup and assignment on a yexpand (including on prototypes)
    is _not_ thread-safe.
*/

class ymodel
{
public:

    ymodel();
    ~ymodel();

private:

    friend class yobject;
    friend class ysymbol;
    friend class yexpand;
    template < typename object_t > friend class yroot;

    void add_root( yobject* object );
    void remove_root( yobject* object );

    ystring* make_symbol( const char* s );
    ystring* make_symbol( ystring* s );
    
    yclass* empty_class();
#if YSLOTS
    yclass* expand_class( yclass* klass, ysymbol key, bool is_number );
#else
    yclass* expand_class( yclass* klass, ysymbol key );
#endif
    
    void mark_grey( yobject* object );

    
private:

    std::mutex roots_mutex;
    std::unordered_map< yobject*, size_t > roots;

    std::mutex symbols_mutex;
    std::unordered_map< symkey, ysymbol > symbols;
    
    std::mutex expand_mutex;
    yclass* expand_empty;
    
    std::mutex greylist_mutex;
    yworklist greylist;

};



/*
    All access to a ymodel must be done within the scope of a ycontext.
*/

class ycontext
{
public:

    ycontext();
    ~ycontext();


private:

    friend class ymodel;
    friend class yobject;
    friend class ysymbol;
    friend class yexpand;

    static __thread ycontext* context;
    
    ymodel*             model;
    ycolour             mark_colour;
    yobject*            allocs;

};




/*
    Collections can only occur when it is safe.
*/

void ycollect();
void ysafepoint();





/*
    yroot< yobject > is a root that keeps an object alive.
*/

template < typename value_t >
class yroot;





/*
    ywb< T > is the write barrier for references on the heap.  The
    previous value of a reference is communicated to the garbage collector.
*/

template < typename value_t >
class ywb;


/*
    ywbtraits< T >::wb is the type to use to write-barrier an unknown type.
    Used in situations where generic containers don't know if their contents
    are references or not.
*/

template < typename value_t >
struct ywbtraits
{
    typedef value_t wb;
};



/*
    ymark( wb, work, colour ) marks a write-barried T from the mark thread.
*/

template < typename wb_t >
void ymark( wb_t& wb, yworklist* work, ycolour colour );



/*
    ymarktraits< T > allows partial specialization of ymark.
*/

template < typename wb_t >
struct ymarktraits
{
    static void mark( wb_t& wb, yworklist* work, ycolour colour );
};




/*

*/

template < typename wb_t >
void ymark( wb_t& wb, yworklist* work, ycolour colour )
{
    ymarktraits< wb_t >::mark( wb, work, colour );
}

template < typename wb_t >
void ymarktraits< wb_t >::mark( wb_t& wb, yworklist* work, ycolour colour )
{
};



#endif
