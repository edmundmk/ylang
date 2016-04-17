//
//  yl_upval.h
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_UPVAL_H
#define YL_UPVAL_H


#include "yl_gcheap.h"
#include "yl_value.h"
#include "yl_cothread.h"



/*
    An upval.
*/

class yl_upval : public yl_gcobject
{
public:

    static yl_gctype gctype;

    yl_upval( yl_cothread* cothread, unsigned index );
    
    void        close();

    yl_value    get_value() const;
    void        set_value( yl_value value );

private:

    enum
    {
        OPEN = YL_GCFLAG_USER1,
    };

    static void destroy( yl_gcheap* heap, yl_gcobject* object );
    static void mark( yl_gcheap* heap, yl_gcobject* object );

    yl_heapref< yl_cothread >   _cothread;
    unsigned                    _index;
    yl_heapval                  _value;

};



/*

*/


inline void yl_upval::close()
{
    if ( check_gcflags( OPEN ) )
    {
        _value.set( _cothread.get()->_stack.at( _index ) );
        clear_gcflags( OPEN );
    }
}

inline yl_value yl_upval::get_value() const
{
    if ( check_gcflags( OPEN ) )
    {
        return _cothread.get()->_stack.at( _index );
    }
    else
    {
        return _value.get();
    }
}

inline void yl_upval::set_value( yl_value value )
{
    if ( check_gcflags( OPEN ) )
    {
        _cothread.get()->_stack.at( _index ) = value;
    }
    else
    {
        _value.set( value );
    }
}




#endif
