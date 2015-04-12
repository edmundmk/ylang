//
//  yl_cothread.h
//
//  Created by Edmund Kapusniak on 12/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_COTHREAD_H
#define YL_COTHREAD_H


#include <vector>
#include "yl_heap.h"
#include "yl_function.h"


class yl_object;
class yl_array;
class yl_table;
class yl_cothread;



/*
    A stack frame.
*/


struct yl_stackframe
{
    yl_function*    function;
    unsigned        base;
    uint8_t         argcount;
    uint8_t         outcount;
    unsigned        stack_base;
    unsigned        upval_base;
    unsigned        iters_base;
    unsigned        ip;
};



/*
    A slot on the execution stack.  Curently a type-tagged 'fat value'.
*/


struct yl_stackslot
{
    yl_objkind  kind;
    union
    {
        double      number;
        yl_heapobj* heapobj;
    };
};



/*
    An entry on the iterator stack.
*/


enum yl_iterkind
{
    YLITER_KEYS,        // over the keys of an object
    YLITER_ARRAY,       // over the elements of an array
    YLITER_TABLE,       // over the key, value of a table
    YLITER_GENERATOR,   // over values produced by a generator cothread
};


class yl_iterator
{
public:

    explicit yl_iterator( yl_object* object );
    explicit yl_iterator( yl_array* array );
    explicit yl_iterator( yl_table* table );
    explicit yl_iterator( yl_cothread* generator );

    bool valid();
    


private:

    yl_iterkind _kind;
    union
    {
        yl_object*      _object;
        yl_array*       _array;
        yl_table*       _table;
        yl_cothread*    _generator;
    };
    
    
};



/*
    A cothread, with stack frames and the actual stack.
*/


class yl_cothread : public yl_heapobj
{
public:

    yl_cothread* alloc();


private:

    std::vector< yl_stackframe >    _frames;
    std::vector< yl_stackslot >     _stack;
    std::vector< yl_upval* >        _upval;
    std::vector< yl_iterator >      _iters;
    unsigned                        _mark;

};



#endif


