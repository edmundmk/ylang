//
//  yvm_gcheap.cpp
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "yvm_gcheap.h"


/*
    This heap implementation is based on Doug Lea's dlmalloc, with changes.
    First, the maximum size of the heap is 4GiB even on 64-bit platforms.
    Second, the alignment of chunks is slightly different, as allocated objects
    have a 32-bit GC word before the object data.

    When allocated, a chunk has the following layout:

                u32 size of previous free chunk (if P==0)
        allocated chunk
                u32 size of this chunk/C=1/P
                u32 GC bookkeeping
        user memory
                ...
        next chunk
                u32 size of next chunk/C/P=1


    When free, a chunk has the following layout:

                u32 size of previous free chunk (if P==0)
        free chunk
                u32 size of this chunk/C=0/P
                u32 next chunk in list
                u32 prev chunk in list
        tree chunks only:
                u32 left child
                u32 right child
                u32 parent
        ...
                u32 size of this chunk
        next chunk
                u32 size of next chunk/C/P=0


    Chunk and object addresses are always a multiple of eight bytes.  It's
    important that the entire heap can be scanned by the garbage collector,
    visiting each allocated object.

    When the heap runs out of memory it attempts to grow.  If this fails, the
    allocation fails.
*/
