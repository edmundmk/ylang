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
class yupval;
class yfunction;




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
    Invoking functions.
    TODO: Support arguments and return values.
*/

void yinvoke( yfunction* function );




/*

*/


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




#endif




