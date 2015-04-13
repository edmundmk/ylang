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
#include "yl_heapobj.h"
#include "yl_value.h"



class yl_funcobj;
class yl_program;
class yl_upval;




/*
    An instance of a function.
*/

class yl_funcobj : public yl_heapobj
{
public:

    static yl_function make_function( yl_funcobj* funcobj );
    static yl_funcobj* alloc( yl_program* program );

    void acquire();
    void release();


private:

    explicit yl_funcobj( yl_program* program );

    uint8_t                     _upcount;
    uint8_t                     _refcount;
    yl_heapref< yl_program >    _program;
    yl_heapref< yl_upval >      _upval[ 0 ];

};




/*
    An upval.
*/

class yl_upval : public yl_heapobj
{
public:


private:

    bool        _open;
    uint32_t    _index;
    yl_value    _value;

};




/*
    A compiled function.
*/

class yl_program : public yl_heapobj
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
    
    size_t                  paramcount();
    bool                    varargs();
    bool                    coroutine();

    size_t                  upcount();
    size_t                  stackcount();
    
    size_t                  valcount();
    const yl_value*         values();
    
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
    
    uint8_t                 _paramcount;
    uint8_t                 _upcount;
    uint8_t                 _stackcount;
    uint8_t                 _itercount      : 6;
    bool                    _varargs        : 1;
    bool                    _coroutine      : 1;

    yl_value*               _values();      // _values[ _valcount ]
    yl_opinst*              _ops();         // _ops[ _opcount ]
    yl_xframe*              _xframes();     // _xframes[ _xfcount ]
    yl_debugvar*            _debugvars();   // _debugvars[ _dvcount ]
    yl_debugspan*           _debugspans();  // _debugspans[ _dscount ]

};



/*

*/



inline yl_string* yl_program::name()
{
    return _name.get();
}

inline size_t yl_program::paramcount()
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

inline size_t yl_program::upcount()
{
    return _upcount;
}

inline size_t yl_program::stackcount()
{
    return _stackcount;
}

inline size_t yl_program::valcount()
{
    return _valcount;
}

inline const yl_value* yl_program::values()
{
    return (yl_value*)( this + 1 );
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




