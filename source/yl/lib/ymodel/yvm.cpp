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
#include "ythunk.h"



/*
    ystandin
*/

ystandin* ystandin::alloc( kind kind )
{
    void* p = malloc( sizeof( ystandin ) );
    return new ( p ) ystandin( &metatype, kind );
}

ystandin::ystandin( ymetatype* metatype, kind kind )
    :   yobject( metatype )
    ,   skind( kind )
{
}


bool ystandin::value_is_this( yvalue value )
{
    switch ( skind )
    {
    case BOOLEAN:
        return value.is_boolean();
    case NUMBER:
        return value.is_number();
    case STRING:
        return value.is_string();
    case FUNCTION:
        return value.is_thunk() || value.is< yfunction >();
    }
}


ymetatype ystandin::metatype = { nullptr, "prototype" };








/*
    ystack
*/


ystack::ystack()
    :   up( 0 )
    ,   mark( 0 )
{
}


void ystack::ensure_stack( size_t size )
{
    if ( stack.size() < size )
    {
        stack.insert( stack.end(), size - stack.size(), yvalue::yundefined );
    }
}

void ystack::ensure_upvals( size_t size )
{
    if ( upvals.size() < size )
    {
        upvals.insert( upvals.end(), size - upvals.size(), nullptr );
    }
}




/*
    yiterator
*/


yiterator::yiterator( yexpand* expand )
    :   kind( KEYS )
    ,   index( 0 )
{
    assert( ! "not implemented" );
}

yiterator::yiterator( yarray* array )
    :   kind( ARRAY_INDEX )
    ,   index( -1 )
    ,   array( array )
    ,   values( array->values )
{
}

yiterator::yiterator( ytable* table )
    :   kind( TABLE_INDEX )
    ,   index( -1 )
    ,   table( table )
    ,   keyvals( table->table.keyvals )
{
}


bool yiterator::valid()
{
    return index != (size_t)-1;
}

void yiterator::next1( yvalue* a )
{
    switch ( kind )
    {
    case KEYS:
    {
        yvalue ignore;
        next_key( a, &ignore );
        break;
    }
    
    case ARRAY_INDEX:
    {
        next_array( a );
        break;
    }
    
    case TABLE_INDEX:
    {
        yvalue ignore;
        next_table( a, &ignore );
        break;
    }
    
    }
}

void yiterator::next2( yvalue* a, yvalue* b )
{
    switch ( kind )
    {
    case KEYS:
    {
        next_key( a, b );
        break;
    }
    
    case ARRAY_INDEX:
    {
        next_array( a );
        *b = yvalue::ynull;
        break;
    }
        
    case TABLE_INDEX:
    {
        next_table( a, b );
        break;
    }
    
    }
}

void yiterator::next( ystack* stack, size_t sp, unsigned count )
{
    assert( count > 2 );
    size_t valcount = 0;

    switch ( kind )
    {
    case KEYS:
    {
        stack->ensure_stack( sp + 2 );
        yvalue* s = stack->stack.data();
        next_key( s + sp, s + sp + 1 );
        valcount = 2;
        break;
    }
    
    case ARRAY_INDEX:
    {
        stack->ensure_stack( sp + 1 );
        yvalue* s = stack->stack.data();
        next_array( s + sp );
        valcount = 1;
        break;
    }
    
    case TABLE_INDEX:
    {
        stack->ensure_stack( sp + 2 );
        yvalue* s = stack->stack.data();
        next_table( s + sp, s + sp + 1 );
        valcount = 2;
        break;
    }
    
    }

    if ( count != Y_MARK )
    {
        for ( size_t i = valcount; i < count; ++i )
        {
            stack->stack[ sp + i ] = yvalue::ynull;
        }
    }
    else
    {
        stack->mark = sp + valcount;
    }

}


void yiterator::next_key( yvalue* a, yvalue* b )
{
    // TODO: Actually do this.
    *a = yvalue::yundefined;
    *b = yvalue::yundefined;
}

void yiterator::next_array( yvalue* a )
{
    index += 1;
    if ( values && values == array->values && index < array->length() )
    {
        *a = values->get( index );
        return;
    }

    *a = yvalue::yundefined;
    index = (size_t)-1;
}

void yiterator::next_table( yvalue* a, yvalue* b )
{
    if ( keyvals && keyvals == table->table.keyvals )
    {
        for ( index += 1; index < keyvals->size(); ++index )
        {
            ykeyval< yvalue, yvalue >& kv = keyvals->at( index );
            if ( kv.next != nullptr )
            {
                *a = kv.key;
                *b = kv.value;
                return;
            }
        }
    }
    
    *a = yvalue::yundefined;
    *b = yvalue::yundefined;
    index = (size_t)-1;
}






/*
    helper functions
*/

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





/*
    yexec
*/


void yexec( size_t sp, unsigned incount, unsigned outcount )
{
    ystack* stack = yscope::scope->stack;


    /*
        If argcount is Y_MARK, then the mark points after the top argument.
    */
    
    if ( incount == Y_MARK )
    {
        incount = (unsigned)( stack->mark - sp );
    }


    /*
        Stack frames on entry look like:
        
                      sp -> function
                            argument
                            argument
            sp + incount ->
    */
    
    yfunction* function = stack->stack[ sp ].as< yfunction >();
    yrecipe* recipe = function->recipe();
    ymodule* module = recipe->module();

    size_t fp = sp;
    
    if ( incount <= recipe->param_count() )
    {
        // Set missing arguments to null.
        for ( unsigned i = incount; i <= recipe->param_count(); ++i )
        {
            stack->stack[ sp + i ] = yvalue::ynull;
        }
    }
    else if ( recipe->is_varargs() )
    {
        // Preserve existing stack frame so that varargs are accessible.
        // Reconstruct a stack frame with the correct arguments above it..
        fp = sp + incount;
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
        s[ i.r() ] = yvalue::ynull;
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
        s[ i.r() ] = yexpand::in( s[ i.a() ].as_string(), s[ i.b() ].as_expand() );
        break;
        
    case Y_IS:
    {
        yvalue value = s[ i.a() ];
        yvalue prototype = s[ i.b() ];
        if ( prototype.is_expand() )
        {
            yexpand* exproto = prototype.as_expand();
            if ( exproto == yscope::scope->model->object_proto() )
                s[ i.r() ] = yvalue::ytrue;
            else if ( value.is_expand() )
                s[ i.r() ] = yexpand::is( value.as_expand(), exproto );
            else
                s[ i.r() ] = yvalue::yfalse;
        }
        else if ( prototype.is< ystandin >() )
        {
            s[ i.r() ] = prototype.as< ystandin >()->value_is_this( value );
        }
        else
        {
            s[ i.r() ] = yvalue::yfalse;
        }
        break;
    }
 
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

    case Y_NEWNU:
        assert( u[ i.c() ] == nullptr );
        u[ i.c() ] = yupval::alloc( s[ i.r() ] );
        break;
    case Y_SETNU:
        u[ i.c() ]->set( s[ i.r() ] );
        break;
    case Y_REFNU:
        s[ i.r() ] = u[ i.c() ]->get();
        break;
    case Y_CLONU:
        u[ i.c() ] = nullptr;
        break;
    
    case Y_SETUP:
        function->getupval( i.c() )->set( s[ i.r() ] );
        break;
    case Y_REFUP:
        s[ i.r() ] = function->getupval( i.c() )->get();
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
    {
        yvalue value = s[ i.a() ];
        if ( value.is< yarray >() )
            stack->iterators.emplace_back( value.as< yarray >() );
        else if ( value.is< ytable >() )
            stack->iterators.emplace_back( value.as< ytable >() );
        else
            throw yerror( "expected indexable value" );
        break;
    }
    
    case Y_ITERKEY:
    {
        stack->iterators.emplace_back( s[ i.a() ].as_expand() );
        break;
    }
    
    case Y_NEXT1:
    {
        stack->iterators.back().next1( s + i.r() );
        break;
    }
    
    case Y_NEXT2:
    {
        stack->iterators.back().next2( s + i.r(), s + i.a() );
        break;
    }
    
    case Y_NEXT:
    {
        stack->iterators.back().next( stack, fp + i.r(), i.b() );
        s = stack->stack.data() + fp;
        break;
    }
    
    case Y_IFITER:
    {
        if ( stack->iterators.back().valid() )
            ip += i.j();
        break;
    }
    
    case Y_IFNITER:
    {
        if ( ! stack->iterators.back().valid() )
            ip += i.j();
        break;
    }
    
    case Y_POPITER:
    {
        stack->iterators.pop_back();
        break;
    }
        
    case Y_TABLE:
    {
        s[ i.r() ] = ytable::alloc();
        break;
    }
    case Y_OBJECT:
    {
        if ( i.a() == Y_NOVAL )
        {
            s[ i.r() ] = yexpand::alloc();
        }
        else
        {
            yexpand* prototype = s[ i.a() ].as_expand();
            yexpand* arrayproto = yscope::scope->model->array_proto();
            yexpand* tableproto = yscope::scope->model->table_proto();
            if ( prototype == arrayproto || yexpand::is( prototype, arrayproto ) )
                s[ i.r() ] = yarray::alloc( prototype );
            else if ( prototype == tableproto || yexpand::is( prototype, tableproto ) )
                s[ i.r() ] = ytable::alloc( prototype );
            else
                s[ i.r() ] = yexpand::alloc( prototype );
        }
        break;
    }

    case Y_ARRAY:
    {
        s[ i.r() ] = yarray::alloc();
        break;
    }
    case Y_UNPACK:
    {
        yarray* a = s[ i.a() ].as< yarray >();
        size_t count = a->length() - i.b();
        stack->mark = fp + i.r() + count;
        stack->ensure_stack( stack->mark );
        s = stack->stack.data() + fp;
        for ( size_t index = 0; index < count; ++index )
        {
            s[ i.r() + index ] = a->getindex( i.b() + index );
        }
        break;
    }
    case Y_APPEND:
    {
        s[ i.a() ].as< yarray >()->append( s[ i.r() ] );
        break;
    }
    case Y_EXTEND:
    {
        yarray* array = s[ i.a() ].as< yarray >();
        for ( size_t index = fp + i.r(); index < stack->mark; ++index )
        {
            array->append( stack->stack[ index ] );
        }
        break;
    }
    
    case Y_CLOSURE:
    {
        yrecipe* recipe = module->recipe( i.c() );
        yfunction* closure = yfunction::alloc( global, recipe );
        for ( size_t index = 0; index < recipe->upval_count(); ++index )
        {
            yinstruction i = *ip++;
            switch ( i.opcode() )
            {
            case Y_ENVNU:
                closure->setupval( index, u[ i.c() ] );
                break;
            case Y_ENVUP:
                closure->setupval( index, function->getupval( i.c() ) );
                break;
            default:
                assert( !"expected upval" );
                break;
            }
        }
        s[ i.r() ] = closure;
        break;
    }
    case Y_ENVNU:
    case Y_ENVUP:
    {
        assert( !"unexpected upval" );
        break;
    }
        
    case Y_VARARG:
    {
        s[ i.r() ] = s[ sp + 1 + i.c() ];
        break;
    }
    case Y_VARALL:
    {
        stack->mark = fp + i.r() + incount - 1;
        stack->ensure_stack( stack->mark );
        s = stack->stack.data() + fp;
        for ( size_t index = 1; index < incount; ++index )
        {
            s[ i.r() + index - 1 ] = stack->stack[ sp + index ];
        }
        break;
    }
    
    case Y_CALL:
    {
        yvalue f = s[ i.r() ];
        if ( ! f.is_thunk() )
        {
            yexec( fp + i.r(), i.a(), i.b() );
            s = stack->stack.data() + fp;
        }
        else
        {
            // Build yframe to pass arguments to native code.
            yvalue* limit = stack->stack.data() + stack->stack.size();
            yframe frame( s + i.r(), limit, i.a() - 1 );
            
            // Call native thunk.
            f.as_thunk()( frame );
            s = stack->stack.data() + fp;
            
            // Ensure we have correct number of results.
            size_t result_count = frame.s - ( s + i.r() );
            if ( i.b() != Y_MARK )
            {
                for ( size_t index = result_count; index < i.b(); ++index )
                {
                    s[ i.r() + index ] = yvalue::ynull;
                }
            }
            else
            {
                stack->mark = frame.s - stack->stack.data();
            }
        }
        break;
    }
    case Y_YCALL:
    case Y_YIELD:
    {
        assert( !"not implemented!" );
        break;
    }
    
    case Y_RETURN:
    {
        // How many values do we actually have?
        size_t valcount = i.a() != Y_MARK ? i.a() : stack->mark - i.r();

        // We might have to move them to the right place.
        if ( fp + i.r() != sp )
        {
            size_t movecount = valcount;
            if ( outcount != Y_MARK )
            {
                movecount = std::min( movecount, (size_t)outcount );
            }
            
            for ( size_t index = 0; index < movecount; ++index )
            {
                stack->stack[ sp + index ] = s[ i.r() + index ];
            }
        }
        
        // How many values is the caller expecting?
        if ( outcount != Y_MARK )
        {
            for ( size_t index = valcount; index < outcount; ++index )
            {
                stack->stack[ sp + index ] = yvalue::ynull;
            }
        }
        else
        {
            stack->mark = sp + valcount;
        }
        return;
    }
    
    
    }

    
    }
    
    
    


}

