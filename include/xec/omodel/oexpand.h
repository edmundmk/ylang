//
//  oexpand.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OCLASS_H
#define OCLASS_H


#include <hashtable.h>
#include "omodel.h"
#include "ovalue.h"


class oexpand;
class oexpanddata;
class oexpandclass;



/*
    oexpand is the base class for every omodel object (except strings).  Expand
    properties are stored in the expand data using a hidden class system.
*/

class oexpand : public ogcbase
{
public:

    static oexpand* alloc();
    
    ovalue  getkey( osymbol key ) const;
    void    setkey( osymbol key, ovalue value );
    void    delkey( osymbol key );


private:

    friend class ogcbase;
    static oimpl::ogctype gctype;
    static void mark( ogcbase*, oimpl::oworklist*, oimpl::ocolour, oimpl::ocolour );
    static void free( ogcbase* );

    void expand_key( osymbol key, ovalue value );

    oexpanddata* expand;

};



/*
    Expand data stores expand properties.
*/

struct oexpanddata
{
    oexpandclass*   expandclass;
    ovalue          properties[];
};



/*
    An expandclass describes the layout of an expand's dynamic properties.
    expandclasses should be cleaned up if they have no instances and they
    have no child expandclasses left alive.
*/

class oexpandclass : public ogcbase
{
private:

    friend class ogcbase;
    static oimpl::ogctype gctype;
    static void mark( ogcbase*, oimpl::oworklist*, oimpl::ocolour, oimpl::ocolour );
    static void free( ogcbase* );

    friend class oexpand;
    friend class omodel;

    oexpand*                            prototype;
    uint32_t                            instance_count;
    uint32_t                            property_count;
    hashtable< osymbol, size_t >        lookup;
    oexpandclass*                       parent;
    hashtable< osymbol, oexpandclass* > children;
    
};




/*
*/

template < typename object_t >
inline bool ovalue::is() const
{
    return is_expand() && ( (oexpand*)( x & POINTER_MASK ) )->is< object_t >();
}

template < typename object_t >
inline object_t* ovalue::as() const
{
    if ( is< object_t >() )
        return (object_t*)( x & POINTER_MASK );
    else
        throw oerror( "expected %s", object_t::gctype->name );
}




inline ovalue oexpand::getkey( osymbol key ) const
{
    if ( expand )
    {
        auto lookup = expand->expandclass->lookup.lookup( key );
        if ( lookup )
        {
            ovalue property = expand->properties[ lookup->value ];
            if ( ! property.is_undefined() )
            {
                return property;
            }
        }

        if ( expand->expandclass->prototype )
        {
            return expand->expandclass->prototype->getkey( key );
        }
    }

    throw oerror( "lookup failed .%s", key.c_str() );
}

inline void oexpand::setkey( osymbol key, ovalue value )
{
    write_barrier();

    if ( expand )
    {
        auto lookup = expand->expandclass->lookup.lookup( key );
        if ( lookup )
        {
            expand->properties[ lookup->value ] = value;
            return;
        }
    }
    
    expand_key( key, value );
}

inline void oexpand::delkey( osymbol key )
{
    write_barrier();

    if ( expand )
    {
        auto lookup = expand->expandclass->lookup.lookup( key );
        if ( lookup )
        {
            expand->properties[ lookup->value ] = ovalue::undefined;
        }
    }
}


#endif



