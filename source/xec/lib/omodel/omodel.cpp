//
//  omodel.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "omodel.h"
#include "ovalue.h"
#include "oexpand.h"



oexpand* expand = oexpand::alloc();

void blah()
{
    expand->getkey( osymbol( "" ) );
}