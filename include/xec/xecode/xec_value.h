//
//  xec_value.h
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_VALUE_H
#define XEC_VALUE_H



/*
    We NaN-box values into doubles like this:

    float     seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    infinity  s111111111110000000000000000000000000000000000000000000000000000
    qnan      s111111111111000000000000000000000000000000000000000000000000000
    snan      s111111111110zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    null      s111111111110100
    bool      s111111111110010
    string    s111111111110110
    object                    pppppppppppppppppppppppppppppppppppppppppppppppp
 
*/


/*
    IEEE 754 doubles are (with nan values on x86-64):
    
            sEEEEEEEEEEEmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
    inf     s111111111110000000000000000000000000000000000000000000000000000
    qnan    0111111111111000000000000000000000000000000000000000000000000000
    snan    0111111111110100000000000000000000000000000000000000000000000000
    nan     x11111111111Qxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            ----------------pppppppppppppppppppppppppppppppppppppppppppppppp
 
    and IEEE 754 floats are:
    
            sEEEEEEEEmmmmmmmmmmmmmmmmmmmmmmm
    inf     s1111111100000000000000000000000
    qnan    01111111110000000000000000000000
    snan    01111111101000000000000000000000
    nan     x11111111Qxxxxxxxxxxxxxxxxxxxxxx
 
    xec has the following types:
    
    floating-point number
    integer
    boolean
    null
    undefined
    object
    table
    array
    string
    upval
    function
    native this
    native method
    native field

    float
            (inf)       000
            (qnan)      100
            (snan)      010
                        110
    short-integer       001 (20-bits)
    boolean/null/und    101
    heap-object         011
                        111
 
 
 

*/



class xec_value
{
public:

    xec_value();
    xec_value( bool v );
    xec_value( int64_t v );
    xec_value( double v );
    


private:

    union
    {
        double      f;
        uintptr_t   i;
    };

};



#endif
