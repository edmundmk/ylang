//
//  oexpand.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OCLASS_H
#define OCLASS_H


#include <hashtable.h>


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
    
    ovalue  getkey( osymbol key ) const;
    void    setkey( osymbol key, ovalue value );
    void    delkey( osymbol key );


protected:

    explicit oexpand( oexpandclass* expandclass );

    oexpandclass*   expandclass;
    ovalue*         properties;

};



/*
    An expandclass describes the layout of an expand's dynamic properties.
*/

// clean up expandclasses if:
//  they have no instances
//  they have no child expandclasses left alive.



class oexpandclass
{
private:

    friend class oexpand;

    ometaclass*     metaclass;
    oexpand*        prototype;
    uint32_t        instance_count;
    uint32_t        property_count;
    hashtable< osymbol, size_t > lookup;
    oexpandclass*   parent;
    hashtable< osymbol, oexpandclass* > children;
    
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

inline ovalue oexpand::getkey( osymbol key ) const
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

inline void oexpand::setkey( osymbol key, ovalue value )
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

inline void oexpand::delkey( osymbol key )
{
    auto lookup = expandclass->lookup.lookup( key );
    if ( lookup )
    {
        // TODO: do gc barrier on previous value.
        properties[ lookup->value ] = ovalue::undefined;
    }
}


#endif



