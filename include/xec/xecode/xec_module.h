//
//  xec_module.h
//
//  Created by Edmund Kapusniak on 17/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_MODULE_H
#define XEC_MODULE_H


#include <stdint.h>
#include <assert.h>
#include "xec_value.h"
#include "xec_code.h"


class xec_module;
class xec_function;



class xec_module
{
public:

    ~xec_module();

    const char*     name();
    
    xec_function*   script_function();
    
    xec_key*        key( unsigned k );
    xec_value       value( unsigned v );
    xec_function*   function( unsigned f );


private:

    friend class xec_ssa_buildcode;

    xec_module();

    char*           mname;
    xec_key**       mkeys;
    size_t          mkeycount;
    xec_value*      mvalues;
    size_t          mvaluecount;
    xec_function**  mfuncs;
    size_t          mfunccount;

};



class xec_function
{
public:

    ~xec_function();

    xec_module*     module();
    const char*     name();
    
    xec_opinst      code( unsigned pc );
    size_t          param_count();
    size_t          upval_count();
    size_t          newup_count();
    size_t          stack_count();
    bool            is_varargs();
    bool            is_coroutine();

    
private:

    friend class xec_ssa_buildcode;

    xec_function();

    xec_module*     mmodule;
    char*           mname;
    xec_opinst*     mcode;
    uint32_t        mcodecount;
    uint32_t        mparamcount;
    uint32_t        mupvalcount;
    uint32_t        mnewupcount;
    uint32_t        mstackcount;
    bool            mvarargs;
    bool            mcoroutine;

};



void xec_module_print( xec_module* module );




/*
*/


inline const char* xec_module::name()
{
    return mname;
}
    
inline xec_function* xec_module::script_function()
{
    assert( 0 < mfunccount );
    return mfuncs[ 0 ];
}
    
inline xec_key* xec_module::key( unsigned k )
{
    assert( k < mkeycount );
    return mkeys[ k ];
}

inline xec_value xec_module::value( unsigned v )
{
    assert( v < mvaluecount );
    return mvalues[ v ];
}

inline xec_function* xec_module::function( unsigned f )
{
    assert( f < mfunccount );
    return mfuncs[ f ];
}




inline xec_module* xec_function::module()
{
    return mmodule;
}

inline const char* xec_function::name()
{
    return mname;
}

inline xec_opinst xec_function::code( unsigned pc )
{
    assert( pc < mcodecount );
    return mcode[ pc ];
}

inline size_t xec_function::param_count()
{
    return mparamcount;
}

inline size_t xec_function::upval_count()
{
    return mupvalcount;
}

inline size_t xec_function::newup_count()
{
    return mnewupcount;
}

inline size_t xec_function::stack_count()
{
    return mstackcount;
}

inline bool xec_function::is_varargs()
{
    return mvarargs;
}

inline bool xec_function::is_coroutine()
{
    return mcoroutine;
}







#endif
