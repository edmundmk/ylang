//
//  xec_object.h
//
//  Created by Edmund Kapusniak on 29/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_OBJECT_H
#define XEC_OBJECT_H


#include <unordered_map>
#include <hash.h>
#include "xec_value.h"


class xec_function;

class xec_objkey;
class xec_upval;
class xec_object;
class xec_array;
class xec_table;
class xec_closure;
class xec_iter;



/*
    Keys are what you use to look up values in objects.
*/


class xec_objkey
{
public:

    static xec_objkey create( const char* key );
    static xec_objkey create( const char* key, size_t size );
    static void destroy( xec_objkey key );

    const char* c_str() const;


private:

    xec_objkey( void* k );

    friend bool operator == ( xec_objkey a, xec_objkey b );
    friend bool operator != ( xec_objkey a, xec_objkey b );
    friend std::hash< xec_objkey >;

    void* k;
    
};


bool operator == ( xec_objkey a, xec_objkey b );
bool operator != ( xec_objkey a, xec_objkey b );

namespace std
{
template <> struct hash< xec_objkey >
{
    size_t operator () ( const xec_objkey& s ) const;
};
}




/*
    Upvals are variables from outer scopes referenced by a function closure.
*/

class xec_upval
{
public:

    xec_upval( xec_value value );
    
    void        incref();
    void        decref();

    xec_value   get() const;
    void        assign( xec_value value );
    

private:

    unsigned        refcount;
    xec_value       mvalue;

};






/*
    Base class for xec objects.
    
    I would quite like to have a 'dynamic class' system where objects are
    re-laid-out as keys are added and removed, and there is no need for a
    hash map per-object.
    
    Also we need to work out better garbage collection.
*/

class xec_object
{
public:

    xec_object();

    void            incref();
    void            decref();

    xec_value       key( xec_objkey key ) const;
    void            setkey( xec_objkey key, xec_value value );

    bool            isarray() const;
    bool            istable() const;
    bool            isclosure() const;
    bool            isiter() const;

    xec_array&      array();
    xec_table&      table();
    xec_closure&    closure();
    xec_iter&       iter();


protected:

    enum kind
    {
        OBJECT,
        ARRAY,
        TABLE,
        CLOSURE,
        ITER,
    };

    xec_object( kind kind );
    

private:

    int         refcount;
    kind        mkind;
    std::unordered_map< xec_objkey, xec_value > mkeys;

};






/*
    Closures are function objects (plus closed-over upvals).
*/


class xec_closure : public xec_object
{
public:

    explicit xec_closure( xec_function* function );
    ~xec_closure();
    
    void            envup( unsigned i, xec_upval* upval );
    
    xec_function*   function() const;
    xec_upval*      upval( unsigned i ) const;
    
private:

    xec_function*   mfunction;
    xec_upval**     mupvals;

};






#endif
