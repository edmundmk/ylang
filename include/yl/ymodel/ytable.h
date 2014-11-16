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
    static ytable* alloc( yexpand* prototype );
    
    size_t  length() const;
    yvalue  getindex( yvalue key ) const;
    void    setindex( yvalue key, yvalue value );


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_table( yobject* object, yworklist* work, ycolour colour );

    explicit ytable( ymetatype* metatype, yclass* klass );


    friend class ymodel;
    static yexpand* make_proto();


private:

    friend class yiterator;

    ymap< yvalue, yvalue > table;

};



/*

*/


inline size_t ytable::length() const
{
    return table.size();
}


inline yvalue ytable::getindex( yvalue key ) const
{
    auto lookup = table.lookup( key );
    if ( lookup )
        return lookup->value;
    else
        return yvalue::yundefined;
}


inline void ytable::setindex( yvalue key, yvalue value )
{
    table.insert( key, value );
}



#endif
