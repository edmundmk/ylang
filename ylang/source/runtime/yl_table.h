//
//  yl_table.h
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_TABLE_H
#define YL_TABLE_H


#include "yl_object.h"




class yl_bucketlist : public yl_heapobj
{
public:


private:


    

};



class yl_table : public yl_object
{
public:

    static yl_table* alloc( size_t capacity );
    static yl_table* alloc( yl_object* prototype, size_t capacity );

    yl_value    get_index( const yl_value& key ) const;
    void        set_index( const yl_value& key, const yl_value& value );


protected:

    yl_table( yl_object* prototype, size_t capacity );


private:

    struct bucket
    {
        bucket();
    
        yl_valref   key;
        yl_valref   value;
        bucket*     next;
    };
    
    
    class bucketlist
    {
    public:
    
        static bucketlist* alloc( size_t size );
        ~bucketlist();
        
        size_t          size() const;
        const bucket&   at( size_t index ) const;
        bucket&         at( size_t index );
        
        
    private:
    
        explicit bucketlist( size_t size );
        
        size_t          _size;
        bucket          _elements[ 0 ];
    
    };


    size_t                  _occupancy;
    yl_objref< bucketlist > _buckets;


};


#endif
