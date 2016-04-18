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
#include "yl_upval.h"
#include "yl_function.h"
#include "yl_program.h"




//#define TRACE
#ifdef TRACE
static void trace( yl_cothread* t, const yl_stackframe& f, unsigned ip );
#endif



/*
    yl_invoke
*/


void yl_invoke( yl_callframe& xf )
{
    // Enter interpreter.
    yl_interp( xf._cothread, xf._base, xf._size, yl_opinst::MARK );
    
    // Recover number of results and update callframe.
    xf._size = xf._cothread->get_top() - xf._base;
}




/*
    Helpers.
*/

static inline double cast_number( yl_value value )
{
    if ( ! value.is_number() )
    {
        throw yl_exception( "expected number" );
    }
    
    return value.number();
}

static inline yl_string* cast_string( yl_value value )
{
    if ( ! value.is( YLOBJ_STRING ) )
    {
        throw yl_exception( "expected string" );
    }
    
    return (yl_string*)value.gcobject();
}

static inline int compare_strings( yl_value a, yl_value b )
{
    yl_string* sa = (yl_string*)a.gcobject();
    yl_string* sb = (yl_string*)b.gcobject();
    
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

static inline yl_object* superof( yl_value v )
{
    if ( v.is_object() )
    {
        yl_object* object = (yl_object*)v.gcobject();
        return object->superof();
    }
    else
    {
        return yl_current->superof( v );
    }
}

static inline yl_object* keyerof( yl_value v )
{
    if ( v.is_object() )
    {
        return (yl_object*)v.gcobject();
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


static void build_frame( yl_stackframe* frame, yl_cothread* t, unsigned sp, unsigned acount )
{
    // If acount is MARK then the mark points after the last argument.
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
    
    yl_value* s = t->stack_peek( sp, acount );
    if ( ! s[ 0 ].is( YLOBJ_FUNCOBJ ) )
    {
        throw yl_exception( "cannot call non-function" );
    }

    yl_funcobj* f = (yl_funcobj*)s[ 0 ].gcobject();
    yl_program* p = f->program();
    
    // Get acount that the function was expecting (including function slot).
    unsigned pcount = 1 + p->paramcount();
    
    unsigned fp;
    if ( p->varargs() && acount > pcount )
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

        fp = sp + ( acount - pcount );
        reverse( s, pcount );
        reverse( s + pcount, acount - pcount );
        reverse( s, acount );
        
    }
    else
    {
        // There are no varargs, use the frame in-place.
        fp = sp;
    }
    
    
    // Allocate stack.
    s = t->stack_alloc( fp, p->stackcount() );


    // Set missing parameters to null.
    assert( pcount <= p->stackcount() );
    for ( unsigned arg = acount; arg < pcount; ++arg )
    {
        s[ arg ] = yl_null;
    }


    // And return a stackframe.
    frame->funcobj   = f;
    frame->ip        = 0;
    frame->sp        = sp;
    frame->fp        = fp;
}



/*
    Main interpreter loop.
*/

void yl_interp( yl_cothread* t, unsigned sp, unsigned acount, unsigned rcount )
{
    // Ensure that the cothread is unlocked.
    assert( yl_current->get_cothread() == t );
    
    
    // Build frame for this call.
    yl_stackframe frame;
    build_frame( &frame, t, sp, acount );
    frame.locup_base = t->get_locup_base();
    frame.iters_base = t->get_iters_base();
    frame.rcount = rcount;
    frame.kind = YL_FRAME_NATIVE;

    // Cache values.
    yl_program*         p       = frame.funcobj->program();
    const yl_heapval*   values  = p->values();
    const yl_opinst*    ops     = p->ops();
    yl_ilcache*         ilcache = p->ilcache();
    
    unsigned            ip      = frame.ip;
    unsigned            fp      = frame.fp;
    yl_value*           s       = t->stack_peek( fp, p->stackcount() );
    
    
    // Main instruction dispatch loop.
    while ( true )
    {

#ifdef TRACE
    trace( t, frame, ip );
#endif
        
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
        s[ r ] = yl_null;
        break;
    }
    
    case YL_BOOL:
    {
        s[ r ] = a ? yl_true : yl_false;
        break;
    }
    
    case YL_NUMBER:
    {
        s[ r ] = values[ c ].get();
        assert( s[ r ].is_number() );
        break;
    }
    
    case YL_STRING:
    {
        s[ r ] = values[ c ].get();
        assert( s[ r ].is( YLOBJ_STRING ) );
        break;
    }

    case YL_GLOBAL:
    {
        assert( ops[ ip ].opcode() == YL_ILCACHE );
        yl_ilcache* ilc = ilcache + ops[ ip ].c();
        ip += 1;
    
        yl_value value = values[ b ].get();
        assert( value.is( YLOBJ_STRING ) );
        yl_string* key = (yl_string*)value.gcobject();
        s[ r ] = yl_current->globals()->get_key( key, ilc );
        if ( s[ r ].is_undef() )
        {
            throw yl_exception( "unknown global '%s'", key->c_str() );
        }
        break;
    }
    
    case YL_SETGLOBAL:
    {
        assert( ops[ ip ].opcode() == YL_ILCACHE );
        yl_ilcache* ilc = ilcache + ops[ ip ].c();
        ip += 1;
    
        yl_value value = values[ b ].get();
        assert( value.is( YLOBJ_STRING ) );
        yl_string* key = (yl_string*)value.gcobject();
        yl_current->globals()->set_key( key, s[ r ], ilc );
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
        s[ r ] = yl_string::concat( sa, sb ).get();
        break;
    }
    
    case YL_EQ:
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = isequal ? yl_true : yl_false;
        break;
    }
    
    case YL_NE:
    {
        bool isequal = equal( s[ a ], s[ b ] );
        s[ r ] = isequal ? yl_false : yl_true;
        break;
    }
    
    case YL_LT:
    {
        bool islt;
        if ( s[ a ].is_number() && s[ b ].is_number() )
        {
            islt = ( s[ a ].number() < s[ b ].number() );
        }
        else if ( s[ a ].is( YLOBJ_STRING ) && s[ b ].is( YLOBJ_STRING ) )
        {
            islt = ( compare_strings( s[ a ], s[ b ] ) < 0 );
        }
        else
        {
            throw yl_exception( "invalid type for comparison" );
        }
        s[ r ] = islt ? yl_true : yl_false;
        break;
    }
    
    case YL_GT:
    {
        bool isgt;
        if ( s[ a ].is_number() && s[ b ].is_number() )
        {
            isgt = ( s[ a ].number() > s[ b ].number() );
        }
        else if ( s[ a ].is( YLOBJ_STRING ) && s[ b ].is( YLOBJ_STRING ) )
        {
            isgt = ( compare_strings( s[ a ], s[ b ] ) > 0 );
        }
        else
        {
            throw yl_exception( "invalid type for comparison" );
        }
        s[ r ] = isgt ? yl_true : yl_false;
        break;
    }
    
    case YL_LE:
    {
        bool isle;
        if ( s[ a ].is_number() && s[ b ].is_number() )
        {
            isle = ( s[ a ].number() <= s[ b ].number() );
        }
        else if ( s[ a ].is( YLOBJ_STRING ) && s[ b ].is( YLOBJ_STRING ) )
        {
            isle = ( compare_strings( s[ a ], s[ b ] ) <= 0 );
        }
        else
        {
            throw yl_exception( "invalid type for comparison" );
        }
        s[ r ] = isle ? yl_true : yl_false;
        break;
    }
    
    case YL_GE:
    {
        bool isge;
        if ( s[ a ].is_number() && s[ b ].is_number() )
        {
            isge = ( s[ a ].number() >= s[ b ].number() );
        }
        else if ( s[ a ].is( YLOBJ_STRING ) && s[ b ].is( YLOBJ_STRING ) )
        {
            isge = ( compare_strings( s[ a ], s[ b ] ) >= 0 );
        }
        else
        {
            throw yl_exception( "invalid type for comparison" );
        }
        s[ r ] = isge ? yl_true : yl_false;
        break;
    }
    
    case YL_LNOT:
    {
        bool istrue = test( s[ a ] );
        s[ r ] = istrue ? yl_false : yl_true;
        break;
    }
    
    case YL_LXOR:
    {
        bool result = test( s[ a ] ) != test( s[ b ] );
        s[ r ] = result ? yl_true : yl_false;
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
        unsigned bcount;
        if ( b != yl_opinst::MARK )
        {
            count = std::min( b, fp - sp );
            bcount = b;
        }
        else
        {
            count = bcount = fp - sp;
        }
        s = t->stack_mark( fp, r + bcount, p->stackcount() );
        
        yl_value* varargs = t->stack_peek( sp, fp - sp );
        for ( unsigned i = 0; i < count; ++i )
        {
            s[ r + i ] = varargs[ i ];
        }
        
        for ( unsigned i = count; i < bcount; ++i )
        {
            s[ r + i ] = yl_null;
        }
        break;
    }
    
    case YL_FUNCTION:
    {
        // Get program object.
        yl_value value = values[ c ].get();
        assert( value.is( YLOBJ_PROGRAM ) );
        yl_program* program = (yl_program*)value.gcobject();
        
        // Create funcobj.
        yl_rootref< yl_funcobj > funcobj = yl_funcobj::alloc( program );

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
                yl_upval*& locup = t->locup( frame.locup_base, a );
                if ( ! locup )
                {
                    locup = yl_gcnew< yl_upval >( t, fp + b ).get();
                }
                funcobj->set_upval( r, locup );
                break;
            }
            
            case YL_UPUPVAL:
            {
                funcobj->set_upval( r, frame.funcobj->get_upval( a ) );
                break;
            }
            
            default:
            {
                assert( ! "invalid code" );
                break;
            }
            }
        }
        
        s[ r ] = funcobj.get();
        break;
    }
    
    case YL_CALL:
    case YL_YCALL:
    {
        if ( s[ r ].is( YLOBJ_FUNCOBJ ) )
        {
            yl_funcobj* funcobj = (yl_funcobj*)s[ r ].gcobject();
            if ( op->opcode() == YL_YCALL || ! funcobj->program()->coroutine() )
            {
                // Push current stack frame.
                frame.ip = ip;
                t->push_frame( frame );

                // Build frame for this call.
                build_frame( &frame, t, fp + r, a );
                frame.locup_base += p->locupcount();
                frame.iters_base += p->iterscount();
                frame.rcount = b;
                frame.kind = YL_FRAME_LOCAL;

                // Recache values.
                p       = frame.funcobj->program();
                values  = p->values();
                ops     = p->ops();
                ilcache = p->ilcache();
                
                ip      = frame.ip;
                sp      = frame.sp;
                fp      = frame.fp;
                s       = t->stack_peek( fp, p->stackcount() );
            
            }
            else
            {
                // Call of coroutine which constructs a new cothread.
                yl_rootref< yl_cothread > cothread = yl_gcnew< yl_cothread >();
                
                if ( a == yl_opinst::MARK )
                {
                    a = t->get_mark() - ( fp + r );
                }
                
                // Copy arguments into cothread.
                yl_value* c = cothread->stack_mark( 0, a, a );
                for ( unsigned i = 0; i < a; ++i )
                {
                    c[ i ] = s[ r + i ];
                }
            
                unsigned bcount;
                if ( b != yl_opinst::MARK )
                {
                    bcount = b;
                }
                else
                {
                    bcount = 1;
                }
                yl_value* s = t->stack_mark( fp, r + bcount, p->stackcount() );
                
                if ( bcount )
                {
                    s[ r ] = cothread.get();
                    for ( unsigned i = 1; i < bcount; ++i )
                    {
                        s[ r + i ] = yl_null;
                    }
                }
                
                // Create open call frame.
                yl_stackframe yopen;
                memset( &yopen, 0, sizeof( yl_stackframe ) );
                yopen.kind = YL_FRAME_YOPEN;
                cothread->push_frame( yopen );
            }

        }
        else if ( s[ r ].is( YLOBJ_THUNKOBJ ) )
        {
            if ( a == yl_opinst::MARK )
            {
                a = t->get_mark() - ( fp + r );
            }
        
            // Native thunk.
            yl_thunkobj* thunkobj = (yl_thunkobj*)s[ r ].gcobject();
            t->stack_trim( fp + r, a );
            yl_callframe xf( t, fp + r, a );
            thunkobj->thunk()( xf );
            
            // Get number of results.
            unsigned count;
            unsigned bcount;
            if ( b != yl_opinst::MARK )
            {
                count = std::min( b, (unsigned)xf.size() );
                bcount = b;
            }
            else
            {
                count = bcount = (unsigned)xf.size();
            }
            
            s = t->stack_mark( fp, r + bcount, p->stackcount() );

            for ( unsigned i = count; i < bcount; ++i )
            {
                s[ r + i ] = yl_null;
            }
        }
        else if ( op->opcode() == YL_CALL && s[ r ].is( YLOBJ_COTHREAD ) )
        {
            yl_cothread* cothread = (yl_cothread*)s[ r ].gcobject();
            
            // Get arguments.
            if ( a == yl_opinst::MARK )
            {
                a = t->get_mark() - ( fp + r );
            }

            // Push current stack frame.
            frame.ip = ip;
            t->push_frame( frame );

            // Switch cothread.
            yl_current->push_cothread( cothread );
            t = cothread;

            if ( ! t->has_frames() )
            {
                throw yl_exception( "attempt to resume finished cothread" );
            }

            // Resume cothread.
            frame = t->pop_frame();

            if ( frame.kind != YL_FRAME_YOPEN )
            {
                // Recache values.
                p       = frame.funcobj->program();
                values  = p->values();
                ops     = p->ops();
                ilcache = p->ilcache();

                ip      = frame.ip;
                sp      = frame.sp;
                fp      = frame.fp;
                
                // Must resume at a YIELD instruction.
                op = ops + ip - 1;
                assert( op->opcode() == YL_YIELD );
                
                // Copy arguments into the new cothread.
                unsigned count;
                unsigned bcount;
                if ( op->b() != yl_opinst::MARK )
                {
                    count = std::min( op->b(), a - 1 );
                    bcount = op->b();
                }
                else
                {
                    assert( a > 0 );
                    count = bcount = a - 1;
                }
                
                yl_value* c = t->stack_mark( fp, op->r() + bcount, p->stackcount() );

                for ( unsigned i = 0; i < count; ++i )
                {
                    c[ op->r() + i ] = s[ r + 1 + i ];
                }
                
                for ( unsigned i = count; i < bcount; ++i )
                {
                    c[ op->r() + i ] = yl_null;
                }
                
                s = c;

            }
            else
            {
                // First call, overwrite arguments.
                if ( a > 1 )
                {
                    yl_value* c = t->stack_mark( 0, a, a );
                    for ( unsigned i = 1; i < a; ++i )
                    {
                        c[ i ] = s[ r + i ];
                    }
                }
                
                // Construct call frame.
                build_frame( &frame, t, 0, cothread->get_mark() );
                frame.locup_base = 0;
                frame.iters_base = 0;
                frame.rcount = 0;
                frame.kind = YL_FRAME_YCALL;
                
                // Recache values.
                p       = frame.funcobj->program();
                values  = p->values();
                ops     = p->ops();
                ilcache = p->ilcache();

                ip      = frame.ip;
                sp      = frame.sp;
                fp      = frame.fp;

                s       = t->stack_peek( fp, p->stackcount() );
            }

        }
        else
        {
            throw yl_exception( "attempt to call an uncallable object" );
        }
        break;
    }
    
    case YL_YIELD:
    case YL_RETURN:
    {
        if ( a == yl_opinst::MARK )
        {
            a = t->get_mark() - ( fp + r );
        }

        if ( op->opcode() == YL_RETURN && frame.kind != YL_FRAME_YCALL )
        {
            // Perform copy down to sp.
            unsigned count;
            unsigned bcount;
            b = frame.rcount;
            if ( b != yl_opinst::MARK )
            {
                count = std::min( b, a );
                bcount = b;
            }
            else
            {
                count = bcount = a;
            }

            assert( sp + count <= fp + r + count );
            yl_value* z = t->stack_peek( sp, count );
            for ( unsigned i = 0; i < count; ++i )
            {
                z[ i ] = s[ r + i ];
            }
            
            // Add nulls.
            s = t->stack_peek( sp, bcount );
            for ( unsigned i = count; i < bcount; ++i )
            {
                s[ i ] = yl_null;
            }
            
            if ( frame.kind == YL_FRAME_LOCAL )
            {
                // Return on same cothread.
                unsigned mark = sp + bcount;
            
                // Pop stack frame.
                frame = t->pop_frame();
                
                // Cache values.
                p       = frame.funcobj->program();
                values  = p->values();
                ops     = p->ops();
                ilcache = p->ilcache();
                
                ip      = frame.ip;
                sp      = frame.sp;
                fp      = frame.fp;
                
                // Stack mark.
                assert( mark >= fp );
                s = t->stack_mark( fp, mark - fp, p->stackcount() );
                break;
            }
            else if ( frame.kind == YL_FRAME_NATIVE )
            {
                // Return native call.
                t->stack_trim( sp, bcount );
                return;
            }
            else
            {
                assert( ! "invalid call frame" );
            }
            
        }
        else
        {
            
            if ( op->opcode() == YL_YIELD )
            {
                // Push current stack frame.
                frame.ip = ip;
                t->push_frame( frame );
            }
            
            // Switch cothread.
            yl_rootref< yl_cothread > cothread = yl_current->pop_cothread();
            assert( t == cothread.get() );
            t = yl_current->get_cothread();
        
            // Resume.
            frame = t->pop_frame();
        
            // Recache values.
            p       = frame.funcobj->program();
            values  = p->values();
            ops     = p->ops();
            ilcache = p->ilcache();

            ip      = frame.ip;
            sp      = frame.sp;
            fp      = frame.fp;
            
            // Must resume at a CALL instruction.
            op = ops + ip - 1;
            assert( op->opcode() == YL_CALL );
            
            // Copy arguments out into calling cothread.
            unsigned count;
            unsigned bcount;
            if ( op->b() != yl_opinst::MARK )
            {
                count = std::min( op->b(), a );
                bcount = op->b();
            }
            else
            {
                assert( a > 0 );
                count = bcount = a;
            }
            
            yl_value* c = t->stack_mark( fp, op->r() + bcount, p->stackcount() );
            
            for ( unsigned i = 0; i < count; ++i )
            {
                c[ op->r() + i ] = s[ r + i ];
            }
            
            for ( unsigned i = count; i < bcount; ++i )
            {
                c[ op->r() + i ] = yl_null;
            }
            
            s = c;
            
        }
        
        break;
    }
    
    case YL_ITER:
    {
        t->iterator( frame.iters_base, r ).open_vals( s[ a ] );
        break;
    }
    
    case YL_ITERKEY:
    {
        t->iterator( frame.iters_base, r ).open_keys( s[ a ] );
        break;
    }
    
    case YL_NEXT1:
    {
        t->iterator( frame.iters_base, a ).next1( &s[ r ] );
        break;
    }
    
    case YL_NEXT2:
    {
        t->iterator( frame.iters_base, a ).next2( &s[ r ], &s[ b ] );
        break;
    }
    
    case YL_NEXT:
    {
        // Get next set of values from iterator.
        yl_value vspace[ 2 ];
        yl_iternext next = t->iterator( frame.iters_base, a ).next( vspace );
        
        // Place values in correct registers.
        unsigned count;
        unsigned bcount;
        if ( b != yl_opinst::MARK )
        {
            count = std::min( b, (unsigned)next.vcount );
            bcount = b;
        }
        else
        {
            count = bcount = (unsigned)next.vcount;
        }
        yl_value* s = t->stack_mark( fp, r + bcount, p->stackcount() );
        
        for ( unsigned i = 0; i < count; ++i )
        {
            s[ r + i ] = next.values[ i ];
        }
        
        for ( unsigned i = count; i < bcount; ++i )
        {
            s[ r + i ] = yl_null;
        }
        break;
    }

    case YL_JMPV:
    {
        if ( t->iterator( frame.iters_base, r ).has_values() )
        {
            ip += j;
        }
        break;
    }
    
    case YL_JMPN:
    {
        if ( ! t->iterator( frame.iters_base, r ).has_values() )
        {
            ip += j;
        }
        break;
    }

    case YL_GETUP:
    {
        yl_upval* upval = frame.funcobj->get_upval( a );
        s[ r ] = upval->get_value();
        break;
    }
    
    case YL_SETUP:
    {
        yl_upval* upval = frame.funcobj->get_upval( a );
        upval->set_value( s[ r ] );
        break;
    }

    case YL_CLOSE:
    {
        t->close_locup( frame.locup_base, a );
        t->close_iterator( frame.iters_base, b );
        break;
    }

    case YL_OBJECT:
    {
        yl_object* prototype;
        if ( a != yl_opinst::NOVAL )
        {
            if ( ! s[ a ].is_object() )
            {
                throw yl_exception( "cannot inherit from non-object value" );
            }
            prototype = (yl_object*)s[ a ].gcobject();
        }
        else
        {
            prototype = yl_current->proto_object();
        }
        s[ r ] = yl_current->new_object( prototype ).get();
        break;
    }
    
    case YL_ARRAY:
    {
        s[ r ] = yl_gcnew< yl_array >( c ).get();
        break;
    }
    
    case YL_TABLE:
    {
        s[ r ] = yl_gcnew< yl_table >( c ).get();
        break;
    }
    
    case YL_SUPER:
    {
        yl_object* object = superof( s[ a ] );
        if ( object )
            s[ r ] = object;
        else
            s[ r ] = yl_null;
        break;
    }
    
    case YL_KEY:
    {
        assert( ops[ ip ].opcode() == YL_ILCACHE );
        yl_ilcache* ilc = ilcache + ops[ ip ].c();
        ip += 1;
    
        yl_value value = values[ b ].get();
        assert( value.is( YLOBJ_STRING ) );
        yl_string* string = (yl_string*)value.gcobject();
        s[ r ] = keyerof( s[ a ] )->get_key( string, ilc );
        if ( s[ r ].is_undef() )
        {
            throw yl_exception( "key not found '%s'", string->c_str() );
        }
        break;
    }
    
    case YL_INKEY:
    {
        if ( ! s[ b ].is( YLOBJ_STRING ) )
        {
            throw yl_exception( "object index must be a string" );
        }
        yl_string* string = (yl_string*)s[ b ].gcobject();
        s[ r ] = keyerof( s[ a ] )->get_key( string->symbol().get() );
        if ( s[ r ].is_undef() )
        {
            throw yl_exception( "key not found '%s'", string->c_str() );
        }
        break;
    }
    
    case YL_INDEX:
    {
        if ( s[ a ].is( YLOBJ_ARRAY ) )
        {
            yl_array* array = (yl_array*)s[ a ].gcobject();
            if ( ! s[ b ].is_number() )
            {
                throw yl_exception( "invalid array index" );
            }
            double index = s[ b ].number();
            if ( ! is_integer( index ) || index < 0 || index >= array->length() )
            {
                throw yl_exception( "invalid array index" );
            }
            s[ r ] = array->get_index( (size_t)index );
        }
        else if ( s[ a ].is( YLOBJ_TABLE ) )
        {
            yl_table* table = (yl_table*)s[ a ].gcobject();
            s[ r ] = table->get_index( s[ b ] );
            if ( s[ r ].is_undef() )
            {
                throw yl_exception( "index not found" );
            }
        }
        else
        {
            throw yl_exception( "unindexable object" );
        }
        break;
    }
    
    case YL_SETKEY:
    {
        assert( ops[ ip ].opcode() == YL_ILCACHE );
        yl_ilcache* ilc = ilcache + ops[ ip ].c();
        ip += 1;
    
        if ( s[ r ].is_object() )
        {
            yl_object* object = (yl_object*)s[ r ].gcobject();
            yl_value value = values[ b ].get();
            assert( value.is( YLOBJ_STRING ) );
            yl_string* string = (yl_string*)value.gcobject();
            object->set_key( string, s[ a ], ilc );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_SETINKEY:
    {
        if ( s[ r ].is_object() )
        {
            yl_object* object = (yl_object*)s[ r ].gcobject();
            if ( ! s[ a ].is( YLOBJ_STRING ) )
            {
                throw yl_exception( "object index must be a string" );
            }
            yl_string* string = (yl_string*)s[ a ].gcobject();
            object->set_key( string->symbol().get(), s[ b ] );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_SETINDEX:
    {
        if ( s[ r ].is( YLOBJ_ARRAY ) )
        {
            yl_array* array = (yl_array*)s[ r ].gcobject();
            if ( ! s[ a ].is_number() )
            {
                throw yl_exception( "invalid array index" );
            }
            double index = s[ a ].number();
            if ( ! is_integer( index ) || index < 0 || index >= array->length() )
            {
                throw yl_exception( "invalid array index" );
            }
            array->set_index( (size_t)index, s[ b ] );
        }
        else if ( s[ r ].is( YLOBJ_TABLE ) )
        {
            yl_table* table = (yl_table*)s[ r ].gcobject();
            table->set_index( s[ a ], s[ b ] );
        }
        else
        {
            throw yl_exception( "unindexable object" );
        }
        break;
    }
    
    case YL_RESPONDS:
    {
        if ( ! s[ b ].is( YLOBJ_STRING ) )
        {
            throw yl_exception( "object index must be a string" );
        }
        yl_string* string = (yl_string*)s[ b ].gcobject();
        yl_value value = keyerof( s[ a ] )->get_key( string->symbol().get() );
        s[ r ] = value.is_undef() ? yl_true : yl_false;
        break;
    }
    
    case YL_DELKEY:
    {
        if ( s[ a ].is_object() )
        {
            yl_object* object = (yl_object*)s[ a ].gcobject();
            yl_value value = values[ b ].get();
            assert( value.is( YLOBJ_STRING ) );
            yl_string* string = (yl_string*)value.gcobject();
            object->del_key( string );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_DELINKEY:
    {
        if ( s[ a ].is_object() )
        {
            yl_object* object = (yl_object*)s[ a ].gcobject();
            if ( ! s[ b ].is( YLOBJ_STRING ) )
            {
                throw yl_exception( "object index must be a string" );
            }
            yl_string* string = (yl_string*)s[ b ].gcobject();
            object->del_key( string->symbol().get() );
        }
        else
        {
            throw yl_exception( "unkeyable object" );
        }
        break;
    }
    
    case YL_IS:
    {
        bool result = false;
        if ( s[ b ].is_object() )
        {
            yl_object* proto = (yl_object*)s[ b ].gcobject();
            for ( yl_object* object = keyerof( s[ a ] );
                            object != nullptr; object = object->superof() )
            {
                if ( object == proto )
                {
                    result = true;
                }
            }
        }
        else
        {
            result = equal( s[ a ], s[ b ] );
        }
        s[ r ] = result ? yl_true : yl_false;
        break;
    }

    case YL_APPEND:
    {
        if ( s[ a ].is( YLOBJ_ARRAY ) )
        {
            yl_array* array = (yl_array*)s[ a ].gcobject();
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
        if ( ! s[ b ].is( YLOBJ_ARRAY ) )
        {
            throw yl_exception( "cannot extend non-array" );
        }

        yl_array* array = (yl_array*)s[ b ].gcobject();
        if ( a == yl_opinst::MARK )
        {
            a = t->get_mark() - ( fp + r );
        }
        array->extend( s + r, a );
        break;
    }
    
    case YL_UNPACK:
    {
        if ( ! s[ a ].is( YLOBJ_ARRAY ) )
        {
            throw yl_exception( "cannot unpack non-array" );
        }
        
        yl_array* array = (yl_array*)s[ a ].gcobject();
        
        unsigned count;
        unsigned bcount;
        if ( b != yl_opinst::MARK )
        {
            count = std::min( b, (unsigned)array->length() );
            bcount = b;
        }
        else
        {
            count = bcount = (unsigned)array->length();
        }
        s = t->stack_mark( fp, r + bcount, p->stackcount() );
        
        for ( unsigned i = 0; i < count; ++i )
        {
            s[ r + i ] = array->get_index( i );
        }
        
        for ( unsigned i = count; i < bcount; ++i )
        {
            s[ r + i ] = yl_null;
        }
        break;
    }
    
    /*
        Exception handling.
    */
    
    case YL_THROW:
    {
        yl_current->throw_exception( s[ r ] );
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
    case YL_ILCACHE:
    {
        assert( ! "invalid code" );
        break;
    }
    
    }

    }
    
}




#ifdef TRACE


static void print_string( const yl_value& v )
{
    const char* s = ( (yl_string*)v.gcobject() )->c_str();
    for ( ; *s; ++s )
    {
        if ( *s >= 0x20 && *s < 0x7F )
        {
            printf( "%c", *s );
        }
        else
        {
            printf( "\\x%02X", *s );
        }
    }
}


static void print_value( const yl_value& v )
{
    if ( v.is_number() )
    {
        printf( "%g", v.number() );
    }
    else
    {
        switch ( v.kind() )
        {
        case YLOBJ_OBJECT:      printf( "object(%p)", v.gcobject() );              break;
        case YLOBJ_EXPOBJ:      printf( "expobj(%p)", v.gcobject() );              break;
        case YLOBJ_ARRAY:       printf( "array(%p)", v.gcobject() );               break;
        case YLOBJ_TABLE:       printf( "table(%p)", v.gcobject() );               break;
        case YLOBJ_STRING:      print_string( v );                                 break;
        case YLOBJ_FUNCOBJ:     printf( "%s", ( (yl_funcobj*)v.gcobject() )->program()->name() );  break;
        case YLOBJ_THUNKOBJ:    printf( "thunkobj(%p)", v.gcobject() );            break;
        case YLOBJ_COTHREAD:    printf( "cothread(%p)", v.gcobject() );            break;
        case YLOBJ_BOOLEAN:     printf( "%s", v.is_true() ? "true" : "false" );    break;
        case YLOBJ_NULLUNDEF:   printf( "%s", v.is_null() ? "null" : "undef" );    break;
        }
    }
    printf( "\n" );
}


static void trace( yl_cothread* t, const yl_stackframe& f, unsigned ip )
{
    unsigned upcount = f.funcobj->program()->upcount();
    for ( unsigned i = 0; i < upcount; ++i )
    {
        yl_upval* uv = f.funcobj->get_upval( i );
        if ( uv )
        {
            printf( "    ^%2u ", i );
            print_value( uv->get_value() );
        }
    }

    unsigned top = t->get_top() - f.fp;
    yl_value* s = t->stack_peek( f.fp, top );
    for ( unsigned i = 0; i < top; ++i )
    {
        printf( "     %2u ", i );
        print_value( s[ i ] );
    }

    printf( "%p %-20s:", t, f.funcobj->program()->name() );
    ylop_print( f.ip, f.funcobj->program()->ops() + ip );
    
}


#endif

