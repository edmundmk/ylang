//
//  yvm.h
//
//  Created by Edmund Kapusniak on 09/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YVM_H
#define YVM_H


#include <vector>
#include "yvalue.h"
#include "yfunction.h"


class yarray;
class ytable;


/*
    A standin acts as the prototype of a primitive type or an object which is
    not an expand.  It is necessary so that the is operator has something to
    check against.
*/

class ystandin : public yobject
{
public:

    enum kind
    {
        BOOLEAN,
        NUMBER,
        STRING,
        FUNCTION,
    };

    static ystandin* alloc( kind kind );
    
    bool value_is_this( yvalue value );

    
protected:

    friend class yobject;
    static ymetatype metatype;
    
    explicit ystandin( ymetatype* metatype, kind kind );
    
    
private:

    kind skind;

};





/*
    An iterator implements for ( : ) and for ( :: ) loops.
*/

class yiterator
{
public:

    enum kind
    {
        KEYS,           // over the keys of an object
        ARRAY_INDEX,    // over the elements of an array
        TABLE_INDEX,    // over the keyvalues of a table
    };

    
    explicit yiterator( yexpand* object );
    explicit yiterator( yarray* array );
    explicit yiterator( ytable* table );


    bool        valid();
    void        next1( yvalue* a );
    void        next2( yvalue* a, yvalue* b );
    void        next( ystack* stack, size_t sp, unsigned count );

    void        next_key( yvalue* a, yvalue* b );
    void        next_array( yvalue* a );
    void        next_table( yvalue* a, yvalue* b );

    void        mark_obj( yworklist* work, ycolour unmarked, ycolour marked );


private:

    kind        kind;
    size_t      index;
    union
    {
        struct
        {
    yexpand*    object;
    yclass*     klass;
        };
        struct
        {
    yarray*     array;
    ytuple< yvalue >* values;
        };
        struct
        {
    ytable*     table;
    ytuple< ykeyval< yvalue, yvalue > >* keyvals;
        };
    };

};




/*
    There are separate stacks containing values, upvals, and iterators.
*/

struct ystack
{
    ystack();

    void ensure_stack( size_t size );
    void ensure_upvals( size_t size );

    std::vector< yvalue >       stack;
    std::vector< yupval* >      upvals;
    std::vector< yiterator >    iterators;
    size_t                      up;
    size_t                      mark;
};



/*
    Evaulation loop.
*/

void yexec( size_t sp, unsigned incount, unsigned outcount );



#endif




