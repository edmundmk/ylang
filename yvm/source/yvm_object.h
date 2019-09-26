//
//  yvm_object.h
//
//  Created by Edmund Kapusniak on 26/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YVM_OBJECT_H
#define YVM_OBJECT_H

#include "yvm_gcheap.h"

struct yvm_object;
struct yvm_slot_class;
struct yvm_value_list;

/*
*/

struct yvm_object
{
    yvm_heap_ref< yvm_slot_class >  slot_class;
    yvm_heap_ref< yvm_value_list >  slots;
};

struct yvm_slot_class
{
    uint32_t inline_key;
    uint32_t slot_index;
    yvm_heap_ref< yvm_string >      key;
    yvm_heap_ref< yvm_slot_class >  parent;
    yvm_heap_ref< yvm_object >      prototype;
    yvm_weak_ref< yvm_slot_class >  head;
    yvm_weak_ref< yvm_slot_class >  next;
};

struct yvm_value_list
{
    yvm_heap_value values[];
};

#endif

