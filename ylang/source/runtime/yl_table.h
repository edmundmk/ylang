//
//  yl_table.h
//
//  Created by Edmund Kapusniak on 21/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_TABLE_H
#define YL_TABLE_H


#include "yl_object.h"


class yl_table : public yl_object
{
public:

    static yl_table* alloc( size_t capacity );
    static yl_table* alloc( yl_object* prototype );

    yl_tagval   get_index( const yl_tagval& key );
    void        set_index( const yl_tagval& key, const yl_tagval& value );

};


#endif
