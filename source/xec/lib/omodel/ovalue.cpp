//
//  ovalue.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ovalue.h"



ovalue ovalue::undefined( ovalue::VALUE_UNDEFINED );


obase* ovalue::get() const
{
    return (obase*)( x & POINTER_MASK );
}





#ifdef OVALUE_BOXING


ometatype oboxed::metatype = { &mark_boxed };

void oboxed::mark_boxed( oworklist* work, obase* object, ocolour colour )
{
    assert( ! "oboxed added to work queue" );
}


#endif