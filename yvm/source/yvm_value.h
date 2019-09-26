//
//  yvm_value.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YVM_VALUE_H
#define YVM_VALUE_H


#include <stdint.h>


/*
    A value is 'nun-boxed'.






*/

struct yvm_value { uint64_t value; };






#endif

