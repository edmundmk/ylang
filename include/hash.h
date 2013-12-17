//
//  hash.h
//
//  Created by Edmund Kapusniak on 28/07/2013.
//  Copyright (c) 2013 Edmund Kapusniak. All rights reserved.
//


#ifndef HASH_H
#define HASH_H


#include <stdlib.h>
#include <stdint.h>


typedef uint32_t hash_t;


hash_t hash( const char* string );
hash_t hash( const void* data, size_t size );



#endif /* HASH_H */

