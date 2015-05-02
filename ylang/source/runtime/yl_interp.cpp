//
//  yl_interp.cpp
//
//  Created by Edmund Kapusniak on 14/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//



#include "yl_interp.h"
#include "yl_cothread.h"
#include <divmod.h>
#include "yl_object.h"
#include "yl_string.h"
#include "yl_array.h"
#include "yl_table.h"



/*
    Helpers.
*/

static inline double cast_number( const yl_value& value )
{
    if ( value.kind() != YLOBJ_NUMBER )
    {
        throw yl_exception( "expected number" );
    }
    
    return value.number();
}

static inline yl_string* cast_string( const yl_value& value )
{
    if ( value.kind() != YLOBJ_STRING )
    {
        throw yl_exception( "expected string" );
    }
    
    return (yl_string*)value.heapobj();
}

static inline int compare_strings( const yl_value& a, const yl_value& b )
{
    yl_string* sa = (yl_string*)a.heapobj();
    yl_string* sb = (yl_string*)b.heapobj();
    
    size_t common_length = std::min( sa->size(), sb->size() );
    int result = memcmp( sa->c_str(), sb->c_str(), common_length );
    if ( result == 0 )
    {
        if ( sa->size() < sb->size() )
            return -1;
        else if ( sa->size() == sb->size() )
            return 0;
        else
            return 1;
    }
    
    return result;
}

static inline bool test( const yl_value& v )
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

static inline yl_object* superof( const yl_value& v )
{
    if ( v.kind() & YLOBJ_IS_OBJECT )
    {
        yl_object* object = (yl_object*)v.heapobj();
        return object->superof();
    }
    else
    {
        return yl_current->superof( v );
    }
}

static inline yl_object* keyerof( const yl_value& v )
{
    if ( v.kind() & YLOBJ_IS_OBJECT )
    {
        return (yl_object*)v.heapobj();
    }
    else
    {
        return yl_current->superof( v );
    }
}



/*
    Build stack frame for a function call.
*/

static void reverse( yl_value* array, size_t size )
{
    for ( size_t i = 0; i < size / 2; ++i )
    {
        std::swap( array[ i ], array[ size - 1 - i ] );
    }
}


static unsigned build_frame( yl_cothread* t, unsigned sp, unsigned acount )
{
    /*
        If acount is MARK then the mark points after the last argument.
    */
    
    if ( acount == yl_opinst::MARK )
    {
        acount = t->get_mark() - sp;
    }
    
    
    /*
        On entry, the stack should look like this:
    
            sp          ->  funcobj
                            argument
                            argument
                            ...
            sp + acount ->
    */
    
    yl_value* stack = t->stack( sp, acount );

    if ( stack[ 0 ].kind() != YLOBJ_FUNCOBJ )
    {
        throw yl_exception( "cannot call non-function" );
    }

    yl_funcobj* f = (yl_funcobj*)stack[ 0 ].heapobj();
    yl_program* p = f->program();
    
    unsigned argcount = acount - 1;
    unsigned paramcount = p->paramcount();
    
    unsigned fp;
    if ( p->varargs() && argcount > paramcount )
    {
        /*
            We must preserve the additional arguments in case they are used:
            
                sp          ->  argument
                                ...
                fp          ->  funcobj
                                argument
                                argument
                                ...
                fp + acount ->
                
            Essentially this means rotating the values on the stack.  There
            is a great trick to rotate an array using reversals:

            http://www.geeksforgeeks.org/program-for-array-rotation-continued-reversal-algorithm/
        */

        fp = sp + ( argcount - paramcount );
        reverse( stack, fp - sp );
        reverse( stack + fp - sp, acount );
        reverse( stack, fp + acount - sp );
        
    }
    else
    {
        /*
            There are no varargs, use the frame in-place.
        */
        
        fp = sp;
    }
    
    if ( argcount < paramcount )
    {
        /*
            Set missing parameters to null.
        */
        
        stack = t->stack( fp, 1 + paramcount );
        for ( unsigned arg = argcount; arg < paramcount; ++arg )
        {
            stack[ 1 + arg ] = yl_value( YLOBJ_NULL, yl_null );
        }
    }

    return fp;
}



/*
    Main interpreter loop.
*/

void yl_interp( yl_cothread* t, unsigned sp, unsigned acount, unsigned rcount )
{
    // Set current cothread.
    yl_current->set_cothread( t );
    
    
    // Remember call depth.
    unsigned call_depth = 0;
    
    
    // Get function.
    yl_value* s = t->stack( sp, 1 + acount );
    if ( s[ 0 ].kind() != YLOBJ_FUNCOBJ )
    {
        throw yl_exception( "cannot call non-function" );
    }
    
    // Build frame for this call.
    unsigned            ip          = 0;
    unsigned            fp          = build_frame( t, sp, acount );
    unsigned            locup_base  = t->get_locup_base();
    unsigned            iters_base  = t->get_iters_base();
    
    
    // Get program.
    yl_funcobj*         f           = (yl_funcobj*)s[ 0 ].heapobj();
    yl_program*         p           = f->program();
    const yl_valref*     values      = p->values();
    const yl_opinst*    ops         = p->ops();


    // Get stacks.
                        s           = t->stack( fp, p->stackcount() );
    yl_upval**          locup       = t->locup( locup_base, p->locupcount() );
    yl_iterator*        iters       = t->iters( iters_base, p->iterscount() );
    
    
    // Main instruction dispatch loop.
    while ( true )
    {
    
    const yl_opinst* op = ops + ip;
    ip += 1;
    
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
        s[ r ] = yl_value( YLOBJ_NULL, yl_null );
        break;
    }
    
    case YL_BOOL:
    {
        s[ r ] = yl_value( YLOBJ_BOOL, a ? yl_true : yl_false );
        break;
    }
    
    case YL_NUMBER:
    {
        s[ r ] = yl_value( values[ c ].get().as_number() );
        break;
    }
    
    case YL_STRING:
    {
        yl_heapobj* value = values[ c ].get().as_heapobj();
        assert( value->kind() == YLOBJ_STRING );
        s[ r ] = yl_value( YLOBJ_STRING, value );
        break;
    }

    case YL_GLOBAL:
    {
        yl_heapobj* value = values[ b ].get().as_heapobj();
        assert( value->kind() == YLOBJ_STRING );
        s[ r ] = yl_current->get_global( (yl_string*)value );
        if ( s[ r ].kind() == YLOBJ_UNDEF )
        {
            throw yl_exception( "unknown global" );
        }
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
        s[ r ] = yl_value( YLOBJ_STRING, yl_string::concat( sa, sb ) );
        break;
    }
    
    case YL_EQ:
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = yl_value( YLOBJ_BOOL, isequal ? yl_true : yl_false );
        break;
    }
    
    case YL_NE:
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = yl_value( YLOBJ_BOOL, isequal ? yl_false : yl_true );
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
        s[ r ] = yl_value( YLOBJ_BOOL, islt ? yl_true : yl_false );
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
        s[ r ] = yl_value( YLOBJ_BOOL, isgt ? yl_true : yl_false );
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
        s[ r ] = yl_value( YLOBJ_BOOL, isle ? yl_true : yl_false );
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
        s[ r ] = yl_value( YLOBJ_BOOL, isge ? yl_true : yl_false );
        break;
    }
    
    case YL_LNOT:
    {
        bool istrue = test( s[ a ] );
        s[ r ] = yl_value( YLOBJ_BOOL, istrue ? yl_false : yl_true );
        break;
    }
    
    case YL_LXOR:
    {
        bool result = test( s[ a ] ) != test( s[ b ] );
        s[ r ] = yl_value( YLOBJ_BOOL, result ? yl_true : yl_false );
        break;
    }

    case YL_JMP:
    {
        ip += j;
        break;
    }
    
    case YL_JMPT:
    {
        if ( test( s[ r ] ) )
        {
            ip += j;
        }
        break;
    }
    
    case YL_JMPF:
    {
        if ( ! test( s[ r ] ) )
        {
            ip += j;
        }
        break;
    }
    
    case YL_VARARG:
    {
        assert( p->varargs() );
        
        unsigned count;
        if ( b != yl_opinst::MARK )
        {
            count = std::min( b, fp - sp );
        }
        else
        {
            count = fp - sp;
            s = t->stack( fp, r + count );
        }
        
        yl_value* varargs = t->stack( sp, fp - sp );
        for ( unsigned i = 0; i < count; ++i )
        {
            s[ r + i ] = varargs[ i ];
        }
        
        if ( b != yl_opinst::MARK )
        {
            for ( unsigned i = count; i < b; ++i )
            {
                s[ r + i ] = yl_value( YLOBJ_NULL, yl_null );
            }
        }
        else
        {
            t->set_mark( fp + r + count );
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
            const yl_opinst* op = ops + ip;
            ip += 1;

            unsigned r = op->r();
            unsigned a = op->a();
            unsigned b = op->b();

            switch ( op->opcode() )
            {
            case YL_UPLOCAL:
            {
                if ( ! locup[ a ] )
                {
                    locup[ a ] = yl_upval::alloc( fp + b );
                }
                funcobj->set_upval( r, locup[ a ] );
                break;
            }
            
            case YL_UPUPVAL:
            {
                funcobj->set_upval( r, f->get_upval( a ) );
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
        // TODO:
        //     If you call a coroutine, then we construct a new cothread.
        //     If you call a cothread, then we pass a new set of parameters in.
        //     Ycalls are always real calls.
        
        // Push current stack frame.
        yl_stackframe frame;
        frame.funcobj       = f;
        frame.ip            = ip;
        frame.sp            = sp;
        frame.fp            = fp;
        frame.locup_base    = locup_base;
        frame.iters_base    = iters_base;
        frame.rcount        = rcount;
        t->push_frame( frame );

        // This call requests b values.
        rcount = b;

        // Check function.
        if ( s[ r ].kind() != YLOBJ_FUNCOBJ )
        {
            throw yl_exception( "cannot call non-function" );
        }

        // Build new frame.
        ip          = 0;
        sp          = fp + r;
        fp          = build_frame( t, sp, a );
        locup_base  += p->locupcount();
        iters_base  += p->iterscount();
        
        // Get function and program.
        f           = (yl_funcobj*)s[ r ].heapobj();
        p           = f->program();
        values      = p->values();
        ops         = p->ops();
        
        // Get stacks.
        s           = t->stack( fp, p->stackcount() );
        locup       = t->locup( locup_base, p->locupcount() );
        iters       = t->iters( iters_base, p->iterscount() );
     
        // Increase call depth.
        call_depth += 1;
        break;
    }
    
    case YL_YIELD:
    {
        assert( ! "coroutines are unimplemented" );
        break;
    }
    
    case YL_RETURN:
    {
        // Find out how many results to copy.
        if ( a == yl_opinst::MARK )
        {
            a = t->get_mark() - r;
        }
        
        unsigned count;
        if ( rcount != yl_opinst::MARK )
        {
            count = std::min( rcount, a );
        }
        else
        {
            count = a;
        }
        
        // Perform copy relative to sp.
        r = fp - sp + r;
        if ( r != 0 )
        {
            s = t->stack( sp, r + a );
            for ( unsigned i = 0; i < count; ++i )
            {
                s[ i ] = s[ r + i ];
            }
        }
        
        if ( rcount != yl_opinst::MARK )
        {
            for ( unsigned i = count; i < rcount; ++i )
            {
                s[ i ] = yl_value( YLOBJ_NULL, yl_null );
            }
        }
        else
        {
            t->set_mark( sp + count );
        }
        
        // Return from previous call.
        if ( call_depth > 0 )
        {
            // Pop stack frame.
            yl_stackframe frame = t->pop_frame();
            f           = frame.funcobj;
            ip          = frame.ip;
            sp          = frame.sp;
            fp          = frame.fp;
            locup_base  = frame.locup_base;
            iters_base  = frame.iters_base;
            rcount      = frame.rcount;
            
            // Get function and program.
            f           = (yl_funcobj*)s[ r ].heapobj();
            p           = f->program();
            values      = p->values();
            ops         = p->ops();
            
            // Get stacks.
            s           = t->stack( fp, p->stackcount() );
            locup       = t->locup( locup_base, p->locupcount() );
            iters       = t->iters( iters_base, p->iterscount() );
            
            // Decrease call depth.
            call_depth -= 1;
        }
        else
        {
            return;
        }

        break;
    }
    
    case YL_ITER:
    {
        iters[ r ].open_vals( s[ a ] );
        break;
    }
    
    case YL_ITERKEY:
    {
        iters[ r ].open_keys( s[ a ] );
        break;
    }
    
    case YL_NEXT1:
    {
        iters[ a ].next1( &s[ r ] );
        break;
    }
    
    case YL_NEXT2:
    {
        iters[ a ].next2( &s[ r ], &s[ b ] );
        break;
    }
    
    case YL_NEXT:
    {
        iters[ a ].next( t, fp + r, b );
        break;
    }

    case YL_JMPV:
    {
        if ( iters[ r ].has_values() )
        {
            ip += j;
        }
        break;
    }
    
    case YL_JMPN:
    {
        if ( ! iters[ r ].has_values() )
        {
            ip += j;
        }
        break;
    }

    case YL_GETUP:
    {
        yl_upval* upval = f->get_upval( a );
        s[ r ] = upval->get_value( t );
        break;
    }
    
    case YL_SETUP:
    {
        yl_upval* upval = f->get_upval( a );
        upval->set_value( t, s[ r ] );
        break;
    }

    case YL_CLOSE:
    {
        for ( size_t i = p->locupcount(); i-- > a; )
        {
            if ( locup[ i ] )
            {
                locup[ i ]->close( t );
                locup[ i ] = nullptr;
            }
        }
        
        for ( size_t i = p->iterscount(); i-- > b; )
        {
            iters[ i ].close();
        }
        
        break;
    }

    case YL_OBJECT:
    {
        yl_object* prototype;
        if ( a != yl_opinst::NOVAL )
        {
            if ( !( s[ a ].kind() & YLOBJ_IS_OBJECT ) )
            {
                throw yl_exception( "cannot inherit from non-object value" );
            }
            prototype = (yl_object*)s[ a ].heapobj();
        }
        else
        {
            prototype = nullptr;
        }
        s[ r ] = yl_current->new_object( prototype );
        break;
    }
    
    case YL_ARRAY:
    {
        s[ r ] = yl_value( YLOBJ_ARRAY, yl_array::alloc( c ) );
        break;
    }
    
    case YL_TABLE:
    {
        s[ r ] = yl_value( YLOBJ_TABLE, yl_table::alloc( c ) );
        break;
    }
    
    case YL_SUPER:
    {
        yl_object* object = superof( s[ a ] );
        if ( object )
            s[ r ] = yl_value( object->kind(), object );
        else
            s[ r ] = yl_value( YLOBJ_NULL, yl_null );
        break;
    }
    
    case YL_KEY:
    {
        yl_heapobj* key = values[ b ].get().as_heapobj();
        assert( key->kind() == YLOBJ_STRING );
        s[ r ] = keyerof( s[ a ] )->get_key( yl_value( YLOBJ_STRING, key ) );
        if ( s[ r ].kind() == YLOBJ_UNDEF )
        {
            yl_string* s = (yl_string*)key;
            throw yl_exception( "key not found '%s'", s->c_str() );
        }
        break;
    }
    
    case YL_INKEY:
    {
        s[ r ] = keyerof( s[ a ] )->get_key( s[ b ] );
        if ( s[ r ].kind() == YLOBJ_UNDEF )
        {
            throw yl_exception( "key not found" );
        }
        break;
    }
    
    case YL_INDEX:
    {
        if ( s[ a ].kind() == YLOBJ_ARRAY )
        {
            yl_array* array = (yl_array*)s[ a ].heapobj();
            if ( s[ b ].kind() != YLOBJ_NUMBER
                    || ! is_integer( s[ b ].number() ) )
            {
                throw yl_exception( "cannot index array with non-integer" );
            }
            s[ r ] = array->get_index( (size_t)s[ b ].number() );
        }
        else if ( s[ a ].kind() == YLOBJ_TABLE )
        {
            yl_table* table = (yl_table*)s[ a ].heapobj();
            s[ r ] = table->get_index( s[ b ] );
        }
        else
        {
            throw yl_exception( "unindexable object" );
        }
        if ( s[ r ].kind() == YLOBJ_UNDEF )
        {
            throw yl_exception( "index not found" );
        }
        break;
    }
    
    case YL_SETKEY:
    {
        if ( s[ r ].kind() & YLOBJ_IS_OBJECT )
        {
            yl_object* object = (yl_object*)s[ r ].heapobj();
            yl_heapobj* key = values[ b ].get().as_heapobj();
            assert( key->kind() == YLOBJ_STRING );
            object->set_key( yl_value( YLOBJ_STRING, key ), s[ a ] );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_SETINKEY:
    {
        if ( s[ r ].kind() & YLOBJ_IS_OBJECT )
        {
            yl_object* object = (yl_object*)s[ r ].heapobj();
            object->set_key( s[ a ], s[ b ] );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_SETINDEX:
    {
        if ( s[ r ].kind() == YLOBJ_ARRAY )
        {
            yl_array* array = (yl_array*)s[ r ].heapobj();
            if ( s[ a ].kind() != YLOBJ_NUMBER
                    || ! is_integer( s[ a ].number() ) )
            {
                throw yl_exception( "cannot index array with non-integer" );
            }
            array->set_index( (size_t)s[ a ].number(), s[ b ] );
        }
        else if ( s[ r ].kind() == YLOBJ_TABLE )
        {
            yl_table* table = (yl_table*)s[ r ].heapobj();
            table->set_index( s[ a ], s[ b ] );
        }
        else
        {
            throw yl_exception( "unindexable object" );
        }
        break;
    }
    
    case YL_DELKEY:
    {
        if ( s[ a ].kind() & YLOBJ_IS_OBJECT )
        {
            yl_object* object = (yl_object*)s[ a ].heapobj();
            yl_heapobj* key = values[ b ].get().as_heapobj();
            assert( key->kind() == YLOBJ_STRING );
            object->del_key( yl_value( YLOBJ_STRING, key ) );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_DELINKEY:
    {
        if ( s[ a ].kind() & YLOBJ_IS_OBJECT )
        {
            yl_object* object = (yl_object*)s[ a ].heapobj();
            object->del_key( s[ b ] );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_IN:
    {
        yl_value value = keyerof( s[ a ] )->get_key( s[ b ] );
        yl_heapobj* result = value.kind() != YLOBJ_UNDEF ? yl_true : yl_false;
        s[ r ] = yl_value( YLOBJ_BOOL, result );
        break;
    }
    
    case YL_IS:
    {
        bool result = false;
        yl_value u = s[ a ];
        yl_value proto = s[ b ];
        while ( true )
        {
            if ( u.kind() == proto.kind() && u.heapobj() == proto.heapobj() )
            {
                result = true;
                break;
            }
            
            yl_object* object = superof( u );
            if ( object )
            {
                u = yl_value( object->kind(), object );
            }
            else
            {
                result = false;
                break;
            }
        }
        s[ r ] = yl_value( YLOBJ_BOOL, result ? yl_true : yl_false );
        break;
    }

    case YL_APPEND:
    {
        if ( s[ a ].kind() == YLOBJ_ARRAY )
        {
            yl_array* array = (yl_array*)s[ a ].heapobj();
            array->append( s[ r ] );
        }
        else
        {
            throw yl_exception( "cannot append to non-array" );
        }
        break;
    }
    
    case YL_EXTEND:
    {
        if ( s[ b ].kind() != YLOBJ_ARRAY )
        {
            throw yl_exception( "cannot extend non-array" );
        }

        yl_array* array = (yl_array*)s[ b ].heapobj();
        if ( a == yl_opinst::MARK )
        {
            a = t->get_mark() - r;
        }
        array->extend( s + r, a );
        break;
    }
    
    case YL_UNPACK:
    {
        if ( s[ b ].kind() != YLOBJ_ARRAY )
        {
            throw yl_exception( "cannot unpack non-array" );
        }
        
        yl_array* array = (yl_array*)s[ b ].heapobj();
        unsigned count;
        if ( b != yl_opinst::MARK )
        {
            count = std::min( b, (unsigned)array->size() );
        }
        else
        {
            count = (unsigned)array->size();
            s = t->stack( fp, r + count );
        }
        
        for ( unsigned i = 0; i < count; ++i )
        {
            s[ r + i ] = array->get_index( i );
        }
        
        if ( b != yl_opinst::MARK )
        {
            for ( unsigned i = count; i < b; ++i )
            {
                s[ r + i ] = yl_value( YLOBJ_NULL, yl_null );
            }
        }
        else
        {
            t->set_mark( fp + r + count );
        }
        
        break;
    }
    
    
    /*
        Exception handling.
    */
    
    case YL_THROW:
    {
        assert( ! "exception handling unimplemented" );
        break;
    }
    
    case YL_EXCEPT:
    {
        assert( ! "exception handling unimplemented" );
        break;
    }
    
    case YL_UNWIND:
    {
        // TODO.
        break;
    }


    case YL_UPLOCAL:
    case YL_UPUPVAL:
    {
        assert( ! "invalid code" );
        break;
    }
    
    }

    }
    
}



