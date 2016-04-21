//
//  yl_exception.cpp
//
//  Created by Edmund Kapusniak on 27/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "ylang.h"
#include "yl_exception.h"
#include <stringf.h>
#include "yl_function.h"
#include "yl_program.h"
#include "yl_debuginfo.h"



yl_exception::yl_exception()
    :   _impl( new yl_exception_impl() )
{
}

yl_exception::yl_exception( std::nullptr_t )
    :   _impl( nullptr )
{
}

yl_exception::yl_exception( const char* format, ... )
    :   _impl( new yl_exception_impl() )
{
    va_list arguments;
    va_start( arguments, format );
    std::string message = vstringf( format, arguments );
    va_end( arguments );
    _impl->value = yl_string::alloc( message.data(), message.size() ).get();
}

yl_exception::yl_exception( const yl_exception& e )
    :   _impl( new yl_exception_impl( *e._impl ) )
{
}

yl_exception::yl_exception( yl_exception&& e )
    :   _impl( std::move( e._impl ) )
{
}

yl_exception& yl_exception::operator = ( yl_exception&& e )
{
    std::swap( _impl, e._impl );
    return *this;
}

yl_exception::~yl_exception()
{
}


const char* yl_exception::what() const throw()
{
    yl_value value = _impl->value.get();
    if ( value.kind() == YLOBJ_STRING )
    {
        yl_string* string = (yl_string*)value.gcobject();
        return string->c_str();
    }
    else
    {
        return "[ylang value]";
    }
}


std::string yl_exception::stacktrace() const throw()
{
    std::string stacktrace;
    for ( const auto& unwound : _impl->unwound )
    {
        if ( unwound.function->kind() == YLOBJ_FUNCOBJ )
        {
            yl_funcobj* funcobj = (yl_funcobj*)unwound.function.get();
            const yl_debuginfo* dinfo = funcobj->program()->debuginfo();
            auto lc = dinfo->line_column( unwound.ip - 1 );
            stacktrace += stringf
                (
                    "    %s:%d:%d: %s\n",
                    dinfo->filename(),
                    lc.first,
                    lc.second,
                    dinfo->funcname()
                );
        }
    }
    return stacktrace;
}








