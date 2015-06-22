//
//  yl_funcobj.h
//
//  Created by Edmund Kapusniak on 05/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#ifndef YL_FUNCTION_H
#define YL_FUNCTION_H


#include <vector>
#include "yl_code.h"
#include "yl_context.h"
#include "yl_value.h"



class yl_funcobj;
class yl_program;
class yl_upval;




/*
    A native thunk.
*/

typedef void (*yl_thunk_function)( yl_callframe& xf );

class yl_thunkobj : public yl_gcobject
{
public:

    static yl_stackref< yl_thunkobj > alloc( yl_thunk_function thunk );

    yl_thunk_function thunk();


private:

    explicit yl_thunkobj( yl_thunk_function thunk );

    yl_thunk_function _thunk;

};



/*
    A bytecode function.
*/

class yl_funcobj : public yl_gcobject
{
public:

    static yl_function  make_function( yl_funcobj* funcobj );
    static yl_stackref< yl_funcobj > alloc( yl_program* program );

    yl_program*         program();
    
    void                set_upval( size_t index, yl_upval* upval );
    yl_upval*           get_upval( size_t index );


private:

    explicit yl_funcobj( yl_program* program );

    unsigned                    _upcount;
    yl_heapref< yl_program >    _program;
    yl_heapref< yl_upval >      _upval[ 0 ];

};



/*
    A compiled function.
*/

class yl_program : public yl_gcobject
{
public:

    static yl_program* alloc
    (
        uint16_t    valcount,
        size_t      opcount,
        size_t      xfcount,
        size_t      dvcount,
        size_t      dscount
    );
    ~yl_program();
    
    void                    print();
    
    yl_string*              name();
    
    unsigned                upcount();
    unsigned                paramcount();
    bool                    varargs();
    bool                    coroutine();

    unsigned                stackcount();
    unsigned                locupcount();
    unsigned                iterscount();
    
    size_t                  valcount();
    const yl_valref*        values();
    
    size_t                  opcount();
    const yl_opinst*        ops();
    

private:

    friend class ygen_emit;
    
    yl_program
    (
        uint16_t    valcount,
        size_t      opcount,
        size_t      xfcount,
        size_t      dvcount,
        size_t      dscount
    );

    uint16_t                _valcount;
    size_t                  _opcount;
    size_t                  _xfcount;
    size_t                  _dvcount;
    size_t                  _dscount;
    
    yl_heapref< yl_string > _name;
    
    uint8_t                 _upcount;
    uint8_t                 _paramcount;
    uint8_t                 _stackcount;
    uint8_t                 _locupcount;
    uint8_t                 _iterscount;
    bool                    _varargs;
    bool                    _coroutine;

    yl_valref*              _values();      // _values[ _valcount ]
    yl_opinst*              _ops();         // _ops[ _opcount ]
    yl_xframe*              _xframes();     // _xframes[ _xfcount ]
    yl_debugvar*            _debugvars();   // _debugvars[ _dvcount ]
    yl_debugspan*           _debugspans();  // _debugspans[ _dscount ]

};



/*

*/


#include "yl_cothread.h"



inline yl_program* yl_funcobj::program()
{
    return _program.get();
}

inline void yl_funcobj::set_upval( size_t index, yl_upval* upval )
{
    assert( index < _upcount );
    _upval[ index ].set( upval );
}

inline yl_upval* yl_funcobj::get_upval( size_t index )
{
    assert( index < _upcount );
    return _upval[ index ].get();
}





inline yl_string* yl_program::name()
{
    return _name.get();
}

inline unsigned yl_program::paramcount()
{
    return _paramcount;
}

inline bool yl_program::varargs()
{
    return _varargs;
}

inline bool yl_program::coroutine()
{
    return _coroutine;
}

inline unsigned yl_program::upcount()
{
    return _upcount;
}

inline unsigned yl_program::stackcount()
{
    return _stackcount;
}

inline unsigned yl_program::locupcount()
{
    return _locupcount;
}

inline unsigned yl_program::iterscount()
{
    return _iterscount;
}

inline size_t yl_program::valcount()
{
    return _valcount;
}

inline const yl_valref* yl_program::values()
{
    return (yl_valref*)( this + 1 );
}

inline size_t yl_program::opcount()
{
    return _opcount;
}

inline const yl_opinst* yl_program::ops()
{
    return (yl_opinst*)( values() + valcount() );
}





#endif




