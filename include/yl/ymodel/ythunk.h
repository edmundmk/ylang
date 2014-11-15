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

    size_t      count() const;
    yvalue      operator [] ( size_t index ) const;

    void        result( yvalue result );


private:

    friend void yexec( size_t, unsigned, unsigned );

    yframe( yvalue* s, yvalue* limit, size_t argcount );

    void        grow_stack();

    yvalue*     s;
    yvalue*     limit;
    size_t      argcount;


};


typedef void (*ythunk)( yframe& );




/*

*/

inline yframe::yframe( yvalue* s, yvalue* limit, size_t argcount )
    :   s( s )
    ,   limit( limit )
    ,   argcount( argcount )
{
}

inline size_t yframe::count() const
{
    assert( argcount != (size_t)-1 );
    return argcount;
}

inline yvalue yframe::operator [] ( size_t index ) const
{
    assert( argcount != (size_t)-1 );
    if ( index < argcount )
        return s[ 1 + index ];
    else
        return nullptr;
}

inline void yframe::result( yvalue result )
{
    if ( s >= limit )
        grow_stack();
    *s++ = result;
    argcount = -1;
}




#endif
