//
//  yssa_codegen.cpp
//
//  Created by Edmund Kapusniak on 04/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_codegen.h"
#include <make_unique.h>
#include "ylang.h"
#include "yssa.h"
#include "yl_string.h"
#include "yl_function.h"
#include "yl_program.h"


struct ygen_module;
struct ygen_program;
struct ygen_string;
struct ygen_emit;

typedef std::unique_ptr< ygen_program > ygen_program_p;
typedef std::unique_ptr< ygen_string >  ygen_string_p;


/*
    Each function has an array of constant values used in the function.
    This includes lookup keys and program objects.
*/


enum ygen_value_kind
{
    YGEN_KEY,
    YGEN_STRING,
    YGEN_NUMBER,
    YGEN_PROGRAM,
};


struct ygen_value
{
    ygen_value_kind kind;
    union
    {
        ygen_string*    string;
        double          number;
        ygen_program*   program;
    };
};



/*
    Construct unmanaged 
*/


struct ygen_module
{
    std::unordered_map< symkey, ygen_string_p > strings;
    std::unordered_map< yssa_function*, ygen_program_p > programs;
};


struct ygen_program
{
    explicit ygen_program( yssa_function* ssafunc );

    yssa_function*              ssafunc;
    yl_rootref< yl_program >    program;

    std::vector< ygen_value >   values;
    std::vector< yl_opinst >    ops;
    std::vector< yl_xframe >    xframes;
    std::vector< yssa_variable* > debugvars;
    std::vector< yl_debugspan > debugspans;

    std::unordered_map< symkey, size_t > strvals;
    std::unordered_map< double, size_t > numvals;
    std::unordered_map< yssa_function*, size_t > funvals;
    
    size_t                      stackcount;
    size_t                      localupcount;
    size_t                      itercount;
};

ygen_program::ygen_program( yssa_function* ssafunc )
    :   ssafunc( ssafunc )
    ,   program( nullptr )
    ,   stackcount( 0 )
    ,   localupcount( 0 )
    ,   itercount( 0 )
{
}


struct ygen_string
{
    ygen_string( const char* text, size_t size );

    yl_rootref< yl_string >     string;

    const char*                 text;
    size_t                      size;
    bool                        iskey;
};


ygen_string::ygen_string( const char* text, size_t size )
    :   string( nullptr )
    ,   text( text )
    ,   size( size )
    ,   iskey( false )
{
}




/*
    A register transfer graph represents moves that happen in parallel.
    It has nodes representing registers and edges representing moves.
    Each node has at most a single incoming edge, but may have many
    outgoing edges.  We can separate the graph into:

      - Cycles.
      - Paths which start at a node with no outgoing edges and follow
            edges backwards to either a node with no incoming edges, or
            a node which is part of a cycle.

    We can perform the register transfers by following this algorithm:

      - Count the number of outgoing edges for each node.
      - While there is at least one node with a zero count:
          - Perform move to this node A from its source node B.
          - Decrement outgoing edge count of source node A.
          - Remove node A from set.
      - The remaining nodes form loops.  While nodes remain:
          - Swap values between a random node A and its source node B.
          - Remove edge between nodes A and B.
          - Create a new edge between node A and the target of node B.
          - Remove node A from set.

    Or, a version of the loop-swapping algorithm that doesn't require us to
    traverse the graph forwards along edges:

      - Pick a random node C.
      - Find its source node B and B's source node A.
      - If A == C then swap the two nodes, and remove both from set.
      - Otherwise swap A and B, and
          - remove B, and
          - change the source node of C to be A.
*/


struct ygen_movgraph
{
    explicit operator bool ();
    void move( unsigned target, unsigned source );
    void emit( ygen_program* p );
    
    std::unordered_map< unsigned, unsigned > moves;
};


ygen_movgraph::operator bool()
{
    return ! moves.empty();
}

void ygen_movgraph::move( unsigned target, unsigned source )
{
    if ( target != source )
    {
        moves[ target ] = source;
    }
}

void ygen_movgraph::emit( ygen_program* p )
{
    // Count outgoing edges.
    std::unordered_map< unsigned, unsigned > outcount;
    for ( auto move : moves )
    {
        unsigned source = move.second;
        outcount[ source ] += 1;
    }
    
    // Build list of nodes with zero outcount.
    std::unordered_set< unsigned > zero;
    std::unordered_set< unsigned > nonzero;
    for ( auto move : moves )
    {
        unsigned target = move.first;
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
        unsigned target = *i;
        unsigned source = moves.at( target );
        
        // Perform move.
        p->ops.emplace_back( YL_MOV, target, source, 0 );
        p->stackcount = std::max( p->stackcount, (size_t)target + 1 );
        
        // Target is no longer in the set.
        zero.erase( i );
        
        // Decrement outcount for source.
        outcount[ source ] -= 1;
        if ( outcount[ source ] == 0
                && nonzero.find( source ) != nonzero.end() )
        {
            nonzero.erase( source );
            zero.insert( source );
        }
    }
    
    // Remaining nodes are in cycles.
    while ( nonzero.size() )
    {
        auto i = nonzero.begin();
        unsigned c = *i;
        unsigned b = moves.at( c );
        unsigned a = moves.at( b );
        
        // Swap.
        p->ops.emplace_back( YL_SWP, a, b, 0 );
        
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
            moves[ c ] = a;
        }
    }
}






/*
    Codegen.
*/


class ygen_emit
{
public:

    explicit ygen_emit( ygen_module* m );

    void codegen_function( yssa_function* function );

    void make_string( ygen_string* s );
    void make_program( ygen_program* p );

    void emit( ygen_program* p );
    
private:

    ygen_string*    add_string( symkey k );
    ygen_program*   add_program( yssa_function* function );

    void            stack( ygen_program* p, uint8_t stacktop, uint8_t rcount );

    unsigned        operand( yssa_opinst* op, size_t index );
    void            arguments( ygen_program* p, size_t index, unsigned first );
    size_t          select( ygen_program* p, size_t index );
    size_t          opgen( ygen_program* p, size_t index );
    
    ygen_module*    m;

};



yl_function yssa_codegen( yssa_module* module )
{
    ygen_module m;
    ygen_emit emit( &m );
    
    // Generate each function.
    for ( size_t i = 0; i < module->functions.size(); ++i )
    {
        yssa_function* function = module->functions.at( i ).get();
        emit.codegen_function( function );
    }


    // Construct final heap objects.
    for ( const auto& s : m.strings )
    {
        emit.make_string( s.second.get() );
    }
    
    for ( const auto& p : m.programs )
    {
        emit.make_program( p.second.get() );
    }
    
    
    // Emit code and fixup references between objects.
    for ( const auto& p : m.programs )
    {
        emit.emit( p.second.get() );
    }
    

    // Create invoke.
    yssa_function* ssafunc = module->functions.at( 0 ).get();
    yl_rootref< yl_program > program = m.programs.at( ssafunc )->program;
    yl_rootref< yl_funcobj > funcobj = yl_funcobj::alloc( program.get() );
    return yl_funcobj::make_function( funcobj.get() );
}




ygen_emit::ygen_emit( ygen_module* m )
    :   m( m )
{
}



ygen_string* ygen_emit::add_string( symkey k )
{
    // Find string.
    auto i = m->strings.find( k );
    if ( i != m->strings.end() )
    {
        return i->second.get();
    }
    else
    {
        ygen_string_p string =
                std::make_unique< ygen_string >( k.c_str(), k.size() );
        ygen_string* s = string.get();
        m->strings.emplace( k, std::move( string ) );
        return s;
    }
}


ygen_program* ygen_emit::add_program( yssa_function* function )
{
    // Get (or create) program object.
    auto i = m->programs.find( function );
    if ( i != m->programs.end() )
    {
        return i->second.get();
    }
    else
    {
        ygen_program_p program =
                std::make_unique< ygen_program >( function );
        ygen_program* p = program.get();
        m->programs.emplace( function, std::move( program ) );
        return p;
    }
}


void ygen_emit::codegen_function( yssa_function* function )
{
    ygen_program* p = add_program( function );
    
    
    // Every program has one hidden parameter, the function object.
    assert( p->stackcount == 0 );
    p->stackcount += 1;
    
    
    // Extract all constants.
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );
        switch ( op->opcode )
        {
        case YL_GLOBAL:
        case YL_SETGLOBAL:
        case YL_KEY:
        case YL_SETKEY:
        case YL_DELKEY:
        {
            symkey k( op->key );
            auto i = p->strvals.find( k );
            if ( i != p->strvals.end() )
            {
                // Upgrade to key, in case value was used earlier as string.
                ygen_value& value = p->values.at( i->second );
                value.kind = YGEN_KEY;
                value.string->iskey = true;
            }
            else
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind = YGEN_KEY;
                value.string = add_string( k );
                value.string->iskey = true;
                p->values.push_back( value );
                p->strvals.emplace( k, index );
            }
            break;
        }
        
        case YL_NUMBER:
        {
            if ( ! p->numvals.count( op->number ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_NUMBER;
                value.number    = op->number;
                p->values.push_back( value );
                p->numvals.emplace( op->number, index );
            }
            break;
        }
        
        case YL_STRING:
        {
            symkey k( op->string->string, op->string->length );
            if ( ! p->strvals.count( k ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_STRING;
                value.string    = add_string( k );
                p->values.push_back( value );
                p->strvals.emplace( k, index );
            }
            break;
        }
        
        case YL_FUNCTION:
        {
            if ( ! p->funvals.count( op->function ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_PROGRAM;
                value.program   = add_program( op->function );
                p->values.push_back( value );
                p->funvals.emplace( op->function, index );
            }
            break;
        }
        
        default:
            break;
        }
    }
    
    
    // And sort the values.
    std::vector< size_t > sorted;
    sorted.reserve( p->values.size() );
    for ( size_t i = 0; i < p->values.size(); ++i )
    {
        sorted.push_back( i );
    }
    
    std::sort
    (
        sorted.begin(),
        sorted.end(),
        [=]( size_t a, size_t b )
        {
            const ygen_value& aval = p->values.at( a );
            const ygen_value& bval = p->values.at( b );
            
            if ( aval.kind < bval.kind )
                return true;
            if ( aval.kind != bval.kind )
                return false;
            
            switch ( aval.kind )
            {
            case YGEN_KEY:
            case YGEN_STRING:
                return strcmp( aval.string->text, bval.string->text ) < 0;
            
            case YGEN_NUMBER:
                return aval.number < bval.number;

            case YGEN_PROGRAM:
                return a <  b;
            }
            
        }
    );
    
    
    // Construct each block.
    std::vector< size_t > indexes;
    std::vector< std::pair< size_t, size_t > > jumps;
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
    
        // Compile each op in block.
        size_t count = 0;
        for ( size_t i = block->lstart; i < block->lfinal; i += count )
        {
            // Ops should be in sequential block order, with no gaps.
            assert( indexes.size() == i );
            
            // 'count' SSA ops map to one or more VM ops.
            size_t opindex = p->ops.size();
            count = opgen( p, i );
            indexes.insert( indexes.end(), count, opindex );
        }
        
        // Find next block.
        yssa_block* next_block = nullptr;
        if ( i + 1 < function->blocks.size() )
        {
            next_block = function->blocks.at( i + 1 ).get();
        }

        // Compile jumps.
        if ( block->test )
        {
            assert( block->next );
            assert( block->fail );
            
            assert( block->test->r != yl_opinst::NOVAL );
            unsigned r = block->test->r;
            yl_opcode jmpt;
            yl_opcode jmpf;
            
            if ( block->test->opcode != YSSA_ITEREACH )
            {
                jmpt = YL_JMPT;
                jmpf = YL_JMPF;
            }
            else
            {
                jmpt = YL_JMPV;
                jmpf = YL_JMPN;
            }
            
            if ( block->next == next_block )
            {
                size_t index = p->ops.size();
                p->ops.emplace_back( jmpf, r, (signed)0 );
                jumps.emplace_back( index, block->fail->lstart );
            }
            else if ( block->fail == next_block )
            {
                size_t index = p->ops.size();
                p->ops.emplace_back( jmpt, r, (signed)0 );
                jumps.emplace_back( index, block->next->lstart );
            }
            else
            {
                size_t index = p->ops.size();
                p->ops.emplace_back( jmpt, r, (signed)0 );
                jumps.emplace_back( index, block->next->lstart );
                index = p->ops.size();
                p->ops.emplace_back( YL_JMP, 0, (signed)0 );
                jumps.emplace_back( index, block->fail->lstart );
            }
        }
        else if ( block->next && block->next != next_block )
        {
            size_t index = p->ops.size();
            p->ops.emplace_back( YL_JMP, 0, (signed)0 );
            jumps.emplace_back( index, block->next->lstart );
        }
    }
    
    indexes.push_back( p->ops.size() );
    
    
    // Fix up jumps.
    for ( auto jump : jumps )
    {
        size_t index = jump.first;
        size_t target = indexes.at( jump.second );
        signed j = (signed)target - (signed)( index + 1 );
        yl_opinst& op = p->ops.at( index );
        op = yl_opinst( op.opcode(), op.r(), j );
    }
    
    
    // Construct xframes.
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
        
        // Skip blocks without exception handlers.
        if ( ! block->xchandler )
        {
            continue;
        }
        
        // Construct xframe.
        yl_xframe xf;
        xf.start            = (unsigned)indexes.at( block->lstart );
        xf.end              = (unsigned)indexes.at( block->lfinal );
        xf.close_upvals     = block->xchandler->xclocalups;
        xf.close_iterators  = block->xchandler->xcitercount;
        xf.handler          = (unsigned)indexes.at( block->xchandler->lstart );
        
        // Check if we can just extend the previous xframe.
        if ( p->xframes.size()
                && p->xframes.back().end == xf.start
                && p->xframes.back().handler == xf.handler )
        {
            yl_xframe& merge = p->xframes.back();
            assert( merge.close_upvals == xf.close_upvals );
            assert( merge.close_iterators == xf.close_iterators );
            merge.end = xf.end;
            continue;
        }
        
        // Otherwise add it.
        p->xframes.push_back( xf );
    }
    
    
    // The first n debugvars are the names of upvals.
    for ( size_t i = 0; i < function->upnames.size(); ++i )
    {
        p->debugvars.push_back( nullptr );
    }
    

    // Construct debug information for variables.
    std::unordered_set< yssa_variable* > variables;
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );

        if ( op->has_associated() || ! op->variable )
        {
            continue;
        }
        
        yssa_variable* v = op->variable;
        if ( variables.count( v ) )
        {
            continue;
        }
        
        unsigned varindex = (unsigned)p->debugvars.size();
        p->debugvars.push_back( v );
        variables.insert( v );
        
        for ( yssa_live_range* live = v->live; live; live = live->next )
        {
            yl_debugspan span;
            span.varindex   = varindex;
            span.start      = (unsigned)indexes.at( live->start );
            span.end        = (unsigned)indexes.at( live->final );
            
            if ( p->debugspans.size()
                    && p->debugspans.back().varindex == varindex
                    && p->debugspans.back().end == span.start )
            {
                p->debugspans.back().end = span.end;
                continue;
            }
            
            p->debugspans.push_back( span );
        }
    }
    
    std::sort
    (
        p->debugspans.begin(),
        p->debugspans.end(),
        []( const yl_debugspan& a, const yl_debugspan& b )
        {
            if ( a.start < b.start )
                return true;
            if ( a.start == b.start && a.end < b.end )
                return true;
            return false;
        }
    );
}



void ygen_emit::stack( ygen_program* p, uint8_t stacktop, uint8_t rcount )
{
    if ( rcount != yl_opinst::NOVAL )
    {
        p->stackcount = std::max( p->stackcount, (size_t)stacktop + rcount );
    }
}


unsigned ygen_emit::operand( yssa_opinst* op, size_t index )
{
    if ( index < op->operand_count )
    {
        assert( op->operand[ index ] );
        assert( op->operand[ index ]->r != yl_opinst::NOVAL );
        return op->operand[ index ]->r;
    }
    else
    {
        return 0;
    }
}


void ygen_emit::arguments( ygen_program* p, size_t index, unsigned first )
{
    yssa_function* function = p->ssafunc;
    yssa_opinst* op = function->ops.at( index );

    ygen_movgraph arguments;
    for ( unsigned i = first; i < op->operand_count; ++i )
    {
        assert( op->operand[ i ] );
        assert( op->operand[ i ]->r != yl_opinst::NOVAL );
        arguments.move( op->stacktop + i - first, op->operand[ i ]->r );
    }

    if ( op->multival )
    {
        assert( op->multival->r == op->stacktop + op->operand_count );
        if ( op->multival->is_call() )
        {
            assert( op->multival->r == op->multival->stacktop );
        }
    }
    
    arguments.emit( p );
}


size_t ygen_emit::select( ygen_program* p, size_t index )
{
    yssa_function* function = p->ssafunc;
    yssa_opinst* op = function->ops.at( index );

    ygen_movgraph results;
    if ( op->r != yl_opinst::NOVAL )
    {
        results.move( op->r, op->stacktop );
    }
    
    size_t n;
    for ( n = 1; index + n < function->ops.size(); ++n )
    {
        yssa_opinst* sel = function->ops.at( index + n );
        if ( sel->opcode == YSSA_SELECT )
        {
            assert( sel->operand_count == 1 );
            assert( sel->operand[ 0 ] == op );
            if ( sel->r != yl_opinst::NOVAL )
            {
                results.move( sel->r, op->stacktop + sel->select );
            }
        }
        else if ( sel->opcode == YSSA_IMPLICIT )
        {
            // Ignore YSSA_IMPLICIT ops.
        }
        else
        {
            break;
        }
    }
    
    results.emit( p );
    return n;
}


size_t ygen_emit::opgen( ygen_program* p, size_t index )
{
    yssa_function* function = p->ssafunc;

    yssa_opinst* op = function->ops.at( index );
    switch ( op->opcode )
    {
    case YL_NOP:
    {
        p->ops.emplace_back( YL_NOP, 0, 0, 0 );
        return 1;
    }
    
    case YL_MOV:
    {
        assert( op->result_count == 1 );
        assert( op->operand_count == 1 );
        if ( op->r != yl_opinst::NOVAL && op->r != op->operand[ 0 ]->r )
        {
            unsigned a = op->operand[ 0 ]->r;
            assert( a != yl_opinst::NOVAL );
            p->ops.emplace_back( (yl_opcode)op->opcode, op->r, a, 0 );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_NULL:
    case YL_NEG:
    case YL_BITNOT:
    case YL_MUL:
    case YL_DIV:
    case YL_MOD:
    case YL_INTDIV:
    case YL_ADD:
    case YL_SUB:
    case YL_LSL:
    case YL_LSR:
    case YL_ASR:
    case YL_BITAND:
    case YL_BITXOR:
    case YL_BITOR:
    case YL_CONCAT:
    case YL_EQ:
    case YL_NE:
    case YL_LT:
    case YL_GT:
    case YL_LE:
    case YL_GE:
    case YL_LNOT:
    case YL_LXOR:
    case YL_SUPER:
    case YL_INKEY:
    case YL_INDEX:
    case YL_RESPONDS:
    case YL_IS:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned a = operand( op, 0 );
            unsigned b = operand( op, 1 );
            p->ops.emplace_back( (yl_opcode)op->opcode, op->r, a, b );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_BOOL:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned a = op->boolean ? 1 : 0;
            p->ops.emplace_back( YL_BOOL, op->r, a, 0 );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_NUMBER:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned c = (unsigned)p->numvals.at( op->number );
            p->ops.emplace_back( YL_NUMBER, op->r, c );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_STRING:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            symkey k( op->string->string, op->string->length );
            unsigned c = (unsigned)p->strvals.at( k );
            p->ops.emplace_back( YL_STRING, op->r, c );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
     
    case YL_GLOBAL:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned b = (unsigned)p->strvals.at( op->key );
            p->ops.emplace_back( YL_GLOBAL, op->r, 0, b );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_SETGLOBAL:
    {
        assert( op->result_count == 0 );
        unsigned r = operand( op, 0 );
        unsigned b = (unsigned)p->strvals.at( op->key );
        p->ops.emplace_back( YL_SETGLOBAL, r, 0, b );
        return 1;
    }

    case YL_FUNCTION:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned c = (unsigned)p->funvals.at( op->function );
            p->ops.emplace_back( YL_FUNCTION, op->r, c );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
    
        size_t n;
        for ( n = 1; index + n < function->ops.size(); ++n )
        {
            yssa_opinst* up = function->ops.at( index + n );
            if ( up->opcode == YL_UPLOCAL )
            {
                if ( op->r != yl_opinst::NOVAL )
                {
                    unsigned b = operand( up, 0 );
                    p->ops.emplace_back( YL_UPLOCAL, up->r, up->a, b );
                    p->localupcount =
                            std::max( p->localupcount, (size_t)up->a + 1 );
                }
            }
            else if ( up->opcode == YL_UPUPVAL )
            {
                if ( op->r != yl_opinst::NOVAL )
                {
                    p->ops.emplace_back( YL_UPUPVAL, up->r, up->a, 0 );
                }
            }
            else
            {
                break;
            }
        }
        return n;
    }
    
    case YL_VARARG:
    case YL_CALL:
    case YL_YCALL:
    case YL_YIELD:
    case YL_RETURN:
    {
        // Get all operands into the correct registers.
        arguments( p, index, 0 );
        
        // Emit call instruction.
        assert( op->stacktop != yl_opinst::NOVAL );
        unsigned a = op->multival ? yl_opinst::MARK : op->operand_count;
        unsigned b = op->result_count;
        p->ops.emplace_back( (yl_opcode)op->opcode, op->stacktop, a, b );
        stack( p, op->stacktop, op->result_count );
        
        // Move all results into correct registers.
        return select( p, index );
    }
     
    case YL_ITER:
    case YL_ITERKEY:
    {
        unsigned a = operand( op, 0 );
        p->ops.emplace_back( (yl_opcode)op->opcode, op->r, a, 0 );
        p->itercount = std::max( p->itercount, (size_t)op->r + 1 );
        return 1;
    }
    
    case YL_NEXT1:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            p->ops.emplace_back( YL_NEXT1, op->r, op->a );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_NEXT2:
    {
        // Find selects.
        unsigned r = yl_opinst::NOVAL;
        unsigned b = yl_opinst::NOVAL;
        
        size_t n;
        for ( n = 1; index + n < function->ops.size(); ++n )
        {
            yssa_opinst* sel = function->ops.at( index + n );
            if ( sel->opcode == YSSA_SELECT )
            {
                assert( sel->operand_count == 1 );
                assert( sel->operand[ 0 ] == op );
                
                if ( sel->select == 0 )
                {
                    r = sel->r;
                }
                else if ( sel->select == 1 )
                {
                    b = sel->r;
                }
                else
                {
                    assert( ! "invalid select index" );
                }
            }
            else
            {
                break;
            }
        }
        
        assert( r != yl_opinst::NOVAL );
        assert( b != yl_opinst::NOVAL );
        p->ops.emplace_back( YL_NEXT2, r, op->a, b );
        p->stackcount = std::max( p->stackcount, (size_t)r + 1 );
        p->stackcount = std::max( p->stackcount, (size_t)b + 1 );
        return n;
    }
    
    case YL_NEXT:
    {
        assert( op->stacktop != yl_opinst::NOVAL );
        p->ops.emplace_back( YL_NEXT, op->stacktop, op->a, op->result_count );
        stack( p, op->stacktop, op->result_count );
        return select( p, index );
    }
 
    case YL_GETUP:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            p->ops.emplace_back( YL_GETUP, op->r, op->a, 0 );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_SETUP:
    {
        p->ops.emplace_back( YL_SETUP, operand( op, 0 ), op->a, 0 );
        return 1;
    }
    
    case YL_OBJECT:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned a = op->operand_count ? operand( op, 0 ) : yl_opinst::NOVAL;
            p->ops.emplace_back( (yl_opcode)op->opcode, op->r, a, 0 );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
 
    case YL_CLOSE:
    {
        p->ops.emplace_back( YL_CLOSE, 0, op->a, op->b );
        return 1;
    }
 
    case YL_ARRAY:
    case YL_TABLE:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            p->ops.emplace_back( (yl_opcode)op->opcode, op->r, op->c );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
     
    case YL_KEY:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned b = (unsigned)p->strvals.at( op->key );
            p->ops.emplace_back( YL_KEY, op->r, operand( op, 0 ), b );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_SETKEY:
    {
        unsigned r = operand( op, 0 );
        unsigned a = operand( op, 1 );
        unsigned b = (unsigned)p->strvals.at( op->key );
        p->ops.emplace_back( YL_SETKEY, r, a, b );
        return 1;
    }
    
    case YL_SETINKEY:
    case YL_SETINDEX:
    {
        assert( op->operand_count == 3 );
        unsigned r = operand( op, 0 );
        unsigned a = operand( op, 1 );
        unsigned b = operand( op, 2 );
        p->ops.emplace_back( (yl_opcode)op->opcode, r, a, b );
        return 1;
    }
    
    case YL_DELKEY:
    {
        unsigned a = operand( op, 0 );
        unsigned b = (unsigned)p->strvals.at( op->key );
        p->ops.emplace_back( YL_DELKEY, 0, a, b );
        return 1;
    }

    case YL_DELINKEY:
    {
        unsigned a = operand( op, 0 );
        unsigned b = operand( op, 1 );
        p->ops.emplace_back( YL_DELINKEY, 0, a, b );
        return 1;
    }
     
    case YL_APPEND:
    {
        unsigned a = operand( op, 0 );
        unsigned r = operand( op, 1 );
        p->ops.emplace_back( YL_APPEND, r, a, 0 );
        return 1;
    }
    
    case YL_EXTEND:
    {
        arguments( p, index, 1 );
        assert( op->stacktop != yl_opinst::NOVAL );
        unsigned a = op->multival ? yl_opinst::MARK : op->operand_count;
        unsigned b = operand( op, 0 );
        p->ops.emplace_back( YL_EXTEND, op->stacktop, a, b );
        return 1;
    }
    
    case YL_UNPACK:
    {
        assert( op->stacktop != yl_opinst::NOVAL );
        unsigned a = operand( op, 0 );
        p->ops.emplace_back( YL_UNPACK, op->stacktop, a, op->result_count );
        stack( p, op->stacktop, op->result_count );
        return select( p, index );
    }
     
    case YL_THROW:
    {
        p->ops.emplace_back( YL_THROW, operand( op, 0 ), 0, 0 );
        return 1;
    }
    
    case YL_EXCEPT:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            p->ops.emplace_back( YL_EXCEPT, op->r, 0, 0 );
            p->stackcount = std::max( p->stackcount, (size_t)op->r + 1 );
        }
        return 1;
    }
    
    case YL_UNWIND:
    {
        p->ops.emplace_back( YL_UNWIND, 0, 0, 0 );
        return 1;
    }
 
    case YSSA_PARAM:
    {
        // Get all parameters into the correct registers.
        ygen_movgraph params;
        
        size_t n;
        for ( n = 0; index + n < function->ops.size(); ++n )
        {
            yssa_opinst* param = function->ops.at( index + n );
            if ( param->opcode == YSSA_PARAM )
            {
                unsigned a = param->select + 1;
                p->stackcount = std::max( p->stackcount, (size_t)a + 1 );
                if ( param->r != yl_opinst::NOVAL )
                {
                    params.move( param->r, a );
                }
            }
            else
            {
                break;
            }
        }
        
        params.emit( p );
        return n;
    }

    case YSSA_IMPLICIT:
    case YSSA_ITEREACH:
    case YSSA_PHI:
    case YSSA_VAR:
    {
        // Ignore these ops.
        return 1;
    }

    case YL_SWP:
    case YL_JMP:
    case YL_JMPT:
    case YL_JMPF:
    case YL_JMPV:
    case YL_JMPN:
    case YL_UPLOCAL:
    case YL_UPUPVAL:
    case YSSA_SELECT:
    case YSSA_REF:
    default:
    {
        assert( ! "unexpected SSA op" );
        return 1;
    }

    
    }
}


void ygen_emit::make_string( ygen_string* s )
{
    if ( s->string )
        return;
    
    yl_rootref< yl_string > string = yl_string::alloc( s->text, s->size );
    if ( s->iskey )
    {
        s->string = string->symbol();
    }
    else
    {
        s->string = string;
    }
}


void ygen_emit::make_program( ygen_program* p )
{
    if ( p->program )
        return;
    
    p->program = yl_program::alloc
    (
        p->values.size(),
        p->ops.size(),
        p->xframes.size(),
        p->debugvars.size(),
        p->debugspans.size()
    );
    
    p->program->_upcount    = p->ssafunc->upcount;
    p->program->_paramcount = p->ssafunc->paramcount;
    p->program->_stackcount = p->stackcount;
    p->program->_locupcount = p->localupcount;
    p->program->_iterscount = p->itercount;
    p->program->_varargs    = p->ssafunc->varargs;
    p->program->_coroutine  = p->ssafunc->coroutine;
    
}


void ygen_emit::emit( ygen_program* p )
{
    p->program->_name = p->ssafunc->funcname;

    yl_valref* values = p->program->_values();
    for ( size_t i = 0; i < p->values.size(); ++i )
    {
        const ygen_value& value = p->values.at( i );
        switch ( value.kind )
        {
        case YGEN_KEY:
        case YGEN_STRING:
            values[ i ].set( value.string->string.get() );
            break;
        case YGEN_NUMBER:
            values[ i ].set( value.number );
            break;
        case YGEN_PROGRAM:
            values[ i ].set( value.program->program.get() );
            break;
        }
    }

    yl_opinst* ops = p->program->_ops();
    for ( size_t i = 0; i < p->ops.size(); ++i )
    {
        ops[ i ] = p->ops.at( i );
    }
    
    yl_xframe* xframes = p->program->_xframes();
    for ( size_t i = 0; i < p->xframes.size(); ++i )
    {
        xframes[ i ] = p->xframes.at( i );
    }
    
    yl_debugvar* debugvars = p->program->_debugvars();
    for ( size_t i = 0; i < p->ssafunc->upnames.size(); ++i )
    {
        debugvars[ i ].name = p->ssafunc->upnames.at( i );
        debugvars[ i ].r = (unsigned)i;
    }
    for ( size_t i = p->ssafunc->upnames.size(); i < p->debugvars.size(); ++i )
    {
        yssa_variable* variable = p->debugvars.at( i );
        debugvars[ i ].name = variable->name;
        debugvars[ i ].r = variable->r;
    }
    
    yl_debugspan* debugspans = p->program->_debugspans();
    for ( size_t i = 0; i < p->debugspans.size(); ++i )
    {
        debugspans[ i ] = p->debugspans.at( i );
    }
    
    
//    p->program->print();
//    printf( "\n" );
    
}










