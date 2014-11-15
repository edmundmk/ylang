//
//  yssa_builder.cpp
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "yssa_builder.h"
#include <make_unique>
#include "yl_script.h"
#include "yssa.h"
#include "yl_ast_visitor.h"





/*
    Some inspiration taken from:

    Simple and Efficient Construction of Static Single Assignment Form
        Braun, Sebastian Buchwald, et al.

    http://www.cdl.uni-saarland.de/papers/bbhlmz13cc.pdf
*/





/*
    Loop and undefined markers used when looking up definitions of variables.
*/

static yssaop* const XSSA_UNDEF = (yssaop*)-1;
static yssaop* const XSSA_LOOP  = (yssaop*)-2;
static yssaop* const XSSA_SELF  = (yssaop*)-3;




/*
    Lvalues and valists.
*/

yssa_lvalue::yssa_lvalue()
    :   opcode( XSSA_NOP )
    ,   object( nullptr )
    ,   index( nullptr )
{
}


yssa_valist::yssa_valist()
    :   multival( nullptr )
{
}




/*
    Where to place nodes or where to link from.
*/

enum yssa_follow_kind
{
    XSSA_FOLLOW_NONE,    // Nowhere, or code is unreachable.
    XSSA_FOLLOW_BLOCK,   // Place nodes in the active block.
    XSSA_FOLLOW_IFTRUE,  // Link as iftrue block.
    XSSA_FOLLOW_IFFALSE, // Link as iffalse block.
};

struct yssa_follow
{
    yssa_follow();
    yssa_follow( yssa_follow_kind kind, yssa_build_block* block );
    void reset( yssa_follow_kind kind, yssa_build_block* block );
    explicit operator bool() const;

    yssa_follow_kind     kind;
    yssa_build_block*    block;
};


yssa_follow::yssa_follow()
    :   kind( XSSA_FOLLOW_NONE )
    ,   block( NULL )
{
}

yssa_follow::yssa_follow( yssa_follow_kind kind, yssa_build_block* block )
    :   kind( kind )
    ,   block( block )
{
}

void yssa_follow::reset( yssa_follow_kind kind, yssa_build_block* block )
{
    this->kind = kind;
    this->block = block;
}

yssa_follow::operator bool() const
{
    return kind != XSSA_FOLLOW_NONE;
}







/*
    Information about in-progress if.
*/

struct yssa_build_if
{
    yssa_follow          iffalse;
    yssa_follow          aftertrue;
};



/*
    And loop.
*/

struct yssa_build_loop
{
    yssa_build_loop();
    
    yssa_build_block*       loop;
    std::deque< yssa_follow > breaks;
    std::deque< yssa_follow > continues;
    bool                    dowhile;
};


yssa_build_loop::yssa_build_loop()
    :   loop( NULL )
    ,   dowhile( false )
{
}




/*
    And information about the entire function.
*/

struct yssa_build_func
{
    explicit yssa_build_func( yssa_func* func );

    yssa_func*                       func;
    yssa_follow                      follow;
    std::deque< yssa_build_if >      ifstack;
    std::deque< yssa_build_loop >    loopstack;
    std::deque< yssa_build_block_p > blocks;
    std::unordered_map< yssa_block*, yssa_build_block* > blockmap;
    std::unordered_map< void*, int > upvals;
};


yssa_build_func::yssa_build_func( yssa_func* func )
    :   func( func )
{
}




/*
    A block which is being built.
*/

struct yssa_build_block
{
    yssa_build_block();

    yssa_block*                             block;
    std::unordered_map< void*, yssaop* >    defined;
    std::unordered_map< yssaop*, void* >    incomplete;
    yssaop*                                 phi;
    bool                                    sealed;
};


yssa_build_block::yssa_build_block()
    :   block( nullptr )
    ,   phi( nullptr )
    ,   sealed( false )
{
}




/*
    Results of name lookup can either be a real definition, or a to-be-added
    phi instruction at a join node.
*/

struct yssa_build_lookup
{
    yssa_build_lookup( yssa_build_block* join );
    yssa_build_lookup( yssaop* def );

    explicit operator bool () const;

    yssa_build_block*   join;
    yssaop*             def;
};


bool operator == ( const yssa_build_lookup& a, const yssa_build_lookup& b )
{
    return a.join == b.join && a.def == b.def;
}

bool operator != ( const yssa_build_lookup& a, const yssa_build_lookup& b )
{
    return !( a == b );
}


yssa_build_lookup::yssa_build_lookup( yssa_build_block* join )
    :   join( join )
    ,   def( nullptr )
{
}

yssa_build_lookup::yssa_build_lookup( yssaop* def )
    :   join( nullptr )
    ,   def( def )
{
}

yssa_build_lookup::operator bool() const
{
    return join != nullptr || def != nullptr;
}







/*
    The SSA builder itself.
*/


yssa_builder::yssa_builder( yssa_module* module )
    :   module( module )
    ,   build_expr( this )
    ,   build_unpack( this )
    ,   build_stmt( this )
{
}

yssa_builder::~yssa_builder()
{
}


bool yssa_builder::build( yl_ast* ast )
{
    region_scope rscope( module->alloc );

    // Set script.
    module->script = ast->script;
    
    // Construct function objects.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* func = ast->functions.at( i );

        yssa_func_p ssaf = std::make_unique< yssa_func >();
        ssaf->module        = module;
        ssaf->sloc          = func->sloc;
        ssaf->funcname      = module->alloc.strdup( func->funcname );
        ssaf->upvalcount    = (int)func->upvals.size();
        ssaf->newupcount    = 0;
        ssaf->paramcount    = (int)func->parameters.size();
        ssaf->varargs       = func->varargs;
        ssaf->coroutine     = func->coroutine;

        funcmap.emplace( func, ssaf.get() );
        module->funcs.push_back( std::move( ssaf ) );
    }
    
    // Perform AST -> SSA conversion, one function at a time.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* func = ast->functions.at( i );
        yssa_func* ssaf = funcmap.at( func );
        build_function( func, ssaf );
    }
    
    return module->script->error_count() == 0;
}








yssaop* yssa_builder::op( yssaop* op )
{
    // If we're in a reachable block, add op to block.
    if ( follow_block() )
    {
        b->follow.block->block->ops.push_back( op );
    }

    return op;
}

const char* yssa_builder::key( const char* key )
{
    auto i = keymap.find( key );
    if ( i != keymap.end() )
    {
        return i->second;
    }

    const char* k = module->alloc.strdup( key );
    keymap.emplace( symkey( k ), k );
    return k;
}


void yssa_builder::define( yl_ast_name* name, yssaop* value )
{
    if ( name->upval )
    {
        // Upvals are visible outside the function so are not 'locals'.
        auto i = b->upvals.find( name );
        if ( i != b->upvals.end() )
        {
            // Existing upval.
            yssaop* setup = op( name->sloc, XSSA_SETUP, value );
            setup->immed = i->second;
        }
        else
        {
            // Create new upval.
            int index = b->func->upvalcount + b->func->newupcount;
            b->upvals.emplace( name, index );
            b->func->newupcount += 1;
            yssaop* newup = op( name->sloc, XSSA_NEWUP, value );
            newup->immed = index;
        }
    }
    else
    {
        // It's a normal local variable, remember definition.
        define_name( name, value );
    }
    
    
    // Also associate definition with a name (for debugging).
    if ( b->follow.block )
    {
        auto i = namemap.find( name );
        yssa_string* ssan;
        if ( i != namemap.end() )
        {
            ssan = i->second;
        }
        else
        {
            ssan = yssa_string::s( name->sloc, name->name );
            namemap.emplace( name, ssan );
        }

        b->func->names.emplace( value, ssan );
    }
}
    

void yssa_builder::define( yl_new_object* object, yssaop* value )
{
    if ( object->upval )
    {
        // Object upvals are defined once and are read-only, so create an
        // upval but also fall through to define a variable.
        int index = b->func->upvalcount + b->func->newupcount;
        b->upvals.emplace( object, index );
        b->func->newupcount += 1;
        yssaop* newup = op( object->sloc, XSSA_NEWUP, value );
        newup->immed = index;
    }

    define_name( object, value );
}

void yssa_builder::define( yl_ast_node* temporary, yssaop* value )
{
    define_name( temporary, value );
}


yssaop* yssa_builder::lookup( int sloc, yl_ast_name* name )
{
    if ( name->upval )
    {
        // If this is an upval then construct code to get its value.
        yssaop* refup = op( sloc, XSSA_REFUP );
        refup->immed = b->upvals.at( name );
        return refup;
    }
    else
    {
        // Return reference to last definition, or a ɸ-instruction.
        return lookup_name( name, name->sloc, name->name );
    }
}

yssaop* yssa_builder::lookup( yl_new_object* object )
{
    // Object upvals are defined once and are read-only, so we can just use
    // the defined value when we want its value locally.
    return lookup_name( object, object->sloc, "object" );
}

yssaop* yssa_builder::lookup( yl_ast_node* temporary )
{
    return lookup_name( temporary, temporary->sloc, "temporary" );
}


void yssa_builder::close_scope( yl_ast_scope* scope )
{
    // Close all upvals in scope.
    for ( auto i = scope->decls.rbegin(); i != scope->decls.rend(); ++i )
    {
        yl_ast_name* decl = *i;
        if ( decl->upval )
        {
            yssaop* cloup = op( decl->sloc, XSSA_CLOUP );
            cloup->immed = b->upvals.at( decl );
        }
    }
    
    // Close object upval if there is one.
    if ( scope->node->kind == XEC_NEW_OBJECT )
    {
        yl_new_object* object = (yl_new_object*)scope->node;
        if ( object->upval )
        {
            yssaop* cloup = op( object->sloc, XSSA_CLOUP );
            cloup->immed = b->upvals.at( object );
        }
    }
}


void yssa_builder::ifthen( yssaop* condition )
{
    b->ifstack.emplace_back();
    yssa_build_if& buildif = b->ifstack.back();
    
    // We need a real block to use as the decision point.
    if ( follow_block() )
    {
        b->follow.block->block->condition = condition;
        b->follow.reset( XSSA_FOLLOW_IFTRUE, b->follow.block );
        buildif.iffalse.reset( XSSA_FOLLOW_IFFALSE, b->follow.block );
    }
}

void yssa_builder::ifelse()
{
    yssa_build_if& buildif = b->ifstack.back();
    
    // Store current follow as it will be relinked to after the if.
    buildif.aftertrue = b->follow;
    
    // Pick up from the iffalse follow point.
    b->follow = buildif.iffalse;
}

void yssa_builder::ifend()
{
    yssa_build_if& buildif = b->ifstack.back();

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
    yssa_build_block* join = make_block();
    link( &buildif.aftertrue, join );
    link( &b->follow, join );
    seal_block( join );
    b->follow.reset( XSSA_FOLLOW_BLOCK, join );
    b->ifstack.pop_back();
}


void yssa_builder::switchopen( yssaop* value )
{
    assert( ! "not implemented" );
}


void yssa_builder::switchcase()
{
    assert( ! "not implemented" );
}


void yssa_builder::switchcase( yssaop* value )
{
    assert( ! "not implemented" );
}

void yssa_builder::switchbreak()
{
    assert( ! "not implemented" );
}

void yssa_builder::switchend()
{
    assert( ! "not implemented" );
}


void yssa_builder::loopopen( bool dowhile )
{
    b->loopstack.emplace_back();
    yssa_build_loop& buildloop = b->loopstack.back();
    buildloop.dowhile = dowhile;
    
    // The entry to the loop is the only kind of block that isn't
    // sealed immediately after creation (we don't know how many
    // continues are going to loop back to it).
    if ( b->follow )
    {
        buildloop.loop = make_block();
        link( &b->follow, buildloop.loop );
        b->follow.reset( XSSA_FOLLOW_BLOCK, buildloop.loop );
    }
    else
    {
        buildloop.loop = NULL;
    }
    
}

void yssa_builder::loopcontinue()
{
    yssa_build_loop& buildloop = b->loopstack.back();

    if ( buildloop.dowhile )
    {
        // Remember current follow point so we can continue to the condition.
        buildloop.continues.push_back( b->follow );
    }
    else if ( buildloop.loop )
    {
        // Link current follow point to the head of the loop.
        link( &b->follow, buildloop.loop );
    }
    
    // Now unreachable.
    b->follow.reset( XSSA_FOLLOW_NONE, NULL );
}

void yssa_builder::loopbreak()
{
    yssa_build_loop& buildloop = b->loopstack.back();
    
    // Remember current follow point so we can break to after the loop.
    buildloop.breaks.push_back( b->follow );

    // Further statements are unreachable.
    b->follow.reset( XSSA_FOLLOW_NONE, NULL );
}

void yssa_builder::loopdowhile()
{
    yssa_build_loop& buildloop = b->loopstack.back();

    // This loop has its condition at the end.  If there were any continues
    // they end up here.
    if ( buildloop.continues.empty() )
    {
        // No continues, just keep going onto the condition.
        return;
    }


    // Create block and link all continues.
    yssa_build_block* dowhile = make_block();
    
    if ( b->follow )
    {
        link( &b->follow, dowhile );
    }
    
    for ( size_t i = 0; i < buildloop.continues.size(); ++i )
    {
        yssa_follow& follow = buildloop.continues.at( i );
        link( &follow, dowhile );
    }
    
    seal_block( dowhile );
    
    
    // Any continues after this point should continue to the loop header.
    buildloop.continues.clear();
    buildloop.dowhile = false;
    
    
    // Continue with this block.
    b->follow.reset( XSSA_FOLLOW_BLOCK, dowhile );
}

void yssa_builder::loopend()
{
    yssa_build_loop& buildloop = b->loopstack.back();

    // Should have either continued or broken.
    assert( ! b->follow );

    // No more continues.
    if ( buildloop.loop )
    {
        seal_block( buildloop.loop );
    }

    // If there were any breaks, then code after the loop is reachable.
    if ( buildloop.breaks.size() == 1 )
    {
        // Only a single exit point from loop, continue from it.
        b->follow = buildloop.breaks.front();
    }
    else if ( buildloop.breaks.size() > 1 )
    {
        // New block joining all breaks.
        yssa_build_block* after = make_block();
        for ( size_t i = 0; i < buildloop.breaks.size(); ++i )
        {
            // Each break block jumps to after the switch.
            yssa_follow& follow = buildloop.breaks.at( i );
            link( &follow, after );
        }
        seal_block( after );
        b->follow.reset( XSSA_FOLLOW_BLOCK, after );
    }

    b->loopstack.pop_back();
}


void yssa_builder::funcreturn()
{
    // Further statements are unreachable.
    b->follow.reset( XSSA_FOLLOW_NONE, NULL );
}


yssa_func* yssa_builder::func( yl_ast_func* func )
{
    return funcmap.at( func );
}

yssaop* yssa_builder::expr( yl_ast_node* node )
{
    return build_expr.visit( node );
}

void yssa_builder::unpack( yssa_valist* l, yl_ast_node* node, int count )
{
    build_unpack.visit( node, l, count );
    if ( count != -1 )
    {
        assert( (int)l->values.size() == count );
        assert( ! l->multival );
    }
}



void yssa_builder::lvalue( yssa_lvalue* lvalue, yl_ast_node* node )
{
    lvalue->sloc = node->sloc;

    switch ( node->kind )
    {
    case XEC_EXPR_LOCAL:
    {
        yl_expr_local* local = (yl_expr_local*)node;
        lvalue->opcode = XSSA_NOP;
        lvalue->local  = local->name;
        break;
    }
    
    case XEC_EXPR_GLOBAL:
    {
        yl_expr_global* global = (yl_expr_global*)node;
        lvalue->opcode = XSSA_GLOBAL;
        lvalue->key = key( global->name );
        break;
    }
    
    case XEC_EXPR_UPREF:
    {
        yl_expr_upref* upref = (yl_expr_upref*)node;
        lvalue->opcode = XSSA_SETUP;
        lvalue->upval  = upref->index;
        break;
    }
    
    case XEC_EXPR_KEY:
    {
        yl_expr_key* keyexpr = (yl_expr_key*)node;
        lvalue->opcode = XSSA_SETKEY;
        lvalue->object = expr( keyexpr->object );
        lvalue->key = key( keyexpr->key );
        break;
    }
    
    case XEC_EXPR_INKEY:
    {
        yl_expr_inkey* inkey = (yl_expr_inkey*)node;
        lvalue->opcode = XSSA_SETINKEY;
        lvalue->object = expr( inkey->object );
        lvalue->index = expr( inkey->key );
        break;
    }
    
    case XEC_EXPR_INDEX:
    {
        yl_expr_index* index = (yl_expr_index*)node;
        lvalue->opcode = XSSA_SETINDEX;
        lvalue->object = expr( index->object );
        lvalue->index = expr( index->index );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

yssaop* yssa_builder::lvalue_value( yssa_lvalue* lvalue )
{
    switch ( lvalue->opcode )
    {
    case XSSA_NOP:
    {
        return lookup( lvalue->sloc, lvalue->local );
    }
    
    case XSSA_GLOBAL:
    {
        yssaop* global = op( lvalue->sloc, XSSA_GLOBAL );
        global->key = lvalue->key;
        return global;
    }
    
    case XSSA_SETUP:
    {
        yssaop* refup = op( lvalue->sloc, XSSA_REFUP );
        refup->immed = lvalue->upval;
        return refup;
    }
    
    case XSSA_SETKEY:
    {
        yssaop* key = op( lvalue->sloc, XSSA_KEY, lvalue->object );
        key->key = lvalue->key;
        return key;
    }
    
    case XSSA_SETINKEY:
    {
        return op( lvalue->sloc, XSSA_INKEY, lvalue->object, lvalue->index );
    }
    
    case XSSA_SETINDEX:
    {
        return op( lvalue->sloc, XSSA_INDEX, lvalue->object, lvalue->index );
    }
    
    default:
        assert( ! "invalid lvalue" );
        break;
    }
}

void yssa_builder::lvalue_assign(
            yssa_lvalue* lvalue, yssaop* val )
{
    switch ( lvalue->opcode )
    {
    case XSSA_NOP:
        define( lvalue->local, val );
        break;
        
    case XSSA_GLOBAL:
    {
        yssaop* setglobal = op( lvalue->sloc, XSSA_SETGLOBAL, val );
        setglobal->key = lvalue->key;
        break;
    }
        
    case XSSA_SETUP:
    {
        yssaop* setup = op( lvalue->sloc, XSSA_SETUP, val );
        setup->immed = lvalue->upval;
        break;
    }

    case XSSA_SETKEY:
    {
        yssaop* setkey = op( lvalue->sloc, XSSA_SETKEY, lvalue->object, val );
        setkey->key = lvalue->key;
        break;
    }
    
    case XSSA_SETINKEY:
    case XSSA_SETINDEX:
    {
        op( lvalue->sloc, lvalue->opcode, lvalue->object, lvalue->index, val );
        break;
    }
        
    default:
        assert( ! "invalid lvalue" );
    }
}






void yssa_builder::build_function( yl_ast_func* astf, yssa_func* ssaf )
{
    // Set up function building.
    yssa_build_func_p c = std::make_unique< yssa_build_func >( ssaf );
    std::swap( b, c );
    
    
    // Open block.
    yssa_build_block* block = make_block();
    seal_block( block );
    b->follow.reset( XSSA_FOLLOW_BLOCK, block );


    // Add parameters.
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        yl_ast_name* param = astf->parameters.at( i );
        yssaop* n = op( param->sloc, XSSA_PARAM );
        n->immed = (int)i;
        define( param, n );
    }


    // Visit function body.
    build_stmt.visit( astf->block );
    
    
    // Close scope and return nothing.
    close_scope( astf->scope );
    yssaop* r = op( astf->sloc, XSSA_RETURN );
    r->multival = nullptr;
    funcreturn();
    
    
    // Go through each block and remove reference ops.
    for ( size_t i = 0; i < b->blocks.size(); ++i )
    {
        yssa_build_block* block = b->blocks.at( i ).get();
        remove_ref( block->block );
    }
    
    
    // Clean up after building.
    std::swap( b, c );
    
    
}



yssa_build_block* yssa_builder::make_block()
{
    yssa_build_block_p build = std::make_unique< yssa_build_block >();
    yssa_block_p block = std::make_unique< yssa_block >();
    
    block->index        = (int)b->func->blocks.size();
    block->condition    = nullptr;
    block->iftrue       = nullptr;
    block->iffalse      = nullptr;
    
    build->block = block.get();
    b->blockmap.emplace( block.get(), build.get() );
    b->func->blocks.push_back( std::move( block ) );
    b->blocks.push_back( std::move( build ) );
    return b->blocks.back().get();
}



void yssa_builder::link( yssa_follow* follow, yssa_build_block* block )
{
    assert( ! block->sealed );

    switch ( follow->kind )
    {
    case XSSA_FOLLOW_NONE:
    {
        // Unreachable code.
        break;
    }
        
    case XSSA_FOLLOW_BLOCK:
    {
        assert( ! follow->block->block->condition );
        assert( ! follow->block->block->next );
        follow->block->block->next = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    
    case XSSA_FOLLOW_IFTRUE:
    {
        assert( follow->block->block->condition );
        assert( ! follow->block->block->iftrue );
        follow->block->block->iftrue = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    
    case XSSA_FOLLOW_IFFALSE:
    {
        assert( follow->block->block->condition );
        assert( ! follow->block->block->iffalse );
        follow->block->block->iffalse = block->block;
        block->block->previous.push_back( follow->block->block );
        break;
    }
    }

}


bool yssa_builder::follow_block()
{
    // Ensure we have a real block.

    switch ( b->follow.kind )
    {
    case XSSA_FOLLOW_NONE:
    {
        // Unreachable code.
        return false;
    }
        
    case XSSA_FOLLOW_BLOCK:
    {
        // Already following.
        return true;
    }
    
    case XSSA_FOLLOW_IFTRUE:
    case XSSA_FOLLOW_IFFALSE:
    {
        // Make real block and link appropriately.
        yssa_build_block* block = make_block();
        link( &b->follow, block );
        seal_block( block );
        b->follow.reset( XSSA_FOLLOW_BLOCK, block );
        return true;
    }
    
    default:
        assert( ! "invalid follow kind" );
        return false;
    }
}



void yssa_builder::define_name( void* name, yssaop* value )
{
    if ( b->follow.block )
    {
        b->follow.block->defined[ name ] = value;
    }
}



yssaop* yssa_builder::lookup_name(
                    void* name, int sloc, const char* text )
{
    if ( b->follow )
    {
        // Look it up.
        yssa_build_lookup lookup =
                        lookup_block( nullptr, b->follow.block, name );
        

        // Should have resolved to a real definition.
        assert( ! lookup.join );
        assert( lookup.def != XSSA_UNDEF );

        
        return lookup.def;
    }
    else
    {
        // Unreachable.
        return nullptr;
    }
}





/*
    Perform recursive name lookup.  Note that we assume a reducible CFG.
*/


yssaop* yssa_builder::lookup_block(
        yssa_build_block* loop, yssa_build_block* block, void* name )
{
    // If there is a local definition then use that.
    auto i = block->defined.find( name );
    if ( i != block->defined.end() )
    {
        return i->second;
    }
    
    
    // Check for a return to the header of a loop (and there is no definition).
    if ( block == loop )
    {
        return XSSA_LOOP;
    }
    
    
    // If it's an unsealed block then we can't proceed - add an incomplete phi.
    if ( ! block->sealed )
    {
        yssaop* phi = yssaop::op( -1, XSSA_PSI, nullptr );
        block->block->phi.push_back( phi );
        block->incomplete.emplace( phi, name );
        block->defined[ name ] = phi;
        return phi;
    }
    
    
    // If there's no predecessors then the name is undefined.
    if ( block->block->previous.size() == 0 )
    {
        return XSSA_UNDEF;
    }
    
    
    // If there's only one predecessor then continue search.
    if ( block->block->previous.size() == 1 )
    {
        yssa_block* prev = block->block->previous.at( 0 );
        yssa_build_block* build = b->blockmap.at( prev );
        yssaop* def = lookup_block( loop, build, name );
        
        if ( def )
        {
            // Cache resolved definition.
            block->defined[ name ] = def;
        }
        
        return def;
    }
    
    
    // This is a join node without a local definition of the value.  Set up
    // to potentially create a phi operation.
    block->phi = nullptr;
    
    
    // Look up the def reachable from each predecessor block, in order.
    std::vector< yssaop* > lookups;
    yssaop* def = lookup_join( loop, block, name, &lookups );
    if ( def )
    {
        // Join collapses away.
        
        if ( def != XSSA_UNDEF )
        {
            block->defined[ name ] = def;
        }
        
        return def;
    }


    // Actually is a ɸ-function.
    ensure_phi( block );
    assert( block->phi );
    
    // Build phi list.
    yssaop* phi = yssaop::op( -1, XSSA_PHI, lookups.size() );
    for ( size_t i = 0; i < lookups.size(); ++i )
    {
        phi->operand[ i ] = lookups[ i ];
    }
    block->phi->operand[ 0 ] = phi;
    
    // The phi is the definition.
    block->defined[ name ] = block->phi;
    return block->phi;
}



void yssa_builder::seal_block( yssa_build_block* block )
{
    // Seal block.
    assert( ! block->sealed );
    block->sealed = true;
    

    // All ɸ-functions should be incomplete.
    for ( size_t i = 0; i < block->block->phi.size(); ++i )
    {

        // We already created an incomplete phi for this name in this block.
        block->phi = block->block->phi.at( i );
        
        
        // Lookup name.
        void* name = block->incomplete.at( block->phi );
        std::vector< yssaop* > lookups;
        yssaop* def = lookup_join( NULL, block, name, &lookups );
        if ( def )
        {
            // Join collapses away.
            
            // Change the phi to a ref.
            block->phi->opcode          = XSSA_REF;
            block->phi->operand[ 0 ]   = def;
            
            // Update definition.
            if ( def != XSSA_UNDEF && block->defined.at( name ) == block->phi )
            {
                block->defined[ name ] = def;
            }
            
            assert( def != XSSA_UNDEF );
            continue;
            
        }
        
     
        // Fill in the incomplete ɸ-function.
        yssaop* phi = yssaop::op( -1, XSSA_PHI, lookups.size() );
        for ( size_t i = 0; i < lookups.size(); ++i )
        {
            phi->operand[ i ] = lookups[ i ];
        }
        block->phi->operand[ 0 ] = phi;
        
    }


    // All names should be resolved.
    block->incomplete.clear();

}



yssaop* yssa_builder::lookup_join(
        yssa_build_block* loop, yssa_build_block* block,
                void* name, std::vector< yssaop* >* lookups )
{
    assert( block->block->previous.size() > 1 );
    lookups->reserve( block->block->previous.size() );


    // Check all previous references.
    for ( size_t i = 0; i < block->block->previous.size(); ++i )
    {
        yssa_block* prev = block->block->previous.at( i );
        yssa_build_block* build_prev = b->blockmap.at( prev );
        
        // Perform lookup by following edge.
        yssaop* def = nullptr;
        if ( build_prev->block->index >= block->block->index )
        {
            // This is a back-edge.  Perform lookup restricted to the subtree
            // dominated by this loop header.
            def = lookup_block( block, build_prev, name );
            if ( def == XSSA_LOOP )
            {
                // Reached the header of the sub-loop - this block.
                def = XSSA_SELF;
            }
        }
        else
        {
            // Continue search normally with existing loop horizon.
            def = lookup_block( loop, build_prev, name );
        }
        
        if ( def == XSSA_UNDEF )
        {
            // Undefined on at least one code path, undefined.
            return XSSA_UNDEF;
        }
        
        lookups->push_back( def );
    }
    
    
    // Attempt to collapse to a single definition - we can do this if all
    // defs are either SELF (or block->phiref) or one other value.
    yssaop* single = nullptr;
    size_t i = 0;
    for ( ; i < lookups->size(); ++i )
    {
        yssaop* def = lookups->at( i );
        if ( def != XSSA_SELF && def != block->phi )
        {
            single = def;
            break;
        }
    }
    
    for ( ; i < lookups->size(); ++i )
    {
        yssaop* def = lookups->at( i );
        if ( def != XSSA_SELF && def != block->phi && def != single )
        {
            single = nullptr;
            break;
        }
    }
    
    if ( single )
    {
        // Return collapsed value.
        return single;
    }
    
    
    // Otherwise, we cannot collapse and must return def list to create a
    // ɸ-function.  But first, we must replace any SELF and LOOP markers with
    // references to real ɸ-ops.  If this join is uncollapsible, then so too
    // will be the join at the containing loop header.
    for ( size_t i = 0; i < lookups->size(); ++i )
    {
        yssaop* def = lookups->at( i );
        
        if ( def == XSSA_LOOP )
        {
            lookups->at( i ) = ensure_phi( loop );
        }
        else if ( def == XSSA_SELF )
        {
            lookups->at( i ) = ensure_phi( block );
        }
    }
    
    
    return nullptr;
}



yssaop* yssa_builder::ensure_phi( yssa_build_block* block )
{
    if ( ! block->phi )
    {
        // Create phi (without payload).
        block->phi = yssaop::op( -1, XSSA_PSI, nullptr );
        block->block->phi.push_back( block->phi );
    }
    
    return block->phi;
}






void yssa_builder::remove_ref( yssa_block* block )
{
    for ( size_t i = 0; i < block->phi.size(); ++i )
    {
        yssaop* op = block->phi.at( i );

        if ( op->opcode == XSSA_PSI )
        {
            op = op->operand[ 0 ];
            block->phi[ i ] = op;
            
            for ( size_t i = 0; i < op->operand_count; ++i )
            {
                if ( op->operand[ i ] )
                {
                    op->operand[ i ] = remove_ref( op->operand[ i ] );
                }
            }
        }
        else if ( op->opcode == XSSA_REF )
        {
            block->phi[ i ] = nullptr;
        }
    }
    
    for ( size_t i = 0; i < block->ops.size(); ++i )
    {
        yssaop* op = block->ops.at( i );
        
        if ( yssaop::has_multival( op->opcode ) && op->multival )
        {
            op->multival = remove_ref( op->multival );
        }
        
        for ( size_t i = 0; i < op->operand_count; ++i )
        {
            if ( op->operand[ i ] )
            {
                op->operand[ i ] = remove_ref( op->operand[ i ] );
            }
        }
    }
    
    if ( block->condition )
    {
        block->condition = remove_ref( block->condition );
    }
}


yssaop* yssa_builder::remove_ref( yssaop* op )
{
    while ( op->opcode == XSSA_REF || op->opcode == XSSA_PSI )
    {
        op = op->operand[ 0 ];
    }
    return op;
}






