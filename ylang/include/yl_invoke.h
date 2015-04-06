//
//  yl_invoke.h
//
//  Created by Edmund Kapusniak on 28/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_INVOKE_H
#define YL_INVOKE_H


#include <cstddef>
#include <utility>


class yl_function;
struct yssa_module;


/*
    A yl_invoke is a C++ handle to a ylang function or callback.

*/


class yl_invoke
{
public:

    yl_invoke();
    yl_invoke( std::nullptr_t );
    yl_invoke& operator = ( std::nullptr_t );
    yl_invoke( const yl_invoke& p );
    yl_invoke& operator = ( const yl_invoke& p );
    yl_invoke( yl_invoke&& p );
    yl_invoke& operator = ( yl_invoke&& p );
    ~yl_invoke();
    
    explicit operator bool () const;
    

private:

    friend yl_invoke yssa_codegen( yssa_module* module );
    
    explicit yl_invoke( yl_function* function );

    void acquire();
    void release();

    yl_function* _function;

};


/*
*/


inline yl_invoke::yl_invoke( yl_function* function )
    :   _function( function )
{
    acquire();
}

inline yl_invoke::yl_invoke()
    :   _function( nullptr )
{
}

inline yl_invoke::yl_invoke( std::nullptr_t )
    :   _function( nullptr )
{
}

inline yl_invoke& yl_invoke::operator = ( std::nullptr_t )
{
    release();
    return *this;
}

inline yl_invoke::yl_invoke( const yl_invoke& p )
    :   _function( p._function )
{
    acquire();
}

inline yl_invoke& yl_invoke::operator = ( const yl_invoke& p )
{
    release();
    _function = p._function;
    acquire();
    return *this;
}

inline yl_invoke::yl_invoke( yl_invoke&& p )
{
    _function = p._function;
    p._function = nullptr;
}

inline yl_invoke& yl_invoke::operator = ( yl_invoke&& p )
{
    std::swap( _function, p._function );
    p.release();
    return *this;
}

inline yl_invoke::~yl_invoke()
{
    release();
}

inline yl_invoke::operator bool () const
{
    return _function != nullptr;
}


#endif




