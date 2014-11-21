//
//  yfunction.h
//
//  Created by Edmund Kapusniak on 09/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YFUNCTION_H
#define YFUNCTION_H


#include <vector>
#include "yobject.h"
#include "yvalue.h"


class yrecipe;
class yframe;
class yupval;
class yfunction;




/*
    yframe is a stack frame, used either to invoke a function or for the
    parameters of a thunk from ylang to C++.  Once you start pushing values
    size() will return 0 and operator [] is no longer valid.
    
    Example:
 
        yframe frame;
        frame.push( parameter )
        frame.push( parameter )
        yinvoke( function, frame );
        yvalue result = frame[ 0 ];
        
 
    Thunk:
    
        void ythunk( yframe& frame )
        {
            ystring* string = frame[ 0 ].as_string();
            double number = frame[ 1 ].as_number();
            double result = number * number;
            frame.push( result );
        }
 
*/


class yframe
{
public:

    yframe();

    size_t size() const;
    yvalue operator [] ( size_t index ) const;

    template < typename ... arguments_t >
    void push( yvalue val, arguments_t ... vals );
    void push( yvalue val );



private:

    friend void yinvoke( yfunction* function, yframe& frame );
    friend void yexec( size_t, unsigned, unsigned );

    yframe( yvalue* s, yvalue* limit, size_t count );

    void        grow_stack();

    yvalue*     s;
    yvalue*     limit;
    size_t      count;

};







/*
    yupvals are variables which are referenced by closures.  Because the
    closure can escape the scope in which the variable is declared, they
    must be allocated on the heap.

    It would be nice to have some kind of (dynamic?) escape analyis to
    identify upvals which do not escape their scopes - this would require
    identifying when closures die.
    
    Or we could package more than one upval into 'frame' objects.  Each block
    knows how many of its variables are upvals, and already accesses them
    through the upval instructions.  Instead of each upval being a separate
    object, each frame is a separate object.  Accessing upvals inside the
    closure would need to identify which frame it comes from.
*/

class yupval : public yobject
{
public:

    static yupval* alloc( yvalue value );

    yvalue  get();
    void    set( yvalue value );


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_upval( yobject* object, yworklist* work, ycolour colour );

    yupval( ymetatype* metatype, yvalue value );
    

private:

    ywb< yvalue > value;

};





/*
    Function closures.
*/

class yfunction : public yobject
{
public:

    static yfunction* alloc( yexpand* global, yrecipe* recipe );
    
    yexpand*    global();
    yrecipe*    recipe();
    yupval*     getupval( size_t u );
    void        setupval( size_t u, yupval* upval );


protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_function( yobject* object, yworklist* work, ycolour colour );

    yfunction( ymetatype* metatype, yexpand* global, yrecipe* recipe );
    
    
private:

    ywb< yexpand* >             fglobal;
    ywb< yrecipe* >             frecipe;
    ywb< ytuple< yupval* >* >   fupvals;

};



/*
    C++ thunks that ylang functions can invoke.
*/

typedef void (*ythunk)( yframe& );




/*
    Invoking functions.
*/

template < typename ... arguments_t >
yvalue yinvoke( yfunction* function, arguments_t ... args );
yvalue yinvoke( yfunction* function );
void yinvoke( yfunction* function, yframe& frame );




/*

*/


/*
    yframe
*/



inline yframe::yframe( yvalue* s, yvalue* limit, size_t count )
    :   s( s )
    ,   limit( limit )
    ,   count( count )
{
}

inline size_t yframe::size() const
{
    return count;
}

inline yvalue yframe::operator [] ( size_t index ) const
{
    if ( index < count )
        return s[ 1 + index ];
    else
        return nullptr;
}

template < typename ... arguments_t >
inline void yframe::push( yvalue val, arguments_t ... vals )
{
    push( val );
    push( vals ... );
}

inline void yframe::push( yvalue val )
{
    if ( s >= limit )
    {
        grow_stack();
    }
    *s++ = val;
    count = 0;
}




/*
    yupval
*/


inline yvalue yupval::get()
{
    return value;
}

inline void yupval::set( yvalue value )
{
    this->value = value;
}



/*
    yfunction
*/

inline yexpand* yfunction::global()
{
    return fglobal;
}

inline yrecipe* yfunction::recipe()
{
    return frecipe;
}

inline yupval* yfunction::getupval( size_t u )
{
    return fupvals->get( u );
}

inline void yfunction::setupval( size_t u, yupval* upval )
{
    fupvals->set( u, upval );
}



/*
    yinvoke
*/


template < typename ... arguments_t >
yvalue yinvoke( yfunction* function, arguments_t ... args )
{
    yframe frame;
    frame.push( args ... );
    yinvoke( function, frame );
    return frame[ 0 ];
}

inline yvalue yinvoke( yfunction* function )
{
    yframe frame;
    yinvoke( function, frame );
    return frame[ 0 ];
}







#endif




