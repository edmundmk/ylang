//
//  yexpand.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YEXPAND_H
#define YEXPAND_H


#include "yobject.h"
#include "ytuple.h"
#include "ymap.h"
#include "ystring.h"
#include "yslots.h"


class yexpand;
class yclass;
class yvalue;




/*
    yexpand is the base class for objects that can have arbitrary properties
    added.  These properties are stored using a 'dynamic hidden class' system
    which builds classes on-the-fly.
*/

class yexpand : public yobject
{
public:

    static yexpand* alloc();
    static yexpand* alloc( yexpand* prototype );
    
    yexpand*    prototype() const;
    yvalue      getkey( ysymbol key ) const;
    void        setkey( ysymbol key, yvalue value );
    void        delkey( ysymbol key );

    yclass*     empty_class();
    

protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_expand( yobject* object, yworklist* work, ycolour colour );

    yexpand( ymetatype* metatype, yclass* klass );


private:

#if YSLOTS
    void        dualkey( ysymbol key, yslotindex index, yvalue value );
    void        expanddual( ysymbol key, yslotindex index, yvalue value );
    void        storedual( yslotindex index, size_t newslot, yvalue value );
#else
    void        expandkey( ysymbol key, yvalue value );
#endif

    ywb< yclass* >              klass;
#if YSLOTS
    ywb< yslots* >              slots;
#else
    ywb< ytuple< yvalue >* >    slots;
#endif
};







/*
    An yclass describes the layout of an expand's dynamic properties.  The
    class for empty objects is per-context and is the root of the class tree.

    An object with the same properties but a different prototype object will
    have a different class.  This allows easier caching of property lookups.
    The root of a prototype's class tree is a hidden property at index 0.  The
    first time an object becomes a prototype its own class changes in order
    to add this hidden property.
*/


class yclass : public yobject
{
public:

    static yclass* alloc();


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_class( yobject* object, yworklist* work, ycolour colour );

    explicit yclass( ymetatype* metatype );


private:

    friend class ymodel;
    friend class yexpand;

    ywb< yexpand* >                 prototype;
#if YSLOTS
    ymap< ysymbol, yslotindex >     lookup;
    ymap< ysymbol, yclass* >        expandref;
    ymap< ysymbol, yclass* >        expandnum;
    size_t                          refslots;
    size_t                          numslots;
#else
    ymap< ysymbol, size_t >         lookup;
    ymap< ysymbol, yclass* >        expand;
#endif
    bool                            is_prototype;
    
};





/*

*/

#include "yvalue.h"



inline yexpand* yexpand::prototype() const
{
    return klass->prototype;
}

inline yvalue yexpand::getkey( ysymbol key ) const
{
    yclass* klass = this->klass;
    yvalue v = yvalue::undefined;

    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
#if YSLOTS
        v = slots->get( lookup->value.slot );
#else
        v = slots->get( lookup->value );
#endif
    }
    
    if ( v.is_undefined() && klass->prototype )
    {
        v = klass->prototype->getkey( key );
    }

    return v;
}

inline void yexpand::setkey( ysymbol key, yvalue value )
{
#if YSLOTS

    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
        yslotindex index = lookup->value;
        if ( value.is_number() == index.dual )
            slots->set( index.slot, value );
        else
            dualkey( key, index, value );
    }
    else
    {
        expanddual( key, yslotindex(), value );
    }

#else

    auto lookup = klass->lookup.lookup( key );
    if ( lookup )
    {
        slots->set( lookup->value, value );
    }
    else
    {
        expandkey( key, value );
    }

#endif
}





#endif
