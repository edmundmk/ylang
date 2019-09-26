//
//  yvm_gcheap.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YVM_GCHEAP_H
#define YVM_GCHEAP_H

/*
    
*/





/*
    Values on the GC heap must be atomic, and require a write barrier.
*/

struct yvm_heap_value
{
};

template < typename T >
struct yvm_heap_ref
{

};


/*
    Weak references on the heap.  Need special handling.
*/

template < typename T >
struct yvm_weak_ref
{
};


/*
    Smart pointer to keep objects alive from C++.
*/

struct yvm_root_value
{
};

template < typename T >
struct yvm_root_ref
{

};

#endif

