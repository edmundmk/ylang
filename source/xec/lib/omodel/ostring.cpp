//
//  ostring.cpp
//
//  Created by Edmund Kapusniak on 27/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ostring.h"
#include <algorithm>



ostring* ostring::strcat( ostring* a, ostring* b )
{
    ostring* result = alloc( a->size() + b->size() );
    memcpy( result->buffer(), a->data(), a->size() );
    memcpy( result->buffer() + a->size(), b->data(), b->size() );
    assert( result->data()[ a->size() + b->size() ] == '\0' );
    return result;
}


int ostring::strcmp( ostring* a, ostring* b )
{
    if ( a == b )
        return 0;
    size_t size = std::min( a->size(), b->size() );
    int result = memcmp( a->data(), b->data(), size );
    if ( result != 0 )
        return result;
    else if ( a->size() < b->size() )
        return -1;
    else if ( a->size() > b->size() )
        return 1;
    else
        return 0;
}
