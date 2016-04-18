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
#include "yl_iterator.h"
#include "yl_value.h"


class yl_funcobj;
class yl_object;
class yl_array;
class yl_table;
class yl_cothread;
class yl_bucketlist;
class yl_upval;



/*
    Check if a number is an integer.
*/

bool is_integer( double number );




/*
    A stack frame.
*/

enum yl_framekind : uint8_t
{
    YL_FRAME_NATIVE,    // first call from native code
    YL_FRAME_LOCAL,     // call on same cothread
    YL_FRAME_YOPEN,     // before first call on new cothread
    YL_FRAME_YCALL,     // first call on new cothread
};


struct yl_stackframe
{
    yl_funcobj*     funcobj;        // function
    unsigned        ip;             // instruction pointer
    unsigned        sp;             // stack pointer
    unsigned        fp;             // frame pointer (after vararg adjustment)
    unsigned        locup_base;     // local upval pointer
    unsigned        iters_base;     // iterators pointer
    uint8_t         rcount;         // requested number of result values
    yl_framekind    kind;           // kind.
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

    yl_cothread();

    bool            has_frames() const;
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
    yl_cothread
*/


inline bool yl_cothread::has_frames() const
{
    return ! _frames.empty();
}


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
    _mark = (unsigned)base + mark;
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


