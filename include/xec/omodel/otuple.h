//
//  otuple.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OTUPLE_H
#define OTUPLE_H



#include "obase.h"



/*
    Tuples are fixed-length arrays of value types.
*/

template < typename element_t >
class otuple : public obase
{
public:

    typedef typename omark< element_t >::wb_type elem_type;

    static otuple* alloc( size_t size );
    
    size_t size() const;
    const elem_type& at( size_t index ) const;
    elem_type& at( size_t index );
    
    
protected:

    static ometatype metatype;
    static void mark_tuple( oworklist* work, obase* object, ocolour colour );

    otuple( ometatype* metatype, size_t size );


private:
    
    size_t      ssize;
    elem_type   slots[];
    
};



/*

*/


template < typename element_t >
inline otuple< element_t >* otuple< element_t >::alloc( size_t size )
{
    void* p = malloc( sizeof( otuple ) + sizeof( elem_type ) * size );
    return new ( p ) otuple( &metatype, size );
}

template < typename element_t >
inline otuple< element_t >::otuple( ometatype* metatype, size_t size )
    :   obase( metatype )
    ,   ssize( size )
{
    for ( size_t i = 0; i < size; ++i )
    {
        new ( slots + i ) elem_type();
    }
}

template < typename element_t >
inline size_t otuple< element_t >::size() const
{
    return ssize;
}

template < typename element_t >
inline const typename otuple< element_t >::elem_type&
                otuple< element_t >::at( size_t index ) const
{
    return slots[ index ];
}

template < typename element_t >
inline typename otuple< element_t >::elem_type&
                otuple< element_t >::at( size_t index )
{
    return slots[ index ];
}

template < typename element_t >
ometatype otuple< element_t >::metatype = { &mark_tuple };

template < typename element_t >
void otuple< element_t >::mark_tuple(
                oworklist* work, obase* object, ocolour colour )
{
    otuple< element_t >* tuple = (otuple< element_t >*)object;
    for ( size_t i = 0; i < tuple->ssize; ++i )
    {
        omark< element_t >::mark( tuple->at( i ), work, colour );
    }
}







#endif
