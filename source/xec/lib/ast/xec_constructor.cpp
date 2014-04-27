//
//  xec_constructor.cpp
//
//  Created by Edmund Kapusniak on 27/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_constructor.h"






xec_constructor_new::xec_constructor_new(
        xec_token* token, xec_expression* type, xec_expression_list* args )
{
}




xec_constructor_list::xec_constructor_list()
{
}

void xec_constructor_list::append_value( xec_expression* value )
{
}

void xec_constructor_list::append_final( xec_expression* final )
{
}




xec_constructor_table::xec_constructor_table()
{
}
    
void xec_constructor_table::append_keyval(
                xec_expression* key, xec_expression* value )
{
}




//xec_constructor_object



xec_constructor_function::xec_constructor_function( xec_token* token,
                xec_expression_list* params, xec_statement_compound* body )
{
}

void xec_constructor_function::set_thiscall( bool thiscall )
{
}

void xec_constructor_function::set_coroutine( bool coroutine )
{
}

