//
//  xssa_ycodeout.cpp
//
//  Created by Edmund Kapusniak on 13/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_ycodeout.h"
#include "xssa_linear.h"
#include "xssa.h"
#include "xec_script.h"




/*
    A register-transfer graph representing a set of moves which must happen
    in parallel.
*/


struct xssa_rtgraph
{
    explicit operator bool ();
    void move( int target, int source );
    
    std::unordered_map< int, int > moves;
};


xssa_rtgraph::operator bool()
{
    return ! moves.empty();
}

void xssa_rtgraph::move( int target, int source )
{
    if ( target != source )
    {
        moves[ target ] = source;
    }
}




/*
    Other helpers.
*/


static size_t collect_ops( std::vector< xssaop* >* ops,
                xssa_linear* l, size_t index, xssa_opcode opcode )
{
    // Gather list of ops with the required opcode.
    while ( index < l->lops.size() )
    {
        xssalop* lop = &l->lops.at( index );
        if ( lop->kind == XSSA_LOP_OP && lop->op->opcode == opcode )
        {
            ops->push_back( lop->op );
            index += 1;
        }
        else
        {
            break;
        }
    }
    
    // Return the index of the last op we found.
    return index - 1;
}


static bool compare_immed( xssaop* a, xssaop* b )
{
    assert( xssaop::has_immed( a->opcode ) );
    assert( a->opcode == b->opcode );
    return a->immed < b->immed;
}







/*
    Class that translates out of SSA formand builds a ycode module.
*/


xssa_ycodeout::xssa_ycodeout()
    :   maxstack( -1 )
{
}



void xssa_ycodeout::build_func( xssa_linear* l )
{
    xssa_func* func = l->func;



    /*
        Translate function out of SSA form.
    */
    
    for ( size_t index = 0; index < l->lops.size(); ++index )
    {
    
    
    xssalop* lop = &l->lops.at( index );
    
    
    
    // At the start of a block, remember label so we can jump to it.
    if ( lop->kind == XSSA_LOP_BEGIN )
    {
        label( lop->block );
        continue;
    }
    
    // At the end of a block, do control flow.
    if ( lop->kind == XSSA_LOP_END )
    {
        control_flow( lop->block );
        continue;
    }
    
    
    
    // live markers are uninteresting.
    if ( lop->kind == XSSA_LOP_LIVE )
    {
        continue;
    }
    
    
    
    
    // Assemble code for normal ops.
    assert( lop->kind == XSSA_LOP_OP );
    xssaop* op = lop->op;
    
    switch ( op->opcode )
    {
    case XSSA_NOP:
        break;
    
    // constants
    case XSSA_NULL:
        inst( Y_NULL, o( op->r ), 0, 0 );
        break;
    case XSSA_NUMBER:
        value( o( op->r ), op->number );
        break;
    case XSSA_BOOL:
        value( o( op->r ), op->boolean );
        break;
    case XSSA_STRING:
        value( o( op->r ), ystring::alloc( op->string->length, op->string->string ) );
        break;

    // unary operations
    case XSSA_POS:
        inst( Y_POS, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;
    case XSSA_NEG:
        inst( Y_NEG, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;
    case XSSA_NOT:
        inst( Y_NOT, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;
    case XSSA_LNOT:
        inst( Y_LNOT, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;

    // binary operations
    case XSSA_MUL:
        inst( Y_MUL, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_DIV:
        inst( Y_DIV, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_MOD:
        inst( Y_MOD, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_INTDIV:
        inst( Y_INTDIV, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_ADD:
        inst( Y_ADD, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_SUB:
        inst( Y_SUB, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_LSL:
        inst( Y_LSL, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_LSR:
        inst( Y_LSR, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_ASR:
        inst( Y_ASR, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_AND:
        inst( Y_AND, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_XOR:
        inst( Y_XOR, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_OR:
        inst( Y_OR, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_CONCAT:
        inst( Y_CONCAT, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    
    // comparisons
    case XSSA_EQ:
        inst( Y_EQ, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_LT:
        inst( Y_LT, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_LE:
        inst( Y_LE, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_IN:
        inst( Y_IN, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_IS:
        inst( Y_IS, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;

    // logical operations
    case XSSA_LXOR:
        inst( Y_LXOR, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    
    // indexing
    case XSSA_KEY:
        inst( Y_KEY, o( op->r ), o( op->operand[ 0 ] ), k( op->key ) );
        break;
    case XSSA_INKEY:
        inst( Y_INKEY, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_INDEX:
        inst( Y_INDEX, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_GLOBAL:
        inst( Y_GLOBAL, o( op->r ), 0, k( op->key ) );
        break;

    // assignment
    case XSSA_SETKEY:
        inst( Y_SETKEY, o( op->operand[ 1 ] ), o( op->operand[ 0 ] ), k( op->key ) );
        break;
    case XSSA_SETINKEY:
        inst( Y_SETINKEY, o( op->operand[ 2 ] ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_SETINDEX:
        inst( Y_SETINDEX, o( op->operand[ 2 ] ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_SETGLOBAL:
        inst( Y_SETGLOBAL, o( op->operand[ 0 ] ), 0, k( op->key ) );
        break;
    
    // key deletion
    case XSSA_DELKEY:
        inst( Y_INKEY, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    case XSSA_DELINKEY:
        inst( Y_INKEY, o( op->r ), o( op->operand[ 0 ] ), o( op->operand[ 1 ] ) );
        break;
    
    // upvals (loads/stores are represented explicitly)
    case XSSA_NEWUP:
    {
        assert( op->immed >= func->upvalcount );
        int nuindex = op->immed - func->upvalcount;
        inst( Y_NEWNU, o( op->operand[ 0 ] ), nuindex );
        break;
    }
    
    case XSSA_SETUP:
    {
        if ( op->immed < func->upvalcount )
        {
            inst( Y_SETUP, o( op->operand[ 0 ] ), op->immed );
        }
        else
        {
            int nuindex = op->immed - func->upvalcount;
            inst( Y_SETNU, o( op->operand[ 0 ] ), nuindex );
        }
        break;
    }
    
    case XSSA_REFUP:
    {
        // Upvals which are inputs to closures shouldn't have been allocated
        // a destination register.
        if ( op->r == (uint8_t)-1 )
        {
            break;
        }
        
        // Otherwise reference the upval.
        if ( op->immed < func->upvalcount )
        {
            inst( Y_REFUP, o( op->r ), op->immed );
        }
        else
        {
            int nuindex = op->immed - func->upvalcount;
            inst( Y_REFNU, o( op->r ), nuindex );
        }
        break;
    }
    
    case XSSA_CLOUP:
    {
        // Collect a run of closes and sort them.
        std::vector< xssaop* > close_ops;
        index = collect_ops( &close_ops, l, index, XSSA_CLOUP );
        std::sort( close_ops.begin(), close_ops.end(), compare_immed );

        // Close adjacent upvals in one go.
        int close_first = -1;
        int close_last  = -1;
        
        for ( size_t i = 0; i < close_ops.size(); ++i )
        {
            xssaop* op = close_ops.at( i );
            if ( close_first == -1 )
            {
                close_first = op->immed;
                close_last  = op->immed;
            }
            else if ( op->immed == close_last + 1 )
            {
                close_last += 1;
            }
            else
            {
                inst( Y_CLONU, close_last + 1 - close_first, close_first );
                close_first = op->immed;
                close_last  = op->immed;
            }
        }
        
        if ( close_first != -1 )
        {
            inst( Y_CLONU, close_last + 1 - close_first, close_first );
        }

        break;
    }
    
    // allocation
    case XSSA_OBJECT:
    {
        int proto = op->operand[ 0 ] ? o( op->operand[ 0 ] ) : Y_NOVAL;
        inst( Y_OBJECT, o( op->r ), proto, 0 );
        break;
    }
    
    case XSSA_TABLE:
    {
        inst( Y_TABLE, o( op->r ), op->immed );
        break;
    }
    
    case XSSA_ARRAY:
    {
        inst( Y_ARRAY, o( op->r ), op->immed );
        break;
    }

    // array operations.
    case XSSA_APPEND:
    {
        inst( Y_APPEND, o( op->operand[ 1 ] ), o( op->operand[ 0 ] ), 0 );
        break;
    }
    
    case XSSA_EXTEND:
    {
        // Extend should only have the array and multival operands.
        assert( op->operand_count == 1 );
        assert( op->multival );
        inst( Y_EXTEND, op->stacktop, o( op->operand[ 0 ] ), 0 );
        break;
    }
    
    // closures
    case XSSA_CLOSURE:
    {
        // Number of upvals must match the number expected.
        assert( op->operand_count == op->func->upvalcount );
    
        // Find index of recipe.  Assume that recipes will end up in the module
        // in the same order as in the SSA form.
        int recipe = -1;
        for ( size_t i = 0; i < func->module->funcs.size(); ++i )
        {
            xssa_func* f = func->module->funcs.at( i ).get();
            if ( op->func == f )
            {
                recipe = (int)i;
                break;
            }
        }
        assert( recipe != -1 );
        
        // Output instruction.
        inst( Y_CLOSURE, o( op->r ), recipe );

        // Output upval references.
        for ( size_t i = 0; i < op->operand_count; ++i )
        {
            xssaop* refup = op->operand[ i ];
            assert( refup->opcode == XSSA_REFUP );
            if ( refup->immed < func->upvalcount )
            {
                inst( Y_ENVUP, 0, refup->immed );
            }
            else
            {
                int nuindex = refup->immed - func->upvalcount;
                inst( Y_ENVNU, 0, nuindex );
            }
        }
        
        break;
    }

    // multival
    case XSSA_VARALL:
    {
        // Stacklike unpack of all varags.
        assert( op->r == op->stacktop );
        inst( Y_VARALL, op->stacktop, 0 );
        index = select( l, index );
        break;
    }
    
    case XSSA_UNPACK:
    {
        // Stacklike unpack of all elements.
        assert( op->operand_count == 1 );
        assert( op->r == op->stacktop );
        inst( Y_UNPACK, op->stacktop, o( op->operand[ 0 ] ), 0 );
        index = select( l, index );
        break;
    }
    
    case XSSA_CALL:
    {
        index = call( Y_CALL, l, index );
        break;
    }
    
    case XSSA_YCALL:
    {
        index = call( Y_YCALL, l, index );
        break;
    }
    
    case XSSA_YIELD:
    {
        index = call( Y_YIELD, l, index );
        break;
    }
    
    // iterators
    case XSSA_ITER:
    {
        inst( Y_ITER, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;
    }
    
    case XSSA_ITERKEY:
    {
        inst( Y_ITERKEY, o( op->r ), o( op->operand[ 0 ] ), 0 );
        break;
    }
    
    case XSSA_POPITER:
    {
        std::vector< xssaop* > popiter_ops;
        index = collect_ops( &popiter_ops, l, index, XSSA_POPITER );
        inst( Y_POPITER, 0, (int)popiter_ops.size() );
        break;
    }
    
    case XSSA_NEXT:
    {
        std::vector< xssaop* > select_ops;
        size_t last = collect_ops( &select_ops, l, index + 1, XSSA_SELECT );
    
        if ( op->result_count == 1 && select_ops.size() == 1 )
        {
            xssaop* sel1 = select_ops[ 0 ];
            inst( Y_NEXT1, o( sel1->r ), 0, 0 );
        }
        else if ( op->result_count == 2 && select_ops.size() == 2 )
        {
            xssaop* sel1 = select_ops[ 0 ];
            xssaop* sel2 = select_ops[ 1 ];
            inst( Y_NEXT2, o( sel1->r ), o( sel2->r ), 0 );
        }
        else
        {
            int b = op->result_count == -1 ? Y_MARK : op->result_count;
            inst( Y_NEXT, op->stacktop, 0, b );
            last = select( l, index );
        }
        
        index = last;
        break;
    }
    
    // flow control
    case XSSA_RETURN:
    {
        arguments( op );
        int a = op->multival ? Y_MARK : op->operand_count;
        inst( Y_RETURN, op->stacktop, a, 0 );
        break;
    }
    
    // selection
    case XSSA_PARAM:
    {
        // Collect parameters.
        std::vector< xssaop* > param_ops;
        index = collect_ops( &param_ops, l, index, XSSA_PARAM );
        
        // Construct register transfer graph.
        xssa_rtgraph rtg;
        for ( size_t i = 0; i < param_ops.size(); ++i )
        {
            xssaop* param = param_ops.at( i );
            rtg.move( param->r, param->immed + 1 );
        }
        
        // Perform moves.
        move( &rtg );
        break;
    }
    
    case XSSA_VARARG:
    {
        inst( Y_VARARG, o( op->r ), op->immed );
        break;
    }
    
    case XSSA_SELECT:
    {
        // Should be dealt with as part of the ops they select from.
        break;
    }
    
    case XSSA_ELEM:
    {
        inst( Y_ELEM, o( op->r ), op->immed );
        break;
    }

    // SSA-form
    case XSSA_PHI:
    {
        // Dealt with in control_flow().
        break;
    }
    
    case XSSA_REF:
    case XSSA_PSI:
    {
        assert( ! "virtual SSA op" );
        break;
    }
    
    }
    
    
    
    }

    
    
    
    /*
        Build module function.
    */
    
    yrecipe* recipe = yrecipe::alloc( code.size() );
    
    recipe->fname          = ystring::alloc( func->funcname );
    recipe->fparamcount    = func->paramcount;
    recipe->fupvalcount    = func->upvalcount;
    recipe->fnewupcount    = func->newupcount;
    recipe->fstackcount    = maxstack + 1;
    recipe->fvarargs       = func->varargs;
    recipe->fcoroutine     = func->coroutine;
    memcpy( recipe->fcode, code.data(), sizeof( yinstruction ) * code.size() );
    
    recipes.push_back( recipe );
    
    
    
    /*
        Clear.
    */
    
    labels.clear();
    code.clear();
    maxstack = -1;
}


ymodule* xssa_ycodeout::build_module( xec_script* script )
{
    ymodule* module = ymodule::alloc();
    module->mname = ystring::alloc( script->get_filename() );

    module->symbols = ytuple< ysymbol >::alloc( keys.size() );
    for ( size_t i = 0; i < keys.size(); ++i )
    {
        module->symbols->set( i, keys.at( i ) );
    }
    
#if YSLOTS

    build_value_slots( module );

#else

    module->values = ytuple< yvalue >::alloc( values.size() );
    for ( size_t i = 0; i < values.size(); ++i )
    {
        module->values->set( i, values.at( i ) );
    }

#endif

    module->recipes = ytuple< yrecipe* >::alloc( recipes.size() );
    for ( size_t i = 0; i < recipes.size(); ++i )
    {
        yrecipe* recipe = recipes.at( i );
        recipe->fmodule = module;
        module->recipes->set( i, recipe );
    }
    
    
    keys.clear();
    values.clear();
    recipes.clear();
    
    return module;
}



void xssa_ycodeout::control_flow( xssa_block* block )
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
        xssa_rtgraph phitrue;
        xssa_rtgraph phifalse;
        
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
                jump( Y_JMP, 0, block->iffalse );
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
                jump( Y_JMP, 0, block->iftrue );
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
                jump( Y_JMP, 0, block->iftrue );
                label( &block->condition );
                move( &phifalse );
                jump( Y_JMP, 0, block->iffalse );
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
                jump( Y_JMP, 0, block->iftrue );
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
                jump( Y_JMP, 0, block->iffalse );
            }
            else
            {
                /*
                        iftrue goto true;
                        goto false;
                */
                
                branch( block->condition, true, block->iftrue );
                jump( Y_JMP, 0, block->iffalse );
            }
        }
    }
    else if ( block->next )
    {
        // Perform phi on entry to next block.
        xssa_rtgraph phinext;
        phi( &phinext, block, block->next );
        if ( phinext )
        {
            move( &phinext );
        }
        
        // Add jump to next block.
        if ( block->next->index != block->index + 1 )
        {
            jump( Y_JMP, 0, block->next );
        }
    }
        
}



int xssa_ycodeout::k( const char* key )
{
    ysymbol k = key;
    for ( size_t i = 0; i < keys.size(); ++i )
    {
        if ( keys[ i ] == k )
            return (int)i;
    }

    int result = (int)keys.size();
    keys.push_back( k );
    return result;
}


int xssa_ycodeout::o( xssaop* operand )
{
    return o( operand->r );
}

int xssa_ycodeout::o( int r )
{
    maxstack = std::max( maxstack, r );
    return r;
}


void xssa_ycodeout::inst( ycode opcode, int r, int a, int b )
{
    code.emplace_back( opcode, (unsigned)r, (unsigned)a, (unsigned)b );
}

void xssa_ycodeout::inst( ycode opcode, int r, int c )
{
    code.emplace_back( opcode, (unsigned)r, (unsigned)c );
}


void xssa_ycodeout::value( int r, yvalue value )
{
    if ( r == -1 )
    {
        return;
    }
    
    size_t i = 0;
    for ( ; i < values.size(); ++i )
    {
        if ( values.at( i ) == value )
        {
            break;
        }
    }
    
    if ( i >= values.size() )
    {
        i = values.size();
        values.push_back( value );
    }
        
    inst( Y_VALUE, o( r ), (int)i );
}




void xssa_ycodeout::arguments( xssaop* op )
{
    // Build rtg from arguments.
    xssa_rtgraph rtg;
    for ( size_t i = 0; i < op->operand_count; ++i )
    {
        int operand = o( op->operand[ i ] );
        rtg.move( op->stacktop + (int)i, operand );
    }

    // Rely on register allocator to place unpacked ops correctly.
    assert( xssaop::has_multival( op->opcode ) );
    if ( op->multival )
    {
        assert( op->multival->stacktop == op->stacktop + op->operand_count );
    }
    
    // Perform moves.
    move( &rtg );
}


size_t xssa_ycodeout::call( ycode opcode, xssa_linear* l, size_t index )
{
    xssalop* lop = &l->lops.at( index );
    assert( lop->kind == XSSA_LOP_OP );
    xssaop* op = lop->op;

    // Build register transfer to get arguments in correct positions.
    arguments( op );
    
    // Call.
    assert( xssaop::has_multival( op->opcode ) );
    int a = op->multival ? Y_MARK : op->operand_count;
    int b = op->result_count == -1 ? Y_MARK : op->result_count;
    inst( opcode, op->stacktop, a, b );

    // Build register transfer graph from following selects to get results in
    // the correct location.
    return select( l, index );
}


size_t xssa_ycodeout::select( xssa_linear* l, size_t index )
{
    // The op that we're at is the one that generates the values to select.
    xssalop* lop = &l->lops.at( index );
    assert( lop->kind == XSSA_LOP_OP );
    xssaop* op = lop->op;

    // Build register transfer graph.
    xssa_rtgraph rtg;

    // Might not have any selects, in which case it might have a single
    // value we want to place in the allocated register.
    if ( op->r != (uint8_t)-1 )
    {
        rtg.move( op->r, op->stacktop );
    }

    // Or there might be selects after all.
    std::vector< xssaop* > select_ops;
    index = collect_ops( &select_ops, l, index + 1, XSSA_SELECT );
    for ( size_t i = 0; i < select_ops.size(); ++i )
    {
        xssaop* select = select_ops.at( i );
        assert( select->immed < op->result_count );
        rtg.move( select->r, op->stacktop + select->immed );
    }
    
    // Perform moves.
    move( &rtg );
    
    return index;
}



void xssa_ycodeout::move( xssa_rtgraph* rtg )
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
        inst( Y_MOV, o( target ), o( source ), 0 );
        
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
        inst( Y_SWP, o( a ), o( b ), 0 );
        
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



void xssa_ycodeout::phi(
                xssa_rtgraph* rtg, xssa_block* from, xssa_block* to )
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
    for ( size_t i = 0; i < to->phi.size(); ++i )
    {
        xssaop* phiop = to->phi.at( i );
        if ( ! phiop || phiop->opcode != XSSA_PHI )
        {
            continue;
        }
        
        assert( phiop->r != (uint8_t)-1 );
        assert( phiop->operand_count == to->previous.size() );
        xssaop* source = phiop->operand[ edgeindex ];
        rtg->move( o( phiop->r ), o( source ) );
    }
}

    
void xssa_ycodeout::label( void* label )
{
    auto i = labels.find( label );
    if ( i != labels.end() )
    {
        jumplabel& l = i->second;
        l.label = (int)code.size();
        for ( size_t i = 0 ; i < l.jumps.size(); ++i )
        {
            int jumppc = l.jumps.at( i );
            yinstruction inst = code.at( jumppc );
            signed j = l.label - ( jumppc + 1 );
            code[ jumppc ] = yinstruction( inst.opcode(), inst.r(), j );
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


void xssa_ycodeout::jump( ycode opcode, int r, void* label )
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


void xssa_ycodeout::branch( xssaop* condition, bool iftrue, void* label )
{
    if ( condition->opcode == XSSA_NEXT )
    {
        ycode opcode = iftrue ? Y_IFITER : Y_IFNITER;
        jump( opcode, 0, label );
    }
    else
    {
        ycode opcode = iftrue ? Y_IFTRUE : Y_IFFALSE;
        assert( condition->r != (uint8_t)-1 );
        jump( opcode, o( condition->r ), label );
    }
}




void xssa_ycodeout::build_value_slots( ymodule* module )
{
#if YSLOTS

    // We must sort values into references and numbers, and provide a mapping
    // from their existing indexes to their sorted indexes.  Indexes for
    // numbers are negative, indexes for references are zero or positive.
    std::vector< yvalue > references;
    std::vector< yvalue > numbers;
    std::vector< int > remap;
    
    for ( size_t i = 0; i < values.size(); ++i )
    {
        yvalue value = values.at( i );
        if ( value.is_number() )
        {
            numbers.push_back( value );
            remap.push_back( -(int)numbers.size() );
        }
        else
        {
            remap.push_back( (int)references.size() );
            references.push_back( value );
        }
    }


    // Construct a slots with the values in the appropriate positions.
    yslots* slots = yslots::alloc( values.size() );

    for ( size_t i = 0; i < references.size(); ++i )
    {
        slots->set( i, references.at( i ) );
    }
    
    size_t watermark = references.size();
    slots->set_watermark( watermark );

    for ( size_t i = 0; i < numbers.size(); ++i )
    {
        slots->set( watermark + i, numbers.at( i ) );
    }
    
    
    // Remap every Y_VALUE instruction in every recipe.
    for ( size_t i = 0; i < recipes.size(); ++i )
    {
        yrecipe* recipe = recipes.at( i );
        for ( size_t i = 0; i < recipe->size(); ++i )
        {
            yinstruction* inst = recipe->fcode + i;
            if ( inst->opcode() == Y_VALUE )
            {
                int newc = remap[ inst->c() ];
                if ( newc < 0 )
                {
                    // The number at watermark has newc -1, the next -2, etc.
                    newc = (int)watermark + -newc - 1;
                }
                *inst = yinstruction( Y_VALUE, inst->r(), newc );
            }
        }
    }


    // Assign slots into module.
    module->values = slots;

#endif
}




