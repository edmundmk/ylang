//
//  yl_iterator.h
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_ITERATOR_H
#define YL_ITERATOR_H


#include <stddef.h>
#include "yl_interp.h"


class yl_value;
class yl_gcheap;
class yl_object;
class yl_array;
class yl_bucketlist;
class yl_cothread;
class yl_slot;


/*
    An entry on the iterator stack.
*/


enum yl_iterkind
{
    YLITER_CLOSED,
    YLITER_KEYS,        // over the keys of an object
    YLITER_ARRAY,       // over the elements of an array
    YLITER_TABLE,       // over the key, value of a table
    YLITER_GENERATOR,   // over values produced by a generator cothread
};


struct yl_iternext
{
    yl_value*   values;
    unsigned    vcount;
};


class yl_iterator
{
public:

    yl_iterator();

    void mark( yl_gcheap* heap ) const;
    void eager_mark( yl_gcheap* heap ) const;

    void open_vals( yl_value value );
    void open_keys( yl_value value );
    void close();

    bool has_values();
    void next1( yl_value* r );
    void next2( yl_value* r, yl_value* b );
    yl_iternext next( yl_value vspace[ 2 ] );


private:

    void next_bucket();
    void generate();
 

    yl_iterkind _kind;
    union
    {
        yl_object*      _object;
        yl_array*       _array;
        yl_bucketlist*  _buckets;
        yl_cothread*    _generator;
    };
    union
    {
        size_t          _index;
        yl_slot*        _slot;
        yl_genvalues    _genvalues;
    };
    
};




#endif


