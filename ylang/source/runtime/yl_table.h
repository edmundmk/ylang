//
//  yl_table.h
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_TABLE_H
#define YL_TABLE_H


#include "yl_object.h"


/*
    A fixed-size array of hash buckets.
*/


class yl_bucketlist : public yl_gcobject
{
public:

    static yl_gctype gctype;

    struct bucket
    {
        bucket();
    
        yl_valref   key;
        yl_valref   value;
        bucket*     next;
    };


    static yl_stackref< yl_bucketlist > alloc( size_t size );
    ~yl_bucketlist();
    
    size_t          size() const;
    const bucket&   at( size_t index ) const;
    bucket&         at( size_t index );
    
    
private:

    explicit yl_bucketlist( size_t size );
    
    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );
    
    size_t          _size;
    bucket          _elements[ 0 ];
    

};


static yl_bucketlist::bucket* const YL_EMPTY_BUCKET =
                (yl_bucketlist::bucket*)-1;





/*
    A hash table.
*/


class yl_table : public yl_object
{
public:

    static yl_gctype gctype;

    typedef yl_bucketlist::bucket bucket;

    static yl_stackref< yl_object > make_prototype();

    static yl_stackref< yl_table > alloc( size_t capacity );
    static yl_stackref< yl_table > alloc( yl_object* prototype, size_t capacity );

    size_t      length() const;

    yl_value    get_index( yl_value key ) const;
    void        set_index( yl_value key, yl_value value );


protected:

    yl_table( yl_object* prototype, size_t capacity );


private:

    friend class yl_iterator;
    
    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

    bucket* lookup( yl_value key ) const;
    bucket* main_position( yl_value key ) const;
    bucket* free_position( bucket* near );
    void    rehash( size_t capacity );

    static yl_table* thunk_this( yl_callframe& xf );
    static void thunk_length( yl_callframe& xf );

    size_t                      _occupancy;
    yl_heapref< yl_bucketlist > _buckets;


};



/*

*/


inline size_t yl_table::length() const
{
    return _occupancy;
}




#endif
