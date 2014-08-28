//
//  xec_module.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_module.h"
#include "xec_string.h"



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
        xec_key::destroy( mkeys[ i ] );
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


