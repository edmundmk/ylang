//
//  ymodule.h
//
//  Created by Edmund Kapusniak on 06/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YMODULE_H
#define YMODULE_H


#include "yobject.h"
#include "ystring.h"
#include "ytuple.h"
#include "yslots.h"
#include "yvalue.h"
#include "ycode.h"


class ymodule;
class yrecipe;



/*
    An ymodule is the compiled representation of a source file.
*/

class ymodule : public yobject
{
public:

    static void print( ymodule* module );
    static ymodule* alloc();

    ystring*    name();

    yrecipe*    script();
    yrecipe*    recipe( unsigned f );

    ysymbol     symbol( unsigned k );
    yvalue      value( unsigned v );
    
    
protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_module( yobject* object, yworklist* work, ycolour colour );
    
    explicit ymodule( ymetatype* metatype );
    

private:

    friend class xec_ssa_buildcode;
    friend class xssa_ycodeout;
    
    ywb< ystring* >             mname;
    ywb< ytuple< ysymbol >* >   symbols;
#if YSLOTS
    ywb< yslots* >              values;
#else
    ywb< ytuple< yvalue >* >    values;
#endif
    ywb< ytuple< yrecipe* >* >  recipes;

};



/*
    An yrecipe contains the code for a single function or coroutine.
*/

class yrecipe : public yobject
{
public:

    static void print( yrecipe* routine );
    static yrecipe* alloc( size_t size );
    
    ymodule*        module();
    ystring*        name();
    
    unsigned        param_count();
    unsigned        upval_count();
    unsigned        newup_count();
    unsigned        stack_count();
    bool            is_varargs();
    bool            is_coroutine();
    
    size_t          size();
    yinstruction*   code();
    
    
protected:

    friend class yobject;
    static ymetatype metatype;
    static void mark_block( yobject* object, yworklist* work, ycolour colour );

    yrecipe( ymetatype* metatype, size_t size );
    

private:

    friend class xec_ssa_buildcode;
    friend class xssa_ycodeout;

    ywb< ymodule* > fmodule;
    ywb< ystring* > fname;
    unsigned        fparamcount;
    unsigned        fupvalcount;
    unsigned        fnewupcount;
    unsigned        fstackcount;
    bool            fvarargs;
    bool            fcoroutine;
    size_t          fsize;
    yinstruction    fcode[];

};




/*

*/


inline ystring* ymodule::name()
{
    return mname;
}

inline yrecipe* ymodule::script()
{
    return recipes->get( 0 );
}

inline yrecipe* ymodule::recipe( unsigned f )
{
    return recipes->get( f );
}

inline ysymbol ymodule::symbol( unsigned k )
{
    return symbols->get( k );
}

inline yvalue ymodule::value( unsigned v )
{
#if YSLOTS
    return values->get( v );
#else
    return values->get( v );
#endif
}



inline ymodule* yrecipe::module()
{
    return fmodule;
}

inline ystring* yrecipe::name()
{
    return fname;
}

inline unsigned yrecipe::param_count()
{
    return fparamcount;
}

inline unsigned yrecipe::upval_count()
{
    return fupvalcount;
}

inline unsigned yrecipe::newup_count()
{
    return fnewupcount;
}

inline unsigned yrecipe::stack_count()
{
    return fstackcount;
}

inline bool yrecipe::is_varargs()
{
    return fvarargs;
}

inline bool yrecipe::is_coroutine()
{
    return fcoroutine;
}

inline size_t yrecipe::size()
{
    return fsize;
}

inline yinstruction* yrecipe::code()
{
    return fcode;
}








#endif
