//
//  yl_object.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_OBJECT_H
#define YL_OBJECT_H


#include <unordered_map>
#include <symkey.h>
#include "yl_context.h"
#include "yl_value.h"
#include "yl_string.h"
#include "yl_code.h"


class yl_object;
class yl_slot;



/*
    A yl_object is the 'object' type supporting lookup of string keys and
    delegation to a prototype.
    
    As in modern JavaScript engines, an object is not itself a hashtable but
    has a 'hidden class' or 'shape' describing the set of keys that have been
    added.  This reduces memory consumption and allows more possibilities for
    optimizing property accesses.
    
    When an object is used as a prototype, it creates (or reuses) an empty
    object slot used as the head of the slot chain for objects that use
    this one as their prototype.  Most objects are 'leaf' objects, so the
    (weak) association between the object and its prototype slot is stored
    out-of-line.
 
*/


class yl_object : public yl_gcobject
{
public:

    static yl_gctype gctype;

    yl_object();
    explicit yl_object( yl_object* prototype );

    void        seal();

    yl_object*  superof() const;

    yl_value    get_key( const yl_symbol& key, yl_ilcache* ilc ) const;
    bool        set_key( const yl_symbol& key, yl_value value, yl_ilcache* ilc );

    yl_value    get_key( const yl_symbol& key ) const;
    bool        set_key( const yl_symbol& key, yl_value value );
    bool        del_key( const yl_symbol& key );

    bool        set_key( const char* key, yl_value value );


protected:

    friend class yl_iterator;
    
    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );


private:

    enum
    {
        SEALED = YL_GCFLAG_USER1,
    };
    
    yl_value    get_key_impl( const yl_symbol& key, yl_ilcache* ilc ) const;
    bool        set_key_impl( const yl_symbol& key, yl_value value, yl_ilcache* ilc );
    

    yl_heapref< yl_slot >       _klass;     // start of slot chain
    yl_heapref< yl_valarray >   _slots;     // array of slot values
    
};





/*
    An object's class is described by a chain of slots.  Each slot names a
    single property of an object and gives its index in the object.
    
    At the end of each slot chain is a slot representing an empty object
    with a particular prototype.  Slots are kept alive by the objects that
    use them.  Each slot in the chain keeps both its symbol and its parent
    slot alive.

    Each slot is a node in the class tree.  Lookup of properties can be
    accomplished by walking the tree.  For objects with many keys, property
    locations may be cached in a hash table.  This hash table can use unsafe
    pointers, as the class's entire slot chain is both immutable and kept
    alive already.
 
*/


class yl_slot : public yl_gcobject
{
public:

    static yl_gctype gctype;

    explicit yl_slot( yl_object* prototype );
    yl_slot( yl_slot* parent, yl_string* symbol );


private:

    friend class yl_object;
    friend class yl_iterator;

    static const size_t EMPTY_KLASS = (size_t)-1;

    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

    uintptr_t                   _klassid;
    yl_heapref< yl_gcobject >   _parent;    // parent slot, or prototype
    yl_heapref< yl_string >     _symbol;    // symbol string
    size_t                      _index;     // index into slot value array
    
    /* weak */ yl_slot*         _head;      // first child
    /* weak */ yl_slot*         _next;      // next sibling
    
};



/*

*/


inline yl_value yl_object::get_key( const yl_symbol& key, yl_ilcache* ilc ) const
{
    if ( ilc->klassid() == _klass.get()->_klassid )
    {
        if ( ilc->is_direct() )
            return ilc->heapval()->get();
        else
            return _slots.get()->at( ilc->index() ).get();
    }
    return get_key_impl( key, ilc );
}

inline bool yl_object::set_key( const yl_symbol& key, yl_value value, yl_ilcache* ilc )
{
    if ( ilc->klassid() == _klass.get()->_klassid )
    {
        if ( ! ilc->is_direct() )
        {
            _slots.get()->at( ilc->index() ).set( value );
            return true;
        }
    }
    return set_key_impl( key, value, ilc );
}


inline yl_value yl_object::get_key( const yl_symbol& key ) const
{
    return get_key_impl( key, nullptr );
}

inline bool yl_object::set_key( const yl_symbol& key, yl_value value )
{
    return set_key_impl( key, value, nullptr );
}





#endif
