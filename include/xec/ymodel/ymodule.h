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
class yroutine;



/*
    An ymodule is the compiled representation of a source file.
*/

class ymodule : public yobject
{
public:

    static void print( ymodule* module );
    static ymodule* alloc();

    ystring*    name();

    yroutine*   script();
    yroutine*   routine( unsigned f );

    ysymbol     symbol( unsigned k );
    yvalue      value( unsigned v );
    
    
protected:

    friend class yobject;
    friend class yvalue;
    static ymetatype metatype;
    static void mark_module( yobject* object, yworklist* work, ycolour colour );
    
    explicit ymodule( ymetatype* metatype );
    

private:

    friend class xec_ssa_buildcode;
    
    ywb< ystring* >             mname;
    ywb< ytuple< ysymbol >* >   symbols;
#if YSLOTS
    ywb< yslots* >              values;
#else
    ywb< ytuple< yvalue >* >    values;
#endif
    ywb< ytuple< yroutine* >* > routines;

};



/*
    An yroutine contains the code for a single function or coroutine.
*/

class yroutine : public yobject
{
public:

    static void print( yroutine* routine );
    static yroutine* alloc( size_t size );
    
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
    friend class yvalue;
    static ymetatype metatype;
    static void mark_block( yobject* object, yworklist* work, ycolour colour );

    yroutine( ymetatype* metatype, size_t size );
    

private:

    friend class xec_ssa_buildcode;

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

inline yroutine* ymodule::script()
{
    return routines->get( 0 );
}

inline yroutine* ymodule::routine( unsigned f )
{
    return routines->get( f );
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



inline ymodule* yroutine::module()
{
    return fmodule;
}

inline ystring* yroutine::name()
{
    return fname;
}

inline unsigned yroutine::param_count()
{
    return fparamcount;
}

inline unsigned yroutine::upval_count()
{
    return fupvalcount;
}

inline unsigned yroutine::newup_count()
{
    return fnewupcount;
}

inline unsigned yroutine::stack_count()
{
    return fstackcount;
}

inline bool yroutine::is_varargs()
{
    return fvarargs;
}

inline bool yroutine::is_coroutine()
{
    return fcoroutine;
}

inline size_t yroutine::size()
{
    return fsize;
}

inline yinstruction* yroutine::code()
{
    return fcode;
}








#endif
