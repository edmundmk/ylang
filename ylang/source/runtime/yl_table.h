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

    static const size_t END = (size_t)-1;


    yl_tagval   get_index( const yl_tagval& key );
    void        set_index( const yl_tagval& key, const yl_tagval& value );

    size_t      begin();
    size_t      next( size_t index, yl_tagval* key, yl_tagval* value );


};


#endif
