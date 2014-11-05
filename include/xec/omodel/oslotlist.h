//
//  oslotlist.h
//
//  Created by Edmund Kapusniak on 04/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OSLOTS_H
#define OSLOTS_H


#include <atomic>
#include "obase.h"


class ovalue;



/*
    On 32-bit systems 64-bit atomic operations are slow.  However all reference
    updates must be atomic to support concurrent gabarge collection.  With the
    NaN-boxed ovalue representation, it is impossible to differentiate a 32-bit
    pointer from the low bits of a double without looking at all 64 bits.
    This is a big problem.
    
    So when we store values into oexpands, we must differentiate between
    references (whose low bits contain pointers) and numbers (which are not
    interesting to the garbage collector).  We use the class system to infer
    the types of properties, sorting them into 'numbers' and 'references'.
    References include special values such as null or true (these have
    representations with the low bits all set, which allows differentiation
    from pointers).
 
    Object properties are stored in slot lists - references are allocated
    slots at the top of the list, growing down, and numbers are allocated
    slots at the bottom of the list, growing up.
    
    If you store a reference into a property identified as a number, or vice
    versa, then the class must expand.  The resulting property in the new
    class becomes a 'dual' property, with both a reference and a number slot.
    To keep property accesses as fast as possible, the main slot of the
    property is the number slot.  When storing a reference, the same value
    is stored into both slots.  The mutator can read it back from the 'number'
    slot, and the mark thread will read the reference part from the reference
    slot.  When storing a number, the value is stored into the number part
    and the reference part is set to undefined.
 
*/


/*
    An oslotlist is a set of slots containing its own write-barrier logic.
    There are size slots, but only slots from 0 to watermark are considered
    by the garbage collector.  The size of the slotlist is fixed (if you need
    more slots, allocate another list).
    
    The watermark always increases and can never decrease.  As the watermark
    rises it should never rise above the lowest slot used for a number.  These
    restrictions must be adhered to in order that we always present a valid
    slot list to the mark thread.
*/


class oslotlist : public obase
{
public:

    static ometatype metatype;

    static oslotlist* alloc( size_t size );
    
    size_t  size() const;
    size_t  watermark() const;
    void    set_watermark( size_t watermark );
    ovalue  load( size_t index ) const;
    void    store( size_t index, ovalue value );


protected:

    static void mark_slotlist( oworklist* work, obase* object, ocolour colour );

    oslotlist( ometatype* type, size_t size );


private:

#if OVALUE32
    static const uint32_t UNDEFINED = UINT32_C( 0xFFFFFFFF );
#endif

    struct slot
    {
        slot();
    
#if OVALUE64
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
    A slot index tells us which slots a property inhabits.  Properties are
    always read from their primary 'slot'.  'dual' is 0 for properties of
    reference type, and 1 for properties with number type.  Higher values
    are the index of the reference slot for a dual property, + 2.
*/

struct oslotindex
{
    static const size_t INVALID     = (size_t)-1;
    static const size_t REFERENCE   = 0;
    static const size_t NUMBER      = 1;
    static const size_t DUALSLOT    = 2;

    oslotindex();
    oslotindex( size_t slot, size_t dual );

    size_t  slot;
    size_t  dual;
};




/*

*/

#include "ovalue.h"


inline size_t oslotlist::size() const
{
    return ssize;
}

inline size_t oslotlist::watermark() const
{
    return smark.load( std::memory_order_relaxed );
}

inline void oslotlist::set_watermark( size_t watermark )
{
    smark.store( (uint32_t)watermark, std::memory_order_relaxed );
}

inline ovalue oslotlist::load( size_t index ) const
{
#if OVALUE64
    return ovalue( slots[ index ].x.load( std::memory_order_relaxed ) );
#else
    uint32_t lo = slots[ index ].lo.load( std::memory_order_relaxed );
    uint32_t hi = slots[ index ].hi;
    return ovalue( lo, hi );
#endif
}

inline void oslotlist::store( size_t index, ovalue value )
{
#if OVALUE64

    // Do write barrier on slots below the watermark.
    if ( index < smark )
    {
        uint64_t x = slots[ index ].x.load( std::memory_order_relaxed );
        if ( x <= ovalue::MAX_REFERENCE )
        {
            obase* p = (obase*)( x & ovalue::POINTER_MASK );
            p->mark();
        }
    }

    // Update slot.
    slots[ index ].x.store( value.x, std::memory_order_relaxed );

#else

    // Do write barrier on slots below the watermark.
    if ( index < smark )
    {
        uint32_t lo = slots[ index ].lo.load( std::memory_order_relaxed );
        if ( lo != UNDEFINED )
        {
            obase* p = (obase*)lo;
            p->mark();
        }
    }
    
    // Update slot.
    slots[ index ].lo.store( value.lo, std::memory_order_relaxed );
    slots[ index ].hi = value.hi;
    
#endif
}


#if OVALUE64

inline oslotlist::slot::slot()
    :   x( ovalue::VALUE_UNDEFINED )
{
}

#else

inline oslotlist::slot::slot()
    :   lo( UNDEFINED )
    ,   hi( UNDEFINED )
{
}

#endif


inline oslotindex::oslotindex()
    :   slot( -1 )
    ,   dual( -1 )
{
}

inline oslotindex::oslotindex( size_t slot, size_t dual )
    :   slot( slot )
    ,   dual( dual )
{
}


#endif
