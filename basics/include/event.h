//
//  event.h
//
//  Created by Edmund Kapusniak on 03/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef EMK_EVENT_H
#define EMK_EVENT_H


#include <forward_list>
#include "delegate.h"


template < typename ... arguments_t >
class event
{
public:

    typedef delegate< void( arguments_t ... ) > delegate_type;

    void subscribe( delegate_type delegate );
    template < typename target_t > void subscribe( target_t* target, void (target_t::*method)( arguments_t ... ) );
    template < typename target_t > void subscribe( const target_t* target, void (target_t::*method)( arguments_t ... ) const );
    void unsubscribe( delegate_type delegate );
    template < typename target_t > void unsubscribe( target_t* target, void (target_t::*method)( arguments_t ... ) );
    template < typename target_t > void unsubscribe( const target_t* target, void (target_t::*method)( arguments_t ... ) const );
    
    void operator () ( arguments_t ... arguments ) const;
    
    
private:

    std::forward_list< delegate_type > subscribers;

};




template < typename ... arguments_t >
void event< arguments_t ... >::subscribe( delegate_type delegate )
{
    subscribers.push_front( delegate );
}

template< typename ... arguments_t >
template< typename target_t >
void event< arguments_t ... >::subscribe( target_t* target, void (target_t::*method)( arguments_t ... ) )
{
    subscribe( bind( target, method ) );
}

template < typename ... arguments_t >
template < typename target_t >
void event< arguments_t ... >::subscribe( const target_t* target, void (target_t::*method)( arguments_t ... ) const )
{
    subscribe( bind( target, method ) );
}


template < typename ... arguments_t >
void event< arguments_t ... >::unsubscribe( delegate_type delegate )
{
    for ( auto previous = subscribers.before_begin(), i = subscribers.begin();
                i != subscribers.end(); previous = i++ )
    {
        if ( *i == delegate )
        {
            subscribers.erase_after( previous );
            break;
        }
    }
}

template< typename ... arguments_t >
template< typename target_t >
void event< arguments_t ... >::unsubscribe( target_t* target, void (target_t::*method)( arguments_t ... ) )
{
    unsubscribe( bind( target, method ) );
}

template < typename ... arguments_t >
template < typename target_t >
void event< arguments_t ... >::unsubscribe( const target_t* target, void (target_t::*method)( arguments_t ... ) const )
{
    unsubscribe( bind( target, method ) );
}


template < typename ... arguments_t >
inline void event< arguments_t ... >::
                operator() ( arguments_t ... arguments ) const
{
    for ( auto i = subscribers.begin(); i != subscribers.end(); )
    {
        const delegate_type& subscriber = *i;
        i++;
        subscriber( arguments ... );
    }
}



#endif

