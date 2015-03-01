//
//  yl_expose.h
//
//  Created by Edmund Kapusniak on 28/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_EXPOSE_H
#define YL_EXPOSE_H



/*
    Attributes marking functions or methods exposed to ylang (ylfunc), or
    entry points or callbacks to ylang (ylcall).

*/


#ifdef __YMETAC__

#define ylfunc  __attribute__(( annotate( "ylfunc"  ) ))
#define ylcall  __attribute__(( annotate( "ylcall"  ) ))

#else

#define ylfunc
#define ylcall

#endif




/*
    yl_expose is the base class of native objects exposed to ylang.

*/


class yl_expose
{
public:

    yl_expose();
    ~yl_expose();
    
    
private:



};



#endif


