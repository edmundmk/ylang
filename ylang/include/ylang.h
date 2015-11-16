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
class yl_gcobject;
class yl_cothread;




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

    friend class yl_context_impl;

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

    friend class yl_context_impl;

    explicit yl_scope( yl_context_impl* context );

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
    yl_function( const yl_function& p );
    yl_function& operator = ( const yl_function& p );
    ~yl_function();
    
    explicit operator bool () const;
    

private:

    friend class yl_funcobj;
    friend class yl_callframe;

    explicit yl_function( yl_gcobject* function );
    void reset( yl_gcobject* p );

    yl_gcobject* _function;

};




/*
    Attributes marking functions or methods exposed to ylang (ylcall), or
    entry points or callbacks to ylang (ylevent).  Use ylname to give a
    function a different name in ylang.

*/


#ifdef __YMETAC__

#define yl_call     __attribute__(( annotate( "yl_call" ) ))
#define yl_event    __attribute__(( annotate( "yl_event" ) ))

#else

#define yl_call
#define yl_event

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
    A yl_callframe passes variable argument lists between ylang and C++.

*/

enum yl_valkind
{
    YLVAL_NULL,
    YLVAL_BOOL,
    YLVAL_NUMBER,
    YLVAL_STRING,
    YLVAL_EXPOSE,
    YLVAL_FUNCTION,
    YLVAL_UNKNOWN,
};


class yl_callframe
{
public:

    yl_callframe();
    ~yl_callframe();

    void            push( std::nullptr_t );
    void            push_bool( bool value );
    void            push( const char* value );
    void            push( double value );
    void            push( yl_expose* value );
    void            push( const yl_function& function );

    size_t          size() const;
    
    yl_valkind      at( size_t index ) const;
    bool            get_bool( size_t index ) const;
    double          get_number( size_t index ) const;
    int             get_integer( size_t index ) const;
    const char*     get_string( size_t index ) const;
    yl_expose*      get_expose( size_t index ) const;
    yl_function     get_function( size_t index ) const;
    yl_gcobject*    get_gcobject( size_t index ) const;

    void            clear();


private:

    friend void yl_invoke( yl_callframe& );
    friend void yl_interp( yl_cothread*, unsigned, unsigned, unsigned );

    yl_callframe( yl_cothread* cothread, unsigned base, unsigned size );

    yl_cothread*    _cothread;
    unsigned        _base;
    unsigned        _size;

};




/*
    The most basic entry point.
*/

void yl_invoke( yl_callframe& xf );






/*
    Private API for the meta compiler.
*/


void _yl_global_function( const char*, void (*)( yl_callframe& ) );





/*

*/


template < typename ... arguments_t >
inline yl_function yl_compile( const char* path, arguments_t ... arguments )
{
    const char* paramv[] = { arguments ... };
    return yl_compile( path, sizeof ... ( arguments ), paramv );
}




inline yl_function::yl_function()
    :   _function( nullptr )
{
}

inline yl_function::yl_function( const yl_function& p )
    :   _function( nullptr )
{
    reset( p._function );
}

inline yl_function& yl_function::operator = ( const yl_function& p )
{
    reset( p._function );
    return *this;
}

inline yl_function::~yl_function()
{
    reset( nullptr );
}

inline yl_function::operator bool () const
{
    return _function != nullptr;
}



#endif
