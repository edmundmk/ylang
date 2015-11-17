//
//  yl_program.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_program.h"
#include "yl_debuginfo.h"



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
    size_t ilcount,
    size_t xfcount
)
{
    size_t size = sizeof( yl_program );
    size += sizeof( yl_heapval ) * valcount;
    size += sizeof( yl_opinst ) * opcount;
    size += sizeof( yl_ilcache ) * ilcount;
    size += sizeof( yl_xframe ) * xfcount;
    return yl_gcalloc< yl_program >( size, valcount, opcount, ilcount, xfcount );
}


yl_program::yl_program
(
    size_t valcount,
    size_t opcount,
    size_t ilcount,
    size_t xfcount
)
    :   _valcount( (unsigned)valcount )
    ,   _opcount( (unsigned)opcount )
    ,   _ilcount( (unsigned)ilcount )
    ,   _xfcount( (unsigned)xfcount )
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
    
    yl_ilcache* ilcache = _ilcache();
    for ( size_t i = 0; i < _ilcount; ++i )
    {
        new ( ilcache + i ) yl_ilcache();
    }
    
    yl_xframe* xframes = _xframes();
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        new ( xframes + i ) yl_xframe();
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
    
    yl_ilcache* ilcache = _ilcache();
    for ( size_t i = 0; i < _ilcount; ++i )
    {
        ilcache[ i ].~yl_ilcache();
    }
    
    yl_xframe* xframes = _xframes();
    for ( size_t i = 0; i < _xfcount; ++i )
    {
        xframes[ i ].~yl_xframe();
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
    printf( "%s\n", _debuginfo ? _debuginfo->funcname() : "[program]" );

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
    
    if ( _debuginfo )
    {
        _debuginfo->print();
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

yl_ilcache* yl_program::_ilcache()
{
    return (yl_ilcache*)( _ops() + _opcount );
}

yl_xframe* yl_program::_xframes()
{
    return (yl_xframe*)( _ilcache() + _ilcount );
}



