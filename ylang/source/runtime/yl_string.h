//
//  yl_string.h
//
//  Created by Edmund Kapusniak on 03/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_STRING_H
#define YL_STRING_H


/*
    Garbage-collected strings.
    
    Strings have an optional hash, which is calculated the first time it is
    used in a table or as an property key.
 
    Strings can be symbols.  The system ensures that there is only one unique
    instance of each symbol with the same string.  This allows faster property
    lookups as testing for equality is a simple pointer test.
 
*/

class yl_string : public yl_heapobj
{
public:




private:

    bool        has_hash;
    bool        is_symbol;
    hash32_t    hash;
    uint32_t    size;
    char        s[];

};


#endif



