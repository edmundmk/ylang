//
//  xec_context.cpp
//
//  Created by Edmund Kapusniak on 29/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_context.h"
#include <stdarg.h>
#include "xec_code.h"
#include "xec_string.h"
#include "xec_object.h"
#include "xec_inline.h"



void xec_context::execute( xec_function* function, const char* params, ... )
{
    // Shouldn't actually do this.  At some point we will need to support
    // reentrancy.
    s.clear();
    
    
    // Make closure from function.
    assert( function->upval_count() == 0 );
    s.push_back( new xec_closure( function ) );
    

    // Push arguments to function.
    va_list arguments;
    va_start( arguments, params );
    
    for ( const char* p = params; p[ 0 ] != '\0'; ++p )
    {
        switch ( p[ 0 ] )
        {
        case 'i':
        {
            int arg = va_arg( arguments, int );
            s.push_back( (double)arg );
            break;
        }
        
        case 'u':
        {
            unsigned arg = va_arg( arguments, unsigned );
            s.push_back( (double)arg );
            break;
        }
        
        case 'f':
        {
            double arg = va_arg( arguments, double );
            s.push_back( arg );
            break;
        }
        
        case 's':
        {
            const char* arg = va_arg( arguments, const char* );
            s.push_back( xec_string::create( arg ) );
            break;
        }
        
        default:
            assert( ! "invalid argument type code" );
            break;
        }
    }
    
    va_end( arguments );


    // Execute the function.
    execute( 0, (unsigned)s.size(), 0 );
}



void xec_context::execute( unsigned fp, unsigned acount, unsigned rcount )
{
    xec_function* function = (xec_function*)&s[ fp ].object();
    fp += 1;
    
    s.resize( fp + function->stack_count() );

    unsigned upvalfp = (unsigned)u.size();
    u.resize( upvalfp + function->newup_count() );

    unsigned pc = 0;
    while ( true )
    {
        xec_opinst code = function->code( pc );
        pc += 1;
        
        switch ( code.opcode() )
        {
        case XEC_NOP:
        {
            break;
        }
    
        case XEC_NULL:
        {
            xec_value& r = s[ fp + code.r() ];
            r.decref();
            r = xec_null;
            break;
        }
        
        case XEC_VALUE:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  v = function->module()->value( code.c() );
            v.incref();
            r.decref();
            r = v;
            break;
        }
    
        case XEC_MOV:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            a.incref();
            r.decref();
            r = a;
            break;
        }
        
        case XEC_SWP:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value& a = s[ fp + code.a() ];
            std::swap( r, a );
            break;
        }

        case XEC_POS:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            check_number( a );
            r.decref();
            r = a.number();
            break;
        }
        
        case XEC_NEG:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            check_number( a );
            r.decref();
            r = -a.number();
            break;
        }
        
        case XEC_NOT:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            check_number( a );
            r.decref();
            r = (double)~(uint32_t)a.number();
            break;
        }
    
        case XEC_MUL:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = a.number() * b.number();
            break;
        }
            
        case XEC_DIV:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = a.number() / b.number();
            break;
        }
        
        case XEC_MOD:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = a.number() - floor( a.number() / b.number() ) * b.number();
            break;
        }
        
        case XEC_INTDIV:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = floor( a.number() / b.number() );
            break;
        }
        
        case XEC_ADD:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = a.number() + b.number();
            break;
        }
        
        case XEC_SUB:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = a.number() - b.number();
            break;
        }

        case XEC_LSL:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (uint32_t)a.number() << (uint32_t)b.number() );
            break;
        }

        case XEC_LSR:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (uint32_t)a.number() >> (uint32_t)b.number() );
            break;
        }

        case XEC_ASR:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (int32_t)a.number() >> (uint32_t)b.number() );
            break;
        }

        case XEC_AND:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (uint32_t)a.number() & (uint32_t)b.number() );
            break;
        }
        
        case XEC_XOR:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (uint32_t)a.number() ^ (uint32_t)b.number() );
            break;
        }

        case XEC_OR:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_number( a );
            check_number( b );
            r.decref();
            r = (double)( (uint32_t)a.number() | (uint32_t)b.number() );
            break;
        }

        case XEC_CONCAT:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_string( a );
            check_string( b );

            xec_string& p = a.string();
            xec_string& q = b.string();

            char* buffer = NULL;
            xec_string* o = xec_string::create( p.size() + q.size(), &buffer );
            memcpy( buffer, p.c_str(), p.size() );
            memcpy( buffer + p.size(), q.c_str(), q.size() );

            r.decref();
            r = o;
            
            break;
        }

        case XEC_TEST:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            r.decref();
            r = a.test();
            break;
        }
        
        case XEC_LNOT:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            r.decref();
            r = ! a.test();
            break;
        }
        
        case XEC_LXOR:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            r.decref();
            r = a.test() != b.test();
            break;
        }
    
        case XEC_KEY:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_objkey k = function->module()->key( code.b() );
            check_object( a );
            
            xec_value v = a.object().key( k );
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_INKEY:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_object( a );
            check_string( b );
            
            xec_string& p = b.string();
            xec_objkey k = xec_objkey::create( p.c_str(), p.size() );
            
            xec_value v = a.object().key( k );
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_INDEX:
        {
            
        }
        
        case XEC_ELEM:
        {
        }
        
        case XEC_GLOBAL:
        {
        }
    
        case XEC_SETKEY:
        case XEC_SETINKEY:
        case XEC_SETINDEX:
        case XEC_SETGLOBAL:
    
        case XEC_DELKEY:
        case XEC_DELINKEY:
    
        case XEC_NEWUP:
        case XEC_SETNU:
        case XEC_REFNU:
        case XEC_CLOSE:

        case XEC_SETUP:
        case XEC_REFUP:
    
        case XEC_EQ:
        case XEC_LT:
        case XEC_LE:
        case XEC_IN:
        case XEC_IS:

        case XEC_JMP:
        case XEC_IFTRUE:
        case XEC_IFFALSE:
        case XEC_IFITER:
        case XEC_IFDONE:

        case XEC_ITER:
        case XEC_ITERKEY:
        case XEC_NEXT1:
        case XEC_NEXT2:
        case XEC_NEXT:

        case XEC_TABLE:
        case XEC_OBJECT:

        case XEC_ARRAY:
        case XEC_UNPACK:
        case XEC_APPEND:
        case XEC_EXTEND:
    
        case XEC_CLOSURE:
        case XEC_ENVNU:
        case XEC_ENVUP:

        case XEC_VARARG:
        case XEC_VARALL:
    
        case XEC_CALL:
        case XEC_YCALL:
        case XEC_YIELD:
        case XEC_NEW:
    
        case XEC_RETURN:
            break;
        
        
        }
    }
}


void xec_context::check_number( xec_value v )
{
}

void xec_context::check_string( xec_value v )
{
}

void xec_context::check_object( xec_value v )
{
}






