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
#include "yl_heapobj.h"
#include "yl_value.h"
#include "yl_string.h"


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


class yl_object : public yl_heapobj
{
public:




private:

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


class yl_slot : public yl_heapobj
{
public:


private:

    struct shortcut_hash
    {
        yl_object* proto;
        std::unordered_map< symkey, size_t > lookup;
    };


    yl_heapref< yl_heapobj >    _parent;    // parent slot, or prototype
    yl_heapref< yl_string >     _symbol;    // symbol string
    size_t                      _index;     // index into slot value array
    
    std::unique_ptr< shortcut_hash > _hash;
    
    /* weak */ yl_slot*         _head;      // first child
    /* weak */ yl_slot*         _next;      // next sibling
    
};




#endif
