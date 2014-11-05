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

    static ometatype metatype;

    static oexpand* alloc();
    static oexpand* alloc( oexpand* prototype );
    
    oexpand*    prototype() const;
    ovalue      getkey( osymbol key ) const;
    void        setkey( osymbol key, ovalue value );
    void        delkey( osymbol key );

    oclass*     empty();
    

protected:

    static void mark_expand( oworklist* work, obase* object, ocolour colour );

    oexpand( ometatype* metatype, oclass* klass );


private:

#if OEXPANDSLOTS
    void        dualkey( osymbol key, oslotindex index, ovalue value );
    void        expandkey( osymbol key, ovalue value );
    void        expanddual( osymbol key, oslotindex index, ovalue value );
    void        storedual( oslotindex index, size_t newslot, ovalue value );
#else
    void        expandkey( osymbol key, ovalue value );
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

    static ometatype metatype;

    static oclass* alloc();


protected:

    static void mark_class( oworklist* work, obase* object, ocolour colour );

    oclass( ometatype* metatype );


private:

    friend class oexpand;

    owb< oexpand* >                     prototype;
#if OEXPANDSLOTS
    okeytable< osymbol, oslotindex >    lookup;
    okeytable< osymbol, oclass* >       expandref;
    okeytable< osymbol, oclass* >       expandnum;
    size_t                              numbercount;
#else
    okeytable< osymbol, size_t >        lookup;
    okeytable< osymbol, oclass* >       expand;
#endif
    bool                                is_prototype;
    
};





/*

*/

#include "ovalue.h"



inline oexpand* oexpand::prototype() const
{
    return klass->prototype;
}

inline ovalue oexpand::getkey( osymbol key ) const
{
    oclass* klass = this->klass;
    ovalue v;

    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
#if OEXPANDSLOTS
        v = slots->load( lookup->value.slot );
#else
        v = slots->at( lookup->value );
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
        slots->at( lookup->value ) = value;
#endif
    }
    else
    {
        expandkey( key, value );
    }
}







#endif
