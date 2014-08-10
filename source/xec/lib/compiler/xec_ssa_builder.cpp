//
//  xec_ssa_builder.cpp
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ssa_builder.h"
#include <make_unique>
#include "xec_script.h"
#include "xec_ssa.h"
#include "xec_ast_visitor.h"





/*
    Some inspiration taken from:

    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/



/*
    Lvalues and valists.
*/

xec_ssa_lvalue::xec_ssa_lvalue()
    :   opcode( (xec_ssa_opcode)-1 )
    ,   object( NULL )
    ,   index( NULL )
{
}


xec_ssa_valist::xec_ssa_valist()
    :   unpacked( NULL )
{
}




/*
    Where to place nodes or where to link from.
*/

enum xec_ssa_follow_kind
{
    XEC_SSA_FOLLOW_NONE,    // Nowhere, or code is unreachable.
    XEC_SSA_FOLLOW_BLOCK,   // Place nodes in the active block.
    XEC_SSA_FOLLOW_IFTRUE,  // Link as iftrue block.
    XEC_SSA_FOLLOW_IFFALSE, // Link as iffalse block.
};

struct xec_ssa_follow
{
    xec_ssa_follow();
    xec_ssa_follow( xec_ssa_follow_kind kind, xec_ssa_build_block* block );
    void reset( xec_ssa_follow_kind kind, xec_ssa_build_block* block );
    explicit operator bool() const;

    xec_ssa_follow_kind     kind;
    xec_ssa_build_block*    block;
};


xec_ssa_follow::xec_ssa_follow()
    :   kind( XEC_SSA_FOLLOW_NONE )
    ,   block( NULL )
{
}

xec_ssa_follow::xec_ssa_follow(
            xec_ssa_follow_kind kind, xec_ssa_build_block* block )
    :   kind( kind )
    ,   block( block )
{
}

void xec_ssa_follow::reset(
            xec_ssa_follow_kind kind, xec_ssa_build_block* block )
{
    this->kind = kind;
    this->block = block;
}

xec_ssa_follow::operator bool() const
{
    return kind != XEC_SSA_FOLLOW_NONE;
}







/*
    Information about in-progress if.
*/

struct xec_ssa_build_if
{
    xec_ssa_follow          iffalse;
    xec_ssa_follow          aftertrue;
};



/*
    And switch.
*/

struct xec_ssa_build_switch
{
    xec_ssa_build_switch();
    
    xec_ssa_node*           value;
    xec_ssa_build_block*    head;
    xec_ssa_build_block*    prev;
    std::deque< xec_ssa_build_block* > breaks;
};

xec_ssa_build_switch::xec_ssa_build_switch()
    :   head( NULL )
{
}



/*
    And loop.
*/

struct xec_ssa_build_loop
{
    xec_ssa_build_loop();
    
    xec_ssa_build_block*    loop;
    std::deque< xec_ssa_follow > breaks;
};


xec_ssa_build_loop::xec_ssa_build_loop()
    :   loop( NULL )
{
}




/*
    And information about the entire function.
*/

struct xec_ssa_build_func
{
    explicit xec_ssa_build_func( xec_ssa_func* func );

    xec_ssa_func*                       func;
    xec_ssa_follow                      follow;
    std::deque< xec_ssa_build_if >      ifstack;
    std::deque< xec_ssa_build_switch >  switchstack;
    std::deque< xec_ssa_build_loop >    loopstack;
    std::deque< std::unique_ptr< xec_ssa_build_block > > blocks;
    std::unordered_map< xec_ssa_block*, xec_ssa_build_block* > blockmap;
    std::unordered_map< xec_ast_name*, int > upvals;
    int                                 visitmark;
};


xec_ssa_build_func::xec_ssa_build_func( xec_ssa_func* func )
    :   func( func )
    ,   visitmark( -1 )
{
}



/*
    A block which is being built.
*/

struct xec_ssa_build_block
{
    xec_ssa_build_block();

    xec_ssa_block*                              block;
    std::unordered_map< void*, xec_ssa_node* >  defined;
    std::unordered_map< xec_ssa_node*, void* >  incomplete;
    int                                         visitmark;
    bool                                        sealed;
};


xec_ssa_build_block::xec_ssa_build_block()
    :   block( NULL )
    ,   visitmark( -1 )
    ,   sealed( false )
{
}




/*
    The SSA builder itself.
*/


xec_ssa_builder::xec_ssa_builder( xec_ssa* root )
    :   root( root )
    ,   build_expr( this )
    ,   build_unpack( this )
    ,   build_stmt( this )
{
}

xec_ssa_builder::~xec_ssa_builder()
{
}


bool xec_ssa_builder::build( xec_ast* ast )
{
    region_scope rscope( root->alloc );

    // Set script.
    root->script = ast->script;
    
    // Construct function objects.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        xec_ast_func* func = ast->functions.at( i );
        xec_ssa_func* ssaf = new ( root->alloc ) xec_ssa_func(
                func->sloc, func->funcname, NULL, (int)func->upvals.size(),
                (int)func->parameters.size(), func->varargs, func->coroutine );
        root->functions.push_back( ssaf );
        funcmap.emplace( func, ssaf );
    }
    
    // Perform AST -> SSA conversion, one function at a time.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        xec_ast_func* func = ast->functions.at( i );
        xec_ssa_func* ssaf = funcmap.at( func );
        build_function( func, ssaf );
    }
    
    root->function = funcmap.at( ast->function );
    return root->script->error_count() == 0;
}

xec_ssa_node* xec_ssa_builder::node( xec_ssa_node* node )
{
    if ( follow_block() )
    {
        b->follow.block->block->nodes.push_back( node );
    }

    return node;
}


void xec_ssa_builder::define( xec_ast_name* name, xec_ssa_node* value )
{
    if ( name->upval )
    {
        // Upvals are visible outside the function so are not 'locals'.
        auto i = b->upvals.find( name );
        if ( i != b->upvals.end() )
        {
            // Existing upval.
            node( packed( name->sloc, XEC_SSA_SETUP, value, i->second ) );
        }
        else
        {
            // Create new upval.
            int index = b->func->upvalcount + b->func->localupcount;
            b->func->localupcount += 1;
            b->upvals.emplace( name, index );
            node( packed( name->sloc, XEC_SSA_NEWUP, value, index ) );
        }
    }
    else
    {
        // It's a normal local variable, remember definition.
        define_name( name, value );
    }
    
    
    // Also associate definition with a name (for debugging).
    auto i = namemap.find( name );
    xec_ssa_name* ssan;
    if ( i != namemap.end() )
    {
        ssan = i->second;
    }
    else
    {
        ssan = new ( root->alloc ) xec_ssa_name( name->sloc, name->name );
        namemap.emplace( name, ssan );
    }
    
    b->follow.block->block->names.emplace( value, ssan );
}
    

void xec_ssa_builder::define( xec_new_object* object, xec_ssa_node* value )
{
    if ( object->upval )
    {
        // Object upvals are defined once and are read-only, so create an
        // upval but also fall through to define a variable.
        int index = b->func->upvalcount + b->func->localupcount;
        b->func->localupcount += 1;
        node( packed( object->sloc, XEC_SSA_NEWUP, value, index ) );
    }

    define_name( object, value );
}

void xec_ssa_builder::define( xec_ast_node* temporary, xec_ssa_node* value )
{
    define_name( temporary, value );
}


xec_ssa_node* xec_ssa_builder::lookup( int sloc, xec_ast_name* name )
{
    if ( name->upval )
    {
        // If this is an upval then construct code to get its value.
        int index = b->upvals.at( name );
        return node( packed( sloc, XEC_SSA_REFUP, nullptr, index ) );
    }
    else
    {
        // Return reference to last definition, or a ɸ-instruction.
        xec_ssa_node* node = lookup_name( name );
        if ( ! node )
        {
            // All names should have been defined before use.
            root->script->error( sloc,
                        "[!] internal undefined '%s'", name->name );
        }
        return node;
    }
}

xec_ssa_node* xec_ssa_builder::lookup( xec_new_object* object )
{
    // Object upvals are defined once and are read-only, so we can just use
    // the defined value when we want its value locally.
    xec_ssa_node* node = lookup_name( object );
    if ( ! node )
    {
        // All names should have been defined before use.
        root->script->error( object->sloc,
                        "[!] internal undefined object" );
    }
    return node;
}

xec_ssa_node* xec_ssa_builder::lookup( xec_ast_node* temporary )
{
    xec_ssa_node* node = lookup_name( temporary );
    if ( ! node )
    {
        // All names should have been defined before use.
        root->script->error( temporary->sloc,
                        "[!] internal undefined temporary" );
    }
    return node;
}


void xec_ssa_builder::close_scope( xec_ast_scope* scope )
{
    // Close all upvals in scope.
    for ( auto i = scope->decls.rbegin(); i != scope->decls.rend(); ++i )
    {
        xec_ast_name* decl = *i;
        if ( decl->upval )
        {
            // TODO.
        }
    }
    
    // Close object upval if there is one.
    if ( scope->node->kind == XEC_NEW_OBJECT )
    {
        xec_new_object* object = (xec_new_object*)scope->node;
        if ( object->upval )
        {
            // TODO.
        }
    }
}


void xec_ssa_builder::ifthen( xec_ssa_node* condition )
{
    b->ifstack.emplace_back();
    xec_ssa_build_if& buildif = b->ifstack.back();
    
    // We need a real block to use as the decision point.
    if ( follow_block() )
    {
        b->follow.block->block->condition = condition;
        b->follow.reset( XEC_SSA_FOLLOW_IFTRUE, b->follow.block );
        buildif.iffalse.reset( XEC_SSA_FOLLOW_IFFALSE, b->follow.block );
    }
}

void xec_ssa_builder::ifelse()
{
    xec_ssa_build_if& buildif = b->ifstack.back();
    
    // Store current follow as it will be relinked to after the if.
    buildif.aftertrue = b->follow;
    
    // Pick up from the iffalse follow point.
    b->follow = buildif.iffalse;
}

void xec_ssa_builder::ifend()
{
    xec_ssa_build_if& buildif = b->ifstack.back();

    if ( ! buildif.aftertrue )
    {
        // True block ended with a break/continue/return, so the current
        // code is effectively just continuing from the else.
        b->ifstack.pop_back();
        return;
    }
    
    if ( ! b->follow )
    {
        // Else clause ended with a break/continue/return, so the current
        // code effectively continues the iftrue block.
        b->follow = buildif.aftertrue;
        b->ifstack.pop_back();
        return;
    }
    
    // Otherwise we have a join point.
    xec_ssa_build_block* join = make_block();
    link( &buildif.aftertrue, join );
    link( &b->follow, join );
    seal_block( join );
    b->follow.reset( XEC_SSA_FOLLOW_BLOCK, join );
    b->ifstack.pop_back();
}


void xec_ssa_builder::switchopen( xec_ssa_node* value )
{
/*
    b->switchstack.emplace_back();
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    buildswitch.value = value;
    buildswitch.head = b->block;
    b->block = NULL;
*/
}


void xec_ssa_builder::switchcase()
{
/*
    xec_ssa_build_switch& buildswitch = b->switchstack.back();

    // Remember previous current block (for fallthrough).
    buildswitch.prev = b->block;
    
    if ( buildswitch.head )
    {
        if ( buildswitch.head->block->condition )
        {
            // There has already been another comparison.
            xec_ssa_build_block* testblock = make_block();
            link_iffalse( buildswitch.head, testblock );
            seal_block( testblock );
            b->block = testblock;
        }
        else
        {
            // This is the first comparison.
            assert( ! buildswitch.head->block->next );
            b->block = buildswitch.head;
        }
    }
    else
    {
        // Unreachable code.
        b->block = NULL;
    }
*/
}


void xec_ssa_builder::switchcase( xec_ssa_node* value )
{
/*
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    
    xec_ssa_build_block* caseblock = NULL;
    if ( buildswitch.prev )
    {
        if ( buildswitch.prev->block->nodes.empty() )
        {
            // Reuse empty case block from previous case.
            assert( ! buildswitch.prev->block->condition );
            assert( ! buildswitch.prev->block->next );
            assert( buildswitch.prev->block->phi.empty() );
            caseblock = buildswitch.prev;
            
            // Can only do this because the block is totally empty.
            caseblock->sealed = false;
        }
        else
        {
            // Make new case block (with fallthrough).
            caseblock = make_block();
            link_next( buildswitch.prev, caseblock );
        }
        
        buildswitch.prev = NULL;
    }
    else
    {
        // Make new case block (no fallthrough).
        caseblock = make_block();
    }
    
    
    // Add comparison.
    xec_ssa_node* compare = node( packed(
                value->sloc, XEC_SSA_EQ, buildswitch.value, value ) );
    

    // Link case block.
    if ( b->block )
    {
        link_iftrue( b->block, compare, caseblock );
    }
    else
    {
        // Unreachable code.
    }
    
    
    // Next case, the head of the switch will be the current block, so
    // we can chain more cases.
    buildswitch.head = b->block;
    
    
    // And we want to add statements to the case block.
    seal_block( caseblock );
    b->block = caseblock;
*/
}

void xec_ssa_builder::switchbreak()
{
/*
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    
    if ( b->block )
    {
        // This block's next block is the one after the switch.
        buildswitch.breaks.push_back( b->block );
    }
    
    // Further statements are unreachable.
    b->block = NULL;
*/
}

void xec_ssa_builder::switchend()
{
/*
    xec_ssa_build_switch& buildswitch = b->switchstack.back();
    xec_ssa_build_block* after = make_block();
    
    if ( buildswitch.head )
    {
        // Failed comparisons end up outside of the switch.
        if ( buildswitch.head->block->condition )
        {
            link_iffalse( buildswitch.head, after );
        }
        else
        {
            link_next( buildswitch.head, after );
        }
    }
    
    for ( size_t i = 0; i < buildswitch.breaks.size(); ++i )
    {
        // Each break block jumps to after the switch.
        xec_ssa_build_block* breakblock = buildswitch.breaks.at( i );
        link_next( breakblock, after );
    }
    
    seal_block( after );
    b->switchstack.pop_back();
    b->block = after;
*/
}


void xec_ssa_builder::loopopen()
{
    b->loopstack.emplace_back();
    xec_ssa_build_loop& buildloop = b->loopstack.back();
    
    // The entry to the loop is the only kind of block that isn't
    // sealed immediately after creation (we don't know how many
    // continues are going to loop back to it).
    buildloop.loop = make_block();
    link( &b->follow, buildloop.loop );
    b->follow.reset( XEC_SSA_FOLLOW_BLOCK, buildloop.loop );
}

void xec_ssa_builder::loopcontinue()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();

    // Link current follow point to the head of the loop.
    link( &b->follow, buildloop.loop );
    
    // Now unreachable.
    b->follow.reset( XEC_SSA_FOLLOW_NONE, NULL );
}

void xec_ssa_builder::loopbreak()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();
    
    // Remember current follow point so we can break to after the loop.
    buildloop.breaks.push_back( b->follow );

    // Further statements are unreachable.
    b->follow.reset( XEC_SSA_FOLLOW_NONE, NULL );
}

void xec_ssa_builder::loopend()
{
    xec_ssa_build_loop& buildloop = b->loopstack.back();

    // Should have either continued or broken.
    assert( ! b->follow );

    // No more continues.
    seal_block( buildloop.loop );

    // If there were any breaks, then continue from a new block.
    if ( buildloop.breaks.size() )
    {
        xec_ssa_build_block* after = make_block();
        for ( size_t i = 0; i < buildloop.breaks.size(); ++i )
        {
            // Each break block jumps to after the switch.
            xec_ssa_follow& follow = buildloop.breaks.at( i );
            link( &follow, after );
        }
        seal_block( after );
        b->follow.reset( XEC_SSA_FOLLOW_BLOCK, after );
    }

    b->loopstack.pop_back();
}


void xec_ssa_builder::funcreturn()
{
    // Further statements are unreachable.
    b->follow.reset( XEC_SSA_FOLLOW_NONE, NULL );
}


xec_ssa_func* xec_ssa_builder::func( xec_ast_func* func )
{
    return funcmap.at( func );
}

xec_ssa_node* xec_ssa_builder::expr( xec_ast_node* node )
{
    return build_expr.visit( node );
}

void xec_ssa_builder::unpack( xec_ssa_valist* l, xec_ast_node* node, int count )
{
    build_unpack.visit( node, l, count );
    if ( count != -1 )
    {
        assert( (int)l->values.size() == count );
        assert( l->unpacked == NULL );
    }
}



void xec_ssa_builder::lvalue( xec_ssa_lvalue* lvalue, xec_ast_node* node )
{
    lvalue->sloc = node->sloc;

    switch ( node->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        xec_expr_local* local = (xec_expr_local*)node;
        lvalue->opcode = XEC_SSA_NOP;
        lvalue->local  = local->name;
        break;
    }
    
    case XEC_EXPR_UPREF:
    {
        xec_expr_upref* upref = (xec_expr_upref*)node;
        lvalue->opcode = XEC_SSA_SETUP;
        lvalue->upval  = upref->index;
        break;
    }
    
    case XEC_EXPR_KEY:
    {
        xec_expr_key* key = (xec_expr_key*)node;
        lvalue->opcode = XEC_SSA_SETKEY;
        lvalue->object = expr( key->object );
        lvalue->literal = key->key;
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        xec_expr_inkey* inkey = (xec_expr_inkey*)node;
        lvalue->opcode = XEC_SSA_SETINKEY;
        lvalue->object = expr( inkey->object );
        lvalue->index = expr( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        xec_expr_index* index = (xec_expr_index*)node;
        lvalue->opcode = XEC_SSA_SETINDEX;
        lvalue->object = expr( index->object );
        lvalue->index = expr( index->index );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

xec_ssa_node* xec_ssa_builder::lvalue_value( xec_ssa_lvalue* lvalue )
{
    switch ( lvalue->opcode )
    {
    case XEC_SSA_NOP:
        return lookup( lvalue->sloc, lvalue->local );
    
    case XEC_SSA_SETUP:
        return node( packed( lvalue->sloc,
                        XEC_SSA_REFUP, nullptr, lvalue->upval ) );
    
    case XEC_SSA_SETKEY:
        return node( packed( lvalue->sloc,
                        XEC_SSA_KEY, lvalue->object, lvalue->literal ) );
    
    case XEC_SSA_SETINKEY:
        return node( packed( lvalue->sloc,
                        XEC_SSA_INKEY, lvalue->object, lvalue->index ) );
    
    case XEC_SSA_SETINDEX:
        return node( packed( lvalue->sloc,
                        XEC_SSA_INDEX, lvalue->object, lvalue->index ) );
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

xec_ssa_node* xec_ssa_builder::lvalue_assign(
            xec_ssa_lvalue* lvalue, xec_ssa_node* val )
{
    switch ( lvalue->opcode )
    {
    case XEC_SSA_NOP:
        define( lvalue->local, val );
        break;
        
    case XEC_SSA_SETUP:
        node( packed( lvalue->sloc, XEC_SSA_SETUP, val, lvalue->upval ) );
        break;
        
    case XEC_SSA_SETKEY:
        node( triple( lvalue->sloc,
                    XEC_SSA_SETKEY, lvalue->object, lvalue->literal, val ) );
        break;
        
    case XEC_SSA_SETINKEY:
    case XEC_SSA_SETINDEX:
        node( triple( lvalue->sloc,
                    lvalue->opcode, lvalue->object, lvalue->index, val ) );
        break;
        
    default:
        assert( ! "invalid lvalue" );
    }

    return val;
}




void xec_ssa_builder::build_function( xec_ast_func* astf, xec_ssa_func* ssaf )
{
    // Set up function building.
    std::unique_ptr< xec_ssa_build_func > c =
            std::make_unique< xec_ssa_build_func >( ssaf );
    std::swap( b, c );
    
    
    // Open block.
    xec_ssa_build_block* block = make_block();
    seal_block( block );
    ssaf->block = block->block;
    b->follow.reset( XEC_SSA_FOLLOW_BLOCK, block );


    // Add parameters.
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        xec_ast_name* param = astf->parameters.at( i );
        xec_ssa_node* n = node( packed(
                        param->sloc, XEC_SSA_PARAM, nullptr, (int)i ) );
        define( param, n );
    }


    // Visit function body.
    build_stmt.visit( astf->block );
    
    
    // Close scope and return null.
    close_scope( astf->scope );
    xec_ssa_node* nullval = node( packed( astf->sloc, XEC_SSA_NULL ) );
    xec_ssa_expand* retstmt = expand( astf->sloc, XEC_SSA_RETURN, 0 );
    retstmt->operands.push_back( nullval );
    node( retstmt );
    funcreturn();
    
    
    // Clean up after building.
    std::swap( b, c );
}



xec_ssa_build_block* xec_ssa_builder::make_block()
{
    std::unique_ptr< xec_ssa_build_block > block =
                    std::make_unique< xec_ssa_build_block >();
    block->block = new ( root->alloc ) xec_ssa_block();
    b->func->blocks.push_back( block->block );
    b->blockmap.emplace( block->block, block.get() );
    b->blocks.push_back( std::move( block ) );
    return b->blocks.back().get();
}



void xec_ssa_builder::link(
            xec_ssa_follow* follow, xec_ssa_build_block* block )
{
    assert( ! block->sealed );

    switch ( follow->kind )
    {
    case XEC_SSA_FOLLOW_NONE:
    {
        // Unreachable code.
        break;
    }
        
    case XEC_SSA_FOLLOW_BLOCK:
    {
        assert( ! follow->block->block->condition );
        assert( ! follow->block->block->next );
        follow->block->block->next = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    
    case XEC_SSA_FOLLOW_IFTRUE:
    {
        assert( follow->block->block->condition );
        assert( ! follow->block->block->iftrue );
        follow->block->block->iftrue = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    
    case XEC_SSA_FOLLOW_IFFALSE:
    {
        assert( follow->block->block->condition );
        assert( ! follow->block->block->iffalse );
        follow->block->block->iffalse = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    }

}


bool xec_ssa_builder::follow_block()
{
    // Ensure we have a real block.

    switch ( b->follow.kind )
    {
    case XEC_SSA_FOLLOW_NONE:
    {
        // Unreachable code.
        return false;
    }
        
    case XEC_SSA_FOLLOW_BLOCK:
    {
        // Already following.
        return true;
    }
    
    case XEC_SSA_FOLLOW_IFTRUE:
    case XEC_SSA_FOLLOW_IFFALSE:
    {
        // Make real block and link appropriately.
        xec_ssa_build_block* block = make_block();
        link( &b->follow, block );
        b->follow.reset( XEC_SSA_FOLLOW_BLOCK, block );
        return true;
    }
    
    default:
        assert( ! "invalid follow kind" );
        return false;
    }
}



void xec_ssa_builder::define_name( void* name, xec_ssa_node* value )
{
    if ( b->follow.block )
    {
        b->follow.block->defined[ name ] = value;
    }
}



xec_ssa_node* xec_ssa_builder::lookup_name( void* name )
{
    if ( b->follow.block )
    {
        // Look it up.
        b->visitmark += 1;
        xec_ssa_node* node = lookup_name( b->follow.block, name );
        return node;
    }
    else
    {
        // Unreachable.
        return NULL;
    }
}



xec_ssa_node* xec_ssa_builder::lookup_name(
                        xec_ssa_build_block* block, void* name )
{
    // Prevent infinite looping through loops which do not define the name.
    if ( block->visitmark == b->visitmark )
    {
        return NULL;
    }
    block->visitmark = b->visitmark;


    // Look for local definition.
    auto i = block->defined.find( name );
    if ( i != block->defined.end() )
    {
        return i->second;
    }


    if ( block->sealed )
    {
        if ( block->block->previous.size() == 0 )
        {
            // No predecessors, not found.
            return NULL;
        }
        else if ( block->block->previous.size() == 1 )
        {
            // One predecessor, look it up recursively.
            xec_ssa_block* prev = block->block->previous.at( 0 );
            return lookup_name( b->blockmap.at( prev ), name );
        }
        else
        {
            // Multiple predecessors (sealed).
            std::unordered_set< xec_ssa_node* > defines;
            for ( size_t i = 0; i < block->block->previous.size(); ++i )
            {
                xec_ssa_block* prev = block->block->previous.at( i );
                xec_ssa_node* d = lookup_name( b->blockmap.at( prev ), name );
                if ( d )
                {
                    defines.emplace( d );
                }
            }
            
            if ( defines.size() == 0 )
            {
                return NULL;
            }
            else if ( defines.size() == 1 )
            {
                return *defines.begin();
            }
            else
            {
                // Create ɸ-function.
                xec_ssa_expand* phi = expand( -1, XEC_SSA_PHI, 1 );
                for ( auto i = defines.begin(); i != defines.end(); ++i )
                {
                    phi->operands.push_back( *i );
                }
                block->block->phi.push_back( phi );
                define_name( name, phi );
                return phi;
            }
            
        }
    }
    else
    {
        // This is an unsealed block - create incomplete ɸ-function.
        xec_ssa_expand* phi = expand( -1, XEC_SSA_PHI, 1 );
        block->block->phi.push_back( phi );
        block->incomplete.emplace( phi, name );
        define_name( name, phi );
        return phi;
    }

}


void xec_ssa_builder::seal_block( xec_ssa_build_block* block )
{
    assert( ! block->sealed );
    
    // All existing ɸ-functions in the block are incomplete.  Note that this
    // process may create pointless ɸ-functions which refer only to a single
    // other name.
    for ( size_t i = 0; i < block->block->phi.size(); ++i )
    {
        xec_ssa_expand* phi = block->block->phi.at( i )->as_expand();
        void* name = block->incomplete.at( phi );
        
        b->visitmark += 1;
        block->visitmark = b->visitmark;
        
        std::unordered_set< xec_ssa_node* > defines;
        for ( size_t i = 0; i < block->block->previous.size(); ++i )
        {
            xec_ssa_block* prev = block->block->previous.at( i );
            xec_ssa_node* d = lookup_name( b->blockmap.at( prev ), name );
            if ( d && d != phi )
            {
                defines.emplace( d );
            }
        }
        
        if ( defines.size() == 0 )
        {
            // All names should have been defined before use.
            root->script->error( -1, "[!] internal undefined name" );
        }
        
        for ( auto i = defines.begin(); i != defines.end(); ++i )
        {
            phi->operands.push_back( *i );
        }
    }
    
    block->sealed = true;
}



#include "xec_ssa_print.h"


bool xec_ssabuild( xec_ast* ast )
{
    xec_ssa ssa;
    xec_ssa_builder builder( &ssa );
    builder.build( ast );
    xec_ssa_print( &ssa );
    return ast->script->error_count() == 0;
}





