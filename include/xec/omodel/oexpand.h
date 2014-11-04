//
//  oexpand.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OEXPAND_H
#define OEXPAND_H


#include "obase.h"
#include "otuple.h"
#include "okeytable.h"
#include "ostring.h"
#include "ovalue.h"


class oexpand;
class oclass;



/*
    oexpand is the base class for objects that can have arbitrary properties
    added.  These properties are stored using a 'dynamic hidden class' system
    which builds classes on-the-fly.
*/

class oexpand : public obase
{
public:

    static oexpand* alloc();
    static oexpand* alloc( oexpand* prototype );
    
    oexpand*    prototype() const;
    ovalue      getkey( osymbol key ) const;
    void        setkey( osymbol key, ovalue value );
    void        delkey( osymbol key );

    oclass*     empty();
    

protected:

    friend class obase;
    static ometatype metatype;
    static void mark_expand( oworklist* work, obase* object, ocolour colour );

    oexpand( ometatype* metatype, oclass* klass );


private:

    void        expandkey( osymbol key, ovalue value );

    owb< oclass* >              klass;
    owb< otuple< ovalue >* >    props;
    
};







/*
    An oclass describes the layout of an expand's dynamic properties.  The
    class for empty objects is per-context and is the root of the class tree.

    An object with the same properties but a different prototype object will
    have a different class.  This allows easier caching of property lookups.
    The root of a prototype's class tree is a hidden property at index 0.  The
    first time an object becomes a prototype its own class changes in order
    to add this hidden property.
*/


class oclass : public obase
{
public:

    static oclass* alloc();


protected:

    friend class obase;
    static ometatype metatype;
    static void mark_class( oworklist* work, obase* object, ocolour colour );


private:

    friend class oexpand;

    owb< oexpand* >                 prototype;
    okeytable< osymbol, size_t >    lookup;
    owb< oclass* >                  parent;
    owb< osymbol >                  parent_key;
    okeytable< osymbol, oclass* >   children;
    bool                            is_prototype;
    
};





/*

*/

inline oexpand* oexpand::alloc()
{
    void* p = malloc( sizeof( oexpand ) );
    return new ( p ) oexpand( &metatype, ocontext::context->empty );
}

inline oexpand* oexpand::alloc( oexpand* prototype )
{
    void* p = malloc( sizeof( oexpand ) );
    return new ( p ) oexpand( &metatype, prototype->empty() );
}


inline oexpand::oexpand( ometatype* metatype, oclass* klass )
    :   obase( metatype )
    ,   klass( klass )
{
}

inline oexpand* oexpand::prototype() const
{
    return klass->prototype;
}

inline ovalue oexpand::getkey( osymbol key ) const
{
    ovalue v;

    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
        v = props->at( lookup->value );
    }
    
    if ( v.is_undefined() && klass->prototype )
    {
        v = klass->prototype->getkey( key );
    }

    return v;
}

inline void oexpand::setkey( osymbol key, ovalue value )
{
    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
        props->at( lookup->value ) = value;
    }
    else
    {
        expandkey( key, value );
    }
}

inline void oexpand::delkey( osymbol key )
{
    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
        props->at( lookup->value ) = ovalue::undefined;
    }
}






#endif


#if OLD

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

    static oexpand* alloc( oexpand* prototype );
    
    oexpand*    prototype() const;
    ovalue      getkey( osymbol key ) const;
    void        setkey( osymbol key, ovalue value );
    void        delkey( osymbol key );


protected:

    oexpand();
    ~oexpand();


private:

    friend class oimpl::omodel;

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
protected:

    oexpandclass();
    ~oexpandclass();


private:

    friend class oimpl::omodel;

    friend class ogcbase;
    static oimpl::ogctype gctype;
    static void mark( ogcbase*, oimpl::oworklist*, oimpl::ocolour, oimpl::ocolour );
    static void free( ogcbase* );

    friend class oexpand;
    friend class omodel;

    oexpand*                            prototype;
    uint32_t                            capacity;
    uint32_t                            size;
    hashtable< osymbol, size_t >        lookup;
    uint32_t                            derivedindex;
    
    uint32_t                            refcount;
    oexpandclass*                       parent;
    osymbol                             parent_key;
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

    return ovalue::undefined;
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



