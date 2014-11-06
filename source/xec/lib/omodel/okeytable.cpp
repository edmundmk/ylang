//
//  okeytable.cpp
//
//  Created by Edmund Kapusniak on 01/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "okeytable.h"
#include "ostring.h"
#include "oexpand.h"



#if OSLOTLIST
template class okeytable< osymbol, oslotindex >;
#else
template class okeytable< osymbol, size_t >;
#endif
template class okeytable< osymbol, oclass* >;


