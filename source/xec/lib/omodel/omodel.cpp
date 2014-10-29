//
//  omodel.cpp
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "omodel.h"
#include "omodel_impl.h"
#include "ovalue.h"
#include "oexpand.h"



/*
    Both the mutator and the marker threads are marking objects with the
    current mark colour.  Newly allocated objects are born already marked.
    
    The mutator's write barrier acts on unmarked objects when an object is
    modified.  The object containing the slot is marked in its entirety.  This
    means we always mark objects using their state at the GC snapshot (since
    objects may only be modified after they are marked).  This causes mutators
    to stall, but is preferable to long pauses.
    
    The marker thread works in the background, marking roots and following
    references from newly marked objects.
    
    
    The mark colours green/orange/purple are cycled through.  One colour is the
    current mark colour.  The previous colour is objects reachable at the last
    snapshot but not yet considered in this cycle: the unmarked colour.
    Objects of the final colour are unreachable (they were not reachable at
    the last snapshot time) that have not yet been swept.
 
    Marking state transitions work like this:
    
        unmarked -> marked   : for objects which have no children.
        unmarked -> pending  : when objects are added to the work list.
        
        pending  -> marking  : either the mutator or collector can mark.
        marking  -> marked   : uncontended mark completion.
 
        marking  -> **locked : if a mutator encounters a marking object.
        **locked -> marked   : accompanied by a broadcast on the condvar.
    
    Colour updates are atomic, but transitions in and out of the locked state
    are additionally restricted - they must only be performed with the marking
    mutex held.  This ensures that mutator threads cannot miss the mark
    completion broadcast.

*/


namespace oimpl
{

__thread ocontext* context;













}



ocontext::ocontext( omodel* model )
{
    assert( oimpl::context == nullptr );
    oimpl::context = model->p->new_context();
}

ocontext::~ocontext()
{
    oimpl::context->model->delete_context( oimpl::context );
}

void ocollect()
{
    oimpl::context->model->collect();
}

void osafepoint()
{
    oimpl::context->model->safepoint();
}

void ogcbase::write_barrier_real( oimpl::ocolour object_colour )
{
    oimpl::context->model->barrier( oimpl::context, this, object_colour );
}










