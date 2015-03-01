//
//  yl_context.h
//
//  Created by Edmund Kapusniak on 28/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_CONTEXT_H
#define YL_CONTEXT_H



class yl_invoke;



/*
    A yl_context allows execution of ylang script code.  Each context is
    isolated from other contexts.  Execution is single-threaded.
    
*/


class yl_context
{
public:

    yl_invoke compile( const char* path );



private:

    

};



/*
    Each thread has a current context.

*/


class yl_scope
{
public:

    explicit yl_context( yl_context* context );
    ~yl_context();

};





#endif
