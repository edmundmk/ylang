 //
//  xec_ssa_buildcode.cpp
//
//  Created by Edmund Kapusniak on 25/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_buildcode.h"
#include "xec_ssa.h"
#include "xec_ssa_cfganalysis.h"
#include "xec_string.h"
#include "xec_code.h"
#include "xec_script.h"
#include "xec_inline.h"




struct xec_ssa_rtgraph
{
    explicit operator bool ()
    {
        return moves.size();
    }

    void move( int target, int source )
    {
        if ( target != source )
        {
            moves[ target ] = source;
        }
    }

    std::unordered_map< int, int > moves;
};





xec_ssa_buildcode::xec_ssa_buildcode( xec_ssa* root )
    :   root( root )
    ,   func( NULL )
    ,   dfo( NULL )
    ,   maxstack( -1 )
{
}



void xec_ssa_buildcode::build_func( xec_ssa_func* func, xec_ssa_dfo* dfo )
{
    this->func = func;
    this->dfo  = dfo;


    /*
        Translate function out of SSA form.
    */

    for ( size_t i = 0; i < dfo->size(); ++i )
    {
        xec_ssa_block* block = dfo->at( i );
        
        // Remember label at start of block.
        label( block );
        
        
        // Translate the operations in the block.
        build_ops( block );
        
        
        // CFG edges out.
        build_cfg( block );
    }
    
    
    /*
        Build module function.
    */
    
    xec_function* function = new xec_function();
    function->mname = strdup( func->funcname );

    function->mcode = (xec_opinst*)malloc( sizeof( xec_opinst ) * code.size() );
    memcpy( function->mcode, code.data(), sizeof( xec_opinst ) * code.size() );
    function->mcodecount  = (uint32_t)code.size();

    function->mparamcount = func->paramcount;
    function->mupvalcount = func->upvalcount;
    function->mnewupcount = func->localupcount;
    function->mstackcount = maxstack + 1;
    function->mvarargs    = func->varargs;
    function->mcoroutine  = func->coroutine;
    functions.push_back( function );
    
    
    /*
        Clear.
    */
    
    labels.clear();
    code.clear();
    maxstack = -1;
}


xec_module* xec_ssa_buildcode::build_module()
{
    xec_module* module = new xec_module();
    module->mname = strdup( root->script->get_filename() );
    
    module->mkeys = (xec_objkey*)malloc( sizeof( xec_objkey ) * keys.size() );
    memcpy( module->mkeys, keys.data(), sizeof( xec_objkey ) * keys.size() );
    module->mkeycount = (uint32_t)keys.size();
    
    module->mvalues = (xec_value*)malloc( sizeof( xec_value ) * values.size() );
    memcpy( module->mvalues, values.data(), sizeof( xec_value ) * values.size() );
    module->mvaluecount = (uint32_t)values.size();
    
    for ( size_t i = 0; i < functions.size(); ++i )
    {
        xec_function* function = functions.at( i );
        function->mmodule = module;
    }
    
    module->mfuncs = (xec_function**)malloc( sizeof( xec_function* ) * functions.size() );
    memcpy( module->mfuncs, functions.data(), sizeof( xec_function* ) * functions.size() );
    module->mfunccount = (uint32_t)functions.size();
    
    keys.clear();
    values.clear();
    functions.clear();
    
    return module;
}




void xec_ssa_buildcode::build_ops( xec_ssa_block* block )
{
    for ( size_t i = 0; i < block->ops->ops.size(); ++i )
    {
        xec_ssa_op* op = &block->ops->ops.at( i );
        
        switch ( op->opcode )
        {
        case XEC_SSA_NOP:
            break;
        case XEC_SSA_REF:
            break;
        
        case XEC_SSA_NULL:
            inst( XEC_NULL, o( op->r ), 0, 0 );
            break;
        case XEC_SSA_POS:
            inst( XEC_POS, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_NEG:
            inst( XEC_NEG, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_NOT:
            inst( XEC_LNOT, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_BITNOT:
            inst( XEC_NOT, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_MUL:
            inst( XEC_MUL, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_DIV:
            inst( XEC_DIV, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_MOD:
            inst( XEC_DIV, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_INTDIV:
            inst( XEC_INTDIV, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_ADD:
            inst( XEC_ADD, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_SUB:
            inst( XEC_SUB, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LSL:
            inst( XEC_LSL, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LSR:
            inst( XEC_LSR, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_ASR:
            inst( XEC_ASR, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITAND:
            inst( XEC_AND, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITXOR:
            inst( XEC_XOR, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_BITOR:
            inst( XEC_OR, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_CONCAT:
            inst( XEC_CONCAT, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_EQ:
            inst( XEC_EQ, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LT:
            inst( XEC_LT, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_LE:
            inst( XEC_LE, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_IN:
            inst( XEC_IN, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_IS:
            inst( XEC_IS, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_XOR:
            inst( XEC_LXOR, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_INKEY:
            inst( XEC_INKEY, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_INDEX:
            inst( XEC_INDEX, o( op->r ), o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_DELINKEY:
            inst( XEC_DELINKEY, o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_ITER:
            inst( XEC_ITER, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_EACH:
            inst( XEC_ITERKEY, o( op->r ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_APPEND:
            inst( XEC_APPEND, o( op->operandb ), o( op->operanda ), 0 );
            break;
        case XEC_SSA_KEY:
            inst( XEC_KEY, o( op->r ), o( op->operanda ), k( op->immkey ) );
            break;
        case XEC_SSA_DELKEY:
            inst( XEC_DELKEY, 0, o( op->operanda ), k( op->immkey ) );
            break;
        case XEC_SSA_GLOBAL:
            inst( XEC_GLOBAL, o( op->r ), 0, k( op->immkey ) );
            break;
        case XEC_SSA_SETGLOBAL:
            inst( XEC_SETGLOBAL, o( op->operanda ), 0, k( op->immkey ) );
            break;
        case XEC_SSA_ARRAY:
            inst( XEC_ARRAY, o( op->r ), op->immkey );
            break;
        case XEC_SSA_TABLE:
            inst( XEC_TABLE, o( op->r ), op->immkey );
            break;
        case XEC_SSA_SETINKEY:
            inst( XEC_SETINKEY, o( op->operandv ),
                            o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_SETINDEX:
            inst( XEC_SETINDEX, o( op->operandv ),
                            o( op->operanda ), o( op->operandb ) );
            break;
        case XEC_SSA_SETKEY:
            inst( XEC_SETKEY, o( op->operandv ),
                            o( op->operanda ), k( op->immkey ) );
            break;
            
            
        case XEC_SSA_NUMBER:
        {
            if ( op->r == -1 )
            {
                break;
            }
        
            xec_value v = xec_value( op->number );
            
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                if ( values.at( i ) == v )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                values.push_back( v );
            }
            
            inst( XEC_VALUE, o( op->r ), (int)i );
            break;
        }
        
        case XEC_SSA_BOOL:
        {
            if ( op->r == -1 )
            {
                break;
            }

            xec_value v = xec_value( op->boolean );
        
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                if ( values.at( i ) == v )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                values.push_back( v );
            }
            
            inst( XEC_VALUE, o( op->r ), (int)i );
            break;
        }
        
        case XEC_SSA_STRING:
        {
            if ( op->r == -1 )
            {
                break;
            }

            xec_ssa_string* s = op->string;
        
            size_t i = 0;
            for ( ; i < values.size(); ++i )
            {
                xec_value v = values.at( i );
                if ( v.isstring() && v.string().size() == s->length &&
                    memcmp( v.string().c_str(), s->string, s->length ) == 0 )
                {
                    break;
                }
            }
            
            if ( i >= values.size() )
            {
                i = values.size();
                xec_string* t = xec_string::create( s->string, s->length );
                values.push_back( t );
            }
            
            inst( XEC_VALUE, o( op->r ), (int)i );
            break;
        }
        
        case XEC_SSA_OBJECT:
        {
            int proto = op->operanda ? o( op->operanda ) : XEC_NOVAL;
            inst( XEC_OBJECT, o( op->r ), proto, 0 );
            break;
        }

            
        case XEC_SSA_PARAM:
        {
            // Build register transfer graph to get parameters in correct
            // registers.
            xec_ssa_rtgraph rtg;
            while ( op->opcode == XEC_SSA_PARAM )
            {
                rtg.move( (int)op->r, op->immkey );
                i += 1;
                if ( i >= block->ops->ops.size() )
                {
                    break;
                }
                op = &block->ops->ops.at( i );
            }
            i -= 1;
            move( &rtg );
            break;
        }
        
        case XEC_SSA_SELECT:
        {
            // Will have compiled selects as part of the op they select from.
            break;
        }
            
        case XEC_SSA_NEWUP:
        {
            assert( op->immkey >= func->upvalcount );
            int nuindex = op->immkey - func->upvalcount;
            inst( XEC_NEWUP, o( op->operanda ), nuindex );
            break;
        }
        
        case XEC_SSA_SETUP:
        {
            if ( op->immkey < func->upvalcount )
            {
                inst( XEC_SETUP, o( op->operanda ), op->immkey );
            }
            else
            {
                int nuindex = op->immkey - func->upvalcount;
                inst( XEC_SETNU, o( op->operanda ), nuindex );
            }
            break;
        }
        
        case XEC_SSA_REFUP:
        {
            if ( op->immkey < func->upvalcount )
            {
                inst( XEC_REFUP, o( op->r ), op->immkey );
            }
            else
            {
                int nuindex = op->immkey - func->upvalcount;
                inst( XEC_REFNU, o( op->r ), nuindex );
            }
            break;
        }
        
        case XEC_SSA_CLOSE:
        {
            // Build ranges of upvals to close from adjacent close ops.
            std::unordered_set< int > close;
            int minclose = INT_MAX;
            int maxclose = INT_MIN;
            
            while ( op->opcode == XEC_SSA_CLOSE )
            {
                assert( op->immkey >= func->upvalcount );
                int nuindex = op->immkey - func->upvalcount;
                close.insert( nuindex );
                minclose = std::min( minclose, nuindex );
                maxclose = std::max( maxclose, nuindex );
                ++i;
                if ( i >= block->ops->ops.size() )
                {
                    break;
                }
                op = &block->ops->ops.at( i );
            }
            --i;
            
            for ( int c = minclose; c < maxclose; ++c )
            {
                if ( close.find( c ) == close.end() )
                {
                    continue;
                }
                
                int start = c;
                while ( close.find( c ) != close.end() )
                {
                    ++c;
                }
                
                inst( XEC_CLOSE, ( c - 1 ) - start, start );
            }
            
            break;
        }
            
        case XEC_SSA_VARARG:
        {
            if ( op->args->rcount == -1 )
            {
                // Stacklike unpack of all varargs.
                assert( op->r == op->args->stacktop );
                inst( XEC_VARALL, o( op->r ), 0 );
            }
            else
            {
                // Recover single vararg.
                inst( XEC_VARARG, o( op->r ), op->args->rcount );
            }
            break;
        }
        
        case XEC_SSA_UNPACK:
        {
            // Should have a single argument, the array.
            assert( op->args->args.size() == 1 );
            int array = o( op->args->args.at( 0 ) );

            if ( op->args->rcount == -1 )
            {
                // Stacklike unpack of all results.
                assert( op->r == op->args->stacktop );
                inst( XEC_UNPACK, o( op->r ), array, 0 );
            }
            else
            {
                // Recover single array element.
                inst( XEC_ELEM, o( op->r ), array, op->args->rcount );
            }
            break;
        }
        
        case XEC_SSA_CALL:
        {
            call( XEC_CALL, block->ops, (int)i );
            break;
        }
        
        case XEC_SSA_YCALL:
        {
            call( XEC_YCALL, block->ops, (int)i );
            break;
        }
            
        case XEC_SSA_YIELD:
        {
            call( XEC_YIELD, block->ops, (int)i );
            break;
        }
            
        case XEC_SSA_NEW:
        {
            // Build register transfer to get arguments in correct positions.
            arguments( op );
            
            // Instruction places new object in the correct place.
            int b = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
            inst( XEC_NEW, o( op->r ), op->args->stacktop, b );
            break;
        }
        
        case XEC_SSA_EXTEND:
        {
            // Extend should have only the array and unpacked args.
            assert( op->args->unpacked );
            inst( XEC_EXTEND, op->args->stacktop,
                            o( op->args->args.at( 0 ) ), 0 );
            break;
        }
        
        case XEC_SSA_NEXT:
        {
            // Should have a single argument, the iterator.
            assert( op->args->args.size() == 1 );
            int iter = o( op->args->args.at( 0 ) );
 
            // There are special cases for next ops returning 1 and 2 values.
            // This is the vast majority of cases (iterating through arrays,
            // tables, or object keys).
            if ( op->args->rcount == 1 )
            {
                // Simplest version.
                int select = -1;
                for ( size_t j = i + 1; j < block->ops->ops.size(); ++j )
                {
                    xec_ssa_op* selop = &block->ops->ops.at( j );
                    
                    if ( selop->opcode != XEC_SSA_SELECT
                            || func->getop( selop->operanda ) != op )
                    {
                        break;
                    }
                    
                    if ( selop->immkey == 0 )
                    {
                        select = o( selop->r );
                    }
                }
                
                inst( XEC_NEXT1, select, iter, 0 );
            }
            else if ( op->args->rcount == 2 )
            {
                // Find the two following selects.
                int select[ 2 ] = { -1, -1 };
                for ( size_t j = i; j < block->ops->ops.size(); ++j )
                {
                    xec_ssa_op* selop = &block->ops->ops.at( j );

                    if ( selop->opcode != XEC_SSA_SELECT
                            || func->getop( selop->operanda ) != op )
                    {
                        break;
                    }
                    
                    if ( selop->immkey == 0 || selop->immkey == 1 )
                    {
                        select[ op->immkey ] = o( op->r );
                    }
                }
                
                // Two-result version.
                inst( XEC_NEXT2, select[ 0 ], select[ 1 ], iter );
            }
            else
            {
                // Generic version which uses stack top.
                int b = op->args->rcount == -1 ? XEC_MARK : op->args->rcount;
                stack( op->args->stacktop, b );
                inst( XEC_NEXT, op->args->stacktop, iter, b );
            
                // Build register transfer graph from following selects to get
                // results into the correct locations.
                select( block->ops, (int)i );
            }
        }

        case XEC_SSA_RETURN:
        {
            // Build register transfer to get arguments in correct position.
            arguments( op );
            
            // Return them.
            int a = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
            inst( XEC_RETURN, op->args->stacktop, a, 0 );
            break;
        }
        
        case XEC_SSA_CLOSURE:
        {
            // Assume that functions will end up in the module in the same
            // order as in the SSA form.
            size_t index = 0;
            for ( ; index < root->functions.size(); ++index )
            {
                if ( root->functions.at( index ) == op->closure->function )
                {
                    break;
                }
            }
            assert( index < root->functions.size() );
            
            // Output instruction.
            inst( XEC_CLOSURE, o( op->r ), (int)index );
            
            // Add upvals to closure.
            assert( op->closure->upvals.size() ==
                            op->closure->function->upvalcount );
            for ( size_t i = 0; i < op->closure->upvals.size(); ++i )
            {
                xec_ssa_opref upref = op->closure->upvals.at( i );
                xec_ssa_op* upop = func->getop( func->operand( upref ) );
                assert( upop->opcode == XEC_SSA_REFUP );
                if ( upop->immkey < func->upvalcount )
                {
                    inst( XEC_ENVUP, 0, upop->immkey );
                }
                else
                {
                    unsigned nuindex =  upop->immkey - func->upvalcount;
                    inst( XEC_ENVNU, 0, nuindex );
                }
            }
            
            break;
        }
        
        case XEC_SSA_PHI:
        case XEC_SSA_LIVE:
        {
            assert( ! "invalid opcode" );
            break;
        }
        }
        
    
    }
}


void xec_ssa_buildcode::build_cfg( xec_ssa_block* block )
{
    /*
        Note that blocks with only one predecessor never have phi rtgs.
        
        There is some scope to optimize branches involving phi rtgs. If both
        rtgs out of a block are the same we can do it before the branch.  If
        one branch is more likely we could do its rtg before the branch and
        then 'undo' it on the other branch.  I'm sure there are others...
    */


    if ( block->condition )
    {
        xec_ssa_rtgraph phitrue;
        xec_ssa_rtgraph phifalse;
        
        phi( &phitrue, block, block->iftrue );
        phi( &phifalse, block, block->iffalse );
    
        if ( block->iftrue->index == block->index + 1 )
        {
            if ( phifalse )
            {
                /*
                        iftrue goto next;
                        false phi
                        goto false;
                    next:
                        true phi
                        ...
                */
                
                branch( block->condition, true, &block->condition );
                move( &phifalse );
                jump( XEC_JMP, 0, block->iffalse );
                label( &block->condition );
            }
            else
            {
                /*
                        iffalse goto false;
                        true phi
                        ...
                */
                
                branch( block->condition, false, block->iffalse );
            }

            if ( phitrue )
            {
                move( &phitrue );
            }
        }
        else if ( block->iffalse->index == block->index + 1 )
        {
            if ( phitrue )
            {
                /*
                        iffalse goto next:
                        true phi
                        goto true;
                    next:
                        false phi
                        ...
                */
                
                branch( block->condition, false, &block->condition );
                move( &phitrue );
                jump( XEC_JMP, 0, block->iftrue );
                label( &block->condition );
            }
            else
            {
                /*
                        iftrue goto true;
                        false phi
                        ...
                */
                
                branch( block->condition, true, block->iftrue );
            }
            
            if ( phifalse )
            {
                move( &phifalse );
            }
        }
        else
        {
            if ( phitrue && phifalse )
            {
                /*
                        iffalse goto falsephi;
                        true phi
                        goto true;
                    false:
                        false phi
                        goto false;
                */
                
                branch( block->condition, false, &block->condition );
                move( &phitrue );
                jump( XEC_JMP, 0, block->iftrue );
                label( &block->condition );
                move( &phifalse );
                jump( XEC_JMP, 0, block->iffalse );
            }
            else if ( phitrue )
            {
                /*
                        iffalse goto false;
                        true phi
                        goto true;
                */
                
                branch( block->condition, false, block->iffalse );
                move( &phitrue );
                jump( XEC_JMP, 0, block->iftrue );
            }
            else if ( phifalse )
            {
                /*
                        iftrue goto true;
                        false phi
                        goto false;
                */
                
                branch( block->condition, true, block->iftrue );
                move( &phifalse );
                jump( XEC_JMP, 0, block->iffalse );
            }
            else
            {
                /*
                        iftrue goto true;
                        goto false;
                */
                
                branch( block->condition, true, block->iftrue );
                jump( XEC_JMP, 0, block->iffalse );
            }
        }
    }
    else if ( block->next )
    {
        // Perform phi on entry to next block.
        xec_ssa_rtgraph phinext;
        phi( &phinext, block, block->next );
        if ( phinext )
        {
            move( &phinext );
        }
        
        // Add jump to next block.
        if ( block->next->index != block->index + 1 )
        {
            jump( XEC_JMP, 0, block->next );
        }
    }
        
}



int xec_ssa_buildcode::k( int immkey )
{
    const char* ssakey = root->keys.at( immkey );
    for ( size_t i = 0; i < keys.size(); ++i )
    {
        xec_objkey key = keys.at( i );
        if ( strcmp( key.c_str(), ssakey ) == 0 )
        {
            return (int)i;
        }
    }
    
    int result = (int)keys.size();
    keys.push_back( xec_objkey::create( ssakey ) );
    return result;
}

int xec_ssa_buildcode::o( xec_ssa_opref operand )
{
    operand = func->operand( operand );
    xec_ssa_op* op = func->getop( operand );
    return o( op->r );
}

int xec_ssa_buildcode::o( int r )
{
    maxstack = std::max( maxstack, r );
    return r;
}


void xec_ssa_buildcode::stack( int r, int b )
{
    if ( b != XEC_MARK )
    {
        maxstack = std::max( maxstack, r + b );
    }
}


void xec_ssa_buildcode::inst( xec_opcode opcode, int r, int a, int b )
{
    if ( r == -1 || a == -1 || b == -1 )
    {
        return;
    }
    
    code.emplace_back( opcode, (unsigned)r, (unsigned)a, (unsigned)b );
}

void xec_ssa_buildcode::inst( xec_opcode opcode, int r, int c )
{
    if ( r == -1 || c == -1 )
    {
        return;
    }
    
    code.emplace_back( opcode, (unsigned)r, (unsigned)c );
}


void xec_ssa_buildcode::call(
                xec_opcode opcode, xec_ssa_slice* slice, int index )
{
    xec_ssa_op* op = &slice->ops.at( index );

    // Build register transfer to get arguments in correct positions.
    arguments( op );
    
    // Call.
    int a = op->args->unpacked ? XEC_MARK : (int)op->args->args.size();
    int b = op->args->rcount == -1 ? XEC_MARK : op->args->rcount;
    stack( op->args->stacktop, b );
    inst( opcode, op->args->stacktop, a, b );

    // Build register transfer graph from following selects to get results in
    // the correct location.
    select( slice, index );
}


void xec_ssa_buildcode::arguments( xec_ssa_op* op )
{
    assert( op->opcode >= XEC_SSA_FIRST_ARG
                && op->opcode <= XEC_SSA_LAST_ARG );

    // Build rtg from arguments.
    xec_ssa_rtgraph rtg;
    for ( size_t i = 0; i < op->args->args.size(); ++i )
    {
        int operand = o( op->args->args.at( i ) );
        rtg.move( op->args->stacktop + (int)i, operand );
    }

    // Rely on register allocator to place unpacked ops correctly.
    if ( op->args->unpacked )
    {
        xec_ssa_op* unpackop = func->getop( op->args->unpacked );
        assert( unpackop->opcode >= XEC_SSA_FIRST_ARG
                        && unpackop->opcode <= XEC_SSA_LAST_ARG );
        assert( unpackop->r == unpackop->args->stacktop );
        assert( unpackop->r == op->args->stacktop + op->args->args.size() );
    }
    
    // Perform moves.
    move( &rtg );
}

void xec_ssa_buildcode::select( xec_ssa_slice* slice, size_t index )
{
    // The op that we're at is the one that generates the values to select.
    xec_ssa_op* op = &slice->ops.at( index );
    assert( op->opcode >= XEC_SSA_FIRST_ARG
                && op->opcode <= XEC_SSA_LAST_ARG );
    int stacktop = op->args->stacktop;

    // Might not have any selects, in which case it might have a single
    // value we want to place in the allocated register.
    xec_ssa_rtgraph rtg;
    if ( op->r != -1 )
    {
        rtg.move( (int)op->r, stacktop );
    }

    // Or there might be selects after all.
    for ( size_t i = index + 1; i < slice->ops.size(); ++i )
    {
        xec_ssa_op* selop = &slice->ops.at( index );
        
        if ( selop->opcode != XEC_SSA_SELECT )
        {
            break;
        }
        
        assert( selop->immkey < op->args->rcount );
        rtg.move( (int)selop->r, stacktop + selop->immkey );
    }
    
    // Perform moves.
    move( &rtg );
}



void xec_ssa_buildcode::move( xec_ssa_rtgraph* rtg )
{
    // The register transfer graph has nodes representing registers and edges
    // representing moves.  Each node has at most a single incoming edge, but
    // may have many outgoing edges.  We can separate the graph into:
    //
    //      Cycles.
    //      Paths which start at a node with no outgoing edges and follow
    //          edges backwards to either a node with no incoming edges, or
    //          a node which is part of a cycle.

    // We can perform the register transfers by following this algorithm:
    //
    //      Count the number of outgoing edges for each node.
    //      While there is at least one node with a zero count:
    //          Perform move to this node A from its source node B.
    //          Decrement outgoing edge count of source node A.
    //          Remove node A from set.
    //      The remaining nodes form loops.  While nodes remain:
    //          Swap values between a random node A and its source node B.
    //          Remove edge between nodes A and B.
    //          Create a new edge between node A and the target of node B.
    //          Remove node A from set.
    //

    // Or, a version of the loop-swapping algorithm that doesn't require us to
    // traverse the graph forwards along edges:
    //
    //      Pick a random node C.
    //      Find its source node B and B's source node A.
    //      If A == C then swap the two nodes, and remove both from set.
    //      Otherwise swap A and B, and
    //          remove B, and
    //          change the source node of C to be A.
    //
    
    // Count outgoing edges.
    std::unordered_map< int, int > outcount;
    for ( auto i = rtg->moves.begin(); i != rtg->moves.end(); ++i )
    {
        int source = i->second;
        outcount[ source ] += 1;
    }
    
    // Build list of nodes with zero outcount.
    std::unordered_set< int > zero;
    std::unordered_set< int > nonzero;
    for ( auto i = rtg->moves.begin(); i != rtg->moves.end(); ++i )
    {
        int target = i->first;
        if ( outcount.find( target ) == outcount.end() )
        {
            zero.insert( target );
        }
        else
        {
            nonzero.insert( target );
        }
    }
    
    // Perform moves to nodes with zero outcount.
    while ( zero.size() )
    {
        auto i = zero.begin();
        int target = *i;
        int source = rtg->moves.at( target );
        
        // Perform move.
        inst( XEC_MOV, o( target ), o( source ), 0 );
        
        // Target is no longer in the set.
        zero.erase( i );
        
        // Decrement outcount for source.
        outcount[ source ] -= 1;
        if ( outcount[ source ] == 0 &&
                    nonzero.find( source ) != nonzero.end() )
        {
            nonzero.erase( source );
            zero.insert( source );
        }
    }
    
    // Remaining nodes are in cycles.
    while ( nonzero.size() )
    {
        auto i = nonzero.begin();
        int c = *i;
        int b = rtg->moves.at( c );
        int a = rtg->moves.at( b );
        
        // Swap.
        inst( XEC_SWP, o( a ), o( b ), 0 );
        
        if ( c == a )
        {
            // Remove both.
            nonzero.erase( a );
            nonzero.erase( b );
        }
        else
        {
            // Remove B.
            nonzero.erase( b );
            
            // Change source node of C to be A.
            rtg->moves[ c ] = a;
        }
    }
}



void xec_ssa_buildcode::phi(
                xec_ssa_rtgraph* rtg, xec_ssa_block* from, xec_ssa_block* to )
{
    // Find index of incoming edge.
    size_t edgeindex = -1;
    for ( size_t i = 0; i < to->previous.size(); ++i )
    {
        if ( to->previous.at( i ) == from )
        {
            edgeindex = (int)i;
            break;
        }
    }
    assert( edgeindex != -1 );
    
    // Build rtg based on phi ops.
    for ( size_t i = 0; i < to->phi->ops.size(); ++i )
    {
        xec_ssa_op& phiop = to->phi->ops.at( i );
        
        if ( phiop.opcode != XEC_SSA_PHI )
        {
            continue;
        }
        
        assert( phiop.r != -1 );
        assert( phiop.phi->definitions.size() == to->previous.size() );
        xec_ssa_opref source = phiop.phi->definitions.at( edgeindex );
        rtg->move( phiop.r, o( source ) );
    }
}

    
void xec_ssa_buildcode::label( void* label )
{
    auto i = labels.find( label );
    if ( i != labels.end() )
    {
        jumplabel& l = i->second;
        l.label = (int)code.size();
        for ( size_t i = 0 ; i < l.jumps.size(); ++i )
        {
            int jumppc = l.jumps.at( i );
            xec_opinst inst = code.at( jumppc );
            signed j = l.label - ( jumppc + 1 );
            code[ jumppc ] = xec_opinst( inst.opcode(), inst.r(), j );
        }
        l.jumps.clear();
    }
    else
    {
        jumplabel l;
        l.label = (int)code.size();
        labels.emplace( label, std::move( l ) );
    }
}


void xec_ssa_buildcode::jump( xec_opcode opcode, int r, void* label )
{
    
    // Check for label.
    int labelpc = (int)code.size() + 1;
    auto i = labels.find( label );
    if ( i != labels.end() )
    {
        jumplabel& l = i->second;
        if ( l.label == -1 )
        {
            l.jumps.push_back( (int)code.size() );
        }
        else
        {
            labelpc = l.label;
        }
    }
    else
    {
        jumplabel l;
        l.label = -1;
        l.jumps.push_back( (int)code.size() );
        labels.emplace( label, std::move( l ) );
    }
    
    signed j = labelpc - ( (int)code.size() + 1 );
    code.emplace_back( opcode, r, j );
    
}


void xec_ssa_buildcode::branch(
                xec_ssa_opref condition, bool iftrue, void* label )
{
    xec_ssa_op* condop = func->getop( func->operand( condition ) );
    if ( condop->opcode == XEC_SSA_NEXT )
    {
        xec_opcode opcode = iftrue ? XEC_IFITER : XEC_IFDONE;
        jump( opcode, 0, label );
    }
    else
    {
        xec_opcode opcode = iftrue ? XEC_IFTRUE : XEC_IFFALSE;
        assert( condop->r != -1 );
        jump( opcode, o( condop->r ), label );
    }
}





