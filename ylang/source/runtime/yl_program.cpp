//
//  yl_program.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_program.h"





yl_gctype yl_program::gctype =
{
    YLOBJ_PROGRAM,
    YL_GCFLAG_NONE,
    "program",
    &yl_program::destroy,
    &yl_program::mark,
    nullptr
};


yl_rootref< yl_program > yl_program::alloc
(
    size_t valcount,
    size_t opcount,
    size_t xfcount,
    size_t dvcount,
    size_t dscount
)
{
    size_t size = sizeof( yl_program );
    size += sizeof( yl_heapval ) * valcount;
    size += sizeof( yl_opinst ) * opcount;
    size += sizeof( yl_xframe ) * xfcount;
    size += sizeof( yl_debugvar ) * dvcount;
    size += sizeof( yl_debugspan ) * dscount;
    return yl_gcalloc< yl_program >( size, valcount, opcount, xfcount, dvcount, dscount );
}


yl_program::yl_program
(
    size_t valcount,
    size_t opcount,
    size_t xfcount,
    size_t dvcount,
    size_t dscount
)
    :   _valcount( valcount )
    ,   _opcount( opcount )
    ,   _xfcount( xfcount )
    ,   _dvcount( dvcount )
    ,   _dscount( dscount )
    ,   _paramcount( 0 )
    ,   _upcount( 0 )
    ,   _stackcount( 0 )
    ,   _varargs( false )
    ,   _coroutine( false )
{
    yl_heapval* values = _values();
    for ( size_t i = 0; i < _valcount; ++i )
    {
        new ( values + i ) yl_heapval();
    }
    
    yl_opinst* ops = _ops();
    for ( size_t i = 0; i < _opcount; ++i )
    {
        new ( ops + i ) yl_opinst();
    }
    
    yl_xframe* xframes = _xframes();
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        new ( xframes + i ) yl_xframe();
    }
    
    yl_debugvar* debugvars = _debugvars();
    for ( size_t i = 0; i < _dvcount; ++i )
    {
        new ( debugvars + i ) yl_debugvar();
    }
    
    yl_debugspan* debugspans = _debugspans();
    for ( size_t i = 0; i < _dscount; ++i )
    {
        new ( debugspans + i ) yl_debugspan();
    }
}

yl_program::~yl_program()
{
    yl_heapval* values = _values();
    for ( size_t i = 0; i < _valcount; ++i )
    {
        values[ i ].~yl_heapval();
    }
    
    yl_opinst* ops = _ops();
    for ( size_t i = 0; i < _opcount; ++i )
    {
        ops[ i ].~yl_opinst();
    }
    
    yl_xframe* xframes = _xframes();
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        xframes[ i ].~yl_xframe();
    }
    
    yl_debugvar* debugvars = _debugvars();
    for ( size_t i = 0; i < _dvcount; ++i )
    {
        debugvars[ i ].~yl_debugvar();
    }
    
    yl_debugspan* debugspans = _debugspans();
    for ( size_t i = 0; i < _dscount; ++i )
    {
        debugspans[ i ].~yl_debugspan();
    }
}

void yl_program::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_program* self = (yl_program*)object;
    self->~yl_program();
}

void yl_program::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_program* self = (yl_program*)object;
    
    yl_heapval* values = self->_values();
    for ( size_t i = 0; i < self->_valcount; ++i )
    {
        values[ i ].mark( heap );
    }    
}


void yl_program::print()
{
    printf( "%s\n", _name.c_str() );

    printf( "    paramcount : %u\n", _paramcount );
    printf( "    upcount    : %u\n", _upcount );
    printf( "    stackcount : %u\n", _stackcount );
    printf( "    itercount  : %u\n", _iterscount );
    printf( "    varargs    : %s\n", _varargs ? "true" : "false" );
    printf( "    coroutine  : %s\n", _coroutine ? "true" : "false" );

    if ( _xfcount )
    {
        printf( "    xframes:\n" );
        yl_xframe* xframes = _xframes();
        for ( size_t i = 0; i < _xfcount; ++i )
        {
            const yl_xframe& xf = xframes[ i ];
            printf
            (
                "        %04X:%04X *%u ~%u @%04X\n",
                xf.start,
                xf.end,
                xf.close_upvals,
                xf.close_iterators,
                xf.handler
            );
        }
    }
    
    if ( _dvcount )
    {
        printf( "    debugvars:\n" );
        yl_debugvar* debugvars = _debugvars();
        yl_debugspan* debugspans = _debugspans();
        for ( size_t varindex = 0; varindex < _dvcount; ++varindex )
        {
            const yl_debugvar& dv = debugvars[ varindex ];
            printf
            (
                "        %s %s%u",
                dv.name.c_str(),
                varindex < _upcount ? "*" : "",
                dv.r
            );
            for ( size_t i = 0; i < _dscount; ++i )
            {
                const yl_debugspan& ds = debugspans[ i ];

                if ( ds.varindex != varindex )
                    continue;
                
                printf( " %04X:%04X", ds.start, ds.end );
            }
            printf( "\n" );
        }
    }

    yl_opinst* ops = _ops();
    for ( size_t i = 0; i < _opcount; ++i )
    {
        ylop_print( i, ops + i );
    }
}


yl_heapval* yl_program::_values()
{
    return (yl_heapval*)( this + 1 );
}

yl_opinst* yl_program::_ops()
{
    return (yl_opinst*)( _values() + _valcount );
}

yl_xframe* yl_program::_xframes()
{
    return (yl_xframe*)( _ops() + _opcount );
}

yl_debugvar* yl_program::_debugvars()
{
    return (yl_debugvar*)( _xframes() + _xfcount );
}

yl_debugspan* yl_program::_debugspans()
{
    return (yl_debugspan*)( _debugvars() + _dvcount );
}



