//
//  yl_context.h
//
//  Created by Edmund Kapusniak on 28/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_CONTEXT_H
#define YL_CONTEXT_H


#include <memory>


class yl_context;
class yl_scope;
class yl_invoke;
class yl_heap;


/*
    A yl_context allows execution of ylang script code.  Each context is
    isolated from other contexts.  Execution is single-threaded.
    
*/


class yl_context
{
public:

    yl_context();
    ~yl_context();


private:

    friend class yl_scope;

    std::unique_ptr< yl_heap > _heap;

};



/*
    Each thread has a current context.

*/


class yl_scope
{
public:

    explicit yl_scope( yl_context* context );
    ~yl_scope();


private:

    yl_heap* _heap;
    yl_heap* _prev;

};





#endif
