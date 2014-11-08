//
//  yslots.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YSLOTS_H
#define YSLOTS_H


#include "yobject.h"


class yvalue;


/*
    A yslots is a list of yvalues containing its own write-barrier logic.
    There are size slots, but only slots from 0 to watermark are considered
    by the garbage collector.  The size of the slotlist is fixed (if you need
    more slots, allocate another list).
    
    The watermark always increases and can never decrease.  As the watermark
    rises it should never rise above the lowest slot used for a number.  These
    restrictions must be adhered to in order that we always present a valid
    slot list to the mark thread.

    A set of slots differs from a tuple, because on systems without fast
    64-bit atomic accesses, we cannot store a yvalue in one atomic operation.
    
    Users of yslots must differentiate between yvalues which are references
    (which are stored below the watermark, allocated from the top down,
    visible to the garbage collector), and yvalues which are numbers (stored
    above the watermark, allocated from the bottom up).
*/


class yslots : public yobject
{
public:

    static yslots* alloc( size_t size );
    
    size_t  size() const;
    size_t  watermark() const;
    void    set_watermark( size_t watermark );
    yvalue  get( size_t i ) const;
    void    set( size_t i, yvalue value );


protected:

    friend class yobject;
    friend class yvalue;
    static ymetatype metatype;
    static void mark_slots( yobject* object, yworklist* work, ycolour colour );

    yslots( ymetatype* type, size_t size );


private:

#if ! Y64BIT
    static const uint32_t UNDEFINED = UINT32_C( 0xFFFFFFFF );
#endif

    struct slot
    {
        slot();
    
#if Y64BIT
        std::atomic< uint64_t > x;
#else
        std::atomic< uint32_t > lo;
        uint32_t                hi;
#endif
    };
    
    uint32_t                ssize;
    std::atomic< uint32_t > smark;
    slot                    slots[];

};



/*
    A slot index tells us which slots a property inhabits, and are used when
    allocating slots in yexpands.  Properties are always read from their
    primary slot index.  dual is 0 for properties of reference type, and 1
    for properties with number type.  Higher values are the index of the
    reference slot for a dual property, + 2.
*/

struct yslotindex
{
    static const size_t INVALID     = (size_t)-1;
    static const size_t REFERENCE   = 0;
    static const size_t NUMBER      = 1;
    static const size_t DUALSLOT    = 2;

    yslotindex();
    yslotindex( size_t slot, size_t dual );

    size_t  slot;
    size_t  dual;
};




/*

*/

#include "yvalue.h"


inline size_t yslots::size() const
{
    return ssize;
}

inline size_t yslots::watermark() const
{
    return smark.load( std::memory_order_relaxed );
}

inline void yslots::set_watermark( size_t watermark )
{
    smark.store( (uint32_t)watermark, std::memory_order_relaxed );
}

inline yvalue yslots::get( size_t i ) const
{
#if Y64BIT
    return yvalue( slots[ i ].x.load( std::memory_order_relaxed ) );
#else
    uint32_t lo = slots[ i ].lo.load( std::memory_order_relaxed );
    uint32_t hi = slots[ i ].hi;
    return yvalue( lo, hi );
#endif
}

inline void yslots::set( size_t i, yvalue value )
{
#if Y64BIT

    // Do write barrier on slots below the watermark.
    if ( i < smark )
    {
        uint64_t x = slots[ i ].x.load( std::memory_order_relaxed );
        if ( x <= yvalue::MAX_REFERENCE )
        {
            yobject* p = (yobject*)( x & yvalue::POINTER_MASK );
            p->mark();
        }
    }

    // Update slot.
    slots[ i ].x.store( value.x, std::memory_order_relaxed );

#else

    // Do write barrier on slots below the watermark.
    if ( i < smark )
    {
        uint32_t lo = slots[ i ].lo.load( std::memory_order_relaxed );
        if ( lo != UNDEFINED )
        {
            yobject* p = (yobject*)lo;
            p->mark();
        }
    }
    
    // Update slot.
    slots[ i ].lo.store( value.lo, std::memory_order_relaxed );
    slots[ i ].hi = value.hi;
    
#endif
}


#if Y64BIT

inline yslots::slot::slot()
    :   x( yvalue::VALUE_UNDEFINED )
{
}

#else

inline yslots::slot::slot()
    :   lo( UNDEFINED )
    ,   hi( UNDEFINED )
{
}

#endif


inline yslotindex::yslotindex()
    :   slot( -1 )
    ,   dual( -1 )
{
}

inline yslotindex::yslotindex( size_t slot, size_t dual )
    :   slot( slot )
    ,   dual( dual )
{
}


#endif

