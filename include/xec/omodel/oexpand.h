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

class oexpand;
class oclass;
class ovalue;


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


protected:

    friend class obase;
    static ometatype metatype;
    static void mark_expand( oworklist* work, obase* object, ocolour colour );

    oexpand( ometatype* metatype, oclass* klass );


private:

    owb< oclass* >                  klass;
    owb< otuple< ovalue >* >        props;

}; 





/*
    Hidden classes are typed.
    
    We have a problem with storing ovalues as Nan-boxed values in objects.
    Concurrent garbage collection forces pointer updates to be atomic.  On
    32-bit systems, atomic 64-bit accesses are slow.  So we have to know if
    a value is a pointer or not by looking at only 32 bits of it.  This makes
    NaN-boxing impossible from the point of view of the garbage collector -
    it cannot decide between a real pointer and the low bits of a double.
    Using pointer tag bits does not help, as we cannot fit a double in 63 bits.
 
    Some solutions considered:
        -   Box numbers (or possibly, numbers which aren't 31-bit ints).
        -   Make write-barriered ovalues 128-bit.
 
    These are both wasteful.
    
    The better solution is to use the hidden class system to store not only
    the locations of properties but their type.  The hidden class system knows
    about only 3 types:
        -   References.
        -   Numbers.
        -   A dualval, which can store either a reference or a number.
        
    References are pointer-sized and we get efficient concurrency with the
    garbage collector back.  Numbers are double-sized, and can store any
    number.  Either a reference or a number can store the 'special' values
    null, undefined, true, or false.
    
    The type of a property is inferred by the first value that is stored to
    it.  If a number is stored into a reference property, or vice-versa, then
    the property becomes a dualval, the class is transitioned and the layout
    updated.  If the first store is a special, we assume a pointer.
    
    References are pointers with the two low bits as a tag:
    
            00  expand
            01  string
            10  object
            11  special - the high bits identify which one.

    There is a special bit pattern reserved to represent the pointer part
    of a dualval which currently contains a number.
 
    Numbers are doubles, with the special values encoded in as NaNs.
 
    So an oexpand's olayout is a special garbage collected object which stores:
        -   The total size of the layout.
        -   The number of references currently allocated in it.
    
    Pointers are always allocated at the start of the layout, and numbers from
    the end backwards.  The number of references in a layout can only increase.
    Dualvals always require allocation of both a reference and a number.  If
    the layout runs out of space, a new layout is allocated and the old one is
    left to the garbage collector.  Empty space is initialized to a bit pattern
    meaning 'undefined' in both the reference and number representations.
    
    These properties ensure that no matter which version of an object, or what
    layout updates that object has had, even with race conditions the garbage
    collector will always find a valid layout at least containing the number
    of references it had when the collection cycle began.
*/





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


private:

    owb< oexpand* >                 prototype;
    okeytable< osymbol, size_t >    lookup;
    owb< oclass* >                  parent;
    owb< osymbol >                  parent_key;
    okeytable< osymbol, oclass* >   children;
    bool                            is_prototype;
    
};





/*

*/


oexpand::oexpand( ometatype* metatype, oclass* klass )
    :   obase( metatype )
{
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



