//
//  omodel.h
//
//  Created by Edmund Kapusniak on 25/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OMODEL_H
#define OMODEL_H


#include <stdint.h>


class oexpand;
class ostring;




/*
    Base class for all objects in the omodel.  Stores a word used for garbage
    collection data.
*/


class obase
{
    uintptr_t   gcword;
};





/*
    A concurrent garbage collector based on this paper:
 
        http://www.cs.bell-labs.com/who/lorenz/papers/ismm98.pdf
 
    The mutator thread is only paused to mark roots, and otherwise the
    collector runs concurrently.  There are two key insights.  The first is
    that we can find all objects reachable at a snapshot time by having
    mutators mark the old values of updated slots.
    
    The second is that the mark of the next epoch can run concurrently with
    the sweep of the current one - the sweeper only sweeps objects dead in
    the current epoch, and the marker marks only those live in the next.  The
    two sets cannot overlap as objects stay dead.
    
*/


typedef unsigned ocolour;


class omodel
{
public:


private:

    ostring*    strings;
    oexpand*    expands;


};






/*
    A thread must open an ocontext before it can create and manipulate omodel.
*/


class ocontext
{


};





#endif
