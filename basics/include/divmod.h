//
//  divmod.h
//
//  Created by Edmund Kapusniak on 03/12/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef DIVMOD_H
#define DIVMOD_H


#include <math.h>


/*
    Floored (integer) division.
*/

inline float intdiv( float a, float b )
{
    return floor( a / b );
}

inline double intdiv( double a, double b )
{
    return floor( a / b );
}


/*
    Euclidean modulo where the remainder has same sign as divisor.
*/

inline float emod( float a, float b )
{
    float v = fmod( a, b );
    if ( ( b < 0 ) != ( v < 0 ) )
    {
        v += b;
    }
    return v;
}

inline double emod( double a, double b )
{
    double v = fmod( a, b );
    if ( ( b < 0 ) != ( v < 0 ) )
    {
        v += b;
    }
    return v;
}


#endif
