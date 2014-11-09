//
//  yvm.cpp
//
//  Created by Edmund Kapusniak on 09/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yvm.h"
#include "ymodule.h"
#include "ystring.h"
#include "yarray.h"
#include "ytable.h"


ystack::ystack()
    :   up( 0 )
    ,   mark( 0 )
{
}



static inline double mod( double a, double b )
{
    return a - floor( a / b ) * b;
}

static inline double intdiv( double a, double b )
{
    return floor( a / b );
}

static inline size_t check_integer( yvalue v )
{
    double f = v.as_number();
    size_t i = (size_t)f;
    if ( i == f )
        return i;
    else
        throw yerror( "expected integer" );
}


void yexec( size_t sp, unsigned argcount, unsigned resultcount )
{
    ystack* stack = yscope::scope->stack;


    /*
        If argcount is Y_MARK, then the mark points to the top argument.
    */
    
    if ( argcount == Y_MARK )
    {
        argcount = (unsigned)( stack->mark - sp );
    }


    /*
        Stack frames on entry look like:
        
            sp  ->  function
                    argument
                    argument
                    ...
    */
    
    yfunction* function = stack->stack[ sp ].as< yfunction >();
    yrecipe* recipe = function->recipe();
    ymodule* module = recipe->module();

    size_t fp = sp;

    if ( argcount < recipe->param_count() )
    {
        // Set missing arguments to null.
        for ( unsigned i = argcount; i < recipe->param_count(); ++i )
        {
            stack->stack[ sp + i ] = nullptr;
        }
    }
    else if ( argcount > recipe->param_count() && recipe->is_varargs() )
    {
        // Preserve existing stack frame so that varargs are accessible.
        // Reconstruct a stack frame with the correct arguments above it..
        fp = sp + 1 + argcount;
        stack->ensure_stack( fp + 1 + recipe->param_count() );
        stack->stack[ fp ] = stack->stack[ sp ];
        for ( int i = 0; i < recipe->param_count(); ++i )
        {
            stack->stack[ fp + 1 + i ] = stack->stack[ sp + 1 + i ];
        }
    }
    
    stack->ensure_stack( fp + recipe->stack_count() );
    
    
    /*
        The upval stack also effectively has 'frames'.
    */
    
    size_t origup = stack->up;
    stack->up = origup + recipe->newup_count();
    stack->ensure_upvals( stack->up );
    
    
    /*
        Evaluate bytecode.
    */
    
    yexpand* global = function->global();
    yvalue* s = stack->stack.data() + fp;
    yupval** u = stack->upvals.data() + origup;
    yinstruction* ip = recipe->code();
    
    while ( true )
    {
        
    yinstruction i = *ip++;
    switch ( i.opcode() )
    {
    
    case Y_NOP:
        break;
    
    case Y_NULL:
        s[ i.r() ] = nullptr;
        break;
    case Y_VALUE:
        s[ i.r() ] = module->value( i.c() );
        break;
    
    case Y_MOV:
        s[ i.r() ] = s[ i.a() ];
        break;
    case Y_SWP:
        std::swap( s[ i.r() ], s[ i.a() ] );
        break;
        
    case Y_POS:
        s[ i.r() ] = +s[ i.a() ].as_number();
        break;
    case Y_NEG:
        s[ i.r() ] = -s[ i.a() ].as_number();
        break;
    case Y_NOT:
        s[ i.r() ] = ~(uint32_t)s[ i.a() ].as_number();
        break;
    
    case Y_MUL:
        s[ i.r() ] = s[ i.a() ].as_number() * s[ i.b() ].as_number();
        break;
    case Y_DIV:
        s[ i.r() ] = s[ i.a() ].as_number() / s[ i.b() ].as_number();
        break;
    case Y_MOD:
        s[ i.r() ] = mod( s[ i.a() ].as_number(), s[ i.b() ].as_number() );
        break;
    case Y_INTDIV:
        s[ i.r() ] = intdiv( s[ i.a() ].as_number(), s[ i.b() ].as_number() );
        break;
    case Y_ADD:
        s[ i.r() ] = s[ i.a() ].as_number() + s[ i.b() ].as_number();
        break;
    case Y_SUB:
        s[ i.r() ] = s[ i.a() ].as_number() - s[ i.b() ].as_number();
        break;
    case Y_LSL:
        s[ i.r() ] = (uint32_t)s[ i.a() ].as_number() << (int)s[ i.b() ].as_number();
        break;
    case Y_LSR:
        s[ i.r() ] = (uint32_t)s[ i.a() ].as_number() >> (int)s[ i.b() ].as_number();
        break;
    case Y_ASR:
        s[ i.r() ] = (uint32_t)( (int32_t)s[ i.a() ].as_number() >> (int)s[ i.b() ].as_number() );
        break;
    case Y_AND:
        s[ i.r() ] = (uint32_t)s[ i.a() ].as_number() & (uint32_t)s[ i.b() ].as_number();
        break;
    case Y_XOR:
        s[ i.r() ] = (uint32_t)s[ i.a() ].as_number() ^ (uint32_t)s[ i.b() ].as_number();
        break;
    case Y_OR:
        s[ i.r() ] = (uint32_t)s[ i.a() ].as_number() | (uint32_t)s[ i.b() ].as_number();
        break;
    case Y_CONCAT:
        s[ i.r() ] = ystring::strcat( s[ i.a() ].as_string(), s[ i.b() ].as_string() );
        break;
    
    case Y_TEST:
        s[ i.r() ] = (bool)s[ i.a() ];
        break;
    case Y_LNOT:
        s[ i.r() ] = ! s[ i.a() ];
        break;
    case Y_LXOR:
        s[ i.r() ] = (bool)s[ i.a() ] != (bool)s[ i.b() ];
        break;
        
    case Y_KEY:
    {
        ysymbol o = module->symbol( i.b() );
        yvalue v = s[ i.a() ].as_expand()->getkey( o );
        if ( ! v.is_undefined() )
            s[ i.r() ] = v;
        else
            throw yerror( "key not found '%s'", o->c_str() );
        break;
    }
    case Y_INKEY:
    {
        ysymbol o = s[ i.b() ].as_string();
        yvalue v = s[ i.a() ].as_expand()->getkey( o );
        if ( ! v.is_undefined() )
            s[ i.r() ] = v;
        else
            throw yerror( "key not found '%s'", o->c_str() );
        break;
    }
    case Y_INDEX:
    {
        yvalue a = s[ i.a() ];
        if ( a.is< yarray >() )
        {
            size_t index = check_integer( s[ i.b() ] );
            s[ i.r() ] = a.cast< yarray >()->getindex( index );
        }
        else if ( a.is< ytable >() )
        {
            yvalue v = a.cast< ytable >()->getindex( s[ i.b() ] );
            if ( ! v.is_undefined() )
                s[ i.r() ] = v;
            else
                throw yerror( "key not found" );
        }
        else
            throw yerror( "expected indexable value" );
        break;
    }
    case Y_ELEM:
    {
        s[ i.r() ] = s[ i.a() ].as< yarray >()->getindex( i.b() );
        break;
    }
    case Y_GLOBAL:
    {
        ysymbol o = module->symbol( i.b() );
        yvalue v = global->getkey( o );
        if ( ! v.is_undefined() )
            s[ i.r() ] = v;
        else
            throw yerror( "undefined global '%s'", o->c_str() );
        break;
    }

    case Y_SETKEY:
    {
        ysymbol o = module->symbol( i.b() );
        s[ i.a() ].as_expand()->setkey( o, s[ i.r() ] );
        break;
    }
    case Y_SETINKEY:
    {
        ysymbol o = s[ i.b() ].as_string();
        s[ i.a() ].as_expand()->setkey( o, s[ i.r() ] );
        break;
    }
    case Y_SETINDEX:
    {
        yvalue a = s[ i.a() ];
        if ( a.is< yarray >() )
            a.cast< yarray >()->setindex( check_integer( s[ i.b() ] ), s[ i.r() ] );
        else if ( a.is< ytable >() )
            a.cast< ytable >()->setindex( s[ i.b() ], s[ i.r() ] );
        else
            throw yerror( "expected indexable value" );
        break;
    }
    case Y_SETGLOBAL:
    {
        global->setkey( module->symbol( i.b() ), s[ i.r() ] );
        break;
    }

    case Y_DELKEY:
        s[ i.a() ].as_expand()->delkey( module->symbol( i.b() ) );
        break;
    case Y_DELINKEY:
        s[ i.b() ].as_expand()->delkey( s[ i.b() ].as_string() );
        break;

    case Y_NEWUP:
        assert( u[ i.c() ] == nullptr );
        u[ i.c() ] = yupval::alloc( s[ i.r() ] );
        break;
    case Y_SETNU:
        u[ i.c() ]->set( s[ i.r() ] );
        break;
    case Y_REFNU:
        s[ i.r() ] = u[ i.c() ]->get();
        break;
    case Y_CLOSE:
        u[ i.c() ] = nullptr;
        break;
    
    case Y_SETUP:
        function->getupval( i.c() )->set( s[ i.r() ] );
        break;
    case Y_REFUP:
        s[ i.r() ] = function->getupval( i.c() )->get();
        break;
    
    case Y_EQ:
        s[ i.r() ] = ( s[ i.a() ] == s[ i.b() ] );
        break;
    case Y_LT:
        s[ i.r() ] = ( s[ i.a() ] < s[ i.b() ] );
        break;
    case Y_LE:
        s[ i.r() ] = ( s[ i.a() ] <= s[ i.b() ] );
        break;
    case Y_IN:
        assert( ! "not implemented!" );
        break;
    case Y_IS:
        assert( ! "not implemented!" );
        break;
 
    case Y_JMP:
    {
        ip += i.j();
        break;
    }
    case Y_IFTRUE:
    {
        if ( s[ i.r() ] )
            ip += i.j();
        break;
    }
    case Y_IFFALSE:
    {
        if ( ! s[i.r() ] )
            ip += i.j();
        break;
    }
    
    case Y_ITER:
    case Y_ITERKEY:
    case Y_NEXT1:
    case Y_NEXT2:
    case Y_NEXT:
    case Y_IFITER:
    case Y_IFNITER:
    case Y_POPITER:
        assert( ! "not implemented!" );
        break;
    
    case Y_TABLE:
    {
        s[ i.r() ] = ytable::alloc();
        break;
    }
    case Y_OBJECT:
    {
        if ( i.a() == Y_NOVAL )
            s[ i.r() ] = yexpand::alloc();
        else
            s[ i.r() ] = yexpand::alloc( s[ i.a() ].as_expand() );
        break;
    }
        
    Y_ARRAY,        // r = new array (reserve c indexes)
    Y_UNPACK,       // r .. set @mark = a[ (immediate)b .. end ]
    Y_APPEND,       // append r to (array)a
    Y_EXTEND,       // append r .. @mark to (array)a
    
    Y_CLOSURE,      // r = function closure of functions[ c ]
    Y_ENVNU,        // add upvals[ c ] to previous function closure
    Y_ENVUP,        // add closure upvals[ c ] to previous function closure

    Y_VARARG,       // r = varargs[ c ]
    Y_VARALL,       // r .. set @top = varargs
    
    Y_CALL,         // r .. r + b = call r .. r + a (with @mark)
    Y_YCALL,        // r .. r + b = yield call r .. r + a (with @mark)
    Y_YIELD,        // r .. r + b = yield r .. r + a (with @mark)
    Y_NEW,          // r = new a .. a + b (with @mark)
    
    Y_RETURN,       // return r .. r + a (with @mark)
    
    
    
    
    
    
    
    }

    
    }
    
    
    


}

