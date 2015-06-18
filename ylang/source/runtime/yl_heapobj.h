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
template < typename object_t > class yl_objref;




/*
    List of all ylang object types.  Note that this must match
    the object types from yl_valkind.
*/

enum yl_objkind : uint8_t
{
    // User-visible types (with tags that must match the NaN-boxing scheme).
    YLOBJ_OBJECT        = 0xFF,     // object
    YLOBJ_EXPOSE        = 0xFE,     // object for exposed native object
    YLOBJ_ARRAY         = 0xFD,     // object derived from array prototype
    YLOBJ_TABLE         = 0xFC,     // object derived from table prototype
//                        0xFB,
//                        0xFA,
//                        0xF9,
    YLOBJ_QNAN          = 0xF8,     // number: quiet NaN
    YLOBJ_STRING        = 0xF7,     // string
    YLOBJ_FUNCOBJ       = 0xF6,     // bytecode function
    YLOBJ_THUNKOBJ      = 0xF5,     // native function
    YLOBJ_COTHREAD      = 0xF4,     // ylang stack object/coroutine
    YLOBJ_PROGRAM       = 0xF3,     // bytecode program (not user-visible)
//                        0xF2,
    YLOBJ_SINGULAR      = 0xF1,     // null/undefined/true/false
    YLOBJ_INFINITY      = 0xF0,     // number: infinity

    // Not user-visible and never NaN-boxed.
    YLOBJ_VALARRAY      = 0x01,     // fixed-size array of values
    YLOBJ_BUCKETLIST    = 0x02,     // fixed-size array of table buckets
    YLOBJ_SLOT          = 0x03,     // node in an object's class tree
    YLOBJ_UPVAL         = 0x04,     // variable referenced from closure
    
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

    friend class yl_context_impl;

    yl_objkind                      _kind;
    std::atomic< yl_mark_colour >   _colour;

};



/*
    Write-barriered references to heapobjs.
*/

template < typename object_t >
class yl_objref
{
public:

    yl_objref();
    yl_objref( object_t* p );
    
    void        set( object_t* p );
    object_t*   get() const;
    

private:

    yl_objref( const yl_objref& ) = delete;
    yl_objref& operator = ( const yl_objref& ) = delete;

    std::atomic< object_t* > _p;

};



/*

*/


inline yl_objkind yl_heapobj::kind() const
{
    return _kind;
}



template < typename object_t >
inline yl_objref< object_t >::yl_objref()
    :   _p( nullptr )
{
}

template < typename object_t >
inline yl_objref< object_t >::yl_objref( object_t* p )
    :   _p( p )
{
}

template < typename object_t >
inline void yl_objref< object_t >::set( object_t* p )
{
    object_t* object = _p.load( std::memory_order_relaxed );
    yl_current->write_barrier( object );    
    _p.store( p, std::memory_order_relaxed );
}

template < typename object_t >
object_t* yl_objref< object_t >::get() const
{
    return _p.load( std::memory_order_relaxed );
}




inline void yl_context_impl::write_barrier( yl_heapobj* object )
{
    if ( ! object )
    {
        return;
    }
    
    yl_mark_colour colour = object->_colour.load( std::memory_order_relaxed );
    if ( colour != _unmarked_colour )
    {
        return;
    }
    
    write_barrier_mark( object );
}




#endif
