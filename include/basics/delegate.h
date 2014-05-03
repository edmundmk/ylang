//
//  delegate.h
//
//  Created by Edmund Kapusniak on 03/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef DELEGATE_H
#define DELEGATE_H


class anyobject_t;


template < typename return_t, typename ... arguments_t >
class delegate_t
{
public:

    typedef return_t (*function_t)( arguments_t ... );
    typedef return_t (anyobject_t::*method_t)( arguments_t ... );

    delegate_t();
    delegate_t( function_t function );
    delegate_t( anyobject_t* object, method_t method );
    
    return_t operator () ( arguments_t ... arguments ) const;
    
private:

    anyobject_t*    object;
    union
    {
        method_t    method;
        function_t  function;
    };

};



template < typename return_t, typename ... arguments_t >
inline delegate_t< return_t, arguments_t ... > delegate(
                            return_t (*function)( arguments_t ... ) )
{
    typedef ::delegate_t< return_t, arguments_t ... > delegate_t;
    return delegate_t( function );
}



template < typename object_t, typename return_t, typename ... arguments_t >
inline delegate_t< return_t, arguments_t ... > delegate(
        object_t* object, return_t (object_t::*method)( arguments_t ... ) )
{
    typedef ::delegate_t< return_t, arguments_t ... > delegate_t;
    typedef typename delegate_t::method_t method_t;
    return delegate_t( (anyobject_t*)object, (method_t)method );
}



template < typename return_t, typename ... arguments_t >
inline delegate_t< return_t, arguments_t ... >::delegate_t()
    :   object( nullptr )
    ,   function( nullptr )
{
}


template < typename return_t, typename ... arguments_t >
inline delegate_t< return_t, arguments_t ... >::
                    delegate_t( function_t function )
    :   object( nullptr )
    ,   function( function )
{
}


template < typename return_t, typename ... arguments_t >
inline delegate_t< return_t, arguments_t ... >::
                    delegate_t( anyobject_t* object, method_t method )
    :   object( object )
    ,   method( method )
{
}


template < typename return_t, typename ... arguments_t >
inline return_t delegate_t< return_t, arguments_t ... >::
                    operator () ( arguments_t ... arguments ) const
{
    if ( object )
        return ( object->*method )( arguments ... );
    else
        return function( arguments ... );
}




#endif
