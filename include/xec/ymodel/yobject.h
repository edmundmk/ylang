//
//  yobject.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YOBJECT_H
#define YOBJECT_H


#include <assert.h>
#include <atomic>
#include "ymodel.h"


class yobject;
class ymetatype;



/*
    A ymodel object.  The atomic word contains both the mark colour (which
    must be updated atomically) and also packs in an immutable pointer to the
    metatype.  The list of yobjects is so the sweeper can find them.
*/

class yobject
{
public:

    template < typename object_t > bool is();
    template < typename object_t > object_t* as();
    
protected:

    explicit yobject( ymetatype* metatype );
    
    
private:

    friend class ymodel;
    friend class yslots;
    template < typename value_t > friend class ywb;
    template < typename value_t > friend struct ymarktraits;

    void mark();
    void mark( yworklist* work, ycolour colour );
    
    
private:
    
    std::atomic< uintptr_t >    word;
    yobject*                    next;

};




/*
    A ymetatype tells the collector how to mark an object, and identifies the
    type for the yobject safe casts.
*/

struct ymetatype
{
    void (*mark)( yobject* object, yworklist* work, ycolour );
    const char* name;
};




/*
    yroot
*/

template < typename object_t >
class yroot< object_t* >
{
public:

    yroot();
    yroot( const yroot& q );
    yroot( object_t* q );
    yroot& operator = ( const yroot& q );
    yroot& operator = ( object_t* q );
    ~yroot();
    
    operator object_t* () const;
    object_t* operator -> () const;


private:

    object_t* p;

};




/*
    Write-barrier specialization for pointers to yobjects.
*/

template < typename object_t >
class ywb< object_t* >
{
public:

    ywb();
    ywb( const ywb& q );
    ywb( object_t* q );
    ywb& operator = ( const ywb& q );
    ywb& operator = ( object_t* q );
    
    operator object_t* () const;
    object_t* operator -> () const;
    

private:

    template < typename value_t > friend struct ymarktraits;
    
    std::atomic< object_t* > p;

};


template < typename object_t >
struct ywbtraits< object_t* >
{
    typedef ywb< object_t* > wb;
};


template < typename object_t >
struct ymarktraits< ywb< object_t* > >
{
    static void mark( ywb< object_t* >& wb, yworklist* work, ycolour colour );
};



/*
    
*/


/*
    yobject
*/


template < typename object_t >
inline bool yobject::is()
{
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ymetatype* metatype = (ymetatype*)( word & ~Y_COLOUR_MASK );
    return metatype == &object_t::metatype;
}

template < typename object_t >
inline object_t* yobject::as()
{
    if ( is< object_t >() )
        return (object_t*)this;
    else
        return nullptr;
}

inline void yobject::mark()
{
    // Called from write barriers to check if this object requires marking.
    // Objects which contain no references are marked directly.  Other objects
    // are added to the grey list for the mark thread to pick up.

    ycolour mark_colour = yscope::scope->mark_colour;
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ymetatype* metatype = (ymetatype*)( word & ~Y_COLOUR_MASK );
    ycolour colour = (ycolour)( word & Y_COLOUR_MASK );
    
    if ( colour != Y_GREY && colour != mark_colour )
    {
        if ( metatype->mark )
        {
            // Add to the mark list.
            yscope::scope->model->mark_grey( this );
        }
        else
        {
            // Mark directly.
            word = (uintptr_t)metatype | mark_colour;
            this->word.store( word, std::memory_order_relaxed );
        }
    }
}

inline void yobject::mark( yworklist* work, ycolour mark_colour )
{
    // Called from mark thread to mark objects.  Objects are again either
    // added to the work list or marked directly.
    
    uintptr_t word = this->word.load( std::memory_order_relaxed );
    ymetatype* metatype = (ymetatype*)( word & ~Y_COLOUR_MASK );
    ycolour colour = (ycolour)( word & Y_COLOUR_MASK );
    
    if ( colour != Y_GREY && colour != mark_colour )
    {
        // Add to work list.
        word = (uintptr_t)metatype | Y_GREY;
        this->word.store( word, std::memory_order_relaxed );
        work->push_back( this );
    }
    else
    {
        // Mark directly.
        word = (uintptr_t)metatype | mark_colour;
        this->word.store( word, std::memory_order_relaxed );
    }
}




/*
    yroot
*/

template < typename object_t >
inline yroot< object_t* >::yroot()
    :   yroot( nullptr )
{
}

template < typename object_t >
inline yroot< object_t* >::yroot( const yroot& q )
    :   yroot( q.p )
{
}

template < typename object_t >
inline yroot< object_t* >::yroot( object_t* q )
    :   p( q )
{
    if ( q )
        yscope::scope->model->add_root( q );
}

template < typename object_t >
yroot< object_t* >& yroot< object_t* >::operator = ( const yroot& q )
{
    return this->operator = ( (object_t*)q );
}

template < typename object_t >
yroot< object_t* >& yroot< object_t* >::operator = ( object_t* q )
{
    if ( p != q )
    {
        if ( q )
            yscope::scope->model->add_root( q );
        if ( p )
            yscope::scope->model->remove_root( p );
    }
    p = q;
    return this;
}

template < typename object_t >
inline yroot< object_t* >::~yroot()
{
    if ( p )
        yscope::scope->model->remove_root( p );
}

template < typename object_t >
inline yroot< object_t* >::operator object_t* () const
{
    return p;
}

template < typename object_t >
inline object_t* yroot< object_t* >::operator -> () const
{
    return p;
}





/*
    ywb< object_t* >
*/

template < typename object_t >
inline ywb< object_t* >::ywb()
    :   ywb( nullptr )
{
}

template < typename object_t >
inline ywb< object_t* >::ywb( const ywb& q )
    :   ywb( (object_t*)q )
{
}

template < typename object_t >
inline ywb< object_t* >::ywb( object_t* q )
    :   p( q )
{
}

template < typename object_t >
inline ywb< object_t* >& ywb< object_t* >::operator = ( const ywb& q )
{
    return this->operator = ( (object_t*)q );
}

template < typename object_t >
inline ywb< object_t* >& ywb< object_t* >::operator = ( object_t* q )
{
    // Perform write barrier on previous value.
    object_t* p = this->p.load( std::memory_order_relaxed );
    if ( p )
    {
        p->mark();
    }
    
    // Update value.
    this->p.store( q, std::memory_order_relaxed );
    return *this;
}

template < typename object_t >
inline ywb< object_t* >::operator object_t* () const
{
    return p.load( std::memory_order_relaxed );
}

template < typename object_t >
inline object_t* ywb< object_t* >::operator -> () const
{
    return p.load( std::memory_order_relaxed );
}

template < typename object_t >
inline void ymarktraits< ywb< object_t* > >::mark(
        ywb< object_t* >& wb, yworklist* work, ycolour colour )
{
    // Because of the potential race with new objects, must consume.
    object_t* p = wb.p.load( std::memory_order_consume );
    if ( p )
    {
        p->mark( work, colour );
    }
}




#endif
