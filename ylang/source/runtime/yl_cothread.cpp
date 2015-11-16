//
//  yl_cothread.cpp
//
//  Created by Edmund Kapusniak on 12/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_cothread.h"
#include "ylang.h"
#include "yl_interp.h"
#include "yl_array.h"
#include "yl_table.h"
#include "yl_upval.h"
#include "yl_function.h"
#include "yl_program.h"




yl_gctype yl_cothread::gctype =
{
    YLOBJ_COTHREAD,
    YL_GCFLAG_NONE,
    "cothread",
    &yl_cothread::destroy,
    &yl_cothread::mark,
    &yl_cothread::eager_mark,
};


yl_cothread::yl_cothread()
{
}


void yl_cothread::destroy( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    self->~yl_cothread();
}

void yl_cothread::mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    
    for ( const auto& frame : self->_frames )
    {
        heap->mark( frame.funcobj );
    }
    
    for ( const auto& value : self->_stack )
    {
        if ( value.is_gcobject() )
        {
            heap->mark( value.gcobject() );
        }
    }
    
    for ( const auto& upval : self->_locup )
    {
        heap->mark( upval );
    }
    
    for ( const auto& iterator : self->_iters )
    {
        iterator.mark( heap );
    }
}

void yl_cothread::eager_mark( yl_gcheap* heap, yl_gcobject* object )
{
    yl_cothread* self = (yl_cothread*)object;
    
    for ( const auto& frame : self->_frames )
    {
        heap->eager_mark( frame.funcobj );
    }
    
    for ( const auto& value : self->_stack )
    {
        if ( value.is_gcobject() )
        {
            heap->eager_mark( value.gcobject() );
        }
    }
    
    for ( const auto& upval : self->_locup )
    {
        heap->eager_mark( upval );
    }
    
    for ( const auto& iterator : self->_iters )
    {
        iterator.eager_mark( heap );
    }
}



void yl_cothread::push_frame( const yl_stackframe& frame )
{
    _frames.push_back( frame );
}

yl_stackframe yl_cothread::pop_frame()
{
    yl_stackframe frame = _frames.back();
    _frames.pop_back();
    return frame;
}


void yl_cothread::close_locup( size_t base, unsigned index )
{
    for ( size_t i = _locup.size(); i-- > base + index; )
    {
        yl_upval*& upval = _locup.at( i );
        if ( upval )
        {
            upval->close( this );
        }
    }
    _locup.resize( base + index );
}

void yl_cothread::close_iterator( size_t base, unsigned index )
{
    for ( size_t i = _iters.size(); i-- > base + index; )
    {
        yl_iterator& iterator = _iters.at( i );
        iterator.close();
    }
    _iters.resize( base + index );
}


unsigned yl_cothread::get_locup_base() const
{
    if ( _frames.size() )
    {
        const yl_stackframe& frame = _frames.back();
        return frame.locup_base + frame.funcobj->program()->locupcount();
    }
    return 0;
}

unsigned yl_cothread::get_iters_base() const
{
    if ( _frames.size() )
    {
        const yl_stackframe& frame = _frames.back();
        return frame.iters_base + frame.funcobj->program()->iterscount();
    }
    return 0;
}



