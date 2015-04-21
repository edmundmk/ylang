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
    YLOBJ_IS_OBJECT = 0x10,

    // Numbers.
    YLOBJ_NUMBER    = 0x00,

    // User-visible
    YLOBJ_SINGULAR  = 0x01,                     // null, undef, true or false
    YLOBJ_STRING    = 0x02,                     // string or symbol
    YLOBJ_OBJECT    = YLOBJ_IS_OBJECT | 0x03,   // object
    YLOBJ_NATIVE    = YLOBJ_IS_OBJECT | 0x04,   // yl_expose native object
    YLOBJ_ARRAY     = YLOBJ_IS_OBJECT | 0x05,   // dynamic array
    YLOBJ_TABLE     = YLOBJ_IS_OBJECT | 0x06,   // hashtable
    YLOBJ_FUNCOBJ   = 0x07,                     // function
    YLOBJ_COTHREAD  = 0x08,                     // coroutine with stack
    
    // Compiled code
    YLOBJ_PROGRAM   = 0x09,     // code for a single function

    // Internal
    YLOBJ_VALARRAY  = 0x0A,     // fixed-size array of yl_values
    YLOBJ_SLOT      = 0x0B,     // node in an object's class tree
    YLOBJ_UPVAL     = 0x0C,     // implements function closures
};





/*
    The base class of ylang heap objects.
*/

class yl_heapobj
{
public:

    yl_objkind kind() const;


protected:

    explicit yl_heapobj( yl_objkind kind );


private:

    template < typename object_t > friend class yl_heapref;

    yl_objkind                      _kind;
    std::atomic< yl_mark_colour >   _colour;

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



inline yl_objkind yl_heapobj::kind() const
{
    return _kind;
}



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
        yl_mark_colour colour =
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
