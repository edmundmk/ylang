//
//  yl_cothread.h
//
//  Created by Edmund Kapusniak on 12/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_COTHREAD_H
#define YL_COTHREAD_H


#include <vector>
#include "yl_heapobj.h"
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
    yl_funcobj*     funcobj;        // function
    unsigned        ip;             // instruction pointer
    unsigned        sp;             // stack pointer
    unsigned        fp;             // frame pointer (after vararg adjustment)
    unsigned        locup_base;     // local upval pointer
    unsigned        iters_base;     // iterators pointer
    unsigned        rcount;         // requested number of result values
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

    void open_vals( const yl_tagval& value );
    void open_keys( const yl_tagval& value );
    void close();

    bool has_values();
    void next1( yl_tagval* r );
    void next2( yl_tagval* r, yl_tagval* b );
    void next( yl_tagval* r, size_t b );


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

    void            push_frame( const yl_stackframe& frame );
    yl_stackframe   pop_frame() const;

    yl_tagval*      stack( size_t base, size_t count );
    yl_upval**      locup( size_t base, size_t count );
    yl_iterator*    iters( size_t base, size_t count );
    
    void            set_mark( unsigned mark );
    unsigned        get_mark() const;
    
    unsigned        get_locup_base() const;
    unsigned        get_iters_base() const;


private:

    std::vector< yl_stackframe >    _frames;
    std::vector< yl_tagval >        _stack;
    std::vector< yl_upval* >        _locup;
    std::vector< yl_iterator >      _iters;
    unsigned                        _mark;

};



/*

*/


inline void yl_cothread::set_mark( unsigned mark )
{
    _mark = mark;
}

inline unsigned yl_cothread::get_mark() const
{
    return _mark;
}





#endif


