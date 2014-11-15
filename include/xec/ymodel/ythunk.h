//
//  ythunk.h
//
//  Created by Edmund Kapusniak on 10/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YTHUNK_H
#define YTHUNK_H


#include "yvalue.h"


class yframe
{
public:

    size_t      argcount();
    yvalue      argument( size_t index );
    void        result( yvalue result );


private:

    friend void yexec( size_t, unsigned, unsigned );

    yframe( yvalue* s, yvalue* limit, unsigned acount );

    void        grow_stack();

    yvalue*     s;
    yvalue*     limit;
    unsigned    acount;


};


typedef void (*ythunk)( yframe );




/*

*/

inline yframe::yframe( yvalue* s, yvalue* limit, unsigned acount )
    :   s( s )
    ,   limit( limit )
    ,   acount( acount )
{
}

inline size_t yframe::argcount()
{
    return acount;
}

inline yvalue yframe::argument( size_t index )
{
    if ( index < acount )
        return s[ 1 + index ];
    else
        return nullptr;
}

inline void yframe::result( yvalue result )
{
    if ( s >= limit )
        grow_stack();
    *s++ = result;
    acount = 0;
}




#endif
