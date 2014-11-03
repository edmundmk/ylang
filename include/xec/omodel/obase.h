//
//  obase.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OBASE_H
#define OBASE_H


#include <atomic>
#include "oheap.h"



/*
    Base of all GC objects.  The atomic word contains both the mark colour
    and an immutable pointer to the metatype.  There is a global list of
    GC objects so the sweeper can find them all.
*/

class obase
{
public:

    template < typename object_t > bool is();
    

    bool mark( ocolour check, ocolour mark );


protected:

    obase( ometatype* metatype );
    

private:

    std::atomic< uintptr_t >    word;
    obase*                      next;

};



/*
    Write barriers for pointers to garbage collected objects.  The pointer
    is atomic as both the garbage collector and mutator threads access it 
    without locking.
*/

template < typename reftype_t >
class owb< reftype_t* >
{
public:

    owb();
    owb& operator = ( reftype_t* q );
    owb& operator = ( const owb& q );

    operator reftype_t* () const;
    reftype_t* operator -> () const;
    reftype_t* consume() const;
    

private:

    std::atomic< reftype_t* > p;
    
};


template < typename reftype_t >
struct omark< reftype_t* >
{
    typedef owb< reftype_t* > wb_type;
    static void mark( const wb_type& value, oworklist* work, ocolour colour );
};




/*
*/




/*
    obase
*/


inline obase::obase( ometatype* metatype )
    :   word( (uintptr_t)metatype | ocontext::context->mark_colour )
    ,   next( ocontext::context->allocs )
{
    ocontext::context->allocs = this;
    
    // Ensure that the initialization of our word (with its colour) is
    // visible before any store of this new object into a slot visible
    // to the collector thread.
    std::atomic_thread_fence( std::memory_order_release );
}


template < typename object_t >
inline bool obase::is()
{
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ometatype* metatype = (ometatype*)( word & ~O_COLOUR );
    return metatype == &object_t::metatype;
}


inline bool obase::mark( ocolour check, ocolour mark )
{
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ocolour colour = (ocolour)( word & O_COLOUR );
    if ( colour != O_GREY && colour != check )
    {
        word = ( word & ~O_COLOUR ) | mark;
        this->word.store( word, std::memory_order_relaxed );
        return true;
    }
    return false;
}


/*
    owb< reftype_t* >
*/

template < typename reftype_t >
inline owb< reftype_t* >::owb()
    :   p( nullptr )
{
}

template < typename reftype_t >
inline owb< reftype_t* >& owb< reftype_t* >::operator = ( reftype_t* q )
{
    // If old value is not marked, mark grey and submit to mutator.
    reftype_t* p = this->p.load( std::memory_order_relaxed );
    if ( p && p->mark( ocontext::context->mark_colour, O_GREY ) )
    {
        ocontext::context->heap->marked_grey( p );
    }

    // Update value.
    this->p.store( q, std::memory_order_relaxed );
    return *this;
}

template < typename reftype_t >
inline owb< reftype_t* >& owb< reftype_t* >::operator = ( const owb& q )
{
    return this->operator = ( (reftype_t*)q );
}

template < typename reftype_t >
inline owb< reftype_t* >::operator reftype_t* () const
{
    return p.load( std::memory_order_relaxed );
}

template < typename reftype_t >
inline reftype_t* owb< reftype_t* >::operator -> () const
{
    return p.load( std::memory_order_relaxed );
}

template < typename reftype_t >
inline reftype_t* owb< reftype_t* >::consume() const
{
    return p.load( std::memory_order_consume );
}


template < typename reftype_t >
inline void omark< reftype_t* >::mark(
                const wb_type& value, oworklist* work, ocolour colour )
{
    reftype_t* p = value.consume();
    if ( p && p->mark( colour, O_GREY ) )
    {
        work->push_back( p );
    }
}






#endif
