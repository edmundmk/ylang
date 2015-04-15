//
//  yl_interp.cpp
//
//  Created by Edmund Kapusniak on 14/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//



#include "yl_interp.h"
#include "yl_cothread.h"
#include <divmod.h>



static inline double cast_number( const yl_tagval& value )
{
    if ( value.kind() != YLOBJ_NUMBER )
    {
        throw yl_exception( "expected number" );
    }
    
    return value.number();
}

static inline yl_string* cast_string( const yl_tagval& value )
{
    if ( value.kind() != YLOBJ_STRING )
    {
        throw yl_exception( "expected string" );
    }
    
    return (yl_string*)value.heapobj();
}



yl_cothread* yl_interp( yl_cothread* cothread )
{
    yl_stackframe* f = cothread->call_frame();
    
    yl_program* p = f->funcobj->program();
    const yl_value*  values = p->values();
    const yl_opinst* ops    = p->ops();
    
    yl_tagval*   s     = cothread->stack( f->stack_base, p->stackcount() );
    yl_upval**   upval = cothread->upval( f->upval_base, p->upcount() );
    yl_iterator* iters = cothread->iters( f->iters_base, p->itercount() );

    
    while ( true )
    {
    
    const yl_opinst* op = ops + f->ip;
    f->ip += 1;
    
    unsigned r = op->r();
    unsigned a = op->a();
    unsigned b = op->b();
    unsigned c = op->c();
    unsigned j = op->j();
    
    switch ( op->opcode() )
    {
    case YL_NOP:
    {
        break;
    }
    
    case YL_MOV:
    {
        s[ r ] = s[ a ];
        break;
    }
    
    case YL_SWP:
    {
        std::swap( s[ r ], s[ a ] );
        break;
    }

    case YL_NULL:
    {
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, yl_null );
        break;
    }
    
    case YL_BOOL:
    {
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, a ? yl_true : yl_false );
        break;
    }
    
    case YL_NUMBER:
    {
        s[ r ] = yl_tagval( values[ c ].get().as_number() );
        break;
    }
    
    case YL_STRING:
    {
        yl_heapobj* value = values[ c ].get().as_heapobj();
        assert( value->kind() == YLOBJ_STRING );
        s[ r ] = yl_tagval( YLOBJ_STRING, value );
        break;
    }


    case YL_GLOBAL:
    {
        yl_heapobj* value = values[ b ].get().as_heapobj();
        assert( value->kind() == YLOBJ_STRING );
        s[ r ] = yl_current->get_global( (yl_string*)value );
        break;
    }
    
    case YL_SETGLOBAL:
    {
        yl_heapobj* value = values[ b ].get().as_heapobj();
        assert( value->kind() == YLOBJ_STRING );
        yl_current->set_global( (yl_string*)value, s[ r ] );
        break;
    }


    case YL_NEG:
    {
        s[ r ] = -cast_number( s[ a ] );
        break;
    }
    
    case YL_BITNOT:
    {
        s[ r ] = ~(uint32_t)cast_number( s[ a ] );
        break;
    }

    case YL_MUL:
    {
        s[ r ] = cast_number( s[ a ] ) * cast_number( s[ b ] );
        break;
    }
    
    case YL_DIV:
    {
        s[ r ] = cast_number( s[ a ] ) / cast_number( s[ b ] );
        break;
    }

    case YL_MOD:
    {
        s[ r ] = emod( cast_number( s[ a ] ), cast_number( s[ b ] ) );
        break;
    }

    case YL_INTDIV:
    {
        s[ r ] = intdiv( cast_number( s[ a ] ), cast_number( s[ b ] ) );
        break;
    }
    
    case YL_ADD:
    {
        s[ r ] = cast_number( s[ a ] ) + cast_number( s[ b ] );
        break;
    }
    
    case YL_SUB:
    {
        s[ r ] = cast_number( s[ a ] ) - cast_number( s[ b ] );
        break;
    }

    case YL_LSL:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        s[ r ] = ia << (int)cast_number( s[ b ] );
        break;
    }

    case YL_LSR:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        s[ r ] = ia >> (int)cast_number( s[ b ] );
        break;
    }

    case YL_ASR:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        s[ r ] = (uint32_t)( (int32_t)ia >> (int)cast_number( s[ b ] ) );
        break;
    }

    case YL_BITAND:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        uint32_t ib = (uint32_t)cast_number( s[ b ] );
        s[ r ] = ia & ib;
        break;
    }
    
    case YL_BITXOR:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        uint32_t ib = (uint32_t)cast_number( s[ b ] );
        s[ r ] = ia ^ ib;
        break;
    }

    case YL_BITOR:
    {
        uint32_t ia = (uint32_t)cast_number( s[ a ] );
        uint32_t ib = (uint32_t)cast_number( s[ b ] );
        s[ r ] = ia | ib;
        break;
    }

    case YL_CONCAT:
    {
        yl_string* sa = cast_string( s[ a ] );
        yl_string* sb = cast_string( s[ b ] );
        s[ r ] = yl_tagval( YLOBJ_STRING, yl_string::concat( sa, sb ) );
        break;
    }
    
    case YL_EQ:
    case YL_NE:
    case YL_LT:
    case YL_GT:
    case YL_LE:
    case YL_GE:
    
    case YL_LNOT:
    case YL_LXOR:


    /*
        Tests and branches.
    */
    
    case YL_JMP:
    case YL_JMPT:
    case YL_JMPF:
    
    
    /*
        Calls.
    */
    
    case YL_VARARG:
    case YL_FUNCTION:
    case YL_CALL:
    case YL_YCALL:
    case YL_YIELD:
    case YL_RETURN:
    
    
    /*
        Iterators.
    */
    
    case YL_ITER:
    case YL_ITERKEY:
    case YL_NEXT1:
    case YL_NEXT2:
    case YL_NEXT:

    case YL_JMPV:
    case YL_JMPN:


    /*
        Upvals.
    */
    
    case YL_GETUP:
    case YL_SETUP:


    /*
        Close upvals and iterators.
    */
    
    case YL_CLOSE:


    /*
        Object model instructions.
    */
    
    case YL_OBJECT:
    case YL_ARRAY:
    case YL_TABLE:
    
    case YL_SUPER:
    
    case YL_KEY:
    case YL_INKEY:
    case YL_INDEX:
    case YL_SETKEY:
    case YL_SETINKEY:
    case YL_SETINDEX:
    case YL_DELKEY:
    case YL_DELINKEY:
    
    case YL_IN:
    case YL_IS:

    case YL_APPEND:
    case YL_EXTEND:
    case YL_UNPACK:
    
    
    /*
        Exception handling.
    */
    
    case YL_THROW:
    case YL_EXCEPT:
    case YL_UNWIND:


    case YL_UPLOCAL:
    case YL_UPUPVAL:
    {
        assert( ! "invalid code" );
        break;
    }
    
    }

    }
    
}



