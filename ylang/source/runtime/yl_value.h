//
//  yl_value.h
//
//  Created by Edmund Kapusniak on 02/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_VALUE_H
#define YL_VALUE_H


/*
    A yl_value is pointer-sized and can hold any ylang type.  That means a
    heapobj reference, a number, null, true, false, or undef.  Accesses MUST
    be atomic to allow the garbage collector to run concurrently.
    

    On 32-bit:
    
        There is not enough space to store a double.  Instead numbers are
        boxed.  Boxed numbers are not heapobjs and are not garbage collected.
        Instead they are owned by the yl_value.
 
        We don't bother with the 'small integer' packing that some VMs do,
        because values are stored unpacked on the stack and checking if a
        number is an integer whenever it's stored is a pain.  We can make
        allocation of boxed numbers very fast using a pool.

        A value with the low bit clear points to a heapobj pointer.  A value
        with the low bit set points to a boxed number.


*/


class yl_value
{
public:

    yl_value();
    ~yl_value();


    bool        is_number() const;

    void        set( double n );
    void        set( yl_heapobj* o );

    double      get_number() const;
    yl_heapobj* get_heapobj() const;


private:

    union
    {
        std::atomic< uintptr_t >    v;
        yl_heapobj*                 p;
    };

};




/*
    Values on the stack are marked while the mutator is suspended and
    therefore do not need to be atomic.

*/





#endif
