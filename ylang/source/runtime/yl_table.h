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
    static yl_table* alloc( yl_object* prototype, size_t capacity );

    yl_value    get( const yl_value& key ) const;
    void        set( const yl_value& key, const yl_value& value );

};


#endif
