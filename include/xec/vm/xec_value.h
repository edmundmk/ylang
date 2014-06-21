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
