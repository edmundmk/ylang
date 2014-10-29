//
//  oexpand.cpp
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oexpand.h"
#include "omodel_impl.h"


// HMMMMM.


static void oexpand_mark( seglist< ogcbase* >* work );
oimpl::ogctype oexpand::gctype = { nullptr, oexpand_mark, "object" };



oexpand* oexpand::alloc()
{
    oexpand* e = (oexpand*)oimpl::context->model->alloc(
                    oimpl::context, &gctype, sizeof( oexpand ) );
    e->expand = nullptr;
    return e;
}










static void oexpand_mark( seglist< ogcbase* >* work )
{

    // TODO.

}






