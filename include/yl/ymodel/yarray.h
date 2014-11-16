//
//  yarray.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YARRAY_H
#define YARRAY_H


#include "yexpand.h"
#include "ytuple.h"
#include "yerror.h"


/*
    Implementation of scripting-language arrays.
*/


class yarray : public yexpand
{
public:

    static yarray* alloc();
    static yarray* alloc( yexpand* prototype );
    
    size_t  length() const;
    yvalue  getindex( size_t i ) const;
    void    setindex( size_t i, yvalue value );

    void    append( yvalue value );
    void    resize( size_t newlength );


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_array( yobject* object, yworklist* work, ycolour colour );

    explicit yarray( ymetatype* metatype, yclass* klass );


    friend class ymodel;
    static yexpand* make_proto();


private:

    void ensure( size_t capacity );

    size_t  count;
    ywb< ytuple< yvalue >* > values;

};



/*

*/

inline size_t yarray::length() const
{
    return count;
}

inline yvalue yarray::getindex( size_t i ) const
{
    if ( values && i < count )
        return values->get( i );
    else
        return yvalue::yundefined;
}

inline void yarray::setindex( size_t i, yvalue value )
{
    if ( values && i < count )
        values->set( i, value );
    else
        throw yerror( "index out of range" );
}



#endif
