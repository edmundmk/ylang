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


#if OVALUE32
#define OEXPANDSLOTS 1
#endif


#if OEXPANDSLOTS
#include "oslotlist.h"
#endif


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
#if OEXPANDSLOTS
    void        dualkey( osymbol key, oslotindex index, ovalue value );
#endif

    owb< oclass* >              klass;
#if OEXPANDSLOTS
    owb< oslotlist* >           slots;
#else
    owb< otuple< ovalue >* >    slots;
#endif
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

    owb< oexpand* >                     prototype;
#if OEXPANDSLOTS
    okeytable< osymbol, oslotindex >    lookup;
#else
    okeytable< osymbol, size_t >        lookup;
#endif
    owb< oclass* >                      parent;
    owb< osymbol >                      parent_key;
#if OEXPANDSLOTS
    okeytable< osymbol, oclass* >       children_addref;
    okeytable< osymbol, oclass* >       children_addnum;
#else
    okeytable< osymbol, oclass* >       children;
#endif
    bool                                is_prototype;
    
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
#if OEXPANDSLOTS
        v = slots->load( lookup->value.slot );
#else
        v = props->at( lookup->value );
#endif
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
#if OEXPANDSLOTS
        oslotindex index = lookup->value;
        if ( value.is_number() == index.dual )
            slots->store( index.slot, value );
        else
            dualkey( key, index, value );
#else
        props->at( lookup->value ) = value;
#endif
    }
    else
    {
        expandkey( key, value );
    }
}






#endif
