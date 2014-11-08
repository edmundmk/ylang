//
//  ytuple.h
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YTUPLE_H
#define YTUPLE_H


#include "yobject.h"


/*
    Tuples are fixed-length arrays of values.
*/

template < typename element_t >
class ytuple : public yobject
{
public:

    typedef typename ywbtraits< element_t >::wb element_type;

    static ytuple* alloc( size_t size );
    
    size_t                  size() const;
    const element_type&     at( size_t i ) const;
    element_type&           at( size_t i );
    const element_type&     get( size_t i ) const;
    void                    set( size_t i, const element_type& v );
    
    
protected:

    friend class yobject;
    friend class yvalue;
    static ymetatype metatype;
    static void mark_tuple( yobject* object, yworklist* work, ycolour colour );

    ytuple( ymetatype* metatype, size_t size );


private:
    
    size_t          tsize;
    element_type    tvalues[];
    
};



/*

*/

template < typename element_t >
ytuple< element_t >* ytuple< element_t >::alloc( size_t size )
{
    void* p = malloc( sizeof( ytuple ) + sizeof( element_type ) * size );
    return new ( p ) ytuple( &metatype, size );
}

template < typename element_t >
ytuple< element_t >::ytuple( ymetatype* metatype, size_t size )
    :   yobject( metatype )
    ,   tsize( size )
{
    for ( size_t i = 0; i < tsize; ++i )
    {
        new ( tvalues + i ) element_type();
    }
}

template < typename element_t >
inline size_t ytuple< element_t >::size() const
{
    return tsize;
}

template < typename element_t >
inline const typename ytuple< element_t >::element_type&
                ytuple< element_t >::at( size_t i ) const
{
    return tvalues[ i ];
}

template < typename element_t >
inline typename ytuple< element_t >::element_type&
                ytuple< element_t >::at( size_t i )
{
    return tvalues[ i ];
}

template < typename element_t >
inline const typename ytuple< element_t >::element_type&
                ytuple< element_t >::get( size_t i ) const
{
    return tvalues[ i ];
}

template< typename element_t >
inline void ytuple< element_t >::set( size_t i, const element_type& value )
{
    tvalues[ i ] = value;
}

template < typename element_t >
ymetatype ytuple< element_t >::metatype = { &mark_tuple, "tuple" };

template < typename element_t >
void ytuple< element_t >::mark_tuple( yobject* object, yworklist* work, ycolour colour )
{
    ytuple< element_t >* tuple = (ytuple< element_t >*)object;
    for ( size_t i = 0; i < tuple->tsize; ++i )
    {
        ymark( tuple->tvalues[ i ], work, colour );
    }
}




#endif


