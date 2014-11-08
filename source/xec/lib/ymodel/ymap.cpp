//
//  ymap.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "ymap.h"
#include "ystring.h"
#include "yexpand.h"


#if YSLOTS
template class ymap< ysymbol, yslotindex >;
#else
template class ymap< ysymbol, size_t >;
#endif

template class ymap< ysymbol, yclass* >;


