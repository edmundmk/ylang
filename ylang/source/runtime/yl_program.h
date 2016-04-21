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
    
    const char*             name() const;
    void                    print() const;
    const yl_debuginfo*     debuginfo() const;
    
    unsigned                upcount() const;
    unsigned                paramcount() const;
    bool                    varargs() const;
    bool                    coroutine() const;

    unsigned                stackcount() const;
    unsigned                locupcount() const;
    unsigned                iterscount() const;
    
    size_t                  valcount() const;
    const yl_heapval*       values() const;
    
    size_t                  opcount() const;
    const yl_opinst*        ops() const;
    
    size_t                  ilcount() const;
    yl_ilcache*             ilcache();
    
    size_t                  xfcount() const;
    const yl_xframe*        xframes() const;
    

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



inline unsigned yl_program::paramcount() const
{
    return _paramcount;
}

inline bool yl_program::varargs() const
{
    return _varargs;
}

inline bool yl_program::coroutine() const
{
    return _coroutine;
}

inline unsigned yl_program::upcount() const
{
    return _upcount;
}

inline unsigned yl_program::stackcount() const
{
    return _stackcount;
}

inline unsigned yl_program::locupcount() const
{
    return _locupcount;
}

inline unsigned yl_program::iterscount() const
{
    return _iterscount;
}

inline size_t yl_program::valcount() const
{
    return _valcount;
}

inline const yl_heapval* yl_program::values() const
{
    return (yl_heapval*)( this + 1 );
}

inline size_t yl_program::opcount() const
{
    return _opcount;
}

inline const yl_opinst* yl_program::ops() const
{
    return (yl_opinst*)( values() + valcount() );
}

inline size_t yl_program::ilcount() const
{
    return _ilcount;
}

inline yl_ilcache* yl_program::ilcache()
{
    return (yl_ilcache*)( ops() + opcount() );
}

inline size_t yl_program::xfcount() const
{
    return _xfcount;
}

inline const yl_xframe* yl_program::xframes() const
{
    return (yl_xframe*)( (yl_ilcache*)( ops() + opcount() ) + ilcount() );
}


#endif


