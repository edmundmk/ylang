//
//  yl_heap.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_HEAP_H
#define YL_HEAP_H


#include <stdint.h>
#include <atomic>
#include "yl_context.h"


class yl_heapobj;
template < typename object_t > class yl_heapref;




/*
    List of all ylang heap object types.
*/

enum yl_objkind : uint8_t
{
    YLOBJ_NUMBER,

    // User-visible
    YLOBJ_STRING,       // string or symbol
    YLOBJ_OBJECT,       // object
    YLOBJ_NATIVE,       // object linked to native yl_expose
    YLOBJ_ARRAY,        // dynamic array
    YLOBJ_TABLE,        // hashtable
    YLOBJ_FUNCOBJ,      // function
    YLOBJ_COTHREAD,     // coroutine with stack
    
    // Compiled code
    YLOBJ_PROGRAM,      // code for a single function

    // Internal
    YLOBJ_VALARRAY,     // fixed-size array of yl_values
    YLOBJ_SLOT,         // node in an object's class tree
    YLOBJ_UPVAL,        // implements function closures
};



/*
    The base class of ylang heap objects.
*/

class yl_heapobj
{
protected:

    explicit yl_heapobj( yl_objkind kind );


private:

    template < typename object_t > friend class yl_heapref;

    yl_objkind                      _kind;
    std::atomic< yl_markcolour >    _colour;

};



/*
    Write-barriered references to heapobjs.
*/

template < typename object_t >
class yl_heapref
{
public:

    yl_heapref();
    yl_heapref( object_t* p );
    
    void        set( object_t* p );
    object_t*   get() const;
    

private:

    yl_heapref( const yl_heapref& ) = delete;
    yl_heapref& operator = ( const yl_heapref& ) = delete;

    std::atomic< object_t* > _p;

};



/*

*/


template < typename object_t >
inline yl_heapref< object_t >::yl_heapref()
    :   _p( nullptr )
{
}

template < typename object_t >
inline yl_heapref< object_t >::yl_heapref( object_t* p )
    :   _p( p )
{
}

template < typename object_t >
inline void yl_heapref< object_t >::set( object_t* p )
{
    object_t* object = _p.load( std::memory_order_relaxed );
    if ( object )
    {
        yl_markcolour colour =
                object->_colour.load( std::memory_order_relaxed );
        if ( colour == yl_current->unmarked_colour() )
        {
            yl_current->write_barrier( object );
        }
    }
    
    _p.store( p, std::memory_order_relaxed );
}

template < typename object_t >
object_t* yl_heapref< object_t >::get() const
{
    return _p.load( std::memory_order_relaxed );
}





#endif
