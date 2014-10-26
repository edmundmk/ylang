//
//  oexpand.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OCLASS_H
#define OCLASS_H


#include <hashtable.h>
#include "osymbol.h"


class oexpand;
class oexpandclass;
class ometaclass;



/*
    oexpand is the base class for every omodel object (except strings).  Each
    instance points to an expandclass, which points to a metaclass.  The
    expandclass of an instance can change at runtime as properties are added.
    The metaclass remains fixed, however.
*/

class oexpand
{
public:

    static oexpand* alloc();
    
    ovalue  getkey( osymref key ) const;
    void    setkey( osymref key, ovalue value );
    void    delkey( osymref key );


protected:

    explicit oexpand( oexpandclass* expandclass );

    oexpandclass*   expandclass;
    ovalue*         properties;

};



/*
    An expandclass describes the layout of an expand's dynamic properties.
*/

class oexpandclass
{
private:

    friend class oexpand;

    ometaclass*     metaclass;
    oexpand*        prototype;
    size_t          count;
    hashtable< osymref, size_t > lookup;
    hashtable< osymref, oexpandclass* > expand;
    
};



/*
    A metaclass describes the static layout and behaviour of an expand.  Many
    object types (arrays, tables, functions, closures, etc.) are subclasses of
    oexpand in native code.  An expand's metaclass is fixed at compile time.
*/

class ometaclass
{
    ovalue  (*get_index_func)( oexpand*, ovalue );
    void    (*set_index_func)( oexpand*, ovalue, ovalue );
    void    (*mark_func)( oexpand* );
    void    (*free_func)( oexpand* );
};




/*
*/

inline ovalue oexpand::getkey( osymref key ) const
{
    auto lookup = expandclass->lookup.lookup( key );
    if ( lookup )
    {
        ovalue property = properties[ lookup->value ];
        if ( ! property.is_undefined() )
        {
            return property;
        }
    }

    if ( expandclass->prototype )
    {
        // Might it possible to cache prototype properties in the lookup?  The
        // cache would have to be invalidated when a prototype was assigned to.
        return expandclass->prototype->getkey( key );
    }

    throw oerror( "key not found" );
}

inline void oexpand::setkey( osymref key, ovalue value )
{
    auto lookup = expandclass->lookup.lookup( key );
    if ( lookup )
    {
        // TODO: do gc barrier on previous value.
        properties[ lookup->value ] = value;
    }
    else
    {
        // TODO: expand the object by finding/building new expandclass.
    }
}

inline void oexpand::delkey( osymref key )
{
    auto lookup = expandclass->lookup.lookup( key );
    if ( lookup )
    {
        // TODO: do gc barrier on previous value.
        properties[ lookup->value ] = ovalue::undefined;
    }
}


#endif



