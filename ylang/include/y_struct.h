//
//  y_struct.h
//
//  Created by Edmund Kapusniak on 05/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_STRUCT_H
#define Y_STRUCT_H


#include <utility>
#include "y_context.h"



/*
    A managed value type.  Usually used on the heap as a member of a y_object,
    but can also be instantiated on the stack using ylocal<>.
*/

struct y_struct
{
};



/*
    A managed value type on the stack.  Locals may contain managed references
    and so must be tracked.
*/

template < typename struct_t >
class ylocal : public struct_t
{
public:

    template < typename ... arguments_t >
        ylocal( arguments_t&& ... arguments );
    using struct_t::operator = ;
    ~ylocal();
    
};



/*
    y_struct layout information is in these two functions.
    y_structs don't necessarily have a vtable and so layout information must
    be provided statically using a specialisation of this template.
*/

template < typename struct_t > void ylocal_push( y_context* context );
template < typename struct_t > void ylocal_pop( y_context* context );




/*
*/


template < typename struct_t >
template < typename ... arguments_t >
ylocal< struct_t >::ylocal( arguments_t&& ... arguments )
    :   struct_t( std::forward< arguments_t >( arguments ) ... )
{
    ylocal_push< struct_t >( y_context::current );
}

template < typename struct_t >
ylocal< struct_t >::~ylocal()
{
    ylocal_pop< struct_t >( y_context::current );
}



#endif
