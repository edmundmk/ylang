//
//  xec_code.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_code.h"
#include <new>
#include <string.h>
#include "xec_string.h"
#include "xec_object.h"



xec_module::xec_module()
    :   mname( NULL )
    ,   mkeys( NULL )
    ,   mkeycount( 0 )
    ,   mvalues( NULL )
    ,   mvaluecount( 0 )
    ,   mfuncs( NULL )
    ,   mfunccount( 0 )
{
}

xec_module::~xec_module()
{
    for ( size_t i = 0; i < mkeycount; ++i )
    {
        xec_objkey::destroy( mkeys[ i ] );
    }
    
    for ( size_t i = 0; i < mvaluecount; ++i )
    {
        if ( mvalues[ i ].isstring() )
        {
            mvalues[ i ].string().decref();
        }
    }
    
    for ( size_t i = 0; i < mfunccount; ++i )
    {
        delete mfuncs[ i ];
    }
    
    free( mname );
    free( mkeys );
    free( mvalues );
    free( mfuncs );
}









xec_function::xec_function()
    :   mmodule( NULL )
    ,   mname( NULL )
    ,   mcode( NULL )
    ,   mcodecount( 0 )
    ,   mparamcount( 0 )
    ,   mupvalcount( 0 )
    ,   mnewupcount( 0 )
    ,   mstackcount( 0 )
    ,   mvarargs( false )
    ,   mcoroutine( false )
{
}

xec_function::~xec_function()
{
    free( mname );
    free( mcode );
}



xec_module* xec_function::module()
{
    return mmodule;
}

const char* xec_function::name()
{
    return mname;
}

xec_opinst xec_function::code( unsigned pc )
{
    assert( pc < mcodecount );
    return mcode[ pc ];
}

unsigned xec_function::code_count()
{
    return mcodecount;
}

unsigned xec_function::param_count()
{
    return mparamcount;
}

unsigned xec_function::upval_count()
{
    return mupvalcount;
}

unsigned xec_function::newup_count()
{
    return mnewupcount;
}

unsigned xec_function::stack_count()
{
    return mstackcount;
}

bool xec_function::is_varargs()
{
    return mvarargs;
}

bool xec_function::is_coroutine()
{
    return mcoroutine;
}



