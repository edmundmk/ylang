//
//  lib.cpp
//
//  Created by Edmund Kapusniak on 04/05/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



namespace lib
{



int atoi( const char* string )
{
    char* endptr = nullptr;
    long result = ::strtol( string, &endptr, 0 );
    if ( endptr[ 0 ] != '\0' )
    {
        throw yl_exception( "unable to parse integer '%s'", string );
    }
    return (int)result;
}


double max( double a, double b )
{
    return std::max( a, b );
}


void printf( yl_callframe& xf )
{
    fputs( sprintf( xf ).c_str(), stdout );
}


static char* append( std::vector< char >& r, size_t len )
{
    size_t index = r.size() - 1;
    r.insert( r.end(), len, '\0' );
    return r.data() + index;
}


std::string sprintf( yl_callframe& xf )
{
    std::vector< char > r;
    r.push_back( '\0' );
    
    const char* format = xf.get_string( 1 );
    size_t index = 2;
    
    for ( const char* c = format; c[ 0 ] != '\0'; )
    {
        if ( c[ 0 ] == '%' )
        {
            // Parse format specifier.
            const char* spec_start = c;

            c += 1;
            
            while ( strchr( "-+ #0", c[ 0 ] ) )
            {
                c += 1;
            }
            
            if ( c[ 0 ] != '*' )
            {
                while ( c[ 0 ] >= '0' && c[ 0 ] <= '9' )
                {
                    c += 1;
                }
            }
            else
            {
                c += 1;
            }
            
            if ( c[ 0 ] == '.' )
            {
                c += 1;
                
                if ( c[ 0 ] != '*' )
                {
                    while ( c[ 0 ] >= '0' && c[ 0 ] <= '9' )
                    {
                        c += 1;
                    }
                }
                else
                {
                    c += 1;
                }
            }
            
            const char* spec_end = c;

            if ( strchr( "hljztL", c[ 0 ] ) )
            {
                if ( c[ 0 ] == 'h' && c[ 1 ] == 'h' )
                    c += 2;
                else if ( c[ 0 ] == 'l' && c[ 1 ] == 'l' )
                    c += 2;
                else
                    c += 1;
            }
            
            char spec_format = c[ 0 ];
            c += 1;


            // Reconstruct format specifier string without length modifiers.
            char spec[ spec_end - spec_start + 2 ];
            memcpy( spec, spec_start, spec_end - spec_start );
            spec[ spec_end - spec_start ] = spec_format;
            spec[ spec_end - spec_start + 1 ] = '\0';
            
            
            // Write argument using format.
            switch ( spec_format )
            {
            case '%':
            {
                putchar( '%' );
                break;
            }
            
            case 'c':
            {
                int arg = xf.get_integer( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
            
            case 's':
            {
                const char* arg = xf.get_string( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
            
            case 'd':
            case 'i':
            {
                // TODO: deal with length modifiers properly.
                int arg = xf.get_integer( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
                    
            case 'u':
            case 'o':
            case 'x':
            case 'X':
            {
                // TODO: deal with length modifiers properly.
                unsigned arg = (unsigned)xf.get_integer( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
            
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'a':
            case 'A':
            case 'g':
            case 'G':
            {
                double arg = xf.get_number( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
            
            case 'p':
            {
                void* arg = xf.get_gcobject( index );
                size_t len = snprintf( nullptr, 0, spec, arg );
                snprintf( append( r, len ), len + 1, spec, arg );
                index += 1;
                break;
            }
            
            default:
            {
                throw yl_exception( "invalid format specifier" );
            }
            }
            
        }
        else
        {
            r.back() = c[ 0 ];
            r.push_back( '\0' );
            c += 1;
        }
    }
    
    return r.data();
}


double sqrt( double x )
{
    return ::sqrt( x );
}


}
