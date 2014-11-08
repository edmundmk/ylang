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


class oarray : public oexpand
{
public:

    static ometatype metatype;
    
    static oarray* alloc();
    
    ovalue  getindex( size_t i ) const;
    void    setindex( size_t i, ovalue value );


protected:

    static void mark_array( oworklist* work, obase* object, ocolour colour );

    explicit oarray( ometatype* metatype );


private:

    owb< otuple< ovalue >* > values;

};



/*

*/

inline ovalue oarray::getindex( size_t i ) const
{
    if ( values && i < values->size() )
        return values->at( i );
    else
        return ovalue::undefined;
}

inline void oarray::setindex( size_t i, ovalue value )
{
    if ( values && i < values->size() )
        values->at( i ) = value;
    else
        throw oerror( "index out of range" );
}



#endif
