//
//  ytable.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YTABLE_H
#define YTABLE_H


#include "yexpand.h"
#include "ymap.h"


/*
    Indexable object which provides a table mapping any value to any value.
*/


class ytable : public yexpand
{
public:

    static ytable* alloc();
    
    yvalue  getindex( yvalue key ) const;
    void    setindex( yvalue key, yvalue value );


protected:

    friend class yobject;
    friend class yvalue;
    static ymetatype metatype;
    static void mark_table( yobject* object, yworklist* work, ycolour colour );

    explicit ytable( ymetatype* metatype );


private:

    ymap< yvalue, yvalue > table;

};



/*

*/


inline yvalue ytable::getindex( yvalue key ) const
{
    auto lookup = table.lookup( key );
    if ( lookup )
        return lookup->value;
    else
        return yvalue::undefined;
}


inline void ytable::setindex( yvalue key, yvalue value )
{
    table.insert( key, value );
}



#endif
