//
//  y_object.h
//
//  Created by Edmund Kapusniak on 31/01/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_OBJECT_H
#define Y_OBJECT_H


#include <cstddef>
#include <type_traits>
#include "y_context.h"


class y_object;
template < typename object_t > class yref;
template < typename object_t > class yroot;
template < typename object_t > class yslot;
template < typename object_t > class yauto;



/*
    Base class for managed references.
*/

template < typename object_t >
class yref
{
public:

    explicit operator bool() const;
    object_t* operator -> () const;
    object_t* get() const;


protected:

    explicit yref( object_t* p );

    object_t* p;

};



/*
    A managed reference from unmanaged memory.
*/

template < typename object_t >
class yroot : public yref< object_t >
{
public:

    yroot();
    yroot( std::nullptr_t );
    yroot& operator = ( std::nullptr_t );
    template < typename derived_t > yroot( const yref< derived_t >& p );
    template < typename derived_t > yroot& operator = ( const yref< derived_t >& p );
    ~yroot();
    
};


/*
    A managed reference from the managed heap.
*/

template < typename object_t >
class yslot : public yref< object_t >
{
public:

    yslot();
    yslot( std::nullptr_t );
    yslot& operator = ( std::nullptr_t );
    template < typename derived_t > yslot( const yref< derived_t >& p );
    template < typename derived_t > yslot& operator = ( const yref< derived_t >& p );

};



/*
    A managed reference from the stack.
*/

template < typename object_t >
class yauto : public yref< object_t >
{
public:

    yauto();
    yauto( std::nullptr_t );
    yauto& operator = ( std::nullptr_t );
    template < typename derived_t > yauto( const yref< derived_t >& p );
    template < typename derived_t > yauto& operator = ( const yref< derived_t >& p );
    ~yauto();


protected:

    friend class y_object;
    template < typename fobject_t, typename ... arguments_t >
        friend yauto< fobject_t > ynew( arguments_t&& ... arguments );
    template < typename fobject_t, typename source_t >
        friend yauto< fobject_t > yas( const yref< source_t >& p );

    explicit yauto( object_t* p );

};



/*
    A managed object.
*/

class y_object
{
public:

    y_object();


protected:

    template < typename object_t, typename ... arguments_t >
    static yauto< object_t > alloc( size_t extra_size, arguments_t& ... arguments );

    virtual ~y_object();

};




/*
    Allocation.
*/

template < typename object_t, typename ... arguments_t >
    yauto< object_t > ynew( arguments_t&& ... arguments );



/*
    Downcasting.
*/

template < typename object_t, typename source_t >
    bool yis( const yref< source_t >& p );
template < typename object_t, typename source_t >
    yauto< object_t > yas( const yref< source_t >& p );




/*
    For y_objects, yslot< object_t >, otherwise plain object_t.
*/

template < typename object_t, bool is_y_object >
struct yslot_wrap_detail;

template < typename object_t >
struct yslot_wrap_detail< object_t, false >
{
    typedef object_t type;
};

template < typename object_t >
struct yslot_wrap_detail< object_t, true >
{
    typedef yslot< object_t > type;
};

template < typename object_t >
struct yslot_wrap
{
    typedef typename yslot_wrap_detail< object_t,
                std::is_base_of< y_object, object_t >::value >::type type;
};




/*
*/



template < typename object_t >
inline yref< object_t >::yref( object_t* p )
    :   p( p )
{
}

template < typename object_t >
inline yref< object_t >::operator bool () const
{
    return p != nullptr;
}

template < typename object_t >
inline object_t* yref< object_t >::operator -> () const
{
    return p;
}

template < typename object_t >
inline object_t* yref< object_t >::get() const
{
    return p;
}





template < typename object_t >
inline yroot< object_t >::yroot()
    :   yref< object_t >( nullptr )
{
}

template < typename object_t >
inline yroot< object_t >::yroot( std::nullptr_t )
    :   yref< object_t >( nullptr )
{
}

template < typename object_t >
inline yroot< object_t >& yroot< object_t >::operator = ( std::nullptr_t )
{
    y_context::current->del_root( this->p );
    this->p = nullptr;
    return *this;
}

template < typename object_t >
template < typename derived_t >
inline yroot< object_t >::yroot( const yref< derived_t >& p )
    :   yref< object_t >( p.get() )
{
    y_context::current->add_root( this->p );
}

template < typename object_t >
template < typename derived_t >
inline yroot< object_t >& yroot< object_t >::operator = ( const yref< derived_t >& p )
{
    y_context::current->del_root( this->p );
    this->p = p.get();
    y_context::current->add_root( this->p );
    return *this;
}

template < typename object_t >
inline yroot< object_t >::~yroot()
{
    y_context::current->del_root( this->p );
}




template < typename object_t >
yslot< object_t >::yslot()
    :   yref< object_t >( nullptr )
{
}

template < typename object_t >
yslot< object_t >::yslot( std::nullptr_t )
    :   yref< object_t >( nullptr )
{
}

template < typename object_t >
yslot< object_t >& yslot< object_t >::operator = ( std::nullptr_t )
{
    y_context::current->write_barrier( this->p );
    this->p = nullptr;
    return *this;
}

template < typename object_t >
template < typename derived_t >
yslot< object_t >::yslot( const yref< derived_t >& p )
    :   yref< object_t >( p.get() )
{
}

template < typename object_t >
template < typename derived_t >
yslot< object_t >& yslot< object_t >::operator = ( const yref< derived_t >& p )
{
    y_context::current->write_barrier( this->p );
    this->p = p.get();
    return *this;
}




template < typename object_t >
yauto< object_t >::yauto( object_t* p )
    :   yref< object_t >( p )
{
    y_context::current->add_auto( &this->p );
}

template < typename object_t >
yauto< object_t >::yauto()
    :   yref< object_t >( nullptr )
{
    y_context::current->add_auto( &this->p );
}

template < typename object_t >
yauto< object_t >::yauto( std::nullptr_t )
    :   yref< object_t >( nullptr )
{
    y_context::current->add_auto( &this->p );
}

template < typename object_t >
yauto< object_t >& yauto< object_t >::operator = ( std::nullptr_t )
{
    this->p = nullptr;
    return *this;
}

template < typename object_t >
template < typename derived_t >
yauto< object_t >::yauto( const yref< derived_t >& p )
    :   yref< object_t >( p.get() )
{
    y_context::current->add_auto( &this->p );
}

template < typename object_t >
template < typename derived_t >
yauto< object_t >& yauto< object_t >::operator = ( const yref< derived_t >& p )
{
    this->p = p.get();
    return *this;
}

template < typename object_t >
yauto< object_t >::~yauto()
{
    y_context::current->del_auto( &this->p );
}




inline y_object::y_object()
{
}

template < typename object_t, typename ... arguments_t >
inline yauto< object_t > y_object::alloc( size_t extra_size, arguments_t& ... arguments )
{
    void* q = y_context::current->alloc( sizeof( object_t ) + extra_size );
    object_t* p = new ( q ) object_t( std::forward< arguments_t >( arguments ) ... );
    return yauto< object_t >( p );
}





template < typename object_t, typename ... arguments_t >
inline yauto< object_t > ynew( arguments_t&& ... arguments )
{
    void* q = y_context::current->alloc( sizeof( object_t ) );
    object_t* p = new ( q ) object_t( std::forward< arguments_t >( arguments ) ... );
    return yauto< object_t >( p );
}

template < typename object_t, typename source_t >
inline bool yis( const yref< source_t >& p )
{
    return dynamic_cast< object_t* >( p.get() ) != nullptr;
}

template < typename object_t, typename source_t >
inline yauto< object_t > yas( const yref< source_t >& p )
{
    return yauto< object_t >( dynamic_cast< object_t* >( p.get() ) );
}




#endif



