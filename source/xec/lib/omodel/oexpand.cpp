//
//  oexpand.cpp
//
//  Created by Edmund Kapusniak on 29/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "oexpand.h"






/*
#include "omodel_impl.h"


oimpl::ogctype oexpand::gctype =
{
    nullptr,
    oexpand::mark,
    oexpand::free,
    "object"
};



oexpand* oexpand::alloc( oexpand* prototype )
{
    oimpl::omodel* model = oimpl::context->model;
    oexpand* e = model->alloc< oexpand >( oimpl::context, &gctype );
    if ( prototype == nullptr )
    {
        e->expand = nullptr;
    }
    else
    {
        // TODO!
    }
    return e;
}


void oexpand::expand_key( osymbol key, ovalue value )
{
    oimpl::context->model->expand_key( oimpl::context, this, key, value );
}


void oexpand::mark( ogcbase* object, oimpl::oworklist* work,
                        oimpl::ocolour marked, oimpl::ocolour unmarked )
{
    oexpand* expand = (oexpand*)object;
    oexpanddata* expdata = expand->expand;
    if ( expdata )
    {
        expdata->expandclass->gcmark( work, unmarked );
        for ( size_t i = 0; i < expdata->expandclass->size; ++i )
        {
            ovalue value = expdata->properties[ i ];
            if ( value.is_string() )
                value.as_string_unchecked()->gcmark( marked );
            else if ( value.is_expand() )
                value.as_expand_unchecked()->gcmark( unmarked );
        }
    }
}

void oexpand::free( ogcbase* object )
{
    oexpand* expand = (oexpand*)object;
    expand->~oexpand();
}






oimpl::ogctype oexpandclass::gctype =
{
    nullptr,
    oexpandclass::mark,
    oexpandclass::free,
    "expandclass"
};


void oexpandclass::mark( ogcbase* object, oimpl::oworklist* work,
                        oimpl::ocolour marked, oimpl::ocolour unmarked )
{
    oexpandclass* expclass = (oexpandclass*)object;

    expclass->prototype->gcmark( work, unmarked );
    for ( auto i = expclass->lookup.begin();
                    i != expclass->lookup.end(); ++i )
    {
        i->key.get()->gcmark( marked );
    }
    
    expclass->parent->gcmark( work, unmarked );
    expclass->parent_key.get()->gcmark( marked );
    for ( auto i = expclass->children.begin();
                    i != expclass->children.end(); ++i )
    {
        i->key.get()->gcmark( marked );
        i->value->gcmark( work, unmarked );
    }
}

void oexpandclass::free( ogcbase* object )
{
    oexpandclass* expclass = (oexpandclass*)object;
    expclass->~oexpandclass();
}



*/









