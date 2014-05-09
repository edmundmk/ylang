//
//  xec_scope.h
//
//  Created by Edmund Kapusniak on 05/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//



#ifndef XEC_SCOPE_H
#define XEC_SCOPE_H


#include <unordered_map>
#include <deque>
#include <memory>
#include <string>


class xec_scope;
class xec_constructor_function;


enum xec_scope_kind
{
    XEC_SCOPE_GLOBAL,
    XEC_SCOPE_IMPLIED,
    XEC_SCOPE_FUNCTION,
    XEC_SCOPE_OBJECT,
    XEC_SCOPE_BLOCK,
    XEC_SCOPE_LOOP,
    XEC_SCOPE_SWITCH,
};



/*
    A scope, containing a set of names.  Function scopes also contain a setting
    indicating if the function was declares with varargs, and a set of names
    in outer scopes that need importing as upvals.
*/

class xec_scope
{
public:

    xec_scope();
    xec_scope* make_child_scope( xec_scope_kind kind );

    void set_function( xec_constructor_function* function );

    xec_scope_kind get_kind();
    xec_scope* get_outer();
    xec_constructor_function* get_function();

    
private:

    xec_scope_kind kind;
    xec_scope* outer_scope;
    std::deque< std::unique_ptr< xec_scope > > children;
    xec_constructor_function* function;

};





#endif
