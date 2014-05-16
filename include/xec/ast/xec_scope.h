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
class xec_name;

class xec_constructor_function;
class xec_declaration_prototype;


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


enum xec_name_kind
{
    XEC_NAME_GLOBAL,
    XEC_NAME_THIS,
    XEC_NAME_VARIABLE,
    XEC_NAME_PARAMETER,
    XEC_NAME_PROTOTYPE,
};



/*
    A scope, containing a set of names.
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
    
    xec_scope*  declare_scope( xec_scope_kind kind, const char* name );
    xec_scope*  lookup_scope( const char* name );
    xec_name*   declare_name( xec_name_kind kind, const char* name );
    xec_name*   lookup_name( const char* name );


    
private:

    xec_scope_kind kind;
    xec_scope* outer_scope;
    std::deque< std::unique_ptr< xec_scope > > children;
    xec_constructor_function* function;

    std::unordered_map< std::string, xec_scope* > scopes;
    std::unordered_map< std::string, std::unique_ptr< xec_name > > names;
    
};



/*
    Names are the things that we are declaring in scopes and looking up.
*/

class xec_name
{
public:

    xec_name( xec_scope* scope, xec_name_kind kind, const char* name );

    void set_prototype( xec_declaration_prototype* prototype );

    xec_scope* get_scope();
    xec_name_kind get_kind();
    const char* get_name();
    xec_declaration_prototype* get_prototype();


private:

    xec_scope*                  scope;
    xec_name_kind               kind;
    const char*                 name;
    xec_declaration_prototype*  prototype;
    
};




#endif







