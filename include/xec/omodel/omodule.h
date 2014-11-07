//
//  omodule.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OMODULE_H
#define OMODULE_H


#include "obase.h"
#include "ostring.h"
#include "otuple.h"
#include "oslotlist.h"
#include "ovalue.h"
#include "ocode.h"


class omodule;
class ofunction;



/*
    An omodule is the compiled representation of a source file.
*/

class omodule : public obase
{
public:

    static void print( omodule* module );

    static ometatype metatype;
    static omodule* alloc();

    ostring*    name();

    ofunction*  script();
    ofunction*  function( unsigned f );

    osymbol     symbol( unsigned k );
    ovalue      value( unsigned v );
    
    
protected:

    static void mark_module( oworklist* work, obase* object, ocolour colour );
    
    explicit omodule( ometatype* metatype );
    

private:

    friend class xec_ssa_buildcode;
    
    owb< ostring* >                 mname;
    owb< otuple< osymbol >* >       symbols;
#if OVALUE64
    owb< otuple< ovalue >* >        values;
#else
    owb< oslotlist* >               values;
#endif
    owb< otuple< ofunction* >* >    functions;

};



/*
    An ofunction contains the code for a single function.
*/

class ofunction : public obase
{
public:

    static void print( ofunction* function );

    static ometatype metatype;
    static ofunction* alloc( size_t size );
    
    omodule*        module();
    ostring*        name();
    
    unsigned        param_count();
    unsigned        upval_count();
    unsigned        newup_count();
    unsigned        stack_count();
    bool            is_varargs();
    bool            is_coroutine();
    
    size_t          size();
    oinstruction*   code();
    
    
protected:
    
    static void mark_function( oworklist* work, obase* object, ocolour colour );

    ofunction( ometatype* metatype, size_t size );
    

private:

    friend class xec_ssa_buildcode;

    owb< omodule* > fmodule;
    owb< ostring* > fname;
    unsigned        fparamcount;
    unsigned        fupvalcount;
    unsigned        fnewupcount;
    unsigned        fstackcount;
    bool            fvarargs;
    bool            fcoroutine;
    size_t          fsize;
    oinstruction    fcode[];

};




/*

*/


inline ostring* omodule::name()
{
    return mname;
}

inline ofunction* omodule::script()
{
    return functions->at( 0 );
}

inline ofunction* omodule::function( unsigned f )
{
    return functions->at( f );
}

inline osymbol omodule::symbol( unsigned k )
{
    return symbols->at( k );
}

inline ovalue omodule::value( unsigned v )
{
#if OVALUE64
    return values->at( v );
#else
    return values->load( v );
#endif
}



inline omodule* ofunction::module()
{
    return fmodule;
}

inline ostring* ofunction::name()
{
    return fname;
}

inline unsigned ofunction::param_count()
{
    return fparamcount;
}

inline unsigned ofunction::upval_count()
{
    return fupvalcount;
}

inline unsigned ofunction::newup_count()
{
    return fnewupcount;
}

inline unsigned ofunction::stack_count()
{
    return fstackcount;
}

inline bool ofunction::is_varargs()
{
    return fvarargs;
}

inline bool ofunction::is_coroutine()
{
    return fcoroutine;
}

inline size_t ofunction::size()
{
    return fsize;
}

inline oinstruction* ofunction::code()
{
    return fcode;
}








#endif
