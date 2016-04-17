//
//  yl_program.h
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_PROGRAM_H
#define YL_PROGRAM_H


#include "yl_gcheap.h"
#include "yl_value.h"
#include "yl_code.h"


class yl_debuginfo;



/*
    A compiled function.
*/

class yl_program : public yl_gcobject
{
public:

    static yl_gctype gctype;

    static yl_rootref< yl_program > alloc
    (
        size_t valcount,
        size_t opcount,
        size_t ilcount,
        size_t xfcount
    );
    
    const char*             name();
    void                    print();
    
    unsigned                upcount();
    unsigned                paramcount();
    bool                    varargs();
    bool                    coroutine();

    unsigned                stackcount();
    unsigned                locupcount();
    unsigned                iterscount();
    
    size_t                  valcount();
    const yl_heapval*       values();
    
    size_t                  opcount();
    const yl_opinst*        ops();
    
    size_t                  ilcount();
    yl_ilcache*             ilcache();
    

private:

    friend_yl_gcalloc;
    friend class ygen_emit;
    
    yl_program
    (
        size_t valcount,
        size_t opcount,
        size_t ilcount,
        size_t xfcount
    );

    ~yl_program();

    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

    unsigned        _valcount;
    unsigned        _opcount;
    unsigned        _ilcount;
    unsigned        _xfcount;
    
    uint8_t         _upcount;
    uint8_t         _paramcount;
    uint8_t         _stackcount;
    uint8_t         _locupcount;
    uint8_t         _iterscount;
    bool            _varargs;
    bool            _coroutine;
    
    std::unique_ptr< yl_debuginfo > _debuginfo;

    yl_heapval*     _values();      // _values[ _valcount ]
    yl_opinst*      _ops();         // _ops[ _opcount ]
    yl_ilcache*     _ilcache();     // _ilcache[ _ilcount ]
    yl_xframe*      _xframes();     // _xframes[ _xfcount ]

};



/*

*/



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

inline const yl_heapval* yl_program::values()
{
    return (yl_heapval*)( this + 1 );
}

inline size_t yl_program::opcount()
{
    return _opcount;
}

inline const yl_opinst* yl_program::ops()
{
    return (yl_opinst*)( values() + valcount() );
}

inline size_t yl_program::ilcount()
{
    return _ilcount;
}

inline yl_ilcache* yl_program::ilcache()
{
    return (yl_ilcache*)( ops() + opcount() );
}



#endif


