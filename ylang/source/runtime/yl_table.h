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

    yl_tagval   get_index( const yl_tagval& index );
    void        set_index( const yl_tagval& index, const yl_tagval& value );


};


#endif
