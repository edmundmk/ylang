//
//  otable.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OTABLE_H
#define OTABLE_H


#include "oexpand.h"
#include "okeytable.h"


/*
    Indexable object which provides a table mapping any value to any value.
*/


class otable : public oexpand
{
public:

    static ometatype metatype;

    static otable* alloc();
    
    ovalue  getindex( ovalue key ) const;
    void    setindex( ovalue key, ovalue value );


protected:

    static void mark_table( oworklist* work, obase* object, ocolour colour );

    explicit otable( ometatype* metatype );

private:

    okeytable< ovalue, ovalue > table;

};



/*

*/


inline ovalue otable::getindex( ovalue key ) const
{
    auto lookup = table.lookup( key );
    if ( lookup )
        return lookup->value;
    else
        return ovalue::undefined;
}


inline void otable::setindex( ovalue key, ovalue value )
{
    table.insert( key, value );
}



#endif
