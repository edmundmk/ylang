//
//  anyalias.h
//
//  Created by Edmund Kapusniak on 17/02/2015.
//  Copyright (c) 2015 birotanker. All rights reserved.
//


#ifndef ANYALIAS_H
#define ANYALIAS_H


#include <stdint.h>


typedef int8_t   __attribute__(( __may_alias__ )) anyint8_t;
typedef int16_t  __attribute__(( __may_alias__ )) anyint16_t;
typedef int32_t  __attribute__(( __may_alias__ )) anyint32_t;
typedef int64_t  __attribute__(( __may_alias__ )) anyint64_t;

typedef uint8_t  __attribute__(( __may_alias__ )) anyuint8_t;
typedef uint16_t __attribute__(( __may_alias__ )) anyuint16_t;
typedef uint32_t __attribute__(( __may_alias__ )) anyuint32_t;
typedef uint64_t __attribute__(( __may_alias__ )) anyuint64_t;

typedef void*    __attribute__(( __may_alias__ )) anypointer_t;



#endif
