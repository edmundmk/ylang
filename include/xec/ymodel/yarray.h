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
    
    yvalue  getindex( size_t i ) const;
    void    setindex( size_t i, yvalue value );


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_array( yobject* object, yworklist* work, ycolour colour );

    explicit yarray( ymetatype* metatype );


private:

    ywb< ytuple< yvalue >* > values;

};



/*

*/

inline yvalue yarray::getindex( size_t i ) const
{
    if ( values && i < values->size() )
        return values->get( i );
    else
        return yvalue::undefined;
}

inline void yarray::setindex( size_t i, yvalue value )
{
    if ( values && i < values->size() )
        values->set( i, value );
    else
        throw yerror( "index out of range" );
}



#endif
