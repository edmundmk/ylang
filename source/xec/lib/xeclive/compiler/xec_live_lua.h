//
//  xec_live_lua.h
//
//  Created by Edmund Kapusniak on 04/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_LIVE_LUA_H
#define XEC_LIVE_LUA_H



class xec_live_lua
{
public:

    xec_live_lua();
    virtual ~xec_live_lua();



private:

    enum
    {
        BASIC,
        EXP,
        UNARY,
        MULTIPLY,
        ADD,
        CONCAT,
        COMPARE,
        AND,
        OR
    };

};





#endif


