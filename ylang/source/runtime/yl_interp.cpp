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



static inline bool equal( const yl_tagval& a, const yl_tagval& b )
{
    if ( a.kind() != b.kind() )
    {
        return false;
    }
    
    if ( a.kind() == YLOBJ_NUMBER )
    {
        return a.number() == b.number();
    }

    if ( a.heapobj() == b.heapobj() )
    {
        return true;
    }
    
    if ( a.kind() == YLOBJ_STRING )
    {
        yl_string* sa = (yl_string*)a.heapobj();
        yl_string* sb = (yl_string*)b.heapobj();

        if ( sa->length() != sb->length() )
        {
            return false;
        }
        
        return memcmp( sa->c_str(), sb->c_str(), sa->length() ) == 0;
    }
    
    return false;
}

static inline int compare_strings( const yl_tagval& a, const yl_tagval& b )
{
    yl_string* sa = (yl_string*)a.heapobj();
    yl_string* sb = (yl_string*)b.heapobj();
    
    size_t common_length = std::min( sa->length(), sb->length() );
    int result = memcmp( sa->c_str(), sb->c_str(), common_length );
    if ( result == 0 )
    {
        if ( sa->length() < sb->length() )
            return -1;
        else if ( sa->length() == sb->length() )
            return 0;
        else
            return 1;
    }
    
    return result;
}

static inline bool test( const yl_tagval& v )
{
    if ( v.kind() == YLOBJ_NUMBER )
    {
        return v.number() != 0.0;
    }
    else
    {
        return v.heapobj() >= yl_true;
    }
}



yl_cothread* yl_interp( yl_cothread* cothread )
{
    yl_stackframe* f = cothread->call_frame();
    
    yl_program* p = f->funcobj->program();
    const yl_value*  values = p->values();
    const yl_opinst* ops    = p->ops();
    
    yl_tagval*   vargs = cothread->stack( f->base + 1, f->argcount );
    yl_tagval*   s     = cothread->stack( f->stack_base, p->stackcount() );
    yl_upval**   locup = cothread->locup( f->locup_base, p->localupcount() );
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
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, isequal ? yl_true : yl_false );
        break;
    }
    
    case YL_NE:
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, isequal ? yl_false : yl_true );
        break;
    }
    
    case YL_LT:
    {
        bool islt;
        if ( s[ a ].kind() == s[ b ].kind() )
        {
            if ( s[ a ].kind() == YLOBJ_NUMBER )
                islt = ( s[ a ].number() < s[ b ].number() );
            else if ( s[ a ].kind() == YLOBJ_STRING )
                islt = ( compare_strings( s[ a ], s[ b ] ) < 0 );
            else
                throw yl_exception( "invalid type for comparison" );
        }
        else
        {
            throw yl_exception( "type mismatch in comparison" );
        }
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, islt ? yl_true : yl_false );
        break;
    }
    
    case YL_GT:
    {
        bool isgt;
        if ( s[ a ].kind() == s[ b ].kind() )
        {
            if ( s[ a ].kind() == YLOBJ_NUMBER )
                isgt = ( s[ a ].number() > s[ b ].number() );
            else if ( s[ a ].kind() == YLOBJ_STRING )
                isgt = ( compare_strings( s[ a ], s[ b ] ) > 0 );
            else
                throw yl_exception( "invalid type for comparison" );
        }
        else
        {
            throw yl_exception( "type mismatch in comparison" );
        }
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, isgt ? yl_true : yl_false );
        break;
    }
    
    case YL_LE:
    {
        bool isle;
        if ( s[ a ].kind() == s[ b ].kind() )
        {
            if ( s[ a ].kind() == YLOBJ_NUMBER )
                isle = ( s[ a ].number() <= s[ b ].number() );
            else if ( s[ a ].kind() == YLOBJ_STRING )
                isle = ( compare_strings( s[ a ], s[ b ] ) <= 0 );
            else
                throw yl_exception( "invalid type for comparison" );
        }
        else
        {
            throw yl_exception( "type mismatch in comparison" );
        }
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, isle ? yl_true : yl_false );
        break;
    }
    
    case YL_GE:
    {
        bool isge;
        if ( s[ a ].kind() == s[ b ].kind() )
        {
            if ( s[ a ].kind() == YLOBJ_NUMBER )
                isge = ( s[ a ].number() >= s[ b ].number() );
            else if ( s[ a ].kind() == YLOBJ_STRING )
                isge = ( compare_strings( s[ a ], s[ b ] ) >= 0 );
            else
                throw yl_exception( "invalid type for comparison" );
        }
        else
        {
            throw yl_exception( "type mismatch in comparison" );
        }
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, isge ? yl_true : yl_false );
        break;
    }
    
    case YL_LNOT:
    {
        bool istrue = test( s[ a ] );
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, istrue ? yl_false : yl_true );
        break;
    }
    
    case YL_LXOR:
    {
        bool result = test( s[ a ] ) != test( s[ b ] );
        s[ r ] = yl_tagval( YLOBJ_SINGULAR, result ? yl_true : yl_false );
        break;
    }

    case YL_JMP:
    {
        f->ip += j;
        break;
    }
    
    case YL_JMPT:
    {
        if ( test( s[ r ] ) )
        {
            f->ip += j;
        }
        break;
    }
    
    case YL_JMPF:
    {
        if ( ! test( s[ r ] ) )
        {
            f->ip += j;
        }
        break;
    }
    
    case YL_VARARG:
    {
        assert( p->varargs() );
        size_t argcopy = std::min( (size_t)f->argcount, (size_t)b );
        for ( size_t i = 0; i < argcopy; ++i )
        {
            s[ r + i ] = vargs[ i ];
        }
        for ( size_t i = argcopy; i < b; ++i )
        {
            s[ r + i ] = yl_tagval( YLOBJ_SINGULAR, yl_null );
        }
        break;
    }
    
    case YL_FUNCTION:
    {
        // Get program object.
        yl_heapobj* progobj = values[ c ].get().as_heapobj();
        assert( progobj->kind() == YLOBJ_PROGRAM );
        yl_program* program = (yl_program*)progobj;
        
        // Create funcobj.
        yl_funcobj* funcobj = yl_funcobj::alloc( program );

        // Add upvals.
        for ( size_t i = 0; i < program->upcount(); ++i )
        {
            const yl_opinst* op = ops + f->ip;
            f->ip += 1;

            unsigned r = op->r();
            unsigned a = op->a();
            unsigned b = op->b();

            switch ( op->opcode() )
            {
            case YL_UPLOCAL:
            {
                if ( ! locup[ a ] )
                {
                    locup[ a ] = yl_upval::alloc( f->stack_base + b );
                }
                funcobj->set_upval( r, locup[ a ] );
                break;
            }
            
            case YL_UPUPVAL:
            {
                funcobj->set_upval( r, f->funcobj->get_upval( a ) );
                break;
            }
            
            default:
            {
                assert( ! "invalid code" );
                break;
            }
            }
        }
        
        break;
    }
    
    case YL_CALL:
    case YL_YCALL:
    {
        // If you call a coroutine, then we construct a new cothread.
        // If you call a cothread, then we pass a new set of parameters in.
        // Ycalls of coroutines or 
    }
    
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



