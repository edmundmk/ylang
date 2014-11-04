//
//  ovalue.h
//
//  Created by Edmund Kapusniak on 25/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//



#if defined( __x86_64__ ) || defined( __arm64__ )
#include "ovalue64.h"
#else
#include "ovalue32.h"
#endif


