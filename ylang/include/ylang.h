//
//  ylang.h
//
//  Created by Edmund Kapusniak on 13/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YLANG_H
#define YLANG_H


#include <memory>
#include <exception>
#include <string>
#include <vector>


class yl_exception;
class yl_context;
class yl_scope;
class yl_function;
class yl_expose;
class yl_callframe;


class yl_context_impl;
class yl_funcobj;




/*
    Exception thrown when there is an error in ylang code.
*/


class yl_exception : public std::exception
{
public:

    yl_exception( const char* format, ... );
    
    const char* what() const throw() override;
    
    
private:

    std::string message;

};







/*
    A yl_context allows execution of ylang script code.  Each context is
    isolated from other contexts.  Execution is single-threaded.
    
*/


class yl_context
{
public:

    yl_context();
    ~yl_context();


private:

    friend class yl_scope;

    std::unique_ptr< yl_context_impl > _impl;

};



/*
    Each thread has a current context.

*/


class yl_scope
{
public:

    explicit yl_scope( yl_context* context );
    ~yl_scope();


private:

    yl_context_impl* _context;
    yl_context_impl* _previous;

};



/*
    Compile a ylang script and return a callable function.
*/


template < typename ... arguments_t >
    yl_function yl_compile( const char* path, arguments_t ... arguments );

yl_function yl_compile( const char* path, size_t paramc, const char* paramv[] );




/*
    Native handle to a ylang function.
*/

class yl_function
{
public:

    yl_function();
    yl_function( std::nullptr_t );
    yl_function& operator = ( std::nullptr_t );
    yl_function( const yl_function& p );
    yl_function& operator = ( const yl_function& p );
    yl_function( yl_function&& p );
    yl_function& operator = ( yl_function&& p );
    ~yl_function();
    
    explicit operator bool () const;
    

private:

    friend class yl_funcobj;

    explicit yl_function( yl_funcobj* funcobj );

    void acquire();
    void release();

    yl_funcobj* _funcobj;

};




/*
    Attributes marking functions or methods exposed to ylang (ylfunc), or
    entry points or callbacks to ylang (ylcall).

*/


#ifdef __YMETAC__

#define ylfunc  __attribute__(( annotate( "ylfunc"  ) ))
#define ylcall  __attribute__(( annotate( "ylcall"  ) ))

#else

#define ylfunc
#define ylcall

#endif




/*
    yl_expose is the base class of native objects exposed to ylang.

*/


class yl_expose
{
public:

    yl_expose();
    ~yl_expose();
    
    
private:



};



/*
    A yl_callframe represents multiple values passed from ylang to C++ or back.

*/


class yl_callframe
{
};




/*
    The most basic entry point.
*/

void yl_invoke( const yl_function& function, yl_callframe& callframe );





/*

*/


template < typename ... arguments_t >
inline yl_function yl_compile( const char* path, arguments_t ... arguments )
{
    const char* paramv[] = { arguments ... };
    return yl_compile( path, sizeof ... ( arguments ), paramv );
}




inline yl_function::yl_function( yl_funcobj* funcobj )
    :   _funcobj( funcobj )
{
    acquire();
}

inline yl_function::yl_function()
    :   _funcobj( nullptr )
{
}

inline yl_function::yl_function( std::nullptr_t )
    :   _funcobj( nullptr )
{
}

inline yl_function& yl_function::operator = ( std::nullptr_t )
{
    release();
    return *this;
}

inline yl_function::yl_function( const yl_function& p )
    :   _funcobj( p._funcobj )
{
    acquire();
}

inline yl_function& yl_function::operator = ( const yl_function& p )
{
    release();
    _funcobj = p._funcobj;
    acquire();
    return *this;
}

inline yl_function::yl_function( yl_function&& p )
{
    _funcobj = p._funcobj;
    p._funcobj = nullptr;
}

inline yl_function& yl_function::operator = ( yl_function&& p )
{
    std::swap( _funcobj, p._funcobj );
    p.release();
    return *this;
}

inline yl_function::~yl_function()
{
    release();
}

inline yl_function::operator bool () const
{
    return _funcobj != nullptr;
}



#endif
