//
//  yl_function.cpp
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_function.h"






void yl_function::acquire()
{
    if ( _function )
    {
        _function->incref();
    }
}

void yl_function::release()
{
    if ( _function )
    {
        _function->decref();
    }
    _function = nullptr;
}





yl_gctype yl_thunkobj::gctype =
{
    "thunkobj",
    &yl_thunkobj::destroy,
    nullptr,
    nullptr
};


yl_stackref< yl_thunkobj > yl_thunkobj::alloc( yl_thunk_function thunk )
{
    void* p = yl_current->allocate( sizeof( yl_thunkobj ) );
    return new ( p ) yl_thunkobj( thunk );
}

yl_thunkobj::yl_thunkobj( yl_thunk_function thunk )
    :   yl_gcobject( YLOBJ_THUNKOBJ, YL_GCFLAG_LEAF )
    ,   _thunk( thunk )
{
}

void yl_thunkobj::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_thunkobj* self = (yl_thunkobj*)object;
    self->~yl_thunkobj();
}

yl_thunk_function yl_thunkobj::thunk()
{
    return _thunk;
}





yl_gctype yl_funcobj::gctype =
{
    "funcobj",
    &yl_funcobj::destroy,
    &yl_funcobj::mark,
    nullptr
};


yl_function yl_funcobj::make_function( yl_funcobj* funcobj )
{
    return yl_function( funcobj );
}


yl_stackref< yl_funcobj > yl_funcobj::alloc( yl_program* program )
{
    uint8_t upcount = program->upcount();
    void* p = yl_current->allocate(
        sizeof( yl_funcobj ) + sizeof( yl_heapref< yl_upval > ) * upcount );
    return new ( p ) yl_funcobj( program );
}


yl_funcobj::yl_funcobj( yl_program* program )
    :   yl_gcobject( YLOBJ_FUNCOBJ )
    ,   _upcount( program->upcount() )
    ,   _program( program )
{
    for ( size_t i = 0; i < _upcount; ++i )
    {
        new ( _upval + i ) yl_stackref< yl_upval >();
    }
}

void yl_funcobj::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_funcobj* self = (yl_funcobj*)object;
    self->~yl_funcobj();
}

void yl_funcobj::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_funcobj* self = (yl_funcobj*)object;
    self->_program.mark( heap );
    for ( size_t i = 0; i < self->_upcount; ++i )
    {
        self->_upval[ i ].mark( heap );
    }
}





yl_gctype yl_program::gctype =
{
    "program",
    &yl_program::destroy,
    &yl_program::mark,
    nullptr
};


yl_stackref< yl_program > yl_program::alloc
(
    size_t valcount,
    size_t opcount,
    size_t xfcount,
    size_t dvcount,
    size_t dscount
)
{
    size_t size = sizeof( yl_program );
    size += sizeof( yl_valref ) * valcount;
    size += sizeof( yl_opinst ) * opcount;
    size += sizeof( yl_xframe ) * xfcount;
    size += sizeof( yl_debugvar ) * dvcount;
    size += sizeof( yl_debugspan ) * dscount;
    void* p = yl_current->allocate( size );
    return new ( p ) yl_program( valcount, opcount, xfcount, dvcount, dscount );
}


yl_program::yl_program
(
    uint16_t    valcount,
    size_t      opcount,
    size_t      xfcount,
    size_t      dvcount,
    size_t      dscount
)
    :   yl_gcobject( YLOBJ_PROGRAM )
    ,   _valcount( valcount )
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
    yl_valref* values = _values();
    for ( size_t i = 0; i < _valcount; ++i )
    {
        new ( values + i ) yl_valref();
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
    yl_valref* values = _values();
    for ( size_t i = 0; i < _valcount; ++i )
    {
        values[ i ].~yl_valref();
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
    
    yl_valref* values = self->_values();
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


yl_valref* yl_program::_values()
{
    return (yl_valref*)( this + 1 );
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








