//
//  ovalue.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ovalue.h"



#if OVALUE64

ovalue ovalue::undefined( VALUE_UNDEFINED );

obase* ovalue::get() const
{
    if ( x >= MIN_REFERENCE && x <= MAX_REFERENCE )
    {
        return (obase*)( x & POINTER_MASK );
    }
    else
    {
        return nullptr;
    }
}


#endif



#if OVALUE32

ometatype oboxed::metatype = { nullptr };

ovalue ovalue::undefined( VALUE_UNDEFINED );

obase* ovalue::get() const
{
    if ( ( x & TAG_MASK ) < TAG_BOXED )
    {
        return (obase*)( x & POINTER_MASK );
    }
    else
    {
        return nullptr;
    }
}



#endif





