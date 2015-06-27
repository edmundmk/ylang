//
//  yl_cothread.h
//
//  Created by Edmund Kapusniak on 12/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_COTHREAD_H
#define YL_COTHREAD_H


#include <vector>
#include "yl_context.h"
#include "yl_function.h"


class yl_object;
class yl_array;
class yl_table;
class yl_cothread;
class yl_bucketlist;




/*
    Check if a number is an integer.
*/

bool is_integer( double number );




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

class yl_upval : public yl_gcobject
{
public:

    static yl_gctype gctype;

    static yl_stackref< yl_upval > alloc( unsigned index );

    void        close( yl_cothread* cothread );

    yl_value    get_value( yl_cothread* cothread ) const;
    void        set_value( yl_cothread* cothread, yl_value value );

private:

    enum
    {
        OPEN = YL_GCFLAG_USER1,
    };

    explicit yl_upval( unsigned index );
    
    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

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


struct yl_iternext
{
    yl_value*   values;
    unsigned    vcount;
};


class yl_iterator
{
public:

    yl_iterator();

    void mark( yl_gcheap* heap ) const;
    void eager_mark( yl_gcheap* heap ) const;

    void open_vals( yl_value value );
    void open_keys( yl_value value );
    void close();

    bool has_values();
    void next1( yl_value* r );
    void next2( yl_value* r, yl_value* b );
    yl_iternext next( yl_value vspace[ 2 ] );


private:

    void next_bucket();


    yl_iterkind _kind;
    union
    {
        yl_object*      _object;
        yl_array*       _array;
        yl_bucketlist*  _buckets;
        yl_cothread*    _generator;
    };
    union
    {
        size_t          _index;
        yl_slot*        _slot;
    };
    
};



/*
    A cothread, with stack frames and the actual stack.

    The following opcodes always push/pop their operands at the stack top:
    
        YL_VARARG
        YL_CALL
        YL_YCALL
        YL_YIELD
        YL_RETURN
        YL_NEXT (not YL_NEXT1 or YL_NEXT2)
        YL_EXTEND
        YL_UNPACK

    When in a function, we request the appropriate stack space for the frame
    by using the stack() method.  This function never shrinks the stack.
    set_mark() sets the stack mark, which can be more or less than the
    current frame, and which must mark the stack top.
 
*/


class yl_cothread : public yl_gcobject
{
public:

    static yl_gctype gctype;

    static yl_stackref< yl_cothread > alloc();

    void            push_frame( const yl_stackframe& frame );
    yl_stackframe   pop_frame();

    yl_value*       stack_alloc( size_t base, unsigned size );
    void            stack_trim( size_t base, unsigned size );
    yl_value*       stack_peek( size_t base, unsigned size );
    yl_value*       stack_mark( size_t base, unsigned mark, unsigned size );
    unsigned        get_top();
    unsigned        get_mark();
    
    yl_upval*&      locup( size_t base, unsigned index );
    void            close_locup( size_t base, unsigned index );
    yl_iterator&    iterator( size_t base, unsigned index );
    void            close_iterator( size_t base, unsigned index );
    
    unsigned        get_locup_base() const;
    unsigned        get_iters_base() const;


protected:

    yl_cothread();

    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );
    static void eager_mark( yl_gcheap* heap, yl_gcobject* object );
    

private:

    friend class yl_callframe;
    friend class yl_upval;

    std::vector< yl_stackframe >    _frames;
    std::vector< yl_value >         _stack;
    std::vector< yl_upval* >        _locup;
    std::vector< yl_iterator >      _iters;
    unsigned                        _mark;

};



/*

*/



inline bool is_integer( double number )
{
    return number == (long)number;
}



/*
    yl_upval
*/


inline void yl_upval::close( yl_cothread* cothread )
{
    if ( check_gcflags( OPEN ) )
    {
        _value.set( cothread->_stack.at( _index ) );
        clear_gcflags( OPEN );
    }
}

inline yl_value yl_upval::get_value( yl_cothread* cothread ) const
{
    if ( check_gcflags( OPEN ) )
    {
        return cothread->_stack.at( _index );
    }
    else
    {
        return _value.get();
    }
}

inline void yl_upval::set_value( yl_cothread* cothread, yl_value value )
{
    if ( check_gcflags( OPEN ) )
    {
        cothread->_stack.at( _index ) = value;
    }
    else
    {
        _value.set( value );
    }
}




/*
    yl_cothread
*/


inline yl_value* yl_cothread::stack_alloc( size_t base, unsigned size )
{
    _stack.resize( base + size );
    return _stack.data() + base;
}

inline void yl_cothread::stack_trim( size_t base, unsigned size )
{
    assert( _stack.size() >= base + size );
    _stack.resize( base + size );
}

inline yl_value* yl_cothread::stack_peek( size_t base, unsigned size )
{
    assert( _stack.size() >= base + size );
    return _stack.data() + base;
}

inline yl_value* yl_cothread::stack_mark( size_t base, unsigned mark, unsigned size )
{
    _stack.resize( base + std::max( mark, size ) );
    _mark = mark;
    return _stack.data() + base;
}

inline unsigned yl_cothread::get_top()
{
    return (unsigned)_stack.size();
}

inline unsigned yl_cothread::get_mark()
{
    return _mark;
}



inline yl_upval*& yl_cothread::locup( size_t base, unsigned index )
{
    if ( _locup.size() <= base + index )
        _locup.resize( base + index + 1 );
    return _locup.at( base + index );
}

inline yl_iterator& yl_cothread::iterator( size_t base, unsigned index )
{
    if ( _iters.size() <= base + index )
        _iters.resize( base + index + 1 );
    return _iters.at( base + index );
}




#endif


