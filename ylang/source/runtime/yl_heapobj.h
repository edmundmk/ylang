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
    // Flags.
    YLOBJ_IS_OBJECT     = 0x80,     // inherits from yl_object

    // User-visible
    YLOBJ_NULL          = 0x00,     // null
    YLOBJ_UNDEF         = 0x01,     // undefined (internal)
    YLOBJ_BOOL          = 0x02,     // true or false
    YLOBJ_NUMBER        = 0x03,     // number
    YLOBJ_STRING        = 0x04,                     // string or symbol
    YLOBJ_OBJECT        = YLOBJ_IS_OBJECT | 0x05,   // object
    YLOBJ_NATIVE        = YLOBJ_IS_OBJECT | 0x06,   // yl_expose native object
    YLOBJ_ARRAY         = YLOBJ_IS_OBJECT | 0x07,   // dynamic array
    YLOBJ_TABLE         = YLOBJ_IS_OBJECT | 0x08,   // hashtable
    YLOBJ_FUNCOBJ       = 0x09,                     // function
    YLOBJ_THUNK         = 0x0A,                     // thunk to native function
    YLOBJ_COTHREAD      = 0x0B,                     // coroutine with stack
    
    // Compiled code
    YLOBJ_PROGRAM       = 0x0C,     // code for a single function

    // Internal
    YLOBJ_VALARRAY      = 0x0D,     // fixed-size array of yl_valrefs
    YLOBJ_BUCKETLIST    = 0x0E,     // fixed-size array of table buckets
    YLOBJ_SLOT          = 0x0F,     // node in an object's class tree
    YLOBJ_UPVAL         = 0x10,     // implements function closures
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

    friend class yl_valref;
    template < typename object_t > friend class yl_objref;

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
object_t* yl_objref< object_t >::get() const
{
    return _p.load( std::memory_order_relaxed );
}





#endif
