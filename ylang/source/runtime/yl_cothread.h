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
    An upval.
*/

class yl_upval : public yl_heapobj
{
public:

    static yl_upval* alloc( unsigned index );

    void        close( yl_cothread* cothread );

    yl_value    get_value( yl_cothread* cothread ) const;
    void        set_value( yl_cothread* cothread, const yl_value& value );

private:

    explicit yl_upval( unsigned index );

    bool        _open;
    unsigned    _index;
    yl_valref   _value;

};





/*
    An entry on the iterator stack.
*/


enum yl_iterkind
{
    YLITER_CLOSED,
    YLITER_KEYS,        // over the keys of an object
    YLITER_ARRAY,       // over the elements of an array
    YLITER_TABLE,       // over the key, value of a table
    YLITER_GENERATOR,   // over values produced by a generator cothread
};


class yl_iterator
{
public:

    yl_iterator();

    void open_vals( const yl_value& value );
    void open_keys( const yl_value& value );
    void close();

    bool has_values();
    void next1( yl_value* r );
    void next2( yl_value* r, yl_value* b );
    void next( yl_cothread* t, unsigned r, unsigned b );


private:

    yl_iterkind _kind;
    union
    {
        yl_object*      _object;
        yl_array*       _array;
        yl_table*       _table;
        yl_cothread*    _generator;
    };
    size_t _index;
    
};



/*
    A cothread, with stack frames and the actual stack.
*/


class yl_cothread : public yl_heapobj
{
public:

    static yl_cothread* alloc();

    void            push_frame( const yl_stackframe& frame );
    yl_stackframe   pop_frame();

    yl_value*       stack( size_t base, size_t count );
    yl_upval**      locup( size_t base, size_t count );
    yl_iterator*    iters( size_t base, size_t count );
    
    void            set_mark( unsigned mark );
    unsigned        get_mark() const;
    
    unsigned        get_locup_base() const;
    unsigned        get_iters_base() const;


protected:

    yl_cothread();


private:

    std::vector< yl_stackframe >    _frames;
    std::vector< yl_value >         _stack;
    std::vector< yl_upval* >        _locup;
    std::vector< yl_iterator >      _iters;
    unsigned                        _mark;

};



/*

*/


inline void yl_upval::close( yl_cothread* cothread )
{
    assert( _open );
    yl_value* s = cothread->stack( _index, 1 );
    _value.set( s[ 0 ] );
    _open = false;
}

inline yl_value yl_upval::get_value( yl_cothread* cothread ) const
{
    if ( _open )
    {
        yl_value* s = cothread->stack( _index, 1 );
        return s[ 0 ];
    }
    else
    {
        return _value.get();
    }
}

inline void yl_upval::set_value( yl_cothread* cothread, const yl_value& value )
{
    if ( _open )
    {
        yl_value* s = cothread->stack( _index, 1 );
        s[ 0 ] = value;
    }
    else
    {
        _value.set( value );
    }
}



inline void yl_cothread::set_mark( unsigned mark )
{
    _mark = mark;
}

inline unsigned yl_cothread::get_mark() const
{
    return _mark;
}





#endif


