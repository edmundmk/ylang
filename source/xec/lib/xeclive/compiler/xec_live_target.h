//
//  xec_live_target.h
//
//  Created by Edmund Kapusniak on 04/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_LIVE_TARGET_H
#define XEC_LIVE_TARGET_H


#include <list>
#include <unordered_map>
#include "xec_seq.h"



struct xec_live_opformat
{
    xec_live_opformat( int precedence, const char* format );

    int             precedence;
    const char*     format;
};



class xec_live_target
{
public:

    xec_live_target();
    virtual ~xec_live_target();
    
    
    void expression( xec_seq* seq );
    
    
protected:

    void opformat( xec_seq_opcode opcode, int precedence, const char* format );


private:

    void realize( xec_seq_op* op, int precedence );

    std::unordered_map< int, xec_live_opformat > opformats;
    

};



#endif
