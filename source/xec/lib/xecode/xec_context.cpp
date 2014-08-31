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


xec_context::xec_context()
{
    g = new xec_object();
}

xec_context::~xec_context()
{
    g->decref();
}


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
    xec_closure& closure = s[ fp ].object().closure();
    xec_function* function = closure.function();
    
    s.resize( fp + function->stack_count() );

    unsigned up = (unsigned)u.size();
    u.resize( up + function->newup_count() );

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
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            check_object( a );
            
            xec_value v;
            xec_object& o = a.object();
            if ( o.isarray() )
            {
                check_integer( b );
                v = o.array().index( (size_t)b.number() );
            }
            else if ( o.istable() )
            {
                v = o.table().index( b );
            }
            else
            {
                // This is an error.
            }
            
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_ELEM:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            unsigned   b = code.b();
            check_object( a );
            
            xec_value v;
            xec_object& o = a.object();
            if ( o.isarray() )
            {
                v = o.array().index( b );
            }
            else
            {
                // This is an error.
            }
            
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_GLOBAL:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_objkey k = function->module()->key( code.b() );
            xec_value v = g->key( k );
            v.incref();
            r.decref();
            r = v;
            break;
        }
    
        case XEC_SETKEY:
        {
            xec_value r = s[ fp + code.r() ];
            xec_value a = s[ fp + code.a() ];
            xec_objkey k = function->module()->key( code.b() );
            check_object( a );
            
            xec_object& o = a.object();
            o.setkey( k, r );
            break;
        }
        
        case XEC_SETINKEY:
        {
            xec_value r = s[ fp + code.r() ];
            xec_value a = s[ fp + code.a() ];
            xec_value b = s[ fp + code.b() ];
            check_object( a );
            check_string( b );
            
            xec_string& p = b.string();
            xec_objkey k = xec_objkey::create( p.c_str(), p.size() );
            
            xec_object& o = a.object();
            o.setkey( k, r );
            break;
        }
        
        case XEC_SETINDEX:
        {
            xec_value r = s[ fp + code.r() ];
            xec_value a = s[ fp + code.a() ];
            xec_value b = s[ fp + code.b() ];
            check_object( a );

            xec_object& o = a.object();
            if ( o.isarray() )
            {
                check_integer( b );
                o.array().setindex( (size_t)b.number(), r );
            }
            else if ( o.istable() )
            {
                o.table().setindex( b, r );
            }
            else
            {
                // This is an error.
            }

            break;
        }
        
        case XEC_SETGLOBAL:
        {
            xec_value r = s[ fp + code.r() ];
            xec_objkey k = function->module()->key( code.b() );
            g->setkey( k, r );
            break;
        }
    
        case XEC_DELKEY:
        {
            // TODO.
            break;
        }
        
        case XEC_DELINKEY:
        {
            // TODO.
            break;
        }
    
        case XEC_NEWUP:
        {
            xec_value r = s[ fp + code.r() ];
            xec_upval*& p = u[ up + code.c() ];
            assert( ! p );
            p = new xec_upval( r );
            break;
        }
        
        case XEC_SETNU:
        {
            xec_value r = s[ fp + code.r() ];
            xec_upval* p = u[ up + code.c() ];
            p->assign( r );
            break;
        }
        
        case XEC_REFNU:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_upval* p = u[ up + code.c() ];
            xec_value v = p->get();
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_CLOSE:
        {
            for ( unsigned c = fp + code.c();
                            c <= fp + code.c() + code.r(); ++c )
            {
                xec_upval*& p = u[ c ];
                p->decref();
                p = NULL;
            }
            break;
        }

        case XEC_SETUP:
        {
            xec_value r = s[ fp + code.r() ];
            xec_upval* p = closure.upval( code.c() );
            p->assign( r );
            break;
        }
        
        case XEC_REFUP:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_upval* p = closure.upval( code.c() );
            xec_value v = p->get();
            v.incref();
            r.decref();
            r = v;
            break;
        }
    
        case XEC_EQ:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            
            xec_value v;
            if ( a.isnumber() && b.isnumber() )
            {
                v = a.number() == b.number();
            }
            else if ( a.isstring() && b.isstring() )
            {
                xec_string& p = a.string();
                xec_string& q = b.string();
                v = p.size() == q.size()
                        && memcmp( p.c_str(), q.c_str(), p.size() ) == 0;
            }
            else
            {
                v = a == b;
            }
            
            r.decref();
            r = v;
            break;
        }
        
        case XEC_LT:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            
            xec_value v;
            if ( a.isnumber() && b.isnumber() )
            {
                v = a.number() < b.number();
            }
            else if ( a.isstring() && b.isstring() )
            {
                xec_string& p = a.string();
                xec_string& q = b.string();
                size_t size = std::min( p.size(), q.size() );
                int compare = memcmp( p.c_str(), q.c_str(), size );
                v = compare < 0 || ( compare == 0 && p.size() < q.size() );
            }
            else
            {
                // This is an error.
            }
            
            r.decref();
            r = v;
            break;
        }
        
        case XEC_LE:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_value  b = s[ fp + code.b() ];
            
            xec_value v;
            if ( a.isnumber() && b.isnumber() )
            {
                v = a.number() <= b.number();
            }
            else if ( a.isstring() && b.isstring() )
            {
                xec_string& p = a.string();
                xec_string& q = b.string();
                size_t size = std::min( p.size(), q.size() );
                int compare = memcmp( p.c_str(), q.c_str(), size );
                v = compare < 0 || ( compare == 0 && p.size() <= q.size() );
            }
            else
            {
                // This is an error.
            }
            
            r.decref();
            r = v;
            break;
        }
        
        case XEC_IN:
        {
            // TODO.
            break;
        }
        
        case XEC_IS:
        {
            // TODO.
            break;
        }

        case XEC_JMP:
        {
            pc += code.j();
            break;
        }
        
        case XEC_IFTRUE:
        {
            xec_value r = s[ fp + code.r() ];
            if ( r.test() )
            {
                pc += code.j();
            }
            break;
        }
        
        case XEC_IFFALSE:
        {
            xec_value r = s[ fp + code.r() ];
            if ( ! r.test() )
            {
                pc += code.j();
            }
            break;
        }

        case XEC_IFITER:
        {
            if ( iter )
            {
                pc += code.j();
            }
            break;
        }
        
        case XEC_IFDONE:
        {
            if ( ! iter )
            {
                pc += code.j();
            }
            break;
        }

        case XEC_ITER:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            xec_iter* i = new xec_iter( a );
            r.decref();
            r = i;
            break;
        }
        
        case XEC_ITERKEY:
        {
            // TODO.
            break;
        }
        
        case XEC_NEXT1:
        {
            xec_value& r = s[ fp + code.r() ];
            xec_value  a = s[ fp + code.a() ];
            check_object( a );
            
            xec_value v;
            xec_object& o = a.object();
            if ( o.isiter() )
            {
                iter = o.iter().next1( &v );
            }
            else
            {
                // This is an error.
            }
            
            v.incref();
            r.decref();
            r = v;
            break;
        }
        
        case XEC_NEXT2:
        {
            // TODO.
            break;
        }
        
        case XEC_NEXT:
        {
            // TODO.
            break;
        }

        case XEC_TABLE:
        {
        }
        
        case XEC_OBJECT:
        {
        }

        case XEC_ARRAY:
        {
        }
        
        case XEC_UNPACK:
        case XEC_APPEND:
        case XEC_EXTEND:
    
        case XEC_CLOSURE:

        case XEC_ENVNU:
        {
            assert( ! "unexpected envnu" );
            break;
        }
        
        case XEC_ENVUP:
        {
            assert( ! "unexpected envup" );
            break;
        }

        case XEC_VARARG:
        {
            // TODO.
            break;
        }
        
        case XEC_VARALL:
        {
            // TODO.
            break;
        }
    
        case XEC_CALL:
        case XEC_YCALL:
        case XEC_YIELD:
        case XEC_NEW:
    
        case XEC_RETURN:
        {
            
            break;
        }
        
        }
    }
}


void xec_context::check_number( xec_value v )
{
}

void xec_context::check_integer( xec_value v )
{
}

void xec_context::check_string( xec_value v )
{
}

void xec_context::check_object( xec_value v )
{
}






